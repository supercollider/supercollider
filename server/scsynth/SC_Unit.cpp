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


#include "SC_Unit.h"
#include "SC_UnitSpec.h"
#include "SC_UnitDef.h"
#include "SC_World.h"
#include "SC_Wire.h"
#include "Unroll.h"
#include <stdio.h>
#include "SC_Prototypes.h"

#ifdef _WIN32
// workaround for IN/OUT conflict with Win32 headers. see SC_Unit.h for details
// (note: the pragma momentarily suppresses compiler warning about such conflict)
#pragma warning(disable: 4005)
#define IN SC_IN
#define OUT SC_OUT
#pragma warning(default: 4005)
#endif

void Unit_ChooseMulAddFunc(Unit* unit);

Unit* Unit_New(World *inWorld, UnitSpec *inUnitSpec, char*& memory)
{
	UnitDef *def = inUnitSpec->mUnitDef;

	Unit *unit = (Unit*)memory;
	memory += def->mAllocSize;

	unit->mWorld = inWorld;
	unit->mUnitDef = def;

	int numInputs = inUnitSpec->mNumInputs;
	int numOutputs = inUnitSpec->mNumOutputs;
	unit->mNumInputs = numInputs;
	unit->mNumOutputs = numOutputs;
	int numPorts = numInputs + numOutputs;

	unit->mInput = (Wire**)memory;
	memory += numPorts * sizeof(Wire*);

	unit->mOutput = unit->mInput + numInputs;

	unit->mInBuf = (float**)memory;
	memory += numPorts * sizeof(float*);

	unit->mOutBuf = unit->mInBuf + numInputs;

	unit->mCalcRate = inUnitSpec->mCalcRate;
	unit->mSpecialIndex = inUnitSpec->mSpecialIndex;
	Rate* rateInfo = unit->mRate = inUnitSpec->mRateInfo;
	unit->mBufLength = rateInfo->mBufLength;

	unit->mDone = false;

	return unit;
}

void Unit_Dtor(Unit *inUnit)
{
}

void Unit_ZeroOutputs(Unit *unit, int inNumSamples)
{
	long numOuts = unit->mNumOutputs;
	for (int i=0; i<numOuts; ++i) {
		float *out = OUT(i);
		Clear(inNumSamples, out);
	}

}

void Unit_EndCalc(Unit *inUnit, int inNumSamples)
{
	inUnit->mDone = true;
	Unit_ZeroOutputs(inUnit, inNumSamples);
}

void Unit_End(Unit *inUnit)
{
	inUnit->mCalcFunc = (UnitCalcFunc)&Unit_EndCalc;
}

