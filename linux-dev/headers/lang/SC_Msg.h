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


#ifndef _SC_Msg_
#define _SC_Msg_

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include "sc_msg_iter.h"

class SC_Msg;

enum { // Handler IDs
    kUnknownAction = 0,
    kRealTimeAction = 1,
    kNonRealTimeAction = 2,
    kEitherTimeAction = 3
};

typedef void (*ReplyFunc)(struct ReplyAddress *inReplyAddr, char* inBuf, int inSize);

struct ReplyAddress
{
	struct sockaddr_in mSockAddr;
	int mSockAddrLen;
	int mSocket;
	ReplyFunc mReplyFunc;	
};

inline void SendReply(ReplyAddress *inReplyAddr, char* inBuf, int inSize)
{
	(inReplyAddr->mReplyFunc)(inReplyAddr, inBuf, inSize);
}

void DumpReplyAddress(ReplyAddress *inReplyAddress);
int32 Hash(ReplyAddress *inReplyAddress);

struct OSC_Packet
{
	char *mData;
	int32 mSize;
	bool mIsBundle;
	
	ReplyAddress mReplyAddr;
};

void FreeOSCPacket(OSC_Packet *inPacket);

#endif


