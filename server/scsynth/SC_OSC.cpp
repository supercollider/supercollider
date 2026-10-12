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

#include "SC_OSC.h"
#include "SC_OscUtils.hpp"
#include "SC_Export.h"
#include "SC_World.h"
#include "sc_msg_iter.h"

// implemented in SC_CoreAudio.cpp
bool ProcessOSCPacket(World* inWorld, std::unique_ptr<OSC_Packet> inPacket);

bool UnrollOSCPacket(World* inWorld, const char* inData, int32 inSize, std::unique_ptr<OSC_Packet> inPacket) {
    if (!strcmp(inData, "#bundle")) { // is a bundle
        const char* data = inData + 16; // skip bundle header
        const char* dataEnd = inData + inSize;

        // get len of nested messages only, without len of nested bundle(s)
        bool hasNestedBundle = false;
        int nestedBundleSize = 16;
        while (data < dataEnd) {
            int32 msgSize = OSCint(data);
            data += sizeof(int32);
            if (strcmp(data, "#bundle")) // is a message
                nestedBundleSize += sizeof(int32) + msgSize;
            else
                hasNestedBundle = true;
            data += msgSize;
        }

        if (hasNestedBundle) {
            // make a new bundle that only contains nested *messages*.
            if (nestedBundleSize > 16) { // not an empty bundle
                // add nested messages to bundle buffer
                auto buf = new char[nestedBundleSize];
                memcpy(buf, inData, 16); // copy bundle header
                data = inData + 16; // skip bundle header
                while (data < dataEnd) {
                    int32 msgSize = OSCint(data);
                    data += sizeof(int32);
                    // skip nested bundles
                    if (strcmp(data, "#bundle")) { // is a message
                        memcpy(buf, data - sizeof(int32), sizeof(int32) + msgSize);
                        buf += msgSize;
                    }
                    data += msgSize;
                }

                inPacket->mSize = nestedBundleSize;
                inPacket->mData = buf;

                // process this packet without its nested bundle(s)
                if (!ProcessOSCPacket(inWorld, std::move(inPacket))) {
                    return false;
                }
            }

            // process nested bundle(s)
            data = inData + 16; // skip bundle header
            while (data < dataEnd) {
                int32 msgSize = OSCint(data);
                data += sizeof(int32);
                if (!strcmp(data, "#bundle")) { // is a bundle
                    auto packet = std::make_unique<OSC_Packet>();
                    packet->mReplyAddr = inPacket->mReplyAddr;

                    if (!UnrollOSCPacket(inWorld, data, msgSize, std::move(packet))) {
                        return false;
                    }
                }
                data += msgSize;
            }

            return true;
        } else { // !hasNestedBundle
            inPacket->mData = new char[inSize];
            memcpy(inPacket->mData, inData, inSize);
            inPacket->mSize = inSize;
            inPacket->mIsBundle = true;

            return ProcessOSCPacket(inWorld, std::move(inPacket));
        }
    } else { // is a message
        inPacket->mData = new char[inSize];
        memcpy(inPacket->mData, inData, inSize);
        inPacket->mSize = inSize;
        inPacket->mIsBundle = false;

        return ProcessOSCPacket(inWorld, std::move(inPacket));
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////

SCSYNTH_DLLEXPORT_C bool World_SendPacketWithContext(World* inWorld, int inSize, const char* inData, ReplyFunc inFunc,
                                                     void* inContext) {
    if (inSize > 0) {
        if (inWorld->mDumpOSC)
            dumpOSC(inWorld->mDumpOSC, inData, inSize);

        auto packet = std::make_unique<OSC_Packet>();
#ifndef __EMSCRIPTEN__
        packet->mReplyAddr.mAddress = boost::asio::ip::address();
#endif
        packet->mReplyAddr.mProtocol = kUDP;
        packet->mReplyAddr.mPort = 0;
        packet->mReplyAddr.mSocket = 0;
        packet->mReplyAddr.mReplyFunc = inFunc;
        packet->mReplyAddr.mReplyData = inContext;

        if (!UnrollOSCPacket(inWorld, inData, inSize, std::move(packet))) {
            return false;
        }
    }
    return true;
}

SCSYNTH_DLLEXPORT_C bool World_SendPacket(World* inWorld, int inSize, const char* inData, ReplyFunc inFunc) {
    return World_SendPacketWithContext(inWorld, inSize, inData, inFunc, nullptr);
}
