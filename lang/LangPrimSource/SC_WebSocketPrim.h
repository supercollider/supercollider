#pragma once

#include <PyrInterpreter.h>
// #include <PyrKernel.h>
#include <PyrObject.h>
#include <PyrPrimitive.h>
// #include <PyrSched.h>
// #include <PyrSymbolTable.h>
#include <SC_Lock.h>

#include "SC_WebSocket.h"

// forward declaration
namespace SC_Websocket {
class WebSocketSession;
class WebSocketClient;
typedef std::variant<std::vector<uint8_t>, std::string> WebSocketData;
}

// implementation of glue-code for sclang via primitives
namespace SC_Websocket_Lang {

PyrObject* convert_message(std::string& message);
PyrObject* convert_message(std::vector<uint8_t>& message);

class WebSocketServer {
public:
    static int Start(VMGlobals* g, int numArgsPushed);
    ;

    static int Stop(VMGlobals* g, int numArgsPushed);

private:
    enum SLOT_OFFSET {
        HOST = 0,
        PORT = 1,
        POINTER = 2,
    };

    static int get_port(PyrObject* webSocketServerObj, int& port);

    static std::string get_host(PyrObject* webSocketServerObj);

    static PyrObject* get_object(VMGlobals* g);
};

class WebSocketConnection {
public:
    static int SendRawMessage(VMGlobals* g, int numArgsPushed);

    static int SendStringMessage(VMGlobals* g, int numArgsPushed);

    static int Close(VMGlobals* g, int numArgsPushed);

private:
    enum SLOT_OFFSET {
        POINTER = 0,
        CLOSED = 1,
    };

    static PyrObject* get_connection(VMGlobals* g);

    static SC_Websocket::WebSocketSession* get_session(PyrObject* connection);

    static std::vector<uint8_t> get_raw_message(VMGlobals* g);

    static std::string get_string_message(VMGlobals* g);

    static void set_connection_closed(PyrObject* connection);
};

class WebSocketClient {
public:
    static int SendStringMessage(VMGlobals* g, int numArgsPushed);

    static int SendByteMessage(VMGlobals* g, int numArgsPushed);

    static int Connect(VMGlobals* g, int numArgsPushed);

    static int Close(VMGlobals* g, int numArgsPushed);

    // called from C++ side
    static void received_message(SC_Websocket::WebSocketClient* client, SC_Websocket::WebSocketData& message);

private:
    enum SLOT_OFFSET {
        HOST = 0,
        PORT = 1,
        POINTER = 2,
        CONNECTED = 3,
    };

    static PyrObject* get_object(VMGlobals* g, const int offset = 0);

    static std::string get_host(PyrObject* webSocketClientObj, int& error);

    static std::string get_string_message(VMGlobals* g, int& error);

    static int get_port(PyrObject* webSocketClientObj, int& error);

    static SC_Websocket::WebSocketClient* get_client(PyrObject* object);
};
}
