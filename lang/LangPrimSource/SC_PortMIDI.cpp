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
changes by jan trutzschler v. f. 9/9/2002
the midiReadProc calls doAction in the class MIDIIn.
with the arguments: inUid, status, chan, val1, val2
added prDisposeMIDIClient
added prRestartMIDI
19/9 call different actions,disconnect midiInPort, midiout: sendmidi
04/feb/03 prListMIDIEndpoints modification by Ron Kuivila added jt.
*/

#include "PortMIDI.h"
#include "PortTime.h"

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
#include <map>

// symbols to call back into lang
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
PyrSymbol* s_midiin;
PyrSymbol* s_numMIDIDev;
PyrSymbol* s_midiclient;

const int kMaxMidiPorts = 16;
int gNumMIDIInPorts = 0, gNumMIDIOutPorts = 0;
bool gMIDIInitialized = false;

PmStream* gMIDIInStreams[kMaxMidiPorts];
PmStream* gMIDIOutStreams[kMaxMidiPorts];

std::map<int,int> gMidiInputIndexToPmDevIndex;
std::map<int,int> gMidiOutputIndexToPmDevIndex;
std::map<int,int> gMidiPmDevIndexToInputIndex;
std::map<int,int> gMidiPmDevIndexToOutputIndex;
pthread_mutex_t gPmStreamMutex;

struct ScopeMutexLock {
	pthread_mutex_t* mutex_;
	ScopeMutexLock(pthread_mutex_t* mutex) : mutex_(mutex) {
		pthread_mutex_lock(mutex_); }
	~ScopeMutexLock( ) {
		pthread_mutex_unlock(mutex_); }
};

/* if INPUT_BUFFER_SIZE is 0, PortMidi uses a default value */
#define PMSTREAM_INPUT_BUFFER_SIZE 0
#define PMSTREAM_OUTPUT_BUFFER_SIZE 100
#define PMSTREAM_DRIVER_INFO NULL
#define PMSTREAM_TIME_PROC NULL
#define PMSTREAM_TIME_INFO NULL

/* use zero latency because we want output to be immediate */
#define LATENCY 0

extern bool compiledOK;

inline void TPmErr(PmError err) {
	if( err != pmNoError)
	throw err;
}

/* timer "interrupt" for processing midi data */
static void PMProcessMidi(PtTimestamp timestamp, void *userData)
{
	ScopeMutexLock mulo(&gPmStreamMutex);
	PmError result;
	PmEvent buffer; /* just one message at a time */

	for( int i = 0 ; i < gNumMIDIInPorts; ++i )
	{
		int pmdid = gMidiInputIndexToPmDevIndex[i];
		PmStream* midi_in = gMIDIInStreams[i];
		if( midi_in )
		{
			while(result = Pm_Poll(midi_in))
			{
				long Tstatus, data1, data2;
				if (Pm_Read(midi_in, &buffer, 1) == pmBufferOverflow) 
				continue;
				// unless there was overflow, we should have a message now 

				Tstatus = Pm_MessageStatus(buffer.message);
				data1 = Pm_MessageData1(buffer.message);
				data2 = Pm_MessageData2(buffer.message);

				// +---------------------------------------------+
				// | Lock the interp. mutex and dispatch message |
				// +---------------------------------------------+
				pthread_mutex_lock (&gLangMutex); // it is needed  -jamesmcc
				if (compiledOK) 
				{
					VMGlobals *g = gMainVMGlobals;
					uint8 status = static_cast<uint8>(Tstatus & 0xF0);			
					uint8 chan = static_cast<uint8>(Tstatus & 0x0F);

					g->canCallOS = false; // cannot call the OS

					++g->sp; SetObject(g->sp, s_midiin->u.classobj); // Set the class MIDIIn
					//set arguments: 

					++g->sp; SetInt(g->sp,  pmdid); //src
					// ++g->sp;  SetInt(g->sp, status); //status
					++g->sp;  SetInt(g->sp, chan); //chan

					//if(status & 0x80) // set the running status for voice messages
						//gRunningStatus = ((status >> 4) == 0xF) ? 0 : pkt->data[i]; // keep also additional info
					switch (status) 
					{
					case 0x80 : //noteOff
						++g->sp; SetInt(g->sp, data1);
						++g->sp; SetInt(g->sp, data2);
						runInterpreter(g, s_midiNoteOffAction, 5);
						break;
					case 0x90 : //noteOn 
						++g->sp; SetInt(g->sp, data1);
						++g->sp; SetInt(g->sp, data2);
						runInterpreter(g, data2 ? s_midiNoteOnAction : s_midiNoteOffAction, 5);
						break;
					case 0xA0 : //polytouch
						++g->sp; SetInt(g->sp, data1);
						++g->sp; SetInt(g->sp, data2);
						runInterpreter(g, s_midiPolyTouchAction, 5);
						break;
					case 0xB0 : //control
						++g->sp; SetInt(g->sp, data1);
						++g->sp; SetInt(g->sp, data2);
						runInterpreter(g, s_midiControlAction, 5);
						break;
					case 0xC0 : //program
						++g->sp; SetInt(g->sp, data1);
						runInterpreter(g, s_midiProgramAction, 4);
						break;
					case 0xD0 : //touch
						++g->sp; SetInt(g->sp, data1);
						runInterpreter(g, s_midiTouchAction, 4);
						break;
					case 0xE0 : //bend	
						++g->sp; SetInt(g->sp, (data2 << 7) | data1);
						runInterpreter(g, s_midiBendAction, 4);
						break;
					/*case 0xF0 :
						// only the first Pm_Event will carry the 0xF0 byte
						// sysex message will be terminated by the EOX status byte 0xF7
						midiProcessSystemPacket(data1, data2, chan);
						break;
					default :	// data byte => continuing sysex message
						if(gRunningStatus && !gSysexFlag) { // handle running status
							status = gRunningStatus & 0xF0; // accept running status only inside a packet beginning
							chan = gRunningStatus & 0x0F;	// with a valid status byte 
							SetInt(g->sp, chan);			
							--i;
							//goto L; // parse again with running status set // mv - get next byte
						}
						chan = 0;
						i += midiProcessSystemPacket(pkt, chan); // process sysex packet
						break; */
					}
					g->canCallOS = false;
				} 
				pthread_mutex_unlock (&gLangMutex); 
			}
		} // if (midi_in)
	} // for loop until numMIDIInPorts
}

/*
-------------------------------------------------------------
*/
void midiCleanUp();
int initMIDI()
{
	try {
		midiCleanUp();

		TPmErr(Pm_Initialize());
		int nbDev = Pm_CountDevices();
		int inIndex = 0;
		int outIndex = 0;
		int pmdid;

		for( int i = 0; i < nbDev ; ++i ) {
			const PmDeviceInfo* devInfo = Pm_GetDeviceInfo(i);
			if( devInfo->input )
			{
				gNumMIDIInPorts++;
				gMidiInputIndexToPmDevIndex[inIndex++] = i;
				gMidiPmDevIndexToInputIndex[i] = inIndex;
			}
			if( devInfo->output )
			{
				gNumMIDIOutPorts++;
				gMidiOutputIndexToPmDevIndex[outIndex++] = i;
				gMidiPmDevIndexToOutputIndex[i] = outIndex;
			}
		}

		for( int i = 0; i < gNumMIDIOutPorts; i++) {
			pmdid = gMidiOutputIndexToPmDevIndex[i];
			Pm_OpenOutput(&gMIDIOutStreams[i], pmdid, NULL, 512, NULL, NULL, 0);
		}

		/* will call our function, PMProcessMidi() every millisecond */
		Pt_Start(1, &PMProcessMidi, 0); /* start a timer with millisecond accuracy */
	}
	catch(PmError) {
		return errFailed;
	}

	gMIDIInitialized = true;
	return errNone;
}
/*
-------------------------------------------------------------
*/
void midiCleanUp()
{
	ScopeMutexLock mulo(&gPmStreamMutex);

	if(gMIDIInitialized) {
		for (int i=0; i<gNumMIDIOutPorts; ++i) {
			Pm_Abort(gMIDIOutStreams[i]);
			Pm_Close(gMIDIOutStreams[i]);
		}
		for (int i=0; i<gNumMIDIInPorts; ++i) {
			Pm_Abort(gMIDIInStreams[i]);
			Pm_Close(gMIDIInStreams[i]);
		}

		gNumMIDIOutPorts = 0;
		gNumMIDIInPorts = 0;
	}

	// set the stream pointers to NULL
	memset(gMIDIInStreams,0,kMaxMidiPorts*sizeof(PmStream*));
	memset(gMIDIOutStreams,0,kMaxMidiPorts*sizeof(PmStream*));

	// delete the objects that map in/out indices to Pm dev indices
	gMidiInputIndexToPmDevIndex.clear();
	gMidiOutputIndexToPmDevIndex.clear();
	gMidiPmDevIndexToInputIndex.clear();
	gMidiPmDevIndexToOutputIndex.clear();

	gMIDIInitialized = false;
}
/*
-------------------------------------------------------------
*/
void midiListEndpoints()
{
}
/*
-------------------------------------------------------------
*/
int prListMIDIEndpoints(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	int numSrc = gNumMIDIInPorts;
	int numDst = gNumMIDIOutPorts;

	PyrObject* idarray = newPyrArray(g->gc, 6 * sizeof(PyrObject), 0 , true);
	SetObject(a, idarray);

	// 0
	PyrObject* idarraySo = newPyrArray(g->gc, numSrc * sizeof(__int32), 0 , true);
	SetObject(idarray->slots+idarray->size++, idarraySo);
	g->gc->GCWrite(idarray, idarraySo);

	// 1
	PyrObject* devarraySo = newPyrArray(g->gc, numSrc * sizeof(PyrObject), 0 , true);
	SetObject(idarray->slots+idarray->size++, devarraySo);
	g->gc->GCWrite(idarray, devarraySo);

	// 2
	PyrObject* namearraySo = newPyrArray(g->gc, numSrc * sizeof(PyrObject), 0 , true);
	SetObject(idarray->slots+idarray->size++, namearraySo);
	g->gc->GCWrite(idarray, namearraySo);

	// 3
	PyrObject* idarrayDe = newPyrArray(g->gc, numDst * sizeof(__int32), 0 , true);
	SetObject(idarray->slots+idarray->size++, idarrayDe);
	g->gc->GCWrite(idarray, idarrayDe);

	// 4
	PyrObject* namearrayDe = newPyrArray(g->gc, numDst * sizeof(PyrObject), 0 , true);
	SetObject(idarray->slots+idarray->size++, namearrayDe);
	g->gc->GCWrite(idarray, namearrayDe);

	// 5
	PyrObject* devarrayDe = newPyrArray(g->gc, numDst * sizeof(PyrObject), 0 , true);
	SetObject(idarray->slots+idarray->size++, devarrayDe);       
	g->gc->GCWrite(idarray, devarrayDe);

	for (int i=0; i<numSrc; ++i) {
		const PmDeviceInfo* devInfo = Pm_GetDeviceInfo(gMidiInputIndexToPmDevIndex[i]);
		
		char cendname[1024], cdevname[1024];
		
		// currently, copy both name strings in endpoint name and dev name
		strncpy(cendname,devInfo->name,1023);
		cendname[1023] = 0;
		strncpy(cdevname,devInfo->name,1023);
		cdevname[1023] = 0;

		PyrString *string = newPyrString(g->gc, cendname, 0, true);
		SetObject(namearraySo->slots+i, string);
		namearraySo->size++;
		g->gc->GCWrite(namearraySo, (PyrObject*)string);

		PyrString *devstring = newPyrString(g->gc, cdevname, 0, true);
		SetObject(devarraySo->slots+i, devstring);
		devarraySo->size++;
		g->gc->GCWrite(devarraySo, (PyrObject*)devstring);

		SetInt(idarraySo->slots+i, i);
		idarraySo->size++;
	}

	//      post("numDst %d\n",  numDst);
	for (int i=0; i<numDst; ++i) {
		const PmDeviceInfo* devInfo = Pm_GetDeviceInfo(gMidiOutputIndexToPmDevIndex[i]);
		char cendname[1024], cdevname[1024];

		// currently, copy both name strings in endpoint name and dev name
		strncpy(cendname,devInfo->name,1023);
		cendname[1023] = 0;
		strncpy(cdevname,devInfo->name,1023);
		cdevname[1023] = 0;

		PyrString *string = newPyrString(g->gc, cendname, 0, true);
		SetObject(namearrayDe->slots+namearrayDe->size++, string);
		g->gc->GCWrite(namearrayDe, (PyrObject*)string);

		PyrString *devstring = newPyrString(g->gc, cdevname, 0, true);

		SetObject(devarrayDe->slots+devarrayDe->size++, devstring);
		g->gc->GCWrite(devarrayDe, (PyrObject*)devstring);

		SetInt(idarrayDe->slots+idarrayDe->size++, i);

	}
	return errNone;
}

/*
-------------------------------------------------------------
*/

int prConnectMIDIIn(struct VMGlobals *g, int numArgsPushed)
{
	ScopeMutexLock mulo(&gPmStreamMutex);

	//PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	int err, inputIndex, uid;
	err = slotIntVal(b, &inputIndex);
	if (err) return errWrongType;
	if (inputIndex < 0 || inputIndex >= gNumMIDIInPorts) 
	return errIndexOutOfRange;

	err = slotIntVal(c, &uid);
	if (err) 
	return errWrongType;

	PmStream* inStream = NULL;
	int pmdid = gMidiInputIndexToPmDevIndex[uid];

	PmError pmerr = Pm_OpenInput( &inStream, pmdid, 
	PMSTREAM_DRIVER_INFO,
	PMSTREAM_INPUT_BUFFER_SIZE,
	PMSTREAM_TIME_PROC,
	PMSTREAM_TIME_INFO );

	if(pmerr != pmNoError)
	return errFailed;

	gMIDIInStreams[uid] = inStream;
	return errNone;
}
/*
-------------------------------------------------------------
*/
int prDisconnectMIDIIn(struct VMGlobals *g, int numArgsPushed)
{
	ScopeMutexLock mulo(&gPmStreamMutex); 

	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	int err, inputIndex, uid;
	err = slotIntVal(b, &inputIndex);
	if (err) return err;
	if (inputIndex < 0 || inputIndex >= gNumMIDIInPorts) return errIndexOutOfRange;
	err = slotIntVal(c, &uid);
	if (err) 
	return err;

	PmError pmerr = Pm_Close(gMIDIInStreams[uid]);

	if(pmerr != pmNoError)
	return errFailed;

	gMIDIInStreams[uid] = NULL;
	return errNone;

}
/*
-------------------------------------------------------------
*/
int prInitMIDI(struct VMGlobals *g, int numArgsPushed)
{
	//PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	int err, numIn, numOut;
	err = slotIntVal(b, &numIn);
	if (err) return errWrongType;

	err = slotIntVal(c, &numOut);
	if (err) 
	return errWrongType;

	return initMIDI();
}

int prDisposeMIDIClient(VMGlobals *g, int numArgsPushed)
{
	midiCleanUp();
	return errNone;
}

int prRestartMIDI(VMGlobals *g, int numArgsPushed)
{
	initMIDI();
	return errNone;
}

/*
void freeSysex(MIDISysexSendRequest* pk)
{
free(pk->data);
free(pk);
}
*/

int prSendSysex(VMGlobals *g, int numArgsPushed)
{	
	/*
int err, uid, size;
PyrInt8Array* packet = g->sp->uob;				
size = packet->size;
Byte *data = (Byte *)malloc(size);

memcpy(data,packet->b, size);

PyrSlot *u = g->sp - 1;
err = slotIntVal(u, &uid);
if (err) return err;

MIDIEndpointRef dest;
MIDIObjectType mtype;
MIDIObjectFindByUniqueID(uid, (MIDIObjectRef*)&dest, &mtype);
if (mtype != kMIDIObjectType_Destination) return errFailed;            
if (!dest) return errFailed;

sendsysex(dest, size, data);
return errNone;
*/
	return errFailed;
}
/*
-------------------------------------------------------------
*/
int prSendMIDIOut(struct VMGlobals *g, int numArgsPushed)
{
	ScopeMutexLock mulo(&gPmStreamMutex);
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
	if (outputIndex < 0 || outputIndex >= gNumMIDIOutPorts) return errIndexOutOfRange;

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

	Pm_WriteShort(gMIDIOutStreams[uid], 0, 
	Pm_Message((hiStatus & 0xF0) | (loStatus & 0x0F) , aval, bval));

	return errNone;
}

// not needed in PortMIDI:
int initMIDIClient()
{
	return errNone;
}

int prInitMIDIClient(struct VMGlobals *g, int numArgsPushed)
{
	return initMIDIClient();
}

void initMIDIPrimitives()
{
	int base, index;

	base = nextPrimitiveIndex();
	index = 0;

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
	pthread_mutex_init (&gPmStreamMutex, NULL);
	midiCleanUp();
}

