/*
    Bela audio driver for SuperCollider.
    Copyright (c) 2016 Dan Stowell. All rights reserved.
    Copyright (c) 2016 Marije Baalman. All rights reserved.
    Copyright (c) 2016 Giulio Moro. All rights reserved.

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

    This file contains elements from SC_PortAudio.cpp and SC_Jack.cpp,
    copyright their authors, and published under the same licence.
*/
#include "SC_CoreAudio.h"
#include "SC_Prototypes.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "SC_Time.hpp"
#include "SC_BelaScope.h"
#include <cmath> // floor
#include <cstdlib> // exit
#include <cstring> // memcpy
#include <cobalt/time.h> // needed for CLOCK_HOST_REALTIME
#include <cobalt/stdio.h> // rt_vprintf

extern "C" {
// This will be wrapped by Xenomai without requiring linker flags
int __wrap_clock_gettime(clockid_t clock_id, struct timespec* tp);
}

#include "Bela.h"
// Xenomai-specific includes
#include <sys/mman.h>

#if (BELA_MAJOR_VERSION == 1 && BELA_MINOR_VERSION < 9)
#    error You need at least Bela API 1.9
#endif

// Audio driver API implementation
int32 server_timeseed() { return timeSeed(); }
int64 gOSCoffset = 0;
int64 oscTimeNow() { return OSCTime(getTime()); }
void initializeScheduler() { gOSCoffset = oscTimeNow(); }

class SC_BelaDriver final : public SC_AudioDriver {
public:
    SC_BelaDriver(World* inWorld);
    virtual ~SC_BelaDriver();

    void BelaAudioCallback(BelaContext* belaContext);
    bool BelaSetup(const BelaContext* belaContext);
    void SignalReceived(int signal);

    static SC_BelaDriver* s_instance;
    static SC_BelaDriver* Construct(World* inWorld);

protected:
    // Driver interface methods
    bool DriverSetup(int* outNumSamples, double* outSampleRate) override;
    bool DriverStart() override;
    bool DriverStop() override;

private:
    int mInputChannelCount = 0, mOutputChannelCount = 0;
    uint32 mSCBufLength;
    float mBelaSampleRate = 0;
};

SC_BelaDriver* SC_BelaDriver::s_instance = nullptr;

SC_AudioDriver* SC_NewAudioDriver(World* inWorld) { return SC_BelaDriver::Construct(inWorld); }

SC_BelaDriver* SC_BelaDriver::Construct(World* inWorld) {
    if (s_instance != nullptr) {
        scprintf("*** ERROR: Asked to construct a second instance of SC_BelaDriver.\n");
        std::exit(1);
    }

    s_instance = new SC_BelaDriver(inWorld);
    return s_instance;
}

SC_BelaDriver::SC_BelaDriver(World* inWorld): SC_AudioDriver(inWorld), mSCBufLength(inWorld->mBufLength) {
    mStartHostSecs = 0;
}

SC_BelaDriver::~SC_BelaDriver() {
    // Clean up any resources allocated for audio
    Bela_cleanupAudio();
    scprintf("SC_BelaDriver: >>Bela_cleanupAudio\n");
    s_instance = nullptr;
    delete mWorld->mBelaScope;
    mWorld->mBelaScope = nullptr;
}

// Return true on success; returning false halts the program.
bool SC_BelaDriver::BelaSetup(const BelaContext* belaContext) {
    mBelaSampleRate = belaContext->audioSampleRate;
    delete mWorld->mBelaScope;
    mWorld->mBelaScope = nullptr;
    if (mWorld->mBelaMaxScopeChannels > 0)
        mWorld->mBelaScope = new BelaScope(mWorld->mBelaMaxScopeChannels, mBelaSampleRate, belaContext->audioFrames);
    return true;
}

// Defined in SC_World.cpp
void sc_SetDenormalFlags();

void SC_BelaDriver::BelaAudioCallback(BelaContext* belaContext) {
    struct timespec tspec;

    sc_SetDenormalFlags();
    // add a pointer to belaWorld
    mWorld->mBelaContext = belaContext;

    // NOTE: code here is adapted from the SC_Jack.cpp, the version not using the DLL

    // Use Xenomai-friendly clock_gettime()
    __wrap_clock_gettime(CLOCK_HOST_REALTIME, &tspec);

    double hostSecs = static_cast<double>(tspec.tv_sec) + static_cast<double>(tspec.tv_nsec) * 1.0e-9;
    double sampleTime = static_cast<double>(belaContext->audioFramesElapsed);

    if (mStartHostSecs == 0) {
        mStartHostSecs = hostSecs;
        mStartSampleTime = sampleTime;
    } else {
        double instSampleRate = (sampleTime - mPrevSampleTime) / (hostSecs - mPrevHostSecs);
        double smoothSampleRate = mSmoothSampleRate;
        smoothSampleRate = smoothSampleRate + 0.002 * (instSampleRate - smoothSampleRate);
        if (fabs(smoothSampleRate - mSampleRate) > 10.) {
            smoothSampleRate = mSampleRate;
        }
        mOSCincrement = static_cast<int64>(mOSCincrementNumerator / smoothSampleRate);
        mSmoothSampleRate = smoothSampleRate;
    }

    mPrevHostSecs = hostSecs;
    mPrevSampleTime = sampleTime;

    try {
        mFromEngine.Free();
        mToEngine.Perform();
        mOscPacketsToEngine.Perform();

        const uint32_t numInputs = belaContext->audioInChannels;
        const uint32_t numOutputs = belaContext->audioOutChannels;

        int numSamples = NumSamplesPerCallback();
        int bufFrames = mWorld->mBufLength;
        int numBufs = numSamples / bufFrames;

        float* inBuses = mWorld->mAudioBus + mWorld->mNumOutputs * bufFrames;
        float* outBuses = mWorld->mAudioBus;

        int minInputs = sc_min(numInputs, mWorld->mNumInputs);
        int minOutputs = sc_min(numOutputs, mWorld->mNumOutputs);

        int anaInputs = 0;
        if (numInputs < mWorld->mNumInputs) {
            anaInputs = sc_min(belaContext->analogInChannels, static_cast<int>(mWorld->mNumInputs - numInputs));
        }
        int anaOutputs = 0;
        if (numOutputs < mWorld->mNumOutputs) {
            anaOutputs = sc_min(belaContext->analogOutChannels, static_cast<int>(mWorld->mNumOutputs - numOutputs));
        }

        // main loop
        mOSCbuftime = (static_cast<int64>(tspec.tv_sec + kSECONDS_FROM_1900_to_1970) << 32)
            + static_cast<int64>(tspec.tv_nsec * kNanosToOSCunits);

        // clear out anything left over in audioOut buffer
        for (int i = 0; i < belaContext->audioFrames * belaContext->audioOutChannels; i++) {
            belaContext->audioOut[i] = 0;
        }

        for (int i = 0; i < numBufs; ++i, mWorld->mBufCounter++) {
            // copy+touch inputs
            int32* tch = mWorld->mAudioBusTouched + mWorld->mNumOutputs;
            std::memcpy(inBuses, belaContext->audioIn, sizeof(belaContext->audioIn[0]) * bufFrames * minInputs);
            for (int k = 0; k < minInputs; ++k) {
                *tch++ = mWorld->mBufCounter;
            }

            std::memcpy(inBuses + minInputs * bufFrames, belaContext->analogIn,
                        sizeof(belaContext->analogIn[0]) * bufFrames * anaInputs);
            for (int k = minInputs; k < (minInputs + anaInputs); ++k) {
                *tch++ = mWorld->mBufCounter;
            }

            // run engine
            int64 schedTime;
            int64 nextTime = mOSCbuftime + mOSCincrement;

            while ((schedTime = mScheduler.NextTime()) <= nextTime) {
                float diffTime = static_cast<float>(schedTime - mOSCbuftime) * mOSCtoSamples + 0.5;
                float diffTimeFloor = std::floor(diffTime);
                mWorld->mSampleOffset = static_cast<int>(diffTimeFloor);
                mWorld->mSubsampleOffset = diffTime - diffTimeFloor;

                if (mWorld->mSampleOffset < 0)
                    mWorld->mSampleOffset = 0;
                else if (mWorld->mSampleOffset >= mWorld->mBufLength)
                    mWorld->mSampleOffset = mWorld->mBufLength - 1;

                SC_ScheduledEvent event = mScheduler.Remove();
                event.Perform();
            }

            mWorld->mSampleOffset = 0;
            mWorld->mSubsampleOffset = 0.f;
            World_Run(mWorld);

            // copy touched outputs
            tch = mWorld->mAudioBusTouched;

            for (int k = 0; k < minOutputs; ++k) {
                if (*tch++ == mWorld->mBufCounter) {
                    std::memcpy(belaContext->audioOut + k * bufFrames, outBuses + k * bufFrames,
                                sizeof(belaContext->audioOut[0]) * bufFrames);
                }
            }

            for (int k = minOutputs; k < (minOutputs + anaOutputs); ++k) {
                if (*tch++ == mWorld->mBufCounter) {
                    unsigned int analogChannel = k - minOutputs; // starting at 0
                    std::memcpy(belaContext->analogOut + analogChannel * bufFrames, outBuses + k * bufFrames,
                                sizeof(belaContext->analogOut[0]) * bufFrames);
                }
            }

            // advance OSC time
            mOSCbuftime = nextTime;
        }

        if (mWorld->mBelaScope)
            mWorld->mBelaScope->logBuffer();

    } catch (std::exception& exc) { scprintf("SC_BelaDriver: exception in real time: %s\n", exc.what()); } catch (...) {
        scprintf("SC_BelaDriver: unknown exception in real time\n");
    }

    mAudioSync.Signal();
}

// ====================================================================

bool SC_BelaDriver::DriverSetup(int* outNumSamples, double* outSampleRate) {
    BelaInitSettings* settings = Bela_InitSettings_alloc();
    Bela_defaultSettings(settings);

    settings->setup = [](BelaContext* belaContext, void* userData) {
        return static_cast<SC_BelaDriver*>(userData)->BelaSetup(belaContext);
    };
    settings->render = [](BelaContext* belaContext, void* userData) {
        static_cast<SC_BelaDriver*>(userData)->BelaAudioCallback(belaContext);
    };
    // add a callback to be called when the audio thread stops. This is useful
    // e.g.: to gracefully exit from scsynth when pressing the Bela button
    settings->audioThreadDone = [](BelaContext*, void* userData) {
        auto* driver = static_cast<SC_BelaDriver*>(userData);
        if (driver)
            driver->SignalReceived(0);
    };

    settings->interleave = 0;
    settings->uniformSampleRate = 1;
    settings->analogOutputsPersist = 0;

    if (mPreferredHardwareBufferFrameSize) {
        settings->periodSize = mPreferredHardwareBufferFrameSize;
    }
    if (settings->periodSize != mSCBufLength) {
        scprintf("Warning in SC_BelaDriver::DriverSetup(): hardware buffer size (%i) different from SC audio buffer "
                 "size (%i). Changed the hardware buffer size to be equal to the SC audio buffer size .\n",
                 settings->periodSize, mSCBufLength);
        settings->periodSize = mSCBufLength;
    }
    // note that Bela doesn't give us an option to choose samplerate, since
    // it's baked-in for a given board, however this can be retrieved in sc_belaSetup()

    // configure the number of analog channels - this will determine their internal samplerate
    settings->useAnalog = 0;

    // explicitly requested number of analog channels
    int numAnalogIn = mWorld->hw->mBelaAnalogInputChannels;
    int numAnalogOut = mWorld->hw->mBelaAnalogOutputChannels;

    // Here is the deal. We need to know:
    // - how many real audio channels are available
    // - how many audio channels the user wants
    // - how many analog channels are available
    // before we can request Bela for:
    // - a given number of analog channels
    // - applying the audio expander capelet on these channels
    // Currently (as of 1.4.0) the Bela API does not allow to
    // know the number of audio channels available.

    BelaHwConfig* cfg = Bela_HwConfig_new(Bela_detectHw());
    int extraAudioIn = mWorld->mNumInputs - cfg->audioInChannels;
    int extraAudioOut = mWorld->mNumOutputs - cfg->audioOutChannels;
    // if we need more audio channels than there actually are audio
    // channels, make sure we have some extra analogs
    if (extraAudioIn > 0) {
        numAnalogIn = sc_max(numAnalogIn, extraAudioIn);
    }
    if (extraAudioOut > 0) {
        numAnalogOut = sc_max(numAnalogOut, extraAudioOut);
    }

    // snap the number of requested analog channels to the 0, 4, 8.
    // 4 will give same actual sample rate as audio, 8 will give half of it.
    if (numAnalogIn > 0) {
        if (numAnalogIn < 5) {
            numAnalogIn = 4;
        } else {
            numAnalogIn = 8;
        }
    }

    if (numAnalogOut > 0) {
        if (numAnalogOut < 5) {
            numAnalogOut = 4;
        } else {
            numAnalogOut = 8;
        }
    }

    // final check: right now the number of analog output channels on bela needs to be the same as analog input
    // channels. this is likely to change in the future, that is why we factored it out
    if (numAnalogOut != numAnalogIn) {
        // Chosing the maximum of the two
        numAnalogOut = sc_max(numAnalogOut, numAnalogIn);
        numAnalogIn = numAnalogOut;
        scprintf("Number of analog input channels must match number of analog outputs. Using %u for both\n",
                 numAnalogIn);
    }
    settings->numAnalogInChannels = numAnalogIn;
    settings->numAnalogOutChannels = numAnalogOut;

    if (settings->numAnalogInChannels > 0 || settings->numAnalogOutChannels > 0) {
        settings->useAnalog = 1;
    }

    // enable the audio expander capelet for the first few "analog as audio" channels
    // inputs and ...
    for (int n = 0; n < extraAudioIn; ++n) {
        scprintf("Using analog in %d as audio in %d\n", n, n + cfg->audioInChannels);
        settings->audioExpanderInputs |= (1 << n);
    }

    // ... outputs
    for (int n = 0; n < extraAudioOut; ++n) {
        scprintf("Using analog out %d as audio out %d\n", n, n + cfg->audioOutChannels);
        settings->audioExpanderOutputs |= (1 << n);
    }

    // configure the number of digital channels
    settings->useDigital = 0;

    if (mWorld->hw->mBelaDigitalChannels > 0) {
        settings->numDigitalChannels = mWorld->hw->mBelaDigitalChannels;
        settings->useDigital = 1;
    }
    if ((mWorld->hw->mBelaHeadphoneLevel >= -63.5)
        && (mWorld->hw->mBelaHeadphoneLevel <= 0.)) { // headphone output level (0dB max; -63.5dB min)
        settings->headphoneLevel = mWorld->hw->mBelaHeadphoneLevel;
    }
    if ((mWorld->hw->mBelaPgaGainLeft >= 0) && (mWorld->hw->mBelaPgaGainLeft <= 59.5)) { // (0db min; 59.5db max)
        settings->pgaGain[0] = mWorld->hw->mBelaPgaGainLeft;
    }
    if ((mWorld->hw->mBelaPgaGainRight >= 0) && (mWorld->hw->mBelaPgaGainRight <= 59.5)) { // (0db min; 59.5db max)
        settings->pgaGain[1] = mWorld->hw->mBelaPgaGainRight;
    }

    if (mWorld->hw->mBelaSpeakerMuted) {
        settings->beginMuted = 1;
    } else {
        settings->beginMuted = 0;
    }
    if ((mWorld->hw->mBelaDacLevel >= -63.5) && (mWorld->hw->mBelaDacLevel <= 0.)) { // (0dB max; -63.5dB min)
        settings->dacLevel = mWorld->hw->mBelaDacLevel;
    }
    if ((mWorld->hw->mBelaAdcLevel >= -12) && (mWorld->hw->mBelaAdcLevel <= 0.)) { // (0dB max; -12dB min)
        settings->adcLevel = mWorld->hw->mBelaAdcLevel;
    }

    settings->numMuxChannels = mWorld->hw->mBelaNumMuxChannels;

    if ((mWorld->hw->mBelaPru == 0) || (mWorld->hw->mBelaPru == 1)) {
        settings->pruNumber = mWorld->hw->mBelaPru;
    }

    scprintf("SC_BelaDriver: >>DriverSetup - Running on PRU (%i)\nConfigured with \n (%i) analog input and (%i) analog "
             "output channels, (%i) digital channels, and (%i) multiplexer channels.\n HeadphoneLevel (%f dB), "
             "pga_gain_left (%f dB) and pga_gain_right (%f dB)\n DAC Level (%f dB), ADC Level (%f dB) "
             "oscilloscope channels (%i)\n",
             settings->pruNumber, settings->numAnalogInChannels, settings->numAnalogOutChannels,
             settings->numDigitalChannels, settings->numMuxChannels, settings->headphoneLevel, settings->pgaGain[0],
             settings->pgaGain[1], settings->dacLevel, settings->adcLevel, mWorld->mBelaMaxScopeChannels);
    if (settings->beginMuted == 1) {
        scprintf("Speakers are muted.\n");
    } else {
        scprintf("Speakers are not muted.\n");
    }

    settings->verbose = mWorld->mVerbosity;
    // This call will initialise the rendering system, which in the process
    // will result in a call to the user-defined setup() function.
    if (Bela_initAudio(settings, this) != 0) {
        scprintf("Error in SC_BelaDriver::DriverSetup(): unable to initialise audio\n");
        return false;
    }

    *outNumSamples = settings->periodSize;
    *outSampleRate = mBelaSampleRate;
    Bela_InitSettings_free(settings);
    Bela_HwConfig_delete(cfg);

    // Set up interrupt handler to catch Control-C and SIGTERM

    auto signalFunc = [](int signal) {
        if (SC_BelaDriver::s_instance != nullptr)
            SC_BelaDriver::s_instance->SignalReceived(signal);
    };
    signal(SIGINT, signalFunc);
    signal(SIGTERM, signalFunc);

    return true;
}

bool SC_BelaDriver::DriverStart() {
    SetPrintFunc(rt_vprintf); // Use Xenomai's realtime-friendly printing function
    if (Bela_startAudio()) {
        scprintf("Error in SC_BelaDriver::DriverStart(): unable to start real-time audio\n");
        return false;
    }
    return true;
}

bool SC_BelaDriver::DriverStop() {
    Bela_stopAudio();
    return true;
}

void SC_BelaDriver::SignalReceived(int signal) {
    scprintf("SC_BelaDriver: signal received: %d; terminating\n", signal);
    mWorld->hw->mTerminating = true;
    mWorld->hw->mQuitProgram->post();
}
