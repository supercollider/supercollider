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

/*
changes by jan trutzschler v. f. 9/9/2002
the midiReadProc calls doAction in the class MIDIIn.
with the arguments: inUid, status, chan, val1, val2
added prDisposeMIDIClient
added prRestartMIDI
19/9 call different actions,disconnect midiInPort, midiout: sendmidi
*/
#include <CoreAudio/HostTime.h>
#include <Carbon/Carbon.h>
#include <CoreMIDI/CoreMIDI.h>
#include "SCBase.h"
#include "VMGlobals.h"
#include "PyrSymbolTable.h"
#include "PyrInterpreter.h"
#include "PyrPrimitive.h"
#include "PyrObjectProto.h"
#include "PyrPrimitiveProto.h"
#include "PyrKernelProto.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSched.h"

PyrSymbol* s_domidiaction;
PyrSymbol* s_midiNoteAction;
PyrSymbol* s_midiTouchAction;
PyrSymbol* s_midiControlAction;
PyrSymbol* s_midiPolyTouchAction;
PyrSymbol* s_midiProgramAction;
PyrSymbol* s_midiBendAction;
//jt
PyrSymbol * s_midiin;


const int kMaxMidiPorts = 16;
MIDIClientRef gMIDIClient = 0;
MIDIPortRef gMIDIInPort[kMaxMidiPorts], gMIDIOutPort[kMaxMidiPorts];
int gNumMIDIInPorts = 0, gNumMIDIOutPorts = 0;
bool gMIDIInitialized = false;

void midiNotifyProc(const MIDINotification *msg, void* refCon)
{
}

static void midiProcessPacket(MIDIPacket *pkt, int uid)
{
 //jt
    if(pkt) {
     pthread_mutex_lock (&gLangMutex); //dont know if this is really needed/seems to be more stable..
		// it is needed  -jamesmcc

      VMGlobals *g = gMainVMGlobals;
      uint8 status = pkt->data[0] & 0xF0;
        g->canCallOS = true;
        
        ++g->sp; SetObject(g->sp, s_midiin->u.classobj); // Set the class MIDIIn
        //set arguments: 
        ++g->sp;SetInt(g->sp,  uid); //src
            // ++g->sp;  SetInt(g->sp, status); //status
        ++g->sp;  SetInt(g->sp, pkt->data[0] - (status & 0xF0)); //chan
        switch (status) {
        case 0x80 : //noteOff
            ++g->sp; SetInt(g->sp,  pkt->data[1]); //val1
            ++g->sp; SetInt(g->sp, 0); //val2 handle noteOff as a noteOn with velocity 0
            runInterpreter(g, s_midiNoteAction, 5);
            break;
        case 0x90 : //noteOn s_midiNoteAction
            ++g->sp; SetInt(g->sp,  pkt->data[1]); //val1
            ++g->sp; SetInt(g->sp,  pkt->data[2]); //val2
            runInterpreter(g, s_midiNoteAction, 5);
            break;
        case 0xA0 : //polytouch
            ++g->sp; SetInt(g->sp,  pkt->data[1]); //val1
            ++g->sp; SetInt(g->sp,  pkt->data[2]); //val2
            runInterpreter(g, s_midiPolyTouchAction, 5);
            break;
        case 0xB0 : //control
            ++g->sp; SetInt(g->sp,  pkt->data[1]); //val1
            ++g->sp; SetInt(g->sp,  pkt->data[2]); //val2
            runInterpreter(g, s_midiControlAction, 5);
            break;
        case 0xC0 : //program
            ++g->sp; SetInt(g->sp,  pkt->data[1]); //val1
            runInterpreter(g, s_midiProgramAction, 4);
            break;
        case 0xD0 : //touch
            ++g->sp; SetInt(g->sp,  pkt->data[1]); //val1
            runInterpreter(g, s_midiTouchAction, 4);
            break;
        case 0xE0 : //bend	this does not work correctly
            ///pkt->data[2]
            ++g->sp; SetInt(g->sp,  pkt->data[1]); //val1
            runInterpreter(g, s_midiBendAction, 4);
            break;
        case 0xF0 :// ?
            break;  
        default :
            ++g->sp; SetInt(g->sp,  pkt->data[1]); //val1
            ++g->sp; SetInt(g->sp,  pkt->data[2]); //val2
            runInterpreter(g, s_domidiaction, 5);
            break;
            
    }
    g->canCallOS = false;
    pthread_mutex_unlock (&gLangMutex); 
    }
}

static void midiReadProc(const MIDIPacketList *pktlist, void* readProcRefCon, void* srcConnRefCon)
{
    MIDIPacket *pkt = (MIDIPacket*)pktlist->packet;
    int uid = (int) srcConnRefCon;
    for (uint32 i=0; i<pktlist->numPackets; ++i) {
        midiProcessPacket(pkt, uid);
        pkt = MIDIPacketNext(pkt);
    }
    
}

void midiCleanUp();

void initMIDI(int numIn, int numOut)
{
	midiCleanUp();
	
    numIn = sc_clip(numIn, 1, kMaxMidiPorts);
    numOut = sc_clip(numOut, 1, kMaxMidiPorts);
    
    int enc = kCFStringEncodingMacRoman;
    CFAllocatorRef alloc = CFAllocatorGetDefault();
    
    CFStringRef clientName = CFStringCreateWithCString(alloc, "SuperCollider", enc);
    
    OSStatus err = MIDIClientCreate(clientName, midiNotifyProc, nil, &gMIDIClient);
    if (err) {
        post("Could not create MIDI client. error %d\n", err);
        return;
    }
    CFRelease(clientName);
    
    for (int i=0; i<numIn; ++i) {
        char str[32];
        sprintf(str, "in%d\n", i);
        CFStringRef inputPortName = CFStringCreateWithCString(alloc, str, enc);
        
        err = MIDIInputPortCreate(gMIDIClient, inputPortName, midiReadProc, &i, gMIDIInPort+i);
        if (err) {
            gNumMIDIInPorts = i;
            post("Could not create MIDI port %s. error %d\n", str, err);
            return;
        }
        CFRelease(inputPortName);
    }
	
	/*int n = MIDIGetNumberOfSources();
	printf("%d sources\n", n);
	for (i = 0; i < n; ++i) {
		MIDIEndpointRef src = MIDIGetSource(i);
		MIDIPortConnectSource(inPort, src, NULL);
	}*/
    
    gNumMIDIInPorts = numIn;
    
    for (int i=0; i<numOut; ++i) {
        char str[32];
        sprintf(str, "out%d\n", i);
        CFStringRef outputPortName = CFStringCreateWithCString(alloc, str, enc);
        
        err = MIDIOutputPortCreate(gMIDIClient, outputPortName, gMIDIOutPort+i);
        if (err) {
            gNumMIDIOutPorts = i;
            post("Could not create MIDI out port. error %d\n", err);
            return;
        }
        
        CFRelease(outputPortName);
    }
    gNumMIDIOutPorts = numIn;
    
}

void midiCleanUp()
{
    for (int i=0; i<gNumMIDIOutPorts; ++i) {
        MIDIPortDispose(gMIDIOutPort[i]);
    }
	gNumMIDIOutPorts = 0;
	
    for (int i=0; i<gNumMIDIInPorts; ++i) {
        MIDIPortDispose(gMIDIInPort[i]);
    }
	gNumMIDIInPorts = 0;
	
    if (gMIDIClient) {
        MIDIClientDispose(gMIDIClient);
		gMIDIClient = 0;
    }
}


void midiListEndpoints()
{
}

/*

need a primitive to connect a source (by name), and disconnect a source.

*/

int prListMIDIEndpoints(struct VMGlobals *g, int numArgsPushed);
int prListMIDIEndpoints(struct VMGlobals *g, int numArgsPushed)
{
    int numSrc = MIDIGetNumberOfSources();
	post("numSrc %d\n",  numSrc);
    for (int i=0; i<numSrc; ++i) {
        MIDIEndpointRef src = MIDIGetSource(i);
		MIDIEntityRef ent;
		MIDIEndpointGetEntity(src, &ent);
		MIDIDeviceRef dev;
		MIDIEntityGetDevice(ent, &dev);
		
        CFStringRef devname, endname;
        MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &devname);
        MIDIObjectGetStringProperty(src, kMIDIPropertyName, &endname);
        SInt32 id;
        MIDIObjectGetIntegerProperty(src, kMIDIPropertyUniqueID, &id);
		char cendname[1024], cdevname[1024];
		CFStringGetCString(devname, cdevname, 1024, kCFStringEncodingUTF8);
		CFStringGetCString(endname, cendname, 1024, kCFStringEncodingUTF8);
		post("in %3d   uid %8d   dev '%s'   endpt '%s'\n", i, id, cdevname, cendname);
        CFRelease(devname);
        CFRelease(endname);
    }
    int numDst = MIDIGetNumberOfDestinations();
	post("numDst %d\n",  numDst);
    for (int i=0; i<numDst; ++i) {
        MIDIEndpointRef dst = MIDIGetDestination(i);
		MIDIEntityRef ent;
		MIDIEndpointGetEntity(dst, &ent);
		MIDIDeviceRef dev;
		MIDIEntityGetDevice(ent, &dev);
		
        CFStringRef devname, endname;
        MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &devname);
        MIDIObjectGetStringProperty(dst, kMIDIPropertyName, &endname);
        SInt32 id;
        MIDIObjectGetIntegerProperty(dst, kMIDIPropertyUniqueID, &id);
		char cendname[1024], cdevname[1024];
		CFStringGetCString(devname, cdevname, 1024, kCFStringEncodingUTF8);
		CFStringGetCString(endname, cendname, 1024, kCFStringEncodingUTF8);
		post("out %3d   uid %8d   dev '%s'   endpt '%s'\n", i, id, cdevname, cendname);
        CFRelease(devname);
        CFRelease(endname);
    }
	return errNone;
}

int prConnectMIDIIn(struct VMGlobals *g, int numArgsPushed);
int prConnectMIDIIn(struct VMGlobals *g, int numArgsPushed)
{
	//PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
        
	int err, inputIndex, uid;
	err = slotIntVal(b, &inputIndex);
	if (err) return err;
	if (inputIndex < 0 || inputIndex >= gNumMIDIInPorts) return errIndexOutOfRange;
	
	err = slotIntVal(c, &uid);
	if (err) return err;

	
	MIDIEndpointRef src=0;
	MIDIObjectType mtype;
	MIDIObjectFindByUniqueID(uid, (MIDIObjectRef*)&src, &mtype);
	if (mtype != kMIDIObjectType_Source) return errFailed;
       
        //pass the uid to the midiReadProc to identify the src
       
	MIDIPortConnectSource(gMIDIInPort[inputIndex], src, (void*)uid);
	
	return errNone;
}
int prDisconnectMIDIIn(struct VMGlobals *g, int numArgsPushed);
int prDisconnectMIDIIn(struct VMGlobals *g, int numArgsPushed)
{
        PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
        
	int err, inputIndex, uid;
	err = slotIntVal(b, &inputIndex);
	if (err) return err;
	if (inputIndex < 0 || inputIndex >= gNumMIDIInPorts) return errIndexOutOfRange;
	err = slotIntVal(c, &uid);
	if (err) return err;

	MIDIEndpointRef src=0;
	MIDIObjectType mtype;
	MIDIObjectFindByUniqueID(uid, (MIDIObjectRef*)&src, &mtype);
	if (mtype != kMIDIObjectType_Source) return errFailed;

	MIDIPortDisconnectSource(gMIDIInPort[inputIndex], src);
	
	return errNone;

}
int prInitMIDI(struct VMGlobals *g, int numArgsPushed);
int prInitMIDI(struct VMGlobals *g, int numArgsPushed)
{
	//PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
        
	int err, numIn, numOut;
	err = slotIntVal(b, &numIn);
	if (err) return err;
	
	err = slotIntVal(c, &numOut);
	if (err) return err;
	
	initMIDI(numIn, numOut);
	
	return errNone;
}
int prDisposeMIDIClient(VMGlobals *g, int numArgsPushed);
int prDisposeMIDIClient(VMGlobals *g, int numArgsPushed)
{
    PyrSlot *a;
    a = g->sp - 1;
    OSStatus err = MIDIClientDispose(gMIDIClient);
    if (err) post("error could not dispose MIDIClient \n");
    return errNone;	

}
int prRestartMIDI(VMGlobals *g, int numArgsPushed);
int prRestartMIDI(VMGlobals *g, int numArgsPushed)
{
    MIDIRestart();
    return errNone;	
}

void sendmidi(int port, MIDIEndpointRef dest, int length, int chan, int status, int aval, int bval, float late);
void sendmidi(int port, MIDIEndpointRef dest, int length, int chan, int status, int aval, int bval, float late)
{
    MIDIPacketList mpktlist;
    MIDIPacketList * pktlist = &mpktlist;
    MIDIPacket * pk = MIDIPacketListInit(pktlist);
    //lets add some latency
    float  latency =  1000000 * late ; //ms to nano 
    UInt64  utime = AudioConvertNanosToHostTime( AudioConvertHostTimeToNanos(AudioGetCurrentHostTime()) + (UInt64)latency);
    ByteCount nData = (ByteCount) length;
    pk->data[0] = (Byte) chan + (status & 0xF0);
    pk->data[1] = (Byte) aval;
    pk->data[2] = (Byte) bval;
    pk = MIDIPacketListAdd(pktlist, sizeof(struct MIDIPacketList) , pk,(MIDITimeStamp) utime,nData,pk->data);
   	/*OSStatus error =*/ MIDISend(gMIDIOutPort[port],  dest, pktlist );
}

int prSendMIDIOut(struct VMGlobals *g, int numArgsPushed);
int prSendMIDIOut(struct VMGlobals *g, int numArgsPushed)
{
        //port, uid, len, status, chan , a, b, latency
	//PyrSlot *m = g->sp - 8;
	PyrSlot *p = g->sp - 7;
        
	PyrSlot *u = g->sp - 6;
	PyrSlot *l = g->sp - 5;
        
	PyrSlot *s = g->sp - 4;
	PyrSlot *c = g->sp - 3;
        
    PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
    PyrSlot *plat = g->sp;

        
	int err, outputIndex, uid, length, chan, status, aval, bval;
    float late;
	err = slotIntVal(p, &outputIndex);
	if (err) return err;
	if (outputIndex < 0 || outputIndex >= gNumMIDIInPorts) return errIndexOutOfRange;
	
	err = slotIntVal(u, &uid);
	if (err) return err;
        err = slotIntVal(l, &length);
	if (err) return err;
        err = slotIntVal(s, &status);
	if (err) return err;
        err = slotIntVal(c, &chan);
	if (err) return err;
        err = slotIntVal(a, &aval);
	if (err) return err;
        err = slotIntVal(b, &bval);
	if (err) return err;
        err = slotFloatVal(plat, &late);
	if (err) return err;

	MIDIEndpointRef dest;
	MIDIObjectType mtype;
	MIDIObjectFindByUniqueID(uid, (MIDIObjectRef*)&dest, &mtype);
	if (mtype != kMIDIObjectType_Destination) return errFailed;
		
	if (!dest) return errFailed;

    sendmidi(outputIndex, dest, length, chan, status, aval, bval, late);
    return errNone;
}

void initMIDIPrimitives()
{
	int base, index;
        
	base = nextPrimitiveIndex();
	index = 0;
        
	s_midiin = getsym("MIDIIn");
    s_domidiaction = getsym("doAction");
    s_midiNoteAction = getsym("doNoteAction");
    s_midiTouchAction = getsym("doTouchAction");
    s_midiControlAction = getsym("doControlAction");
    s_midiPolyTouchAction = getsym("doPolyTouchAction");
    s_midiProgramAction = getsym("doProgramAction");
    s_midiBendAction = getsym("doBendAction");

    definePrimitive(base, index++, "_ListMIDIEndpoints", prListMIDIEndpoints, 1, 0);	
	definePrimitive(base, index++, "_InitMIDI", prInitMIDI, 3, 0);	
	definePrimitive(base, index++, "_ConnectMIDIIn", prConnectMIDIIn, 3, 0);
	definePrimitive(base, index++, "_DisconnectMIDIIn", prDisconnectMIDIIn, 3, 0);
	definePrimitive(base, index++, "_DisposeMIDIClient", prDisposeMIDIClient, 1, 0);
	definePrimitive(base, index++, "_RestartMIDI", prRestartMIDI, 1, 0);
        
    definePrimitive(base, index++, "_SendMIDIOut", prSendMIDIOut, 9, 0);
    if(gMIDIClient) midiCleanUp();
}
