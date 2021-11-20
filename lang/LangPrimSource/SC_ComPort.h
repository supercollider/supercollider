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

//////////////////////////////////////////////////////////////////////////////////////////////////////////

const int kTextBufSize = 65536;

using HandleDataFunc = std::function<void(std::unique_ptr<char[]>, size_t)>;

enum class HandlerType { OSC };

// One MessageHandler<> class for each HandlerType.
// The signature of handleMessage can differ between handler types, so templatizing the class
// allows us to specify a message handler func using e.g. MessageHandler<HandlerType::OSC>::handleMessage
template <HandlerType H> struct MessageHandler { };
template <> struct MessageHandler<HandlerType::OSC> {
    static void handleMessage(Protocol protocol, int replySocket, const boost::asio::ip::address& replyAddress,
                              int replyPort, std::unique_ptr<char[]> data, size_t dataSize, int localPort);
};

class SC_UdpInPort {
public:
    SC_UdpInPort(int inPortNum, HandlerType, int portsToCheck = 10);
    ~SC_UdpInPort() = default;

    auto RealPortNum() const { return mPortNum; }
    auto& getSocket() { return mUdpSocket; }

private:
    void initHandler(HandlerType type);

    void handleReceivedUDP(const boost::system::error_code& error, std::size_t bytes_transferred);
    void startReceiveUDP();

    int mPortNum;
    HandleDataFunc mHandleFunc;
    boost::array<char, kTextBufSize> mRecvBuffer;
    boost::asio::ip::udp::endpoint mRemoteEndpoint;
    boost::asio::ip::udp::socket mUdpSocket;
};

class SC_UdpCustomInPort : public SC_UdpInPort {
public:
    SC_UdpCustomInPort(int inPortNum, HandlerType);
    ~SC_UdpCustomInPort() = default;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////


class SC_TcpConnection : public std::enable_shared_from_this<SC_TcpConnection> {
public:
    using pointer = std::shared_ptr<SC_TcpConnection>;

    SC_TcpConnection(boost::asio::io_service& ioService, int portNum, HandlerType);

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

class SC_TcpInPort {
public:
    SC_TcpInPort(int inPortNum, int inMaxConnections, int inBacklog, HandlerType);

    void startAccept();
    void handleAccept(SC_TcpConnection::pointer new_connection, const boost::system::error_code& error);

private:
    HandleDataFunc mHandleFunc;
    const HandlerType mHandlerType;
    const int mPortNum;
    boost::asio::ip::tcp::acceptor mAcceptor;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////


class SC_TcpClientPort {
public:
    typedef void (*ClientNotifyFunc)(void* clientData);

public:
    SC_TcpClientPort(unsigned long inAddress, int inPort, HandlerType, ClientNotifyFunc notifyFunc = 0,
                     void* clientData = 0);
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
