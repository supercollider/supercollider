#ifdef __EMSCRIPTEN__

#    include "SC_World.h"
#    include "SC_ReplyImpl.hpp"
#    include "SC_ComPort.cpp"
#    include <emscripten.h>
#    include <iostream>
#    include <emscripten/bind.h>


namespace scsynth {

static void webReplyFunc(struct ReplyAddress* addr, char* msg, int size);

/**
 * We have to interact with the world via OSC, but as we are
 * not allowed to open a network socket in the browser context so we instead
 * setup a JavaScript binding to pass OSC messages to the world.
 */
class SC_WasmBinding {
public:
    SC_WasmBinding(struct World* world): mWorld(world) { SC_WasmBinding::current = this; };

    /**
     * this will be called from JavaScript with a byte array
     */
    bool receive(const emscripten::val& uint8Array) {
        unsigned int length = uint8Array["length"].as<unsigned int>();

        // first transfer the byte array into a vec<byte>
        std::vector<uint8_t> byteArray;
        byteArray.resize(length);
        auto memory = emscripten::val::module_property("HEAPU8")["buffer"];
        auto memoryView = uint8Array["constructor"].new_(memory, reinterpret_cast<uintptr_t>(byteArray.data()), length);
        memoryView.call<void>("set", uint8Array);
        // and then use its data pointer to pass it to world
        char* mBufPtr = reinterpret_cast<char*>(byteArray.data());

        OSC_Packet* packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));
        packet->mReplyAddr.mProtocol = kWeb;
        packet->mReplyAddr.mAddress = boost::asio::ip::make_address("127.0.0.1"); // not used
        packet->mReplyAddr.mPort = 57120; // necessary hardcoded value, not relevant for us
        packet->mReplyAddr.mSocket = 57110; // not used
        packet->mReplyAddr.mReplyFunc = webReplyFunc;
        packet->mReplyAddr.mReplyData = nullptr;

        bool ok = UnrollOSCPacket(mWorld, length, mBufPtr, packet);

        if (!ok) {
            free(packet);
        }

        return ok;
    }
    static SC_WasmBinding* current;

    /**
     * mWorld will call this when it needs to reply to an OSC message.
     * This method will pass the content of the OSC message to the wasm module.
     * If a callback is set within the module, we will call it with the OSC message.
     */
    static void replyReceiver(struct ReplyAddress* addr, char* msg, int size) {
        // clang-format off
        MAIN_THREAD_EM_ASM({
            if (typeof Module.oscReceiver == 'function') {
                let msgPointer = $0;
                let msgSize = $1;
                let data = new Uint8Array(Module.HEAPU8.buffer, msgPointer, msgSize);
                Module.oscReceiver(data);
            } else {
                console.log("Dropped OSC reply b/c oscReceiver method is not declared.");
            };
        }, msg, size);
        // clang-format on
    }

private:
    struct World* mWorld;
};

// we create a singleton of our SC_WasmBinding class through a global variable
// this will be initiated/set by the SC_WasmBinding constructor
// which will be called by scynth_main
SC_WasmBinding* SC_WasmBinding::current = nullptr;


// static function which will dispatch to the SC_WasmBinding instance
static void webReplyFunc(struct ReplyAddress* addr, char* msg, int size) {
    if (SC_WasmBinding::current == nullptr) {
        throw std::runtime_error("SC_WasmBinding has not been initiated!\n");
    }
    SC_WasmBinding::current->replyReceiver(addr, msg, size);
}

}

using namespace scsynth;

// exported function to make it available via javascript
bool sendOscMessage(const emscripten::val& uint8Array) {
    if (SC_WasmBinding::current == nullptr) {
        throw std::runtime_error("SC_WasmBinding has not been initiated!\n");
    }
    return SC_WasmBinding::current->receive(uint8Array);
}

// static exports to javascript via embind
EMSCRIPTEN_BINDINGS(Web_Audio) {
    emscripten::function("sendOscMessage", &sendOscMessage, emscripten::allow_raw_pointers());
}

#endif
