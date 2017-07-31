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

/*
changes by charles picasso 14/april/2008 (sysex parsing + added running status)
changes by jan trutzschler v. f. 9/9/2002
the midiReadProc calls doAction in the class MIDIIn.
with the arguments: inUid, status, chan, val1, val2
added prDisposeMIDIClient
added prRestartMIDI
19/9 call different actions,disconnect midiInPort, midiout: sendmidi
04/feb/03 prListMIDIEndpoints modification by Ron Kuivila added jt.
*/
#if SC_IPHONE
#include <mach/mach_time.h>
#else
#include <CoreAudio/HostTime.h>
#include <Carbon/Carbon.h>
#endif
#include <CoreMIDI/CoreMIDI.h>
#include <vector>
#include "SCBase.h"
#include "VMGlobals.h"
#include "PyrSymbolTable.h"
#include "PyrInterpreter.h"
#include "PyrKernel.h"

#include "PyrPrimitive.h"
#include "PyrObjectProto.h"
#include "PyrPrimitiveProto.h"
#include "PyrKernelProto.h"
#include "SC_InlineUnaryOp.h"
#include "SC_InlineBinaryOp.h"
#include "PyrSched.h"
#include "GC.h"

PyrSymbol* s_domidiaction;
PyrSymbol* s_midiNoteOnAction;
PyrSymbol* s_midiNoteOffAction;
PyrSymbol* s_midiTouchAction;
PyrSymbol* s_midiControlAction;
PyrSymbol* s_midiPolyTouchAction;
PyrSymbol* s_midiProgramAction;
PyrSymbol* s_midiBendAction;
PyrSymbol* s_midiSysexAction;
PyrSymbol* s_midiInvalidSysexAction;
PyrSymbol* s_midiSysrtAction;
PyrSymbol* s_midiSMPTEAction;
//jt
PyrSymbol * s_midiin;
PyrSymbol * s_numMIDIDev;
PyrSymbol * s_midiclient;
const int kMaxMidiPorts = 128;
MIDIClientRef gMIDIClient = 0;
MIDIPortRef gMIDIInPort[kMaxMidiPorts], gMIDIOutPort[kMaxMidiPorts];
int gNumMIDIInPorts = 0, gNumMIDIOutPorts = 0;
bool gMIDIInitialized = false;
//cp
static bool gSysexFlag = false;
static Byte gRunningStatus = 0;
std::vector<Byte> gSysexData;

void midiNotifyProc(const MIDINotification *msg, void* refCon)
{
}

extern bool compiledOK;

#if 0
static void dumpSysexData() {
	if(gSysexData.size() <= 0)
		return;
	std::vector<Byte>::const_iterator iter = gSysexData.begin(), end = gSysexData.end();
	int i=0;
	while(iter != end) {
		if((i % 16) == 0 && (i > 0))
			printf("\n");
		++i;
		printf("%02X ", *iter++);
	}
	printf("\n");
	printf("sysex data dump size:  %i bytes.\n", gSysexData.size());
}
#endif

static void sysexBegin() {
	gRunningStatus = 0; // clear running status
	gSysexData.clear();
	gSysexFlag = true;
}

static void scCallSysexAction(PyrSymbol* action, int recoverFromUID) {
	VMGlobals *g = gMainVMGlobals;
	if(recoverFromUID) { // rebuild the VM so sc won't crash with two following calls
		++g->sp; SetObject(g->sp, s_midiin->u.classobj); // Set the class MIDIIn
		++g->sp; SetInt(g->sp,  recoverFromUID); //src
		++g->sp;
	}
	PyrInt8Array* sysexArray = newPyrInt8Array(g->gc, gSysexData.size(), 0, true);
	sysexArray->size = gSysexData.size();
	std::copy(gSysexData.begin(), gSysexData.end(), sysexArray->b);
	SetObject(g->sp, (PyrObject*) sysexArray);			// chan argument unneeded as there
	runInterpreter(g, action, 3 );			// special sysex action in the lang
}

static void sysexEnd(int lastUID) {
	gSysexFlag = false;
	scCallSysexAction(s_midiSysexAction, lastUID);
}

static void sysexEndInvalid() {
	gSysexFlag = false;
	scCallSysexAction(s_midiInvalidSysexAction, 0);
}

static int midiProcessSystemPacket(MIDIPacket *pkt, int chan) {
	int index, data;
	VMGlobals *g = gMainVMGlobals;
	switch (chan) {
	case 7: // added cp: Sysex EOX must be taken into account if first on data packet
	case 0:
		{
		int last_uid = 0;
		int m = pkt->length;
		Byte* p_pkt = pkt->data;
		Byte pktval;

		while(m--) {
			pktval = *p_pkt++;
			if(pktval & 0x80) { // status byte
				if(pktval == 0xF7) { // end packet
					gSysexData.push_back(pktval); // add EOX
					if(gSysexFlag)
						sysexEnd(last_uid); // if last_uid != 0 rebuild the VM.
					else
						sysexEndInvalid(); // invalid 1 byte with only EOX can happen
					break;
				}
				else if(pktval == 0xF0) { // new packet
					if(gSysexFlag) {// invalid new one/should not happen -- but handle in case
						// store the last uid value previous to invalid data to rebuild VM after sysexEndInvalid call
						// since it may call sysexEnd() just after it !
						if(slotIntVal(g->sp-1, &last_uid)) {
							post("error: failed retrieving uid value !");
							last_uid = -1;
						}
						sysexEndInvalid();
					}
					sysexBegin(); // new sysex in
					gSysexData.push_back(pktval); // add SOX
				}
				else {// abnormal data in middle of sysex packet
					gSysexData.push_back(pktval); // add it as an abort message
					sysexEndInvalid(); // flush invalid
					m = 0; // discard all packet
					break;
				}
			}
			else if(gSysexFlag)
				gSysexData.push_back(pktval); // add Byte
			else // garbage - handle in case - discard it
				break;
		}
		return (pkt->length-m);
		}
	break;

	case 1 :
		index = pkt->data[1] >> 4;
		data  = pkt->data[1] & 0xf;
		switch (index) { case 1: case 3: case 5: case 7: { data = data << 4; } }
		SetInt(g->sp,  index);		 						// chan unneeded
		++g->sp; SetInt(g->sp, data);						// special smpte action in the lang
		runInterpreter(g, s_midiSMPTEAction, 4 );
		return 2;

	case 2 : 	//songptr
		++g->sp; SetInt(g->sp,  (pkt->data[2] << 7) | pkt->data[1]); //val1
		runInterpreter(g, s_midiSysrtAction, 4);
		return 3;

	case 3 :	// song select
		++g->sp; SetInt(g->sp,  pkt->data[1]); //val1
		runInterpreter(g, s_midiSysrtAction, 4);
		return 2;

	case 6:		//tunerequest
	case 8:		//clock
	case 9:		//tick
	case 10:	//start
	case 11:	//continue
	case 12: 	//stop
	case 14:	//activeSense
	case 15:	//reset
		gRunningStatus = 0; // clear running status
		runInterpreter(g, s_midiSysrtAction, 3);
		return 1;

	default:
		g->sp -= 3; // nevermind
		break;
	}

	return (1);
}

static void midiProcessPacket(MIDIPacket *pkt, size_t uid)
{
//jt
	if(pkt) {
	gLangMutex.lock(); //dont know if this is really needed/seems to be more stable..
		// it is needed  -jamesmcc
	if (compiledOK) {
			VMGlobals *g = gMainVMGlobals;

			int i = 0; //cp : changed uint8 to int if packet->length >= 256 bug:(infinite loop)
			while (i < pkt->length) {
				uint8 status = pkt->data[i] & 0xF0;
				uint8 chan = pkt->data[i] & 0x0F;
				g->canCallOS = false; // cannot call the OS

				++g->sp; SetObject(g->sp, s_midiin->u.classobj); // Set the class MIDIIn
				//set arguments:
				++g->sp; SetInt(g->sp, uid); //src
					// ++g->sp;  SetInt(g->sp, status); //status
				++g->sp; SetInt(g->sp, chan); //chan

				if(status & 0x80) // set the running status for voice messages
					gRunningStatus = ((status >> 4) == 0xF) ? 0 : pkt->data[i]; // keep also additional info
			L:
				switch (status) {
				case 0x80 : //noteOff
					++g->sp; SetInt(g->sp, pkt->data[i+1]); //val1
					++g->sp; SetInt(g->sp, pkt->data[i+2]); //val2
					runInterpreter(g, s_midiNoteOffAction, 5);
					i += 3;
					break;
				case 0x90 : //noteOn
					++g->sp; SetInt(g->sp, pkt->data[i+1]); //val1
					++g->sp; SetInt(g->sp, pkt->data[i+2]); //val2
// 					runInterpreter(g, pkt->data[i+2] ? s_midiNoteOnAction : s_midiNoteOffAction, 5);
					runInterpreter(g, s_midiNoteOnAction, 5);
					i += 3;
					break;
				case 0xA0 : //polytouch
					++g->sp; SetInt(g->sp, pkt->data[i+1]); //val1
					++g->sp; SetInt(g->sp, pkt->data[i+2]); //val2
					runInterpreter(g, s_midiPolyTouchAction, 5);
					i += 3;
					break;
				case 0xB0 : //control
					++g->sp; SetInt(g->sp, pkt->data[i+1]); //val1
					++g->sp; SetInt(g->sp, pkt->data[i+2]); //val2
					runInterpreter(g, s_midiControlAction, 5);
					i += 3;
					break;
				case 0xC0 : //program
					++g->sp; SetInt(g->sp, pkt->data[i+1]); //val1
					runInterpreter(g, s_midiProgramAction, 4);
					i += 2;
					break;
				case 0xD0 : //touch
					++g->sp; SetInt(g->sp,  pkt->data[i+1]); //val1
					runInterpreter(g, s_midiTouchAction, 4);
					i += 2;
					break;
				case 0xE0 : //bend
					++g->sp; SetInt(g->sp,  (pkt->data[i+2] << 7) | pkt->data[i+1]); //val1
					runInterpreter(g, s_midiBendAction, 4);
					i += 3;
					break;
				case 0xF0 :
					i += midiProcessSystemPacket(pkt, chan);
					break;
				default :	// data byte => continuing sysex message
					if(gRunningStatus && !gSysexFlag) { // modified cp: handling running status. may be we should here
						status = gRunningStatus & 0xF0; // accept running status only inside a packet beginning
						chan = gRunningStatus & 0x0F;	// with a valid status byte ?
						SetInt(g->sp, chan);
						--i;
						goto L; // parse again with running status set
					}
					chan = 0;
					i += midiProcessSystemPacket(pkt, chan);
					break;
			}
		}
		g->canCallOS = false;
	}
	gLangMutex.unlock();
	}
}

static void midiReadProc(const MIDIPacketList *pktlist, void* readProcRefCon, void* srcConnRefCon)
{
	MIDIPacket *pkt = (MIDIPacket*)pktlist->packet;
	size_t uid = (size_t) srcConnRefCon;
	for (uint32 i=0; i<pktlist->numPackets; ++i) {
		midiProcessPacket(pkt, uid);
		pkt = MIDIPacketNext(pkt);
	}
}

int midiCleanUp();

int initMIDI(int numIn, int numOut)
{
	OSStatus err;
	CFAllocatorRef alloc = CFAllocatorGetDefault();
	int enc = kCFStringEncodingMacRoman;

	midiCleanUp();
	if(numIn > kMaxMidiPorts) { printf("MIDI: note that maximum midi in ports is limited to %d.\n", kMaxMidiPorts); }
	if(numOut > kMaxMidiPorts) { printf("MIDI: note that maximum midi out ports is limited to %d.\n", kMaxMidiPorts); }
	numIn = sc_clip(numIn, 1, kMaxMidiPorts);
	numOut = sc_clip(numOut, 1, kMaxMidiPorts);

	if (!gMIDIClient) {
		CFStringRef clientName = CFStringCreateWithCString(alloc, "SuperCollider", enc);

		err = MIDIClientCreate(clientName, midiNotifyProc, nil, &gMIDIClient);
		if (err) {
			post("Could not create MIDI client. error: ");
			return errFailed;
		}
		CFRelease(clientName);
	}

	for (int i=0; i<numIn; ++i) {
		char str[32];
		sprintf(str, "in%d\n", i);
		CFStringRef inputPortName = CFStringCreateWithCString(alloc, str, enc);

		err = MIDIInputPortCreate(gMIDIClient, inputPortName, midiReadProc, &i, gMIDIInPort+i);
		if (err) {
			gNumMIDIInPorts = i;
			post("Could not create MIDI port %s. error %d\n", str, err);
			return errFailed;
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
			return errFailed;
		}

		CFRelease(outputPortName);
	}
	gNumMIDIOutPorts = numOut;
	return errNone;
}

int midiCleanUp()
{
	/*
	* do not catch errors when disposing ports
	* MIDIClientDispose should normally dispose the ports attached to it
	* but clean up the pointers in case
	*/
	int i = 0;
	for (i=0; i<gNumMIDIOutPorts; ++i) {
		MIDIPortDispose(gMIDIOutPort[i]);
		gMIDIOutPort[i] = 0;
	}
	gNumMIDIOutPorts = 0;

	for (i=0; i<gNumMIDIInPorts; ++i) {
		MIDIPortDispose(gMIDIInPort[i]);
		gMIDIInPort[i] = 0;
	}
	gNumMIDIInPorts = 0;

// Disposing and immediately re-initializing an identical midi client can cause crashes. Don't dispose - instead, reuse the one we've created already.
//	if (gMIDIClient) {
//		if( MIDIClientDispose(gMIDIClient) ) {
//			post( "Error: failed to dispose MIDIClient\n" );
//			return errFailed;
//		}
//		gMIDIClient = 0;
//	}
	return errNone;
}


void midiListEndpoints()
{
}



int prListMIDIEndpoints(struct VMGlobals *g, int numArgsPushed);
int prListMIDIEndpoints(struct VMGlobals *g, int numArgsPushed)
{
	OSStatus error;
	PyrSlot *a = g->sp;
	int numSrc = MIDIGetNumberOfSources();
	int numDst = MIDIGetNumberOfDestinations();

	PyrObject* idarray = newPyrArray(g->gc, 6 * sizeof(PyrObject), 0 , true);
		SetObject(a, idarray);

	PyrObject* idarraySo = newPyrArray(g->gc, numSrc * sizeof(SInt32), 0 , true);
		SetObject(idarray->slots+idarray->size++, idarraySo);
		g->gc->GCWriteNew(idarray, idarraySo); // we know idarraySo is white so we can use GCWriteNew

	PyrObject* devarraySo = newPyrArray(g->gc, numSrc * sizeof(PyrObject), 0 , true);
		SetObject(idarray->slots+idarray->size++, devarraySo);
		g->gc->GCWriteNew(idarray, devarraySo); // we know devarraySo is white so we can use GCWriteNew

		PyrObject* namearraySo = newPyrArray(g->gc, numSrc * sizeof(PyrObject), 0 , true);
		SetObject(idarray->slots+idarray->size++, namearraySo);
		g->gc->GCWriteNew(idarray, namearraySo); // we know namearraySo is white so we can use GCWriteNew

	PyrObject* idarrayDe = newPyrArray(g->gc, numDst * sizeof(SInt32), 0 , true);
		SetObject(idarray->slots+idarray->size++, idarrayDe);
		g->gc->GCWriteNew(idarray, idarrayDe); // we know idarrayDe is white so we can use GCWriteNew

	PyrObject* namearrayDe = newPyrArray(g->gc, numDst * sizeof(PyrObject), 0 , true);
		SetObject(idarray->slots+idarray->size++, namearrayDe);
		g->gc->GCWriteNew(idarray, namearrayDe); // we know namearrayDe is white so we can use GCWriteNew

	PyrObject* devarrayDe = newPyrArray(g->gc, numDst * sizeof(PyrObject), 0 , true);
		SetObject(idarray->slots+idarray->size++, devarrayDe);
		g->gc->GCWriteNew(idarray, devarrayDe); // we know devarrayDe is white so we can use GCWriteNew


	for (int i=0; i<numSrc; ++i) {
		MIDIEndpointRef src = MIDIGetSource(i);
		SInt32 id;
		MIDIObjectGetIntegerProperty(src, kMIDIPropertyUniqueID, &id);

		MIDIEntityRef ent;
		error = MIDIEndpointGetEntity(src, &ent);

		CFStringRef devname, endname;
		char cendname[1024], cdevname[1024];

		// Virtual sources don't have entities
		if(error)
		{
			MIDIObjectGetStringProperty(src, kMIDIPropertyName, &devname);
			MIDIObjectGetStringProperty(src, kMIDIPropertyName, &endname);
			if (devname) CFStringGetCString(devname, cdevname, 1024, kCFStringEncodingUTF8);
			if (endname) CFStringGetCString(endname, cendname, 1024, kCFStringEncodingUTF8);
		}
		else
		{
			MIDIDeviceRef dev;

			MIDIEntityGetDevice(ent, &dev);
			MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &devname);
			MIDIObjectGetStringProperty(src, kMIDIPropertyName, &endname);
			if (devname) CFStringGetCString(devname, cdevname, 1024, kCFStringEncodingUTF8);
			if (endname) CFStringGetCString(endname, cendname, 1024, kCFStringEncodingUTF8);
		}

		PyrString *string = newPyrString(g->gc, cendname, 0, true);
		SetObject(namearraySo->slots+i, string);
		namearraySo->size++;
		g->gc->GCWriteNew(namearraySo, (PyrObject*)string); // we know string is white so we can use GCWriteNew

		PyrString *devstring = newPyrString(g->gc, cdevname, 0, true);
		SetObject(devarraySo->slots+i, devstring);
		devarraySo->size++;
		g->gc->GCWriteNew(devarraySo, (PyrObject*)devstring); // we know devString is white so we can use GCWriteNew

		SetInt(idarraySo->slots+i, id);
		idarraySo->size++;

		if (devname) CFRelease(devname);
		if (endname) CFRelease(endname);
	}



//      post("numDst %d\n",  numDst);
	for (int i=0; i<numDst; ++i) {
		MIDIEndpointRef dst = MIDIGetDestination(i);
		SInt32 id;
		MIDIObjectGetIntegerProperty(dst, kMIDIPropertyUniqueID, &id);

		MIDIEntityRef ent;
		error = MIDIEndpointGetEntity(dst, &ent);

		CFStringRef devname, endname;
		char cendname[1024], cdevname[1024];

		// Virtual destinations don't have entities either
		if(error)
		{
			MIDIObjectGetStringProperty(dst, kMIDIPropertyName, &devname);
			MIDIObjectGetStringProperty(dst, kMIDIPropertyName, &endname);
			if (devname) CFStringGetCString(devname, cdevname, 1024, kCFStringEncodingUTF8);
			if (endname) CFStringGetCString(endname, cendname, 1024, kCFStringEncodingUTF8);

		}
		else
		{
			MIDIDeviceRef dev;

			MIDIEntityGetDevice(ent, &dev);
			MIDIObjectGetStringProperty(dev, kMIDIPropertyName, &devname);
			MIDIObjectGetStringProperty(dst, kMIDIPropertyName, &endname);
			if (devname) CFStringGetCString(devname, cdevname, 1024, kCFStringEncodingUTF8);
			if (endname) CFStringGetCString(endname, cendname, 1024, kCFStringEncodingUTF8);
		}

		PyrString *string = newPyrString(g->gc, cendname, 0, true);
		SetObject(namearrayDe->slots+namearrayDe->size++, string);
		g->gc->GCWriteNew(namearrayDe, (PyrObject*)string); // we know string is white so we can use GCWriteNew

		PyrString *devstring = newPyrString(g->gc, cdevname, 0, true);

		SetObject(devarrayDe->slots+devarrayDe->size++, devstring);
		g->gc->GCWriteNew(devarrayDe, (PyrObject*)devstring); // we know devstring is white so we can use GCWriteNew

		SetInt(idarrayDe->slots+idarrayDe->size++, id);

		if (devname) CFRelease(devname);
		if (endname) CFRelease(endname);

	}
	return errNone;
}



int prConnectMIDIIn(struct VMGlobals *g, int numArgsPushed);
int prConnectMIDIIn(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *inputIndexSlot = g->sp - 1;
	PyrSlot *uidSlot = g->sp;

	int err, inputIndex, uid;

	err = slotIntVal(inputIndexSlot, &inputIndex);
	if (err)
		return errWrongType;
	if (inputIndex < 0 || inputIndex >= gNumMIDIInPorts)
		return errIndexOutOfRange;

	err = slotIntVal(uidSlot, &uid);
	if (err)
		return errWrongType;

	MIDIEndpointRef src = 0;
	MIDIObjectType mtype;
	MIDIObjectFindByUniqueID(uid, &src, &mtype);
	if (mtype != kMIDIObjectType_Source)
		return errFailed;

	// MIDIPortConnectSource's third parameter is just a unique value used to help
	// identify the source. It expects a void*, so we reinterpret_cast from uid to
	// avoid a compiler warning.
	// See: https://stackoverflow.com/questions/9051292/midireadproc-using-srcconnrefcon-to-listen-to-only-one-source
	MIDIPortConnectSource(gMIDIInPort[inputIndex], src, reinterpret_cast<void*>(uid));

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
	MIDIObjectFindByUniqueID(uid, &src, &mtype);
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
	if (err) return errWrongType;

	err = slotIntVal(c, &numOut);
	if (err) return errWrongType;

	return initMIDI(numIn, numOut);
}
int prDisposeMIDIClient(VMGlobals *g, int numArgsPushed);
int prDisposeMIDIClient(VMGlobals *g, int numArgsPushed)
{
	return midiCleanUp();
}
int prRestartMIDI(VMGlobals *g, int numArgsPushed);
int prRestartMIDI(VMGlobals *g, int numArgsPushed)
{
	MIDIRestart();
	return errNone;
}

void freeSysex(MIDISysexSendRequest* pk);
void freeSysex(MIDISysexSendRequest* pk)
{
	free(pk);
}


int prSendSysex(VMGlobals *g, int numArgsPushed);
int prSendSysex(VMGlobals *g, int numArgsPushed)
{
	int err, uid, size;

	if( !isKindOfSlot(g->sp, s_int8array->u.classobj) )
		return errWrongType;

	PyrInt8Array* packet = slotRawInt8Array(g->sp);
	size = packet->size;

	PyrSlot *u = g->sp - 1;
	err = slotIntVal(u, &uid);
	if (err) return err;

	MIDIEndpointRef dest;
	MIDIObjectType mtype;
	MIDIObjectFindByUniqueID(uid, (MIDIObjectRef*)&dest, &mtype);
	if (mtype != kMIDIObjectType_Destination) return errFailed;
	if (!dest) return errFailed;

	MIDISysexSendRequest *pk = (MIDISysexSendRequest*) malloc (sizeof(MIDISysexSendRequest) + size);
	Byte *data = (Byte *)pk + sizeof(MIDISysexSendRequest);

	memcpy(data,packet->b, size);
	pk->complete = false;
	pk -> destination = dest;
	pk -> data = data;
	pk -> bytesToSend = size;
	pk->completionProc = freeSysex;
	pk->completionRefCon = 0;

	return ((MIDISendSysex(pk) == (OSStatus)0) ? errNone : errFailed);
}

#if SC_IPHONE

static struct mach_timebase_info machTimebaseInfo() {
    struct mach_timebase_info info;
    mach_timebase_info(&info);
    return info;
}

static MIDITimeStamp midiTime(float latencySeconds)
{
    // add the latency expressed in seconds, to the current host time base.
    static struct mach_timebase_info info = machTimebaseInfo(); // cache the timebase info.
    Float64 latencyNanos = 1000000000 * latencySeconds;
    MIDITimeStamp latencyMIDI = (latencyNanos / (Float64)info.numer) * (Float64)info.denom;
    return (MIDITimeStamp)mach_absolute_time() + latencyMIDI;
}

#else

static MIDITimeStamp midiTime(float latencySeconds)
{
    // add the latency expressed in seconds, to the current host time base.
    UInt64 latencyNanos =  1000000000 * latencySeconds ; //secs to nano
    return (MIDITimeStamp)AudioGetCurrentHostTime() + AudioConvertNanosToHostTime(latencyNanos);
}

#endif

void sendmidi(int port, MIDIEndpointRef dest, int length, int hiStatus, int loStatus, int aval, int bval, float late);
void sendmidi(int port, MIDIEndpointRef dest, int length, int hiStatus, int loStatus, int aval, int bval, float late)
{
	MIDIPacketList mpktlist;
	MIDIPacketList * pktlist = &mpktlist;
	MIDIPacket * pk = MIDIPacketListInit(pktlist);
	ByteCount nData = (ByteCount) length;
	pk->data[0] = (Byte) (hiStatus & 0xF0) | (loStatus & 0x0F);
	pk->data[1] = (Byte) aval;
	pk->data[2] = (Byte) bval;
	pk = MIDIPacketListAdd(pktlist, sizeof(struct MIDIPacketList) , pk, midiTime(late), nData, pk->data);
	/*OSStatus error =*/ MIDISend(gMIDIOutPort[port],  dest, pktlist );
}

int prSendMIDIOut(struct VMGlobals *g, int numArgsPushed);
int prSendMIDIOut(struct VMGlobals *g, int numArgsPushed)
{
		//port, uid, len, hiStatus, loStatus, a, b, latency
	//PyrSlot *m = g->sp - 8;
	PyrSlot *p = g->sp - 7;

	PyrSlot *u = g->sp - 6;
	PyrSlot *l = g->sp - 5;

	PyrSlot *his = g->sp - 4;
	PyrSlot *los = g->sp - 3;

	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *plat = g->sp;


	int err, outputIndex, uid, length, hiStatus, loStatus, aval, bval;
	float late;
	err = slotIntVal(p, &outputIndex);
	if (err) return err;
	if (outputIndex < 0 || outputIndex >= gNumMIDIInPorts) return errIndexOutOfRange;

	err = slotIntVal(u, &uid);
	if (err) return err;
		err = slotIntVal(l, &length);
	if (err) return err;
		err = slotIntVal(his, &hiStatus);
	if (err) return err;
		err = slotIntVal(los, &loStatus);
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

	sendmidi(outputIndex, dest, length, hiStatus, loStatus, aval, bval, late);
	return errNone;
}

// not needed in CoreMIDI:
int initMIDIClient()
{
	return errNone;
}
int prInitMIDIClient(struct VMGlobals *g, int numArgsPushed);
int prInitMIDIClient(struct VMGlobals *g, int numArgsPushed)
{
	return initMIDIClient();
}
//--------------

void initMIDIPrimitives()
{
	int base, index;

	base = nextPrimitiveIndex();
	index = 0;
	gSysexData.reserve(1024);

	s_midiin = getsym("MIDIIn");
	s_domidiaction = getsym("doAction");
	s_midiNoteOnAction = getsym("doNoteOnAction");
	s_midiNoteOffAction = getsym("doNoteOffAction");
	s_midiTouchAction = getsym("doTouchAction");
	s_midiControlAction = getsym("doControlAction");
	s_midiPolyTouchAction = getsym("doPolyTouchAction");
	s_midiProgramAction = getsym("doProgramAction");
	s_midiBendAction = getsym("doBendAction");
	s_midiSysexAction = getsym("doSysexAction");
	s_midiInvalidSysexAction = getsym("doInvalidSysexAction"); // client can handle incorrect case
	s_midiSysrtAction = getsym("doSysrtAction");
	s_midiSMPTEAction = getsym("doSMPTEaction");
	s_numMIDIDev = getsym("prSetNumberOfDevices");
	s_midiclient = getsym("MIDIClient");

	definePrimitive(base, index++, "_ListMIDIEndpoints", prListMIDIEndpoints, 1, 0);
	definePrimitive(base, index++, "_InitMIDI", prInitMIDI, 3, 0);
	definePrimitive(base, index++, "_InitMIDIClient", prInitMIDIClient, 1, 0);
	definePrimitive(base, index++, "_ConnectMIDIIn", prConnectMIDIIn, 3, 0);
	definePrimitive(base, index++, "_DisconnectMIDIIn", prDisconnectMIDIIn, 3, 0);
	definePrimitive(base, index++, "_DisposeMIDIClient", prDisposeMIDIClient, 1, 0);
	definePrimitive(base, index++, "_RestartMIDI", prRestartMIDI, 1, 0);
	definePrimitive(base, index++, "_SendMIDIOut", prSendMIDIOut, 9, 0);
	definePrimitive(base, index++, "_SendSysex", prSendSysex, 3, 0);
	if(gMIDIClient) midiCleanUp();
}
