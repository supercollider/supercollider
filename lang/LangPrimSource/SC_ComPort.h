/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once

#include "SC_ReplyImpl.hpp"
#include "SC_Types.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/core/noncopyable.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////////////////

const int kTextBufSize = 65536;

enum class HandlerType { OSC, Raw };

using HandleDataFunc = std::function<void(std::unique_ptr<char[]>, size_t)>;

//////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Detail {

class TCPConnection : public std::enable_shared_from_this<TCPConnection>, private boost::noncopyable {
public:
    using pointer = std::shared_ptr<TCPConnection>;

    TCPConnection(boost::asio::io_context& ioContext, int portNum, HandlerType);

    void start();
    auto& getSocket() { return mSocket; }

private:
    void handleLengthReceived(const boost::system::error_code& error, size_t bytes_transferred);
    void handleMsgReceived(const boost::system::error_code& error, size_t bytes_transferred);
    void initHandler(HandlerType);

    HandleDataFunc mHandleFunc;
    boost::asio::ip::tcp::socket mSocket;
    int32 mOSCMsgLength;
    std::unique_ptr<char[]> mData;
    const int mPortNum;
};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace InPort {

class UDP : private boost::noncopyable {
public:
    UDP(int inPortNum, HandlerType, int portsToCheck = 10);
    ~UDP() = default;

    auto RealPortNum() const { return mPortNum; }
    auto& getSocket() { return mUdpSocket; }

private:
    void initHandler(HandlerType type);

    void handleReceivedUDP(const boost::system::error_code& error, std::size_t bytes_transferred);
    void startReceiveUDP();

    int mPortNum;
    HandleDataFunc mHandleFunc;
    static constexpr int receiveBufferSize = 8 * 1024 * 1024;
    static constexpr int sendBufferSize = 8 * 1024 * 1024;
    std::array<char, kTextBufSize> mRecvBuffer;
    boost::asio::ip::udp::endpoint mRemoteEndpoint;
    boost::asio::ip::udp::socket mUdpSocket;
};

class UDPCustom : public UDP {
public:
    UDPCustom(int inPortNum, HandlerType);
    ~UDPCustom() = default;
};

class TCP : private boost::noncopyable {
public:
    TCP(int inPortNum, int inMaxConnections, int inBacklog, HandlerType);

private:
    void startAccept();
    void handleAccept(Detail::TCPConnection::pointer new_connection, const boost::system::error_code& error);

    HandleDataFunc mHandleFunc;
    const HandlerType mHandlerType;
    const int mPortNum;
    boost::asio::ip::tcp::acceptor mAcceptor;
};

} // namespace InPort

//////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace OutPort {

class TCP : private boost::noncopyable {
public:
    typedef void (*ClientNotifyFunc)(void* clientData);

public:
    TCP(std::uint64_t inAddress, int inPort, HandlerType, ClientNotifyFunc notifyFunc = 0, void* clientData = 0);
    int Close();

    boost::asio::ip::tcp::socket& Socket() { return mSocket; }

private:
    void startReceive();
    void handleLengthReceived(const boost::system::error_code& error, size_t bytes_transferred);
    void handleMsgReceived(const boost::system::error_code& error, size_t bytes_transferred);
    void initHandler(HandlerType);

    HandleDataFunc mHandleFunc;
    int32 mOSCMsgLength;
    std::unique_ptr<char[]> mData;
    boost::asio::ip::tcp::socket mSocket;
    boost::asio::ip::tcp::endpoint mEndpoint;
    ClientNotifyFunc mClientNotifyFunc;
    void* mClientData;
};

} // namespace OutPort
