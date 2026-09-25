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

#include <string>
#include <emscripten.h>
#include <iostream>
#include <emscripten/bind.h>

#include "osc/OscOutboundPacketStream.h"
#include "osc/OscReceivedElements.h"
#include "osc/OscTypes.h"

constexpr int OUTPUT_BUFFER_SIZE = 8192;

/**
 * A class to construct OSC messages via wasm so that
 * we don't need another JavaScript library to construct
 * OSC messages.
 *
 * This mimics the osc-pack API but as it relies on operator
 * overloading we have to provide some glue code as embind/JavaScript
 * can not overload functions and operators.
 *
 * This is not a production-tested and feature-complete OSC implementation, but
 * allows to use scsynth without additional OSC libraries.
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
        // @todo add type checking...
        auto length = uint8Array["length"].as<unsigned int>();

        auto buf = std::make_unique<char[]>(length);
        emscripten::val memoryView(emscripten::typed_memory_view(length, buf.get()));
        memoryView.call<void>("set", uint8Array);

        mStream << osc::Blob(buf.get(), length);

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

    // copy the buffer into a JS-GC owned Uint8Array
    emscripten::val getData() {
        auto view = emscripten::val(emscripten::typed_memory_view(mStream.Size(), mStream.Data()));
        auto result = emscripten::val::global("Uint8Array").new_(view);
        return result;
    }

private:
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream mStream;
};

/**
 * Parse OSC arguments from a ReceivedMessage into a JS array.
 */
static emscripten::val parseOscArgs(const osc::ReceivedMessage& message) {
    auto arguments = emscripten::val::array();
    for (auto arg = message.ArgumentsBegin(); arg != message.ArgumentsEnd(); ++arg) {
        switch (auto typeTag = arg->TypeTag()) {
        case osc::INT32_TYPE_TAG:
            arguments.call<void>("push", emscripten::val(arg->AsInt32Unchecked()));
            break;
        case osc::FLOAT_TYPE_TAG:
            arguments.call<void>("push", emscripten::val(arg->AsFloatUnchecked()));
            break;
        case osc::STRING_TYPE_TAG:
            arguments.call<void>("push", emscripten::val(arg->AsStringUnchecked()));
            break;
        case osc::DOUBLE_TYPE_TAG:
            arguments.call<void>("push", emscripten::val(arg->AsDoubleUnchecked()));
            break;
        case osc::TRUE_TYPE_TAG | osc::FALSE_TYPE_TAG:
            arguments.call<void>("push", emscripten::val(arg->AsBoolUnchecked()));
            break;
        case osc::NIL_TYPE_TAG:
            arguments.call<void>("push", emscripten::val::null());
        case osc::BLOB_TYPE_TAG: {
            const void* blobData;
            osc::osc_bundle_element_size_t blobSize;
            arg->AsBlobUnchecked(blobData, blobSize);
            auto blobView = emscripten::typed_memory_view(blobSize, static_cast<const char*>(blobData));
            auto uint8Ctor = emscripten::val::global("Uint8Array");
            auto blob = uint8Ctor.new_(blobView);
            arguments.call<void>("push", blob);
            break;
        }
        default:
            std::cout << "unsupported OSC argument type '" << typeTag << "', using undefined" << std::endl;
            arguments.call<void>("push", emscripten::val::undefined());
            break;
        }
    }
    return arguments;
}

/**
 * A helper function to parse a JS uInt8Array representing an
 * OSC message or bundle (such as returned by scsynth through its JS callback)
 * into a JS object.
 *
 * For bundles: address is "#bundle", arguments is an array of ParsedOscMessage objects.
 * For messages: address is the OSC address, arguments is an array of argument values.
 */
emscripten::val parseOscBlobToJs(const emscripten::val& uint8Array) {
    if (!uint8Array.instanceof (emscripten::val::global("Uint8Array"))) {
        auto typeName = uint8Array["constructor"]["name"].as<std::string>();
        printf("passed osc message needs to be an Uint8Array, but received %s\n", typeName.c_str());
        return emscripten::val::null();
    }
    size_t length = uint8Array["length"].as<size_t>();

    // copy the content of the js-buffer into wasm territory through a memory view
    auto buf = std::make_unique<char[]>(length);
    emscripten::val memoryView(emscripten::typed_memory_view(length, buf.get()));
    memoryView.call<void>("set", uint8Array);

    osc::ReceivedPacket packet(buf.get(), length);

    if (!packet.IsBundle()) {
        osc::ReceivedMessage message(packet);
        auto object = emscripten::val::object();
        object.set("address", message.AddressPattern());
        object.set("arguments", parseOscArgs(message));
        return object;
    }

    osc::ReceivedBundle bundle(packet);

    auto parsedBundle = emscripten::val::object();
    parsedBundle.set("address", "#bundle");
    parsedBundle.set("arguments", emscripten::val::array());
    for (auto elem = bundle.ElementsBegin(); elem != bundle.ElementsEnd(); ++elem) {
        if (elem->IsMessage()) {
            osc::ReceivedMessage subMsg(*elem);
            auto subJsMessage = emscripten::val::object();
            subJsMessage.set("address", subMsg.AddressPattern());
            subJsMessage.set("arguments", parseOscArgs(subMsg));
            parsedBundle["arguments"].call<void>("push", subJsMessage);
        } else {
            // don't throw an error as this will not be catched by the wasm runtime, see
            // https://emscripten.org/docs/porting/exceptions.html
            std::cerr << "Nested OSC bundles are not supported - OSC message is incomplete." << std::endl;
        }
    }
    return parsedBundle;
}

EMSCRIPTEN_BINDINGS(OSC_Helper) {
    emscripten::class_<OscMessageBuilder>("OscMessage")
        .constructor<>()
        .function("beginMessage", &OscMessageBuilder::beginMessage, emscripten::return_value_policy::reference())
        .function("endMessage", &OscMessageBuilder::endMessage, emscripten::return_value_policy::reference())
        .function("beginBundle", &OscMessageBuilder::beginBundle, emscripten::return_value_policy::reference())
        .function("endBundle", &OscMessageBuilder::endBundle, emscripten::return_value_policy::reference())
        .function("addBlob", &OscMessageBuilder::addBlob, emscripten::return_value_policy::reference())
        .function("addInt", &OscMessageBuilder::addInt, emscripten::return_value_policy::reference())
        .function("addFloat", &OscMessageBuilder::addFloat, emscripten::return_value_policy::reference())
        .function("addString", &OscMessageBuilder::addString, emscripten::return_value_policy::reference())
        .function("getData", &OscMessageBuilder::getData, emscripten::return_value_policy::reference());

    emscripten::function("parseOscMessage", &parseOscBlobToJs, emscripten::allow_raw_pointers());
}

// Make bindings visible to the linker b/c otherwise dead code elimination kicks in.
// still need to be added to exported_functions,
// but by doing this our whole binding will be picked up.
extern "C" EMSCRIPTEN_KEEPALIVE void webOscBindingAnchor() {}
