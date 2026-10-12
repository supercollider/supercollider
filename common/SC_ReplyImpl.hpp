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

#pragma once

#include "SC_Reply.h"

#ifndef __EMSCRIPTEN__
#    include <boost/asio.hpp>
#endif

enum Protocol { kUDP, kTCP };

struct ReplyAddress {
#ifndef __EMSCRIPTEN__
    boost::asio::ip::address mAddress;
#endif
    enum Protocol mProtocol;
    int mPort;
    int mSocket;

    ReplyFunc mReplyFunc;
    void* mReplyData;
};

inline void null_reply_func(const ReplyAddress* addr, const char* msg, int size) {}

inline bool operator==(const ReplyAddress& a, const ReplyAddress& b) {
#ifdef __EMSCRIPTEN__
    return a.mProtocol == b.mProtocol && a.mPort == b.mPort && a.mSocket == b.mSocket;
#else
    return a.mAddress == b.mAddress && a.mProtocol == b.mProtocol && a.mPort == b.mPort && a.mSocket == b.mSocket;
#endif
}

inline bool operator<(const ReplyAddress& a, const ReplyAddress& b) {
#ifndef __EMSCRIPTEN__
    if (a.mAddress != b.mAddress) {
        return a.mAddress < b.mAddress;
    }
#endif
    if (a.mPort != b.mPort) {
        return a.mPort < b.mPort;
    } else if (a.mSocket != b.mSocket) {
        return a.mSocket < b.mSocket;
    } else {
        return a.mProtocol < b.mProtocol;
    }
}

inline void SendReply(const ReplyAddress* inReplyAddr, const char* inBuf, int inSize) {
    (inReplyAddr->mReplyFunc)(inReplyAddr, inBuf, inSize);
}
