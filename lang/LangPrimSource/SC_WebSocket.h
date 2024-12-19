#pragma once

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>

using tcp = boost::asio::ip::tcp;

class SC_WebSocket {
    SC_WebSocket(tcp::socket socket);
    void start_consuming();

private:
    tcp::socket socket_;
};
