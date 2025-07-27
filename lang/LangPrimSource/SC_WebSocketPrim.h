#pragma once

#include <PyrPrimitive.h>
#include "SC_WebSocket.h"

// forward declaration
namespace SC_Websocket {
class WebSocketSession;
class WebSocketClient;
typedef std::variant<std::vector<uint8_t>, std::string> WebSocketData;
}

// namespace to connect the boost beast websocket implementation
// from `SC_WebSocket` to sclang primitives.
namespace SC_Websocket_Lang {

PyrObject* convertMessage(std::string& message);
PyrObject* convertMessage(std::vector<uint8_t>& message);

class WebSocketServer {
public:
    static int start(VMGlobals* g, int numArgsPushed);

    static int stop(VMGlobals* g, int numArgsPushed);

private:
    enum SLOT_OFFSET {
        HOST = 0,
        PORT = 1,
        POINTER = 2,
    };

    static int getPort(PyrObject* object, int& port);

    static std::string getHost(PyrObject* object);

    static PyrObject* getObject(VMGlobals* g);
};

class WebSocketConnection {
public:
    // methods called from sclang
    static int sendRawMessage(VMGlobals* g, int numArgsPushed);

    static int sendStringMessage(VMGlobals* g, int numArgsPushed);

    static int close(VMGlobals* g, int numArgsPushed);

    // callback functions called from boost beast
    static void newLangConnection(SC_Websocket::WebSocketSession* session, int portNumber);

    static void closeLangConnection(SC_Websocket::WebSocketSession* session);

    static void receiveLangMessage(SC_Websocket::WebSocketSession* session, SC_Websocket::WebSocketData& message);

private:
    enum SLOT_OFFSET {
        POINTER = 0,
        CLOSED = 1,
    };

    static PyrObject* getConnection(VMGlobals* g);

    static SC_Websocket::WebSocketSession* getSession(PyrObject* connection);

    static void setConnectionClosed(PyrObject* connection);

    static PyrObject* createConnection(SC_Websocket::WebSocketSession* session);
};

class WebSocketClient {
public:
    // methods called from sclang
    static int sendStringMessage(VMGlobals* g, int numArgsPushed);

    static int sendRawMessage(VMGlobals* g, int numArgsPushed);

    static int connect(VMGlobals* g, int numArgsPushed);

    static int close(VMGlobals* g, int numArgsPushed);

    // callback functions called from boost beast
    static void receivedMessage(SC_Websocket::WebSocketClient* client, SC_Websocket::WebSocketData& message);

    static void setConnectionStatus(SC_Websocket::WebSocketClient* client, bool connected);

private:
    enum SLOT_OFFSET {
        HOST = 0,
        PORT = 1,
        POINTER = 2,
        CONNECTED = 3,
    };

    static PyrObject* getObject(VMGlobals* g);

    static std::string getHost(PyrObject* webSocketClientObj, int& error);

    static int getPort(PyrObject* webSocketClientObj, int& error);

    static SC_Websocket::WebSocketClient* getClient(PyrObject* object);
};
}
