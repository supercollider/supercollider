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

#include "SC_Endian.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "sc_msg_iter.h"
#include "SC_OscUtils.hpp"

#include <ctype.h>
#include <stdexcept>
#include <stdarg.h>
#include <cerrno>

#include <sys/types.h>
#include "OSC_Packet.h"

#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/typeof/typeof.hpp>

#include "SC_Lock.h"

#include "nova-tt/semaphore.hpp"
#include "nova-tt/thread_priority.hpp"

#ifdef USE_RENDEZVOUS
#    include "Rendezvous.h"
#endif


bool ProcessOSCPacket(World* inWorld, OSC_Packet* inPacket);

namespace scsynth {

//////////////////////////////////////////////////////////////////////////////////////////////////////////


static bool UnrollOSCPacket(World* inWorld, int inSize, char* inData, OSC_Packet* inPacket) {
    if (!strcmp(inData, "#bundle")) { // is a bundle
        char* data;
        char* dataEnd = inData + inSize;
        int len = 16;
        bool hasNestedBundle = false;

        // get len of nested messages only, without len of nested bundle(s)
        data = inData + 16; // skip bundle header
        while (data < dataEnd) {
            int32 msgSize = OSCint(data);
            data += sizeof(int32);
            if (strcmp(data, "#bundle")) // is a message
                len += sizeof(int32) + msgSize;
            else
                hasNestedBundle = true;
            data += msgSize;
        }

        if (hasNestedBundle) {
            if (len > 16) { // not an empty bundle
                // add nested messages to bundle buffer
                char* buf = (char*)malloc(len);
                inPacket->mSize = len;
                inPacket->mData = buf;

                memcpy(buf, inData, 16); // copy bundle header
                data = inData + 16; // skip bundle header
                while (data < dataEnd) {
                    int32 msgSize = OSCint(data);
                    data += sizeof(int32);
                    if (strcmp(data, "#bundle")) { // is a message
                        memcpy(buf, data - sizeof(int32), sizeof(int32) + msgSize);
                        buf += msgSize;
                    }
                    data += msgSize;
                }

                // process this packet without its nested bundle(s)
                if (!ProcessOSCPacket(inWorld, inPacket)) {
                    free(buf);
                    return false;
                }
            }

            // process nested bundle(s)
            data = inData + 16; // skip bundle header
            while (data < dataEnd) {
                int32 msgSize = OSCint(data);
                data += sizeof(int32);
                if (!strcmp(data, "#bundle")) { // is a bundle
                    OSC_Packet* packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));
                    memcpy(packet, inPacket, sizeof(OSC_Packet)); // clone inPacket

                    if (!UnrollOSCPacket(inWorld, msgSize, data, packet)) {
                        free(packet);
                        return false;
                    }
                }
                data += msgSize;
            }
        } else { // !hasNestedBundle
            char* buf = (char*)malloc(inSize);
            inPacket->mSize = inSize;
            inPacket->mData = buf;
            memcpy(buf, inData, inSize);

            if (!ProcessOSCPacket(inWorld, inPacket)) {
                free(buf);
                return false;
            }
        }
    } else { // is a message
        char* buf = (char*)malloc(inSize);
        inPacket->mSize = inSize;
        inPacket->mData = buf;
        memcpy(buf, inData, inSize);

        if (!ProcessOSCPacket(inWorld, inPacket)) {
            free(buf);
            return false;
        }
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_Thread gAsioThread;
boost::asio::io_service ioService;

const int kTextBufSize = 65536;


static void udp_reply_func(struct ReplyAddress* addr, char* msg, int size) {
    using namespace boost::asio;

    ip::udp::socket* socket = reinterpret_cast<ip::udp::socket*>(addr->mReplyData);
    ip::udp::endpoint endpoint(addr->mAddress, addr->mPort);

    boost::system::error_code errc;
    socket->send_to(buffer(msg, size), endpoint, 0, errc);

    if (errc)
        printf("%s\n", errc.message().c_str());
}

static void tcp_reply_func(struct ReplyAddress* addr, char* msg, int size) {
    // Write size as 32bit unsigned network-order integer
    uint32 u = sc_htonl(size);

    using namespace boost::asio;

    // FIXME: connection could be destroyed!
    ip::tcp::socket* socket = reinterpret_cast<ip::tcp::socket*>(addr->mReplyData);

#if 0
	ip::tcp::socket::message_flags flags = 0;
#    ifdef MSG_NOSIGNAL
	flags = MSG_NOSIGNAL;
#    endif
#endif

    boost::system::error_code errc;
    write(*socket, buffer(&u, sizeof(uint32)), errc);
    if (errc)
        printf("%s\n", errc.message().c_str());

    write(*socket, buffer(msg, size), errc);
    if (errc)
        printf("%s\n", errc.message().c_str());
}


class SC_UdpInPort {
    struct World* mWorld;
    int mPortNum;
    std::string mbindTo;
    boost::array<char, kTextBufSize> recvBuffer;

    boost::asio::ip::udp::endpoint remoteEndpoint;

#ifdef USE_RENDEZVOUS
    SC_Thread mRendezvousThread;
#endif

    void handleReceivedUDP(const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (error == boost::asio::error::operation_aborted)
            return; /* we're done */

        if (error) {
            printf("SC_UdpInPort: received error - %s", error.message().c_str());
            startReceiveUDP();
            return;
        }

        if (mWorld->mDumpOSC)
            dumpOSC(mWorld->mDumpOSC, bytes_transferred, recvBuffer.data());

        OSC_Packet* packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));

        packet->mReplyAddr.mProtocol = kUDP;
        packet->mReplyAddr.mAddress = remoteEndpoint.address();
        packet->mReplyAddr.mPort = remoteEndpoint.port();
        packet->mReplyAddr.mSocket = udpSocket.native_handle();
        packet->mReplyAddr.mReplyFunc = udp_reply_func;
        packet->mReplyAddr.mReplyData = (void*)&udpSocket;

        packet->mSize = bytes_transferred;

        if (!UnrollOSCPacket(mWorld, bytes_transferred, recvBuffer.data(), packet))
            free(packet);

        startReceiveUDP();
    }

    void startReceiveUDP() {
        using namespace boost;
        udpSocket.async_receive_from(asio::buffer(recvBuffer), remoteEndpoint,
                                     boost::bind(&SC_UdpInPort::handleReceivedUDP, this, asio::placeholders::error,
                                                 asio::placeholders::bytes_transferred));
    }

public:
    boost::asio::ip::udp::socket udpSocket;

    SC_UdpInPort(struct World* world, std::string bindTo, int inPortNum):
        mWorld(world),
        mPortNum(inPortNum),
        mbindTo(bindTo),
        udpSocket(ioService) {
        using namespace boost::asio;
        BOOST_AUTO(protocol, ip::udp::v4());
        udpSocket.open(protocol);

        udpSocket.bind(ip::udp::endpoint(boost::asio::ip::address::from_string(bindTo), inPortNum));

        boost::asio::socket_base::send_buffer_size option(65536);
        udpSocket.set_option(option);

#ifdef USE_RENDEZVOUS
        if (world->mRendezvous) {
            SC_Thread thread(boost::bind(PublishPortToRendezvous, kSCRendezvous_UDP, sc_htons(mPortNum)));
            mRendezvousThread = std::move(thread);
        }
#endif

        startReceiveUDP();
    }
};


class SC_TcpConnection : public boost::enable_shared_from_this<SC_TcpConnection> {
public:
    struct World* mWorld;
    typedef boost::shared_ptr<SC_TcpConnection> pointer;
    boost::asio::ip::tcp::socket socket;

    SC_TcpConnection(struct World* world, boost::asio::io_service& ioService, class SC_TcpInPort* parent):
        mWorld(world),
        socket(ioService),
        mParent(parent) {}

    ~SC_TcpConnection();

    void start() {
        const int kMaxPasswordLen = 32;
        char buf[kMaxPasswordLen];
        int32 size;
        int32 msglen;

        boost::system::error_code error;
        boost::asio::ip::tcp::no_delay noDelayOption(true);
        socket.set_option(noDelayOption, error);

        // first message must be the password. 4 tries.
        bool validated = mWorld->hw->mPassword[0] == 0;
        for (int i = 0; !validated && i < 4; ++i) {
            // FIXME: error handling!
            size = boost::asio::read(socket, boost::asio::buffer((void*)&msglen, sizeof(int32)));
            if (size < 0)
                return;

            msglen = sc_ntohl(msglen);
            if (msglen > kMaxPasswordLen)
                break;

            size = boost::asio::read(socket, boost::asio::buffer((void*)buf, msglen));
            if (size < 0)
                return;

            validated = strcmp(buf, mWorld->hw->mPassword) == 0;

            std::this_thread::sleep_for(std::chrono::seconds(i + 1)); // thwart cracking.
        }

        if (validated)
            startReceiveMessage();
    }

private:
    void startReceiveMessage() {
        namespace ba = boost::asio;
        async_read(socket, ba::buffer(&OSCMsgLength, sizeof(OSCMsgLength)),
                   boost::bind(&SC_TcpConnection::handleLengthReceived, shared_from_this(), ba::placeholders::error,
                               ba::placeholders::bytes_transferred));
    }

    int32 OSCMsgLength;
    char* data;
    class SC_TcpInPort* mParent;

    void handleLengthReceived(const boost::system::error_code& error, size_t bytes_transferred) {
        if (error) {
            if (error == boost::asio::error::eof)
                return; // connection closed

            printf("handleLengthReceived: error %s", error.message().c_str());
            return;
        }

        namespace ba = boost::asio;
        // msglen is in network byte order
        OSCMsgLength = sc_ntohl(OSCMsgLength);

        data = (char*)malloc(OSCMsgLength);

        async_read(socket, ba::buffer(data, OSCMsgLength),
                   boost::bind(&SC_TcpConnection::handleMsgReceived, shared_from_this(), ba::placeholders::error,
                               ba::placeholders::bytes_transferred));
    }

    void handleMsgReceived(const boost::system::error_code& error, size_t bytes_transferred) {
        if (error) {
            free(data);
            if (error == boost::asio::error::eof)
                return; // connection closed

            printf("handleMsgReceived: error %s", error.message().c_str());
            return;
        }

        assert(bytes_transferred == OSCMsgLength);

        if (mWorld->mDumpOSC)
            dumpOSC(mWorld->mDumpOSC, bytes_transferred, data);

        OSC_Packet* packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));

        packet->mReplyAddr.mProtocol = kTCP;
        packet->mReplyAddr.mReplyFunc = tcp_reply_func;
        packet->mReplyAddr.mReplyData = (void*)&socket;

        packet->mSize = OSCMsgLength;

        if (!UnrollOSCPacket(mWorld, bytes_transferred, data, packet))
            free(packet);

        startReceiveMessage();
    }
};

class SC_TcpInPort {
    struct World* mWorld;
    boost::asio::ip::tcp::acceptor acceptor;

#ifdef USE_RENDEZVOUS
    SC_Thread mRendezvousThread;
#endif

    std::atomic<int> mAvailableConnections;
    friend class SC_TcpConnection;

public:
    SC_TcpInPort(struct World* world, const std::string& bindTo, int inPortNum, int inMaxConnections, int inBacklog):
        mWorld(world),
        acceptor(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::address::from_string(bindTo), inPortNum)),
        mAvailableConnections(inMaxConnections) {
        // FIXME: backlog???

#ifdef USE_RENDEZVOUS
        if (world->mRendezvous) {
            SC_Thread thread(boost::bind(PublishPortToRendezvous, kSCRendezvous_TCP, sc_htons(inPortNum)));
            mRendezvousThread = std::move(thread);
        }
#endif

        startAccept();
    }

    void startAccept() {
        if (mAvailableConnections > 0) {
            --mAvailableConnections;
            SC_TcpConnection::pointer newConnection(new SC_TcpConnection(mWorld, ioService, this));

            acceptor.async_accept(
                newConnection->socket,
                boost::bind(&SC_TcpInPort::handleAccept, this, newConnection, boost::asio::placeholders::error));
        }
    }

    void handleAccept(SC_TcpConnection::pointer newConnection, const boost::system::error_code& error) {
        if (!error)
            newConnection->start();
        startAccept();
    }

    void connectionDestroyed() {
        if (!mWorld->mRunning)
            return;
        mAvailableConnections += 1;
        startAccept();
    }
};

SC_TcpConnection::~SC_TcpConnection() { mParent->connectionDestroyed(); }


//////////////////////////////////////////////////////////////////////////////////////////////////////////

static void asioFunction() {
#ifdef NOVA_TT_PRIORITY_RT
    std::pair<int, int> priorities = nova::thread_priority_interval_rt();
    nova::thread_set_priority_rt((priorities.first + priorities.second) / 2);
#else
    std::pair<int, int> priorities = nova::thread_priority_interval();
    nova::thread_set_priority(priorities.second);
#endif

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

bool asioThreadStarted() { return gAsioThread.joinable(); }

}

using namespace scsynth;

//////////////////////////////////////////////////////////////////////////////////////////////////////////


SCSYNTH_DLLEXPORT_C bool World_SendPacketWithContext(World* inWorld, int inSize, char* inData, ReplyFunc inFunc,
                                                     void* inContext) {
    if (inSize > 0) {
        if (inWorld->mDumpOSC)
            dumpOSC(inWorld->mDumpOSC, inSize, inData);

        OSC_Packet* packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));

        packet->mReplyAddr.mAddress = boost::asio::ip::address();
        packet->mReplyAddr.mReplyFunc = inFunc;
        packet->mReplyAddr.mReplyData = inContext;
        packet->mReplyAddr.mSocket = 0;

        if (!UnrollOSCPacket(inWorld, inSize, inData, packet)) {
            free(packet);
            return false;
        }
    }
    return true;
}

SCSYNTH_DLLEXPORT_C bool World_SendPacket(World* inWorld, int inSize, char* inData, ReplyFunc inFunc) {
    return World_SendPacketWithContext(inWorld, inSize, inData, inFunc, 0);
}

SCSYNTH_DLLEXPORT_C int World_OpenUDP(struct World* inWorld, const char* bindTo, int inPort) {
    try {
        new SC_UdpInPort(inWorld, bindTo, inPort);
        return true;
    } catch (std::exception& exc) {
        scprintf("Exception in World_OpenUDP: %s\n", exc.what());
    } catch (...) {
    }
    return false;
}

SCSYNTH_DLLEXPORT_C int World_OpenTCP(struct World* inWorld, const char* bindTo, int inPort, int inMaxConnections,
                                      int inBacklog) {
    try {
        new SC_TcpInPort(inWorld, bindTo, inPort, inMaxConnections, inBacklog);
        return true;
    } catch (std::exception& exc) {
        scprintf("Exception in World_OpenTCP: %s\n", exc.what());
    } catch (...) {
    }
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
