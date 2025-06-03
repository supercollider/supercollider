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
#include "SC_CoreAudio.h"
#include <stdarg.h>
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "SC_Time.hpp"
#include <math.h>
#include <stdlib.h>
#include "SC_PaUtils.hpp"

#include "portaudio.h"
#define SC_PA_USE_DLL

int32 server_timeseed() { return timeSeed(); }

#ifdef SC_PA_USE_DLL
#    include "SC_TimeDLL.hpp"
// =====================================================================
// Timing

static inline int64 sc_PAOSCTime() { return OSCTime(getTime()); }

static inline double sc_PAOSCTimeSeconds() { return (uint64)sc_PAOSCTime() * kOSCtoSecs; }

int64 oscTimeNow() { return sc_PAOSCTime(); }

void initializeScheduler() {}

#else // SC_PA_USE_DLL

int64 gOSCoffset = 0;

static inline int64 GetCurrentOSCTime() { return OSCTime(getTime()); }

int64 oscTimeNow() { return GetCurrentOSCTime(); }

int64 PaStreamTimeToOSC(PaTime pa_time) {
    uint64 s, f;
    s = (uint64)pa_time;
    f = (uint64)((pa_time - s) * 1000000 * kMicrosToOSCunits);

    return (s << 32) + f;
}

void initializeScheduler() { gOSCoffset = GetCurrentOSCTime(); }

#endif // SC_PA_USE_DLL

class SC_PortAudioDriver : public SC_AudioDriver {
    int mInputChannelCount, mOutputChannelCount;
    PaStream* mStream;
    PaTime mPaStreamStartupTime;
    int64 mPaStreamStartupTimeOSC;
#ifdef SC_PA_USE_DLL
    double mMaxOutputLatency;
    SC_TimeDLL mDLL;
#endif
protected:
    // Driver interface methods
    virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
    virtual bool DriverStart();
    virtual bool DriverStop();

public:
    SC_PortAudioDriver(struct World* inWorld);
    virtual ~SC_PortAudioDriver();

    int PortAudioCallback(const void* input, void* output, unsigned long frameCount,
                          const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags);
};

SC_AudioDriver* SC_NewAudioDriver(struct World* inWorld) { return new SC_PortAudioDriver(inWorld); }

#define PRINT_PORTAUDIO_ERROR(function, errorcode)                                                                     \
    scprintf("SC_PortAudioDriver: PortAudio failed at %s with error: '%s'\n", #function, Pa_GetErrorText(errorcode))

SC_PortAudioDriver::SC_PortAudioDriver(struct World* inWorld):
    SC_AudioDriver(inWorld),
    mStream(0)
#ifdef SC_PA_USE_DLL
    ,
    mMaxOutputLatency(0.)
#endif
{
    PaError paerror = Pa_Initialize();
    if (paerror != paNoError)
        PRINT_PORTAUDIO_ERROR(Pa_Initialize, paerror);
}

SC_PortAudioDriver::~SC_PortAudioDriver() {
    if (mStream) {
        PaError paerror = Pa_CloseStream(mStream);
        if (paerror != paNoError)
            PRINT_PORTAUDIO_ERROR(Pa_CloseStream, paerror);
    }
    Pa_Terminate();
}

static int SC_PortAudioStreamCallback(const void* input, void* output, unsigned long frameCount,
                                      const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags,
                                      void* userData) {
    SC_PortAudioDriver* driver = (SC_PortAudioDriver*)userData;

    return driver->PortAudioCallback(input, output, frameCount, timeInfo, statusFlags);
}
void sc_SetDenormalFlags();
int SC_PortAudioDriver::PortAudioCallback(const void* input, void* output, unsigned long frameCount,
                                          const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags) {
    sc_SetDenormalFlags();
    World* world = mWorld;
    (void)frameCount, timeInfo, statusFlags; // suppress unused parameter warnings
#ifdef SC_PA_USE_DLL
    mDLL.Update(sc_PAOSCTimeSeconds());

#    if SC_PA_DEBUG_DLL
    static int tick = 0;
    if (++tick >= 10) {
        tick = 0;
        scprintf("DLL: t %.6f p %.9f sr %.6f e %.9f avg(e) %.9f inc %.9f\n", mDLL.PeriodTime(), mDLL.Period(),
                 mDLL.SampleRate(), mDLL.Error(), mDLL.AvgError(), mOSCincrement * kOSCtoSecs);

        // scprintf("mOSCbuftime1 %llu \t %llu \t %f \n",GetCurrentOSCTime(),(uint64)((mDLL.PeriodTime() -
        // mMaxOutputLatency) * kSecondsToOSCunits + .5),((mDLL.PeriodTime() - mMaxOutputLatency) * kSecondsToOSCunits +
        // .5));
    }
#    endif
#endif
    try {
#if !defined(SC_PA_USE_DLL)
        // synchronise against the output buffer - timeInfo->currentTime is 0.0 bug in PA?
        if (mPaStreamStartupTime == 0 && mPaStreamStartupTimeOSC == 0) {
            mPaStreamStartupTimeOSC = GetCurrentOSCTime();
            mPaStreamStartupTime = timeInfo->outputBufferDacTime;
        }
        mOSCbuftime = PaStreamTimeToOSC(timeInfo->outputBufferDacTime - mPaStreamStartupTime) + mPaStreamStartupTimeOSC;
#endif

        mFromEngine.Free();
        mToEngine.Perform();
        mOscPacketsToEngine.Perform();

        const float** inBuffers = (const float**)input;
        float** outBuffers = (float**)output;

        int numSamples = NumSamplesPerCallback();
        int bufFrames = mWorld->mBufLength;
        int numBufs = numSamples / bufFrames;

        float* inBuses = mWorld->mAudioBus + mWorld->mNumOutputs * bufFrames;
        float* outBuses = mWorld->mAudioBus;
        int32* inTouched = mWorld->mAudioBusTouched + mWorld->mNumOutputs;
        int32* outTouched = mWorld->mAudioBusTouched;

        int bufFramePos = 0;
#ifdef SC_PA_USE_DLL
        int64 oscTime = mOSCbuftime = (uint64)((mDLL.PeriodTime() + mMaxOutputLatency) * kSecondsToOSCunits + .5);
        // 		int64 oscInc = mOSCincrement = (int64)(mOSCincrementNumerator / mDLL.SampleRate());
        int64 oscInc = mOSCincrement = (uint64)((mDLL.Period() / numBufs) * kSecondsToOSCunits + .5);
        mSmoothSampleRate = mDLL.SampleRate();
        double oscToSamples = mOSCtoSamples = mSmoothSampleRate * kOSCtoSecs /* 1/pow(2,32) */;
#else
        int64 oscTime = mOSCbuftime;
        int64 oscInc = mOSCincrement;
        double oscToSamples = mOSCtoSamples;
#endif
        // main loop
        for (int i = 0; i < numBufs; ++i, mWorld->mBufCounter++, bufFramePos += bufFrames) {
            int32 bufCounter = mWorld->mBufCounter;
            int32* tch;

            // copy+touch inputs
            tch = inTouched;
            for (int k = 0; k < mInputChannelCount; ++k) {
                const float* src = inBuffers[k] + bufFramePos;
                float* dst = inBuses + k * bufFrames;
                memcpy(dst, src, bufFrames * sizeof(float));
                *tch++ = bufCounter;
            }

            // run engine
            int64 schedTime;
            int64 nextTime = oscTime + oscInc;
            // DEBUG
            /*
            if (mScheduler.Ready(nextTime)) {
                double diff = (mScheduler.NextTime() - mOSCbuftime)*kOSCtoSecs;
                scprintf("rdy %.6f %.6f %.6f %.6f \n", (mScheduler.NextTime()-gStartupOSCTime) * kOSCtoSecs,
            (mOSCbuftime-gStartupOSCTime)*kOSCtoSecs, diff, (nextTime-gStartupOSCTime)*kOSCtoSecs);
            }
            */
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

            // copy touched outputs
            tch = outTouched;
            for (int k = 0; k < mOutputChannelCount; ++k) {
                float* dst = outBuffers[k] + bufFramePos;
                if (*tch++ == bufCounter) {
                    const float* src = outBuses + k * bufFrames;
                    memcpy(dst, src, bufFrames * sizeof(float));
                } else {
                    memset(dst, 0, bufFrames * sizeof(float));
                }
            }

            // update buffer time
            oscTime = mOSCbuftime = nextTime;
        }
    } catch (std::exception& exc) {
        scprintf("SC_PortAudioDriver: exception in real time: %s\n", exc.what());
    } catch (...) { scprintf("SC_PortAudioDriver: unknown exception in real time\n"); }

    double cpuUsage = Pa_GetStreamCpuLoad(mStream) * 100.0;
    mAvgCPU = mAvgCPU + 0.1 * (cpuUsage - mAvgCPU);
    if (cpuUsage > mPeakCPU || --mPeakCounter <= 0) {
        mPeakCPU = cpuUsage;
        mPeakCounter = mMaxPeakCounter;
    }

    mAudioSync.Signal();

    return paContinue;
}

// ====================================================================
//
//
bool SC_PortAudioDriver::DriverSetup(int* outNumSamples, double* outSampleRate) {
    int mDeviceInOut[2];
    PaError paerror;
    const PaDeviceInfo* pdi;
    const PaStreamInfo* psi;
    PaTime suggestedLatencyIn, suggestedLatencyOut;
    PaDeviceIndex numDevices = Pa_GetDeviceCount();

    // print out all options:
    fprintf(stdout, "\nDevice options:\n");
    for (int i = 0; i < numDevices; i++) {
        pdi = Pa_GetDeviceInfo(i);
        fprintf(stdout, "- %s\n  (%d ins, %d outs)\n", GetPaDeviceName(i).c_str(), pdi->maxInputChannels,
                pdi->maxOutputChannels);
    }

    auto* inDeviceName = mWorld->hw->mInDeviceName;
    auto* outDeviceName = mWorld->hw->mOutDeviceName;
    mDeviceInOut[0] = GetPaDeviceFromName(inDeviceName, true);
    mDeviceInOut[1] = GetPaDeviceFromName(outDeviceName, false);

    // report requested devices
    fprintf(stdout, "\nRequested devices:\n");
    if (mWorld->mNumInputs) {
        auto nameIsEmpty = (inDeviceName && !inDeviceName[0]) || (inDeviceName == nullptr);
        fprintf(stdout, "  In%s:\n  - %s\n",
                nameIsEmpty
                    ? ""
                    : (mDeviceInOut[0] == paNoDevice ? " (matching device NOT found)" : " (matching device found)"),
                (nameIsEmpty ? "(default)" : inDeviceName));
    }
    if (mWorld->mNumOutputs) {
        auto nameIsEmpty = (outDeviceName && !outDeviceName[0]) || (outDeviceName == nullptr);
        fprintf(stdout, "  Out%s:\n  - %s\n",
                nameIsEmpty
                    ? ""
                    : (mDeviceInOut[1] == paNoDevice ? " (matching device NOT found)" : " (matching device found)"),
                (nameIsEmpty ? "(default)" : outDeviceName));
    }

    fprintf(stdout, "\n");
    paerror = TryGetDefaultPaDevices(&mDeviceInOut[0], &mDeviceInOut[1], mWorld->mNumInputs, mWorld->mNumOutputs,
                                     mPreferredSampleRate);

    // if we got an error from TryGetDefaultPaDevices, stop here
    if (paerror != paNoError) {
        PRINT_PORTAUDIO_ERROR(Pa_OpenStream, paerror);
        return false;
    }

    *outNumSamples = mWorld->mBufLength;
    if (mPreferredSampleRate)
        *outSampleRate = mPreferredSampleRate;
    else if (mDeviceInOut[1] != paNoDevice)
        *outSampleRate = Pa_GetDeviceInfo(mDeviceInOut[1])->defaultSampleRate;
    else if (mDeviceInOut[0] != paNoDevice)
        *outSampleRate = Pa_GetDeviceInfo(mDeviceInOut[0])->defaultSampleRate;
    else
        *outSampleRate = 44100.;


    if (mDeviceInOut[0] != paNoDevice || mDeviceInOut[1] != paNoDevice) {
        if (mPreferredHardwareBufferFrameSize)
            // controls the suggested latency by hardwareBufferSize switch -Z
            suggestedLatencyIn = suggestedLatencyOut = mPreferredHardwareBufferFrameSize / (*outSampleRate);
        else {
            if (mDeviceInOut[0] != paNoDevice)
                suggestedLatencyIn = Pa_GetDeviceInfo(mDeviceInOut[0])->defaultLowInputLatency;
            if (mDeviceInOut[1] != paNoDevice)
                suggestedLatencyOut = Pa_GetDeviceInfo(mDeviceInOut[1])->defaultLowOutputLatency;
        }

        fprintf(stdout, "\nBooting with:\n");

        if (mDeviceInOut[0] != paNoDevice) {
            // avoid to allocate the 128 virtual channels reported by the portaudio library for ALSA "default"
            mInputChannelCount =
                std::min<size_t>(mWorld->mNumInputs, Pa_GetDeviceInfo(mDeviceInOut[0])->maxInputChannels);
            fprintf(stdout, "  In: %s\n", GetPaDeviceName(mDeviceInOut[0]).c_str());
        } else {
            mInputChannelCount = 0;
        }

        if (mDeviceInOut[1] != paNoDevice) {
            // avoid to allocate the 128 virtual channels reported by the portaudio library for ALSA "default"
            mOutputChannelCount =
                std::min<size_t>(mWorld->mNumOutputs, Pa_GetDeviceInfo(mDeviceInOut[1])->maxOutputChannels);
            fprintf(stdout, "  Out: %s\n", GetPaDeviceName(mDeviceInOut[1]).c_str());
        } else {
            mOutputChannelCount = 0;
        }

        PaStreamParameters* inStreamParams_p;
        PaStreamParameters inStreamParams;
        if (mDeviceInOut[0] != paNoDevice) {
            inStreamParams = MakePaStreamParameters(mDeviceInOut[0], mInputChannelCount, suggestedLatencyIn);
            inStreamParams_p = &inStreamParams;
        } else {
            inStreamParams_p = NULL;
        }

        PaStreamParameters* outStreamParams_p;
        PaStreamParameters outStreamParams;
        if (mDeviceInOut[1] != paNoDevice) {
            outStreamParams = MakePaStreamParameters(mDeviceInOut[1], mOutputChannelCount, suggestedLatencyOut);
            outStreamParams_p = &outStreamParams;
        } else {
            outStreamParams_p = NULL;
        }

        // check if format is supported
        // this sometimes gives a more accurate error information than Pa_OpenStream's error
        paerror = Pa_IsFormatSupported(inStreamParams_p, outStreamParams_p, *outSampleRate);
        if (paerror != paNoError) {
            PRINT_PORTAUDIO_ERROR(Pa_OpenStream, paerror);
            return false;
        }

        paerror = Pa_OpenStream(&mStream, inStreamParams_p, outStreamParams_p, *outSampleRate, *outNumSamples, paNoFlag,
                                SC_PortAudioStreamCallback, this);

        if (paerror != paNoError)
            PRINT_PORTAUDIO_ERROR(Pa_OpenStream, paerror);
        else {
            psi = Pa_GetStreamInfo(mStream);
            if (!psi)
                fprintf(stdout, "  Could not obtain further info from portaudio stream\n");
            else {
                fprintf(stdout, "  Sample rate: %.3f\n", psi->sampleRate);
                fprintf(stdout, "  Latency (in/out): %.3f / %.3f sec\n", psi->inputLatency, psi->outputLatency);
#ifdef SC_PA_USE_DLL
                mMaxOutputLatency = psi->outputLatency;
#endif
            }
        }
        return paerror == paNoError;
    }

    // should not be necessary, but a last try with OpenDefaultStream...
    paerror = Pa_OpenDefaultStream(&mStream, mWorld->mNumInputs, mWorld->mNumOutputs, paFloat32 | paNonInterleaved,
                                   *outSampleRate, *outNumSamples, SC_PortAudioStreamCallback, this);
    mInputChannelCount = mWorld->mNumInputs;
    mOutputChannelCount = mWorld->mNumOutputs;
    if (paerror != paNoError)
        PRINT_PORTAUDIO_ERROR(Pa_OpenDefaultStream, paerror);
    return paerror == paNoError;
}

bool SC_PortAudioDriver::DriverStart() {
    if (!mStream)
        return false;

    PaError paerror = Pa_StartStream(mStream);
    if (paerror != paNoError)
        PRINT_PORTAUDIO_ERROR(Pa_StartStream, paerror);

    // sync times
    mPaStreamStartupTimeOSC = 0;
    mPaStreamStartupTime = 0;
    // it would be better to do the sync here, but the timeInfo in the callback is incomplete
    // mPaStreamStartupTimeOSC = GetCurrentOSCTime();
    // mPaStreamStartupTime = Pa_GetStreamTime(mStream);
#ifdef SC_PA_USE_DLL
    mDLL.Reset(mSampleRate, mNumSamplesPerCallback, SC_TIME_DLL_BW, sc_PAOSCTimeSeconds());
#endif
    return paerror == paNoError;
}

bool SC_PortAudioDriver::DriverStop() {
    if (!mStream)
        return false;

    PaError paerror = Pa_StopStream(mStream);
    if (paerror != paNoError)
        PRINT_PORTAUDIO_ERROR(Pa_StopStream, paerror);

    return paerror == paNoError;
}
