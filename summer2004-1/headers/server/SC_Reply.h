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
#ifdef SC_WIN32
# include <winsock2.h>
#else
#include <netinet/in.h>
#endif                 
#include "sc_msg_iter.h"

typedef void (*ReplyFunc)(struct ReplyAddress *inReplyAddr, char* inBuf, int inSize);

void null_reply_func(struct ReplyAddress* addr, char* msg, int size);

struct ReplyAddress
{
	struct sockaddr_in mSockAddr;
	int mSockAddrLen;
	int mSocket;
	ReplyFunc mReplyFunc;	
};

bool operator==(const ReplyAddress& a, const ReplyAddress& b);

inline void SendReply(ReplyAddress *inReplyAddr, char* inBuf, int inSize)
{
	(inReplyAddr->mReplyFunc)(inReplyAddr, inBuf, inSize);
}

#endif


