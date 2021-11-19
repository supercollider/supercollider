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
#include <boost/typeof/typeof.hpp>

#define scprintf printf
#include "SC_OscUtils.hpp"
#undef scprintf

void ProcessOSCPacket(std::unique_ptr<OSC_Packet> inPacket, int inPortNum, double time);

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

void handleMessageOSC(Protocol protocol, int replySocket, const boost::asio::ip::address& replyAddress, int replyPort,
                      std::unique_ptr<char[]> data, size_t dataSize, int localPort) {

    const double timeReceived = elapsedTime(); // get time now to minimize jitter due to lang load

    auto packet = std::unique_ptr<OSC_Packet>();

    packet->mReplyAddr.mProtocol = protocol;
    packet->mReplyAddr.mSocket = replySocket;
    packet->mReplyAddr.mAddress = replyAddress;
    packet->mReplyAddr.mPort = replyPort;
    packet->mData = std::move(data);
    packet->mSize = dataSize;

    ProcessOSCPacket(std::move(packet), localPort, timeReceived);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_UdpInPort::SC_UdpInPort(int inPortNum, int portsToCheck): mPortNum(inPortNum), mUdpSocket(ioService) {
    using namespace boost::asio;

    BOOST_AUTO(protocol, ip::udp::v4());

    mUdpSocket.open(protocol);

    for (int offset = 0; offset != portsToCheck; ++offset) {
        try {
            mUdpSocket.bind(ip::udp::endpoint(protocol, inPortNum + offset));
            mPortNum = inPortNum + offset;
            break;
        } catch (std::exception const&) {
            if (offset == (portsToCheck - 1))
                throw std::runtime_error("unable to bind udp socket\n");
        }
    }

    boost::asio::socket_base::send_buffer_size option(65536);
    mUdpSocket.set_option(option);

    startReceiveUDP();
}


void SC_UdpInPort::startReceiveUDP() {
    using namespace boost;
    mUdpSocket.async_receive_from(
        asio::buffer(mRecvBuffer), mRemoteEndpoint,
        [this](auto error, auto bytesTransferred) { handleReceivedUDP(error, bytesTransferred); });
}

void SC_UdpInPort::handleReceivedUDP(const boost::system::error_code& error, std::size_t bytesTransferred) {
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

    std::unique_ptr<char[]> data(new char[bytesTransferred]);
    memcpy(data.get(), mRecvBuffer.data(), bytesTransferred);

    handleMessageOSC(kUDP, mUdpSocket.native_handle(), mRemoteEndpoint.address(), mRemoteEndpoint.port(), std::move(data),
                     bytesTransferred, mPortNum);

    startReceiveUDP();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_UdpCustomInPort::SC_UdpCustomInPort(int inPortNum): SC_UdpInPort(inPortNum, 1) { }

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_TcpInPort::SC_TcpInPort(int inPortNum, int inMaxConnections, int inBacklog):
    mAcceptor(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), inPortNum)), mPortNum(inPortNum) {
    // FIXME: handle max connections
    // FIXME: backlog???

    startAccept();
}

void SC_TcpInPort::startAccept() {
    SC_TcpConnection::pointer newConnection(new SC_TcpConnection(ioService, mPortNum));

    mAcceptor.async_accept(newConnection->getSocket(),
                          [this, newConnection](auto error) { handleAccept(newConnection, error); });
}

void SC_TcpInPort::handleAccept(SC_TcpConnection::pointer newConnection, const boost::system::error_code& error) {
    if (!error)
        newConnection->start();
    startAccept();
}

void SC_TcpConnection::start() {
    namespace ba = boost::asio;
    ba::async_read(mSocket, ba::buffer(&mOSCMsgLength, sizeof(mOSCMsgLength)),
                   [receiver = shared_from_this()](auto error, auto bytesTransferred) {
                       receiver->handleLengthReceived(error, bytesTransferred);
                   });
}

void SC_TcpConnection::handleLengthReceived(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error)
        return;

    namespace ba = boost::asio;
    // msglen is in network byte order
    mOSCMsgLength = sc_ntohl(mOSCMsgLength);

    mData.reset(new char[mOSCMsgLength]);

    ba::async_read(mSocket, ba::buffer(mData.get(), mOSCMsgLength),
                   [receiver = shared_from_this()](auto error, auto bytesReceived) {
                       receiver->handleMsgReceived(error, bytesReceived);
                   });
}

void SC_TcpConnection::handleMsgReceived(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error) {
        mData.reset();
        return;
    }

    assert(bytes_transferred == mOSCMsgLength);

    const int replyPort = 0;
    const boost::asio::ip::address replyAddress;
    handleMessageOSC(kTCP, mSocket.native_handle(), replyAddress, replyPort, std::move(mData), mOSCMsgLength, mPortNum);

    start();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_TcpClientPort::SC_TcpClientPort(unsigned long inAddress, int inPort, ClientNotifyFunc notifyFunc, void* clientData):
    mSocket(ioService),
    mEndpoint(boost::asio::ip::address_v4(inAddress), inPort),
    mClientNotifyFunc(notifyFunc),
    mClientData(clientData) {
    using namespace boost::asio;

    boost::system::error_code error;
    ip::tcp::no_delay noDelayOption(true);
    mSocket.set_option(noDelayOption, error);

    mSocket.connect(mEndpoint);

    startReceive();
}

void SC_TcpClientPort::startReceive() {
    namespace ba = boost::asio;
    ba::async_read(mSocket, ba::buffer(&mOSCMsgLength, sizeof(mOSCMsgLength)),
                   [this](auto error, auto bytesTransferred) { handleLengthReceived(error, bytesTransferred); });
}

void SC_TcpClientPort::handleLengthReceived(const boost::system::error_code& error, size_t bytes_transferred) {
    if (error == boost::asio::error::connection_aborted) {
        if (mClientNotifyFunc)
            (*mClientNotifyFunc)(mClientData);
    }

    if (error)
        return;

    // msglen is in network byte order
    mOSCMsgLength = sc_ntohl(mOSCMsgLength);
    mData.reset(new char[mOSCMsgLength]);

    namespace ba = boost::asio;
    ba::async_read(mSocket, ba::buffer(mData.get(), mOSCMsgLength),
                   [this](auto error, auto bytesTransferred) { handleMsgReceived(error, bytesTransferred); });
}

void SC_TcpClientPort::handleMsgReceived(const boost::system::error_code& error, size_t bytes_transferred) {
    double timeReceived = elapsedTime(); // get time now to minimize jitter due to lang load
    if (error == boost::asio::error::connection_aborted) {
        if (mClientNotifyFunc)
            (*mClientNotifyFunc)(mClientData);
    }

    if (error) {
        mData.reset();
        return;
    }

    assert(bytes_transferred == mOSCMsgLength);

    handleMessageOSC(kTCP, mSocket.native_handle(), mSocket.remote_endpoint().address(),
                     mSocket.remote_endpoint().port(), std::move(mData), mOSCMsgLength, mSocket.local_endpoint().port());

    startReceive();
}

int SC_TcpClientPort::Close() {
    boost::system::error_code error;

    mSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, error);
    mSocket.close();

    if (error) {
        if (error != boost::asio::error::not_connected) {
            ::error("Socket shutdown failed, closed socket anyway. %s", error.message().c_str());
            return errFailed;
        }
    }

    return errNone;
}
