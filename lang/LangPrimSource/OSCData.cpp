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

#include "PyrPrimitive.h"
#include "PyrKernel.h"
#include "PyrInterpreter.h"
#include "PyrSched.h"
#include "PyrSymbol.h"
#include "GC.h"
#include <string.h>
#include <math.h>
#include <stdexcept>
#include <new>
#include <vector>
#include <iostream>

#include "scsynthsend.h"
#include "sc_msg_iter.h"
#include "SCBase.h"
#include "server_shm.hpp"

#include "SC_ComPort.h"
#include "SC_Endian.h"
#include "SC_Lock.h"
#include "SC_Msg.h"
#include "SC_SndBuf.h"
#include "SC_WorldOptions.h"

#include <boost/asio.hpp>

#ifndef _WIN32
#    include <sys/socket.h>
#    include <ifaddrs.h>

#else

#    include <iphlpapi.h>

#endif

struct InternalSynthServerGlobals {
    struct World* mWorld;
    int mNumSharedControls;
    float* mSharedControls;
};

const int kNumDefaultSharedControls = 1024;
float gDefaultSharedControls[kNumDefaultSharedControls];
bool gUseDoubles = false;

InternalSynthServerGlobals gInternalSynthServer = { 0, kNumDefaultSharedControls, gDefaultSharedControls };

SC_UdpInPort* gUDPport = 0;

PyrString* newPyrString(VMGlobals* g, char* s, int flags, bool runGC);

PyrSymbol *s_call, *s_write, *s_recvoscmsg, *s_recvoscbndl, *s_netaddr;
extern bool compiledOK;

std::vector<SC_UdpCustomInPort*> gCustomUdpPorts;


///////////

inline bool IsBundle(char* ptr) { return strcmp(ptr, "#bundle") == 0; }

///////////

const int ivxNetAddr_Hostaddr = 0;
const int ivxNetAddr_PortID = 1;
// const int ivxNetAddr_Hostname = 2; // unused
const int ivxNetAddr_Socket = 3;

static int makeSynthMsgWithTags(big_scpacket* packet, PyrSlot* slots, int size);

int makeSynthBundle(big_scpacket* packet, PyrSlot* slots, int size, bool useElapsed);

static int addMsgSlot(big_scpacket* packet, PyrSlot* slot) {
    switch (GetTag(slot)) {
    case tagInt:
        packet->addi(slotRawInt(slot));
        break;
    case tagSym:
        packet->adds(slotRawSymbol(slot)->name);
        break;
    case tagObj:
        if (isKindOf(slotRawObject(slot), class_string)) {
            PyrString* stringObj = slotRawString(slot);
            packet->adds(stringObj->s, stringObj->size);
        } else if (isKindOf(slotRawObject(slot), class_int8array)) {
            PyrInt8Array* arrayObj = slotRawInt8Array(slot);
            packet->addb(arrayObj->b, arrayObj->size);
        } else if (isKindOf(slotRawObject(slot), class_array)) {
            PyrObject* arrayObj = slotRawObject(slot);
            big_scpacket packet2;
            if (arrayObj->size > 1 && isKindOfSlot(arrayObj->slots + 1, class_array)) {
                makeSynthBundle(&packet2, arrayObj->slots, arrayObj->size, true);
            } else {
                int error = makeSynthMsgWithTags(&packet2, arrayObj->slots, arrayObj->size);
                if (error != errNone)
                    return error;
            }
            packet->addb((uint8*)packet2.data(), packet2.size());
        }
        break;
    case tagNil:
    case tagTrue:
    case tagFalse:
    case tagChar:
    case tagPtr:
        break;
    default:
        if (gUseDoubles)
            packet->addd(slotRawFloat(slot));
        else
            packet->addf(slotRawFloat(slot));
        break;
    }
    return errNone;
}

static int addMsgSlotWithTags(big_scpacket* packet, PyrSlot* slot) {
    switch (GetTag(slot)) {
    case tagInt:
        packet->addtag('i');
        packet->addi(slotRawInt(slot));
        break;
    case tagSym:
        packet->addtag('s');
        packet->adds(slotRawSymbol(slot)->name);
        break;
    case tagObj:
        if (isKindOf(slotRawObject(slot), class_string)) {
            PyrString* stringObj = slotRawString(slot);
            packet->addtag('s');
            packet->adds(stringObj->s, stringObj->size);
        } else if (isKindOf(slotRawObject(slot), class_int8array)) {
            PyrInt8Array* arrayObj = slotRawInt8Array(slot);
            packet->addtag('b');
            packet->addb(arrayObj->b, arrayObj->size);
        } else if (isKindOf(slotRawObject(slot), class_array)) {
            PyrObject* arrayObj = slotRawObject(slot);
            if (arrayObj->size) {
                packet->addtag('b');
                big_scpacket packet2;
                if (arrayObj->size > 1 && isKindOfSlot(arrayObj->slots + 1, class_array)) {
                    makeSynthBundle(&packet2, arrayObj->slots, arrayObj->size, true);
                } else {
                    int error = makeSynthMsgWithTags(&packet2, arrayObj->slots, arrayObj->size);
                    if (error != errNone)
                        return error;
                }
                packet->addb((uint8*)packet2.data(), packet2.size());
            } else {
                packet->addtag('i');
                packet->addi(0);
            }
        }
        break;
    case tagTrue:
        packet->addtag('i');
        packet->addi(1);
        break;
    case tagChar:
        packet->addtag(slotRawChar(slot));
        break;
    case tagFalse:
    case tagNil:
    case tagPtr:
        packet->addtag('i');
        packet->addi(0);
        break;
    default:
        if (gUseDoubles) {
            packet->addtag('d');
            packet->addd(slotRawFloat(slot));
        } else {
            packet->addtag('f');
            packet->addf(slotRawFloat(slot));
        }
        break;
    }
    return errNone;
}

static int makeSynthMsgWithTags(big_scpacket* packet, PyrSlot* slots, int size) {
    packet->BeginMsg();

    // First component: OSC Address Pattern.
    // For convenience, we allow the user to omit the initial '/', when
    //  expressing it as a symbol (e.g. \g_new) - we add it back on here, for OSC compliance.
    if (GetTag(slots) == tagSym && slotRawSymbol(slots)->name[0] != '/') {
        packet->adds_slpre(slotRawSymbol(slots)->name);
    } else {
        int error = addMsgSlot(packet, slots);
        if (error != errNone)
            return error;
    }

    // skip space for tags
    packet->maketags(size);

    packet->addtag(',');

    try {
        for (int i = 1; i < size; ++i) {
            int error = addMsgSlotWithTags(packet, slots + i);
            if (error != errNone)
                return error;
        }
    } catch (std::runtime_error& e) {
        error("makeSynthMsgWithTags: %s\n", e.what());
        return errFailed;
    }

    packet->EndMsg();

    return errNone;
}

void PerformOSCBundle(int inSize, char* inData, PyrObject* inReply, int inPortNum);
void PerformOSCMessage(int inSize, char* inData, PyrObject* inReply, int inPortNum, double time);
static PyrObject* ConvertReplyAddress(ReplyAddress* inReply);

static void localServerReplyFunc(struct ReplyAddress* inReplyAddr, char* inBuf, int inSize) {
    double timeReceived = elapsedTime();
    bool isBundle = IsBundle(inBuf);

    gLangMutex.lock();
    if (compiledOK) {
        PyrObject* replyObj = ConvertReplyAddress(inReplyAddr);
        if (isBundle) {
            PerformOSCBundle(inSize, inBuf, replyObj, gUDPport->RealPortNum());
        } else {
            PerformOSCMessage(inSize, inBuf, replyObj, gUDPport->RealPortNum(), timeReceived);
        }
    }
    gLangMutex.unlock();
}

int makeSynthBundle(big_scpacket* packet, PyrSlot* slots, int size, bool useElapsed) {
    double time;
    int err;
    int64 oscTime;

    err = slotDoubleVal(slots, &time);
    if (!err) {
        if (useElapsed) {
            oscTime = ElapsedTimeToOSC(time);
        } else {
            oscTime = (int64)(time * kSecondsToOSC);
        }
    } else {
        oscTime = 1; // immediate
    }
    packet->OpenBundle(oscTime);

    for (int i = 1; i < size; ++i) {
        if (isKindOfSlot(slots + i, class_array)) {
            PyrObject* obj = slotRawObject(&slots[i]);
            int error = makeSynthMsgWithTags(packet, obj->slots, obj->size);
            if (error != errNone)
                return error;
        }
    }
    packet->CloseBundle();
    return errNone;
}

static int netAddrSend(PyrObject* netAddrObj, int msglen, char* bufptr, bool sendMsgLen = true) {
    using namespace boost::asio;

    if (IsPtr(netAddrObj->slots + ivxNetAddr_Socket)) {
        SC_TcpClientPort* comPort = (SC_TcpClientPort*)slotRawPtr(netAddrObj->slots + ivxNetAddr_Socket);

        // send TCP
        ip::tcp::socket& socket = comPort->Socket();

        boost::system::error_code errc;

        if (sendMsgLen) {
            // send length of message in network byte-order
            int32 sizebuf = sc_htonl(msglen);
            write(socket, buffer(&sizebuf, sizeof(int32)), errc);
        }

        if (!errc)
            write(socket, buffer(bufptr, msglen), errc);

        if (errc) {
            ::error("Error in netAddrSend: %s\n", errc.message().c_str());
            return errFailed;
        }

        return errNone;

    } else {
        if (gUDPport == 0)
            return errFailed;


        // send UDP
        int addr;
        int err = slotIntVal(netAddrObj->slots + ivxNetAddr_Hostaddr, &addr);
        if (err)
            return err;

        if (addr == 0) {
#ifndef NO_INTERNAL_SERVER
            if (gInternalSynthServer.mWorld) {
                World_SendPacket(gInternalSynthServer.mWorld, msglen, bufptr, &localServerReplyFunc);
            }
#endif
            return errNone;
        }

        int port;
        err = slotIntVal(netAddrObj->slots + ivxNetAddr_PortID, &port);
        if (err)
            return err;

        unsigned long ulAddress = (unsigned int)addr;

        using namespace boost::asio;
        ip::udp::endpoint address(ip::address_v4(ulAddress), port);

        gUDPport->Socket().send_to(buffer(bufptr, msglen), address);
    }

    return errNone;
}


///////////

inline size_t OSCStrLen(char* str) { return (strlen(str) + 4) & ~3; }


int makeSynthBundle(big_scpacket* packet, PyrSlot* slots, int size, bool useElapsed);

static void netAddrTcpClientNotifyFunc(void* clientData) {
    extern bool compiledOK;

    gLangMutex.lock();
    if (compiledOK) {
        PyrObject* netAddrObj = (PyrObject*)clientData;
        VMGlobals* g = gMainVMGlobals;
        g->canCallOS = false;
        ++g->sp;
        SetObject(g->sp, netAddrObj);
        runInterpreter(g, getsym("prConnectionClosed"), 1);
        g->canCallOS = false;
    }
    gLangMutex.unlock();
}

static int prNetAddr_Connect(VMGlobals* g, int numArgsPushed) {
    PyrSlot* netAddrSlot = g->sp;
    PyrObject* netAddrObj = slotRawObject(netAddrSlot);

    int err, port, addr;

    err = slotIntVal(netAddrObj->slots + ivxNetAddr_PortID, &port);
    if (err)
        return err;

    err = slotIntVal(netAddrObj->slots + ivxNetAddr_Hostaddr, &addr);
    if (err)
        return err;

    unsigned long ulAddress = (unsigned int)addr;

    try {
        SC_TcpClientPort* comPort = new SC_TcpClientPort(ulAddress, port, netAddrTcpClientNotifyFunc, netAddrObj);
        SetPtr(netAddrObj->slots + ivxNetAddr_Socket, comPort);
    } catch (std::exception const& e) {
        printf("NetAddr-Connect failed with exception: %s\n", e.what());
        return errFailed;
    }

    return errNone;
}

static int prNetAddr_Disconnect(VMGlobals* g, int numArgsPushed) {
    int err = errNone;

    PyrSlot* netAddrSlot = g->sp;
    PyrObject* netAddrObj = slotRawObject(netAddrSlot);

    SC_TcpClientPort* comPort = (SC_TcpClientPort*)slotRawPtr(netAddrObj->slots + ivxNetAddr_Socket);
    if (comPort) {
        err = comPort->Close();
        SetPtr(netAddrObj->slots + ivxNetAddr_Socket, NULL);
    }

    return err;
}


static int prNetAddr_SendMsg(VMGlobals* g, int numArgsPushed) {
    PyrSlot* netAddrSlot = g->sp - numArgsPushed + 1;
    PyrSlot* args = netAddrSlot + 1;
    big_scpacket packet;

    int numargs = numArgsPushed - 1;
    int error = makeSynthMsgWithTags(&packet, args, numargs);
    if (error != errNone)
        return error;

    return netAddrSend(slotRawObject(netAddrSlot), packet.size(), (char*)packet.buf);
}


static int prNetAddr_SendBundle(VMGlobals* g, int numArgsPushed) {
    PyrSlot* netAddrSlot = g->sp - numArgsPushed + 1;
    PyrSlot* args = netAddrSlot + 1;
    big_scpacket packet;

    double time;
    int err = slotDoubleVal(args, &time);
    if (!err) {
        time += slotRawFloat(&g->thread->seconds);
        SetFloat(args, time);
    }
    int numargs = numArgsPushed - 1;
    makeSynthBundle(&packet, args, numargs, true);

    // for (int i=0; i<packet.size()/4; i++) post("%d %p\n", i, packet.buf[i]);

    return netAddrSend(slotRawObject(netAddrSlot), packet.size(), (char*)packet.buf);
}

static int prNetAddr_SendRaw(VMGlobals* g, int numArgsPushed) {
    PyrSlot* netAddrSlot = g->sp - 1;
    PyrSlot* arraySlot = g->sp;
    PyrObject* netAddrObj = slotRawObject(netAddrSlot);

    if (!IsObj(arraySlot) || !isKindOf(slotRawObject(arraySlot), class_rawarray)) {
        error("sendRaw arg must be a kind of RawArray.\n");
        return errWrongType;
    }
    PyrObject* array = slotRawObject(arraySlot);

    char* bufptr = (char*)array->slots;
    int32 msglen = array->size * gFormatElemSize[array->obj_format];

    return netAddrSend(netAddrObj, msglen, bufptr, false);
}

static int prNetAddr_GetBroadcastFlag(VMGlobals* g, int numArgsPushed) {
    if (gUDPport == 0)
        return errFailed;

    boost::system::error_code ec;
    boost::asio::socket_base::broadcast option;
    gUDPport->udpSocket.get_option(option, ec);

    if (ec)
        return errFailed;

    SetBool(g->sp, option.value());
    return errNone;
}

static int prNetAddr_SetBroadcastFlag(VMGlobals* g, int numArgsPushed) {
    if (gUDPport == 0)
        return errFailed;

    boost::system::error_code ec;
    boost::asio::socket_base::broadcast option(IsTrue(g->sp));
    gUDPport->udpSocket.set_option(option, ec);

    if (ec)
        return errFailed;

    return errNone;
}

static int prNetAddr_BundleSize(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp;
    big_scpacket packet;
    int numargs = slotRawObject(args)->size;
    if (numargs < 1)
        return errFailed;
    makeSynthBundle(&packet, slotRawObject(args)->slots, numargs, true);
    SetInt(args, packet.size());
    return errNone;
}

static int prNetAddr_MsgSize(VMGlobals* g, int numArgsPushed) {
    PyrSlot* args = g->sp;
    big_scpacket packet;

    int numargs = slotRawObject(args)->size;
    if (numargs < 1)
        return errFailed;
    int error = makeSynthMsgWithTags(&packet, slotRawObject(args)->slots, numargs);
    if (error != errNone)
        return error;

    SetInt(args, packet.size());
    return errNone;
}

static int prNetAddr_UseDoubles(VMGlobals* g, int numArgsPushed) {
    // PyrSlot* netAddrSlot = g->sp - 1;
    PyrSlot* flag = g->sp;

    gUseDoubles = IsTrue(flag);

    return errNone;
}

static int prArray_OSCBytes(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    PyrObject* array = slotRawObject(a);
    PyrSlot* args = array->slots;
    int numargs = array->size;
    if (numargs < 1)
        return errFailed;
    big_scpacket packet;

    if (IsFloat(args) || IsNil(args) || IsInt(args)) {
        makeSynthBundle(&packet, args, numargs, false);
    } else if (IsSym(args) || isKindOfSlot(args, class_string)) {
        int error = makeSynthMsgWithTags(&packet, args, numargs);
        if (error != errNone)
            return error;
    } else {
        return errWrongType;
    }

    int size = packet.size();
    PyrInt8Array* obj = newPyrInt8Array(g->gc, size, 0, true);
    obj->size = size;
    memcpy(obj->b, packet.data(), size);
    SetObject(a, (PyrObject*)obj);
    // for (int i=0; i<packet.size()/4; i++) post("%d %p\n", i, packet.buf[i]);

    return errNone;
}

// Create a new <PyrInt8Array> object and copy data from `msg.getb'.
// Bytes are properly untyped, but there is no <UInt8Array> type.

static PyrInt8Array* MsgToInt8Array(sc_msg_iter& msg, bool runGC);
static PyrInt8Array* MsgToInt8Array(sc_msg_iter& msg, bool runGC) {
    int size = msg.getbsize();
    VMGlobals* g = gMainVMGlobals;
    PyrInt8Array* obj = newPyrInt8Array(g->gc, size, 0, runGC);
    obj->size = size;
    msg.getb((char*)obj->b, obj->size);
    return obj;
}

static const double dInfinity = std::numeric_limits<double>::infinity();

static PyrObject* ConvertOSCMessage(int inSize, char* inData) {
    char* cmdName = inData;
    int cmdNameLen = OSCstrlen(cmdName);
    sc_msg_iter msg(inSize - cmdNameLen, inData + cmdNameLen);

    int numElems;
    if (inSize == cmdNameLen) {
        numElems = 0;
    } else {
        if (!msg.tags) {
            numElems = 0;
            error("OSC messages must have type tags.  %s\n", cmdName);
        } else {
            numElems = strlen(msg.tags);
        }
    }
    // post("tags %s %d\n", msg.tags, numElems);

    VMGlobals* g = gMainVMGlobals;
    PyrObject* obj = newPyrArray(g->gc, numElems + 1, 0, false);
    PyrSlot* slots = obj->slots;

    SetSymbol(slots + 0, getsym(cmdName));

    for (int i = 0; i < numElems; ++i) {
        char tag = msg.nextTag();
        // post("%d %c\n", i, tag);
        switch (tag) {
        case 'i':
            SetInt(slots + i + 1, msg.geti());
            break;
        case 'f':
            SetFloat(slots + i + 1, msg.getf());
            break;
        case 'd':
            SetFloat(slots + i + 1, msg.getd());
            break;
        case 's':
            SetSymbol(slots + i + 1, getsym(msg.gets()));
            // post("sym '%s'\n", slots[i+1].us->name);
            break;
        case 'b': // fall through
        case 'm':
            SetObject(slots + i + 1, (PyrObject*)MsgToInt8Array(msg, false));
            break;
        case 'c':
            SetChar(slots + i + 1, (char)msg.geti());
            break;
        case 't':
            SetFloat(slots + i + 1, OSCToElapsedTime(msg.gett()));
            break;

            // argument tags without any associated value
        case 'T':
            SetTrue(slots + i + 1);
            msg.count++;
            break;
        case 'F':
            SetFalse(slots + i + 1);
            msg.count++;
            break;
        case 'I':
            SetFloat(slots + i + 1, dInfinity);
            msg.count++;
            break;
        case 'N':
            SetNil(slots + i + 1);
            msg.count++;
            break;
            // else add the type tag as a char (jrhb 2009)
        default:
            SetChar(slots + i + 1, tag);
            msg.gets();
        }
    }
    obj->size = numElems + 1;
    return obj;
}

static PyrObject* ConvertReplyAddress(ReplyAddress* inReply) {
    VMGlobals* g = gMainVMGlobals;
    PyrObject* obj = instantiateObject(g->gc, s_netaddr->u.classobj, 2, true, false);
    PyrSlot* slots = obj->slots;
    SetInt(slots + 0, inReply->mAddress.to_v4().to_ulong());
    SetInt(slots + 1, inReply->mPort);
    return obj;
}

void PerformOSCBundle(int inSize, char* inData, PyrObject* replyObj, int inPortNum) {
    // convert all data to arrays

    int64 oscTime = OSCtime(inData + 8);
    double seconds = OSCToElapsedTime(oscTime);

    VMGlobals* g = gMainVMGlobals;
    char* data = inData + 16;
    char* dataEnd = inData + inSize;
    while (data < dataEnd) {
        int32 msgSize = OSCint(data);
        data += sizeof(int32);
        if (IsBundle(data)) {
            PerformOSCBundle(msgSize, data, replyObj, inPortNum);
        } else // is a message
        {
            ++g->sp;
            SetObject(g->sp, g->process);
            ++g->sp;
            SetFloat(g->sp, seconds);
            ++g->sp;
            SetObject(g->sp, replyObj);
            ++g->sp;
            SetInt(g->sp, inPortNum);

            PyrObject* arrayObj = ConvertOSCMessage(msgSize, data);
            ++g->sp;
            SetObject(g->sp, arrayObj);
            runInterpreter(g, s_recvoscmsg, 5);
        }
        data += msgSize;
    }
}

void PerformOSCMessage(int inSize, char* inData, PyrObject* replyObj, int inPortNum, double time) {
    PyrObject* arrayObj = ConvertOSCMessage(inSize, inData);

    // call virtual machine to handle message
    VMGlobals* g = gMainVMGlobals;
    ++g->sp;
    SetObject(g->sp, g->process);
    ++g->sp;
    SetFloat(g->sp, time); // time
    ++g->sp;
    SetObject(g->sp, replyObj);
    ++g->sp;
    SetInt(g->sp, inPortNum);
    ++g->sp;
    SetObject(g->sp, arrayObj);

    runInterpreter(g, s_recvoscmsg, 5);
}

void FreeOSCPacket(OSC_Packet* inPacket) {
    // post("->FreeOSCPacket %p\n", inPacket);
    if (inPacket) {
        free(inPacket->mData);
        free(inPacket);
    }
}

// takes ownership of inPacket
void ProcessOSCPacket(OSC_Packet* inPacket, int inPortNum, double time) {
    // post("recv '%s' %d\n", inPacket->mData, inPacket->mSize);
    inPacket->mIsBundle = IsBundle(inPacket->mData);

    gLangMutex.lock();
    if (compiledOK) {
        PyrObject* replyObj = ConvertReplyAddress(&inPacket->mReplyAddr);
        if (compiledOK) {
            if (inPacket->mIsBundle) {
                PerformOSCBundle(inPacket->mSize, inPacket->mData, replyObj, inPortNum);
            } else {
                PerformOSCMessage(inPacket->mSize, inPacket->mData, replyObj, inPortNum, time);
            }
        }
    }
    gLangMutex.unlock();

    FreeOSCPacket(inPacket);
}

void startAsioThread();
void stopAsioThread();

void init_OSC(int port) {
#ifdef _WIN32
    WSAData wsaData;
    int nCode;
    if ((nCode = WSAStartup(MAKEWORD(1, 1), &wsaData)) != 0) {
        error("sclang: init_OSC: WSAStartup() failed with error code %d.\n", nCode);
    }
#endif

    startAsioThread();

    try {
        gUDPport = new SC_UdpInPort(port);
    } catch (std::exception const& e) {
        postfl("No networking: %s", e.what());
    }
}

int prOpenUDPPort(VMGlobals* g, int numArgsPushed);
int prOpenUDPPort(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;
    int port;
    int err = slotIntVal(b, &port);
    if (err)
        return err;

    SC_UdpCustomInPort* newUDPport;

    try {
        SetTrue(a);
        newUDPport = new SC_UdpCustomInPort(port);
        gCustomUdpPorts.push_back(newUDPport);
    } catch (...) {
        SetFalse(a);
        postfl("Could not bind to requested port. This may mean it is in use already by another application.\n");
    }
    return errNone;
}

void closeAllCustomPorts();
void closeAllCustomPorts() {
    // close all custom sockets
    for (int i = 0; i < gCustomUdpPorts.size(); i++) {
        delete gCustomUdpPorts[i];
    }
    gCustomUdpPorts.clear();
}

void cleanup_OSC() {
    postfl("cleaning up OSC\n");

    stopAsioThread();

#ifdef _WIN32
    WSACleanup();
#endif
}

extern boost::asio::io_service ioService;

static int prGetHostByName(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    char hostname[256];

    int err = slotStrVal(a, hostname, 255);
    if (err)
        return err;

#if 1
    struct hostent* he = gethostbyname(hostname);
    if (!he) {
#    ifdef _WIN32
        int err = WSAGetLastError();
        error("gethostbyname(\"%s\") failed with error code %i.\n", hostname, err);
#    endif
        return errFailed;
    }

    SetInt(a, sc_ntohl(*(int*)he->h_addr));

    return errNone;

#else
    boost::asio::ip::address address;
    boost::system::error_code err_c;
    using boost::asio::ip::udp;

    udp::resolver resolver(ioService);
    udp::resolver::query query(hostname, "");
    udp::resolver::iterator iterator = resolver.resolve(query, err_c);

    if (err_c) {
        error("cmp::resolver::query: %s\n", err_c.message().c_str());
        return errFailed;
    } else
        SetInt(a, iterator->endpoint().address().to_v4().to_ulong());

    return errNone;
#endif
}

int prGetLangPort(VMGlobals* g, int numArgsPushed);
int prGetLangPort(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    if (!gUDPport)
        return errFailed;
    SetInt(a, gUDPport->RealPortNum());
    return errNone;
}

int prMatchLangIP(VMGlobals* g, int numArgsPushed);
int prMatchLangIP(VMGlobals* g, int numArgsPushed) {
    PyrSlot* argString = g->sp;
    char ipstring[40];
    int err = slotStrVal(argString, ipstring, 39);
    if (err)
        return err;

    std::string loopback("127.0.0.1");
    // check for loopback address
    if (!loopback.compare(ipstring)) {
        SetTrue(g->sp - 1);
        return errNone;
    }

#ifdef _WIN32

    DWORD rv, size = 0;
    PIP_ADAPTER_ADDRESSES adapter_addresses, aa;
    PIP_ADAPTER_UNICAST_ADDRESS ua;

    // first get the size of the required buffer
    rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, NULL, &size);
    if (rv != ERROR_BUFFER_OVERFLOW) {
        error("GetAdaptersAddresses() failed...");
        return errFailed;
    }
    // now allocate a buffer for the linked list
    adapter_addresses = (PIP_ADAPTER_ADDRESSES)malloc(size);

    rv = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, NULL, adapter_addresses, &size);
    if (rv != ERROR_SUCCESS) {
        error("GetAdaptersAddresses() failed...");
        free(adapter_addresses);
        return errFailed;
    }

    for (aa = adapter_addresses; aa != NULL; aa = aa->Next) {
        for (ua = aa->FirstUnicastAddress; ua != NULL; ua = ua->Next) {
            char buf[40];
            memset(buf, 0, sizeof(buf));
            getnameinfo(ua->Address.lpSockaddr, ua->Address.iSockaddrLength, buf, sizeof(buf), NULL, 0, NI_NUMERICHOST);
            if (strcmp(ipstring, buf) == 0) {
                SetTrue(g->sp - 1);
                free(adapter_addresses);
                return errNone;
            }
        }
    }

    free(adapter_addresses);

#else

    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in* sa;
    char* addr;

    int result = getifaddrs(&ifap);
    if (result) {
        error(strerror(errno));
        return errFailed;
    }

    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        int family = ifa->ifa_addr->sa_family;
        if (family == AF_INET || family == AF_INET6) {
            sa = (struct sockaddr_in*)ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            if (strcmp(ipstring, addr) == 0) {
                SetTrue(g->sp - 1);
                freeifaddrs(ifap);
                return errNone;
            }
        }
    }

    freeifaddrs(ifap);
#endif

    SetFalse(g->sp - 1);
    return errNone;
}

int prExit(VMGlobals* g, int numArgsPushed);
int prExit(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    exit(slotRawInt(a));

    // post("exit %d\n", slotRawInt(a));
    // DumpBackTrace(g);
    return errNone;
}

extern "C" {
int vpost(const char* fmt, va_list vargs);
}

#ifndef NO_INTERNAL_SERVER
int prBootInProcessServer(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    if (!gInternalSynthServer.mWorld) {
        SetPrintFunc(&vpost);
        WorldOptions options;

        PyrObject* optionsObj = slotRawObject(a);
        PyrSlot* optionsSlots = optionsObj->slots;

        static char mInputStreamsEnabled[512], mOutputStreamsEnabled[512], mDeviceName[512];
        int err;

        err = slotIntVal(optionsSlots + 0, (int*)&options.mNumAudioBusChannels);
        if (err)
            return err;

        err = slotIntVal(optionsSlots + 1, (int*)&options.mNumControlBusChannels);
        if (err)
            return err;

        err = slotIntVal(optionsSlots + 2, (int*)&options.mNumInputBusChannels);
        if (err)
            return err;

        err = slotIntVal(optionsSlots + 3, (int*)&options.mNumOutputBusChannels);
        if (err)
            return err;

        err = slotIntVal(optionsSlots + 4, (int*)&options.mNumBuffers);
        if (err)
            return err;

        err = slotIntVal(optionsSlots + 5, (int*)&options.mMaxNodes);
        if (err)
            return err;

        err = slotIntVal(optionsSlots + 6, (int*)&options.mMaxGraphDefs);
        if (err)
            return err;

        err = slotIntVal(optionsSlots + 8, (int*)&options.mBufLength);
        if (err)
            return err;

        if (NotNil(optionsSlots + 9)) {
            err = slotIntVal(optionsSlots + 9, (int*)&options.mPreferredHardwareBufferFrameSize);
            if (err)
                return err;
        }

        err = slotIntVal(optionsSlots + 10, (int*)&options.mRealTimeMemorySize);
        if (err)
            return err;

        err = slotIntVal(optionsSlots + 11, (int*)&options.mNumRGens);
        if (err)
            return err;

        err = slotIntVal(optionsSlots + 12, (int*)&options.mMaxWireBufs);
        if (err)
            return err;

        if (NotNil(optionsSlots + 13)) {
            err = slotIntVal(optionsSlots + 13, (int*)&options.mPreferredSampleRate);
            if (err)
                return err;
        }

        options.mLoadGraphDefs = IsTrue(optionsSlots + 14) ? 1 : 0;

#    ifdef __APPLE__
        err = slotStrVal(optionsSlots + 15, mInputStreamsEnabled, 512);
        if (err)
            options.mInputStreamsEnabled = NULL;
        else
            options.mInputStreamsEnabled = mInputStreamsEnabled;

        err = slotStrVal(optionsSlots + 16, mOutputStreamsEnabled, 512);
        if (err)
            options.mOutputStreamsEnabled = NULL;
        else
            options.mOutputStreamsEnabled = mOutputStreamsEnabled;
#    endif

        err = slotStrVal(optionsSlots + 17, mDeviceName, 512);
        if (err)
            options.mInDeviceName = options.mOutDeviceName = NULL;
        else
            options.mInDeviceName = options.mOutDeviceName = mDeviceName;

        options.mNumSharedControls = gInternalSynthServer.mNumSharedControls;
        options.mSharedControls = gInternalSynthServer.mSharedControls;

        // internal servers use the PID to identify the shared memory region
#    if defined(SC_IPHONE)
        options.mSharedMemoryID = 0;
#    elif !defined(_WIN32)
        options.mSharedMemoryID = getpid();
#    else
        options.mSharedMemoryID = GetCurrentProcessId();
#    endif

        gInternalSynthServer.mWorld = World_New(&options);
    }

    return errNone;
}

int getScopeBuf(uint32 index, SndBuf* buf, bool& didChange) {
    if (gInternalSynthServer.mWorld) {
        int serverErr = World_CopySndBuf(gInternalSynthServer.mWorld, index, buf, true, &didChange);
        if (serverErr)
            return errFailed;
    } else {
        didChange = false;
    }
    return errNone;
}

int prQuitInProcessServer(VMGlobals* g, int numArgsPushed) {
    // PyrSlot *a = g->sp;

    if (gInternalSynthServer.mWorld) {
        World* world = gInternalSynthServer.mWorld;
        gInternalSynthServer.mWorld = 0;

        SC_Thread thread(std::bind(World_WaitForQuit, world, false));

        thread.detach();
    }

    return errNone;
}
#else // NO_INTERNAL_SERVER
int prQuitInProcessServer(VMGlobals* g, int numArgsPushed) {
    // no-op. Better to have this than to overwrite in lang.
    return errNone;
}
#endif // NO_INTERNAL_SERVER

inline int32 BUFMASK(int32 x) { return (1 << (31 - CLZ(x))) - 1; }

int prAllocSharedControls(VMGlobals* g, int numArgsPushed);
int prAllocSharedControls(VMGlobals* g, int numArgsPushed) {
    // PyrSlot *a = g->sp - 1;
    PyrSlot* b = g->sp;

    if (gInternalSynthServer.mWorld) {
        post("can't allocate while internal server is running\n");
        return errNone;
    }
    if (gInternalSynthServer.mSharedControls != gDefaultSharedControls) {
        free(gInternalSynthServer.mSharedControls);
        gInternalSynthServer.mSharedControls = gDefaultSharedControls;
    }
    int numSharedControls;
    int err = slotIntVal(b, &numSharedControls);
    if (err)
        return err;
    if (numSharedControls <= 0) {
        gInternalSynthServer.mNumSharedControls = 0;
    } else if (numSharedControls < kNumDefaultSharedControls) {
        gInternalSynthServer.mNumSharedControls = numSharedControls;
    } else {
        gInternalSynthServer.mNumSharedControls = numSharedControls;
        gInternalSynthServer.mSharedControls = (float*)calloc(numSharedControls, sizeof(float));
    }
    return errNone;
}


int prGetSharedControl(VMGlobals* g, int numArgsPushed);
int prGetSharedControl(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    int index;
    int err = slotIntVal(b, &index);
    if (err)
        return err;
    if (index < 0 || index >= gInternalSynthServer.mNumSharedControls) {
        SetFloat(a, 0.);
        return errNone;
    }
    float val = gInternalSynthServer.mSharedControls[index];
    SetFloat(a, val);
    return errNone;
}

int prSetSharedControl(VMGlobals* g, int numArgsPushed);
int prSetSharedControl(VMGlobals* g, int numArgsPushed) {
    // PyrSlot *a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    int index;
    int err = slotIntVal(b, &index);
    if (err)
        return err;

    float val;
    err = slotFloatVal(c, &val);
    if (err)
        return err;

    if (index < 0 || index >= gInternalSynthServer.mNumSharedControls) {
        return errNone;
    }
    gInternalSynthServer.mSharedControls[index] = val;
    return errNone;
}

static int disconnectSharedMem(VMGlobals* g, PyrObject* object) {
    int ptrIndex = 0;
    PyrSlot* ptrSlot = object->slots + ptrIndex;

    if (IsNil(ptrSlot))
        // already disconnected
        return errNone;

    assert(IsPtr(ptrSlot));

    server_shared_memory_client* client = (server_shared_memory_client*)slotRawPtr(ptrSlot);
    delete client;
    SetNil(ptrSlot);
    return errNone;
}

int prConnectSharedMem(VMGlobals* g, int numArgsPushed) {
#if !defined(SC_IPHONE)
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    assert(IsObj(a));

    PyrObject* self = slotRawObject(a);
    int portNumber = slotRawInt(b);

    int ptrIndex = 0;
    int finalizerIndex = 1;

    try {
        server_shared_memory_client* client = new server_shared_memory_client(portNumber);
        SetPtr(self->slots + ptrIndex, client);

        InstallFinalizer(g, self, finalizerIndex, disconnectSharedMem);

        postfl("Shared memory server interface initialized\n");
    } catch (std::exception& e) {
        postfl("Cannot connect to shared memory: %s\n", e.what());
        return errFailed;
    }
#else
    postfl("Warning: Shared memory server interface disabled on iphone\n");
#endif
    return errNone;
}

int prDisconnectSharedMem(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;

    assert(IsObj(a));
    PyrObject* self = slotRawObject(a);
    return disconnectSharedMem(g, self);
}

int prGetControlBusValue(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;

    assert(IsObj(a));
    PyrObject* self = slotRawObject(a);
    int ptrIndex = 0;
    PyrSlot* ptrSlot = self->slots + ptrIndex;
    if (NotPtr(ptrSlot))
        return errFailed;

    if (!IsInt(b))
        return errFailed;

    int busIndex = slotRawInt(b);

    if (NotPtr(ptrSlot))
        return errFailed;

    server_shared_memory_client* client = (server_shared_memory_client*)slotRawPtr(ptrSlot);

    float value = client->get_control_busses()[busIndex];
    SetFloat(a, value);
    return errNone;
}

int prGetControlBusValues(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    assert(IsObj(a));
    PyrObject* self = slotRawObject(a);
    int ptrIndex = 0;
    PyrSlot* ptrSlot = self->slots + ptrIndex;
    if (NotPtr(ptrSlot))
        return errFailed;

    if (!IsInt(b))
        return errFailed;

    int busIndex = slotRawInt(b);

    if (!IsInt(c))
        return errFailed;

    int numberOfChannels = slotRawInt(c);

    server_shared_memory_client* client = (server_shared_memory_client*)slotRawPtr(ptrSlot);

    PyrObject* ret = newPyrArray(g->gc, numberOfChannels, 0, 1);
    ret->size = numberOfChannels;

    for (int i = 0; i != numberOfChannels; ++i) {
        float value = client->get_control_busses()[busIndex + i];
        SetFloat(ret->slots + i, value);
    }

    SetObject(a, ret);
    return errNone;
}

int prSetControlBusValue(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    assert(IsObj(a));
    PyrObject* self = slotRawObject(a);
    int ptrIndex = 0;
    PyrSlot* ptrSlot = self->slots + ptrIndex;
    if (NotPtr(ptrSlot))
        return errFailed;

    if (!IsInt(b))
        return errFailed;

    int busIndex = slotRawInt(b);

    if (NotPtr(ptrSlot))
        return errFailed;

    float value;
    int error = slotFloatVal(c, &value);
    if (error != errNone)
        return error;

    server_shared_memory_client* client = (server_shared_memory_client*)slotRawPtr(ptrSlot);

    client->get_control_busses()[busIndex] = value;
    return errNone;
}

int prSetControlBusValues(VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 2;
    PyrSlot* b = g->sp - 1;
    PyrSlot* c = g->sp;

    assert(IsObj(a));
    PyrObject* self = slotRawObject(a);
    int ptrIndex = 0;
    PyrSlot* ptrSlot = self->slots + ptrIndex;
    if (NotPtr(ptrSlot))
        return errFailed;

    if (!IsInt(b))
        return errFailed;

    int busIndex = slotRawInt(b);

    if (!IsObj(c))
        return errFailed;

    PyrObject* values = slotRawObject(c);
    server_shared_memory_client* client = (server_shared_memory_client*)slotRawPtr(ptrSlot);
    float* control_busses = client->get_control_busses() + busIndex;

    for (int i = 0; i != values->size; ++i) {
        float value;
        int error = slotFloatVal(values->slots + i, &value);
        if (error != errNone)
            return error;

        control_busses[i] = value;
    }
    return errNone;
}

void init_OSC_primitives();
void init_OSC_primitives() {
    int base, index;

    base = nextPrimitiveIndex();
    index = 0;

    definePrimitive(base, index++, "_NetAddr_Connect", prNetAddr_Connect, 1, 0);
    definePrimitive(base, index++, "_NetAddr_Disconnect", prNetAddr_Disconnect, 1, 0);
    definePrimitive(base, index++, "_NetAddr_SendMsg", prNetAddr_SendMsg, 1, 1);
    definePrimitive(base, index++, "_NetAddr_SendBundle", prNetAddr_SendBundle, 2, 1);
    definePrimitive(base, index++, "_NetAddr_SendRaw", prNetAddr_SendRaw, 2, 0);
    definePrimitive(base, index++, "_NetAddr_GetBroadcastFlag", prNetAddr_GetBroadcastFlag, 1, 0);
    definePrimitive(base, index++, "_NetAddr_SetBroadcastFlag", prNetAddr_SetBroadcastFlag, 2, 0);
    definePrimitive(base, index++, "_NetAddr_BundleSize", prNetAddr_BundleSize, 1, 0);
    definePrimitive(base, index++, "_NetAddr_MsgSize", prNetAddr_MsgSize, 1, 0);

    definePrimitive(base, index++, "_NetAddr_UseDoubles", prNetAddr_UseDoubles, 2, 0);
    definePrimitive(base, index++, "_Array_OSCBytes", prArray_OSCBytes, 1, 0);
    definePrimitive(base, index++, "_GetHostByName", prGetHostByName, 1, 0);
    definePrimitive(base, index++, "_GetLangPort", prGetLangPort, 1, 0);
    definePrimitive(base, index++, "_MatchLangIP", prMatchLangIP, 2, 0);
    definePrimitive(base, index++, "_Exit", prExit, 1, 0);
#ifndef NO_INTERNAL_SERVER
    definePrimitive(base, index++, "_BootInProcessServer", prBootInProcessServer, 1, 0);
#endif
    definePrimitive(base, index++, "_QuitInProcessServer", prQuitInProcessServer, 1, 0);
    definePrimitive(base, index++, "_AllocSharedControls", prAllocSharedControls, 2, 0);
    definePrimitive(base, index++, "_SetSharedControl", prSetSharedControl, 3, 0);
    definePrimitive(base, index++, "_GetSharedControl", prGetSharedControl, 2, 0);
    definePrimitive(base, index++, "_OpenUDPPort", prOpenUDPPort, 2, 0);

    // server shared memory interface
    definePrimitive(base, index++, "_ServerShmInterface_connectSharedMem", prConnectSharedMem, 2, 0);
    definePrimitive(base, index++, "_ServerShmInterface_disconnectSharedMem", prDisconnectSharedMem, 1, 0);
    definePrimitive(base, index++, "_ServerShmInterface_getControlBusValue", prGetControlBusValue, 2, 0);
    definePrimitive(base, index++, "_ServerShmInterface_getControlBusValues", prGetControlBusValues, 3, 0);

    definePrimitive(base, index++, "_ServerShmInterface_setControlBusValue", prSetControlBusValue, 3, 0);
    definePrimitive(base, index++, "_ServerShmInterface_setControlBusValues", prSetControlBusValues, 3, 0);

    // post("initOSCRecs###############\n");
    s_call = getsym("call");
    s_write = getsym("write");
    s_recvoscmsg = getsym("recvOSCmessage");
    s_recvoscbndl = getsym("recvOSCbundle");
    s_netaddr = getsym("NetAddr");
}
