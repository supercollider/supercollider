#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

#include "SC_WebSocket.h"

#include <PyrInterpreter.h>
#include <PyrKernel.h>
#include <PyrObject.h>
#include <PyrPrimitive.h>
#include <PyrSched.h>
#include <PyrSymbolTable.h>
#include <SC_Lock.h>
#include <thread>
#include <queue>
#include <VMGlobals.h>

#include "SC_WorldOptions.h"
#include "boost/asio/io_service.hpp"
#include "boost/asio/strand.hpp"

// (un)-comment to toggle debugging print statements
#define SC_WEBSOCKET_DEBUG

namespace beast = boost::beast;

namespace SC_Websocket {

/** A wrapper class for the websocket communication thread.
This gets initiated into a static variable upon request.
Due to this static lifetime the singleton only gets deleted when sclang closes.
*/
class WebSocketThread {
public:
    static std::shared_ptr<WebSocketThread> getInstance() {
        static std::shared_ptr<WebSocketThread> instance_ = std::make_shared<WebSocketThread>();
        return instance_;
    }

    // Deleted copy/move constructors
    WebSocketThread(const WebSocketThread&) = delete;
    WebSocketThread& operator=(const WebSocketThread&) = delete;
    WebSocketThread(WebSocketThread&&) = delete;
    WebSocketThread& operator=(WebSocketThread&&) = delete;

    boost::asio::io_context& get_context() { return io_context; }

    void start() {
        if (!thread) {
#ifdef SC_WEBSOCKET_DEBUG
            scprintf("Start websocket thread\n");
#endif
            thread = std::make_shared<SC_Thread>([this]() {
                auto work = boost::asio::make_work_guard(io_context);
                io_context.run();
            });
        }
    }

    void stop() {
        io_context.stop();
        if (thread && thread->joinable()) {
            thread->join();
        }
        thread.reset();
    }

    ~WebSocketThread() {
        scprintf("Clean websocket thread\n");
        stop();
    }

    //@todo how to make this private?
    WebSocketThread() {}

private:
    boost::asio::io_context io_context;
    std::shared_ptr<SC_Thread> thread;
};

// a websocket message can either be a byte array or a string
// see https://developer.mozilla.org/en-US/docs/Web/API/WebSocket/message_event
using WebSocketData = std::variant<std::vector<uint8_t>, std::string>;

// a WebSocketSession is essentially a websocket connection
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
    beast::websocket::stream<tcp::socket> ws;
    beast::flat_buffer buffer;
    // data to be sent to the client
    std::queue<WebSocketData> out_queue;
    // a primitive mutex - see `do_write` and `on_write`
    bool isWriting = false;
    // used to indicate that the connection should be closed upon next message
    bool shouldClose = false;
    int listeningPort;

public:
    // we store a reference pointer to ourselves upon creation
    // this pointer acts as an identifier on the sclang side,
    // so it is possible to distinguish connections and we can
    // call the the callback on the matching sclang WebSocketConnection
    WebSocketSession* ownAddress;
    // take ownership of socket
    explicit WebSocketSession(boost::asio::ip::tcp::socket&& socket, int listeningPort):
        ws(std::move(socket)),
        listeningPort(listeningPort) {}

    // starts consumption of a websocket connection
    void run() {
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Start run of session\n");
#endif
        boost::asio::dispatch(ws.get_executor(),
                              beast::bind_front_handler(&WebSocketSession::on_run, shared_from_this()));
    }

    void enqueue_message(WebSocketData message) {
        // dispatch via asio to ensure thread safety
        boost::asio::dispatch(ws.get_executor(), [message, self = shared_from_this()]() mutable {
            self->out_queue.push(message);
            self->await_writing_ws_message();
        });
    }

    /**
     * The connection gets closed by setting the member variable `shouldClose`
     * to true and trigger an empty message which will release the connection by
     * not extending its lifetime.
     */
    void close() {
        shouldClose = true;
        enqueue_message(std::string(""));
    }

private:
    void on_run() {
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Session started\n");
#endif
        ws.set_option(beast::websocket::stream_base::timeout::suggested((beast::role_type::server)));

        ws.async_accept(beast::bind_front_handler(&WebSocketSession::on_accept, shared_from_this()));
    }

    void on_accept(beast::error_code ec) {
        if (ec) {
#ifdef SC_WEBSOCKET_DEBUG
            scprintf("Session accept error: %s\n", ec.message().c_str());
#endif
            return;
        }
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Session accepted\n");
#endif

        call_sclang_new_connection();
        await_receiving_ws_message();
    }

    // aka do_read in boost
    void await_receiving_ws_message() {
        ws.async_read(buffer,
                      beast::bind_front_handler(&WebSocketSession::consume_received_ws_message, shared_from_this()));
    }

    // consume a received websocket message - aka on_read in boost
    void consume_received_ws_message(beast::error_code ec, std::size_t bytes_transferred) {
        if (ec) {
            if (shouldClose) {
                return;
            };
            if (ec == beast::websocket::error::closed) {
#ifdef SC_WEBSOCKET_DEBUG
                scprintf("Session closed\n");
#endif
            } else {
                scprintf("Websocket connection error: %s\n", ec.message().c_str());
            };
            call_sclang_close();
            return;
        }
        call_sclang_message_received(bytes_transferred);
        // continue async loop to await websocket message
        await_receiving_ws_message();
    }

    // consume out queue until empty - aka do_write on beast
    void await_writing_ws_message(void) {
        if (shouldClose) {
            ws.close("Goodbye from sclang");
#ifdef SC_WEBSOCKET_DEBUG
            scprintf("Closing session\n");
#endif
            return;
        }
        if (!isWriting && !out_queue.empty()) {
            isWriting = true;
            auto message = out_queue.front();

            // if a string, indicate it as a text message
            ws.text(std::holds_alternative<std::string>(message));

            ws.async_write(boost::asio::buffer(to_asio_buffer(message)),
                           boost::beast::bind_front_handler(&WebSocketSession::on_ws_write, shared_from_this()));
        }
    }

    // aka on_write
    void on_ws_write(boost::beast::error_code ec, std::size_t bytes_transferred) {
        isWriting = false;
        if (ec) {
            scprintf("Sending websocket message failed: %s", ec.message().c_str());
        }
        out_queue.pop();
        // do this loop until our queue is empty
        await_writing_ws_message();
    }

    // sclang parts
    void call_sclang_new_connection() {
        VMGlobals* g = gMainVMGlobals;
        gLangMutex.lock();
        auto sclang_connection = create_sclang_connection();
        ++g->sp;
        SetObject(g->sp, getsym("WebSocketServer")->u.classobj);
        ++g->sp;
        SetInt(g->sp, listeningPort);
        ++g->sp;
        SetObject(g->sp, sclang_connection);
        runInterpreter(g, getsym("prNewConnection"), 3);
        gLangMutex.unlock();
    }

    // informs sclang that our websocket connection is now closed
    void call_sclang_close() {
        VMGlobals* g = gMainVMGlobals;
        gLangMutex.lock();
        ++g->sp;
        SetObject(g->sp, getsym("WebSocketConnection")->u.classobj);
        ++g->sp;
        SetPtr(g->sp, ownAddress);
        runInterpreter(g, getsym("prConnectionDisconnect"), 2);
        gLangMutex.unlock();
    }

    void call_sclang_message_received(std::size_t bytes_transferred) {
        VMGlobals* g = gMainVMGlobals;
        gLangMutex.lock();
        ++g->sp;
        SetObject(g->sp, getsym("WebSocketConnection")->u.classobj);
        ++g->sp;
        SetPtr(g->sp, ownAddress);
        ++g->sp;
        if (ws.got_text()) {
            SetObject(g->sp, get_string(bytes_transferred));
        } else {
            SetObject(g->sp, get_byte_array(bytes_transferred));
        };
        runInterpreter(g, getsym("prReceiveMessage"), 3);
        gLangMutex.unlock();
    }

    PyrObject* create_sclang_connection() {
        VMGlobals* g = gMainVMGlobals;
        PyrObject* newConnection = instantiateObject(g->gc, getsym("WebSocketConnection")->u.classobj, 1, true, false);
        SetPtr(newConnection->slots, ownAddress);
        return newConnection;
    }

    PyrObject* get_string(int bytes_transferred) {
        VMGlobals* g = gMainVMGlobals;
        std::string string = beast::buffers_to_string(buffer.data());
        PyrObject* scString = (PyrObject*)newPyrString(g->gc, string.c_str(), 0, true);
        buffer.consume(bytes_transferred);
        return scString;
    }

    PyrObject* get_byte_array(int bytes_transferred) {
        VMGlobals* g = gMainVMGlobals;
        PyrObject* array = newPyrArray(g->gc, bytes_transferred, 0, false);
        PyrSlot* slots = array->slots;

        const char* rawBytes = static_cast<const char*>(buffer.data().data());
        for (size_t i = 0; i < bytes_transferred; i++) {
            SetInt(slots + i, static_cast<uint8_t>(rawBytes[i]));
        }
        array->size = bytes_transferred;
        buffer.consume(bytes_transferred);
        return array;
    }

    // converter between WebSocketData and beast flat_buffer
    boost::asio::const_buffer to_asio_buffer(const WebSocketData& message) {
        return std::visit([](const auto& arg) { return boost::asio::buffer(arg); }, message);
    }
};

// acts as a server which listens for incoming connections
class WebSocketListener : public std::enable_shared_from_this<WebSocketListener> {
    boost::asio::io_context& mIoContext;
    boost::asio::ip::tcp::acceptor mAcceptor;
    std::shared_ptr<WebSocketThread> mThread;

public:
    // take ownership shared ptr of our web socket thread so we maintain the lifetime of the thread
    WebSocketListener(const std::shared_ptr<WebSocketThread>& web_socket_thread,
                      boost::asio::ip::tcp::endpoint endpoint, boost::beast::error_code& ec):
        mIoContext(web_socket_thread->get_context()),
        mAcceptor(web_socket_thread->get_context()),
        mThread(web_socket_thread) {
        mAcceptor.open(endpoint.protocol(), ec);
        if (ec) {
#ifdef SC_WEBSOCKET_DEBUG
            scprintf("Could not open endpoint - %s\n", ec.message().c_str());
#endif
            return;
        }

        mAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
        if (ec) {
#ifdef SC_WEBSOCKET_DEBUG
            scprintf("Could not set reuse address: %s\n", ec.message().c_str());
#endif
            return;
        }

        mAcceptor.bind(endpoint, ec);
        if (ec) {
#ifdef SC_WEBSOCKET_DEBUG
            scprintf("Could not bind to endpoint: %s\n", ec.message().c_str());
#endif
            return;
        }

        mAcceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
#ifdef SC_WEBSOCKET_DEBUG
            scprintf("Could not listen on endpoint: %s\n", ec.message().c_str());
#endif
            return;
        }
    }

    void run() {
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Starting websocket listener...\n");
#endif
        doAccept();
    }

    void stop() {
        boost::system::error_code ec;
        mAcceptor.close(ec);
        if (ec) {
            scprintf("Could not close websocket: %s\n", ec.message().c_str());
        }
    }

private:
    void doAccept() {
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Starting websocket accept...\n");
#endif
        mAcceptor.async_accept(boost::asio::make_strand(mIoContext),
                               beast::bind_front_handler(&WebSocketListener::onAccept, shared_from_this()));
    }

    void onAccept(beast::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted) {
                return;
            }
            scprintf("Could not accept connection: %s\n", ec.message().c_str());
            return;
        }
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("accepted connection\n");
#endif
        auto session = std::make_shared<WebSocketSession>(std::move(socket), mAcceptor.local_endpoint().port());
        session->ownAddress = session.get();
        session->run();
        doAccept();
    }
};

/**
 * Glue-code for sclang.
 */
class WebSocketServer {
public:
    static int Start(VMGlobals* g, int numArgsPushed) {
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

        const auto webSocketThread = WebSocketThread::getInstance();
        webSocketThread->start();

        boost::beast::error_code ec;
        auto listener = std::make_shared<WebSocketListener>(std::move(webSocketThread),
                                                            boost::asio::ip::tcp::endpoint(address, port), ec);
        if (ec) {
            scprintf("Could not start websocket listener: %s\n", ec.message().c_str());
            return errFailed;
        }
        listener->run();

        SetPtr(object->slots + SLOT_OFFSET::POINTER, listener.get());

        return errNone;
    };

    static int Stop(VMGlobals* g, int numArgsPushed) {
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Close webserver\n");
#endif
        const auto object = get_object(g);
        const auto ptr = slotRawPtr(object->slots + SLOT_OFFSET::POINTER);
        const auto listener = static_cast<WebSocketListener*>(ptr);
        listener->stop();
        return errNone;
    }


private:
    enum SLOT_OFFSET {
        HOST = 0,
        PORT = 1,
        POINTER = 2,
    };

    static int get_port(PyrObject* webSocketServerObj, int& port) {
        return slotIntVal(webSocketServerObj->slots + SLOT_OFFSET::PORT, &port);
    }

    static std::string get_host(PyrObject* webSocketServerObj) {
        return std::get<1>(slotStrStdStrVal(webSocketServerObj->slots + SLOT_OFFSET::HOST));
    }

    static PyrObject* get_object(VMGlobals* g) {
        PyrSlot* webSocketServerSlot = g->sp;
        return slotRawObject(webSocketServerSlot);
    }
};

/**
 * Acts as a namespace for sclang primitive glue code and
 * does not hold the actual WebSocket connection.
 */
class WebSocketConnection {
public:
    static int SendRawMessage(VMGlobals* g, int numArgsPushed) {
        auto connection = get_connection(g);
        auto message = get_raw_message(g);
        auto session = get_session(connection);

        session->enqueue_message(message);

        return errNone;
    }

    static int SendStringMessage(VMGlobals* g, int numArgsPushed) {
        auto connection = get_connection(g);
        auto message = get_string_message(g);
        auto session = get_session(connection);

        session->enqueue_message(message);

        return errNone;
    }

    static int Close(VMGlobals* g, int numArgsPushed) {
        auto connection = get_connection(g);
        auto session = get_session(connection);
        session->close();
        set_connection_closed(connection);
        return errNone;
    }

private:
    enum SLOT_OFFSET {
        POINTER = 0,
        CLOSED = 1,
    };

    static PyrObject* get_connection(VMGlobals* g) {
        PyrSlot* webSocketServerSlot = g->sp;
        return slotRawObject(webSocketServerSlot);
    }

    static WebSocketSession* get_session(PyrObject* connection) {
        auto ptr = slotRawPtr(connection->slots + SLOT_OFFSET::POINTER);
        return static_cast<WebSocketSession*>(ptr);
    }

    static std::vector<uint8_t> get_raw_message(VMGlobals* g) {
        ++g->sp;
        if (!isKindOfSlot(g->sp, getsym("Int8Array")->u.classobj)) {
            scprintf("Raw message must be an Int8Array\n");
            // @todo return here?
        };
        auto sclangByteArray = slotRawInt8Array(g->sp);
        auto data = std::vector<uint8_t>(sclangByteArray->b, sclangByteArray->b + sclangByteArray->size);
        return data;
    }

    static std::string get_string_message(VMGlobals* g) { return std::get<1>(slotStrStdStrVal(++g->sp)); }

    static void set_connection_closed(PyrObject* connection) { SetTrue(connection->slots + SLOT_OFFSET::CLOSED); }
};
}

void init_WebSocket_primitives() {
    int base = nextPrimitiveIndex();
    int index = 0;

    definePrimitive(base, index++, "_WebSocketServer_Start", SC_Websocket::WebSocketServer::Start, 1, 0);
    definePrimitive(base, index++, "_WebSocketServer_Stop", SC_Websocket::WebSocketServer::Stop, 1, 0);
    definePrimitive(base, index++, "_WebSocketConnection_SendRawMessage",
                    SC_Websocket::WebSocketConnection::SendRawMessage, 1, 0);
    definePrimitive(base, index++, "_WebSocketConnection_SendStringMessage",
                    SC_Websocket::WebSocketConnection::SendStringMessage, 1, 0);
    definePrimitive(base, index++, "_WebSocketConnection_Close", SC_Websocket::WebSocketConnection::Close, 1, 0);
}
