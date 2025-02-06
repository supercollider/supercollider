#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "SC_WebSocketPrim.h"
#include <PyrInterpreter.h>

namespace beast = boost::beast;
using tcp = boost::asio::ip::tcp;

namespace SC_Websocket {
typedef std::variant<std::vector<uint8_t>, std::string> WebSocketData;
// a websocket message can either be a byte array or a string
// see https://developer.mozilla.org/en-US/docs/Web/API/WebSocket/message_event

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

    boost::asio::io_context& get_context();

    void start();

    void stop();

    ~WebSocketThread();

    //@todo how to make this private?
    WebSocketThread();

private:
    boost::asio::io_context io_context;
    std::shared_ptr<SC_Thread> thread;
};

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
    explicit WebSocketSession(boost::asio::ip::tcp::socket&& socket, int listeningPort);

    // starts consumption of a websocket connection
    void run();

    void enqueue_message(WebSocketData message);

    /**
     * The connection gets closed by setting the member variable `shouldClose`
     * to true and trigger an empty message which will release the connection by
     * not extending its lifetime.
     */
    void close();

    // converter between WebSocketData and beast flat_buffer
    static boost::asio::const_buffer to_asio_buffer(const WebSocketData& message);

private:
    void on_run();

    void on_accept(beast::error_code ec);

    // aka do_read in boost
    void await_receiving_ws_message();

    // consume a received websocket message - aka on_read in boost
    void consume_received_ws_message(beast::error_code ec, std::size_t bytes_transferred);

    // consume out queue until empty - aka do_write on beast
    void await_writing_ws_message(void);

    // aka on_write
    void on_ws_write(beast::error_code ec, std::size_t bytes_transferred);

    // sclang parts
    void call_sclang_new_connection();

    // informs sclang that our websocket connection is now closed
    void call_sclang_close();

    void call_sclang_message_received(std::size_t bytes_transferred);

    PyrObject* create_sclang_connection();

    PyrObject* get_string(int bytes_transferred);

    PyrObject* get_byte_array(int bytes_transferred);
};

// acts as a server which listens for incoming connections
class WebSocketListener : public std::enable_shared_from_this<WebSocketListener> {
    boost::asio::io_context& mIoContext;
    boost::asio::ip::tcp::acceptor mAcceptor;
    std::shared_ptr<WebSocketThread> mThread;

public:
    // take ownership shared ptr of our web socket thread so we maintain the lifetime of the thread
    WebSocketListener(const std::shared_ptr<WebSocketThread>& web_socket_thread,
                      boost::asio::ip::tcp::endpoint endpoint, boost::beast::error_code& ec);

    void run();

    void stop();

private:
    void doAccept();

    void onAccept(beast::error_code ec, boost::asio::ip::tcp::socket socket);
};

class WebSocketClient : public std::enable_shared_from_this<WebSocketClient> {
    beast::websocket::stream<beast::tcp_stream> ws_;
    boost::asio::ip::tcp::resolver resolver_;
    beast::flat_buffer buffer;
    std::string host;
    bool connected = false;
    bool isWriting = false;
    bool shouldClose = false;
    std::queue<WebSocketData> out_queue;

public:
    // access to self - acts as identifier
    WebSocketClient* self;

    WebSocketClient(boost::asio::io_context& io_context);
    ;

    // @todo why is port a string?
    void run(const std::string& host_, std::string& port);

    void send_message(WebSocketData message) {
        // Send the message
        /*
        ws.async_write(
            net::buffer(message),
            beast::bind_front_handler(
                &WebSocketClient::on_write,
                shared_from_this()));

         */
    }

    beast::error_code close_connection();

    void enqueue_message(WebSocketData message);

private:
    void on_resolve(beast::error_code ec, boost::asio::ip::tcp::resolver::results_type results);

    void on_connect(beast::error_code ec, boost::asio::ip::tcp::resolver::results_type::endpoint_type endpoint);

    void on_handshake(beast::error_code ec);

    void do_read();

    void on_read(beast::error_code ec, std::size_t bytes_transferred);

    void on_close() { connected = false; }

    void await_writing_ws_message();
    ;

    void on_ws_write(beast::error_code ec, std::size_t bytes_transferred);
};
}
