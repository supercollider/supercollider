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


#pragma once

#include <stdio.h>
#include <stdlib.h>

#include "SC_ReplyImpl.hpp"
#include "sc_msg_iter.h"

class SC_Msg;

enum { // Handler IDs
    kUnknownAction = 0,
    kRealTimeAction = 1,
    kNonRealTimeAction = 2,
    kEitherTimeAction = 3
};

int32 Hash(ReplyAddress* inReplyAddress);

struct OSC_Packet {
    char* mData;
    int32 mSize;
    bool mIsBundle;

    ReplyAddress mReplyAddr;
};

void FreeOSCPacket(OSC_Packet* inPacket);
