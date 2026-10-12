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

#include "SC_ReplyImpl.hpp"
#include "SC_Types.h"

#include <memory>

struct World;

struct OSC_Packet {
    // OSC_Packet does not necessarily own the data. Also, depending on how the packet was created,
    // the data must be freed on the RT or NRT thread.
    char* mData = nullptr;
    int32 mSize = 0;
    bool mIsBundle = false;
    ReplyAddress mReplyAddr {};
};

/** @brief processes an OSC packet and unrolls nested bundles if necessary.
 *  @note inPacket is already initialized, except for the actual OSC data
 */
bool UnrollOSCPacket(World* inWorld, const char* inData, int32 inSize, std::unique_ptr<OSC_Packet> inPacket);
