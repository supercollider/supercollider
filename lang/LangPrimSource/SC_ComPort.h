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

	void handleReceivedUDP(const boost::system::error_code& error,
						   std::size_t bytes_transferred);

	void startReceiveUDP();

public:
	boost::asio::ip::udp::socket udpSocket;

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

class SC_TcpConnection
{
public:
	typedef boost::shared_ptr<SC_TcpConnection> pointer;
	boost::asio::ip::tcp::socket socket;

	SC_TcpConnection(boost::asio::io_service & ioService,
					 class SC_TcpInPort * parent):
		socket(ioService), mParent(parent)
	{}

	void start();

private:
	int32 OSCMsgLength;
	char * data;
	class SC_TcpInPort * mParent;

	void handleLengthReceived(const boost::system::error_code& error,
							  size_t bytes_transferred);

	void handleMsgReceived(const boost::system::error_code& error,
						   size_t bytes_transferred);
};

class SC_TcpInPort
{
	boost::asio::ip::tcp::acceptor acceptor;

public:
	const int mPortNum;

	SC_TcpInPort(int inPortNum, int inMaxConnections, int inBacklog);

	void startAccept();
	void handleAccept(SC_TcpConnection::pointer new_connection,
					  const boost::system::error_code& error);
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

