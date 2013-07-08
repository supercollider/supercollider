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

#include <boost/asio.hpp>
#include <boost/array.hpp>

#ifdef _WIN32
# include <winsock2.h>
typedef int socklen_t;
# define bzero( ptr, count ) memset( ptr, 0, count )
#else
#include <sys/types.h>
#include <sys/socket.h>
#endif

#include "SC_Msg.h"
#include "boost/atomic.hpp"
#include "nova-tt/semaphore.hpp"
#include "SC_Lock.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_CmdPort
{
protected:
	thread mThread;

	void Start();
	virtual ReplyFunc GetReplyFunc()=0;
public:
	SC_CmdPort(){}
	virtual ~SC_CmdPort();

	virtual void* Run()=0;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_ComPort : public SC_CmdPort
{
protected:
	int mPortNum;
	int mSocket;
	struct sockaddr_in mBindSockAddr;

	void closeSocket();

public:
	SC_ComPort(int inPortNum);
	virtual ~SC_ComPort();

	int Socket() { return mSocket; }

	int PortNum() const { return mPortNum; }
	int RealPortNum() const { return sc_ntohs(mBindSockAddr.sin_port); }
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

const int kTextBufSize = 65536;

class SC_UdpInPort
{
	int mPortNum;
	boost::array<char, kTextBufSize> recvBuffer;

	boost::asio::ip::udp::endpoint remoteEndpoint;
	boost::asio::ip::udp::socket udpSocket;

	void handleReceivedUDP(const boost::system::error_code& error,
						   std::size_t bytes_transferred);

	void startReceiveUDP();

public:
	int RealPortNum() const { return mPortNum; }
	int Socket() { return udpSocket.native_handle(); }

	virtual void* Run(){return 0;} // remove

	SC_UdpInPort(int inPortNum, int portsToCheck = 10);
	~SC_UdpInPort();
};

class SC_UdpCustomInPort : public SC_UdpInPort
{
public:
	SC_UdpCustomInPort(int inPortNum);
	~SC_UdpCustomInPort();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_TcpInPort : public SC_ComPort
{
	nova::semaphore mConnectionAvailable;
	int mBacklog;

protected:
	virtual ReplyFunc GetReplyFunc();

public:
	SC_TcpInPort(int inPortNum, int inMaxConnections, int inBacklog);

	virtual void* Run();

	void ConnectionTerminated();
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_TcpConnectionPort : public SC_ComPort
{
	SC_TcpInPort *mParent;

protected:
	virtual ReplyFunc GetReplyFunc();

public:
	SC_TcpConnectionPort(SC_TcpInPort *inParent, int inSocket);
	virtual ~SC_TcpConnectionPort();

	virtual void* Run();
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////

class SC_TcpClientPort : public SC_ComPort
{
public:
	typedef void (*ClientNotifyFunc)(void* clientData);

public:
	SC_TcpClientPort(int inSocket, ClientNotifyFunc notifyFunc=0, void* clientData=0);
	virtual ~SC_TcpClientPort();

	virtual void* Run();
	void Close();

protected:
	virtual ReplyFunc GetReplyFunc();

private:
	struct sockaddr_in	mReplySockAddr;
	int					mCmdFifo[2];
	ClientNotifyFunc	mClientNotifyFunc;
	void*				mClientData;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif

