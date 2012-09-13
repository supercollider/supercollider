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
#include "SC_Lock.h"
#include "SC_HiddenWorld.h"
#include "SC_WorldOptions.h"
#include "sc_msg_iter.h"
#include <ctype.h>
#include <stdexcept>
#include <stdarg.h>

#include <sys/types.h>
#include "OSC_Packet.h"

#ifdef _WIN32
	# include <winsock2.h>
	typedef int socklen_t;
	# define bzero( ptr, count ) memset( ptr, 0, count )
#else
	#include <netinet/tcp.h>
#endif

#if defined(__linux__) || defined(__FreeBSD__)
	#include <errno.h>
	#include <unistd.h>
#endif

#if defined(SC_IPHONE) || defined(__APPLE__)
	#include <errno.h>
#endif

#ifdef USE_RENDEZVOUS
	#include "Rendezvous.h"
#endif

bool ProcessOSCPacket(World *inWorld, OSC_Packet *inPacket);

namespace {

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_CmdPort
{
protected:
	pthread_t mThread;
	struct World *mWorld;

	void Start();
	virtual ReplyFunc GetReplyFunc()=0;
public:
	SC_CmdPort(struct World *inWorld);
	virtual ~SC_CmdPort() {}

	virtual void* Run()=0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_ComPort : public SC_CmdPort
{
protected:
	int mPortNum;
	int mSocket;
	struct sockaddr_in mBindSockAddr;

	#ifdef USE_RENDEZVOUS
	pthread_t mRendezvousThread;
	#endif

public:
	SC_ComPort(struct World *inWorld, int inPortNum);
	virtual ~SC_ComPort();

	int Socket() { return mSocket; }

	int PortNum() const { return mPortNum; }
	#ifdef USE_RENDEZVOUS
	// default implementation does nothing (this is correct for
	// SC_TcpConnectionPort). Subclasses may override.
	virtual void PublishToRendezvous() {  };
	#endif
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

const size_t kMaxUDPSize = 65535;

class SC_UdpInPort : public SC_ComPort
{
protected:
	struct sockaddr_in mReplySockAddr;
	unsigned char mReadBuf[kMaxUDPSize];
	virtual ReplyFunc GetReplyFunc();

public:
	SC_UdpInPort(struct World *inWorld, int inPortNum);
	~SC_UdpInPort();

	int PortNum() const { return mPortNum; }

	void* Run();
	#ifdef USE_RENDEZVOUS
	virtual void PublishToRendezvous();
	#endif

};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_TcpInPort : public SC_ComPort
{
	SC_Semaphore mConnectionAvailable;
	int mBacklog;

protected:
	virtual ReplyFunc GetReplyFunc();

public:
	SC_TcpInPort(struct World *inWorld, int inPortNum, int inMaxConnections, int inBacklog);

	virtual void* Run();

	void ConnectionTerminated();
	#ifdef USE_RENDEZVOUS
	virtual void PublishToRendezvous();
	#endif
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_TcpConnectionPort : public SC_ComPort
{
	SC_TcpInPort *mParent;
	unsigned char mReadBuf[kMaxUDPSize];

protected:
	virtual ReplyFunc GetReplyFunc();

public:
	SC_TcpConnectionPort(struct World *inWorld, SC_TcpInPort *inParent, int inSocket);
	virtual ~SC_TcpConnectionPort();

	virtual void* Run();
};



int recvall(int socket, void *msg, size_t len);
int sendallto(int socket, const void *msg, size_t len, struct sockaddr *toaddr, int addrlen);
int sendall(int socket, const void *msg, size_t len);

static void dumpOSCmsg(int inSize, char* inData)
{
	int size;
	const char *data;

	if (inData[0]) {
		const char *addr = inData;
		if (strcmp(addr, "/status") == 0) // skip /status messages
			return;

		data = OSCstrskip(inData);
		size = inSize - (data - inData);
		scprintf("[ \"%s\",", addr);
	} else {
		scprintf("[ %d,", OSCint(inData));
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
				scprintf(" %d", msg.geti());
				break;
			case 'f' :
				scprintf(" %g", msg.getf());
				break;
			case 'd' :
				scprintf(" %g", msg.getd());
				break;
			case 's' :
				scprintf(" \"%s\"", msg.gets());
				break;
			case 'b' :
				scprintf(" DATA[%d]", msg.getbsize());
				msg.skipb();
				break;
			case '[' :
				scprintf("[");
				msg.count++;
				break;
			case ']' :
				scprintf("]");
				msg.count++;
				break;
		    default :
				scprintf(" !unknown tag '%c' 0x%02x !", isprint(c)?c:'?', (unsigned char)c & 255);
				goto leave;
		}
		if (msg.remain() && (c!= '[')) scprintf(",");
	}
leave:
	scprintf(" ]");
}

void hexdump(int size, char* data)
{
	char ascii[20];
	int padsize = (size + 15) & -16;
	scprintf("size %d\n", size);
	for (int i=0; i<padsize; ++i)
	{
		if ((i&15)==0)
		{
			scprintf("%4d   ", i);
		}
		if (i >= size)
		{
			scprintf("   ");
			ascii[i&15] = 0;
		}
		else
		{
			scprintf("%02x ", (unsigned char)data[i] & 255);

			if (isprint(data[i])) ascii[i&15] = data[i];
			else ascii[i&15] = '.';
		}
		if ((i&15)==15)
		{
			ascii[16] = 0;
			scprintf("  |%s|\n", ascii);
		}
		else if ((i&3)==3)
		{
			scprintf(" ");
		}
	}
	scprintf("\n");
}

static void dumpOSC(int mode, int size, char* inData)
{
	if (mode & 1)
	{
		if (strcmp(inData, "#bundle") == 0)
		{
			char* data = inData + 8;
			scprintf("[ \"#bundle\", %lld, ", OSCtime(data));
			data += 8;
			char* dataEnd = inData + size;
			while (data < dataEnd) {
				int32 msgSize = OSCint(data);
				data += sizeof(int32);
				scprintf("\n    ");
				dumpOSCmsg(msgSize, data);
				data += msgSize;
				if (data < dataEnd) scprintf(",");
			}
			scprintf("\n]\n");
		}
		else
		{
			dumpOSCmsg(size, inData);
			scprintf("\n");
		}
	}

	if (mode & 2) hexdump(size, inData);
}

}

SC_DLLEXPORT_C bool World_SendPacketWithContext(World *inWorld, int inSize, char *inData, ReplyFunc inFunc, void *inContext)
{
	bool result = false;
	if (inSize > 0) {
		if (inWorld->mDumpOSC) dumpOSC(inWorld->mDumpOSC, inSize, inData);

		OSC_Packet* packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));
		char *data = (char*)malloc(inSize);
		packet->mReplyAddr.mSockAddr.sin_addr.s_addr = 0;
		packet->mReplyAddr.mSockAddr.sin_port = 0;
		packet->mReplyAddr.mReplyFunc = inFunc;
		packet->mReplyAddr.mReplyData = inContext;
		packet->mSize = inSize;
		packet->mData = data;
		packet->mReplyAddr.mSocket = 0;
		memcpy(data, inData, inSize);

		result = ProcessOSCPacket(inWorld, packet);
	}
	return result;
}

SC_DLLEXPORT_C bool World_SendPacket(World *inWorld, int inSize, char *inData, ReplyFunc inFunc)
{
	return World_SendPacketWithContext(inWorld, inSize, inData, inFunc, 0);
}

SC_DLLEXPORT_C int World_OpenUDP(struct World *inWorld, int inPort)
{
	try {
		new SC_UdpInPort(inWorld, inPort);
		return true;
	} catch (std::exception& exc) {
		scprintf("Exception in World_OpenUDP: %s\n", exc.what());
	} catch (...) {
	}
	return false;
}

SC_DLLEXPORT_C int World_OpenTCP(struct World *inWorld, int inPort, int inMaxConnections, int inBacklog)
{
	try {
		new SC_TcpInPort(inWorld, inPort, inMaxConnections, inBacklog);
		return true;
	} catch (std::exception& exc) {
		scprintf("Exception in World_OpenTCP: %s\n", exc.what());
	} catch (...) {
	}
	return false;
}

void set_real_time_priority(pthread_t thread)
{
	int policy;
	struct sched_param param;

	pthread_getschedparam (thread, &policy, &param);
#ifdef __linux__
	policy = SCHED_FIFO;
	const char* env = getenv("SC_SCHED_PRIO");
	// jack uses a priority of 10 in realtime mode, so this is a good default
	const int defprio = 5;
	const int minprio = sched_get_priority_min(policy);
	const int maxprio = sched_get_priority_max(policy);
	const int prio = env ? atoi(env) : defprio;
	param.sched_priority = sc_clip(prio, minprio, maxprio);
#else
	policy = SCHED_RR;         // round-robin, AKA real-time scheduling
	param.sched_priority = 63; // you'll have to play with this to see what it does
#endif
	pthread_setschedparam (thread, policy, &param);
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

SC_CmdPort::SC_CmdPort(struct World *inWorld) : mWorld(inWorld)
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_ComPort::SC_ComPort(struct World *inWorld, int inPortNum)
	: SC_CmdPort(inWorld), mPortNum(inPortNum), mSocket(-1)
{
}

SC_ComPort::~SC_ComPort()
{
#ifdef _WIN32
    if (mSocket != -1) closesocket(mSocket);
#else
    if (mSocket != -1) close(mSocket);
#endif
}

void* com_thread_func(void* arg);
void* com_thread_func(void* arg)
{
    SC_CmdPort *thread = (SC_CmdPort*)arg;
    void* result = thread->Run();
    return result;
}

void SC_CmdPort::Start()
{
    pthread_create (&mThread, NULL, com_thread_func, (void*)this);
    set_real_time_priority(mThread);
}

#ifdef USE_RENDEZVOUS
void* rendezvous_thread_func(void* arg);
void* rendezvous_thread_func(void* arg)
{
	SC_ComPort* port = reinterpret_cast<SC_ComPort*>(arg);
	port->PublishToRendezvous();
	return NULL;
}

void SC_UdpInPort::PublishToRendezvous()
{
	PublishPortToRendezvous(kSCRendezvous_UDP, htons(mPortNum));
}

void SC_TcpInPort::PublishToRendezvous()
{
	PublishPortToRendezvous(kSCRendezvous_TCP, htons(mPortNum));
}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_UdpInPort::SC_UdpInPort(struct World *inWorld, int inPortNum)
	: SC_ComPort(inWorld, inPortNum)
{
	if ((mSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		throw std::runtime_error("failed to create udp socket\n");
	}

	{
		int bufsize = 65536;
#ifdef _WIN32
		setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, (char*)&bufsize, sizeof(bufsize));
#else
		setsockopt(mSocket, SOL_SOCKET, SO_SNDBUF, &bufsize, sizeof(bufsize));
#endif
	}

	//scprintf("@@@ sizeof(ReplyAddress) %d\n", sizeof(ReplyAddress));

	bzero((char *)&mBindSockAddr, sizeof(mBindSockAddr));
	mBindSockAddr.sin_family = AF_INET;
	mBindSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	mBindSockAddr.sin_port = htons(mPortNum);

	if (bind(mSocket, (struct sockaddr *)&mBindSockAddr, sizeof(mBindSockAddr)) < 0) {
		throw std::runtime_error("unable to bind udp socket\n");
	}

	Start();

#ifdef USE_RENDEZVOUS
	if(inWorld->mRendezvous){
		pthread_create(&mRendezvousThread,
			NULL,
			rendezvous_thread_func,
			(void*)this);
	}
#endif
}

SC_UdpInPort::~SC_UdpInPort()
{
#ifdef _WIN32
    if (mSocket != -1) closesocket(mSocket);
#else
	if (mSocket != -1) close(mSocket);
#endif
}


}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

static void DumpReplyAddress(ReplyAddress *inReplyAddress)
{
	scprintf("mSockAddrLen %d\n", inReplyAddress->mSockAddrLen);
	scprintf("mSocket %d\n", inReplyAddress->mSocket);
#ifdef __APPLE__
	scprintf("mSockAddr.sin_len %d\n", inReplyAddress->mSockAddr.sin_len);
#endif
	scprintf("mSockAddr.sin_family %d\n", inReplyAddress->mSockAddr.sin_family);
	scprintf("mSockAddr.sin_port %d\n", inReplyAddress->mSockAddr.sin_port);
	scprintf("mSockAddr.sin_addr.s_addr %d\n", inReplyAddress->mSockAddr.sin_addr.s_addr);
	scprintf("mReplyFunc %p\n", (intptr_t)inReplyAddress->mReplyFunc);
}

/*
int32 GetHash(ReplyAddress *inReplyAddress)
{
	int32 hash = Hash(inReplyAddress->mSockAddr.sin_addr.s_addr);

	hash += (inReplyAddress->mSockAddr.sin_len << 24)
		| (inReplyAddress->mSockAddr.sin_family << 16)
		| inReplyAddress->mSockAddr.sin_port;

	return Hash(hash);
}

ReplyAddress GetKey(ReplyAddress *inReplyAddress)
{
	return *inReplyAddress
}
*/

namespace {

void udp_reply_func(struct ReplyAddress *addr, char* msg, int size)
{
	int total = sendallto(addr->mSocket, msg, size, (sockaddr*)&addr->mSockAddr, addr->mSockAddrLen);
	if (total < size) DumpReplyAddress(addr);
}

ReplyFunc SC_UdpInPort::GetReplyFunc()
{
	return udp_reply_func;
}

void* SC_UdpInPort::Run()
{
	OSC_Packet *packet = 0;
	while (true) {
		if (!packet) {
			// preallocate packet before we need it.
			packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));
		}

		packet->mReplyAddr.mSockAddrLen = sizeof(sockaddr_in);
#ifdef _WIN32
		int size = recvfrom(mSocket, (char *)mReadBuf, kMaxUDPSize , 0,
#else
		int size = recvfrom(mSocket, mReadBuf, kMaxUDPSize , 0,
#endif
								(struct sockaddr *) &packet->mReplyAddr.mSockAddr, (socklen_t*)&packet->mReplyAddr.mSockAddrLen);

		if (size > 0) {
			char *data = (char*)malloc(size);
			memcpy(data, mReadBuf, size);
			if (mWorld->mDumpOSC) dumpOSC(mWorld->mDumpOSC, size, data);

			packet->mReplyAddr.mReplyFunc = udp_reply_func;
			packet->mReplyAddr.mReplyData = 0;
			packet->mSize = size;
			packet->mData = data;
			packet->mReplyAddr.mSocket = mSocket;

			if (!ProcessOSCPacket(mWorld, packet))
			{
				scprintf("command FIFO full\n");
				free(data);
				free(packet);
			}
			packet = 0;
			data = 0;
		}
	}
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_TcpInPort::SC_TcpInPort(struct World *inWorld, int inPortNum, int inMaxConnections, int inBacklog)
	: SC_ComPort(inWorld, inPortNum), mConnectionAvailable(inMaxConnections),
        mBacklog(inBacklog)
{
    if((mSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        throw std::runtime_error("failed to create tcp socket\n");
    }

	//const int on = 1;
	//setsockopt( mSocket, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));

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
#ifdef USE_RENDEZVOUS
	if(inWorld->mRendezvous){
		pthread_create(&mRendezvousThread,
			NULL,
			rendezvous_thread_func,
			(void*)this);
	}
#endif
}

void* SC_TcpInPort::Run()
{
    while (true)
    {
        mConnectionAvailable.Acquire();
        struct sockaddr_in address; /* Internet socket address stuct */
        socklen_t addressSize=sizeof(struct sockaddr_in);
        int socket = accept(mSocket,(struct sockaddr*)&address,&addressSize);
        if (socket < 0) {
        	mConnectionAvailable.Release();
        } else {
        	new SC_TcpConnectionPort(mWorld, this, socket);
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

SC_TcpConnectionPort::SC_TcpConnectionPort(struct World *inWorld, SC_TcpInPort *inParent, int inSocket)
    : SC_ComPort(inWorld, 0), mParent(inParent)
{
    mSocket = inSocket;

#ifdef _WIN32
	const char on = 1;
	setsockopt( mSocket, IPPROTO_TCP, TCP_NODELAY, (const char*)&on, sizeof(on));
#else
	const int on = 1;
	setsockopt( mSocket, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on));
#endif

    Start();
}

SC_TcpConnectionPort::~SC_TcpConnectionPort()
{
#ifdef _WIN32
	closesocket(mSocket);
#else
	close(mSocket);
#endif
    mParent->ConnectionTerminated();
}

void tcp_reply_func(struct ReplyAddress *addr, char* msg, int size);
void tcp_reply_func(struct ReplyAddress *addr, char* msg, int size)
{
    uint32 u ;
    // Write size as 32bit unsigned network-order integer
	u = size;
    u = htonl ( u ) ;
    sendall ( addr->mSocket , &u , 4 ) ;
    // Write message.
    sendall ( addr->mSocket , msg , size ) ;
}


ReplyFunc SC_TcpConnectionPort::GetReplyFunc()
{
	return tcp_reply_func;
}

void* SC_TcpConnectionPort::Run()
{
	const int kMaxPasswordLen = 32;
	char buf[kMaxPasswordLen];
	OSC_Packet *packet = 0;
	// wait for login message
	int32 size;
	int32 msglen;

	// first message must be the password. 4 tries.
	bool validated = mWorld->hw->mPassword[0] == 0;
	for (int i=0; !validated && i<4; ++i) {
		size = recvall(mSocket, &msglen, sizeof(int32) );
		if (size < 0) goto leave;

		msglen = ntohl(msglen);
		if (msglen > kMaxPasswordLen) break;

		size = recvall(mSocket, buf, msglen);
		if (size < 0) goto leave;

		validated = strcmp(buf, mWorld->hw->mPassword) == 0;
#ifdef _WIN32
		if (!validated) Sleep(i+1);	// thwart cracking.
#else
		if (!validated) sleep(i+1);	// thwart cracking.
#endif
	}

	if (validated) {
		while (true) {
			if (!packet) {
				packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));
			}
			size = recvall(mSocket, &msglen, sizeof(int32));
			if (size != sizeof(int32)) goto leave;

			// sk: msglen is in network byte order
			msglen = ntohl(msglen);

			char *data = (char*)malloc(msglen);
			size = recvall(mSocket, data, msglen);
			if (size < msglen) goto leave;

			if (mWorld->mDumpOSC) dumpOSC(mWorld->mDumpOSC, size, data);

			packet->mReplyAddr.mReplyFunc = tcp_reply_func;
			packet->mReplyAddr.mReplyData = 0;
			packet->mSize = msglen;
			packet->mData = data;
			packet->mReplyAddr.mSocket = mSocket;
			if (!ProcessOSCPacket(mWorld, packet)) {
				scprintf("command FIFO full\n");
				free(data);
				free(packet);
			}
			packet = 0;
		}
	}
leave:
    delete this; // ohh this could cause a crash if a reply tries to access it..
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

int recvall(int socket, void *msg, size_t len)
{
	int total = 0;
	while (total < (int)len)
	{
#ifdef _WIN32
		int numbytes = recv(socket, (char*)msg, len - total, 0);
#else
		int numbytes = recv(socket, msg, len - total, 0);
#endif
		if (numbytes <= 0) return total;
		total += numbytes;
		msg = (void*)((char*)msg + numbytes);
	}
	return total;
}

int sendallto(int socket, const void *msg, size_t len, struct sockaddr *toaddr, int addrlen)
{
	int total = 0;
	while (total < (int)len)
	{
#ifdef _WIN32
		int numbytes = sendto(socket, (char*)msg, len - total, 0, toaddr, addrlen);
#else
		int numbytes = sendto(socket, msg, len - total, 0, toaddr, addrlen);
#endif
		if (numbytes < 0) {
			scprintf("sendallto errno %d %s\n", errno, strerror(errno));
			return total;
		}
		total += numbytes;
		msg = (void*)((char*)msg + numbytes);
	}
	return total;
}

int sendall(int socket, const void *msg, size_t len)
{
	int total = 0;
	while (total < (int)len)
	{
#ifdef _WIN32
		int numbytes = send(socket, (const char*)msg, len - total, 0);
#else
		int numbytes = send(socket, msg, len - total, 0);
#endif
		if (numbytes < 0) return total;
		total += numbytes;
		msg = (void*)((char*)msg + numbytes);
	}
	return total;
}

}
