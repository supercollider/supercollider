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

#include "SC_Lock.h"

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/typeof/typeof.hpp>

// sk: determine means of blocking SIGPIPE for send(2) (implementation
// dependent)

#define HAVE_SO_NOSIGPIPE 0
#define HAVE_MSG_NOSIGNAL 0

#if defined(SO_NOSIGPIPE)
# undef HAVE_SO_NOSIGPIPE
# define HAVE_SO_NOSIGPIPE 1
#elif defined(MSG_NOSIGNAL)
# undef HAVE_MSG_NOSIGNAL
# define HAVE_MSG_NOSIGNAL 1
#endif

int recvall(int socket, char *msg, size_t len);
int recvallfrom(int socket, char *msg, size_t len, struct sockaddr *fromaddr, int addrlen);
int sendallto(int socket, const char *msg, size_t len, struct sockaddr *toaddr, int addrlen);
int sendall(int socket, const char *msg, size_t len);

void ProcessOSCPacket(OSC_Packet *inPacket, int inPortNum);

static void dumpOSCmsg(int inSize, char* inData)
{
	int size;
	const char *data;

	if (inData[0]) {
		char *addr = inData;
		data = OSCstrskip(inData);
		size = inSize - (data - inData);
		printf("[ \"%s\",", addr);
	}
	else
	{
		printf("[ %d,", OSCint(inData));
		data = inData + 4;
		size = inSize - 4;
	}

	sc_msg_iter msg(size, data);

	while (msg.remain())
	{
		char c = msg.nextTag('i');
		switch(c)
		{
			case 'i' :
				printf(" %d", msg.geti());
				break;
			case 'f' :
				printf(" %g", msg.getf());
				break;
			case 'd' :
				printf(" %g", msg.getd());
				break;
			case 's' :
				printf(" \"%s\"", msg.gets());
				break;
			case 'b' :
				printf(" DATA[%zu]", msg.getbsize());
				msg.skipb();
				break;
			case 'm' : {
				char midi [4];
				msg.getb (midi, 4);
				printf(" MIDI[0x%02x 0x%02x 0x%02x 0x%02x]", midi[0], midi[1], midi[2], midi[3]);
				break;
			}
			case 'c' :
				printf(" %c", (char)msg.geti());
				break;
			case 't' :
				printf(" %lld", msg.gett());
				break;
			case 'T' :
				printf(" true");
				msg.count ++;
				break;
			case 'F' :
				printf(" false");
				msg.count ++;
				break;
			case 'I' :
				printf(" infinitum");
				msg.count ++;
				break;
			case 'N' :
				printf(" nil");
				msg.count ++;
				break;
			default :
				printf(" !unknown tag '%c' 0x%02x !", isprint(c)?c:'?', (unsigned char)c & 255);
				goto leave;
		}
		if (msg.remain()) printf(",");
	}
leave:
	printf(" ]");
}

static void hexdump(int size, char* data)
{
	char ascii[20];
	int padsize = (size + 15) & -16;
	printf("size %d\n", size);
	for (int i=0; i<padsize; ++i)
	{
		if ((i&15)==0)
		{
			printf("%4d   ", i);
		}
		if (i >= size)
		{
			printf("   ");
			ascii[i&15] = 0;
		}
		else
		{
			printf("%02x ", (unsigned char)data[i] & 255);

			if (isprint(data[i])) ascii[i&15] = data[i];
			else ascii[i&15] = '.';
		}
		if ((i&15)==15)
		{
			ascii[16] = 0;
			printf("  |%s|\n", ascii);
		}
		else if ((i&3)==3)
		{
			printf(" ");
		}
	}
	printf("\n");
}

void dumpOSC(int mode, int size, char* inData)
{
	if (mode & 1)
	{
		if (strcmp(inData, "#bundle") == 0)
		{
			char* data = inData + 8;
			printf("[ \"#bundle\", %lld, ", OSCtime(data));
			data += 8;
			char* dataEnd = inData + size;
			while (data < dataEnd) {
				int32 msgSize = OSCint(data);
				data += sizeof(int32);
				printf("\n    ");
				dumpOSCmsg(msgSize, data);
				data += msgSize;
				if (data < dataEnd) printf(",");
			}
			printf("\n]\n");
		}
		else
		{
			dumpOSCmsg(size, inData);
			printf("\n");
		}
	}

	if (mode & 2) hexdump(size, inData);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////

thread gAsioThread;
boost::asio::io_service ioService;

static void asioFunction()
{
	boost::asio::io_service::work work(ioService);
	ioService.run();
}

void startAsioThread()
{
	thread asioThread (&asioFunction);
	gAsioThread = thread_namespace::move(asioThread);
}

void stopAsioThread()
{
	ioService.stop();
	gAsioThread.join();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

static void DumpReplyAddress(ReplyAddress *inReplyAddress)
{
	printf("mSockAddrLen %d\n", inReplyAddress->mSockAddrLen);
	printf("mSocket %d\n", inReplyAddress->mSocket);
#ifdef SC_DARWIN
	printf("mSockAddr.sin_len %d\n", inReplyAddress->mSockAddr.sin_len);
#endif
	printf("mSockAddr.sin_family %d\n", inReplyAddress->mSockAddr.sin_family);
	printf("mSockAddr.sin_port %d\n", inReplyAddress->mSockAddr.sin_port);
	printf("mSockAddr.sin_addr.s_addr %d\n", inReplyAddress->mSockAddr.sin_addr.s_addr);
	printf("mReplyFunc %p\n", inReplyAddress->mReplyFunc);
}


void udp_reply_func(struct ReplyAddress *addr, char* msg, int size)
{
//	printf("->udp_reply_func\n");
	int total = sendallto(addr->mSocket, msg, size, (sockaddr*)&addr->mSockAddr, addr->mSockAddrLen);
//	printf("<-udp_reply_func  %d of %d\n", total, size);
	if (total < size) DumpReplyAddress(addr);
}

void tcp_reply_func(struct ReplyAddress *addr, char* msg, int size)
{
	sendall(addr->mSocket, msg, size);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_UdpInPort::SC_UdpInPort(int inPortNum, int portsToCheck):
	mPortNum(inPortNum), udpSocket(ioService)
{
	using namespace boost::asio;

	BOOST_AUTO(protocol, ip::udp::v4());

	udpSocket.open(protocol);

	for (int offset = 0; offset != portsToCheck; ++offset) {
		try {
			udpSocket.bind(ip::udp::endpoint(protocol, inPortNum + offset));
			break;
		} catch (std::exception const & ) {
			if (offset == (portsToCheck - 1))
				throw std::runtime_error("unable to bind udp socket\n");
		}
	}

	boost::asio::socket_base::send_buffer_size option(65536);
	udpSocket.set_option(option);

	startReceiveUDP();
}

SC_UdpInPort::~SC_UdpInPort()
{}


void SC_UdpInPort::startReceiveUDP()
{
	using namespace boost;
	udpSocket.async_receive_from(asio::buffer(recvBuffer), remoteEndpoint,
								 boost::bind(&SC_UdpInPort::handleReceivedUDP, this,
									  asio::placeholders::error, asio::placeholders::bytes_transferred));
}

void SC_UdpInPort::handleReceivedUDP(const boost::system::error_code& error,
									 std::size_t bytesTransferred)
{
	if (error == boost::asio::error::operation_aborted)
		return;    /* we're done */

	if (error) {
		printf("SC_UdpInPort: received error - %s", error.message().c_str());
		startReceiveUDP();
		return;
	}

	OSC_Packet * packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));

	packet->mReplyAddr.mSockAddrLen = sizeof(sockaddr_in);
	packet->mReplyAddr.mReplyFunc = udp_reply_func;
	packet->mReplyAddr.mSockAddr.sin_port        = sc_htons(remoteEndpoint.port());
	packet->mReplyAddr.mSockAddr.sin_addr.s_addr = sc_htonl(remoteEndpoint.address().to_v4().to_ulong());

	char *data = (char*)malloc(bytesTransferred);
	packet->mSize = bytesTransferred;
	packet->mData = data;
	packet->mReplyAddr.mSocket = udpSocket.native_handle();
	memcpy(data, recvBuffer.data(), bytesTransferred);

	ProcessOSCPacket(packet, mPortNum);
	startReceiveUDP();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_UdpCustomInPort::SC_UdpCustomInPort(int inPortNum)
	: SC_UdpInPort(inPortNum, 1)
{}

SC_UdpCustomInPort::~SC_UdpCustomInPort()
{}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_TcpInPort::SC_TcpInPort(int inPortNum, int inMaxConnections, int inBacklog):
	acceptor(ioService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), inPortNum)),
	mPortNum(inPortNum)
{
	// FIXME: handle max connections
	// FIXME: backlog???

	startAccept();
}

void SC_TcpInPort::startAccept()
{
	SC_TcpConnection::pointer newConnection (new SC_TcpConnection(ioService, this));

	acceptor.async_accept(newConnection->socket,
						  boost::bind(&SC_TcpInPort::handleAccept, this, newConnection,
									  boost::asio::placeholders::error));
}

void SC_TcpInPort::handleAccept(SC_TcpConnection::pointer newConnection, const boost::system::error_code &error)
{
	if (!error)
		newConnection->start();
	startAccept();
}

void SC_TcpConnection::start()
{
	namespace ba = boost::asio;
	socket.async_read_some(ba::buffer(&OSCMsgLength, sizeof(OSCMsgLength)),
						   boost::bind(&SC_TcpConnection::handleLengthReceived, this,
									   ba::placeholders::error,
									   ba::placeholders::bytes_transferred));
}

void SC_TcpConnection::handleLengthReceived(const boost::system::error_code &error, size_t bytes_transferred)
{
	if (error)
		return;

	namespace ba = boost::asio;
	// msglen is in network byte order
	OSCMsgLength = sc_ntohl(OSCMsgLength);

	data = (char*)malloc(OSCMsgLength);

	socket.async_read_some(ba::buffer(data, OSCMsgLength),
						   boost::bind(&SC_TcpConnection::handleMsgReceived, this,
									   ba::placeholders::error,
									   ba::placeholders::bytes_transferred));
}

void SC_TcpConnection::handleMsgReceived(const boost::system::error_code &error, size_t bytes_transferred)
{
	if (error) {
		free(data);
		return;
	}

	assert(bytes_transferred == OSCMsgLength);

	OSC_Packet * packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));

	packet->mReplyAddr.mReplyFunc = tcp_reply_func;
	packet->mSize = OSCMsgLength;
	packet->mData = data;
	packet->mReplyAddr.mSocket    = socket.native_handle();

	ProcessOSCPacket(packet, mParent->mPortNum);

	start();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_TcpClientPort::SC_TcpClientPort(long inAddress, int inPort, ClientNotifyFunc notifyFunc, void *clientData):
	socket(ioService),
	endpoint(boost::asio::ip::address_v4(inAddress), inPort),
	mClientNotifyFunc(notifyFunc),
	mClientData(clientData)
{
	using namespace boost::asio;

	boost::system::error_code error;
	ip::tcp::no_delay noDelayOption(true);
	socket.set_option(noDelayOption, error);

	socket.connect(endpoint);

	startReceive();
}

void SC_TcpClientPort::startReceive()
{
	namespace ba = boost::asio;
	socket.async_read_some(ba::buffer(&OSCMsgLength, sizeof(OSCMsgLength)),
						   boost::bind(&SC_TcpClientPort::handleLengthReceived, this,
									   ba::placeholders::error,
									   ba::placeholders::bytes_transferred));
}

void SC_TcpClientPort::handleLengthReceived(const boost::system::error_code &error, size_t bytes_transferred)
{
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
	socket.async_read_some(ba::buffer(data, OSCMsgLength),
						   boost::bind(&SC_TcpClientPort::handleMsgReceived, this,
									   ba::placeholders::error,
									   ba::placeholders::bytes_transferred));
}

void SC_TcpClientPort::handleMsgReceived(const boost::system::error_code &error, size_t bytes_transferred)
{
	if (error == boost::asio::error::connection_aborted) {
		if (mClientNotifyFunc)
			(*mClientNotifyFunc)(mClientData);
	}

	if (error) {
		free(data);
		return;
	}

	assert(bytes_transferred == OSCMsgLength);

	OSC_Packet * packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));

	packet->mReplyAddr.mReplyFunc = tcp_reply_func;
	packet->mSize                 = OSCMsgLength;
	packet->mData                 = data;
	packet->mReplyAddr.mSocket    = socket.native_handle();
	packet->mReplyAddr.mSockAddr.sin_port        = sc_htons(socket.remote_endpoint().port());
	packet->mReplyAddr.mSockAddr.sin_addr.s_addr = sc_htonl(socket.remote_endpoint().address().to_v4().to_ulong());

	ProcessOSCPacket(packet, endpoint.port());

	startReceive();
}

void SC_TcpClientPort::Close()
{
	socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

int recvall(int socket, char *msg, size_t len)
{
	size_t total = 0;
	while (total < len) {
		int numbytes = recv(socket, msg, len - total, 0);
		if (numbytes <= 0) return total;
		total += numbytes;
		msg   += numbytes;
	}
	return total;
}

int recvallfrom(int socket, char *msg, size_t len, struct sockaddr *fromaddr, int addrlen)
{
	size_t total = 0;
	while (total < len) {
		socklen_t addrlen2 = addrlen;
		int numbytes = recvfrom(socket, msg, len - total, 0, fromaddr, &addrlen2);
		if (numbytes < 0) return total;
		total += numbytes;
		msg   += numbytes;
	}
	return total;
}

int sendallto(int socket, const char *msg, size_t len, struct sockaddr *toaddr, int addrlen)
{
	size_t total = 0;
	while (total < len) {
		int numbytes = sendto(socket, msg, len - total, 0, toaddr, addrlen);
		//printf("numbytes %d  total %d  len %d\n", numbytes, total, len);
		if (numbytes < 0) {
			printf("******* errno %d %s\n", errno, strerror(errno));
			return total;
		}
		total += numbytes;
		msg   += numbytes;
	}
	return total;
}

int sendall(int socket, const char *msg, size_t len)
{
	size_t total = 0;
	while (total < len) {
#if HAVE_MSG_NOSIGNAL
		int flags = MSG_NOSIGNAL;
#else
		int flags = 0;
#endif // HAVE_MSG_NOSIGNAL
		int numbytes = send(socket, msg, len - total, flags);
		if (numbytes < 0) return total;
		total += numbytes;
		msg   += numbytes;
	}
	return total;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
