#pragma once

#include <queue>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "SC_WebSocketPrim.h"
#include <SC_Lock.h>

namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

// websocket server implementation using boost beast
// communication with primitives is handeled in `SC_WebSocketPrim`.
// See boost beast documentation for `do_read` -> `on_read` worklflow.
namespace SC_Websocket {

// a websocket message can either be a byte array or a string
// see https://developer.mozilla.org/en-US/docs/Web/API/WebSocket/message_event
typedef std::variant<std::vector<uint8_t>, std::string> WebSocketData;

/** A wrapper class for the websocket communication thread.
This gets initiated into a static variable upon request.
Due to this static lifetime the singleton only gets deleted when sclang closes.
*/
class WebSocketThread {
public:
    static std::shared_ptr<WebSocketThread> getInstance();

    // Deleted copy/move constructors
    WebSocketThread(const WebSocketThread&) = delete;
    WebSocketThread& operator=(const WebSocketThread&) = delete;
    WebSocketThread(WebSocketThread&&) = delete;
    WebSocketThread& operator=(WebSocketThread&&) = delete;

    boost::asio::io_context& getContext();

    void start();

    void stop();

    ~WebSocketThread();

    // how to make this private?
    WebSocketThread();

private:
    boost::asio::io_context m_ioContext;
    std::shared_ptr<SC_Thread> m_thread;
};

// A WebSocketSession is essentially a websocket connection from our WebSocket server.
// This gets build by the WebSocketListener.
class WebSocketSession : public std::enable_shared_from_this<WebSocketSession> {
    beast::websocket::stream<tcp::socket> m_ws;
    beast::flat_buffer m_buffer;
    std::queue<WebSocketData> m_outQueue;
    // a primitive mutex - see `do_write` and `on_write`
    bool m_isWriting = false;
    int m_listeningPort;

public:
    // we store a reference pointer to ourselves upon creation
    // this pointer acts as an identifier on the sclang side,
    // so it is possible to distinguish connections and we can
    // call the callback on the matching sclang WebSocketConnection
    WebSocketSession* m_ownAddress;

    // take ownership of socket
    explicit WebSocketSession(boost::asio::ip::tcp::socket&& socket, int listeningPort);

    void run();

    void enqueueMessage(WebSocketData message);

    void close();

    // converts WebSocketData to beast flat_buffer
    static boost::asio::const_buffer toAsioBuffer(const WebSocketData& message);

private:
    void onRun();

    void onAccept(beast::error_code ec);

    void doRead();

    void onRead(beast::error_code ec, std::size_t bytesTransferred);

    void doWrite(void);

    void onWrite(beast::error_code ec, std::size_t bytesTransferred);
};

// acts as a server which listens for incoming connections
class WebSocketListener : public std::enable_shared_from_this<WebSocketListener> {
    boost::asio::io_context& m_ioContext;
    boost::asio::ip::tcp::acceptor m_acceptor;
    std::shared_ptr<WebSocketThread> m_thread;

public:
    // take ownership shared ptr of our web socket thread so we maintain the lifetime of the thread
    WebSocketListener(const std::shared_ptr<WebSocketThread>& webSocketThread, boost::asio::ip::tcp::endpoint endpoint,
                      boost::beast::error_code& ec);

    void run();

    void stop();

private:
    void doAccept();

    void onAccept(beast::error_code ec, boost::asio::ip::tcp::socket socket);
};

class WebSocketClient : public std::enable_shared_from_this<WebSocketClient> {
    beast::websocket::stream<beast::tcp_stream> m_ws;
    boost::asio::ip::tcp::resolver m_resolver;
    beast::flat_buffer m_buffer;
    std::string m_host;
    bool m_connected = false;
    bool m_isWriting = false;
    std::queue<WebSocketData> m_outQueue;

public:
    // access to self - acts as identifier for sclang callbacks
    WebSocketClient* m_self;

    WebSocketClient(boost::asio::io_context& ioContext);

    // why is port a string?!
    void run(const std::string& host_, std::string& port);

    beast::error_code closeConnection();

    void enqueueMessage(WebSocketData message);

private:
    void onResolve(beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);

    void onConnect(beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint);

    void onHandshake(beast::error_code ec);

    void doRead();

    void onRead(beast::error_code ec, std::size_t bytesTransferred);

    void onClose() { m_connected = false; }

    void doWrite();

    void onWrite(beast::error_code ec, std::size_t bytesTransferred);
};
}
