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

#include "SC_BoundsMacros.h"
#include "SC_ComPort.h"
#include "SC_Endian.h"
#include <ctype.h>
#include <stdexcept>
#include <stdarg.h>
#include "SCBase.h"
#include <cerrno>
#include "PyrSched.h"

#include "SC_Lock.h"
#include "SC_Msg.h"
#include "SC_ReplyImpl.hpp"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/typeof/typeof.hpp>

#define scprintf printf
#include "SC_OscUtils.hpp"
#undef scprintf

void ProcessOSCPacket(OSC_Packet* inPacket, int inPortNum, double time);

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_Thread gAsioThread;
boost::asio::io_service ioService;


static void asioFunction() {
    boost::asio::io_service::work work(ioService);
    ioService.run();
}

void startAsioThread() {
    SC_Thread asioThread(&asioFunction);
    gAsioThread = std::move(asioThread);
}

void stopAsioThread() {
    ioService.stop();
    gAsioThread.join();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_UdpInPort::SC_UdpInPort(int inPortNum, int portsToCheck): mPortNum(inPortNum), udpSocket(ioService) {
    using namespace boost::asio;

    BOOST_AUTO(protocol, ip::udp::v4());

    udpSocket.open(protocol);

    for (int offset = 0; offset != portsToCheck; ++offset) {
        try {
            udpSocket.bind(ip::udp::endpoint(protocol, inPortNum + offset));
            mPortNum = inPortNum + offset;
            break;
        } catch (std::exception const&) {
            if (offset == (portsToCheck - 1))
                throw std::runtime_error("unable to bind udp socket\n");
        }
    }

    boost::asio::socket_base::send_buffer_size option(65536);
    udpSocket.set_option(option);

    startReceiveUDP();
}

SC_UdpInPort::~SC_UdpInPort() {}


void SC_UdpInPort::startReceiveUDP() {
    using namespace boost;
    udpSocket.async_receive_from(asio::buffer(recvBuffer), remoteEndpoint,
                                 boost::bind(&SC_UdpInPort::handleReceivedUDP, this, asio::placeholders::error,
                                             asio::placeholders::bytes_transferred));
}

void SC_UdpInPort::handleReceivedUDP(const boost::system::error_code& error, std::size_t bytesTransferred) {
    double timeReceived = elapsedTime(); // get time now to minimize jitter due to lang load
    if (error == boost::asio::error::operation_aborted)
        return; /* we're done */

    if (error == boost::asio::error::connection_refused) {
        // avoid windows error message
        startReceiveUDP();
        return;
    }

    if (error) {
        printf("SC_UdpInPort: received error - %s\n", error.message().c_str());
        startReceiveUDP();
        return;
    }

    OSC_Packet* packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));

    packet->mReplyAddr.mProtocol = kUDP;
    packet->mReplyAddr.mAddress = remoteEndpoint.address();
    packet->mReplyAddr.mPort = remoteEndpoint.port();
    packet->mReplyAddr.mSocket = udpSocket.native_handle();

    char* data = (char*)malloc(bytesTransferred);
    packet->mSize = bytesTransferred;
    packet->mData = data;
    memcpy(data, recvBuffer.data(), bytesTransferred);

    ProcessOSCPacket(packet, mPortNum, timeReceived);
    startReceiveUDP();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_UdpCustomInPort::SC_UdpCustomInPort(int inPortNum): SC_UdpInPort(inPortNum, 1) {}

SC_UdpCustomInPort::~SC_UdpCustomInPort() {}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_TcpInPort::SC_TcpInPort(int inPortNum, int inMaxConnections, int inBacklog):
    acceptor(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), inPortNum)),
    mPortNum(inPortNum) {
    // FIXME: handle max connections
    // FIXME: backlog???

    startAccept();
}

void SC_TcpInPort::startAccept() {
    SC_TcpConnection::pointer newConnection(new SC_TcpConnection(ioService, this));

    acceptor.async_accept(
        newConnection->socket,
        boost::bind(&SC_TcpInPort::handleAccept, this, newConnection, boost::asio::placeholders::error));
}

void SC_TcpInPort::handleAccept(SC_TcpConnection::pointer newConnection, const boost::system::error_code& error) {
    if (!error)
        newConnection->start();
    startAccept();
}

void SC_TcpConnection::start() {
    namespace ba = boost::asio;
    ba::async_read(socket, ba::buffer(&OSCMsgLength, sizeof(OSCMsgLength)),
                   boost::bind(&SC_TcpConnection::handleLengthReceived, shared_from_this(), ba::placeholders::error,
                               ba::placeholders::bytes_transferred));
}

void SC_TcpConnection::handleLengthReceived(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error)
        return;

    namespace ba = boost::asio;
    // msglen is in network byte order
    OSCMsgLength = sc_ntohl(OSCMsgLength);

    data = (char*)malloc(OSCMsgLength);

    ba::async_read(socket, ba::buffer(data, OSCMsgLength),
                   boost::bind(&SC_TcpConnection::handleMsgReceived, shared_from_this(), ba::placeholders::error,
                               ba::placeholders::bytes_transferred));
}

void SC_TcpConnection::handleMsgReceived(const boost::system::error_code& error, size_t bytes_transferred) {
    double timeReceived = elapsedTime(); // get time now to minimize jitter due to lang load
    if (error) {
        free(data);
        return;
    }

    assert(bytes_transferred == OSCMsgLength);

    OSC_Packet* packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));

    packet->mReplyAddr.mProtocol = kTCP;
    packet->mReplyAddr.mSocket = socket.native_handle();
    packet->mSize = OSCMsgLength;
    packet->mData = data;

    ProcessOSCPacket(packet, mParent->mPortNum, timeReceived);

    start();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_TcpClientPort::SC_TcpClientPort(unsigned long inAddress, int inPort, ClientNotifyFunc notifyFunc, void* clientData):
    socket(ioService),
    endpoint(boost::asio::ip::address_v4(inAddress), inPort),
    mClientNotifyFunc(notifyFunc),
    mClientData(clientData) {
    using namespace boost::asio;

    boost::system::error_code error;
    ip::tcp::no_delay noDelayOption(true);
    socket.set_option(noDelayOption, error);

    socket.connect(endpoint);

    startReceive();
}

void SC_TcpClientPort::startReceive() {
    namespace ba = boost::asio;
    ba::async_read(socket, ba::buffer(&OSCMsgLength, sizeof(OSCMsgLength)),
                   boost::bind(&SC_TcpClientPort::handleLengthReceived, this, ba::placeholders::error,
                               ba::placeholders::bytes_transferred));
}

void SC_TcpClientPort::handleLengthReceived(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error == boost::asio::error::connection_aborted) {
        if (mClientNotifyFunc)
            (*mClientNotifyFunc)(mClientData);
    }

    if (error)
        return;

    // msglen is in network byte order
    OSCMsgLength = sc_ntohl(OSCMsgLength);
    data = (char*)malloc(OSCMsgLength);

    namespace ba = boost::asio;
    ba::async_read(socket, ba::buffer(data, OSCMsgLength),
                   boost::bind(&SC_TcpClientPort::handleMsgReceived, this, ba::placeholders::error,
                               ba::placeholders::bytes_transferred));
}

void SC_TcpClientPort::handleMsgReceived(const boost::system::error_code& error, size_t bytes_transferred) {
    double timeReceived = elapsedTime(); // get time now to minimize jitter due to lang load
    if (error == boost::asio::error::connection_aborted) {
        if (mClientNotifyFunc)
            (*mClientNotifyFunc)(mClientData);
    }

    if (error) {
        free(data);
        return;
    }

    assert(bytes_transferred == OSCMsgLength);

    OSC_Packet* packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));

    packet->mReplyAddr.mProtocol = kTCP;
    packet->mReplyAddr.mSocket = socket.native_handle();
    packet->mReplyAddr.mAddress = socket.remote_endpoint().address();
    packet->mReplyAddr.mPort = socket.remote_endpoint().port();

    packet->mSize = OSCMsgLength;
    packet->mData = data;

    ProcessOSCPacket(packet, socket.local_endpoint().port(), timeReceived);

    startReceive();
}

int SC_TcpClientPort::Close() {
    boost::system::error_code error;

    socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
    socket.close();

    if (error) {
        if (error != boost::asio::error::not_connected) {
            ::error("Socket shutdown failed, closed socket anyway. %s", error.message().c_str());
            return errFailed;
        }
    }

    return errNone;
}
