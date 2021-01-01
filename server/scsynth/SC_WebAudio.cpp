/*
    WebAudio driver interface.
    Copyright (c) 2020 Hanns Holger Rutz.

    ====================================================================

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
#include "SC_TimeDLL.hpp"
#include "SC_Time.hpp"
#include <math.h>
#include <stdlib.h>
#include <emscripten.h>
#include <emscripten/bind.h>

using namespace emscripten;

static const char* kWebAudioIdent = "SC_WebAudio";

// #define SC_WEB_AUDIO_DRIVER_DEBUG

int32 server_timeseed() { return timeSeed(); }

int64 oscTimeNow() { return OSCTime(getTime()); }

static double waOscTimeSeconds() { return OSCTime(getTime()) * kOSCtoSecs; }

void initializeScheduler() {}

// to be able to see runtime exception text in the browser
extern "C" void EMSCRIPTEN_KEEPALIVE print_error_to_console(intptr_t pointer) {
    auto error = reinterpret_cast<std::runtime_error*>(pointer);
    scprintf("%s\n", error->what());
}

class SC_WebAudioDriver : public SC_AudioDriver {
    // the number of input and output channels connect to/from Web Audio
    int mNumInChannels, mNumOutChannels;
    // the number of frames per processing step in Web Audio.
    // i.e. what is returned as outNumSamplesPerCallback
    int mBufSize;
    double mSampleRate;
    // the Float32Array pointers for Web Audio
    float *mBufInPtr, *mBufOutPtr;
    double mMaxOutputLatency;
    SC_TimeDLL mDLL;

protected:
    virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
    virtual bool DriverStart();
    virtual bool DriverStop();

public:
    SC_WebAudioDriver(struct World* inWorld);
    virtual ~SC_WebAudioDriver();

    // passes the Float32Array buffers allocated from JS to this instance,
    // where they are stored in mBufInPtr, mBufOutPtr, etc.
    // this is called from JS during driver setup.
    virtual void WaInitBuffers(uintptr_t bufInPtr, int numInChannels, uintptr_t bufOutPtr, int numOutChannels,
                               int bufSize, double sampleRate, double outputLatency);

    // the main processing callback.
    // this is called from JS during the script processor node's `onaudioprocess`.
    virtual void WaRun();

    // access to singleton instance which is needed from JS
    static SC_WebAudioDriver* instance;
};

SC_WebAudioDriver* SC_WebAudioDriver::instance = NULL;

// function callable from JS to obtain the singleton instance
extern "C" SC_WebAudioDriver* audio_driver() {
    if (SC_WebAudioDriver::instance == NULL) {
        throw std::runtime_error("SC_WebAudio: instance has not yet been created\n");
    }
    return SC_WebAudioDriver::instance;
}

EMSCRIPTEN_BINDINGS(Web_Audio) {
    emscripten::class_<SC_WebAudioDriver>("SC_WebAudioDriver")
        .function("WaRun", &SC_WebAudioDriver::WaRun)
        .function("WaInitBuffers", &SC_WebAudioDriver::WaInitBuffers, emscripten::allow_raw_pointers());
    emscripten::function("audio_driver", &audio_driver, emscripten::allow_raw_pointers());
}

SC_AudioDriver* SC_NewAudioDriver(struct World* inWorld) {
    // set up exception error printing function
    // clang-format off
    EM_ASM({
        window.onerror = function(message, url, line, column, e) {
            if (typeof e == 'number') { // e is a pointer to std:runtime_error
                Module.ccall('print_error_to_console', 'number', ['number'], [e]);
            }
        }
    });
// clang-format on
return new SC_WebAudioDriver(inWorld);
}

SC_WebAudioDriver::SC_WebAudioDriver(struct World* inWorld): SC_AudioDriver(inWorld), mMaxOutputLatency(0.0) {
    if (SC_WebAudioDriver::instance != NULL) {
        throw std::runtime_error("SC_WebAudio: can only have one instance\n");
    }
    SC_WebAudioDriver::instance = this;
}

SC_WebAudioDriver::~SC_WebAudioDriver() {
    mBufInPtr = NULL;
    mBufOutPtr = NULL;
    SC_WebAudioDriver::instance = NULL;
    // clang-format off
    EM_ASM({
        var ad = Module.audioDriver; 
        if (ad) {
            if (ad.context) {
                ad.context.close();
            }
            if (ad.bufInPtr) {
                Module._free(ad.bufInPtr);
            }
            if (ad.bufOutPtr) {
                Module._free(ad.bufOutPtr);
            }
            Module.audioDriver = undefined;
        }
    });
    // clang-format on
}

void SC_WebAudioDriver::WaInitBuffers(uintptr_t bufInPtr, int numInChannels, uintptr_t bufOutPtr, int numOutChannels,
                                      int bufSize, double sampleRate, double outputLatency) {
#ifdef SC_WEB_AUDIO_DRIVER_DEBUG
    scprintf("%s: WaInitBuffers.\n", kWebAudioIdent);
#endif
    // cf. https://stackoverflow.com/questions/20355880/#27364643
    this->mBufInPtr = reinterpret_cast<float*>(bufInPtr);
    this->mBufOutPtr = reinterpret_cast<float*>(bufOutPtr);
    this->mNumInChannels = numInChannels;
    this->mNumOutChannels = numOutChannels;
    this->mBufSize = bufSize;
    this->mSampleRate = sampleRate;
    this->mMaxOutputLatency = outputLatency;
}

void sc_SetDenormalFlags();

void SC_WebAudioDriver::WaRun() {
    sc_SetDenormalFlags();
    World* world = mWorld;
    mDLL.Update(waOscTimeSeconds());

    try {
        mFromEngine.Free();
        mToEngine.Perform();
        // scprintf("%s: --> mOscPacketsToEngine\n", kWebAudioIdent);
        mOscPacketsToEngine.Perform();
        // scprintf("%s: <-- mOscPacketsToEngine\n", kWebAudioIdent);

        float* bufIn = mBufInPtr;
        float* bufOut = mBufOutPtr;
        int bufSizeWA = mBufSize;
        int numFrames = NumSamplesPerCallback(); // should be the same as bufSizeWA; redundant? assert?
        int stepSize = mWorld->mBufLength;
        int numSteps = numFrames / stepSize;

        float* inBuses = mWorld->mAudioBus + mWorld->mNumOutputs * stepSize;
        float* outBuses = mWorld->mAudioBus;
        int32* inTouched = mWorld->mAudioBusTouched + mWorld->mNumOutputs;
        int32* outTouched = mWorld->mAudioBusTouched;

        int minInputs = sc_min(mNumInChannels, (int)mWorld->mNumInputs);
        int minOutputs = sc_min(mNumOutChannels, (int)mWorld->mNumOutputs);

        int offStep = 0;

        // main loop
        int64 oscTime = mOSCbuftime = (mDLL.PeriodTime() + mMaxOutputLatency) * kSecondsToOSCunits + 0.5;
        int64 oscInc = mOSCincrement = (mDLL.Period() / numSteps) * kSecondsToOSCunits + 0.5;
        mSmoothSampleRate = mDLL.SampleRate();
        double oscToSamples = mOSCtoSamples = mSmoothSampleRate * kOSCtoSecs;

        // we step through `numFrames` aka `bufSizeWA` in SC's internal
        // block size aka `stepSize`, making `numSteps` steps.
        for (int i = 0; i < numSteps; i++, mWorld->mBufCounter++, offStep += stepSize) {
            int32 bufCounter = mWorld->mBufCounter;
            int32* tch;

            // copy and touch inputs
            tch = inTouched;
            for (int k = 0, bOffWA = offStep, bOffSC = 0; k < minInputs; k++, bOffWA += bufSizeWA, bOffSC += stepSize) {
                float* src = bufIn + bOffWA;
                float* dst = inBuses + bOffSC;
                for (int n = 0; n < stepSize; n++) {
                    *dst++ = *src++;
                }
                *tch++ = bufCounter;
            }

            // run engine
            int64 schedTime;
            int64 nextTime = oscTime + oscInc;

            while ((schedTime = mScheduler.NextTime()) <= nextTime) {
                float diffTime = (float)(schedTime - oscTime) * oscToSamples + 0.5;
                float diffTimeFloor = floor(diffTime);
                world->mSampleOffset = (int)diffTimeFloor;
                world->mSubsampleOffset = diffTime - diffTimeFloor;

                if (world->mSampleOffset < 0) {
                    world->mSampleOffset = 0;
                } else if (world->mSampleOffset >= world->mBufLength) {
                    world->mSampleOffset = world->mBufLength - 1;
                }

                SC_ScheduledEvent event = mScheduler.Remove();
                event.Perform();
            }

            world->mSampleOffset = 0;
            world->mSubsampleOffset = 0.0f;
            World_Run(world);

            // copy touched outputs
            tch = outTouched;
            for (int k = 0, bOffWA = offStep, bOffSC = 0; k < minOutputs;
                 k++, bOffWA += bufSizeWA, bOffSC += stepSize) {
                float* dst = bufOut + bOffWA;
                if (*tch++ == bufCounter) {
                    float* src = outBuses + bOffSC;
                    for (int n = 0; n < stepSize; n++) {
                        *dst++ = *src++;
                    }
                } else {
                    for (int n = 0; n < stepSize; n++) {
                        *dst++ = 0.0f;
                    }
                }
            }

            // advance OSC time
            mOSCbuftime = oscTime = nextTime;
        }

    } catch (std::exception& exc) {
        scprintf("%s: exception in real time: %s\n", kWebAudioIdent, exc.what());
    } catch (...) {
        scprintf("%s: unknown exception in real time\n", kWebAudioIdent);
    }

    // TODO: how to measure CPU usage?

    // double cpuUsage = ...
    // mAvgCPU = mAvgCPU + 0.1 * (cpuUsage - mAvgCPU);
    // if (cpuUsage > mPeakCPU || --mPeakCounter <= 0) {
    //     mPeakCPU = cpuUsage;
    //     mPeakCounter = mMaxPeakCounter;
    // }

    mAudioSync.Signal();
}

bool SC_WebAudioDriver::DriverSetup(int* outNumSamples, double* outSampleRate) {
#ifdef SC_WEB_AUDIO_DRIVER_DEBUG
    scprintf("%s: DriverSetup.\n", kWebAudioIdent);
#endif

    // clang-format off
    int res = EM_ASM_INT({
        var AudioContext = window.AudioContext || window.webkitAudioContext;
        if (!AudioContext) return -1;
        var bufSizePref     = $0;   // mPreferredHardwareBufferFrameSize
        var sr              = $1;   // mPreferredSampleRate
        var numInSC         = $2;   // mWorld->mNumInputs
        var numOutSC        = $3;   // mWorld->mNumOutputs
        if (!Module.audioDriver) Module.audioDriver = {};
        var ad              = Module.audioDriver;
        var opt             = {};
        if (sr) opt.sampleRate = sr;
        ad.context          = new AudioContext(opt);
        ad.inChanCount      = numInSC;
        // note: Firefox and Chrome behave differently. In Firefox,
        // input can be captured without requiring a connection
        // to AudioContext; but in Chrome, a connection
        // to audioContext.destination must always be made.
        // Therefore, ensure there is at least one output channel
        // on the script processor, even if it stays silent.
        ad.outChanCount     = Math.max(1, numOutSC);
        // note: using zero buffer size gives us default buffer size;
        ad.proc             = ad.context.createScriptProcessor(bufSizePref, ad.inChanCount, ad.outChanCount);
        ad.bufSize          = ad.proc.bufferSize;
        ad.sampleRate       = ad.context.sampleRate;
        var bytesPerChan    = ad.bufSize * Float32Array.BYTES_PER_ELEMENT;
        var numInBytes      = ad.inChanCount  * bytesPerChan;
        var numOutBytes     = ad.outChanCount * bytesPerChan;
        ad.bufInPtr         = Module._malloc(numInBytes );
        ad.bufOutPtr        = Module._malloc(numOutBytes);
        ad.floatBufIn       = [];
        ad.floatBufOut      = [];
        ad.connected        = false;
        for (var ch = 0; ch < ad.inChanCount; ch++) {
            ad.floatBufIn [ch] = new Float32Array(Module.HEAPU8.buffer, ad.bufInPtr  + (ch * bytesPerChan), ad.bufSize);
        }
        for (var ch = 0; ch < ad.outChanCount; ch++) {
            ad.floatBufOut[ch] = new Float32Array(Module.HEAPU8.buffer, ad.bufOutPtr + (ch * bytesPerChan), ad.bufSize);
        }
        var self            = Module.audio_driver();
        var latency         = ad.context.baseLatency || 0.0;
        self.WaInitBuffers(ad.bufInPtr, ad.inChanCount, ad.bufOutPtr, ad.outChanCount, 
            ad.bufSize, ad.sampleRate, latency);
        ad.proc.onaudioprocess = function(ev) {
            var bufSize = ad.bufSize;
            var bIn     = ev.inputBuffer;
            if (bIn) { // inputBuffer is null when inChanCount is zero!
                var bInSC   = ad.floatBufIn;
                var numIn   = Math.min(bIn.numberOfChannels, ad.inChanCount);
                for (var ch = 0; ch < numIn; ch++) {
                    // note: cannot use `copyFromChannel` because running under `SharedArrayBuffer` 
                    // bIn.copyFromChannel(bInSC[ch], ch, 0);
                    var aBuf = bIn.getChannelData(ch);
                    bInSC[ch].set(aBuf);
                }
            }
            self.WaRun();
            var bOut    = ev.outputBuffer;
            var bOutSC  = ad.floatBufOut;
            var numOut  = Math.min(bOut.numberOfChannels, ad.outChanCount);
            for (var ch = 0; ch < numOut; ch++) {
                // note: cannot use `copyToChannel` because running under `SharedArrayBuffer` 
                // bOut.copyToChannel(bOutSC[ch], ch, 0);
                var aBuf = bOut.getChannelData(ch);
                aBuf.set(bOutSC[ch]);
            }
        };
        // connect input (asynchronous)
        if (ad.inChanCount > 0) {
            var md      = window.navigator.mediaDevices;
            var req     = md.getUserMedia({ audio: true });
            req.then(function(ms) {
                ad.input = ad.context.createMediaStreamSource(ms);
                if (ad.connected) { // if already running, connect immediately
                    ad.input.connect(ad.proc);
                }
            }).catch(function(err) {
                console.log("Error: could not obtain audio input device - " + err.name + "; " + err.message);
            });
        }

        return 0;
    }, mPreferredHardwareBufferFrameSize, mPreferredSampleRate, mWorld->mNumInputs, mWorld->mNumOutputs);
    // clang-format on

    if (res != 0)
        return false;

    *outNumSamples = mBufSize;
    *outSampleRate = mSampleRate;
    return true;
}

bool SC_WebAudioDriver::DriverStart() {
#ifdef SC_WEB_AUDIO_DRIVER_DEBUG
    scprintf("%s: DriverStart.\n", kWebAudioIdent);
#endif

    // clang-format off
    int res = EM_ASM_INT({
        var ad = Module.audioDriver;
        if (!ad) return -1;
      
        ad.connected = true;
        if (ad.input) {
            ad.input.connect(ad.proc);
        }
        ad.proc.connect(ad.context.destination);

        return 0;
    });
    // clang-format on

    mDLL.Reset(mSampleRate, mNumSamplesPerCallback, SC_TIME_DLL_BW, waOscTimeSeconds());
    return res == 0;
}

bool SC_WebAudioDriver::DriverStop() {
#ifdef SC_WEB_AUDIO_DRIVER_DEBUG
    scprintf("%s: DriverStop.\n", kWebAudioIdent);
#endif

    // clang-format off
    int res = EM_ASM_INT({
        var ad = Module.audioDriver;
        if (!ad) return -1;

        ad.connected = false;
        ad.proc.disconnect(ad.context.destination);
        if (ad.input) {
            ad.input.disconnect(ad.proc);
        }

        return 0;
    });
    // clang-format on

    return res == 0;
}
