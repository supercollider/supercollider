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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "SC_ComPort.h"
//#include "SC_Lock.h"
#include <ctype.h>
#include <stdexcept>
#include <stdarg.h>

int recvall(int socket, void *msg, size_t len);
int recvallfrom(int socket, void *msg, size_t len, struct sockaddr *fromaddr, int addrlen);
int sendallto(int socket, const void *msg, size_t len, struct sockaddr *toaddr, int addrlen);
int sendall(int socket, const void *msg, size_t len);

void ProcessOSCPacket(OSC_Packet *inPacket);

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_CmdPort::SC_CmdPort()
{
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_ComPort::SC_ComPort(int inPortNum)
	: mPortNum(inPortNum), mSocket(-1)
{
}

SC_ComPort::~SC_ComPort()
{
    if (mSocket != -1) close(mSocket);
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
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SC_UdpInPort::SC_UdpInPort(int inPortNum)
	: SC_ComPort(inPortNum)
{	
	if ((mSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		throw std::runtime_error("failed to create udp socket\n");
	}

	bzero((char *)&mBindSockAddr, sizeof(mBindSockAddr));
	mBindSockAddr.sin_family = AF_INET;
	mBindSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

        bool bound = false;
        for (int i=0; i<10 && !bound; ++i) {
            mBindSockAddr.sin_port = htons(mPortNum+i);
            if (bind(mSocket, (struct sockaddr *)&mBindSockAddr, sizeof(mBindSockAddr)) >= 0) {
                bound = true;
            }
	}
        if (!bound) throw std::runtime_error("unable to bind udp socket\n");
	Start();
}

SC_UdpInPort::~SC_UdpInPort()
{
	if (mSocket != -1) close(mSocket);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

void DumpReplyAddress(ReplyAddress *inReplyAddress)
{
	printf("mSockAddrLen %d\n", inReplyAddress->mSockAddrLen);
	printf("mSocket %d\n", inReplyAddress->mSocket);
	printf("mSockAddr.sin_len %d\n", inReplyAddress->mSockAddr.sin_len);
	printf("mSockAddr.sin_family %d\n", inReplyAddress->mSockAddr.sin_family);
	printf("mSockAddr.sin_port %d\n", inReplyAddress->mSockAddr.sin_port);
	printf("mSockAddr.sin_addr.s_addr %d\n", inReplyAddress->mSockAddr.sin_addr.s_addr);
	printf("mReplyFunc %08X\n", inReplyAddress->mReplyFunc);
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
	char buf[kTextBufSize];
	OSC_Packet *packet = 0;
	while (true) {		
		if (!packet) {
			packet = (OSC_Packet*)malloc(sizeof(OSC_Packet));
		}
		packet->mReplyAddr.mSockAddrLen = sizeof(sockaddr_in);
		int size = recvfrom(mSocket, buf, kTextBufSize , 0,
								(struct sockaddr *) &packet->mReplyAddr.mSockAddr, &packet->mReplyAddr.mSockAddrLen);
		
		if (size > 0) {
			char *data = (char*)malloc(size);
			packet->mReplyAddr.mReplyFunc = udp_reply_func;
			packet->mSize = size;
			packet->mData = data;
			packet->mReplyAddr.mSocket = mSocket;
			memcpy(data, buf, size);
			ProcessOSCPacket(packet);
			packet = 0;
		}
	}
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
        int socket = accept(mSocket,(struct sockaddr*)&address,&addressSize);
        if (socket < 0) {
        	mConnectionAvailable.Release();
        } else {
        	SC_TcpConnectionPort *conn = new SC_TcpConnectionPort(this, socket);
        }
    }
    return 0;
}

void SC_TcpInPort::ConnectionTerminated()
{
        mConnectionAvailable.Release();
}

void null_reply_func(struct ReplyAddress *addr, char* msg, int size);
void null_reply_func(struct ReplyAddress *addr, char* msg, int size)
{
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
	close(mSocket);
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
	char buf[kTextBufSize];
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
		
		char *data = (char*)malloc(msglen);
		size = recvall(mSocket, data, msglen);
		if (size < msglen) goto leave;
		
		packet->mReplyAddr.mReplyFunc = tcp_reply_func;
		packet->mSize = msglen;
		packet->mData = data;
		packet->mReplyAddr.mSocket = mSocket;
		ProcessOSCPacket(packet);
		packet = 0;
	}
leave:
    delete this; // ohh this could cause a crash if a reply tries to access it..
    return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

int recvall(int socket, void *msg, size_t len)
{
	int total = 0;
	while (total < len)
	{
		int numbytes = recv(socket, msg, len - total, 0);
		if (numbytes < 0) return total;
		total += numbytes;
		msg = (void*)((char*)msg + numbytes);
	}
	return total;
}

int recvallfrom(int socket, void *msg, size_t len, struct sockaddr *fromaddr, int addrlen)
{
	int total = 0;
	while (total < len)
	{
		int addrlen2 = addrlen;
		int numbytes = recvfrom(socket, msg, len - total, 0, fromaddr, &addrlen2);
		if (numbytes < 0) return total;
		total += numbytes;
		msg = (void*)((char*)msg + numbytes);
	}
	return total;
}

int sendallto(int socket, const void *msg, size_t len, struct sockaddr *toaddr, int addrlen)
{
	int total = 0;
	while (total < len)
	{
		int numbytes = sendto(socket, msg, len - total, 0, toaddr, addrlen);
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
	int total = 0;
	while (total < len) 
	{
		int numbytes = send(socket, msg, len - total, 0);
		if (numbytes < 0) return total;
		total += numbytes;
		msg = (void*)((char*)msg + numbytes);
	}
	return total;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

