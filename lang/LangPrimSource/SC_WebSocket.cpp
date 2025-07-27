#include <queue>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>

#include "SC_WebSocket.h"
#include "SC_WebSocketPrim.h"
#include "SC_WorldOptions.h"

#define SC_WEBSOCKET_DEBUG

namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

namespace SC_Websocket {

WebSocketData convertData(beast::flat_buffer& buffer, size_t bytesTransferred, bool isText) {
    WebSocketData data;
    if (isText) {
        data = beast::buffers_to_string(buffer.data());
    } else {
        auto array_data = std::vector<uint8_t>(bytesTransferred);
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

boost::asio::io_context& WebSocketThread::getContext() { return m_ioContext; }

void WebSocketThread::start() {
    if (!m_thread) {
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Start websocket thread\n");
#endif
        m_thread = std::make_shared<SC_Thread>([this]() {
            auto work = boost::asio::make_work_guard(m_ioContext);
            m_ioContext.run();
        });
    }
}

void WebSocketThread::stop() {
    m_ioContext.stop();
    if (m_thread && m_thread->joinable()) {
        m_thread->join();
    }
    m_thread.reset();
}

WebSocketThread::~WebSocketThread() {
    scprintf("Clean websocket thread\n");
    stop();
}

WebSocketThread::WebSocketThread() {}

WebSocketSession::WebSocketSession(boost::asio::ip::tcp::socket&& socket, int listeningPort):
    m_ws(std::move(socket)),
    m_listeningPort(listeningPort) {}

void WebSocketSession::run() {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Start run of session\n");
#endif
    boost::asio::dispatch(m_ws.get_executor(), beast::bind_front_handler(&WebSocketSession::onRun, shared_from_this()));
}

void WebSocketSession::enqueueMessage(WebSocketData message) {
    // dispatch via asio to ensure thread safety
    boost::asio::dispatch(m_ws.get_executor(), [message, self = shared_from_this()]() mutable {
        self->m_outQueue.push(message);
        self->doWrite();
    });
}

void WebSocketSession::close() { m_ws.close("Goodbye"); }

boost::asio::const_buffer WebSocketSession::toAsioBuffer(const WebSocketData& message) {
    return std::visit([](const auto& arg) { return boost::asio::buffer(arg); }, message);
}

void WebSocketSession::onRun() {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Session started\n");
#endif
    m_ws.set_option(beast::websocket::stream_base::timeout::suggested((beast::role_type::server)));

    m_ws.async_accept(beast::bind_front_handler(&WebSocketSession::onAccept, shared_from_this()));
}

void WebSocketSession::onAccept(beast::error_code ec) {
    if (ec) {
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Session accept error: %s\n", ec.message().c_str());
#endif
        return;
    }
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Session accepted\n");
#endif

    SC_Websocket_Lang::WebSocketConnection::newLangConnection(m_ownAddress, m_listeningPort);
    doRead();
}

void WebSocketSession::doRead() {
    m_ws.async_read(m_buffer, beast::bind_front_handler(&WebSocketSession::onRead, shared_from_this()));
}

void WebSocketSession::onRead(beast::error_code ec, std::size_t bytesTransferred) {
    if (ec) {
        if (ec == boost::asio::error::eof || ec == beast::websocket::error::closed
            || ec == boost::asio::error::operation_aborted) {
#ifdef SC_WEBSOCKET_DEBUG
            scprintf("Session closed\n");
#endif
        } else {
            scprintf("Websocket connection error: %s\n", ec.message().c_str());
        };
        SC_Websocket_Lang::WebSocketConnection::closeLangConnection(m_ownAddress);
        return;
    }
    auto message = convertData(m_buffer, bytesTransferred, m_ws.got_text());
    SC_Websocket_Lang::WebSocketConnection::receiveLangMessage(m_ownAddress, message);
    // continue async loop to await websocket message
    doRead();
}

void WebSocketSession::doWrite() {
    if (!m_isWriting && !m_outQueue.empty()) {
        m_isWriting = true;
        auto message = m_outQueue.front();

        // if a string, indicate it as a text message
        m_ws.text(std::holds_alternative<std::string>(message));

        m_ws.async_write(boost::asio::buffer(toAsioBuffer(message)),
                         boost::beast::bind_front_handler(&WebSocketSession::onWrite, shared_from_this()));
    }
}

void WebSocketSession::onWrite(beast::error_code ec, std::size_t bytesTransferred) {
    m_isWriting = false;
    if (ec) {
        scprintf("Sending websocket message failed: %s", ec.message().c_str());
    }
    m_outQueue.pop();
    // do this loop until our queue is empty
    doWrite();
}

WebSocketListener::WebSocketListener(const std::shared_ptr<WebSocketThread>& webSocketThread,
                                     boost::asio::ip::tcp::endpoint endpoint, boost::beast::error_code& ec):
    m_ioContext(webSocketThread->getContext()),
    m_acceptor(webSocketThread->getContext()),
    m_thread(webSocketThread) {
    m_acceptor.open(endpoint.protocol(), ec);
    if (ec) {
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Could not open endpoint - %s\n", ec.message().c_str());
#endif
        return;
    }

    m_acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
    if (ec) {
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Could not set reuse address: %s\n", ec.message().c_str());
#endif
        return;
    }

    m_acceptor.bind(endpoint, ec);
    if (ec) {
#ifdef SC_WEBSOCKET_DEBUG
        scprintf("Could not bind to endpoint: %s\n", ec.message().c_str());
#endif
        return;
    }

    m_acceptor.listen(boost::asio::socket_base::max_listen_connections, ec);
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
    m_acceptor.close(ec);
    if (ec) {
        scprintf("Could not close websocket: %s\n", ec.message().c_str());
    }
}

void WebSocketListener::doAccept() {
#ifdef SC_WEBSOCKET_DEBUG
    scprintf("Starting websocket accept...\n");
#endif
    m_acceptor.async_accept(boost::asio::make_strand(m_ioContext),
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
    auto session = std::make_shared<WebSocketSession>(std::move(socket), m_acceptor.local_endpoint().port());
    session->m_ownAddress = session.get();
    session->run();
    doAccept();
}

WebSocketClient::WebSocketClient(boost::asio::io_context& ioContext):
    m_ws(boost::asio::make_strand(ioContext)),
    m_resolver(boost::asio::make_strand(ioContext)) {}

void WebSocketClient::run(const std::string& host_, std::string& port) {
    m_host = host_;

    m_resolver.async_resolve(m_host, port, beast::bind_front_handler(&WebSocketClient::onResolve, shared_from_this()));
}

beast::error_code WebSocketClient::closeConnection() {
    beast::error_code ec;
    m_ws.close(beast::websocket::close_code::normal, ec);
    return ec;
}

void WebSocketClient::enqueueMessage(WebSocketData message) {
    // dispatch via asio to ensure thread safety
    boost::asio::dispatch(m_ws.get_executor(), [message, self = shared_from_this()]() mutable {
        self->m_outQueue.push(message);
        self->doWrite();
    });
}

void WebSocketClient::onResolve(beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results) {
    if (ec) {
        scprintf("Could not resolve host: %s\n", ec.message().c_str());
        return;
    }
    beast::get_lowest_layer(m_ws).async_connect(
        results, beast::bind_front_handler(&WebSocketClient::onConnect, shared_from_this()));
}

void WebSocketClient::onConnect(beast::error_code ec,
                                boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint) {
    if (ec) {
        scprintf("Could not connect to host: %s\n", ec.message().c_str());
        return;
    }
    // Set a decorator to change the User-Agent of the handshake
    m_ws.set_option(beast::websocket::stream_base::decorator([](beast::websocket::request_type& req) {
        req.set(beast::http::field::user_agent, std::string("sclang websocket-client"));
    }));

    // Update the host_ string. This will provide the value of the
    // Host HTTP header during the WebSocket handshake.
    // See https://tools.ietf.org/html/rfc7230#section-5.4
    m_host += ':' + std::to_string(endpoint.port());

    // Perform the websocket handshake
    m_ws.async_handshake(m_host, "/", beast::bind_front_handler(&WebSocketClient::onHandshake, shared_from_this()));
}

void WebSocketClient::onHandshake(beast::error_code ec) {
    if (ec) {
        scprintf("Could not handshake: %s\n", ec.message().c_str());
    }
    m_connected = true;
    SC_Websocket_Lang::WebSocketClient::setConnectionStatus(m_self, true);
    doRead();
}

void WebSocketClient::doRead() {
    m_ws.async_read(m_buffer, beast::bind_front_handler(&WebSocketClient::onRead, shared_from_this()));
}

void WebSocketClient::onRead(beast::error_code ec, std::size_t bytesTransferred) {
    if (ec) {
        m_connected = false;
        SC_Websocket_Lang::WebSocketClient::setConnectionStatus(m_self, false);
        if (ec == boost::system::errc::operation_canceled || ec == boost::asio::error::eof) {
            return;
        }
        scprintf("Failed to read websocket message: %s\n", ec.message().c_str());
        return;
    };

    auto message = convertData(m_buffer, bytesTransferred, m_ws.got_text());

    SC_Websocket_Lang::WebSocketClient::receivedMessage(m_self, message);

    doRead();
}

void WebSocketClient::doWrite() {
    if (!m_connected) {
        scprintf("WebSocket client is not connected - can not send out message\n");
        return;
    }
    if (!m_isWriting && !m_outQueue.empty()) {
        m_isWriting = true;
        auto message = m_outQueue.front();

        m_ws.text(std::holds_alternative<std::string>(message));
        m_ws.async_write(boost::asio::buffer(WebSocketSession::toAsioBuffer(message)),
                         beast::bind_front_handler(&WebSocketClient::onWrite, shared_from_this()));
    }
}

void WebSocketClient::onWrite(beast::error_code ec, std::size_t bytesTransferred) {
    m_isWriting = false;
    if (ec) {
        scprintf("Failed to write websocket message: %s\n", ec.message().c_str());
    }
    m_outQueue.pop();
    doWrite();
}
}
