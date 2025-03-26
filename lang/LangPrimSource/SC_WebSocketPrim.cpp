#include <boost/asio.hpp>

#include "PyrPrimitive.h"
#include "PyrKernel.h"
#include "SC_Lock.h"
#include "PyrSched.h"
#include "PyrInterpreter.h"
#include "SC_WorldOptions.h"
#include "PyrSymbolTable.h"

#include "SC_WebSocketPrim.h"
#include "SC_WebSocket.h"

using namespace SC_Websocket_Lang;

std::vector<uint8_t> getByteVector(VMGlobals* g, int& error) {
    if (!isKindOfSlot(g->sp, getsym("Int8Array")->u.classobj)) {
        scprintf("Raw message must be an Int8Array\n");
        error = errBadPrimitive;
        return std::vector<uint8_t>();
    };
    auto sclangByteArray = slotRawInt8Array(g->sp);
    auto data = std::vector<uint8_t>(sclangByteArray->b, sclangByteArray->b + sclangByteArray->size);
    return data;
}

std::string getStringMessage(VMGlobals* g, int& error) {
    auto result = slotStrStdStrVal(g->sp);
    error = std::get<0>(result);
    return std::get<1>(result);
}


PyrObject* SC_Websocket_Lang::convertMessage(std::string& message) {
    VMGlobals* g = gMainVMGlobals;
    PyrObject* scString = reinterpret_cast<PyrObject*>(newPyrString(g->gc, message.c_str(), 0, true));
    return scString;
}

PyrObject* SC_Websocket_Lang::convertMessage(std::vector<uint8_t>& message) {
    VMGlobals* g = gMainVMGlobals;
    PyrObject* array = newPyrArray(g->gc, message.size(), 0, false);
    PyrSlot* slots = array->slots;
    for (size_t i = 0; i < message.size(); i++) {
        SetInt(slots + i, message[i]);
    }
    array->size = message.size();
    return array;
}

int WebSocketServer::start(VMGlobals* g, int numArgsPushed) {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Start webserver");
#endif

    const auto object = getObject(g);
    int error, port;
    if ((error = getPort(object, port))) {
        return error;
    };
    const auto host = getHost(object);

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

int WebSocketServer::stop(VMGlobals* g, int numArgsPushed) {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Close webserver\n");
#endif
    const auto object = getObject(g);
    const auto ptr = slotRawPtr(object->slots + SLOT_OFFSET::POINTER);
    const auto listener = static_cast<SC_Websocket::WebSocketListener*>(ptr);
    listener->stop();
    return errNone;
}

int WebSocketServer::getPort(PyrObject* object, int& port) {
    return slotIntVal(object->slots + SLOT_OFFSET::PORT, &port);
}

std::string WebSocketServer::getHost(PyrObject* object) {
    return std::get<1>(slotStrStdStrVal(object->slots + SLOT_OFFSET::HOST));
}

PyrObject* WebSocketServer::getObject(VMGlobals* g) {
    PyrSlot* webSocketServerSlot = g->sp;
    return slotRawObject(webSocketServerSlot);
}

int WebSocketConnection::sendRawMessage(VMGlobals* g, int numArgsPushed) {
    int error = errNone;
    auto message = getByteVector(g, error);
    if (error) {
        return error;
    };
    --g->sp;
    auto connection = getConnection(g);
    ++g->sp;
    ++g->sp;

    auto session = getSession(connection);
    session->enqueueMessage(message);

    return errNone;
}

int WebSocketConnection::sendStringMessage(VMGlobals* g, int numArgsPushed) {
    int error = errNone;
    auto message = getStringMessage(g, error);
    if (error) {
        return error;
    };
    --g->sp;
    auto connection = getConnection(g);
    ++g->sp;
    ++g->sp;

    auto session = getSession(connection);
    session->enqueueMessage(message);

    return errNone;
}

int WebSocketConnection::close(VMGlobals* g, int numArgsPushed) {
    auto connection = getConnection(g);
    auto session = getSession(connection);
    session->close();
    return errNone;
}

void WebSocketConnection::newLangConnection(SC_Websocket::WebSocketSession* session, int portNumber) {
    VMGlobals* g = gMainVMGlobals;
    gLangMutex.lock();
    auto sclangConnection = createConnection(session);
    SetObject(++g->sp, getsym("WebSocketServer")->u.classobj);
    SetInt(++g->sp, portNumber);
    SetObject(++g->sp, sclangConnection);
    runInterpreter(g, getsym("prNewConnection"), 3);
    gLangMutex.unlock();
}

void WebSocketConnection::closeLangConnection(SC_Websocket::WebSocketSession* session) {
    VMGlobals* g = gMainVMGlobals;
    gLangMutex.lock();
    SetObject(++g->sp, getsym("WebSocketConnection")->u.classobj);
    SetPtr(++g->sp, session);
    runInterpreter(g, getsym("prDisconnect"), 2);
    gLangMutex.unlock();
}

void WebSocketConnection::receiveLangMessage(SC_Websocket::WebSocketSession* session,
                                             SC_Websocket::WebSocketData& message) {
    VMGlobals* g = gMainVMGlobals;
    gLangMutex.lock();
    SetObject(++g->sp, getsym("WebSocketConnection")->u.classobj);
    SetPtr(++g->sp, session);

    if (std::holds_alternative<std::string>(message)) {
        SetObject(++g->sp, convertMessage(std::get<std::string>(message)));
    } else {
        SetObject(++g->sp, convertMessage(std::get<std::vector<uint8_t>>(message)));
    };

    runInterpreter(g, getsym("prReceiveMessage"), 3);
    gLangMutex.unlock();
}

PyrObject* WebSocketConnection::getConnection(VMGlobals* g) {
    PyrSlot* webSocketServerSlot = g->sp;
    return slotRawObject(webSocketServerSlot);
}

SC_Websocket::WebSocketSession* WebSocketConnection::getSession(PyrObject* connection) {
    auto ptr = slotRawPtr(connection->slots + SLOT_OFFSET::POINTER);
    return static_cast<SC_Websocket::WebSocketSession*>(ptr);
}

PyrObject* WebSocketConnection::createConnection(SC_Websocket::WebSocketSession* session) {
    VMGlobals* g = gMainVMGlobals;
    PyrObject* newConnection = instantiateObject(g->gc, getsym("WebSocketConnection")->u.classobj, 1, true, false);
    SetPtr(newConnection->slots, session);
    return newConnection;
}

int WebSocketClient::sendStringMessage(VMGlobals* g, int numArgsPushed) {
    int error = errNone;
    auto message = getStringMessage(g, error);
    if (error) {
        scprintf("Failed to extract string message\n");
        return error;
    }
    --g->sp;
    auto object = getObject(g);
    ++g->sp;
    ++g->sp;

    auto client = getClient(object);
    client->enqueueMessage(message);

    return errNone;
}

int WebSocketClient::sendRawMessage(VMGlobals* g, int numArgsPushed) {
    int error = errNone;
    auto message = getByteVector(g, error);
    if (error) {
        scprintf("Failed to extract byte message\n");
        return error;
    }
    --g->sp;
    auto object = getObject(g);
    ++g->sp;
    ++g->sp;

    auto client = getClient(object);
    client->enqueueMessage(message);

    return errNone;
}

int WebSocketClient::connect(VMGlobals* g, int numArgsPushed) {
    auto object = getObject(g);

    int error = errNone;
    std::string host = getHost(object, error);
    if (error) {
        scprintf("Error parsing host\n");
        return error;
    }
    int port = getPort(object, error);
    if (error) {
        scprintf("Error parsing port\n");
        return error;
    }
    auto portString = std::to_string(port);

    const auto webSocketThread = SC_Websocket::WebSocketThread::getInstance();
    webSocketThread->start();

    auto client = std::make_shared<SC_Websocket::WebSocketClient>(webSocketThread->getContext());

    client->run(host, portString);

    SetPtr(object->slots + SLOT_OFFSET::POINTER, client.get());
    client->m_self = client.get();

    return errNone;
}

int WebSocketClient::close(VMGlobals* g, int numArgsPushed) {
    auto object = getObject(g);
    auto client = getClient(object);
    auto ec = client->closeConnection();
    SetFalse(object->slots + SLOT_OFFSET::CONNECTED);
    return errNone;
}

void WebSocketClient::receivedMessage(SC_Websocket::WebSocketClient* client, SC_Websocket::WebSocketData& message) {
    VMGlobals* g = gMainVMGlobals;
    gLangMutex.lock();
    SetObject(++g->sp, getsym("WebSocketClient")->u.classobj);
    SetPtr(++g->sp, client);
    if (std::holds_alternative<std::string>(message)) {
        SetObject(++g->sp, convertMessage(std::get<std::string>(message)));
    } else {
        SetObject(++g->sp, convertMessage(std::get<std::vector<uint8_t>>(message)));
    };
    runInterpreter(g, getsym("prReceivedMessage"), 3);
    gLangMutex.unlock();
}

void WebSocketClient::setConnectionStatus(SC_Websocket::WebSocketClient* client, bool connected) {
    VMGlobals* g = gMainVMGlobals;
    gLangMutex.lock();
    SetObject(++g->sp, getsym("WebSocketClient")->u.classobj);
    SetPtr(++g->sp, client);
    SetBool(++g->sp, connected);
    runInterpreter(g, getsym("prSetConnectionStatus"), 3);
    gLangMutex.unlock();
}

PyrObject* WebSocketClient::getObject(VMGlobals* g) { return slotRawObject(g->sp); }

std::string WebSocketClient::getHost(PyrObject* webSocketClientObj, int& error) {
    auto result = slotStdStrVal(webSocketClientObj->slots + SLOT_OFFSET::HOST);
    error = std::get<0>(result);
    return std::get<1>(result);
}

int WebSocketClient::getPort(PyrObject* webSocketClientObj, int& error) {
    int value;
    error = slotIntVal(webSocketClientObj->slots + SLOT_OFFSET::PORT, &value);
    return value;
}

SC_Websocket::WebSocketClient* WebSocketClient::getClient(PyrObject* object) {
    auto ptr = slotRawPtr(object->slots + SLOT_OFFSET::POINTER);
    return static_cast<SC_Websocket::WebSocketClient*>(ptr);
}

void init_WebSocket_primitives() {
    int base = nextPrimitiveIndex();
    int index = 0;

    definePrimitive(base, index++, "_WebSocketServer_Start", WebSocketServer::start, 1, 0);
    definePrimitive(base, index++, "_WebSocketServer_Stop", WebSocketServer::stop, 1, 0);
    definePrimitive(base, index++, "_WebSocketConnection_SendRawMessage", WebSocketConnection::sendRawMessage, 2, 0);
    definePrimitive(base, index++, "_WebSocketConnection_SendStringMessage", WebSocketConnection::sendStringMessage, 2,
                    0);
    definePrimitive(base, index++, "_WebSocketConnection_Close", WebSocketConnection::close, 1, 0);
    definePrimitive(base, index++, "_WebSocketClient_Connect", WebSocketClient::connect, 1, 0);
    definePrimitive(base, index++, "_WebSocketClient_Close", WebSocketClient::close, 1, 0);
    definePrimitive(base, index++, "_WebSocketClient_SendStringMessage", WebSocketClient::sendStringMessage, 2, 0);
    definePrimitive(base, index++, "_WebSocketClient_SendRawMessage", WebSocketClient::sendRawMessage, 2, 0);
}
