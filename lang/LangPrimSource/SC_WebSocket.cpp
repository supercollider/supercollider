#include "SC_WebSocket.h"

#include <PyrKernel.h>
#include <PyrSched.h>
#include <PyrSymbolTable.h>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include "boost/asio/io_service.hpp"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include <thread>
#include <queue>
#include <VMGlobals.h>
#include <asio/buffers_iterator.hpp>

#include "SC_WebSocketPrim.h"
#include "SC_WorldOptions.h"
#include "boost/asio/buffers_iterator.hpp"

#define SC_WEBSOCKET_DEBUG

namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

// handles the actual websocket implementation using boost::beast
namespace SC_Websocket {

WebSocketData convert_data(beast::flat_buffer& buffer, size_t bytesTransferred, bool isText) {
    WebSocketData data;
    if (isText) {
        data = beast::buffers_to_string(buffer.data());
    } else {
        auto array_data = std::vector<u_int8_t>(bytesTransferred);
        std::memcpy(array_data.data(), buffer.data().data(), bytesTransferred);
        data = array_data;
    }
    buffer.consume(bytesTransferred);
    return data;
}

std::shared_ptr<WebSocketThread> WebSocketThread::getInstance() {
    static std::shared_ptr<WebSocketThread> instance_ = std::make_shared<WebSocketThread>();
    return instance_;
}

boost::asio::io_context& WebSocketThread::get_context() { return io_context; }

void WebSocketThread::start() {
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

void WebSocketThread::stop() {
    io_context.stop();
    if (thread && thread->joinable()) {
        thread->join();
    }
    thread.reset();
}

WebSocketThread::~WebSocketThread() {
    scprintf("Clean websocket thread\n");
    stop();
}

WebSocketThread::WebSocketThread() {}

WebSocketSession::WebSocketSession(boost::asio::ip::tcp::socket&& socket, int listeningPort):
    ws(std::move(socket)),
    listeningPort(listeningPort) {}

void WebSocketSession::run() {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Start run of session\n");
#endif
    boost::asio::dispatch(ws.get_executor(), beast::bind_front_handler(&WebSocketSession::on_run, shared_from_this()));
}

void WebSocketSession::enqueue_message(WebSocketData message) {
    // dispatch via asio to ensure thread safety
    boost::asio::dispatch(ws.get_executor(), [message, self = shared_from_this()]() mutable {
        self->out_queue.push(message);
        self->await_writing_ws_message();
    });
}

void WebSocketSession::close() {
    shouldClose = true;
    enqueue_message(std::string(""));
}

boost::asio::const_buffer WebSocketSession::to_asio_buffer(const WebSocketData& message) {
    return std::visit([](const auto& arg) { return boost::asio::buffer(arg); }, message);
}

void WebSocketSession::on_run() {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Session started\n");
#endif
    ws.set_option(beast::websocket::stream_base::timeout::suggested((beast::role_type::server)));

    ws.async_accept(beast::bind_front_handler(&WebSocketSession::on_accept, shared_from_this()));
}

void WebSocketSession::on_accept(beast::error_code ec) {
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

void WebSocketSession::await_receiving_ws_message() {
    ws.async_read(buffer,
                  beast::bind_front_handler(&WebSocketSession::consume_received_ws_message, shared_from_this()));
}

void WebSocketSession::consume_received_ws_message(beast::error_code ec, std::size_t bytes_transferred) {
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

void WebSocketSession::await_writing_ws_message() {
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

void WebSocketSession::on_ws_write(beast::error_code ec, std::size_t bytes_transferred) {
    isWriting = false;
    if (ec) {
        scprintf("Sending websocket message failed: %s", ec.message().c_str());
    }
    out_queue.pop();
    // do this loop until our queue is empty
    await_writing_ws_message();
}

void WebSocketSession::call_sclang_new_connection() {
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

void WebSocketSession::call_sclang_close() {
    VMGlobals* g = gMainVMGlobals;
    gLangMutex.lock();
    ++g->sp;
    SetObject(g->sp, getsym("WebSocketConnection")->u.classobj);
    ++g->sp;
    SetPtr(g->sp, ownAddress);
    runInterpreter(g, getsym("prConnectionDisconnect"), 2);
    gLangMutex.unlock();
}

void WebSocketSession::call_sclang_message_received(std::size_t bytes_transferred) {
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

PyrObject* WebSocketSession::create_sclang_connection() {
    VMGlobals* g = gMainVMGlobals;
    PyrObject* newConnection = instantiateObject(g->gc, getsym("WebSocketConnection")->u.classobj, 1, true, false);
    SetPtr(newConnection->slots, ownAddress);
    return newConnection;
}

PyrObject* WebSocketSession::get_string(int bytes_transferred) {
    VMGlobals* g = gMainVMGlobals;
    std::string string = beast::buffers_to_string(buffer.data());
    PyrObject* scString = (PyrObject*)newPyrString(g->gc, string.c_str(), 0, true);
    buffer.consume(bytes_transferred);
    return scString;
}

PyrObject* WebSocketSession::get_byte_array(int bytes_transferred) {
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

WebSocketListener::WebSocketListener(const std::shared_ptr<WebSocketThread>& web_socket_thread,
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

void WebSocketListener::run() {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Starting websocket listener...\n");
#endif
    doAccept();
}

void WebSocketListener::stop() {
    boost::system::error_code ec;
    mAcceptor.close(ec);
    if (ec) {
        scprintf("Could not close websocket: %s\n", ec.message().c_str());
    }
}

void WebSocketListener::doAccept() {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Starting websocket accept...\n");
#endif
    mAcceptor.async_accept(boost::asio::make_strand(mIoContext),
                           beast::bind_front_handler(&WebSocketListener::onAccept, shared_from_this()));
}

void WebSocketListener::onAccept(beast::error_code ec, boost::asio::ip::tcp::socket socket) {
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

WebSocketClient::WebSocketClient(boost::asio::io_context& io_context):
    ws_(boost::asio::make_strand(io_context)),
    resolver_(boost::asio::make_strand(io_context)) {}

void WebSocketClient::run(const std::string& host_, std::string& port) {
    host = host_;

    resolver_.async_resolve(host, port, beast::bind_front_handler(&WebSocketClient::on_resolve, shared_from_this()));
}

beast::error_code WebSocketClient::close_connection() {
    connected = false;
    beast::error_code ec;
    ws_.close(beast::websocket::close_code::normal, ec);
    return ec;
}

void WebSocketClient::enqueue_message(WebSocketData message) {
    // dispatch via asio to ensure thread safety
    boost::asio::dispatch(ws_.get_executor(), [message, self = shared_from_this()]() mutable {
        self->out_queue.push(message);
        self->await_writing_ws_message();
    });
}

void WebSocketClient::on_resolve(beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
    if (ec) {
        scprintf("Could not resolve host: %s\n", ec.message().c_str());
        return;
    }
    beast::get_lowest_layer(ws_).async_connect(
        results, beast::bind_front_handler(&WebSocketClient::on_connect, shared_from_this()));
}

void WebSocketClient::on_connect(beast::error_code ec,
                                 boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint) {
    if (ec) {
        scprintf("Could not connect to host: %s\n", ec.message().c_str());
        return;
    }
    // Set a decorator to change the User-Agent of the handshake
    ws_.set_option(beast::websocket::stream_base::decorator([](beast::websocket::request_type& req) {
        req.set(beast::http::field::user_agent, std::string("sclang websocket-client"));
    }));

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    host += ':' + std::to_string(endpoint.port());

    // Perform the websocket handshake
    ws_.async_handshake(host, "/", beast::bind_front_handler(&WebSocketClient::on_handshake, shared_from_this()));
}

void WebSocketClient::on_handshake(beast::error_code ec) {
    if (ec) {
        scprintf("Could not handshake: %s\n", ec.message().c_str());
    }
    connected = true;
    do_read();
}

void WebSocketClient::do_read() {
    ws_.async_read(buffer, beast::bind_front_handler(&WebSocketClient::on_read, shared_from_this()));
}

void WebSocketClient::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);
    if (ec) {
        if (ec == boost::asio::error::eof) {
            return;
        }
        if (ec == boost::asio::error::operation_aborted) {
            return;
        }
        scprintf("Failed to read websocket message: %s\n", ec.message().c_str());
        return;
    };

    auto message = convert_data(buffer, bytes_transferred, ws_.got_text());

    SC_Websocket_Lang::WebSocketClient::received_message(self, message);

    // continue consuming
    if (connected) {
        do_read();
    }
}

void WebSocketClient::await_writing_ws_message() {
    if (shouldClose) {
        // @todo make this async
        ws_.close("Goodbye from sclang");
        connected = false;
        return;
    }
    if (!isWriting && !out_queue.empty()) {
        isWriting = true;
        auto message = out_queue.front();

        ws_.text(std::holds_alternative<std::string>(message));
        ws_.async_write(boost::asio::buffer(WebSocketSession::to_asio_buffer(message)),
                        beast::bind_front_handler(&WebSocketClient::on_ws_write, shared_from_this()));
    }
}

void WebSocketClient::on_ws_write(beast::error_code ec, std::size_t bytes_transferred) {
    isWriting = false;
    if (ec) {
        scprintf("Failed to write websocket message: %s\n", ec.message().c_str());
    }
    out_queue.pop();
    await_writing_ws_message();
}
}
