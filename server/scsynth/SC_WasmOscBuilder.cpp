#ifdef __EMSCRIPTEN__

#    include "osc/OscOutboundPacketStream.h"
#    include "osc/OscReceivedElements.h"
#    include <string>
#    include <emscripten.h>
#    include <emscripten/bind.h>

constexpr int OUTPUT_BUFFER_SIZE = 1024;

/**
 * A class to construct OSC messages via wasm so that
 * we don't need another JavaScript library to construct
 * OSC messages.
 *
 * This mimics the osc-pack API but as it relies on operator
 * overloading we have to provide some glue code as embind/JavaScript
 * can not overload functions and operators.
 */
class OscMessageBuilder {
public:
    OscMessageBuilder(): mStream(buffer, OUTPUT_BUFFER_SIZE) {}

    OscMessageBuilder& beginMessage(std::string address) {
        mStream << osc::BeginMessage(address.c_str());
        return *this;
    }

    OscMessageBuilder& endMessage() {
        mStream << osc::EndMessage;
        return *this;
    }

    OscMessageBuilder& beginBundle() {
        mStream << osc::BeginBundle;
        return *this;
    }

    OscMessageBuilder& endBundle() {
        mStream << osc::EndBundle;
        return *this;
    }

    OscMessageBuilder& addBlob(const emscripten::val& uint8Array) {
        unsigned int length = uint8Array["length"].as<unsigned int>();

        // first transfer the byte array into a vec<byte>
        std::vector<uint8_t> byteArray;
        byteArray.resize(length);
        auto memory = emscripten::val::module_property("HEAPU8")["buffer"];
        auto memoryView = uint8Array["constructor"].new_(memory, reinterpret_cast<uintptr_t>(byteArray.data()), length);
        memoryView.call<void>("set", uint8Array);
        char* mBufPtr = reinterpret_cast<char*>(byteArray.data());

        struct osc::Blob blob = osc::Blob(mBufPtr, length);

        mStream << blob;

        return *this;
    }

    OscMessageBuilder& addInt(int value) {
        mStream << osc::int32(value);
        return *this;
    }

    OscMessageBuilder& addFloat(float value) {
        mStream << value;
        return *this;
    }

    OscMessageBuilder& addString(const std::string& value) {
        mStream << value.c_str();
        return *this;
    }

    emscripten::val getData() { return emscripten::val(emscripten::typed_memory_view(mStream.Size(), mStream.Data())); }

private:
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream mStream;
};

#endif
