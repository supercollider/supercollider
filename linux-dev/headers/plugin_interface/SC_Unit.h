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
 

#ifndef _SC_Unit_
#define _SC_Unit_

#include "SC_Types.h"

typedef void (*UnitCtorFunc)(struct Unit* inUnit);
typedef void (*UnitDtorFunc)(struct Unit* inUnit);

typedef void (*UnitCalcFunc)(struct Unit *inThing, int inNumSamples);

struct Unit 
{
	struct World *mWorld;
	struct UnitDef *mUnitDef;
	struct Graph *mParent;
	int16 mNumInputs, mNumOutputs;
	int16 mCalcRate;
	int16 mSpecialIndex;		// used by unary and binary ops
	int16 mParentIndex;
	int16 mDone;
	
	struct Wire **mInput, **mOutput;
	struct Rate *mRate;
	struct Dimension *mDimension;
	float **mInBuf, **mOutBuf;
	
	UnitCalcFunc mCalcFunc;
	int mBufLength;
};
typedef struct Unit Unit;

// easy macros, the unit variable must be named 'unit'.
#define IN(index)  (unit->mInBuf[index])
#define OUT(index) (unit->mOutBuf[index])
#define INRATE(index) (unit->mInput[index]->mCalcRate)
#define SETCALC(func) (unit->mCalcFunc = (UnitCalcFunc)&func)

#define CALCSLOPE(next,prev) ((next - prev) * unit->mRate->mSlopeFactor)
#define SAMPLERATE (unit->mRate->mSampleRate)
#define SAMPLEDUR (unit->mRate->mSampleDur)
#define BUFLENGTH (unit->mBufLength)

typedef void (*UnitCmdFunc)(struct Unit *unit, struct sc_msg_iter *args);
typedef void (*PlugInCmdFunc)(void* inUserData, struct sc_msg_iter *args);

#endif
