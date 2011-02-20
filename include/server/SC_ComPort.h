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


#ifndef _SC_ComPort_
#define _SC_ComPort_

#include <sys/types.h>
#ifdef _WIN32
# include <winsock2.h>
#else
# include <sys/socket.h>
#endif
#include "OSC_Packet.h"
#include "SC_Sem.h"

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

#endif
