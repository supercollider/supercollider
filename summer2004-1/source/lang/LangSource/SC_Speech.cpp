/*
 *  SC_Speech.h
 *  SC3lang
 *
 *  Created by jan truetzschler v. falkenstein on Wed Apr 16 2003.
 *  Copyright (c) 2003 sampleAndHold.org. All rights reserved.
    
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

 *
 */

#include <Carbon/Carbon.h>
#include "InitAlloc.h"

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



/////////////////////
const int kMaxSpeechChannels = 128;
PyrSymbol * s_speech;
PyrSymbol * s_speechwordAction;
PyrSymbol * s_speechdoneAction;
SpeechChannel fCurSpeechChannel[kMaxSpeechChannels];

pascal void OurSpeechDoneCallBackProc ( SpeechChannel inSpeechChannel, long inRefCon );
pascal void OurSpeechDoneCallBackProc ( SpeechChannel inSpeechChannel, long inRefCon )
{
    //call action here;
    // post("text done");
    pthread_mutex_lock (&gLangMutex); 
	VMGlobals *g = gMainVMGlobals;
	g->canCallOS = true;
	++g->sp; SetObject(g->sp, s_speech->u.classobj); // Set the class MIDIIn
        //set arguments: 
	++g->sp;SetInt(g->sp, (int) inRefCon); //src
	runInterpreter(g, s_speechdoneAction, 2);

    g->canCallOS = false;
	pthread_mutex_unlock (&gLangMutex); 
}

pascal void OurWordCallBackProc ( SpeechChannel inSpeechChannel, long inRefCon, long inWordPos, short inWordLen);
pascal void OurWordCallBackProc ( SpeechChannel inSpeechChannel, long inRefCon, long inWordPos, short inWordLen) {
    //post("word done");
	pthread_mutex_lock (&gLangMutex); 
	VMGlobals *g = gMainVMGlobals;
	g->canCallOS = true;
	++g->sp; SetObject(g->sp, s_speech->u.classobj); 
    //set arguments: 
	++g->sp; SetInt(g->sp, (int) inRefCon); //src
	runInterpreter(g, s_speechwordAction, 2);

    g->canCallOS = false;
	pthread_mutex_unlock (&gLangMutex); 
}

int prInitSpeech(struct VMGlobals *g, int numArgsPushed);
int prInitSpeech(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	//PyrSlot *a = g->sp-1;
	PyrSlot *b = g->sp;
    int chan;
    slotIntVal(b, &chan);
	if (chan < 0 || chan >= kMaxSpeechChannels) return errIndexOutOfRange;
	
	for (int i=0; i<chan; ++i) {
        NewSpeechChannel( NULL, fCurSpeechChannel+i );
        theErr = SetSpeechInfo (fCurSpeechChannel[i], soSpeechDoneCallBack, OurSpeechDoneCallBackProc);
        theErr = SetSpeechInfo (fCurSpeechChannel[i], soWordCallBack, OurWordCallBackProc);
        theErr = SetSpeechInfo (fCurSpeechChannel[i], soRefCon, (void*) i);
	}
    return errNone;
}

//NewSpeechDoneUPP(SpeechDoneProcPtr userRoutine);
//theErr = SetSpeechInfo (fCurSpeechChannel, soSpeechDoneCallBack, OurSpeechDoneCallBackProc);

int prSpeakText(struct VMGlobals *g, int numArgsPushed);
int prSpeakText(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	PyrSlot *a = g->sp-1;
	PyrSlot *b = g->sp;
	int chan;
	char *theTextToSpeak = (char*)pyr_pool_compile->Alloc((a->uo->size + 1)* sizeof(char));
	MEMFAIL(theTextToSpeak);

	slotStrVal(a, theTextToSpeak, a->uo->size+1);
	slotIntVal(b, &chan);

	//if(!fCurSpeechChannel) theErr = NewSpeechChannel( NULL, &fCurSpeechChannel );
	theErr = SpeakText( fCurSpeechChannel[chan], theTextToSpeak, strlen(theTextToSpeak));
	pyr_pool_compile->Free(theTextToSpeak);
	return errNone;
}

int prSetSpeechRate(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechRate(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	double val;
	int chan;
	slotIntVal(b, &chan);
	slotDoubleVal(c, &val);
	Fixed newRate = val * 65536.0;
//	if(!fCurSpeechChannel) theErr = NewSpeechChannel( NULL, &fCurSpeechChannel );
    theErr = SetSpeechInfo (fCurSpeechChannel[chan], soRate, &newRate);
	return errNone;
}

int prSetSpeechPitch(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechPitch(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	double val;
	int chan;
	slotIntVal(b, &chan);
	slotDoubleVal(c, &val);
	Fixed newVal = val * 65536.0;
	//if(!fCurSpeechChannel) theErr = NewSpeechChannel( NULL, &fCurSpeechChannel );
    theErr = SetSpeechPitch (fCurSpeechChannel[chan], newVal);
	return errNone;
}

int prSetSpeechPitchMod(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechPitchMod(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	double val;
	int chan;
	slotIntVal(b, &chan);
	slotDoubleVal(c, &val);
	Fixed newVal = val * 65536.0;
//	if(!fCurSpeechChannel) theErr = NewSpeechChannel( NULL, &fCurSpeechChannel );
    theErr = SetSpeechInfo (fCurSpeechChannel[chan], soPitchMod, &newVal);
	return errNone;
}

int prSetSpeechVolume(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechVolume(struct VMGlobals *g, int numArgsPushed) {

	OSErr theErr = noErr;
	PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	double val;
	int chan;
	slotIntVal(b, &chan);
	slotDoubleVal(c, &val);
	Fixed newVal = val * 65536.0;
//	if(!fCurSpeechChannel) theErr = NewSpeechChannel( NULL, &fCurSpeechChannel );
    theErr = SetSpeechInfo (fCurSpeechChannel[chan], soVolume, &newVal);
	return errNone;
}

//		theErr = PauseSpeechAt (fCurSpeechChannel, kImmediate);
//		theErr = ContinueSpeech (fCurSpeechChannel);
int prSetSpeechPause(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechPause(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	int val;
	int chan;
	slotIntVal(b, &chan);
	slotIntVal(c, &val);
	//Fixed newVal = val * 65536.0;
	if(val) {
        theErr = ContinueSpeech(fCurSpeechChannel[chan] );
	} else {
        theErr = PauseSpeechAt(fCurSpeechChannel[chan], kImmediate);
	}
	return errNone;
}

int prSetSpeechVoice(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechVoice(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	int val;
	int chan;

	VoiceSpec theVoiceSpec;
	slotIntVal(b, &chan);
	slotIntVal(c, &val);
    theErr = GetIndVoice (val, &theVoiceSpec);
	if (SetSpeechInfo (fCurSpeechChannel[chan], soCurrentVoice, &theVoiceSpec) == incompatibleVoice) return (!errNone);

	return errNone;
}

void initSpeechPrimitives ()
{
	int base, index;
        
	base = nextPrimitiveIndex();
	index = 0;
	
	s_speechwordAction = getsym("doWordAction");
	s_speechdoneAction = getsym("doSpeechDoneAction");
	s_speech = getsym("Speech");
	
	definePrimitive(base, index++, "_SpeakText", prSpeakText, 2, 0);
	definePrimitive(base, index++, "_InitSpeech", prInitSpeech, 2, 0);
	definePrimitive(base, index++, "_SetSpeechRate", prSetSpeechRate, 3, 0);
	definePrimitive(base, index++, "_SetSpeechPitch", prSetSpeechPitch, 3, 0);
	definePrimitive(base, index++, "_SetSpeechPitchMod", prSetSpeechPitchMod, 3, 0);
	definePrimitive(base, index++, "_SetSpeechVoice", prSetSpeechVoice, 3, 0);
	definePrimitive(base, index++, "_SetSpeechVolume", prSetSpeechVolume, 3, 0);
	definePrimitive(base, index++, "_SetSpeechPause", prSetSpeechPause, 3, 0); //0 pause, 1 continue
}


