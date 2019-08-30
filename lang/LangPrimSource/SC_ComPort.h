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

#include "SC_Types.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

//////////////////////////////////////////////////////////////////////////////////////////////////////////

const int kTextBufSize = 65536;

class SC_UdpInPort {
    int mPortNum;
    boost::array<char, kTextBufSize> recvBuffer;

    boost::asio::ip::udp::endpoint remoteEndpoint;

    void handleReceivedUDP(const boost::system::error_code& error, std::size_t bytes_transferred);

    void startReceiveUDP();

public:
    boost::asio::ip::udp::socket udpSocket;

    int RealPortNum() const { return mPortNum; }
    boost::asio::ip::udp::socket& Socket() { return udpSocket; }

    SC_UdpInPort(int inPortNum, int portsToCheck = 10);
    ~SC_UdpInPort();
};

class SC_UdpCustomInPort : public SC_UdpInPort {
public:
    SC_UdpCustomInPort(int inPortNum);
    ~SC_UdpCustomInPort();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_TcpConnection : public boost::enable_shared_from_this<SC_TcpConnection> {
public:
    typedef boost::shared_ptr<SC_TcpConnection> pointer;
    boost::asio::ip::tcp::socket socket;

    SC_TcpConnection(boost::asio::io_service& ioService, class SC_TcpInPort* parent):
        socket(ioService),
        mParent(parent) {}

    void start();

private:
    int32 OSCMsgLength;
    char* data;
    class SC_TcpInPort* mParent;

    void handleLengthReceived(const boost::system::error_code& error, size_t bytes_transferred);

    void handleMsgReceived(const boost::system::error_code& error, size_t bytes_transferred);
};

class SC_TcpInPort {
    boost::asio::ip::tcp::acceptor acceptor;

public:
    const int mPortNum;

    SC_TcpInPort(int inPortNum, int inMaxConnections, int inBacklog);

    void startAccept();
    void handleAccept(SC_TcpConnection::pointer new_connection, const boost::system::error_code& error);
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////


class SC_TcpClientPort {
public:
    typedef void (*ClientNotifyFunc)(void* clientData);

public:
    SC_TcpClientPort(unsigned long inAddress, int inPort, ClientNotifyFunc notifyFunc = 0, void* clientData = 0);
    int Close();

    boost::asio::ip::tcp::socket& Socket() { return socket; }

private:
    int32 OSCMsgLength;
    char* data;

    void startReceive();
    void handleLengthReceived(const boost::system::error_code& error, size_t bytes_transferred);

    void handleMsgReceived(const boost::system::error_code& error, size_t bytes_transferred);

    boost::asio::ip::tcp::socket socket;
    boost::asio::ip::tcp::endpoint endpoint;

    ClientNotifyFunc mClientNotifyFunc;
    void* mClientData;
};
