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

// language does run in its own thread
static pthread_t gSclangWasmThread;

// forward declaration
void wasmTick(void* arg);

/** @brief runs JS code on the main browser thread where window/document exist
 *
 *  @param arg takes ownership by freeing the char* after execution.
 * **/
static void runJsOnMainThread(void* arg) {
    char* code = static_cast<char*>(arg);
    emscripten_run_script(code);
    free(code);
}

/** @brief a sclang primitive to run code in the js main thread */
static int prRunJsCode(struct VMGlobals* g, int numArgsPushed) {
    bool isString = g->sp->getObjectHdr() && g->sp->getClass() == class_string;
    if (!isString) {
        return errWrongType;
    }
    auto code = g->sp->getPyrObjType<PyrString>();

    // transfer ownership of the code copy to runJsOnMainThread to avoid lifetime issues
    emscripten_async_run_in_main_runtime_thread(EM_FUNC_SIG_VI, runJsOnMainThread, strndup(code->s, code->size));

    return errNone;
}

/** @brief sends a raw OSC byte array to the IDE. This will not be forwarded to the server.  */
static int prIdeSend(struct VMGlobals* g, int numArgsPushed) {
    if (!isKindOfSlot(g->sp, class_int8array)) {
        return errWrongType;
    }
    PyrInt8Array* array = slotRawInt8Array(g->sp);
    int size = array->size;
    char* copy = static_cast<char*>(malloc(size));
    std::memcpy(copy, array->b, size);
    MAIN_THREAD_ASYNC_EM_ASM(
        {
            if (Module['onIdeSend']) {
                Module['onIdeSend'](HEAPU8.slice($0, $0 + $1));
            }
            Module['_free']($0);
        },
        copy, size);
    return errNone;
}

void SC_WasmClient::onLibraryStartup() {
    SC_LanguageClient::onLibraryStartup();
    int index = 0;
    int base = nextPrimitiveIndex();
    definePrimitive(base, index++, "_Wasm_runCode", prRunJsCode, 2, 0);
    definePrimitive(base, index++, "_Wasm_ideSend", prIdeSend, 2, 0);
    definePrimitive(base, index++, "_AppClock_SchedNotify", primitiveTicker, 1, 0);
}

void SC_WasmClient::runCode(const std::string& code) {
    setCmdLine(code.c_str());
    runLibrary(s_interpretPrintCmdLine);
}

void SC_WasmClient::scheduleTick(double delayMs) {
    emscripten_clear_timeout(mTickTimeoutId);
    mTickTimeoutId = emscripten_set_timeout(wasmTick, delayMs, nullptr);
}

void SC_WasmClient::runCodeSilent(const std::string& code) {
    setCmdLine(code.c_str());
    runLibrary(s_interpretCmdLine);
}

int SC_WasmClient::run(int argc, char** argv) {
    auto options = Options();
    initRuntime(options);

    compileLibrary(false);

    if (!gCompiledOK) {
        post("ERROR: Library has not been compiled successfully.\n");
        return 1;
    }
    runMain();

    return 0;
}

void SC_WasmClient::ticker() {
    // the pending timeout that called us has timet out
    mTickTimeoutId = 0;
    double secs;
    lock();
    const bool haveNext = tickLocked(&secs);
    unlock();

    if (haveNext) {
        double now = elapsedTime();
        double delayMs = (secs - now) * 1000.0;
        // lower bound clip to avoid suffocation of JS thread(?)
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

int SC_WasmClient::primitiveTicker(VMGlobals* g, int numArgsPushed) {
    if (auto client = static_cast<SC_WasmClient*>(instance())) {
        // defer execution to js runtime
        client->scheduleTick(1.0);
    }
    return errNone;
};

void wasmTick(void*) {
    if (auto client = static_cast<SC_WasmClient*>(SC_WasmClient::instance())) {
        client->ticker();
    }
}

void SC_WasmClient::postText(const char* str, size_t len) {
    for (size_t i = 0; i < len; i++) {
        std::cout << str[i];
    }
}

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

static void* bootInterpreter(void* args) {
    auto client = new SC_WasmClient("sclang");
    std::cout << "Welcome to sclang.wasm!" << std::endl;
    auto options = SC_LanguageClient::Options();
    client->initRuntime(options);
    compileLibrary(false);

    if (!gCompiledOK) {
        std::cout << "ERROR: Library has not been compiled successfully." << std::endl;
        return nullptr;
    }
    client->runMain();
    emscripten_exit_with_live_runtime();
};

/**
 * @brief Evaluates code in the sclang interpreter.
 * This should only be called from the gSclangWasmThread in order to avoid deadlocks of the gLangMutex lock.
 *
 * @param arg the char* gets freed after execution
 */
void executeCode(void* arg) {
    char* code = static_cast<char*>(arg);
    if (auto client = static_cast<SC_WasmClient*>(SC_WasmClient::instance())) {
        client->runCode(code);
    }
    free(code);
}

/**
 * @brief Evalutes the code silently within the sclang interpreter.
 * @see executeCode()
 */
void executeCodeSilent(void* arg) {
    char* code = static_cast<char*>(arg);
    if (auto client = static_cast<SC_WasmClient*>(SC_WasmClient::instance())) {
        client->runCodeSilent(code);
    }
    free(code);
}

void runCodeOnSclangThread(const std::string& code) {
    char* codeCopy = strdup(code.c_str());
    emscripten_dispatch_to_thread_async(gSclangWasmThread, EM_FUNC_SIG_VI, executeCode, nullptr, codeCopy);
}

void runCodeSilentOnSclangThread(const std::string& code) {
    char* codeCopy = strdup(code.c_str());
    emscripten_dispatch_to_thread_async(gSclangWasmThread, EM_FUNC_SIG_VI, executeCodeSilent, nullptr, codeCopy);
}

void runCode(std::string code) {
    auto client = static_cast<SC_WasmClient*>(SC_WasmClient::instance());
    if (!client) {
        std::cout << "wasm client not initialized!" << std::endl;
        return;
    }
    client->runCode(code);
}


void ProcessOSCPacket(std::unique_ptr<OSC_Packet> inPacket, int inPortNum, double time);

/**
 * @param arg must point to an OSC_Packet* - also takes ownership. */
static void runOscMessage(void* arg) {
    auto* packet = static_cast<OSC_Packet*>(arg);
    ProcessOSCPacket(std::unique_ptr<OSC_Packet>(packet), 57120, elapsedTime());
}

void passOscMessageToSclangThread(std::string data) {
    if (!SC_WasmClient::instance()) {
        std::cout << "wasm client not initialized!" << std::endl;
        return;
    }
    // data contains raw OSC bytes (embind copies Uint8Array into std::string)
    // build the packet on this thread, copy the bytes, dispatch to sclang thread
    // data is not leaking b/c ownership gets passed to the sclang thread
    auto packet = new OSC_Packet();
    packet->mData = std::make_unique<char[]>(data.size());
    std::memcpy(packet->mData.get(), data.data(), data.size());
    packet->mSize = data.size();
    packet->mReplyAddr.mPort = 57110;
    packet->mReplyAddr.mProtocol = kUDP;
    packet->mReplyAddr.mReplyData = nullptr;
    packet->mReplyAddr.mSocket = 12345;

    emscripten_dispatch_to_thread_async(gSclangWasmThread, EM_FUNC_SIG_VI, runOscMessage, nullptr, packet);
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
    char* rawMessage = static_cast<char*>(malloc(msglen));
    std::memcpy(rawMessage, bufptr, msglen);
    // pass message to main thread
    MAIN_THREAD_ASYNC_EM_ASM(
        {
            if (Module.onOsc) {
                var data = HEAPU8.slice($0, $0 + $1);
                Module.onOsc(data);
                Module['_free']($0);
            }
        },
        rawMessage, msglen);
    return errNone;
}

// js export

void cBootInterpreter() {
    if (SC_WasmClient::instance() != nullptr) {
        std::cout << "sclang already running" << std::endl;
        return;
    }
    pthread_create(&gSclangWasmThread, nullptr, bootInterpreter, nullptr);
}

EMSCRIPTEN_BINDINGS(sclangWasm) {
    emscripten::function("bootInterpreter", &cBootInterpreter);
    emscripten::function("runCode", &runCodeOnSclangThread);
    emscripten::function("runCodeSilent", &runCodeSilentOnSclangThread);
    emscripten::function("sendOsc", &passOscMessageToSclangThread);
}

// export this to avoid dead code elimination
EMSCRIPTEN_KEEPALIVE extern "C" void scWasmBindingAnchor() {

};
