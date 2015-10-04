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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

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

#import <AppKit/NSSpeechSynthesizer.h>
#import <Foundation/NSString.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSArray.h>
#import <Foundation/NSEnumerator.h>
#import <Foundation/NSDictionary.h>

//comment the following line to use cocoa speech
#define useCarbonSpeech

/////////////////////
const int kMaxSpeechChannels = 32;
PyrSymbol * s_speech;
PyrSymbol * s_speechwordAction;
PyrSymbol * s_speechdoneAction;
#ifdef useCarbonSpeech
SpeechChannel fCurSpeechChannel[kMaxSpeechChannels];
char *speechStrings[kMaxSpeechChannels];
#else
//NSSpeechSynthesizer* speechSynths[kMaxSpeechChannels];
NSArray * speechSynthsArray;
#endif

pascal void OurSpeechDoneCallBackProc ( SpeechChannel inSpeechChannel, long inRefCon );
pascal void OurSpeechDoneCallBackProc ( SpeechChannel inSpeechChannel, long inRefCon )
{
	//call action here;
	// post("text done");
#ifdef useCarbonSpeech
	gLangMutex.lock();
	VMGlobals *g = gMainVMGlobals;
	g->canCallOS = true;
	++g->sp; SetObject(g->sp, s_speech->u.classobj); // Set the class
	//set arguments:
	++g->sp;SetInt(g->sp, (int) inRefCon); //src
	runInterpreter(g, s_speechdoneAction, 2);
	if(speechStrings[(int) inRefCon] != NULL){
		free(speechStrings[(int) inRefCon]);
		speechStrings[(int) inRefCon] = NULL;
	}
	g->canCallOS = false;
	gLangMutex.unlock();
#endif
}

pascal void OurWordCallBackProc ( SpeechChannel inSpeechChannel, long inRefCon, long inWordPos, short inWordLen);
pascal void OurWordCallBackProc ( SpeechChannel inSpeechChannel, long inRefCon, long inWordPos, short inWordLen) {
    //post("word done");
#ifdef useCarbonSpeech
	gLangMutex.lock();
	VMGlobals *g = gMainVMGlobals;
	g->canCallOS = true;
	++g->sp; SetObject(g->sp, s_speech->u.classobj);
    //set arguments:
	++g->sp; SetInt(g->sp, (int) inRefCon); //src
	runInterpreter(g, s_speechwordAction, 2);

    g->canCallOS = false;
	gLangMutex.unlock();
#endif
}

int prInitSpeech(struct VMGlobals *g, int numArgsPushed);
int prInitSpeech(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	//PyrSlot *a = g->sp-1;
	PyrSlot *b = g->sp;
    int chan, err;
    err = slotIntVal(b, &chan);
	if (err) return err;
	if (chan < 0 || chan >= kMaxSpeechChannels) return errIndexOutOfRange;
#ifdef useCarbonSpeech
	for (int i=0; i<chan; ++i) {
		if(fCurSpeechChannel[i]) DisposeSpeechChannel(fCurSpeechChannel[i]);
        NewSpeechChannel( NULL, fCurSpeechChannel+i );
        theErr = SetSpeechInfo (fCurSpeechChannel[i], soSpeechDoneCallBack, (const void*)OurSpeechDoneCallBackProc);
        theErr = SetSpeechInfo (fCurSpeechChannel[i], soWordCallBack, (const void*)OurWordCallBackProc);
        theErr = SetSpeechInfo (fCurSpeechChannel[i], soRefCon, (void*) i);
	}
#else
	if(chan <= [speechSynthsArray count]) return errNone;
	if(speechSynthsArray){
//		NSEnumerator * voiceEnumerator = [speechSynthsArray objectEnumerator];
//		NSSpeechSynthesizer * aVoice;
//		while(aVoice = [voiceEnumerator nextObject]) {
//			[voiceEnumerator release];
//		}

		[speechSynthsArray release];
		speechSynthsArray = NULL;
	}

	NSSpeechSynthesizer* speechSynths[chan];
	for (int i=0; i<chan; ++i) speechSynths[i] = 	[[NSSpeechSynthesizer alloc] init];
	speechSynthsArray = [[NSArray arrayWithObjects: speechSynths count: chan] retain];

#endif
    return errNone;
}

//NewSpeechDoneUPP(SpeechDoneProcPtr userRoutine);
//theErr = SetSpeechInfo (fCurSpeechChannel, soSpeechDoneCallBack, OurSpeechDoneCallBackProc);

int prSpeakText(struct VMGlobals *g, int numArgsPushed);
int prSpeakText(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	PyrSlot *a = g->sp-1;
	PyrSlot *str = g->sp;

	int chan, err;

	err = slotIntVal(a, &chan);
	if (err) return err;
	chan = sc_clip(chan, 0, kMaxSpeechChannels);
#ifdef useCarbonSpeech
	chan = sc_clip(chan, 0, kMaxSpeechChannels);
	if(speechStrings[chan] != NULL) {
		post("voice %i already speaking\n", chan);
		return errNone;
	} else {
	//	speechStrings[chan] = (char*)pyr_pool_compile->Alloc((a->uo->size + 1)* sizeof(char));
		speechStrings[chan] = (char*) malloc((slotRawObject(str)->size + 1)* sizeof(char));

		MEMFAIL(speechStrings[chan]);
		slotStrVal(str, speechStrings[chan], slotRawObject(str)->size+1);

		//if(!fCurSpeechChannel) theErr = NewSpeechChannel( NULL, &fCurSpeechChannel );
		theErr = SpeakText( fCurSpeechChannel[chan], speechStrings[chan], strlen(speechStrings[chan]));
	}
#else
//    gLangMutex.lock();

	NSAutoreleasePool *autoreleasepool= [[NSAutoreleasePool alloc] init];
	NSSpeechSynthesizer * spsynth = [speechSynthsArray objectAtIndex: chan];
	char cstr [str->uo->size+1];
	slotStrVal(str, cstr, str->uo->size+1);
//	if([spsynth isSpeaking]) [spsynth stopSpeaking];
//	NSString * nsstring = [NSString stringWithCString: cstr encoding: NSASCIIStringEncoding];
	[spsynth startSpeakingString: [NSString stringWithCString: cstr encoding: NSASCIIStringEncoding]];
//	[nsstring release];
//	[nsstring release];
	[autoreleasepool release];
//    gLangMutex.unlock();

#endif
	return errNone;
}

int prSetSpeechRate(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechRate(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	//PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	double val;
	int chan, err;
	err = slotIntVal(b, &chan);
	if (err) return err;
	err = slotDoubleVal(c, &val);
	if (err) return err;
#ifdef useCarbonSpeech
	Fixed newRate = (Fixed)(val * 65536.0);
    theErr = SetSpeechInfo (fCurSpeechChannel[chan], soRate, &newRate);
#else
//	NSSpeechSynthesizer * spsynth = speechSynths[chan];
//	if(!spsynth) return errNone;
//	[spsynth setRate: val];
#endif
	return errNone;
}

int prSetSpeechPitch(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechPitch(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	//PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	double val;
	int chan, err;
	err = slotIntVal(b, &chan);
	if (err) return err;
	err = slotDoubleVal(c, &val);
	if (err) return err;
#ifdef useCarbonSpeech
	Fixed newVal = (Fixed)(val * 65536.0);
	//if(!fCurSpeechChannel) theErr = NewSpeechChannel( NULL, &fCurSpeechChannel );
    theErr = SetSpeechPitch (fCurSpeechChannel[chan], newVal);
#else
	NSSpeechSynthesizer * spsynth = [speechSynthsArray objectAtIndex: chan];
	if(!spsynth) return errNone;
// 10.5 only ... ;-(
//	[spsynth setObject: val forProperty: NSSpeechPitchBaseProperty error: &err];
#endif
	return errNone;
}

int prSetSpeechPitchMod(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechPitchMod(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	//PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	double val;
	int chan, err;
	err = slotIntVal(b, &chan);
	if (err) return err;
	err = slotDoubleVal(c, &val);
	if (err) return err;
#ifdef useCarbonSpeech
	Fixed newVal = (Fixed)(val * 65536.0);
//	if(!fCurSpeechChannel) theErr = NewSpeechChannel( NULL, &fCurSpeechChannel );
    theErr = SetSpeechInfo (fCurSpeechChannel[chan], soPitchMod, &newVal);
#endif
	return errNone;
}

int prSetSpeechVolume(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechVolume(struct VMGlobals *g, int numArgsPushed) {

	OSErr theErr = noErr;
	//PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	double val;
	int chan, err;
	err = slotIntVal(b, &chan);
	if (err) return err;
	err = slotDoubleVal(c, &val);
	if (err) return err;
#ifdef useCarbonSpeech

	Fixed newVal = (Fixed)(val * 65536.0);
//	if(!fCurSpeechChannel) theErr = NewSpeechChannel( NULL, &fCurSpeechChannel );
    theErr = SetSpeechInfo (fCurSpeechChannel[chan], soVolume, &newVal);
#else
// 10.5 :-(
//	NSSpeechSynthesizer * spsynth = speechSynths[chan];
//	if(!spsynth) return errNone;
//	[spsynth setVolume: val];
#endif
	return errNone;
}

//		theErr = PauseSpeechAt (fCurSpeechChannel, kImmediate);
//		theErr = ContinueSpeech (fCurSpeechChannel);
int prSetSpeechPause(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechPause(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	//PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	int val;
	int chan, err;
	err = slotIntVal(b, &chan);
	if (err) return err;
	err = slotIntVal(c, &val);
	if (err) return err;
#ifdef useCarbonSpeech
	if(val) {
        theErr = ContinueSpeech(fCurSpeechChannel[chan] );
	} else {
        theErr = PauseSpeechAt(fCurSpeechChannel[chan], kImmediate);
	}
#else
//	NSSpeechSynthesizer * spsynth = speechSynths[chan];
//	if(!spsynth) return errNone;
//	[spsynth setRate: val];
#endif
	return errNone;
}

int prSetSpeechStop(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechStop(struct VMGlobals *g, int numArgsPushed){

	//PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	int  selector [3] = {kImmediate, kEndOfWord, kEndOfWord};
	int val;
	int chan, err;
	err = slotIntVal(b, &chan);
	if (err) return err;
	err = slotIntVal(c, &val);
	if (err) return err;
#ifdef useCarbonSpeech
	StopSpeechAt(fCurSpeechChannel[chan], selector[val]);
	if(speechStrings[chan] != NULL) {
		free(speechStrings[chan]);
		speechStrings[chan] = NULL;
	}
#else
	NSSpeechSynthesizer * spsynth =  [speechSynthsArray objectAtIndex: chan];
	if(!spsynth) return errNone;
	[spsynth stopSpeaking];
#endif

	return errNone;
}

int prSetSpeechVoice(struct VMGlobals *g, int numArgsPushed);
int prSetSpeechVoice(struct VMGlobals *g, int numArgsPushed){

	OSErr theErr = noErr;
	//PyrSlot *a = g->sp-2;
	PyrSlot *b = g->sp-1;
	PyrSlot *c = g->sp;
	int val;
	int chan, err;

	err = slotIntVal(b, &chan);
	if (err) return err;
	err = slotIntVal(c, &val);
	if (err) return err;
#ifdef useCarbonSpeech
	VoiceSpec theVoiceSpec;

	theErr = GetIndVoice (val, &theVoiceSpec);
	if (SetSpeechInfo (fCurSpeechChannel[chan], soCurrentVoice, &theVoiceSpec) == incompatibleVoice) {
		post("ERROR: Incompatible Voice\n");
		return errFailed;
	}
#else
	NSSpeechSynthesizer * spsynth =  [speechSynthsArray objectAtIndex: chan];
	if(!spsynth) return errNone;

	[spsynth setVoice: [[NSSpeechSynthesizer availableVoices] objectAtIndex: val]];

#endif
	return errNone;
}

#ifndef useCarbonSpeech
int prGetSpeechVoiceNames(struct VMGlobals *g, int numArgsPushed);
int prGetSpeechVoiceNames(struct VMGlobals *g, int numArgsPushed){

	PyrSlot *a = g->sp-1;
	PyrSlot *b = g->sp;

	NSAutoreleasePool *autoreleasepool= [[NSAutoreleasePool alloc] init];

	NSString * aVoice = NULL;
	NSEnumerator * voiceEnumerator = [[NSSpeechSynthesizer availableVoices] objectEnumerator];
	PyrObject* allVoices = newPyrArray(g->gc, (int) [[NSSpeechSynthesizer availableVoices] count]  * sizeof(PyrObject), 0 , true);
	SetObject(a, allVoices);

	while(aVoice = [voiceEnumerator nextObject]) {
		NSDictionary * dictionaryOfVoiceAttributes = [NSSpeechSynthesizer attributesForVoice:aVoice];
		NSString *	voiceDisplayName = [dictionaryOfVoiceAttributes objectForKey:NSVoiceName];

		PyrString *namestring = newPyrString(g->gc, [voiceDisplayName cStringUsingEncoding:[NSString defaultCStringEncoding]], 0, true);
		SetObject(allVoices->slots+allVoices->size++, namestring);
		g->gc->GCWriteNew(allVoices, (PyrObject*) namestring); // we know namestring is white so we can use GCWriteNew

	}
	[autoreleasepool release];

	return errNone;

}
#endif

int prSpeechVoiceIsSpeaking(struct VMGlobals *g, int numArgsPushed);
int prSpeechVoiceIsSpeaking(struct VMGlobals *g, int numArgsPushed){
	PyrSlot *out = g->sp-1;
	PyrSlot *b = g->sp;
    int chan, err;
    err = slotIntVal(b, &chan);
	if (err) return err;
#ifdef useCarbonSpeech

	if(speechStrings[chan] != NULL) SetTrue(out);
	else SetFalse(out);
#else
	NSSpeechSynthesizer * spsynth =  [speechSynthsArray objectAtIndex: chan];
	if(!spsynth) return errNone;
	if([spsynth isSpeaking])  SetTrue(out);
	else SetFalse(out);
#endif
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

	definePrimitive(base, index++, "_SpeakText", prSpeakText, 3, 0);
	definePrimitive(base, index++, "_InitSpeech", prInitSpeech, 2, 0);
	definePrimitive(base, index++, "_SetSpeechRate", prSetSpeechRate, 3, 0);
	definePrimitive(base, index++, "_SetSpeechPitch", prSetSpeechPitch, 3, 0);
	definePrimitive(base, index++, "_SetSpeechPitchMod", prSetSpeechPitchMod, 3, 0);
	definePrimitive(base, index++, "_SetSpeechVoice", prSetSpeechVoice, 3, 0);
	definePrimitive(base, index++, "_SetSpeechVolume", prSetSpeechVolume, 3, 0);
	definePrimitive(base, index++, "_SetSpeechPause", prSetSpeechPause, 3, 0); //0 pause, 1 continue
	definePrimitive(base, index++, "_SetSpeechStopAt", prSetSpeechStop, 3, 0); //0 kImmediate, 1 kEndOfWord, 2 kEndOfSentence
	definePrimitive(base, index++, "_SpeechVoiceIsSpeaking", prSpeechVoiceIsSpeaking, 2, 0);
#ifndef useCarbonSpeech
	definePrimitive(base, index++, "_GetSpeechVoiceNames", prGetSpeechVoiceNames, 2, 0);
#endif

#ifdef useCarbonSpeech
	for(int i=0; i<kMaxSpeechChannels; ++i){
		speechStrings[i] = NULL;
		if(fCurSpeechChannel[i]) DisposeSpeechChannel(fCurSpeechChannel[i]);
		fCurSpeechChannel[i] = NULL;
	}
#else
	#if 0
	if(speechSynthsArray){
//		[speechSynthsArray enumerator]
		[speechSynthsArray release];
		speechSynthsArray = NULL;
	}
	#endif
#endif
}


