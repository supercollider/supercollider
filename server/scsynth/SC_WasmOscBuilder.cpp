#ifdef __EMSCRIPTEN__

#    include "osc/OscOutboundPacketStream.h"
#    include "osc/OscReceivedElements.h"
#    include <string>
#    include <emscripten.h>
#    include <emscripten/bind.h>

namespace scsynth {

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
        mStream << osc::BeginBundle();
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

/**
 * Will be translated to an JS object by embind
 */
struct ParsedOscMessage {
    std::string address;
    emscripten::val arguments = emscripten::val::array();
};

/**
 * A helper function to parse a JS uInt8Array representing an
 * OSC message (such as returned by scsynth through its JS callback)
 * nnto a JS object.
 */
ParsedOscMessage parseOscBlobToJs(const emscripten::val& uint8Array) {
    unsigned int length = uint8Array["length"].as<unsigned int>();

    // first transfer the byte array into a vec<byte>
    std::vector<uint8_t> byteArray;
    byteArray.resize(length);
    auto memory = emscripten::val::module_property("HEAPU8")["buffer"];
    auto memoryView = uint8Array["constructor"].new_(memory, reinterpret_cast<uintptr_t>(byteArray.data()), length);
    memoryView.call<void>("set", uint8Array);
    // and then use its data pointer to pass it to world
    char* mBufPtr = reinterpret_cast<char*>(byteArray.data());

    const auto packet = osc::ReceivedPacket(mBufPtr, (size_t)length);
    auto message = osc::ReceivedMessage(packet);

    ParsedOscMessage parsedMessage {};
    parsedMessage.address = message.AddressPattern();

    for (auto arg = message.ArgumentsBegin(); arg != message.ArgumentsEnd(); ++arg) {
        // attention here - if the code accessing JavaScript is wrong here
        // this will not be a compile error but a runtime error!
        if (arg->IsInt32()) {
            parsedMessage.arguments.call<void>("push", emscripten::val(arg->AsInt32Unchecked()));
        } else if (arg->IsFloat()) {
            parsedMessage.arguments.call<void>("push", emscripten::val(arg->AsFloatUnchecked()));
        } else if (arg->IsString()) {
            parsedMessage.arguments.call<void>("push", emscripten::val(arg->AsStringUnchecked()));
        } else {
            throw std::runtime_error("Unsupported argument type in OSC message.");
        }
    }

    return parsedMessage;
}
}

using namespace scsynth;

EMSCRIPTEN_BINDINGS(OSC_Helper) {
    emscripten::class_<OscMessageBuilder>("OscMessage")
        .constructor<>()
        .function("beginMessage", &OscMessageBuilder::beginMessage)
        .function("endMessage", &OscMessageBuilder::endMessage)
        .function("beginBundle", &OscMessageBuilder::beginBundle)
        .function("endBundle", &OscMessageBuilder::endBundle)
        .function("addBlob", &OscMessageBuilder::addBlob)
        .function("addInt", &OscMessageBuilder::addInt)
        .function("addFloat", &OscMessageBuilder::addFloat)
        .function("addString", &OscMessageBuilder::addString)
        .function("getData", &OscMessageBuilder::getData, emscripten::allow_raw_pointers());

    emscripten::value_object<ParsedOscMessage>("ParsedOscMessage")
        .field("address", &ParsedOscMessage::address)
        .field("arguments", &ParsedOscMessage::arguments);

    emscripten::function("parseOscMessage", &parseOscBlobToJs, emscripten::allow_raw_pointers());
}

// Make bindings visible to the linker - still need to be added to
// exported_functions, but by doing this our whole binding will be picked up.
extern "C" EMSCRIPTEN_KEEPALIVE void parseOscBlobToJs() {}

#endif
