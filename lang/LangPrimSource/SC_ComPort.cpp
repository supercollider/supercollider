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
#include <fcntl.h>

#ifndef SC_WIN32
# include <unistd.h>
#else
#include "SC_Win32Utils.h"
#endif

#ifdef __linux__
# include <errno.h>
#endif

#ifdef __FreeBSD__
# include <errno.h>
#endif

#if defined(SC_DARWIN) || defined(SC_IPHONE)
#include <errno.h>
#endif

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

int recvall(int socket, void *msg, size_t len);
int recvallfrom(int socket, void *msg, size_t len, struct sockaddr *fromaddr, int addrlen);
int sendallto(int socket, const void *msg, size_t len, struct sockaddr *toaddr, int addrlen);
int sendall(int socket, const void *msg, size_t len);

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

SC_CmdPort::SC_CmdPort()
{
}

SC_CmdPort::~SC_CmdPort()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_ComPort::SC_ComPort(int inPortNum)
	: mPortNum(inPortNum), mSocket(-1)
{
}

SC_ComPort::~SC_ComPort()
{
#ifdef SC_WIN32
    if (mSocket != -1) closesocket(mSocket);
#else
    if (mSocket != -1) close(mSocket);
#endif
}

static void* com_thread_func(void* arg)
{
    SC_CmdPort *thread = (SC_CmdPort*)arg;
    void* result = thread->Run();
    return result;
}

void SC_CmdPort::Start()
{
    pthread_create (&mThread, NULL, com_thread_func, (void*)this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_UdpInPort::SC_UdpInPort(int inPortNum)
	: SC_ComPort(inPortNum)
{
	if ((mSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		throw std::runtime_error("failed to create udp socket\n");
	}

	{
		int bufsize = 65536;
#ifdef SC_WIN32
		setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize));
#else
		setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
#endif
	}

	bzero((char *)&mBindSockAddr, sizeof(mBindSockAddr));
	mBindSockAddr.sin_family = AF_INET;
	mBindSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

        bool bound = false;
        for (int i=0; i<10 && !bound; ++i) {
			mPortNum = mPortNum+i;
            mBindSockAddr.sin_port = htons(mPortNum);
            if (bind(mSocket, (struct sockaddr *)&mBindSockAddr, sizeof(mBindSockAddr)) >= 0) {
                bound = true;
            }
	}
        if (!bound) throw std::runtime_error("unable to bind udp socket\n");
	Start();
}

SC_UdpInPort::~SC_UdpInPort()
{
#ifdef SC_WIN32
	if (mSocket != -1) closesocket(mSocket);
#else
	if (mSocket != -1) close(mSocket);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_UdpCustomInPort::SC_UdpCustomInPort(int inPortNum)
: SC_ComPort(inPortNum)
{
	if ((mSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		throw std::runtime_error("failed to create udp socket\n");
	}

	{
		int bufsize = 65536;
#ifdef SC_WIN32
		setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize));
#else
		setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
#endif
	}

	bzero((char *)&mBindSockAddr, sizeof(mBindSockAddr));
	mBindSockAddr.sin_family = AF_INET;
	mBindSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bool bound = false;
	mBindSockAddr.sin_port = htons(mPortNum);
	if (bind(mSocket, (struct sockaddr *)&mBindSockAddr, sizeof(mBindSockAddr)) >= 0) {
		bound = true;
	}
	if (!bound) throw std::runtime_error("unable to bind udp socket\n");
	Start();
}

SC_UdpCustomInPort::~SC_UdpCustomInPort()
{
	mRunning.store(false);
	pthread_join(mThread, NULL);
#ifdef SC_WIN32
	if (mSocket != -1) closesocket(mSocket);
#else
	if (mSocket != -1) close(mSocket);
#endif

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

/*
int32 Hash(ReplyAddress *inReplyAddress)
{
	int32 hash;
	int32 *word = (int32*)&inReplyAddress->mSockAddr;
	hash = Hash(inReplyAddress->mSockAddr.sin_addr.s_addr);

	hash += inReplyAddress->mSockAddr.sin_len << 24
		| inReplyAddress->mSockAddr.sin_family << 16
		| inReplyAddress->mSockAddr.sin_port;

	hash = Hash(hash);
	return hash;
}
*/

void udp_reply_func(struct ReplyAddress *addr, char* msg, int size);
void udp_reply_func(struct ReplyAddress *addr, char* msg, int size)
{
	printf("->udp_reply_func\n");
	int total = sendallto(addr->mSocket, msg, size, (sockaddr*)&addr->mSockAddr, addr->mSockAddrLen);
	printf("<-udp_reply_func  %d of %d\n", total, size);
	if (total < size) DumpReplyAddress(addr);
}

ReplyFunc SC_UdpInPort::GetReplyFunc()
{
	return udp_reply_func;
}

void* SC_UdpInPort::Run()
{
	OSC_Packet *packet = 0;

	//printf("SC_UdpInPort::Run\n"); fflush(stdout);

	while (true) {
		if (!packet) {
			packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));
		}
		packet->mReplyAddr.mSockAddrLen = sizeof(sockaddr_in);
		int size = recvfrom(mSocket, buf, kTextBufSize , 0,
								(struct sockaddr *) &packet->mReplyAddr.mSockAddr, (socklen_t*)&packet->mReplyAddr.mSockAddrLen);

		if (size > 0) {
			//dumpOSC(3, size, buf);
			//fflush(stdout);

			char *data = (char*)malloc(size);
			packet->mReplyAddr.mReplyFunc = udp_reply_func;
			packet->mSize = size;
			packet->mData = data;
			packet->mReplyAddr.mSocket = mSocket;
			memcpy(data, buf, size);
			ProcessOSCPacket(packet, mPortNum);
			packet = 0;
		}
	}
	return 0;
}

ReplyFunc SC_UdpCustomInPort::GetReplyFunc()
{
	return udp_reply_func;
}

void* SC_UdpCustomInPort::Run()
{
	OSC_Packet *packet = 0;

	const int fd = mSocket;
	const int max_fd = fd+1;

	mRunning.store(true);
	while (mRunning.load(boost::memory_order_consume)) {
		fd_set rfds;

		FD_ZERO(   &rfds);
		FD_SET(fd, &rfds);

		struct timeval timeout;
		timeout.tv_sec = 0;
		timeout.tv_usec = 500000;

		int n = select(max_fd, &rfds, 0, 0, &timeout);
		if ((n > 0) && FD_ISSET(fd, &rfds)) {
			if (!packet) {
				packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));
			}
			packet->mReplyAddr.mSockAddrLen = sizeof(sockaddr_in);
			int size = recvfrom(mSocket, buf, kTextBufSize , 0,
								(struct sockaddr *) &packet->mReplyAddr.mSockAddr, (socklen_t*)&packet->mReplyAddr.mSockAddrLen);

			if (size > 0 && mRunning.load(boost::memory_order_consume)) {
				//dumpOSC(3, size, buf);
				//fflush(stdout);

				char *data = (char*)malloc(size);
				packet->mReplyAddr.mReplyFunc = udp_reply_func;
				packet->mSize = size;
				packet->mData = data;
				packet->mReplyAddr.mSocket = mSocket;
				memcpy(data, buf, size);
				ProcessOSCPacket(packet, mPortNum);
				packet = 0;
			}
		}
	}
	FreeOSCPacket(packet); // just in case
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_TcpInPort::SC_TcpInPort(int inPortNum, int inMaxConnections, int inBacklog)
	: SC_ComPort(inPortNum), mConnectionAvailable(inMaxConnections),
        mBacklog(inBacklog)
{

    if((mSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        throw std::runtime_error("failed to create tcp socket\n");
    }
    //setsockopt(mSocket, SOL_SOCKET, TCP_NODELAY);

    bzero((char *)&mBindSockAddr, sizeof(mBindSockAddr));
    mBindSockAddr.sin_family = AF_INET;
    mBindSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    mBindSockAddr.sin_port = htons(mPortNum);

    if(bind(mSocket, (struct sockaddr *)&mBindSockAddr, sizeof(mBindSockAddr)) < 0)
    {
        throw std::runtime_error("unable to bind tcp socket\n");
    }
    if(listen(mSocket, mBacklog) < 0)
    {
        throw std::runtime_error("unable to listen tcp socket\n");
    }

    Start();
}

void* SC_TcpInPort::Run()
{
    while (true)
    {
        mConnectionAvailable.Acquire();
        struct sockaddr_in address; /* Internet socket address stuct */
        int addressSize=sizeof(struct sockaddr_in);
        int socket = accept(mSocket,(struct sockaddr*)&address,(socklen_t*)&addressSize);
        if (socket < 0) {
        	mConnectionAvailable.Release();
        } else {
        	new SC_TcpConnectionPort(this, socket);
        }
    }
    return 0;
}

void SC_TcpInPort::ConnectionTerminated()
{
        mConnectionAvailable.Release();
}

ReplyFunc SC_TcpInPort::GetReplyFunc()
{
	return null_reply_func;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_TcpConnectionPort::SC_TcpConnectionPort(SC_TcpInPort *inParent, int inSocket)
    : SC_ComPort(0), mParent(inParent)
{
    mSocket = inSocket;
    Start();
}

SC_TcpConnectionPort::~SC_TcpConnectionPort()
{
#ifdef SC_WIN32
	closesocket(mSocket);
#else
	close(mSocket);
#endif
    mParent->ConnectionTerminated();
}

void tcp_reply_func(struct ReplyAddress *addr, char* msg, int size);
void tcp_reply_func(struct ReplyAddress *addr, char* msg, int size)
{
	sendall(addr->mSocket, msg, size);
}

ReplyFunc SC_TcpConnectionPort::GetReplyFunc()
{
	return tcp_reply_func;
}

extern const char* gPassword;

void* SC_TcpConnectionPort::Run()
{
	OSC_Packet *packet = 0;
	// wait for login message
	int32 size;
	int32 msglen;

	while (true) {
		if (!packet) {
			packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));
		}
		size = recvall(mSocket, &msglen, sizeof(int32));
		if (size < 0) goto leave;

		// sk: msglen is in network byte order
		msglen = ntohl(msglen);

		char *data = (char*)malloc(msglen);
		size = recvall(mSocket, data, msglen);
		if (size < msglen) goto leave;

		packet->mReplyAddr.mReplyFunc = tcp_reply_func;
		packet->mSize = msglen;
		packet->mData = data;
		packet->mReplyAddr.mSocket = mSocket;
		ProcessOSCPacket(packet, mPortNum);
		packet = 0;
	}
leave:
    delete this; // ohh this could cause a crash if a reply tries to access it..
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef SC_WIN32
# include <sys/select.h>
#endif
//SC_WIN32

SC_TcpClientPort::SC_TcpClientPort(int inSocket, ClientNotifyFunc notifyFunc, void *clientData)
	: SC_ComPort(0),
	  mClientNotifyFunc(notifyFunc),
	  mClientData(clientData)
{
	mSocket = inSocket;
	socklen_t sockAddrLen = sizeof(mReplySockAddr);

	if (getpeername(mSocket, (struct sockaddr*)&mReplySockAddr, &sockAddrLen) == -1) {
		memset(&mReplySockAddr, 0, sizeof(mReplySockAddr));
		mReplySockAddr.sin_family = AF_INET;
		mReplySockAddr.sin_addr.s_addr = htonl(INADDR_NONE);
		mReplySockAddr.sin_port = htons(0);
	}

#if HAVE_SO_NOSIGPIPE
	int sockopt = 1;
	setsockopt(mSocket, SOL_SOCKET, SO_NOSIGPIPE, &sockopt, sizeof(sockopt));
#endif // HAVE_SO_NOSIGPIPE

	if (pipe(mCmdFifo) == -1) {
		mCmdFifo[0] = mCmdFifo[1] = -1;
	}

	Start();
}

SC_TcpClientPort::~SC_TcpClientPort()
{
#ifdef SC_WIN32
	closesocket(mCmdFifo[0]);
	closesocket(mCmdFifo[1]);
	closesocket(mSocket);
#else
	close(mCmdFifo[0]);
	close(mCmdFifo[1]);
	close(mSocket);
#endif
}

void* SC_TcpClientPort::Run()
{
	OSC_Packet *packet = 0;
	int32 size;
	int32 msglen;

	int cmdfd = mCmdFifo[0];
	int sockfd = mSocket;
	int nfds = sc_max(cmdfd, sockfd) + 1;

	bool cmdClose = false;

	pthread_detach(mThread);

	while (true) {
		fd_set rfds;
		FD_ZERO(&rfds);
		FD_SET(cmdfd, &rfds);
		FD_SET(sockfd, &rfds);

		if ((select(nfds, &rfds, 0, 0, 0) == -1) || (cmdClose = FD_ISSET(cmdfd, &rfds)))
			goto leave;

		if (!FD_ISSET(sockfd, &rfds))
			continue;

		packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));
		if (!packet) goto leave;

		packet->mData = 0;

		size = recvall(sockfd, &msglen, sizeof(int32));
		if (size < (int32)sizeof(int32)) goto leave;

		// msglen is in network byte order
		msglen = ntohl(msglen);

		packet->mData = (char*)malloc(msglen);
		if (!packet->mData) goto leave;

		size = recvall(sockfd, packet->mData, msglen);
		if (size < msglen) goto leave;

		memcpy(&packet->mReplyAddr.mSockAddr, &mReplySockAddr, sizeof(mReplySockAddr));
		packet->mReplyAddr.mSockAddrLen = sizeof(mReplySockAddr);
		packet->mReplyAddr.mSocket = sockfd;
		packet->mReplyAddr.mReplyFunc = tcp_reply_func;
		packet->mSize = msglen;
		ProcessOSCPacket(packet, mPortNum);

		packet = 0;
	}

leave:
	if (packet) {
		free(packet->mData);
		free(packet);
	}
	// Only call notify function when not closed explicitly
	if (!cmdClose && mClientNotifyFunc) {
		(*mClientNotifyFunc)(mClientData);
	}
	delete this;
    return 0;
}

void SC_TcpClientPort::Close()
{
	char cmd = 0;
#ifdef SC_WIN32
	win32_pipewrite(mCmdFifo[1], &cmd, sizeof(cmd));
#else
	size_t written = write(mCmdFifo[1], &cmd, sizeof(cmd));
	if (written != sizeof(cmd))
		post("warning: invalid write in SC_TcpClientPort::Close");
#endif
}

ReplyFunc SC_TcpClientPort::GetReplyFunc()
{
	return tcp_reply_func;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

int recvall(int socket, void *msg, size_t len)
{
	size_t total = 0;
	while (total < len)
	{
#ifdef SC_WIN32
    int numbytes = recv(socket, reinterpret_cast<char*>(msg), len - total, 0);
#else
    int numbytes = recv(socket, msg, len - total, 0);
#endif
		if (numbytes <= 0) return total;
		total += numbytes;
		msg = (void*)((char*)msg + numbytes);
	}
	return total;
}

int recvallfrom(int socket, void *msg, size_t len, struct sockaddr *fromaddr, int addrlen)
{
	size_t total = 0;
	while (total < len)
	{
		socklen_t addrlen2 = addrlen;
#ifdef SC_WIN32
		int numbytes = recvfrom(socket, reinterpret_cast<char*>(msg), len - total, 0, fromaddr, &addrlen2);
#else
		int numbytes = recvfrom(socket, msg, len - total, 0, fromaddr, &addrlen2);
#endif
		if (numbytes < 0) return total;
		total += numbytes;
		msg = (void*)((char*)msg + numbytes);
	}
	return total;
}

int sendallto(int socket, const void *msg, size_t len, struct sockaddr *toaddr, int addrlen)
{
	size_t total = 0;
	while (total < len)
	{
#ifdef SC_WIN32
		int numbytes = sendto(socket, reinterpret_cast<const char*>(msg), len - total, 0, toaddr, addrlen);
#else
		int numbytes = sendto(socket, msg, len - total, 0, toaddr, addrlen);
#endif
		//printf("numbytes %d  total %d  len %d\n", numbytes, total, len);
		if (numbytes < 0) {
			printf("******* errno %d %s\n", errno, strerror(errno));
			return total;
		}
		total += numbytes;
		msg = (void*)((char*)msg + numbytes);
	}
	return total;
}

int sendall(int socket, const void *msg, size_t len)
{
	size_t total = 0;
	while (total < len)
	{
#if HAVE_MSG_NOSIGNAL
		int flags = MSG_NOSIGNAL;
#else
		int flags = 0;
#endif // HAVE_MSG_NOSIGNAL
#ifdef SC_WIN32
    int numbytes = send(socket, reinterpret_cast<const char*>(msg), len - total, flags);
#else
		int numbytes = send(socket, msg, len - total, flags);
#endif
		if (numbytes < 0) return total;
		total += numbytes;
		msg = (void*)((char*)msg + numbytes);
	}
	return total;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

