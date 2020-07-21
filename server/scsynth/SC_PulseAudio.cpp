/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/
#include <stdarg.h>
#include <math.h>
#include <stdlib.h>
#include <rtaudio/RtAudio.h>

#include "SC_CoreAudio.h"
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "SC_Time.hpp"


int32 server_timeseed() { return timeSeed(); }

#include "SC_TimeDLL.hpp"
// =====================================================================
// Timing

static inline int64 sc_PAOSCTime() { return OSCTime(getTime()); }

static inline double sc_PAOSCTimeSeconds() { return (uint64)sc_PAOSCTime() * kOSCtoSecs; }

int64 oscTimeNow() { return sc_PAOSCTime(); }

void initializeScheduler() {}


class SC_PulseAudioDriver : public SC_AudioDriver {
    int m_inputChannelCount, m_outputChannelCount;
    int64 m_paStreamStartupTime;
    int64 m_paStreamStartupTimeOSC;
    RtAudio* m_audio;
    double m_maxOutputLatency;
    SC_TimeDLL m_DLL;

    int rtCallback(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double streamTime,
                   RtAudioStreamStatus status);
    static int rtCallbackStatic(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double streamTime,
                                RtAudioStreamStatus status, void* userData);

protected:
    // Driver interface methods
    virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate) override;
    virtual bool DriverStart() override;
    virtual bool DriverStop() override;

public:
    SC_PulseAudioDriver(struct World* inWorld);
    virtual ~SC_PulseAudioDriver();
};

// This is the "entry point" for our code. This will be called by the server to initialize the audio backend
SC_AudioDriver* SC_NewAudioDriver(struct World* inWorld) { return new SC_PulseAudioDriver(inWorld); }

SC_PulseAudioDriver::SC_PulseAudioDriver(struct World* inWorld): SC_AudioDriver(inWorld), m_maxOutputLatency(0.) {
    scprintf("In SC_PulseAudioDriver::SC_PulseAudioDriver\n");
    // We ask RtAudio to use the PulseAudio backend
    m_audio = new RtAudio(RtAudio::LINUX_PULSE);
}

SC_PulseAudioDriver::~SC_PulseAudioDriver() {
    scprintf("In SC_PulseAudioDriver::~SC_PulseAudioDriver\n");
    m_audio->closeStream();
    delete m_audio;
}

// This is the callback static entry point. It will just relay the call to the object's proper method,
// using the userData pointer
int SC_PulseAudioDriver::rtCallbackStatic(void* outputBuffer, void* inputBuffer, unsigned int nFrames,
                                          double streamTime, RtAudioStreamStatus status, void* userData) {
    SC_PulseAudioDriver* driver = (SC_PulseAudioDriver*)userData;

    return driver->rtCallback(outputBuffer, inputBuffer, nFrames, streamTime, status);
}

void sc_SetDenormalFlags();

// This is where all the data movement between SuperCollider and the sound server happens.
// Since PulseAudio uses interleaved samples, that's what we use here
int SC_PulseAudioDriver::rtCallback(void* outputBuffer, void* inputBuffer, unsigned int nFrames, double streamTime,
                                    RtAudioStreamStatus status) {
    auto systemTimeBefore = getTime();                                        
    sc_SetDenormalFlags();
    World* world = mWorld;

    m_DLL.Update(sc_PAOSCTimeSeconds());

    // This is where all the data movement takes place
    try {
        mFromEngine.Free();
        mToEngine.Perform();
        mOscPacketsToEngine.Perform();

        int numSamples = NumSamplesPerCallback();
        int bufFrames = mWorld->mBufLength;
        int numBufs = numSamples / bufFrames;

        float* inBuses = mWorld->mAudioBus + mWorld->mNumOutputs * bufFrames;
        float* outBuses = mWorld->mAudioBus;
        int32* inTouched = mWorld->mAudioBusTouched + mWorld->mNumOutputs;
        int32* outTouched = mWorld->mAudioBusTouched;

        int bufFramePos = 0;
        int64 oscTime = mOSCbuftime = (uint64)((m_DLL.PeriodTime() + m_maxOutputLatency) * kSecondsToOSCunits + .5);
        int64 oscInc = mOSCincrement = (uint64)((m_DLL.Period() / numBufs) * kSecondsToOSCunits + .5);
        mSmoothSampleRate = m_DLL.SampleRate();
        double oscToSamples = mOSCtoSamples = mSmoothSampleRate * kOSCtoSecs /* 1/pow(2,32) */;

        // main loop
        for (int i = 0; i < numBufs; ++i, mWorld->mBufCounter++, bufFramePos += bufFrames) {
            int32 bufCounter = mWorld->mBufCounter;
            int32* tch;

            // Process inputs, marking them as touched
            tch = inTouched;
            for (int k = 0; k < m_inputChannelCount; ++k) {
                *tch++ = bufCounter;
                float* dst = inBuses + k * bufFrames;
                const float* src = (float*)inputBuffer + k + (bufFramePos * m_inputChannelCount);
                for (int n = 0; n < bufFrames; n++) {
                    *dst = *src;
                    src += m_inputChannelCount;
                    dst++;
                }
            }

            // run engine
            int64 schedTime;
            int64 nextTime = oscTime + oscInc;
            while ((schedTime = mScheduler.NextTime()) <= nextTime) {
                float diffTime = (float)(schedTime - oscTime) * oscToSamples + 0.5;
                float diffTimeFloor = floor(diffTime);
                world->mSampleOffset = (int)diffTimeFloor;
                world->mSubsampleOffset = diffTime - diffTimeFloor;

                if (world->mSampleOffset < 0)
                    world->mSampleOffset = 0;
                else if (world->mSampleOffset >= world->mBufLength)
                    world->mSampleOffset = world->mBufLength - 1;

                SC_ScheduledEvent event = mScheduler.Remove();
                event.Perform();
            }
            world->mSampleOffset = 0;
            world->mSubsampleOffset = 0.0f;

            World_Run(world);

            // Process outputs (considering which ones have been touched)
            tch = outTouched;
            for (int k = 0; k < m_outputChannelCount; ++k) {
                float* dst = (float*)outputBuffer + k + (bufFramePos * m_outputChannelCount);
                if (tch[k] == bufCounter) {
                    const float* src = outBuses + k * bufFrames;
                    for (int n = 0; n < bufFrames; n++) {
                        *dst = *src;
                        src++;
                        dst += m_outputChannelCount;
                    }
                } else {
                    for (int n = 0; n < bufFrames; n++) {
                        *dst = 0;
                        dst += m_outputChannelCount;
                    }
                }
            }
            // update buffer time
            oscTime = mOSCbuftime = nextTime;
        }
    } catch (std::exception& exc) {
        scprintf("SC_PulseAudioDriver: exception in real time: %s\n", exc.what());
    } catch (...) {
        scprintf("SC_PulseAudioDriver: unknown exception in real time\n");
    }

    // calculate CPU load
    auto systemTimeAfter = getTime();
    auto duration = systemTimeAfter - systemTimeBefore;
    //double calcTime = (double)(systemTimeAfter - systemTimeBefore) * 1e-6;
    double calcTime = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() * 1e-9;
    double cpuUsage = calcTime * mBuffersPerSecond * 100.;
    mAvgCPU = mAvgCPU + 0.1 * (cpuUsage - mAvgCPU);
    if (cpuUsage > mPeakCPU || --mPeakCounter <= 0) {
        mPeakCPU = cpuUsage;
        mPeakCounter = mMaxPeakCounter;
    }

    mAudioSync.Signal();
    return 0;
}


// ========================================================================
// The SC_AudioDriver interface methods that we need to implement are below

bool SC_PulseAudioDriver::DriverSetup(int* outNumSamples, double* outSampleRate) {
    int rc;
    int device;
    scprintf("In SC_PulseAudioDriver::DriverSetup\n");

    // Show the devices
    int numDevices = m_audio->getDeviceCount();
    if (numDevices == 0) {
        scprintf("No audio devices found\n");
        return false;
    }

    scprintf("Found %d device(s):\n", numDevices);
    std::vector<RtAudio::DeviceInfo> infos;
    for (int i = 0; i < numDevices; i++) {
        try {
            RtAudio::DeviceInfo info;
            info = m_audio->getDeviceInfo(i);
            scprintf("  - %s   (device #%d with %d ins %d outs)\n", info.name.c_str(), i, info.inputChannels,
                     info.outputChannels);

            infos.push_back(info);
        } catch (RtAudioError& e) {
            e.printMessage();
            break;
        }
    }

    // Use the default devices
    unsigned int outputDevice = m_audio->getDefaultOutputDevice();
    unsigned int inputDevice = m_audio->getDefaultInputDevice();

    // Number of input and output channels to use
    m_outputChannelCount = std::min<size_t>(mWorld->mNumOutputs, infos[outputDevice].duplexChannels);
    m_inputChannelCount = std::min<size_t>(mWorld->mNumInputs, infos[inputDevice].duplexChannels);

    // What HW buffer size to use?
    *outNumSamples = mPreferredHardwareBufferFrameSize ? mPreferredHardwareBufferFrameSize : 512;
    // What sample rate to use? If we get one, make sure it is supported, and fall back to the default if not
    if (mPreferredSampleRate) {
        std::vector<unsigned int> supportedSampleRates { infos[outputDevice].sampleRates };
        if (std::find(supportedSampleRates.begin(), supportedSampleRates.end(), (unsigned int)mPreferredSampleRate)
            != supportedSampleRates.end()) {
            *outSampleRate = mPreferredSampleRate;
        } else {
            scprintf("Requested sample rate NOT supported. Setting to 44.1 kHz\n");
            *outSampleRate = infos[outputDevice].preferredSampleRate;
        }
    } else {
        *outSampleRate = infos[outputDevice].preferredSampleRate;
    }

    // Configure the stream parameters
    RtAudio::StreamParameters outParameters;
    outParameters.deviceId = outputDevice;
    outParameters.nChannels = m_outputChannelCount;
    RtAudio::StreamParameters inParameters;
    inParameters.deviceId = inputDevice;
    inParameters.nChannels = m_inputChannelCount;
    RtAudio::StreamOptions options;
    options.flags = RTAUDIO_MINIMIZE_LATENCY | RTAUDIO_SCHEDULE_REALTIME;
    options.streamName = "SuperCollider";
    try {
        scprintf("Opening stream with %d output and %d input channels\n", outParameters.nChannels,
                 inParameters.nChannels);
        // Depending on whether we are using inputs, we open the stream with the appropriate parameters
        if (m_inputChannelCount > 0) {
            m_audio->openStream(&outParameters, &inParameters, RTAUDIO_FLOAT32, *outSampleRate, (unsigned int *)outNumSamples,
                                &SC_PulseAudioDriver::rtCallbackStatic, this, &options);
        } else {
            m_audio->openStream(&outParameters, NULL, RTAUDIO_FLOAT32, *outSampleRate, (unsigned int *)outNumSamples,
                                &SC_PulseAudioDriver::rtCallbackStatic, this, &options);
        }

        m_maxOutputLatency = 1.0 * *outNumSamples / *outSampleRate;
        scprintf("Sample rate: %.3f\n", *outSampleRate);
        scprintf("Block size: %d\n", *outNumSamples);
        scprintf("Output latency: %.3f sec\n", m_maxOutputLatency);
    } catch (RtAudioError& e) {
        e.printMessage();
        return false;
    }
    return true;
}

bool SC_PulseAudioDriver::DriverStart() {
    // sync times
    m_paStreamStartupTimeOSC = 0;
    m_paStreamStartupTime = 0;

    // Start the streaming
    try {
        m_audio->startStream();
    } catch (RtAudioError& e) {
        e.printMessage();
        return false;
    }

    m_DLL.Reset(mSampleRate, mNumSamplesPerCallback, SC_TIME_DLL_BW, sc_PAOSCTimeSeconds());
    return true;
}

bool SC_PulseAudioDriver::DriverStop() {
    // We just stop the stream
    m_audio->stopStream();
    return true;
}
