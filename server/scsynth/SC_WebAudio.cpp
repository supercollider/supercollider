/*
    SuperCollider real time audio synthesis system wasm binding
    Copyright (c) 2026 Dennis Scheiba. All rights reserved.
    https://supercollider.github.io/

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <sys/time.h>
#include <emscripten/bind.h>
#include <emscripten/webaudio.h>

#include "SC_CoreAudio.h"
#include "SC_HiddenWorld.h"
#include "SC_Prototypes.h"
#include "SC_Time.hpp"
#include "SC_World.h"
#include "SC_WorldOptions.h"

// forward declarations
static void jsReplyFunc(ReplyAddress*, char* msg, int size);
bool ProcessOSCPacket(World*, OSC_Packet*); // SC_CoreAudio function
class SC_WebAudioDriver;

// size of the print ring buffer - keep this large to avoid any corruption
static constexpr int PRINT_BUFFER_SIZE = 0x100000;

// global variables are initiated during scaffolding of the audio worklet
// and makes passing state around easier since there are some
// async operations involved during the setup procedure.

// OSC time offset: converts emscripten_get_now() elapsed time to absolute NTP-epoch OSC time
static int64 gOSCoffset = 0;
// the singleton instance of the web audio driver
static SC_WebAudioDriver* gScWebAudioDriver = nullptr;
// world of scsynth
static World* gWorld = nullptr;
// stack for audio worklet
static uint8_t gAudioWorkletStack[1024 * 1024];
// global such that it can be also accessed from JS for mic access
static EMSCRIPTEN_WEBAUDIO_T gAudioContext = 0;
// global such that ic can be also accessed from JS for mic access
static EMSCRIPTEN_AUDIO_WORKLET_NODE_T gScsynthWorkletNode = 0;


/**
 * @brief Exposed to JavaScript such that it can be used to configure the server.
 * WorldOptions itself is not exposed in order to limit access.
 */
struct ScWebAudioOptions {
    uint32 numBuffers = 1024;
    uint32 maxNodes = 1024;
    uint32 maxGraphDefs = 1024;
    uint32 maxWireBufs = 64;
    uint32 numAudioBusChannels = 1024;
    uint32 numInputBusChannels = 0;
    uint32 numOutputBusChannels = 2;
    uint32 numControlBusChannels = 16384;
    uint32 bufLength = 64;
    uint32 realTimeMemorySize = 8192;
    uint32 verbosity = 0;
};

/**
 * @brief Helper method to convert JS spaced ScWebAudioOptions struct to
 * internal WorldOptions struct.
 *
 * @param userOptions options received from JS
 * @return WorldOption to boot the server
 */
WorldOptions toScWorld(ScWebAudioOptions* userOptions) {
    WorldOptions options;
    options.mNumBuffers = userOptions->numBuffers;
    options.mMaxNodes = userOptions->maxNodes;
    options.mMaxGraphDefs = userOptions->maxGraphDefs;
    options.mMaxWireBufs = userOptions->maxWireBufs;
    options.mNumAudioBusChannels = userOptions->numAudioBusChannels;
    options.mNumInputBusChannels = userOptions->numInputBusChannels;
    options.mNumOutputBusChannels = userOptions->numOutputBusChannels;
    options.mNumControlBusChannels = userOptions->numControlBusChannels;
    options.mBufLength = userOptions->bufLength;

    // hardcoded options
    options.mLoadGraphDefs = false;
    options.mRealTime = 1;
    options.mSharedMemoryID = 0;
    options.mVerbosity = 0;
    // @todo make this user configurable?
    options.mMaxLogins = 4;
    return options;
}

/**
 * @brief Implementation of an AudioWorklet as an audio driver for scsynth.
 * See https://emscripten.org/docs/api_reference/wasm_audio_worklets.html and
 * https://github.com/emscripten-core/emscripten/blob/main/test/webaudio/audioworklet.c
 *
 */
class SC_WebAudioDriver : public SC_AudioDriver {
public:
    SC_WebAudioDriver(World* inWorld): SC_AudioDriver(inWorld) {};
    ~SC_WebAudioDriver() override = default;

    /** @brief calculates the next 128 (quantum) samples */
    bool processBlock(int numInputs, const AudioSampleFrame* inputs, int numOutputs, AudioSampleFrame* outputs);
    /** @brief passes osc bytes into the scsynth world */
    bool sendOsc(const char* data, int size);

protected:
    bool DriverSetup(int* outNumSamplesPerCallback, double* outSampleRate) override {
        // hardcoded values from AudioWorklet - these should not change
        *outSampleRate = WASM_SAMPLERATE;
        *outNumSamplesPerCallback = WASM_QUANTUM_SIZE;
        return true;
    };
    bool DriverStart() override;
    bool DriverStop() override { return true; };

private:
    // web browsers only allow for quantum size of 128, see
    // https://developer.mozilla.org/en-US/docs/Web/API/AudioWorkletProcessor/process
    // https://www.w3.org/TR/webaudio-1.1/#render-quantum-size
    static constexpr size_t WASM_QUANTUM_SIZE = 128;
    // 48k seems to be a default in the web, see e.g. opus.
    static constexpr size_t WASM_SAMPLERATE = 48000;
    // whether mOSCbuftime has been initialized with wall-clock time
    bool mInitializedOSCTime = false;
};

SC_AudioDriver* SC_NewAudioDriver(World* inWorld) { return new SC_WebAudioDriver(inWorld); }

/** @brief the exposed C function which the WebAudio worklet calls into to get the next n samples. */
static bool scsynthProcessCallback(int numInputs, const AudioSampleFrame* inputs, int numOutputs,
                                   AudioSampleFrame* outputs, int numParams, const AudioParamFrame* params,
                                   void* userData) {
    SC_WebAudioDriver* driver = static_cast<SC_WebAudioDriver*>(userData);
    return driver->processBlock(numInputs, inputs, numOutputs, outputs);
}

bool SC_WebAudioDriver::processBlock(int numInputs, const AudioSampleFrame* inputs, int numOutputs,
                                     AudioSampleFrame* outputs) {
    World* world = mWorld;

    // see SC_PortAudioDriver
    mFromEngine.Free();
    mToEngine.Perform();
    mOscPacketsToEngine.Perform();

    const int numSamples = mNumSamplesPerCallback;
    const int bufFrames = world->mBufLength;
    const int numBufs = numSamples / bufFrames;

    float* inBuses = world->mAudioBus + world->mNumOutputs * bufFrames;
    const float* outBuses = world->mAudioBus;
    int32* inTouched = world->mAudioBusTouched + world->mNumOutputs;
    const int32* outTouched = world->mAudioBusTouched;

    // Initialize mOSCbuftime once from wall-clock, then let sample-counting
    // advance it each callback (like CoreAudio/PortAudio/Jack drivers).
    // We can't call oscTimeNow() every callback because emscripten_get_now()
    // may not work reliably inside the AudioWorklet context.
    if (!mInitializedOSCTime) {
        mOSCbuftime = oscTimeNow();
        mInitializedOSCTime = true;
    }
    int64 oscTime = mOSCbuftime;
    const int64 oscInc = mOSCincrement;

    int bufFramePos = 0;
    for (int i = 0; i < numBufs; i++, world->mBufCounter++, bufFramePos += bufFrames) {
        const int32 bufCounter = world->mBufCounter;

        // copy inputs
        for (int ch = 0; ch < world->mNumInputs && ch < inputs[0].numberOfChannels; ch++) {
            const float* src = inputs[0].data + ch * WASM_QUANTUM_SIZE + bufFramePos;
            float* dst = inBuses + ch * bufFrames;
            memcpy(dst, src, bufFrames * sizeof(float));
            inTouched[ch] = bufCounter;
        }

        // scheduling
        const int64 nextTime = oscTime + oscInc;
        while (mScheduler.NextTime() <= nextTime) {
            SC_ScheduledEvent event = mScheduler.Remove();
            event.Perform();
        }
        world->mSampleOffset = 0;
        world->mSubsampleOffset = 0.f;

        // run the world calculation
        World_Run(world);

        // copy outputs - outputs[0] is a single stereo bus (numberOfOutputs=1, outputChannelCounts=[2])
        for (int ch = 0; ch < mWorld->mNumOutputs && ch < outputs[0].numberOfChannels; ch++) {
            float* dst = outputs[0].data + ch * WASM_QUANTUM_SIZE + bufFramePos;
            if (outTouched[ch] == bufCounter) {
                memcpy(dst, outBuses + ch * bufFrames, bufFrames * sizeof(float));
            } else {
                memset(dst, 0, bufFrames * sizeof(float));
            }
        }

        oscTime = mOSCbuftime = nextTime;
    }

    // this wakes up the NRT pthread!
    mAudioSync.Signal();

    // true = keep processing
    return true;
}

bool SC_WebAudioDriver::sendOsc(const char* data, const int size) {
    auto ptr = malloc(sizeof(OSC_Packet));
    if (!ptr) {
        scprintf("Could not process OSC message, out of wasm memory\n");
        return false;
    }
    auto packet = static_cast<OSC_Packet*>(ptr);

    // this additional copy can be removed once this has been refactored to use the
    // `World_SendPacket` API, see below
    auto rawBuffer = static_cast<char*>(malloc(size * sizeof(char)));
    memcpy(rawBuffer, data, size);

    packet->mData = rawBuffer;
    packet->mSize = size;
    packet->mIsBundle = data[0] == 35; // maybe too naive?
    packet->mReplyAddr.mReplyFunc = jsReplyFunc;
    packet->mReplyAddr.mReplyData = nullptr;
    packet->mReplyAddr.mPort = 57120;
    packet->mReplyAddr.mProtocol = kUDP;
    // use invalid socket and port
    packet->mReplyAddr.mSocket = -1;
    packet->mReplyAddr.mPort = 0;

    // @todo This implementation can not handle nested bundles.
    // use `World_SendPacket` API which is part of `SC_ComPort.cpp`,
    // but it does not require the network stack, so it could be
    // separated from any network implementation.
    if (!ProcessOSCPacket(mWorld, packet)) {
        free(rawBuffer);
        free(packet);
    }
    return true;
}


bool SC_WebAudioDriver::DriverStart() {
    int outputChannelCounts[1] = { static_cast<int>(mWorld->mNumOutputs) };
    EmscriptenAudioWorkletNodeCreateOptions options = {};
    // an audio worklet input can have multiple channels
    options.numberOfInputs = mWorld->mNumInputs > 0 ? 1 : 0;
    options.numberOfOutputs = 1;
    options.outputChannelCounts = outputChannelCounts;

    gScsynthWorkletNode =
        emscripten_create_wasm_audio_worklet_node(gAudioContext, "scsynth", &options, scsynthProcessCallback, this);

    emscripten_audio_node_connect(gScsynthWorkletNode, gAudioContext, 0, 0);

    return true;
}

/*
 *  Fake implementations
 */

// fill SC_ReplyImpl.hpp implementation holes
void null_reply_func(struct ReplyAddress* addr, char* msg, int size) {}
bool operator==(const ReplyAddress& a, const ReplyAddress& b) { return true; }
bool operator<(const ReplyAddress& a, const ReplyAddress& b) { return true; }

// Fill ComPort implementation holes
namespace scsynth {
void startAsioThread() {}
void stopAsioThread() {}
bool asioThreadStarted() { return true; }
}


// required by CoreAudio
void initializeScheduler() {
    // Compute offset to convert emscripten_get_now() to absolute NTP-epoch OSC time,
    // matching sclang's syncOSCOffsetWithTimeOfDay() so both share the same time base.
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64 todInOSC = ((int64)(tv.tv_sec + kSECONDS_FROM_1900_to_1970) << 32) + (int64)(tv.tv_usec * kMicrosToOSCunits);
    double nowSecs = emscripten_get_now() * 0.001;
    int64 nowInOSC = (int64)(nowSecs * kSecondsToOSCunits);
    gOSCoffset = todInOSC - nowInOSC;
}

extern "C" {
// CoreAudio implementations
int64 oscTimeNow() {
    double nowSecs = emscripten_get_now() * 0.001;
    return (int64)(nowSecs * kSecondsToOSCunits) + gOSCoffset;
}

int32 server_timeseed() { return static_cast<int32>(emscripten_get_now() * 1000.0); }

// SC_ComPort implementation
int World_OpenUDP(World*, const char*, int) { return 1; }
int World_OpenTCP(World*, const char*, int, int, int) { return 1; }
}

/*
 *  JS glue code
 */


/**
 * @brief OSC replies from the server to JavaScript go through this function.
 * The caller is responsible for freeing the passed message.
 * A copy is made b/c the function is async and the object could be out-of-scope already by then.
 */
static void jsReplyFunc(ReplyAddress*, char* msg, int size) {
    // offset within sc heap
    char* copy = static_cast<char*>(malloc(size));
    if (!copy)
        return;
    memcpy(copy, msg, size);
    MAIN_THREAD_ASYNC_EM_ASM(
        {
            // create a view into SC C++ heap
            const heap = new Uint8Array(Module.scHeap);
            // copy the content of `copy` from the C++ heap to the Uint8Array
            Module['onOscReply'](heap.slice($0, $0 + $1));
            Module['_free']($0);
        },
        copy, size);
}

/** @brief This ring buffer is used for capturing the printing from scsynth and
 *  pass it to the NRT JS main thread.
 *  getNextLine is polled from JS side (using setInterval).
 *
 *  Since we need a non-blocking ring buffer which stores consecutive data of arbitrary length (and therefore needs
 *  to wrap) and the actual length of the message is not known prior, we roll our own ring buffer here.
 *  The atomics have been suggested and approved by spacechild1.
 */
class PrintBuffer {
public:
    PrintBuffer(uint32_t size): mBufSize(size), mBuffer(new char[size]) {}

    /** @brief pushes a message into ring buffer.
     *  If no place left, the message will be discarded.
     */
    int push(const char* fmt, va_list vargs) {
        std::array<char, 4096> scratchpad;
        const auto printSize = static_cast<uint32_t>(vsnprintf(scratchpad.data(), scratchpad.size(), fmt, vargs));

        // avoid buffer corruption when RT and NRT thread write into the buffer at the same time!
        std::lock_guard lock(mWriteMutex);

        const uint32_t writePos = mWriteHead.load(std::memory_order_relaxed);
        const uint32_t readPos = mReadHead.load(std::memory_order_acquire);

        // one slot needs to be empty to distinguish empty and full
        const uint32_t freeBytes =
            (readPos > writePos) ? (readPos - writePos - 1) : (mBufSize - writePos + readPos - 1);
        if (printSize > freeBytes) {
            // not enough space — drop the whole message
            return 0;
        }

        // memcopy w/ optional wrap
        const uint32_t remBytesToEnd = mBufSize - writePos;
        if (printSize <= remBytesToEnd) {
            memcpy(mBuffer.get() + writePos, &scratchpad, printSize);
        } else {
            memcpy(mBuffer.get() + writePos, &scratchpad, remBytesToEnd);
            memcpy(mBuffer.get(), &scratchpad + remBytesToEnd, printSize - remBytesToEnd);
        }
        mWriteHead.store((writePos + printSize) % mBufSize, std::memory_order_release);
        return printSize;
    }

    /** @brief Returns the next complete line (up to but not including '\n') or none value. */
    std::optional<std::string> getNextLine() {
        const uint32_t readPos = mReadHead.load(std::memory_order_relaxed);
        const uint32_t writePos = mWriteHead.load(std::memory_order_acquire);

        if (readPos == writePos)
            return {};

        const uint32_t remainingBytes = (writePos >= readPos) ? (writePos - readPos) : (mBufSize - readPos + writePos);

        // scan for '\n'
        uint32_t lineLength = 0;
        for (; lineLength < remainingBytes; ++lineLength) {
            if (mBuffer[(readPos + lineLength) % mBufSize] == '\n')
                break;
        }
        if (lineLength == remainingBytes) {
            // no complete line yet
            return {};
        }
        std::string result;
        result.resize(lineLength);
        for (uint32_t j = 0; j < lineLength; ++j)
            // @todo use memcpy instead of bytewise copy, see push method
            result[j] = mBuffer[(readPos + j) % mBufSize];

        // we explicitly need to skip the \n char when updating the read head
        mReadHead.store((readPos + lineLength + 1) % mBufSize, std::memory_order_release);

        return result;
    }

private:
    uint32_t mBufSize;
    // @todo use std::array<char, PRINT_BUFFER_SIZE> mBuffer as optimization
    // see https://github.com/supercollider/supercollider/pull/7428#discussion_r3348498248
    std::unique_ptr<char[]> mBuffer;
    std::atomic<uint32_t> mReadHead { 0 };
    std::atomic<uint32_t> mWriteHead { 0 };
    // protect writing via mutex lock b/c this can be called from NRT and RT thread.
    // mutex is normally not "allowed" in RT thread, but scsynth is doing the same
    // by relying on stdout. additionally, while the RT thread can write some (e.g. poll),
    // the NRT thread only gets called rarely when e.g. a malloc failed.
    std::mutex mWriteMutex;
};

static PrintBuffer gPrintBuffer(PRINT_BUFFER_SIZE);

static int wasmPrintFunc(const char* fmt, va_list vargs) { return gPrintBuffer.push(fmt, vargs); }

static emscripten::val nextLine() {
    auto line = gPrintBuffer.getNextLine();
    if (line) {
        return emscripten::val(std::move(*line));
    }
    return emscripten::val::null();
}

// emscripten scsynth setup code
// this is 3 staged

/** @brief stage 3 - creates the worklet */
static void onProcessorCreated(EMSCRIPTEN_WEBAUDIO_T context, EM_BOOL success, void* userData) {
    if (!success) {
        printf("AudioProcessor init failed\n");
        return;
    }
    gAudioContext = context;

    SetPrintFunc(wasmPrintFunc);

    const auto userOptions = static_cast<ScWebAudioOptions*>(userData);
    auto options = toScWorld(userOptions);
    // got allocated in `bootWithOptions`
    delete userOptions;

    // we currently do not have a way to shut down the server manually, so any kind of world shut down and
    // freeing its resources is simply performed by closing the tab which will close the sandbox.
    gWorld = World_New(&options);
    if (!gWorld) {
        printf("Failed to create scsynth world\n");
        return;
    }

    gScWebAudioDriver = static_cast<SC_WebAudioDriver*>(gWorld->hw->mAudioDriver);
}

/** @brief stage 2 - worklet has been created - use it to register the audio worklet processor */
static void audioWorkletReady(EMSCRIPTEN_WEBAUDIO_T context, EM_BOOL success, void* userData) {
    if (!success) {
        printf("AudioWorklet init failed\n");
        return;
    }

    WebAudioWorkletProcessorCreateOptions options = {
        .name = "scsynth",
    };

    emscripten_create_wasm_audio_worklet_processor_async(context, &options, onProcessorCreated, userData);
}

/**
 * @brief Stage 1 - create audio context on c++ side and pass this context to audioWorkletReady.
 * This is the entrypointexposed to JS, see below for glue code
 */
void scsynthInit(ScWebAudioOptions* options) {
    const EmscriptenWebAudioCreateAttributes attributes = {
        .latencyHint = "interactive",
        .sampleRate = 48000,
    };

    EMSCRIPTEN_WEBAUDIO_T context = emscripten_create_audio_context(&attributes);

    emscripten_start_wasm_audio_worklet_thread_async(context, gAudioWorkletStack, sizeof(gAudioWorkletStack),
                                                     audioWorkletReady, options);
}

EMSCRIPTEN_BINDINGS(scWebAudio) {
    emscripten::function("bootWithOptions", emscripten::optional_override([](ScWebAudioOptions options) {
                             // gets freed in `onProcessorCreated`
                             auto copiedOptions = new ScWebAudioOptions(options);
                             scsynthInit(copiedOptions);
                         }));
    emscripten::function("nextLine", nextLine);

    /** @brief function to send raw osc messages to the server.
     *
     * The passed value must be a Uint8Array w/ the binary content of an OSC message.
     * The passed data is owned by JS heap, so we have to copy it to our side and create a copy.
     */
    emscripten::function("sendOsc", emscripten::optional_override([](const emscripten::val& uint8array) {
                             if (!uint8array.instanceof (emscripten::val::global("Uint8Array"))) {
                                 auto typeName = uint8array["constructor"]["name"].as<std::string>();
                                 printf("passed osc message needs to be an Uint8Array, but received %s\n",
                                        typeName.c_str());
                                 return;
                             }
                             if (gScWebAudioDriver == nullptr) {
                                 printf("scsynthSendOsc: server not ready\n");
                                 return;
                             }
                             auto length = uint8array["length"].as<size_t>();
                             auto buffer = std::make_unique<char[]>(length);
                             // memory view maps the heap linearly so it can be treated like a ptr from within JS land
                             emscripten::val memoryView(emscripten::typed_memory_view(length, buffer.get()));
                             // copy the user provided bytes to our c++/wasm heap
                             memoryView.call<void>("set", uint8array);
                             gScWebAudioDriver->sendOsc(buffer.get(), length);
                         }));

    /** @brief debug function to check scsynth state */
    emscripten::function("scsynthIsReady", emscripten::optional_override([] { return gScWebAudioDriver != nullptr; }));

    /** @brief helper function for connecting a mic into the scsynth worklet */
    emscripten::function("getAudioContext", emscripten::optional_override(([]() { return gAudioContext; })));

    /** @brief helper function for connecting a mic into the scsynth worklet */
    emscripten::function("getScsynthWorkletNode",
                         emscripten::optional_override(([]() { return gScsynthWorkletNode; })));


    emscripten::class_<ScWebAudioOptions>("Options")
        .constructor<>()
        .property("numBuffers", &ScWebAudioOptions::numBuffers)
        .property("maxNodes", &ScWebAudioOptions::maxNodes)
        .property("maxGraphDefs", &ScWebAudioOptions::maxGraphDefs)
        .property("maxWireBufs", &ScWebAudioOptions::maxWireBufs)
        .property("numAudioBusChannels", &ScWebAudioOptions::numAudioBusChannels)
        .property("numInputBusChannels", &ScWebAudioOptions::numInputBusChannels)
        .property("numOutputBusChannels", &ScWebAudioOptions::numOutputBusChannels)
        .property("numControlBusChannels", &ScWebAudioOptions::numControlBusChannels)
        .property("bufLength", &ScWebAudioOptions::bufLength)
        .property("realTimeMemorySize", &ScWebAudioOptions::realTimeMemorySize)
        .property("verbosity", &ScWebAudioOptions::verbosity);
};
