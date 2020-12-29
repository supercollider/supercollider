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
#include "SC_Time.hpp"
#include <math.h>
#include <stdlib.h>
#include <emscripten.h>
#include <emscripten/val.h>
#include <emscripten/bind.h>

using namespace emscripten;

int32 server_timeseed() { return timeSeed(); } // TODO

int64 oscTimeNow() { return OSCTime(getTime()); } // TODO

void initializeScheduler() {}

// to be able to see runtime exception text in the browser
extern "C" void EMSCRIPTEN_KEEPALIVE print_error_to_console(intptr_t pointer) {
  auto error = reinterpret_cast<std::runtime_error *>(pointer);
  scprintf("%s\n", error->what());
}

class SC_WebAudioDriver : public SC_AudioDriver {
    int mInputChannelCount, mOutputChannelCount, mBufSize;
    double mSampleRate;
    
    float *mBufInPtr, *mBufOutPtr;

protected:
    virtual bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate);
    virtual bool DriverStart();
    virtual bool DriverStop();

public:
    SC_WebAudioDriver(struct World* inWorld);
    virtual ~SC_WebAudioDriver();

    virtual void WebAudioInitBuffers(
        uintptr_t bufInPtr  , int inputChannelCount, 
        uintptr_t bufOutPtr , int outputChannelCount, int bufSize, double sampleRate);

    virtual void WebAudioProcess();

    int WebAudioCallback(const void* input, void* output, unsigned long frameCount);

    static SC_WebAudioDriver* instance;
};

SC_WebAudioDriver* SC_WebAudioDriver::instance = NULL;

extern "C" SC_WebAudioDriver* audio_driver() {
    if (SC_WebAudioDriver::instance == NULL) {
        throw std::runtime_error("SC_WebAudioDriver instance has not yet been created\n");
    }
    return SC_WebAudioDriver::instance;
}

EMSCRIPTEN_BINDINGS(Audio_Driver) {
    emscripten::class_<SC_WebAudioDriver>("SC_WebAudioDriver")
        .function("WebAudioProcess"     , &SC_WebAudioDriver::WebAudioProcess)
        .function("WebAudioInitBuffers" , &SC_WebAudioDriver::WebAudioInitBuffers, emscripten::allow_raw_pointers())
        ;
    emscripten::function("audio_driver", &audio_driver, emscripten::allow_raw_pointers());
}

SC_AudioDriver* SC_NewAudioDriver(struct World* inWorld) { 
    // set up exception error printing function
    EM_ASM({
        window.onerror = function(message, url, line, column, e) {
            if (typeof e == 'number') { // e is a pointer to std:runtime_error
                Module.ccall('print_error_to_console', 'number', ['number'], [e]);
            }
        }
    });
    return new SC_WebAudioDriver(inWorld); 
}

SC_WebAudioDriver::SC_WebAudioDriver(struct World* inWorld): SC_AudioDriver(inWorld) {
    if (SC_WebAudioDriver::instance != NULL) {
        throw std::runtime_error("SC_WebAudioDriver can only have one instance\n");
    }
    SC_WebAudioDriver::instance = this;
}

SC_WebAudioDriver::~SC_WebAudioDriver() {
}

static int SC_WebAudioStreamCallback(const void* input, void* output, unsigned long frameCount,
                                      void* userData) {
    SC_WebAudioDriver* driver = (SC_WebAudioDriver*)userData;

    return driver->WebAudioCallback(input, output, frameCount);
}

void sc_SetDenormalFlags();

int SC_WebAudioDriver::WebAudioCallback(const void* input, void* output, unsigned long frameCount) {
    sc_SetDenormalFlags();
    World* world = mWorld;
    (void) frameCount; // suppress unused parameter warnings

    mAudioSync.Signal();

    return 0;
}

void SC_WebAudioDriver::WebAudioInitBuffers(
    uintptr_t bufInPtr  , int inputChannelCount, 
    uintptr_t bufOutPtr , int outputChannelCount, int bufSize, double sampleRate) {

    printf("SC_WebAudio: WebAudioInitBuffers.\n");
    // cf. https://stackoverflow.com/questions/20355880/#27364643
    this->mBufInPtr             = reinterpret_cast<float*>(bufInPtr );
    this->mBufOutPtr            = reinterpret_cast<float*>(bufOutPtr);
    this->mInputChannelCount    = inputChannelCount;
    this->mOutputChannelCount   = outputChannelCount;
    this->mBufSize              = bufSize;
    this->mSampleRate           = sampleRate;
}

void SC_WebAudioDriver::WebAudioProcess() {

}

bool SC_WebAudioDriver::DriverSetup(int* outNumSamples, double* outSampleRate) {
    scprintf("SC_WebAudio: DriverSetup.\n");

    int res = EM_ASM_INT({
        var AudioContext = window.AudioContext || window.webkitAudioContext;
        if (!AudioContext) return -1;
        if (!Module.audioDriver) Module.audioDriver = {};
        var ad              = Module.audioDriver;
        var opt             = {};
        // $1 is mPreferredSampleRate;
        if ($1) opt.sampleRate = $1;
        ad.context          = new AudioContext(opt);
        ad.inChanCount      = 0;
        ad.outChanCount     = 2;
        // $0 is mPreferredHardwareBufferFrameSize;
        // note: using zero buffer size gives us default buffer size;
        ad.proc             = ad.context.createScriptProcessor($0, ad.inChanCount, ad.outChanCount);
        ad.bufSize          = ad.proc.bufferSize;
        ad.sampleRate       = ad.context.sampleRate;
        var bytesPerChan    = ad.bufSize * Float32Array.BYTES_PER_ELEMENT;
        var numInBytes      = ad.inChanCount  * bytesPerChan;
        var numOutBytes     = ad.outChanCount * bytesPerChan;
        ad.bufInPtr         = Module._malloc(numInBytes );
        ad.bufOutPtr        = Module._malloc(numOutBytes);
        ad.floatBufIn       = [];
        ad.floatBufOut      = [];
        for (var ch = 0; ch < ad.inChanCount; ch++) {
            ad.floatBufIn [ch] = new Float32Array(Module.HEAPU8.buffer, ad.bufInPtr  + (ch * bytesPerChan), ad.bufSize);
        }
        for (var ch = 0; ch < ad.outChanCount; ch++) {
            ad.floatBufOut[ch] = new Float32Array(Module.HEAPU8.buffer, ad.bufOutPtr + (ch * bytesPerChan), ad.bufSize);
        }
        var self            = Module.audio_driver();
        self.WebAudioInitBuffers(ad.bufInPtr, ad.inChanCount, ad.bufOutPtr, ad.outChanCount, ad.bufSize, ad.sampleRate);
        ad.proc.onaudioprocess = function(e) {
            self.process();
            var bOut    = e.outputBuffer;
            var bProc   = ad.floatBufOut;
            var numCh   = Math.min(bOut.numberOfChannels, ad.outChanCount);
            var bufSize = ad.bufSize;
            for (var ch = 0; ch < numCh; ch++) {
                bOut.copyToChannel(bProc[ch], ch, 0);
            }
        };
        return 0;
    }, mPreferredHardwareBufferFrameSize, mPreferredSampleRate);

    if (res != 0) return false;

    *outNumSamples = mBufSize;
    *outSampleRate = mSampleRate;
    return true;
}

bool SC_WebAudioDriver::DriverStart() {
    scprintf("SC_WebAudio: DriverStart.\n");
    return true;
}

bool SC_WebAudioDriver::DriverStop() {
    scprintf("SC_WebAudio: DriverStop.\n");
    return true;
}
