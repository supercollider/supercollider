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


#include "SC_Lib.h"
#include "SC_Lib_Cintf.h"
#include "SC_SequencedCommand.h"
#include "scsynthsend.h"
#include <string.h>
#include <ctype.h>
#include "SC_Prototypes.h"
#include "SC_Str4.h"
#include "SC_WorldOptions.h"

const size_t PATH_BUF_SIZE(256);

void SendDone(ReplyAddress* inReply, const char* inCommandName) {
    small_scpacket packet;
    packet.adds("/done");
    packet.maketags(2);
    packet.addtag(',');
    packet.addtag('s');
    packet.adds(inCommandName);
    SendReply(inReply, packet.data(), packet.size());
}

void SendDoneWithIntValue(ReplyAddress* inReply, const char* inCommandName, int value) {
    small_scpacket packet;
    packet.adds("/done");
    packet.maketags(3);
    packet.addtag(',');
    packet.addtag('s');
    packet.adds(inCommandName);
    packet.addtag('i');
    packet.addi(value);
    SendReply(inReply, packet.data(), packet.size());
}

static void appendVarArgs(small_scpacket& packet, const char* oscFormat, va_list args) {
    // support all the types the server currently does
    while (*oscFormat != '\0') {
        if (*oscFormat == 'i') {
            int32 i = va_arg(args, int32);
            packet.addtag('i');
            packet.addi(i);
        } else if (*oscFormat == 'h') {
            int64 h = va_arg(args, int64);
            packet.addtag('h');
            packet.addii(h);
        } else if (*oscFormat == 'f') {
            float32 f = (float32)va_arg(args, double);
            packet.addtag('f');
            packet.addf(f);
        } else if (*oscFormat == 'd') {
            double d = va_arg(args, double);
            packet.addtag('d');
            packet.addd(d);
        } else if (*oscFormat == 's') {
            const char* s = va_arg(args, char*);
            packet.addtag('s');
            packet.adds(s);
        } else if (*oscFormat == 'b') {
            size_t len = va_arg(args, size_t);
            ++oscFormat;
            uint8* b = va_arg(args, uint8*);
            packet.addtag('b');
            packet.addb(b, len);
        }

        ++oscFormat;
    }
}

void SendDoneWithVarArgs(struct ReplyAddress* inReply, const char* inCommandName, const char* oscFormat, ...) {
    small_scpacket packet;
    int numArgs = strlen(oscFormat);
    packet.adds("/done");
    packet.maketags(2 + numArgs);
    packet.addtag(',');
    packet.addtag('s');
    packet.adds(inCommandName);

    va_list args;
    va_start(args, oscFormat);
    appendVarArgs(packet, oscFormat, args);
    va_end(args);

    SendReply(inReply, packet.data(), packet.size());
}

void SendFailure(ReplyAddress* inReply, const char* inCommandName, const char* errString) {
    small_scpacket packet;
    packet.adds("/fail");
    packet.maketags(3);
    packet.addtag(',');
    packet.addtag('s');
    packet.addtag('s');
    packet.adds(inCommandName);
    packet.adds(errString);
    SendReply(inReply, packet.data(), packet.size());
}

void SendFailureWithIntValue(ReplyAddress* inReply, const char* inCommandName, const char* errString, uint32 index) {
    small_scpacket packet;
    packet.adds("/fail");
    packet.maketags(4);
    packet.addtag(',');
    packet.addtag('s');
    packet.addtag('s');
    packet.adds(inCommandName);
    packet.adds(errString);
    packet.addtag('i');
    packet.addi((int)index);
    SendReply(inReply, packet.data(), packet.size());
}

void SendFailureWithVarArgs(struct ReplyAddress* inReply, const char* inCommandName, const char* errString,
                            const char* oscFormat, ...) {
    small_scpacket packet;
    int numArgs = strlen(oscFormat);
    packet.adds("/fail");
    packet.maketags(3 + numArgs);
    packet.addtag(',');
    packet.addtag('s');
    packet.addtag('s');
    packet.adds(inCommandName);
    packet.adds(errString);

    va_list args;
    va_start(args, oscFormat);
    appendVarArgs(packet, oscFormat, args);
    va_end(args);

    SendReply(inReply, packet.data(), packet.size());
}

void ReportLateness(ReplyAddress* inReply, float32 seconds) {
    small_scpacket packet;
    packet.adds("/late");
    packet.maketags(2);
    packet.addtag(',');
    packet.addtag('f');
    packet.addf(seconds);
    SendReply(inReply, packet.data(), packet.size());
}

SC_NamedObj::SC_NamedObj() { SetName("?"); }

SC_NamedObj::~SC_NamedObj() {}

void SC_NamedObj::SetName(const int32* inName) {
    if (str4len(inName) > (int)kSCNameLen)
        return;
    str4cpy(mName, inName);
    mHash = Hash(mName);
}

void SC_NamedObj::SetName(const char* inName) {
    if (str4len(inName) > (int)kSCNameLen)
        return;
    str4cpy(mName, inName);
    mHash = Hash(mName);
}

SC_LibCmd::SC_LibCmd(SC_CommandFunc inFunc): mFunc(inFunc) {}

SCErr SC_LibCmd::Perform(struct World* inWorld, int inSize, char* inData, ReplyAddress* inReply) {
    SCErr err;
    //	int kSendError = 1;		// i.e., 0x01 | 0x02;
    try {
        err = (mFunc)(inWorld, inSize, inData, inReply);
    } catch (int iexc) {
        err = iexc;
    } catch (std::exception& exc) {
        if (inWorld->mLocalErrorNotification <= 0 && inWorld->mErrorNotification) {
            CallSendFailureCommand(inWorld, (char*)Name(), exc.what(), inReply);
            scprintf("FAILURE IN SERVER %s %s\n", (char*)Name(), exc.what());
        }
        return kSCErr_Failed;
    } catch (...) {
        err = kSCErr_Failed;
    }
    if (err && (inWorld->mLocalErrorNotification <= 0 && inWorld->mErrorNotification)) {
        char errstr[128];
        SC_ErrorString(err, errstr);
        CallSendFailureCommand(inWorld, (char*)Name(), errstr, inReply);
        scprintf("FAILURE IN SERVER %s %s\n", (char*)Name(), errstr);
    }
    return err;
}

SCErr NewCommand(const char* inPath, uint32 inCommandNumber, SC_CommandFunc inFunc) {
    char path[PATH_BUF_SIZE];
    snprintf(path, PATH_BUF_SIZE, "/%s", inPath);

    SC_LibCmd* cmd = new SC_LibCmd(inFunc);
    cmd->SetName(path);
    gCmdLib->Add(cmd);

    // support OSC commands without the leading slash
    SC_LibCmd* cmd2 = new SC_LibCmd(inFunc);
    cmd2->SetName(inPath);
    gCmdLib->Add(cmd2);

    // support integer OSC commands
    gCmdArray[inCommandNumber] = cmd;

    return kSCErr_None;
}
