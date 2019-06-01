/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    Copyright (c) 2012 Tim Blechmann
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

#include "SC_Reply.h"
#include "SC_ReplyImpl.hpp"

void null_reply_func(struct ReplyAddress* addr, char* msg, int size) {}

bool operator==(const ReplyAddress& a, const ReplyAddress& b) {
    return a.mAddress == b.mAddress && a.mProtocol == b.mProtocol && a.mPort == b.mPort && a.mSocket == b.mSocket;
}

bool operator<(const ReplyAddress& a, const ReplyAddress& b) {
    if (a.mAddress != b.mAddress) {
        return a.mAddress < b.mAddress;
    } else if (a.mPort != b.mPort) {
        return a.mPort < b.mPort;
    } else if (a.mSocket != b.mSocket) {
        return a.mSocket < b.mSocket;
    } else {
        return a.mProtocol < b.mProtocol;
    }
}
