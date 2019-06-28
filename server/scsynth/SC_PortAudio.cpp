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

#include "portaudio.h"
#define SC_PA_USE_DLL

#ifdef __APPLE__
#    include <pa_mac_core.h>
#endif

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

enum class IOType { Input, Output };

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

private:
    void GetPaDeviceFromName(const char* device, int* mInOut, IOType ioType);
    PaError CheckPaDevices(int* inDevice, int* outDevice, int numIns, int numOuts, double sampleRate);
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
        int64 oscTime = mOSCbuftime = (uint64)((mDLL.PeriodTime() - mMaxOutputLatency) * kSecondsToOSCunits + .5);
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
    } catch (...) {
        scprintf("SC_PortAudioDriver: unknown exception in real time\n");
    }

    double cpuUsage = Pa_GetStreamCpuLoad(mStream) * 100.0;
    mAvgCPU = mAvgCPU + 0.1 * (cpuUsage - mAvgCPU);
    if (cpuUsage > mPeakCPU || --mPeakCounter <= 0) {
        mPeakCPU = cpuUsage;
        mPeakCounter = mMaxPeakCounter;
    }

    mAudioSync.Signal();

    return paContinue;
}

void SC_PortAudioDriver::GetPaDeviceFromName(const char* device, int* mInOut, IOType ioType) {
    const PaDeviceInfo* pdi;
    const PaHostApiInfo* apiInfo;
    char devString[256];
    PaDeviceIndex numDevices = Pa_GetDeviceCount();
    *mInOut = paNoDevice;

    for (int i = 0; i < numDevices; i++) {
        pdi = Pa_GetDeviceInfo(i);
#ifndef __APPLE__
        apiInfo = Pa_GetHostApiInfo(pdi->hostApi);
        strcpy(devString, apiInfo->name);
        strcat(devString, " : ");
#endif
        strcat(devString, pdi->name);
        if (strstr(devString, device)) {
            if (ioType == IOType::Input) {
                if (pdi->maxInputChannels > 0)
                    *mInOut = i;
            } else {
                if (pdi->maxOutputChannels > 0)
                    *mInOut = i;
            }
        }
    }
}

// this function will select default PA devices if they are not defined
// it will also try to check for some configuration problems
// numIns, numOuts and sampleRate are only the requested values, can change later
PaError SC_PortAudioDriver::CheckPaDevices(int* inDevice, int* outDevice, int numIns, int numOuts, double sampleRate) {
    // make independent checks whether we are using only input, only output, or both input and outputTouched
    if (numIns && !numOuts) {
        // inputs only
        *outDevice = paNoDevice;
#ifdef _WIN32
        // in case we don't have a proper device, let's try to open one on ASIO
        if (*inDevice == paNoDevice) {
            *inDevice = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(paASIO))->defaultInputDevice;
            if (*inDevice != paNoDevice)
                fprintf(stdout, "Selected default ASIO input device\n");
        }
#endif
        // check for requested sample rate
        if (*inDevice != paNoDevice) {
            if (sampleRate) {
                // check if device can support requested SR
                PaSampleFormat fmt = paFloat32 | paNonInterleaved;
                PaStreamParameters parameters;
                parameters.device = *inDevice;
                parameters.sampleFormat = fmt;
                parameters.hostApiSpecificStreamInfo = NULL;
                parameters.channelCount = Pa_GetDeviceInfo(*outDevice)->maxOutputChannels;
                PaError err = Pa_IsFormatSupported(&parameters, nullptr, sampleRate);
                if (err != paNoError) {
                    fprintf(stdout, "PortAudio error: %s\nRequested sample rate %f for device %s is not supported\n",
                            Pa_GetErrorText(err), sampleRate, Pa_GetDeviceInfo(*outDevice)->name);
                }
            }
        }
        // in case we still don't have a proper device, use the default device
        if (*inDevice == paNoDevice) {
            *inDevice = Pa_GetDefaultInputDevice();
            if (*inDevice != paNoDevice)
                fprintf(stdout, "Selected default system input device\n");
        }
    } else if (!numIns && numOuts) {
        // outputs only
        *inDevice = paNoDevice;
#ifdef _WIN32
        // in case we don't have a proper device, let's try to open one on ASIO
        if (*outDevice == paNoDevice) {
            *outDevice = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(paASIO))->defaultOutputDevice;
            if (*outDevice != paNoDevice)
                fprintf(stdout, "Selected default ASIO output device\n");
        }
#endif
        // check for requested sample rate
        if (*outDevice != paNoDevice) {
            if (sampleRate) {
                // check if device can support requested SR
                PaSampleFormat fmt = paFloat32 | paNonInterleaved;
                PaStreamParameters parameters;
                parameters.device = *outDevice;
                parameters.sampleFormat = fmt;
                parameters.hostApiSpecificStreamInfo = NULL;
                parameters.channelCount = Pa_GetDeviceInfo(*outDevice)->maxOutputChannels;
                PaError err =
                    Pa_IsFormatSupported(nullptr, &parameters, Pa_GetDeviceInfo(*outDevice)->defaultSampleRate);
                if (err != paNoError) {
                    fprintf(stdout, "PortAudio error: %s\nRequested sample rate %f for device %s is not supported\n",
                            Pa_GetErrorText(err), sampleRate, Pa_GetDeviceInfo(*inDevice)->name);
                }
            }
        }

        // in case we still don't have a proper device, use the default device
        if (*outDevice == paNoDevice) {
            *outDevice = Pa_GetDefaultOutputDevice();
            if (*outDevice != paNoDevice)
                fprintf(stdout, "Selected default system output device\n");
        }
    } else if (numIns && numOuts) {
        // inputs and outputs
        // if one device is specified, let's try to open another one on matching api
        if (*inDevice == paNoDevice && *outDevice != paNoDevice) {
            *inDevice = Pa_GetHostApiInfo(Pa_GetDeviceInfo(*outDevice)->hostApi)->defaultInputDevice;
            if (*inDevice != paNoDevice)
                fprintf(stdout, "Selected default %s input device\n",
                        Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->name);
        }
        if (*inDevice != paNoDevice && *outDevice == paNoDevice) {
            *outDevice = Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->defaultOutputDevice;
            if (*outDevice != paNoDevice)
                fprintf(stdout, "Selected default %s output device\n",
                        Pa_GetHostApiInfo(Pa_GetDeviceInfo(*outDevice)->hostApi)->name);
        }
#ifdef _WIN32
        // check if devices are having mismatched API, but only if they are defined
        if (*inDevice != paNoDevice && *outDevice != paNoDevice) {
            if (Pa_GetDeviceInfo(*inDevice)->hostApi != Pa_GetDeviceInfo(*outDevice)->hostApi) {
                fprintf(stdout, "Requested devices %s and %s use different API. Setting output device to %s : %s.\n",
                        Pa_GetDeviceInfo(*inDevice)->name, Pa_GetDeviceInfo(*outDevice)->name,
                        Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->name,
                        Pa_GetDeviceInfo(Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->defaultOutputDevice)
                            ->name);
                *outDevice = Pa_GetHostApiInfo(Pa_GetDeviceInfo(*inDevice)->hostApi)->defaultOutputDevice;
            }
        }
        // in case we don't have a proper device, let's try to open one on ASIO
        if (*inDevice == paNoDevice || *outDevice == paNoDevice) {
            *inDevice = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(paASIO))->defaultInputDevice;
            *outDevice = Pa_GetHostApiInfo(Pa_HostApiTypeIdToHostApiIndex(paASIO))->defaultOutputDevice;
            if (*inDevice != paNoDevice && *outDevice != paNoDevice)
                fprintf(stdout, "Selected default ASIO input/output devices\n");
        }
#endif
        // check for matching sampleRate or requested sample rate
        if (*inDevice != paNoDevice && *outDevice != paNoDevice) {
            // these parameters are taken from the DriverSetup, here used for checking only
            PaSampleFormat fmt = paFloat32 | paNonInterleaved;
            PaStreamParameters in_parameters, out_parameters;
            in_parameters.device = *inDevice;
            in_parameters.sampleFormat = fmt;
            in_parameters.hostApiSpecificStreamInfo = NULL;
            in_parameters.channelCount = Pa_GetDeviceInfo(*inDevice)->maxInputChannels;
            out_parameters.device = *outDevice;
            out_parameters.sampleFormat = fmt;
            out_parameters.hostApiSpecificStreamInfo = NULL;
            out_parameters.channelCount = Pa_GetDeviceInfo(*outDevice)->maxOutputChannels;
            if (sampleRate) {
                // check if devices can support requested SR
                PaError err = Pa_IsFormatSupported(&in_parameters, &out_parameters, sampleRate);
                if (err != paNoError) {
                    fprintf(stdout, "\nRequested sample rate %f for devices %s and %s is not supported.\n", sampleRate,
                            Pa_GetDeviceInfo(*inDevice)->name, Pa_GetDeviceInfo(*outDevice)->name);
                    return err;
                }
            } else {
                // if we don't request SR, check if devices have maching SR
                uint32 inSR = uint32(Pa_GetDeviceInfo(*inDevice)->defaultSampleRate);
                uint32 outSR = uint32(Pa_GetDeviceInfo(*outDevice)->defaultSampleRate);
                if (inSR != outSR) {
                    // if defaults are different, check if both devices can be opened using the OUTPUT's SR
                    PaError err = Pa_IsFormatSupported(&in_parameters, &out_parameters,
                                                       Pa_GetDeviceInfo(*outDevice)->defaultSampleRate);
                    if (err != paNoError) {
                        fprintf(stdout,
                                "\nRequested devices %s and %s use different sample rates. "
                                "Please set matching sample rates "
                                "in the Windows Sound Control Panel and try again.\n",
                                Pa_GetDeviceInfo(*inDevice)->name, Pa_GetDeviceInfo(*outDevice)->name);
                        return err;
                    }
                }
            }
        }

        // in case we still don't have a proper device, use the default device
        if (*inDevice == paNoDevice || *outDevice == paNoDevice) {
            *inDevice = Pa_GetDefaultInputDevice();
            *outDevice = Pa_GetDefaultOutputDevice();
            if (*inDevice != paNoDevice && *outDevice != paNoDevice)
                fprintf(stdout, "Selected default system input/output devices\n");
        }
    } else {
        // no inputs nor outputs
        *inDevice = paNoDevice;
        *outDevice = paNoDevice;
    }
    return paNoError;
}
// ====================================================================
//
//
bool SC_PortAudioDriver::DriverSetup(int* outNumSamples, double* outSampleRate) {
    int mDeviceInOut[2];
    PaError paerror;
    const PaDeviceInfo* pdi;
    const PaHostApiInfo* apiInfo;
    const PaStreamInfo* psi;
    PaTime suggestedLatencyIn, suggestedLatencyOut;
    PaDeviceIndex numDevices = Pa_GetDeviceCount();

    // print out all options:
    fprintf(stdout, "\nDevice options:\n");
    for (int i = 0; i < numDevices; i++) {
        pdi = Pa_GetDeviceInfo(i);
#ifndef __APPLE__
        apiInfo = Pa_GetHostApiInfo(pdi->hostApi);
        fprintf(stdout, "  - %s : %s   (device #%d with %d ins %d outs)\n", apiInfo->name, pdi->name, i,
                pdi->maxInputChannels, pdi->maxOutputChannels);
#else
        fprintf(stdout, "  - %s   (device #%d with %d ins %d outs)\n", pdi->name, i, pdi->maxInputChannels,
                pdi->maxOutputChannels);
#endif
    }

    mDeviceInOut[0] = paNoDevice;
    mDeviceInOut[1] = paNoDevice;
    if (mWorld->hw->mInDeviceName)
        GetPaDeviceFromName(mWorld->hw->mInDeviceName, &mDeviceInOut[0], IOType::Input);
    if (mWorld->hw->mOutDeviceName)
        GetPaDeviceFromName(mWorld->hw->mOutDeviceName, &mDeviceInOut[1], IOType::Output);

    // report requested devices
    fprintf(stdout, "\nRequested devices:\n");
    if (mWorld->mNumInputs) {
        fprintf(stdout, "  In (matching device ");
        if (mDeviceInOut[0] == paNoDevice)
            fprintf(stdout, "NOT found");
        else
            fprintf(stdout, "found");
        fprintf(stdout, "):\n  - %s\n", mWorld->hw->mInDeviceName);
    }
    if (mWorld->mNumOutputs) {
        fprintf(stdout, "  Out (matching device ");
        if (mDeviceInOut[1] == paNoDevice)
            fprintf(stdout, "NOT found");
        else
            fprintf(stdout, "found");
        fprintf(stdout, "):\n  - %s\n", mWorld->hw->mOutDeviceName);
    }

    fprintf(stdout, "\n");
    paerror = CheckPaDevices(&mDeviceInOut[0], &mDeviceInOut[1], mWorld->mNumInputs, mWorld->mNumOutputs,
                             mPreferredSampleRate);

    // if we got an error from CheckPaDevices, stop here
    if (paerror != paNoError) {
        PRINT_PORTAUDIO_ERROR(Pa_OpenStream, paerror);
        return paerror == paNoError;
    }

    *outNumSamples = mWorld->mBufLength;
    if (mPreferredSampleRate)
        *outSampleRate = mPreferredSampleRate;
    else {
        if (mDeviceInOut[0] != paNoDevice)
            *outSampleRate = Pa_GetDeviceInfo(mDeviceInOut[0])->defaultSampleRate;
        if (mDeviceInOut[1] != paNoDevice)
            *outSampleRate = Pa_GetDeviceInfo(mDeviceInOut[1])->defaultSampleRate;
    }


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
        PaSampleFormat fmt = paFloat32 | paNonInterleaved;
        if (mDeviceInOut[0] != paNoDevice) {
            // avoid to allocate the 128 virtual channels reported by the portaudio library for ALSA "default"
            mInputChannelCount =
                std::min<size_t>(mWorld->mNumInputs, Pa_GetDeviceInfo(mDeviceInOut[0])->maxInputChannels);
            fprintf(stdout, "  In: %s : %s\n", Pa_GetHostApiInfo(Pa_GetDeviceInfo(mDeviceInOut[0])->hostApi)->name,
                    Pa_GetDeviceInfo(mDeviceInOut[0])->name);
        } else {
            mInputChannelCount = 0;
        }

        if (mDeviceInOut[1] != paNoDevice) {
            // avoid to allocate the 128 virtual channels reported by the portaudio library for ALSA "default"
            mOutputChannelCount =
                std::min<size_t>(mWorld->mNumOutputs, Pa_GetDeviceInfo(mDeviceInOut[1])->maxOutputChannels);
            fprintf(stdout, "  Out: %s : %s\n", Pa_GetHostApiInfo(Pa_GetDeviceInfo(mDeviceInOut[1])->hostApi)->name,
                    Pa_GetDeviceInfo(mDeviceInOut[1])->name);
        } else {
            mOutputChannelCount = 0;
        }

#ifdef __APPLE__
        // use PortAudio setting tuned for pro audio apps
        PaMacCoreStreamInfo macInfo;
        PaMacCore_SetupStreamInfo(&macInfo, paMacCorePro);
#endif
        PaStreamParameters* inStreamParams_p;
        PaStreamParameters inStreamParams;
        if (mDeviceInOut[0] != paNoDevice) {
            inStreamParams.device = mDeviceInOut[0];
            inStreamParams.channelCount = mInputChannelCount;
            inStreamParams.sampleFormat = fmt;
            inStreamParams.suggestedLatency = suggestedLatencyIn;
#ifdef __APPLE__
            inStreamParams.hostApiSpecificStreamInfo = &macInfo;
#else
            inStreamParams.hostApiSpecificStreamInfo = nullptr;
#endif
            inStreamParams_p = &inStreamParams;
        } else {
            inStreamParams_p = NULL;
        }

        PaStreamParameters* outStreamParams_p;
        PaStreamParameters outStreamParams;
        if (mDeviceInOut[1] != paNoDevice) {
            outStreamParams.device = mDeviceInOut[1];
            outStreamParams.channelCount = mOutputChannelCount;
            outStreamParams.sampleFormat = fmt;
            outStreamParams.suggestedLatency = suggestedLatencyOut;
#ifdef __APPLE__
            outStreamParams.hostApiSpecificStreamInfo = &macInfo;
#else
            outStreamParams.hostApiSpecificStreamInfo = nullptr;
#endif
            outStreamParams_p = &outStreamParams;
        } else {
            outStreamParams_p = NULL;
        }

        // check if format is supported, this sometimes gives a more accurate error information than Pa_OpenStream's
        // error
        paerror = Pa_IsFormatSupported(inStreamParams_p, outStreamParams_p, *outSampleRate);
        if (paerror != paNoError) {
            PRINT_PORTAUDIO_ERROR(Pa_OpenStream, paerror);
            return paerror == paNoError;
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
                fprintf(stdout, "  Sample rate: %.1f\n", psi->sampleRate);
                fprintf(stdout, "  Latency (in/out): %.3f / %.3f sec\n", psi->inputLatency, psi->outputLatency);
#ifdef SC_PA_USE_DLL
                mMaxOutputLatency = psi->outputLatency;
#endif
            }
        }
        return paerror == paNoError;
    }

    // should not be necessary, but a last try with OpenDefaultStream...
    fprintf(stdout, "\nNo devices specified, attempting to use default system devices\n");
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
