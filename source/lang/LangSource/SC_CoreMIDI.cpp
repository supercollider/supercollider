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


#include <Carbon/Carbon.h>
#include <CoreMIDI/CoreMIDI.h>
#include "SCBase.h"
#include "SC_BoundsMacros.h"
#include "PyrPrimitive.h"
#include "VMGlobals.h"

extern PyrSymbol* s_doaction;

const int kMaxMidiPorts = 16;
MIDIClientRef gMIDIClient = 0;
MIDIPortRef gMIDIInPort[kMaxMidiPorts], gMIDIOutPort[kMaxMidiPorts];
int gNumMIDIInPorts = 0, gNumMIDIOutPorts = 0;
bool gMIDIInitialized = false;

void midiNotifyProc(const MIDINotification *msg, void* refCon)
{
}

void midiProcessPacket(MIDIPacket *pkt)
{
    uint8 status = pkt->data[0] & 0xF0;
    switch (status) {
        case 0x80 :
            break;
        case 0x90 :
            break;
        case 0xA0 :
            break;
        case 0xB0 :
            break;
        case 0xC0 :
            break;
        case 0xD0 :
            break;
        case 0xE0 :
            break;
        case 0xF0 :
            break;  
    }
}

void midiReadProc(const MIDIPacketList *pktlist, void* readProcRefCon, void* srcConnRefCon)
{
    MIDIPacket *pkt = (MIDIPacket*)pktlist->packet;
    for (int i=0; i<pktlist->numPackets; ++i) {
        midiProcessPacket(pkt);
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
        
        err = MIDIInputPortCreate(gMIDIClient, inputPortName, midiReadProc, nil, gMIDIInPort+i);
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
        CFStringRef name;
        MIDIObjectGetStringProperty(src, kMIDIPropertyName, &name);
        SInt32 id;
        MIDIObjectGetIntegerProperty(src, kMIDIPropertyUniqueID, &id);
		char cname[1024];
		CFStringGetCString(name, cname, 1024, kCFStringEncodingUTF8);
		post("in  %3d %8d %s\n", i, id, cname);
        CFRelease(name);
    }
    int numDst = MIDIGetNumberOfDestinations();
	post("numDst %d\n",  numDst);
    for (int i=0; i<numDst; ++i) {
        MIDIEndpointRef dst = MIDIGetDestination(i);
        CFStringRef name;
        MIDIObjectGetStringProperty(dst, kMIDIPropertyName, &name);
        SInt32 id;
        MIDIObjectGetIntegerProperty(dst, kMIDIPropertyUniqueID, &id);
		char cname[1024];
		CFStringGetCString(name, cname, 1024, kCFStringEncodingUTF8);
		post("out %3d   uid %8d   name %s\n", i, id, cname);
        CFRelease(name);
    }
	return errNone;
}

MIDIEndpointRef MIDIGetSourceByUID(SInt32 inUID);
MIDIEndpointRef MIDIGetSourceByUID(SInt32 inUID)
{
	int n = MIDIGetNumberOfSources();
	for (int i = 0; i < n; ++i) {
		MIDIEndpointRef src = MIDIGetSource(i);
		SInt32 uid;
		MIDIObjectGetIntegerProperty(src, kMIDIPropertyUniqueID, &uid);
		if (uid == inUID) return src;
	}
	return 0;
}

int prConnectMIDIIn(struct VMGlobals *g, int numArgsPushed);
int prConnectMIDIIn(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;
	
	int err, inputIndex, uid;
	err = slotIntVal(b, &inputIndex);
	if (err) return err;
	if (inputIndex < 0 || inputIndex >= gNumMIDIInPorts) return errIndexOutOfRange;
	
	err = slotIntVal(c, &uid);
	if (err) return err;
	
	gMIDIInPort[inputIndex];

	MIDIEndpointRef src = MIDIGetSourceByUID(uid);
	if (!src) return errFailed;
	MIDIPortConnectSource(gMIDIInPort[inputIndex], src, NULL);
	
	return errNone;
}

int prInitMIDI(struct VMGlobals *g, int numArgsPushed);
int prInitMIDI(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2;
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

void initMIDIPrimitives()
{
	int base, index;
        
	base = nextPrimitiveIndex();
	index = 0;
	
	definePrimitive(base, index++, "_ListMIDIEndpoints", prListMIDIEndpoints, 1, 0);	
	definePrimitive(base, index++, "_InitMIDI", prInitMIDI, 3, 0);	
	definePrimitive(base, index++, "_ConnectMIDIIn", prConnectMIDIIn, 3, 0);	
	
}
