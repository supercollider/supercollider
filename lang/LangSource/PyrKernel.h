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

This file contains the definitions of the core objects that implement the class system.

*/

#pragma once

#include "PyrObject.h"
#include "VMGlobals.h"

#define classClassNumInstVars 19

enum { classIsIntrinsic = 1, classHasIndexableInstances = 2 };

struct PyrClass : public PyrObjectHdr {
    PyrSlot name;
    PyrSlot nextclass;
    PyrSlot superclass;
    PyrSlot subclasses;
    PyrSlot methods;

    PyrSlot instVarNames;
    PyrSlot classVarNames;
    PyrSlot iprototype; // instance prototype
    PyrSlot cprototype; // class var prototype

    PyrSlot constNames;
    PyrSlot constValues; // const values

    PyrSlot instanceFormat;
    PyrSlot instanceFlags;
    PyrSlot classIndex;
    PyrSlot classFlags;
    PyrSlot maxSubclassIndex; // used by isKindOf
    PyrSlot filenameSym;
    PyrSlot charPos;
    PyrSlot classVarIndex;
};


inline bool isKindOf(PyrObjectHdr* obj, struct PyrClass* testclass) {
    int objClassIndex = slotRawInt(&obj->classptr->classIndex);
    return objClassIndex >= slotRawInt(&testclass->classIndex)
        && objClassIndex <= slotRawInt(&testclass->maxSubclassIndex);
}

inline bool isKindOfSlot(PyrSlot* slot, struct PyrClass* testclass) {
    return IsObj(slot) && isKindOf(slotRawObject(slot), testclass);
}

/*
    operations on class:
    numInstVars()
    numClassVars()

*/

struct PyrFrame : public PyrObjectHdr {
    PyrSlot method;
    PyrSlot caller;
    PyrSlot context;
    PyrSlot homeContext;
    PyrSlot ip;
    PyrSlot vars[1];
};

#define FRAMESIZE 5

struct PyrProcess : public PyrObjectHdr {
    PyrSlot classVars;
    PyrSlot interpreter;
    PyrSlot curThread, mainThread;
    PyrSlot sysSchedulerQueue;
    PyrSlot nowExecutingPath;
};


enum { tInit, tStart, tReady, tRunning, tSleeping, tSuspended, tDone };

struct PyrThread : public PyrObjectHdr {
    PyrSlot state, func, stack, method, block, frame, ip, sp;
    PyrSlot numpop, receiver, numArgsPushed;
    PyrSlot parent, terminalValue;
    PyrSlot primitiveError;
    PyrSlot primitiveIndex;
    PyrSlot randData;
    PyrSlot beats, seconds, clock, nextBeat, endBeat, endValue;
    PyrSlot environment;
    PyrSlot exceptionHandler;
    PyrSlot threadPlayer;
    PyrSlot executingPath;
    PyrSlot oldExecutingPath;
    PyrSlot stackSize;
};

#define EVALSTACKDEPTH 512


struct PyrMethodRaw {
#ifdef PYR_SLOTS_GENERIC
    long padding; // used for the tag in the generic pyrslot implementation
#endif
    unsigned short unused1;
    unsigned short specialIndex;
    unsigned short methType;
    unsigned short frameSize;

#ifdef PYR_SLOTS_GENERIC
    long padding2; // used for the tag in generic pyrslot implementation, second slot
#endif

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

struct PyrBlock : public PyrObjectHdr {
    PyrSlot rawData1;
    PyrSlot rawData2;
    PyrSlot code; // byte codes, nil if inlined
    PyrSlot selectors; // method selectors, class names, closures table
    PyrSlot constants; // floating point constants table (to alleviate the literal table problem)
    PyrSlot prototypeFrame; // prototype of an activation frame
    PyrSlot contextDef; // ***defining block context
    PyrSlot argNames; // ***arguments to block
    PyrSlot varNames; // ***variables in block
    PyrSlot sourceCode; // source code if it is a closed function.
};

struct PyrMethod : public PyrBlock {
    PyrSlot ownerclass;
    PyrSlot name;
    PyrSlot primitiveName;
    PyrSlot filenameSym;
    PyrSlot charPos;
    // PyrSlot byteMeter;
    // PyrSlot callMeter;
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
    methRedirectSuper,
    methForwardInstVar,
    methForwardClassVar,
    methPrimitive,
    methBlock
};

struct PyrClosure : public PyrObjectHdr {
    PyrSlot block;
    PyrSlot context;
};

struct PyrInterpreter : public PyrObjectHdr {
    PyrSlot cmdLine, context;
    PyrSlot a, b, c, d, e, f, g, h, i, j;
    PyrSlot k, l, m, n, o, p, q, r, s, t;
    PyrSlot u, v, w, x, y, z;
    PyrSlot codeDump, preProcessor;
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
    svInf,

    svNumSpecialValues
};

extern PyrSlot gSpecialValues[svNumSpecialValues];

extern PyrMethod* gNullMethod; // used to fill row table

PyrObject* instantiateObject(class PyrGC* gc, PyrClass* classobj, int size, bool fill, bool collect);

PyrObject* newPyrObject(class PyrGC* gc, size_t inNumBytes, int inFlags, int inFormat, bool inCollect);
PyrString* newPyrString(class PyrGC* gc, const char* s, int flags, bool collect);
PyrString* newPyrStringN(class PyrGC* gc, int size, int flags, bool collect);
PyrObject* newPyrArray(class PyrGC* gc, int size, int flags, bool collect);
PyrSymbolArray* newPyrSymbolArray(class PyrGC* gc, int size, int flags, bool collect);
PyrInt8Array* newPyrInt8Array(class PyrGC* gc, int size, int flags, bool collect);
PyrInt32Array* newPyrInt32Array(class PyrGC* gc, int size, int flags, bool collect);
PyrDoubleArray* newPyrDoubleArray(class PyrGC* gc, int size, int flags, bool collect);

PyrObject* copyObject(class PyrGC* gc, PyrObject* inobj, bool collect);
PyrObject* copyObjectRange(class PyrGC* gc, PyrObject* inobj, int start, int end, bool collect);
