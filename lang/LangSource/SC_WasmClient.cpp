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

#include "SC_WasmClient.h"

#include <iostream>
#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/threading.h>

#include "PyrKernel.h"
#include "PyrPrimitive.h"
#include "PyrSched.h"
#include "SC_ComPort.h"
#include "SC_World.h"
#include "VMGlobals.h"
#include "SC_Msg.h"

enum class InterpreterStatus {
    Idle,
    Booting,
    CompilationFailed,
    Running,
};

// language does run in its own thread
static pthread_t gSclangWasmWorkerThread;

// interpreter status are written from the sclang thread, but needs to be available from js side
// so we need to use a mutex to avoid memory scramble ;)
static std::mutex gInterpreterStatusMutex;
static InterpreterStatus gInterpreterStatus = InterpreterStatus::Idle;

// forward declaration
static void wasmTimeout(void* userData);
static void wasmTick();

/**
 * Runs JS code in a safe manner which is necessary b/c otherwise it may
 * corrupt our app.
 * Returns true iff the user provided code did not throw.
 */
EM_JS(bool, runJsCodeWithTryCatch, (const char* rawCode, const int size), {
    var jsCode = UTF8ToString(rawCode, size);
    try {
        eval(jsCode);
        return true;
    } catch (e) {
        // clang-format off
        console.warn(`JS.runCode : exception on ${jsCode} : ${e}`);
        // clang-format on
        return false;
    };
});

/** @brief a sclang primitive to run code in the js main thread */
static int prRunJsCode(VMGlobals* g, int numArgsPushed) {
    if (!emscripten_is_main_runtime_thread()) {
        error("JS.runCode needs to run in the main browser thread - use .defer");
        return errFailed;
    }
    const bool isString = g->sp->getObjectHdr() && g->sp->getClass() == class_string;
    if (!isString) {
        return errWrongType;
    }
    const auto code = g->sp->getPyrObjType<PyrString>();
    const auto success = runJsCodeWithTryCatch(code->s, code->size);
    if (!success) {
        error("JS.runCode ran into an exception - see console for more information");
    }
    return success ? errNone : errFailed;
}

/** @brief sends a raw OSC byte array to the IDE. This will not be forwarded to the server.  */
static int prIdeSend(VMGlobals* g, int numArgsPushed) {
    if (!isKindOfSlot(g->sp, class_int8array)) {
        return errWrongType;
    }
    const auto* array = slotRawInt8Array(g->sp);
    const int size = array->size;
    auto* copy = static_cast<char*>(malloc(size));
    std::memcpy(copy, array->b, size);
    MAIN_THREAD_ASYNC_EM_ASM(
        {
            try {
                if (Module['onIdeSend']) {
                    Module['onIdeSend'](HEAPU8.slice($0, $0 + $1));
                }
            } finally { Module['_free']($0); }
        },
        copy, size);
    return errNone;
}

/** @brief responds to _AppClock_SchedNotify primitive which notifies
 *  the browser main thread that something was scheduled.
 */
static int prAppClockSchedNotify(VMGlobals* g, int numArgsPushed) {
    emscripten_dispatch_to_thread_async(emscripten_main_runtime_thread_id(), EM_FUNC_SIG_V, &wasmTick, nullptr);
    return errNone;
}

void SC_WasmClient::onLibraryStartup() {
    SC_LanguageClient::onLibraryStartup();
    int index = 0;
    const int base = nextPrimitiveIndex();
    definePrimitive(base, index++, "_Wasm_runCode", prRunJsCode, 2, 0);
    definePrimitive(base, index++, "_Wasm_ideSend", prIdeSend, 2, 0);
    definePrimitive(base, index++, "_AppClock_SchedNotify", prAppClockSchedNotify, 1, 0);
}

void SC_WasmClient::runCode(const std::string& code, const bool silent) {
    setCmdLine(code.c_str());
    runLibrary(silent ? s_interpretCmdLine : s_interpretPrintCmdLine);
}

void SC_WasmClient::scheduleTick(double delayMs) {
    // std::chrono::high_resolution_clock is nano seconds in emscripten
    const double deadline = elapsedTime() + delayMs * 0.001;
    // there is already a timeout which will invoke us earlier - so bail out
    if (mTickTimeoutId != 0 && mTickDeadline <= deadline) {
        return;
    }
    if (mTickTimeoutId != 0) {
        // we can clear the existing timer since we will create a new one
        // which will wake us up sooner than the existing one
        emscripten_clear_timeout(mTickTimeoutId);
    }
    mTickTimeoutId = emscripten_set_timeout(wasmTimeout, delayMs, nullptr);
    mTickDeadline = deadline;
}

void SC_WasmClient::tickTimeout() {
    mTickTimeoutId = 0;
    tick();
}

void SC_WasmClient::tick() {
    if (!trylock()) {
        // currently locked - instead of blocking the main thread, we will defer to the next round
        // "Thou shalt not block the browser main thread in wasm" ->
        // https://emscripten.org/docs/porting/pthreads.html#blocking-on-the-main-browser-thread
        scheduleTick(1.0);
        return;
    }
    double secs;
    const bool haveNext = tickLocked(&secs);
    unlock();

    if (haveNext) {
        const double now = elapsedTime();
        double delayMs = (secs - now) * 1000.0;
        // lower bound clip to give others room to breath
        if (delayMs < 1.0) {
            delayMs = 1.0;
        }
        if (!isnan(delayMs) && !isinf(delayMs)) {
            scheduleTick(delayMs);
        } else {
            std::cout << "Invalid delayMs value for AppClock ticker: " << delayMs << std::endl;
        }
    }
}

/**
 * C function which gets invoked by a JS timeout on the main thread.
 * Forwards it to the wasm language client.
 *
 * @param userData \c nullptr, required by emscripten call signature
 */
void wasmTimeout(void* userData) {
    const auto client = static_cast<SC_WasmClient*>(SC_WasmClient::instance());
    // this can never be null b/c we only get called from within a primitive
    // or at client init
    assert(client != nullptr);
    client->tickTimeout();
}

/**
 * Gets called by \c prAppClockSchedNotify to inform about new items in the AppClock scheduler queue.
 * Contrary to \c wasmTimeout , this does not reset an existing timer.
 *
 * Runs on the main thread of the browser.
 */
static void wasmTick() {
    const auto client = static_cast<SC_WasmClient*>(SC_WasmClient::instance());
    // this can never be null b/c we only get called from within a primitive
    // or at client init
    assert(client != nullptr);
    client->tick();
}

void SC_WasmClient::postText(const char* str, size_t len) { std::cout.write(str, len); }

void SC_WasmClient::postFlush(const char* str, size_t len) {
    std::cout.write(str, len);
    std::cout.flush();
}

void SC_WasmClient::postError(const char* str, size_t len) {
    std::cout << "error";
    std::cout.write(str, len);
    std::cout << std::endl;
}

void SC_WasmClient::flush() { std::cout << std::endl; }

/** @brief Called as entry point by the dedicated gSclangWasmThread, which will boot the interpreter.
 */
static void* wasmWorkerThreadFunction(void* args) {
    SC_WasmClient client("sclang");
    std::cout << "Welcome to sclang.wasm!" << std::endl;
    SC_LanguageClient::Options options;
    client.initRuntime(options);
    const auto compileSuccess = compileLibrary(false, false);
    {
        std::lock_guard lock(gInterpreterStatusMutex);
        if (!compileSuccess) {
            gInterpreterStatus = InterpreterStatus::CompilationFailed;
            std::cout << "ERROR: Library has not been compiled successfully." << std::endl;
            return nullptr;
        }
        gInterpreterStatus = InterpreterStatus::Running;
    }
    // this does not block
    client.runMain();
    // this not just keeps the owned resources "alive", but also
    // keeps the worker thread alive such that the JS runtime can
    // process events like passing incoming OSC messages or runCode.
    emscripten_exit_with_live_runtime();
};

/**
 * @brief Evaluates code in the sclang interpreter.
 * This should only be called from the \c gSclangWasmThread.
 *
 * @warning Check if the interpreter is running before calling this function.
 *
 * @param arg the char* gets freed after execution
 * @param silent determines if the return value will be print to stdout
 */
static void executeCode(void* arg, const bool silent) {
    auto* code = static_cast<char*>(arg);

    const auto client = static_cast<SC_WasmClient*>(SC_WasmClient::instance());
    // client can not be null here b/c we only set interpreter running
    // when the client was created
    assert(client != nullptr);
    client->runCode(code, silent);

    free(code);
}


static void runCodeOnSclangThread(const std::string& code, const bool silent = false) {
    {
        std::lock_guard lock(gInterpreterStatusMutex);
        if (gInterpreterStatus != InterpreterStatus::Running) {
            std::cout << "sclang client not running!" << std::endl;
            return;
        }
    }
    char* codeCopy = strdup(code.c_str());
    emscripten_dispatch_to_thread_async(gSclangWasmWorkerThread, EM_FUNC_SIG_VII, executeCode, nullptr, codeCopy,
                                        silent);
}

// acts as overload - emscripten does not support default arguments, so we provide an indirection here
static void runCodeSclangThreadLoud(const std::string& code) { runCodeOnSclangThread(code, false); }

void ProcessOSCPacket(std::unique_ptr<OSC_Packet> inPacket, int inPortNum, double time);

/**
 * @param arg must point to an OSC_Packet* - also takes ownership. */
static void runOscMessage(void* arg) {
    auto* packet = static_cast<OSC_Packet*>(arg);
    ProcessOSCPacket(std::unique_ptr<OSC_Packet>(packet), 57120, elapsedTime());
}

static void passOscMessageToSclangThread(const emscripten::val& data) {
    {
        std::lock_guard lock(gInterpreterStatusMutex);
        if (gInterpreterStatus != InterpreterStatus::Running) {
            std::cout << "sclang client not running!" << std::endl;
            return;
        }
    }

    if (!data.instanceof (emscripten::val::global("Uint8Array"))) {
        std::cerr << "passed osc message needs to be an Uint8Array, but received " << data.typeOf().as<std::string>()
                  << std::endl;
        return;
    }

    const auto length = data["length"].as<size_t>();

    // data is not leaking b/c ownership gets passed to the sclang thread
    const auto packet = new OSC_Packet();

    packet->mData = std::make_unique<char[]>(length);
    // memory view maps the heap linearly so it can be treated like a ptr from within JS land
    const emscripten::val memoryView(emscripten::typed_memory_view(length, packet->mData.get()));
    // copy the user provided bytes to our c++/wasm heap
    memoryView.call<void>("set", data);

    packet->mSize = length;
    packet->mReplyAddr.mPort = 57110;
    packet->mReplyAddr.mProtocol = kUDP;
    packet->mReplyAddr.mReplyData = nullptr;
    packet->mReplyAddr.mSocket = 12345;

    emscripten_dispatch_to_thread_async(gSclangWasmWorkerThread, EM_FUNC_SIG_VI, runOscMessage, nullptr, packet);
}

// patches
void startAsioThread() {}
void stopAsioThread() {}
void initSerialPrimitives() {}

InPort::UDP::UDP(int inPortNum, HandlerType, int portsToCheck) {}
InPort::UDPCustom::UDPCustom(int inPortNum, HandlerType handlerType): UDP(inPortNum, handlerType, 1) {}
OutPort::TCP::TCP(std::uint64_t inAddress, int inPort, HandlerType, ClientNotifyFunc notifyFunc, void* clientData) {}
int OutPort::TCP::Close() { return errNone; }

SCSYNTH_DLLEXPORT_C bool World_SendPacket(World* inWorld, int inSize, char* inData, ReplyFunc inFunc) { return true; }
SCSYNTH_DLLEXPORT_C bool World_SendPacketWithContext(World* inWorld, int inSize, char* inData, ReplyFunc inFunc,
                                                     void* inContext) {
    return true;
}

// OSCData.cpp / network patches

/** @brief Module.onOsc needs to be called on the main JS thread.
 *  This helper gets passed a raw pointer to an OSC message,
 *  which then invokes onOsc using this main JS thread.
 *  The passed message will be freed.
 *
 *  See https://emscripten.org/docs/porting/pthreads.html#proxying
 *  and https://emscripten.org/docs/api_reference/proxying.h.html
 */
int netAddrSend(PyrObject* netAddrObj, int msglen, char* bufptr, bool sendMsgLen) {
    // prepend size of the message
    auto* rawMessage = static_cast<char*>(malloc(msglen));
    if (!rawMessage) {
        post("NetAddr: Could not allocate memory for OSC message");
        return errFailed;
    }
    std::memcpy(rawMessage, bufptr, msglen);
    // pass message to main thread
    MAIN_THREAD_ASYNC_EM_ASM(
        {
            try {
                if (Module.onOsc) {
                    var data = HEAPU8.slice($0, $0 + $1);
                    Module.onOsc(data);
                }
            } finally { Module['_free']($0); }
        },
        rawMessage, msglen);
    return errNone;
}

// js export

static void bootInterpreter() {
    {
        std::lock_guard lock(gInterpreterStatusMutex);
        if (gInterpreterStatus != InterpreterStatus::Idle) {
            std::cout << "sclang already booted" << std::endl;
            return;
        }
        gInterpreterStatus = InterpreterStatus::Booting;
    }
    pthread_create(&gSclangWasmWorkerThread, nullptr, wasmWorkerThreadFunction, nullptr);
}

EMSCRIPTEN_BINDINGS(sclangWasm) {
    emscripten::function("bootInterpreter", &bootInterpreter);
    // emscripten does not respect c++ default arguments
    // .runCode(code, silent)
    emscripten::function("runCode", &runCodeOnSclangThread);
    // .runCode(code) => .runCode(code, silent=false) in C++
    emscripten::function("runCode", &runCodeSclangThreadLoud);
    emscripten::function("sendOsc", &passOscMessageToSclangThread);
}

// export this to avoid dead code elimination
EMSCRIPTEN_KEEPALIVE extern "C" void scWasmBindingAnchor() {

};
