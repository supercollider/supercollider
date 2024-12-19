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
#include <VMGlobals.h>

#include "SC_WorldOptions.h"
#include "boost/asio/io_service.hpp"
#include "boost/asio/strand.hpp"


namespace beast = boost::beast;

namespace SC_Websocket {

class WebSocketThread {
public:
    // mutex not necessary b/c we only access it from sclang main?
    static std::shared_ptr<WebSocketThread> getInstance() {
        auto instance_ = instance.lock();
        if (!instance_) {
            instance_.reset(new WebSocketThread());
            instance = instance_;
        }
        return instance_;
    }

    // copy/move operations are removed for singleton pattern
    WebSocketThread(const WebSocketThread&) = delete;
    WebSocketThread& operator=(const WebSocketThread&) = delete;
    WebSocketThread(WebSocketThread&&) = delete;
    WebSocketThread& operator=(WebSocketThread&&) = delete;

    boost::asio::io_context& get_context() { return io_context; }

    void start() {
        if (!thread) {
            scprintf("Start websocket thread\n");
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

private:
    WebSocketThread() {}
    static std::weak_ptr<WebSocketThread> instance;
    boost::asio::io_context io_context;
    std::shared_ptr<SC_Thread> thread;
};

std::weak_ptr<WebSocketThread> WebSocketThread::instance;


class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
    beast::websocket::stream<tcp::socket> ws_;
    beast::flat_buffer buffer_;
    std::queue<std::vector<uint8_t>> outQueue;
    // a primitive mutex
    bool isWriting = false;
    bool isClosed = false;
    int listeningPort_;

public:
    WebSocketSession* ownAddress;
    // take ownership of socket
    explicit WebSocketSession(boost::asio::ip::tcp::socket&& socket, int listeningPort):
        ws_(std::move(socket)),
        listeningPort_(listeningPort) {}

    void run() {
        scprintf("Start run of session\n");
        boost::asio::dispatch(ws_.get_executor(),
                              beast::bind_front_handler(&WebSocketSession::on_run, shared_from_this()));
    }

    void on_run() {
        scprintf("Session started\n");
        ws_.set_option(beast::websocket::stream_base::timeout::suggested((beast::role_type::server)));

        ws_.async_accept(beast::bind_front_handler(&WebSocketSession::on_accept, shared_from_this()));
    }

    void on_accept(beast::error_code ec) {
        if (ec) {
            scprintf("Session accept error: %s\n", ec.message().c_str());
            return;
        }
        scprintf("Session accepted\n");

        VMGlobals* g = gMainVMGlobals;
        gLangMutex.lock();
        auto sclang_connection = create_sclang_connection();
        ++g->sp;
        SetObject(g->sp, getsym("WebSocketServer")->u.classobj);
        ++g->sp;
        SetInt(g->sp, listeningPort_);
        ++g->sp;
        SetObject(g->sp, sclang_connection);
        runInterpreter(g, getsym("prNewConnection"), 3);
        gLangMutex.unlock();

        do_read();
    }

    void do_read() {
        // read message into buffer
        ws_.async_read(buffer_, beast::bind_front_handler(&WebSocketSession::on_read, shared_from_this()));
    }

    void on_close() {
        isClosed = true;
        VMGlobals* g = gMainVMGlobals;
        gLangMutex.lock();
        ++g->sp;
        SetObject(g->sp, getsym("WebSocketConnection")->u.classobj);
        ++g->sp;
        SetPtr(g->sp, ownAddress);
        runInterpreter(g, getsym("prConnectionDisconnect"), 2);
        gLangMutex.unlock();
    }

    void on_read(beast::error_code ec, std::size_t bytes_transferred) {
        if (ec == beast::websocket::error::closed) {
            scprintf("Session closed\n");
        }
        if (ec) {
            scprintf("Session error: %s\n", ec.message().c_str());
            on_close();
            return;
        }

        VMGlobals* g = gMainVMGlobals;
        gLangMutex.lock();
        ++g->sp;
        SetObject(g->sp, getsym("WebSocketConnection")->u.classobj);
        ++g->sp;
        SetPtr(g->sp, ownAddress);
        ++g->sp;
        if (ws_.got_text()) {
            SetObject(g->sp, get_string(bytes_transferred));
        } else {
            SetObject(g->sp, get_byte_array(bytes_transferred));
        };
        runInterpreter(g, getsym("prReceiveMessage"), 3);
        gLangMutex.unlock();

        do_read();
    }

    void enqueue_message(std::vector<uint8_t> message) {
        boost::asio::dispatch(ws_.get_executor(), [message, self = shared_from_this()]() mutable {
            self->outQueue.push(message);
            self->do_write();
        });
    }

private:
    PyrObject* create_sclang_connection() {
        VMGlobals* g = gMainVMGlobals;
        PyrObject* newConnection = instantiateObject(g->gc, getsym("WebSocketConnection")->u.classobj, 1, true, false);
        SetPtr(newConnection->slots, ownAddress);
        return newConnection;
    }

    PyrObject* get_string(int bytes_transferred) {
        VMGlobals* g = gMainVMGlobals;
        std::string string = beast::buffers_to_string(buffer_.data());
        PyrObject* scString = (PyrObject*)newPyrString(g->gc, string.c_str(), 0, true);
        buffer_.consume(bytes_transferred);
        return scString;
    }

    /**
     * assumes a byte array has been transferred
     */
    PyrObject* get_byte_array(int bytes_transferred) {
        VMGlobals* g = gMainVMGlobals;
        PyrObject* array = newPyrArray(g->gc, bytes_transferred, 0, false);
        PyrSlot* slots = array->slots;

        const char* rawBytes = static_cast<const char*>(buffer_.data().data());
        for (size_t i = 0; i < bytes_transferred; i++) {
            SetInt(slots + i, static_cast<uint8_t>(rawBytes[i]));
        }
        array->size = bytes_transferred;
        buffer_.consume(bytes_transferred);
        return array;
    }

    void do_write(void) {
        if (isClosed) {
            scprintf("Session has been closed, so we don't write anymore\n");
            return;
        }
        if (!isWriting && !outQueue.empty()) {
            isWriting = true;
            ws_.binary(true);
            ws_.async_write(boost::asio::buffer(outQueue.front()),
                            boost::beast::bind_front_handler(&WebSocketSession::on_write, shared_from_this()));
        }
    }

    void on_write(boost::beast::error_code ec, std::size_t bytes_transferred) {
        isWriting = false;
        if (ec) {
            scprintf("Sending message failed: %s", ec.message().c_str());
        }
        outQueue.pop();
        // do this loop until our queue is empty
        do_write();
    }
};

class WebSocketListener : public std::enable_shared_from_this<WebSocketListener> {
    boost::asio::io_context& mIoContext;
    boost::asio::ip::tcp::acceptor mAcceptor;
    std::shared_ptr<WebSocketThread> mThread;

public:
    // take ownership of the shared_ptr so
    WebSocketListener(const std::shared_ptr<WebSocketThread> web_socket_thread, boost::asio::ip::tcp::endpoint endpoint,
                      boost::beast::error_code& ec):
        mIoContext(web_socket_thread->get_context()),
        mAcceptor(web_socket_thread->get_context()),
        mThread(web_socket_thread) {
        // @todo if this fails we should not call run anymore
        mAcceptor.open(endpoint.protocol(), ec);
        if (ec) {
            scprintf("could not open endpoint\n");
            return;
        }

        mAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
        if (ec) {
            scprintf("could not set reuse address\n");
            return;
        }

        mAcceptor.bind(endpoint, ec);
        if (ec) {
            scprintf("could not bind endpoint\n");
            return;
        }

        mAcceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
            scprintf("could not listen on endpoint\n");
            return;
        }
    }

    void run() {
        scprintf("Starting websocket listener...\n");
        doAccept();
    }

    void stop() {
        boost::system::error_code ec;
        mAcceptor.close(ec);
        if (ec) {
            scprintf("could not close socket: %s\n", ec.message().c_str());
        }
    }

private:
    void doAccept() {
        scprintf("Starting websocket accept...\n");
        mAcceptor.async_accept(boost::asio::make_strand(mIoContext),
                               beast::bind_front_handler(&WebSocketListener::onAccept, shared_from_this()));
    }

    void onAccept(beast::error_code ec, boost::asio::ip::tcp::socket socket) {
        if (ec) {
            scprintf("could not accept connection: %s\n", ec.message().c_str());
            return;
        } else {
            scprintf("accepted connection\n");
            auto session = std::make_shared<WebSocketSession>(std::move(socket), mAcceptor.local_endpoint().port());
            session->ownAddress = session.get();
            session->run();
        }

        doAccept();
    }
};

class WebSocketServer {
public:
    static int Start(VMGlobals* g, int numArgsPushed) {
        scprintf("Start webserver");

        const auto object = get_object(g);
        int error, port;
        std::string host;

        if ((error = get_port(object, port))) {
            return error;
        };
        if ((error = get_host(object, host))) {
            return error;
        }

        scprintf("Server started on port %i\n", port);
        scprintf("Server started on host %s\n", host.c_str());

        auto const address = boost::asio::ip::make_address_v4(host);

        const auto webSocketThread = WebSocketThread::getInstance();
        webSocketThread->start();


        boost::beast::error_code ec;
        auto listener = std::make_shared<WebSocketListener>(std::move(webSocketThread),
                                                            boost::asio::ip::tcp::endpoint(address, port), ec);

        if (ec) {
            scprintf("could not start websocket listener\n");
            return errFailed;
        }
        listener->run();

        return errNone;
    };

    static int Stop(VMGlobals* g, int numArgsPushed) {
        scprintf("Close webserver\n");
        return 0;
    }


private:
    enum SLOT_OFFSET {
        HOST = 0,
        PORT = 1,
    };

    static int get_port(PyrObject* webSocketServerObj, int& port) {
        return slotIntVal(webSocketServerObj->slots + SLOT_OFFSET::PORT, &port);
    }

    static int get_host(PyrObject* webSocketServerObj, std::string& host) {
        // sclang is using c-style strings internally
        host.resize(128);
        return slotStrVal(webSocketServerObj->slots + SLOT_OFFSET::HOST, host.data(), host.size());
    }

    static PyrObject* get_object(VMGlobals* g) {
        PyrSlot* webSocketServerSlot = g->sp;
        return slotRawObject(webSocketServerSlot);
    }
};

class WebSocketConnection {
public:
    static int SendMessage(VMGlobals* g, int numArgsPushed) {
        auto connection = get_connection(g);
        auto message = get_message(g);
        auto session = get_session(connection);

        session->enqueue_message(message);

        return errNone;
    }

    static int Close(VMGlobals* g, int numArgsPushed) {
        auto connection = get_connection(g);
        auto session = get_session(connection);
        return errNone;
    }

private:
    static PyrObject* get_connection(VMGlobals* g) {
        PyrSlot* webSocketServerSlot = g->sp;
        return slotRawObject(webSocketServerSlot);
    }

    static WebSocketSession* get_session(PyrObject* connection) {
        auto ptr = slotRawPtr(connection->slots);
        return static_cast<WebSocketSession*>(ptr);
    }

    static std::vector<uint8_t> get_message(VMGlobals* g) {
        ++g->sp;
        if (!isKindOfSlot(g->sp, getsym("Int8Array")->u.classobj)) {
            scprintf("Not a valid object type\n");
        };
        auto sclangByteArray = slotRawInt8Array(g->sp);
        auto data = std::vector<uint8_t>(sclangByteArray->b, sclangByteArray->b + sclangByteArray->size);
        return data;
    }

    static int get_beast_session(PyrObject* webSocketConnectionObj, WebSocketSession*& session) {}
};
}

void init_WebSocket_primitives() {
    int base = nextPrimitiveIndex();
    int index = 0;

    definePrimitive(base, index++, "_WebSocketServer_Start", SC_Websocket::WebSocketServer::Start, 1, 0);
    definePrimitive(base, index++, "_WebSocketServer_Stop", SC_Websocket::WebSocketServer::Stop, 1, 0);
    definePrimitive(base, index++, "_WebSocketConnection_SendMessage", SC_Websocket::WebSocketConnection::SendMessage,
                    1, 0);
    definePrimitive(base, index++, "_WebSocketConnection_Close", SC_Websocket::WebSocketConnection::Close, 1, 0);
}
