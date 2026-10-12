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

#include "SC_ComPort.h"
#include "SC_Endian.h"
#include "SC_HiddenWorld.h"
#include "SC_Prototypes.h"
#include "SC_WorldOptions.h"
#include "SC_OSC.h"
#include "SC_OscUtils.hpp"

#include "nova-tt/thread_priority.hpp"

#ifdef USE_RENDEZVOUS
#    include "Rendezvous.h"
#endif

#include <thread>

using namespace std::placeholders;
using namespace boost;

/////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace scsynth {

namespace {

std::thread gAsioThread;
asio::io_context gIoContext;

// wrap the work guard in a std::unique_ptr so we can recreate it before we restart the ASIO thread.
using WorkGuard = asio::executor_work_guard<asio::io_context::executor_type>;
std::unique_ptr<WorkGuard> gIoWorkGuard;

void asioFunction() {
    /* NB: on macOS we just keep the default thread priority */
#ifdef NOVA_TT_PRIORITY_RT
    int priority = nova::thread_priority_interval_rt().first;
    nova::thread_set_priority_rt(priority);
#endif

    gIoContext.run();
}

} // namespace

void postAsioThread(std::function<void()> task) { asio::post(gIoContext, std::move(task)); }

void startAsioThread() {
    assert(!gAsioThread.joinable() && gIoWorkGuard == nullptr);
    gIoContext.restart();
    gIoWorkGuard = std::make_unique<WorkGuard>(asio::make_work_guard(gIoContext));
    gAsioThread = std::thread(asioFunction);
}

void stopAsioThread() {
    // release the work guard
    gIoWorkGuard.reset();
    // let the ASIO thread finish all pending jobs
    gAsioThread.join();
}

bool asioThreadStarted() { return gAsioThread.joinable(); }

} // namespace scsynth

//////////////////////////////////////////////////////////////////////////////////////////////////////////

static void udp_reply_func(const ReplyAddress* addr, const char* msg, int size) {
    auto socket = reinterpret_cast<asio::ip::udp::socket*>(addr->mReplyData);
    asio::ip::udp::endpoint endpoint(addr->mAddress, addr->mPort);

    system::error_code errc;
    socket->send_to(asio::buffer(msg, size), endpoint, 0, errc);

    if (errc && socket->is_open())
        scprintf("UDP error: %s\n", errc.message().c_str());
}

SC_UdpInPort::SC_UdpInPort(World* world, std::string_view bindTo, int inPortNum):
    mWorld(world),
    mPortNum(inPortNum),
    mbindTo(bindTo),
    mSocket(scsynth::gIoContext, asio::ip::udp::v4()) {
    mSocket.bind(asio::ip::udp::endpoint(asio::ip::make_address(bindTo), inPortNum));
    if (inPortNum == 0)
        mPortNum = mSocket.local_endpoint().port();

    try {
        asio::socket_base::send_buffer_size sendBufferSize;
        mSocket.get_option(sendBufferSize);
        int defaultBufferSize = sendBufferSize.value();
        if (defaultBufferSize < SC_UdpInPort::sendBufferSize) {
            sendBufferSize = SC_UdpInPort::sendBufferSize;
            system::error_code ec;
            mSocket.set_option(sendBufferSize, ec);
            if (ec && defaultBufferSize < SC_UdpInPort::fallbackBufferSize) {
                sendBufferSize = SC_UdpInPort::fallbackBufferSize;
                mSocket.set_option(sendBufferSize);
            }
        }
    } catch (const system::system_error& e) { scprintf("WARNING: failed to set send buffer size (%s)\n", e.what()); }

    try {
        asio::socket_base::receive_buffer_size receiveBufferSize;
        mSocket.get_option(receiveBufferSize);
        int defaultBufferSize = receiveBufferSize.value();
        if (defaultBufferSize < SC_UdpInPort::receiveBufferSize) {
            receiveBufferSize = SC_UdpInPort::receiveBufferSize;
            system::error_code ec;
            mSocket.set_option(receiveBufferSize, ec);
            if (ec && defaultBufferSize < SC_UdpInPort::fallbackBufferSize) {
                receiveBufferSize = SC_UdpInPort::fallbackBufferSize;
                mSocket.set_option(receiveBufferSize);
            }
        }
    } catch (const system::system_error& e) { scprintf("WARNING: failed to set receive buffer size (%s)\n", e.what()); }

#ifdef USE_RENDEZVOUS
    if (world->mRendezvous) {
        mRendezvousThread = std::thread(PublishPortToRendezvous, kSCRendezvous_UDP, mPortNum);
    }
#endif

    startReceiveUDP();
}

SC_UdpInPort::~SC_UdpInPort() {
    close();
#ifdef USE_RENDEZVOUS
    mRendezvousThread.join();
#endif
}

void SC_UdpInPort::close() {
    if (mSocket.is_open()) {
        try {
            mSocket.close();
        } catch (const system::system_error& e) { scprintf("ERROR: Could not close UDP socket: %s\n", e.what()); }
    }
}

void SC_UdpInPort::handleReceivedUDP(const system::error_code& error, std::size_t bytesTransferred) {
    if (error) {
        if (error == asio::error::connection_refused || error == asio::error::connection_reset) {
            // ignore windows error message
            startReceiveUDP();
        } else if (mSocket.is_open()) {
            scprintf("(scsynth) SC_UdpInPort: received error - %s\n", error.message().c_str());
#if 1
            startReceiveUDP();
#endif
        }
        // don't try to receive if the socket has been already closed!
        return;
    }

    if (mWorld->mDumpOSC)
        dumpOSC(mWorld->mDumpOSC, mRecvBuffer.data(), bytesTransferred);

    auto packet = std::make_unique<OSC_Packet>();
    packet->mReplyAddr.mProtocol = kUDP;
    packet->mReplyAddr.mAddress = mRemoteEndpoint.address();
    packet->mReplyAddr.mPort = mRemoteEndpoint.port();
    packet->mReplyAddr.mSocket = mSocket.native_handle();
    packet->mReplyAddr.mReplyFunc = udp_reply_func;
    packet->mReplyAddr.mReplyData = (void*)&mSocket;

    UnrollOSCPacket(mWorld, mRecvBuffer.data(), bytesTransferred, std::move(packet));

    startReceiveUDP();
}

void SC_UdpInPort::startReceiveUDP() {
    mSocket.async_receive_from(asio::buffer(mRecvBuffer), mRemoteEndpoint,
                               std::bind(&SC_UdpInPort::handleReceivedUDP, this, _1, _2));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

static void tcp_reply_func(const ReplyAddress* addr, const char* msg, int size) {
    // Write size as 32bit unsigned network-order integer
    uint32 u = sc_htonl(size);

    // FIXME: connection could be destroyed!
    auto socket = reinterpret_cast<asio::ip::tcp::socket*>(addr->mReplyData);

#if 0
    asio::ip::tcp::socket::message_flags flags = 0;
#    ifdef MSG_NOSIGNAL
    flags = MSG_NOSIGNAL;
#    endif
#endif

    system::error_code errc;
    asio::write(*socket, asio::buffer(&u, sizeof(uint32)), errc);
    if (!errc) {
        asio::write(*socket, asio::buffer(msg, size), errc);
    }

    if (errc && socket->is_open())
        scprintf("TCP error: %s\n", errc.message().c_str());
}

SC_TcpConnection::SC_TcpConnection(World* world, asio::io_context& ioContext, class SC_TcpInPort* parent):
    mWorld(world),
    mParent(parent),
    mSocket(ioContext) {}

SC_TcpConnection::~SC_TcpConnection() { doClose(); }

bool SC_TcpConnection::start() {
    try {
        // start() should only be called once the connection has been accepted,
        // so the socket should be open.
        assert(mSocket.is_open());
        mReplyAddress.mProtocol = kTCP;
        mReplyAddress.mPort = mSocket.remote_endpoint().port();
        mReplyAddress.mSocket = mSocket.native_handle();
        mReplyAddress.mAddress = mSocket.remote_endpoint().address();
    } catch (const system::system_error& e) {
        scprintf("ERROR: %s\n", e.what());
        return false;
    }

    system::error_code error;
    asio::ip::tcp::no_delay noDelayOption(true);
    mSocket.set_option(noDelayOption, error);

    // first message must be the password. 4 tries.
    bool validated = mWorld->hw->mPassword[0] == 0;
    for (int i = 0; !validated && i < 4; ++i) {
        // FIXME: error handling!
        int32 msglen = 0;
        asio::read(mSocket, asio::buffer((void*)&msglen, sizeof(int32)), error);
        if (error)
            return false;

        msglen = sc_ntohl(msglen);
        if (msglen > kMaxPasswordLen)
            break;

        char buf[kMaxPasswordLen];
        asio::read(mSocket, asio::buffer((void*)buf, msglen), error);
        if (error)
            return false;

        validated = strncmp(buf, mWorld->hw->mPassword, msglen) == 0;

        std::this_thread::sleep_for(std::chrono::seconds(i + 1)); // thwart cracking.
    }

    if (validated) {
        startReceiveMessage();
        return true;
    } else {
        return false;
    }
}

void SC_TcpConnection::close() {
    doClose();
    // if the parent closes the connection, it does not want to be notified!
    mParent = nullptr;
}

void SC_TcpConnection::doClose() {
    if (mSocket.is_open()) {
        try {
            mSocket.close();
        } catch (const system::system_error& e) { scprintf("ERROR: Could not close TCP socket: %s\n", e.what()); }
    }
}

void SC_TcpConnection::startReceiveMessage() {
    async_read(mSocket, asio::buffer(&mOSCMsgLength, sizeof(mOSCMsgLength)),
               std::bind(&SC_TcpConnection::handleLengthReceived, shared_from_this(), _1, _2));
}

void SC_TcpConnection::handleLengthReceived(const system::error_code& error, size_t bytesTransferred) {
    if (error) {
        if (mSocket.is_open()) {
            if (error != asio::error::eof)
                scprintf("handleLengthReceived: error %s", error.message().c_str());

            doClose();

            if (mParent)
                mParent->connectionClosed(shared_from_this());
        }
        return;
    }

    // msglen is in network byte order
    const auto msgLength = sc_ntohl(mOSCMsgLength);

    mOSCMsgData.resize(msgLength);

    async_read(mSocket, asio::buffer(mOSCMsgData.data(), msgLength),
               std::bind(&SC_TcpConnection::handleMsgReceived, shared_from_this(), _1, _2));
}

void SC_TcpConnection::handleMsgReceived(const system::error_code& error, size_t bytesTransferred) {
    if (error) {
        if (mSocket.is_open()) {
            if (error != asio::error::eof)
                scprintf("handleMsgReceived: error %s", error.message().c_str());

            doClose();

            if (mParent)
                mParent->connectionClosed(shared_from_this());
        }
        return;
    }

    assert(bytesTransferred == mOSCMsgData.size());

    if (mWorld->mDumpOSC)
        dumpOSC(mWorld->mDumpOSC, mOSCMsgData.data(), bytesTransferred);

    auto packet = std::make_unique<OSC_Packet>();
    packet->mReplyAddr.mAddress = mSocket.remote_endpoint().address();
    packet->mReplyAddr.mProtocol = kTCP;
    packet->mReplyAddr.mPort = mSocket.remote_endpoint().port();
    packet->mReplyAddr.mSocket = mSocket.native_handle();
    packet->mReplyAddr.mReplyFunc = tcp_reply_func;
    packet->mReplyAddr.mReplyData = (void*)&mSocket;

    UnrollOSCPacket(mWorld, mOSCMsgData.data(), bytesTransferred, std::move(packet));

    startReceiveMessage();
}


SC_TcpInPort::SC_TcpInPort(World* world, std::string_view bindTo, int inPortNum, int inMaxConnections, int inBacklog):
    mWorld(world),
    mAcceptor(scsynth::gIoContext, asio::ip::tcp::endpoint(asio::ip::make_address(bindTo), inPortNum)),
    mMaxNumConnections(inMaxConnections) {
    // FIXME: backlog???

#ifdef USE_RENDEZVOUS
    if (world->mRendezvous) {
        mRendezvousThread = std::thread(PublishPortToRendezvous, kSCRendezvous_TCP,
                                        inPortNum == 0 ? mAcceptor.local_endpoint().port() : inPortNum);
    }
#endif

    startAccept();
}

SC_TcpInPort::~SC_TcpInPort() {
#ifdef USE_RENDEZVOUS
    mRendezvousThread.join();
#endif
    close();
}

void SC_TcpInPort::close() {
    if (mAcceptor.is_open()) {
        try {
            mAcceptor.close();
        } catch (const system::system_error& e) { scprintf("ERROR: Could not close TCP acceptor: %s\n", e.what()); }
        for (auto connection : mConnections) {
            connection->close();
        }
        mConnections.clear();
    }
}

void SC_TcpInPort::startAccept() {
    if (mConnections.size() < mMaxNumConnections) {
        auto newConnection = std::make_shared<SC_TcpConnection>(mWorld, scsynth::gIoContext, this);

        mAcceptor.async_accept(newConnection->socket(),
                               std::bind(&SC_TcpInPort::handleAccept, this, newConnection, _1));
    }
}

void SC_TcpInPort::handleAccept(SC_TcpConnection::pointer newConnection, const system::error_code& error) {
    if (!error) {
        if (newConnection->start()) {
            mConnections.emplace(newConnection);
            assert(mConnections.size() <= mMaxNumConnections);
        }
    }
    // wait for another connection (unless we have been closed)
    if (mAcceptor.is_open())
        startAccept();
}

void SC_TcpInPort::connectionClosed(SC_TcpConnection::pointer connection) {
    // TCP connections are closed on the ASIO thread, so we need to lock the NRT thread.
    World_NRTLock(mWorld);
    World_RemoveClient(mWorld, connection->replyAddress());
    World_NRTUnlock(mWorld);
    mConnections.erase(connection);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T, typename... Args> static T* protectedOpenPort(const char* socketType, Args&&... args) noexcept {
    try {
        return new T(std::forward<Args>(args)...);
    } catch (const system::system_error& exc) {
        // Special verbose message to help with common issue. Issue #3969
        if (exc.code() == boost::system::errc::address_in_use) {
            scprintf("\n*** ERROR: failed to open %s socket: address in use.\n"
                     "This could be because another instance of scsynth is already using it.\n"
                     "You can use SuperCollider (sclang) to kill all running servers by running `Server.killAll`.\n"
                     "You can also kill scsynth using a terminal or your operating system's task manager.\n",
                     socketType);
        } else {
            scprintf("\n*** ERROR: failed to open %s socket: %s\n", socketType, exc.what());
        }
    } catch (const std::exception& e) {
        scprintf("\n*** ERROR: failed to open %s socket: %s\n", socketType, e.what());
    } catch (...) { scprintf("\n*** ERROR: failed to open %s socket: Unknown error\n", socketType); }
    return nullptr;
}

SCSYNTH_DLLEXPORT_C bool World_OpenUDP(World* inWorld, const char* bindTo, int inPort) {
    if (inWorld->hw->mUdpInPort == nullptr) {
        if (auto port = protectedOpenPort<SC_UdpInPort>("UDP", inWorld, bindTo, inPort)) {
            inWorld->hw->mUdpInPort = port;
            return true;
        }
    } else {
        scprintf("\n*** ERROR: UDP port already opened.\n");
    }
    return false;
}

SCSYNTH_DLLEXPORT_C bool World_OpenTCP(World* inWorld, const char* bindTo, int inPort, int inMaxConnections,
                                       int inBacklog) {
    if (inWorld->hw->mUdpInPort == nullptr) {
        if (auto port = protectedOpenPort<SC_TcpInPort>("TCP", inWorld, bindTo, inPort, inMaxConnections, inBacklog)) {
            inWorld->hw->mTcpInPort = port;
            return true;
        }
    } else {
        scprintf("\n*** ERROR: TCP port already opened.\n");
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
