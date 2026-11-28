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

#include <boost/asio.hpp>

#include <array>
#include <string>
#include <unordered_set>

#ifdef USE_RENDEZVOUS
#    include <thread>
#endif

struct World;


class SC_UdpInPort {
public:
    SC_UdpInPort(World* world, std::string_view bindTo, int inPortNum);
    ~SC_UdpInPort();

    SC_UdpInPort(const SC_UdpInPort&) = delete;
    SC_UdpInPort(SC_UdpInPort&&) = delete;

    //! always call on the ASIO thread!
    void close();

private:
    static constexpr size_t kTextBufSize = 65536;
    static constexpr int receiveBufferSize = 4 * 1024 * 1024;
    static constexpr int sendBufferSize = 4 * 1024 * 1024;
    static constexpr int fallbackBufferSize = 1 * 1024 * 1024;

    void startReceiveUDP();
    void handleReceivedUDP(const boost::system::error_code& error, std::size_t bytesTransferred);

    World* mWorld;
    int mPortNum;
    std::string mbindTo;
    std::array<char, kTextBufSize> mRecvBuffer;
    boost::asio::ip::udp::socket mSocket;
    boost::asio::ip::udp::endpoint mRemoteEndpoint;
#ifdef USE_RENDEZVOUS
    std::thread mRendezvousThread;
#endif
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_TcpInPort;

class SC_TcpConnection : public std::enable_shared_from_this<SC_TcpConnection> {
public:
    using pointer = std::shared_ptr<SC_TcpConnection>;
    using TcpSocket = boost::asio::ip::tcp::socket;

    SC_TcpConnection(World* world, boost::asio::io_context& ioContext, SC_TcpInPort* parent);
    ~SC_TcpConnection();

    SC_TcpConnection(const SC_TcpConnection&) = delete;
    SC_TcpConnection(SC_TcpConnection&&) = delete;

    //! called on the ASIO thread by SC_TcpInPport
    bool start();

    //! always call on the ASIO thread!
    void close();

    TcpSocket& socket() { return mSocket; }
    const ReplyAddress& replyAddress() const { return mReplyAddress; }

private:
    static constexpr size_t kMaxPasswordLen = 32;

    void startReceiveMessage();
    void handleLengthReceived(const boost::system::error_code& error, size_t bytesTransferred);
    void handleMsgReceived(const boost::system::error_code& error, size_t bytesTransferred);
    void doClose();

    World* mWorld;
    SC_TcpInPort* mParent;
    TcpSocket mSocket;
    int32_t mOSCMsgLength = 0;
    std::vector<char> mOSCMsgData;
    //! acts as the identification within SC world, which is necessary when unregistering upon disconnect.
    ReplyAddress mReplyAddress;
};

class SC_TcpInPort {
public:
    SC_TcpInPort(World* world, std::string_view bindTo, int inPortNum, int inMaxConnections, int inBacklog);
    ~SC_TcpInPort();

    SC_TcpInPort(const SC_TcpInPort&) = delete;
    SC_TcpInPort(SC_TcpInPort&&) = delete;

    //! always call on the ASIO thread!
    void close();

    //! called on the ASIO thread by SC_TcpConnection
    void connectionClosed(SC_TcpConnection::pointer connection);

private:
    void startAccept();
    void handleAccept(SC_TcpConnection::pointer newConnection, const boost::system::error_code& error);

    World* mWorld;
    boost::asio::ip::tcp::acceptor mAcceptor;
    size_t mMaxNumConnections;
    std::unordered_set<SC_TcpConnection::pointer> mConnections;
#ifdef USE_RENDEZVOUS
    std::thread mRendezvousThread;
#endif
};
