/*
	SuperCollider real time audio synthesis system
	Copyright (c) 2002 James McCartney. All rights reserved.
	Copyright (c) 2013 Tim Blechmann.
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

#ifndef SC_REPLYIMPL_HPP
#define SC_REPLYIMPL_HPP

#include "SC_Reply.h"

#if defined(_WIN32)
# include "SC_Win32Utils.h"
#else
# include <netinet/in.h>
#endif // _WIN32

struct ReplyAddress
{
	struct sockaddr_in mSockAddr;
	int mSockAddrLen;
	int mSocket;
	ReplyFunc mReplyFunc;
	void *mReplyData;
};

void null_reply_func(struct ReplyAddress* addr, char* msg, int size);

bool operator==(const ReplyAddress& a, const ReplyAddress& b);

inline void SendReply(struct ReplyAddress *inReplyAddr, char* inBuf, int inSize)
{
	(inReplyAddr->mReplyFunc)(inReplyAddr, inBuf, inSize);
}

#endif // SC_REPLYIMPL_HPP
