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

This file contains the definitions of the core objects that implement the class system.

*/

#ifndef _PYRKERNEL_H_
#define _PYRKERNEL_H_

#include "PyrObject.h"
#include "VMGlobals.h"

#define classClassNumInstVars 16

enum { classIsIntrinsic = 1, classHasIndexableInstances = 2, classCompileUGen = 4 };

struct PyrClass : public PyrObjectHdr 
{
	PyrSlot name;
	PyrSlot nextclass;
	PyrSlot superclass;
	PyrSlot subclasses;
	PyrSlot methods;
	
	PyrSlot instVarNames;
	PyrSlot classVarNames;
	PyrSlot iprototype;		// instance prototype
	PyrSlot cprototype;		// class var prototype
	
	PyrSlot instanceFormat;
	PyrSlot instanceFlags;
	PyrSlot classIndex;		
	PyrSlot classFlags;
	PyrSlot maxSubclassIndex; // used by isKindOf
	PyrSlot filenameSym;
	PyrSlot charPos;
};



inline bool isKindOf(PyrObjectHdr *obj, struct PyrClass *testclass)
{
	int objClassIndex = obj->classptr->classIndex.ui;	
	return objClassIndex >= testclass->classIndex.ui && objClassIndex <= testclass->maxSubclassIndex.ui;
}

inline bool isKindOfSlot(PyrSlot *slot, struct PyrClass *testclass)
{
	return IsObj(slot) && isKindOf(slot->uo, testclass);
}

/* 
	operations on class:
	numInstVars()
	numClassVars()
	
*/

struct PyrFrame {	
	PyrSlot vars[1];
	PyrSlot myself;
	PyrSlot method;
	PyrSlot caller;
	PyrSlot context;
	PyrSlot homeContext;
	PyrSlot ip;
};

#define FRAMESIZE 6
#define USESTACKFRAMES 1

struct PyrProcess : public PyrObjectHdr 
{
		
	PyrSlot classVars;
	PyrSlot interpreter;
	PyrSlot curThread, mainThread;
	PyrSlot processID;
	PyrSlot sysSchedulerQueue;
};


enum { tInit, tStart, tReady, tRunning, tSleeping, tBlocked, tYieldToChild, tYieldToParent, tDone };

struct PyrThread : public PyrObjectHdr 
{
		
	PyrSlot state, func, stack, stackSize, method, block, frame, ip, sp;
	PyrSlot top, numpop, returnLevels, receiver, numArgsPushed;
	PyrSlot parent, terminalValue;
	PyrSlot primitiveError;
	PyrSlot primitiveIndex;
	PyrSlot randData;
	PyrSlot time, waitTime;
};

#define EVALSTACKDEPTH 2048
#define SCHEDQLEN 512
#define THREADSLEVELS 32



struct PyrMethodRaw {

	unsigned short unused1;
	unsigned short specialIndex;
	unsigned short methType;
	unsigned short frameSize;
	
	unsigned char unused2;
	unsigned char numargs;
	unsigned char varargs;
	unsigned char numvars;
	unsigned char numtemps;
	unsigned char needsHeapContext;
	unsigned char popSize;
	unsigned char posargs;
	
};


#define METHRAW(obj) ((PyrMethodRaw*)&(((PyrBlock*)obj)->rawData1))

struct PyrBlock : public PyrObjectHdr 
{
		
	PyrSlot rawData1;			
	PyrSlot rawData2;			
	PyrSlot code;				// byte codes, nil if inlined
	PyrSlot literals;			// literal table
	PyrSlot prototypeFrame;		// prototype of an activation frame
	PyrSlot context;			// ***defining block context
	PyrSlot argNames;			// ***arguments to block
	PyrSlot varNames;			// ***variables in block
};

struct PyrMethod : public PyrBlock 
{
	PyrSlot ownerclass;
	PyrSlot name;
	PyrSlot primitiveName;
	PyrSlot filenameSym;
	PyrSlot charPos;
	//PyrSlot byteMeter;
	//PyrSlot callMeter;
};

enum {	
	methNormal = 0,
	methReturnSelf,
	methReturnLiteral,
	methReturnArg,
	methReturnInstVar,
	methAssignInstVar,
	methReturnClassVar,
	methAssignClassVar,
	methRedirect,
	methForward,
	methPrimitive,
	methBlock
};

struct PyrClosure : public PyrObjectHdr 
{
	
	PyrSlot block;
	PyrSlot context;
};

struct PyrInterpreter : public PyrObjectHdr 
{
		
	PyrSlot cmdLine, context;
	PyrSlot a, b, c, d, e, f, g, h, i, j; 
	PyrSlot k, l, m, n, o, p, q, r, s, t;
	PyrSlot u, v, w, x, y, z;
};

struct PyrDict : public PyrObjectHdr 
{
	
	PyrSlot numelems;
	PyrSlot elems;
};

/* special values */
enum {
	svNil,
	svFalse,
	svTrue,
	svNegOne,
	svZero,
	svOne,
	svTwo,
	svFHalf,
	svFNegOne,
	svFZero,
	svFOne,
	svFTwo,
	svEnd,
	
	svNumSpecialValues
};

extern double gSpecialValues[svNumSpecialValues];

extern PyrMethod *gNullMethod; // used to fill row table

PyrObject* instantiateObject(class GC *gc, PyrClass* classobj, int size, 
	bool fill, bool collect);
	
PyrObject* newPyrObject(class GC *gc, size_t inNumBytes, int inFlags, int inFormat, bool inCollect);
PyrString* newPyrString(class GC *gc, const char *s, int flags, bool collect);
PyrString* newPyrStringN(class GC *gc, int size, int flags, bool collect);
PyrObject* newPyrArray(class GC *gc, int size, int flags, bool collect);
PyrSymbolArray* newPyrSymbolArray(class GC *gc, int size, int flags, bool collect);
PyrInt8Array* newPyrInt8Array(class GC *gc, int size, int flags, bool collect);
PyrInt32Array* newPyrInt32Array(class GC *gc, int size, int flags, bool collect);

PyrObject* copyObject(class GC *gc, PyrObject *inobj, bool collect);
PyrObject* copyObjectRange(class GC *gc, PyrObject *inobj, int start, int end, bool collect);

inline void SetFrame(PyrSlot* slot, PyrFrame* frame) 
{
	 (slot)->ui = ((int)(frame)); 
	  (slot)->utag = tagSFrame - METHRAW((frame)->method.uoblk)->needsHeapContext; 
}

inline void SetFrameOrNil(PyrSlot* slot, PyrFrame* frame)    
{ 
	  if (frame) { 
	  	 (slot)->ui = ((int)(frame)); 
	  	 (slot)->utag = tagSFrame - METHRAW((frame)->method.uoblk)->needsHeapContext; 
	  } else { 
	  	 (slot)->utag = tagNil;  
	     (slot)->ui = 0; 
	  }
}

#endif
