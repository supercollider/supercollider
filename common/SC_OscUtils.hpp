/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    Copyright (c) 2013 Tim Blechmann

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
#include <cinttypes>

static bool dumpOSCmsg(int inSize, char* inData, bool skipStatus = false) {
    int size;
    const char* data;

    if (inData[0]) {
        const char* addr = inData;
        if (skipStatus) {
            if (strcmp(addr, "/status") == 0 || strcmp(addr, "status") == 0) // skip /status messages
                return false; // nothing has been printed
        }

        data = OSCstrskip(inData);
        size = inSize - (data - inData);
        scprintf("[ \"%s\",", addr);
    } else {
        scprintf("[ %d,", OSCint(inData));
        data = inData + 4;
        size = inSize - 4;
    }

    sc_msg_iter msg(size, data);

    while (msg.remain()) {
        char c = msg.nextTag('i');
        switch (c) {
        case 'i':
            scprintf(" %d", msg.geti());
            break;
        case 'f':
            scprintf(" %g", msg.getf());
            break;
        case 'd':
            scprintf(" %g", msg.getd());
            break;
        case 's':
            scprintf(" \"%s\"", msg.gets());
            break;
        case '[':
            scprintf(" [");
            msg.count++;
            break;
        case ']':
            scprintf(" ]");
            msg.count++;
            break;
        case 'b':
            scprintf(" DATA[%zu]", msg.getbsize());
            msg.skipb();
            break;
        case 'm': {
            char midi[4];
            msg.getb(midi, 4);
            scprintf(" MIDI[0x%02x 0x%02x 0x%02x 0x%02x]", midi[0], midi[1], midi[2], midi[3]);
            break;
        }
        case 'c':
            scprintf(" %c", (char)msg.geti());
            break;
        case 't':
            scprintf(" %" PRId64 "", msg.gett());
            break;
        case 'T':
            scprintf(" true");
            msg.count++;
            break;
        case 'F':
            scprintf(" false");
            msg.count++;
            break;
        case 'I':
            scprintf(" infinitum");
            msg.count++;
            break;
        case 'N':
            scprintf(" nil");
            msg.count++;
            break;
        default:
            scprintf(" !unknown tag '%c' 0x%02x !", isprint(c) ? c : '?', (unsigned char)c & 255);
            goto leave;
        }
        if (msg.remain() && (c != '['))
            scprintf(",");
    }
leave:
    scprintf(" ]");
    return true; // something has been printed
}

static void hexdump(int size, char* data) {
    char ascii[20];
    int padsize = (size + 15) & -16;
    scprintf("size %d\n", size);
    for (int i = 0; i < padsize; ++i) {
        if ((i & 15) == 0) {
            scprintf("%4d   ", i);
        }
        if (i >= size) {
            scprintf("   ");
            ascii[i & 15] = 0;
        } else {
            scprintf("%02x ", (unsigned char)data[i] & 255);

            if (isprint(data[i]))
                ascii[i & 15] = data[i];
            else
                ascii[i & 15] = '.';
        }
        if ((i & 15) == 15) {
            ascii[16] = 0;
            scprintf("  |%s|\n", ascii);
        } else if ((i & 3) == 3) {
            scprintf(" ");
        }
    }
    scprintf("\n");
}

static bool dumpOSCbndl(int indent, int size, char* inData) {
    char* data = inData + 8;
    char* dataEnd = inData + size;

    scprintf("[ \"#bundle\", %" PRIu64 ", ", OSCtime(data));
    data += 8;
    while (data < dataEnd) {
        int contentPrinted;

        int32 msgSize = OSCint(data);
        data += sizeof(int32);

        scprintf("\n");
        for (int i = 0; i < indent + 1; i++)
            scprintf("  ");

        if (!strcmp(data, "#bundle"))
            contentPrinted = dumpOSCbndl(indent + 1, msgSize, data);
        else
            contentPrinted = dumpOSCmsg(msgSize, data, true);
        data += msgSize;
        if ((data < dataEnd) && contentPrinted)
            scprintf(",");
    }
    scprintf("\n");
    for (int i = 0; i < indent; i++)
        scprintf("  ");
    scprintf("]");

    return true;
}

static void dumpOSC(int mode, int size, char* inData) {
    if (mode & 1) {
        int indent = 0;
        bool contentPrinted;

        if (strcmp(inData, "#bundle") == 0)
            contentPrinted = dumpOSCbndl(indent, size, inData);
        else
            contentPrinted = dumpOSCmsg(size, inData, true);

        if (contentPrinted)
            scprintf("\n");
    }

    if (mode & 2)
        hexdump(size, inData);
}

#if 0 // debugging code
static void DumpReplyAddress(ReplyAddress *inReplyAddress)
{
	scprintf("mAddress %s\n", inReplyAddress->mAddress.to_string().c_str());
	if (inReplyAddress->mProtocol == kUDP)
		scprintf("mProtocol UDP\n");
	else
		scprintf("mProtocol TCP\n");

	scprintf("mPort %d\n",   inReplyAddress->mPort);
	scprintf("mSocket %d\n", inReplyAddress->mSocket);

	scprintf("mReplyFunc %p\n", (void*)inReplyAddress->mReplyFunc);
}
#endif
