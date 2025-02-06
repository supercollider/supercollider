#include "boost/asio.hpp"

#include <PyrPrimitive.h>
#include <PyrKernel.h>
#include <SC_Lock.h>
#include <PyrSched.h>
#include <PyrInterpreter.h>
#include "SC_WorldOptions.h"
#include "SC_WebSocketPrim.h"

#include <PyrSymbolTable.h>

#include "SC_WebSocket.h"

using namespace SC_Websocket_Lang;

PyrObject* SC_Websocket_Lang::convert_message(std::string& message) {
    VMGlobals* g = gMainVMGlobals;
    PyrObject* scString = reinterpret_cast<PyrObject*>(newPyrString(g->gc, message.c_str(), 0, true));
    return scString;
}

PyrObject* SC_Websocket_Lang::convert_message(std::vector<u_int8_t>& message) {
    VMGlobals* g = gMainVMGlobals;
    PyrObject* array = newPyrArray(g->gc, message.size(), 0, false);
    PyrSlot* slots = array->slots;
    for (size_t i = 0; i < message.size(); i++) {
        SetInt(slots + i, message[i]);
    }
    array->size = message.size();
    return array;
}

int WebSocketServer::Start(VMGlobals* g, int numArgsPushed) {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Start webserver");
#endif

    const auto object = get_object(g);
    int error, port;
    if ((error = get_port(object, port))) {
        return error;
    };
    const auto host = get_host(object);

#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Server started on port %i\n", port);
    scprintf("Server started on host %s\n", host.c_str());
#endif

    auto const address = boost::asio::ip::make_address_v4(host);

    const auto webSocketThread = SC_Websocket::WebSocketThread::getInstance();
    webSocketThread->start();

    beast::error_code ec;
    auto listener = std::make_shared<SC_Websocket::WebSocketListener>(
        std::move(webSocketThread), boost::asio::ip::tcp::endpoint(address, port), ec);
    if (ec) {
        scprintf("Could not start websocket listener: %s\n", ec.message().c_str());
        return errFailed;
    }
    listener->run();

    SetPtr(object->slots + SLOT_OFFSET::POINTER, listener.get());

    return errNone;
}

int WebSocketServer::Stop(VMGlobals* g, int numArgsPushed) {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Close webserver\n");
#endif
    const auto object = get_object(g);
    const auto ptr = slotRawPtr(object->slots + SLOT_OFFSET::POINTER);
    const auto listener = static_cast<SC_Websocket::WebSocketListener*>(ptr);
    listener->stop();
    return errNone;
}

int WebSocketServer::get_port(PyrObject* webSocketServerObj, int& port) {
    return slotIntVal(webSocketServerObj->slots + SLOT_OFFSET::PORT, &port);
}

std::string WebSocketServer::get_host(PyrObject* webSocketServerObj) {
    return std::get<1>(slotStrStdStrVal(webSocketServerObj->slots + SLOT_OFFSET::HOST));
}

PyrObject* WebSocketServer::get_object(VMGlobals* g) {
    PyrSlot* webSocketServerSlot = g->sp;
    return slotRawObject(webSocketServerSlot);
}

int WebSocketConnection::SendRawMessage(VMGlobals* g, int numArgsPushed) {
    auto connection = get_connection(g);
    auto message = get_raw_message(g);
    auto session = get_session(connection);

    session->enqueue_message(message);

    return errNone;
}

int WebSocketConnection::SendStringMessage(VMGlobals* g, int numArgsPushed) {
    auto connection = get_connection(g);
    auto message = get_string_message(g);
    auto session = get_session(connection);

    session->enqueue_message(message);

    return errNone;
}

int WebSocketConnection::Close(VMGlobals* g, int numArgsPushed) {
    auto connection = get_connection(g);
    auto session = get_session(connection);
    session->close();
    set_connection_closed(connection);
    return errNone;
}

PyrObject* WebSocketConnection::get_connection(VMGlobals* g) {
    PyrSlot* webSocketServerSlot = g->sp;
    return slotRawObject(webSocketServerSlot);
}

SC_Websocket::WebSocketSession* WebSocketConnection::get_session(PyrObject* connection) {
    auto ptr = slotRawPtr(connection->slots + SLOT_OFFSET::POINTER);
    return static_cast<SC_Websocket::WebSocketSession*>(ptr);
}

std::vector<uint8_t> WebSocketConnection::get_raw_message(VMGlobals* g) {
    ++g->sp;
    if (!isKindOfSlot(g->sp, getsym("Int8Array")->u.classobj)) {
        scprintf("Raw message must be an Int8Array\n");
        // @todo return here?
    };
    auto sclangByteArray = slotRawInt8Array(g->sp);
    auto data = std::vector<uint8_t>(sclangByteArray->b, sclangByteArray->b + sclangByteArray->size);
    return data;
}

// @todo make this a bit nicer so it can also return an error code
std::string WebSocketConnection::get_string_message(VMGlobals* g) { return std::get<1>(slotStrStdStrVal(++g->sp)); }

void WebSocketConnection::set_connection_closed(PyrObject* connection) {
    SetTrue(connection->slots + SLOT_OFFSET::CLOSED);
}

int WebSocketClient::SendStringMessage(VMGlobals* g, int numArgsPushed) {
    auto object = get_object(g, -1);
    int error;
    auto message = get_string_message(g, error);
    if (error) {
        scprintf("Failed to extract string message\n");
        return error;
    }
    auto client = get_client(object);

    client->enqueue_message(message);

    return errNone;
}

int WebSocketClient::SendByteMessage(VMGlobals* g, int numArgsPushed) { return errNone; }

int WebSocketClient::Connect(VMGlobals* g, int numArgsPushed) {
    auto object = get_object(g);

    int error;
    std::string host = get_host(object, error);
    if (error) {
        scprintf("Error parsing host\n");
        return error;
    }
    int port = get_port(object, error);
    if (error) {
        scprintf("Error parsing port\n");
        return error;
    }
    auto portString = std::to_string(port);

    const auto webSocketThread = SC_Websocket::WebSocketThread::getInstance();
    webSocketThread->start();

    auto client = std::make_shared<SC_Websocket::WebSocketClient>(webSocketThread->get_context());

    client->run(host, portString);

    SetPtr(object->slots + SLOT_OFFSET::POINTER, client.get());
    client->self = client.get();

    return errNone;
}

int WebSocketClient::Close(VMGlobals* g, int numArgsPushed) {
    auto object = get_object(g);
    auto client = get_client(object);
    auto ec = client->close_connection();

    if (ec != boost::asio::error::eof) {
        scprintf("Error closing connection: %s\n", ec.message().c_str());
        return errFailed;
    }

    SetFalse(object->slots + SLOT_OFFSET::CONNECTED);
    return errNone;
}

void WebSocketClient::received_message(SC_Websocket::WebSocketClient* client, SC_Websocket::WebSocketData& message) {
    VMGlobals* g = gMainVMGlobals;
    gLangMutex.lock();
    SetObject(++g->sp, getsym("WebSocketClient")->u.classobj);
    SetPtr(++g->sp, client);
    if (std::holds_alternative<std::string>(message)) {
        SetObject(++g->sp, convert_message(std::get<std::string>(message)));
    } else {
        SetObject(++g->sp, convert_message(std::get<std::vector<u_int8_t>>(message)));
    };
    runInterpreter(g, getsym("prReceivedMessage"), 3);
    gLangMutex.unlock();
}

PyrObject* WebSocketClient::get_object(VMGlobals* g, const int offset) {
    PyrSlot* webSocketServerSlot = g->sp + offset;
    return slotRawObject(webSocketServerSlot);
}

std::string WebSocketClient::get_host(PyrObject* webSocketClientObj, int& error) {
    auto result = slotStdStrVal(webSocketClientObj->slots + SLOT_OFFSET::HOST);
    error = std::get<0>(result);
    return std::get<1>(result);
}

std::string WebSocketClient::get_string_message(VMGlobals* g, int& error) {
    auto result = slotStrStdStrVal(g->sp);
    error = std::get<0>(result);
    return std::get<1>(result);
}

int WebSocketClient::get_port(PyrObject* webSocketClientObj, int& error) {
    int value;
    error = slotIntVal(webSocketClientObj->slots + SLOT_OFFSET::PORT, &value);
    return value;
}

SC_Websocket::WebSocketClient* WebSocketClient::get_client(PyrObject* object) {
    auto ptr = slotRawPtr(object->slots + SLOT_OFFSET::POINTER);
    return static_cast<SC_Websocket::WebSocketClient*>(ptr);
}

void init_WebSocket_primitives() {
    int base = nextPrimitiveIndex();
    int index = 0;

    definePrimitive(base, index++, "_WebSocketServer_Start", WebSocketServer::Start, 1, 0);
    definePrimitive(base, index++, "_WebSocketServer_Stop", WebSocketServer::Stop, 1, 0);
    definePrimitive(base, index++, "_WebSocketConnection_SendRawMessage",
                    // @todo this has 2 args?
                    WebSocketConnection::SendRawMessage, 1, 0);
    definePrimitive(base, index++, "_WebSocketConnection_SendStringMessage",
                    // @todo this has 2 args?
                    WebSocketConnection::SendStringMessage, 1, 0);
    definePrimitive(base, index++, "_WebSocketConnection_Close", WebSocketConnection::Close, 1, 0);
    definePrimitive(base, index++, "_WebSocketClient_Connect", WebSocketClient::Connect, 1, 0);
    definePrimitive(base, index++, "_WebSocketClient_Close", WebSocketClient::Close, 1, 0);
    definePrimitive(base, index++, "_WebSocketClient_SendStringMessage", WebSocketClient::SendStringMessage, 2, 0);
    definePrimitive(base, index++, "_WebSocketClient_SendByteMessage", WebSocketClient::SendByteMessage, 2, 0);
}