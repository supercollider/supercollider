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


#include "SC_PlugIn.h"

static InterfaceTable *ft;

//////////////////////////////////////////////////////////////////////////////////////////////////

struct IOUnit : public Unit
{
	int32* m_busTouched;
	float m_fbusChannel;
	float *m_bus;
};

struct XOut : public IOUnit
{
	float m_xfade;
};

struct OffsetOut : public IOUnit
{
	float *m_saved;
};

extern "C"
{
	void load(InterfaceTable *inTable);

	void Control_Ctor(Unit *inUnit);
	void Control_next_k(Unit *unit, int inNumSamples);
	void Control_next_1(Unit *unit, int inNumSamples);

	void ControlTrig_Ctor(Unit *inUnit);
	void ControlTrig_next_k(Unit *unit, int inNumSamples);

	void InTrig_Ctor(IOUnit *unit);
	void InTrig_next_k(IOUnit *unit, int inNumSamples);

	void In_Ctor(IOUnit *unit);
	void In_next_a(IOUnit *unit, int inNumSamples);
	void In_next_k(IOUnit *unit, int inNumSamples);

	void Out_Ctor(IOUnit *unit);
	void Out_next_a(IOUnit *unit, int inNumSamples);
	void Out_next_k(IOUnit *unit, int inNumSamples);

	void XOut_Ctor(XOut *unit);
	void XOut_next_a(XOut *unit, int inNumSamples);
	void XOut_next_k(XOut *unit, int inNumSamples);

	void ReplaceOut_Ctor(IOUnit *unit);
	void ReplaceOut_next_a(IOUnit *unit, int inNumSamples);
	void ReplaceOut_next_k(IOUnit *unit, int inNumSamples);

	void OffsetOut_Ctor(OffsetOut *unit);
	void OffsetOut_Dtor(OffsetOut* unit);
	void OffsetOut_next_a(OffsetOut *unit, int inNumSamples);

}

//////////////////////////////////////////////////////////////////////////////////////////////////

void Control_next_k(Unit *unit, int inNumSamples)
{
	int numChannels = unit->mNumOutputs;
	float **mapin = unit->mParent->mMapControls + unit->mSpecialIndex;
	for (int i=0; i<numChannels; ++i, mapin++) {
		float *out = OUT(i);
		*out = **mapin;
		//Print("cin %d %g\n", i, *out);
	}
}

void Control_next_1(Unit *unit, int inNumSamples)
{
	float **mapin = unit->mParent->mMapControls + unit->mSpecialIndex;
	float *out = OUT(0);
	*out = **mapin;
}

void Control_Ctor(Unit* unit)
{
	//Print("Control_Ctor\n");
	if (unit->mNumOutputs == 1) {
		SETCALC(Control_next_1);
		Control_next_1(unit, 1);
	} else {
		SETCALC(Control_next_k);
		Control_next_k(unit, 1);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void ControlTrig_next_k(Unit *unit, int inNumSamples)
{
	float *out = OUT(0);
	*out = (unit->mWorld->mBufCounter == unit->mParent->mControlTouched) ? 1.f : 0.f;
}

void ControlTrig_Ctor(Unit* unit)
{
	//Print("Control_Ctor\n");
	SETCALC(ControlTrig_next_k);
	ControlTrig_next_k(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void In_next_a(IOUnit *unit, int inNumSamples)
{
	World *world = unit->mWorld;
	int bufLength = world->mBufLength;
	int numChannels = unit->mNumOutputs;

	float fbusChannel = ZIN0(0);
	if (fbusChannel != unit->m_fbusChannel) {
		unit->m_fbusChannel = fbusChannel;
		int busChannel = (int)fbusChannel;
		int lastChannel = busChannel + numChannels;
		
		if (!(busChannel < 0 || lastChannel > world->mNumAudioBusChannels)) {
			unit->m_bus = world->mAudioBus + (busChannel * bufLength);
			unit->m_busTouched = world->mAudioBusTouched + busChannel;
		}
	}
	
	float *in = unit->m_bus;
	int32 *touched = unit->m_busTouched;
	int32 bufCounter = unit->mWorld->mBufCounter;
	
	for (int i=0; i<numChannels; ++i, in += bufLength) {
		float *out = OUT(i);
		if (touched[i] == bufCounter) Copy(inNumSamples, out, in);
		else Fill(inNumSamples, out, 0.f);
	}
}

void In_next_k(IOUnit *unit, int inNumSamples)
{
//Print("->In_next_k\n");
	World *world = unit->mWorld;
	int numChannels = unit->mNumOutputs;

	float fbusChannel = ZIN0(0);
	if (fbusChannel != unit->m_fbusChannel) {
		unit->m_fbusChannel = fbusChannel;
		int busChannel = (int)fbusChannel;
		int lastChannel = busChannel + numChannels;
		
		if (!(busChannel < 0 || lastChannel > world->mNumControlBusChannels)) {
			unit->m_bus = world->mControlBus + busChannel;
		}
	}
	
	float *in = unit->m_bus;
	for (int i=0; i<numChannels; ++i, in++) {
		float *out = OUT(i);
		*out = *in;
	}
//Print("<-In_next_k\n");
}

void In_Ctor(IOUnit* unit)
{
Print("->In_Ctor\n");
	World *world = unit->mWorld;
	unit->m_fbusChannel = -1.;

	if (unit->mCalcRate == calc_FullRate) {
		SETCALC(In_next_a);
		unit->m_bus = world->mAudioBus;
		unit->m_busTouched = world->mAudioBusTouched;
		In_next_a(unit, 1);
	} else {
		SETCALC(In_next_k);
		unit->m_bus = world->mControlBus;
		//unit->m_busTouched = world->mControlBusTouched;
		In_next_k(unit, 1);
	}
//Print("<-In_Ctor\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void InTrig_next_k(IOUnit *unit, int inNumSamples)
{
	World *world = unit->mWorld;
	int numChannels = unit->mNumOutputs;

	float fbusChannel = ZIN0(0);
	if (fbusChannel != unit->m_fbusChannel) {
		unit->m_fbusChannel = fbusChannel;
		int busChannel = (int)fbusChannel;
		int lastChannel = busChannel + numChannels;
		
		if (!(busChannel < 0 || lastChannel > world->mNumControlBusChannels)) {
			unit->m_bus = world->mControlBus + busChannel;
			unit->m_busTouched = world->mControlBusTouched + busChannel;
		}
	}
	
	float *in = unit->m_bus;
	int32 *touched = unit->m_busTouched;
	int32 bufCounter = unit->mWorld->mBufCounter;
	for (int i=0; i<numChannels; ++i, in++) {
		float *out = OUT(i);
		if (touched[i] == bufCounter) *out = *in;
		else *out = 0.f;
	}
}

void InTrig_Ctor(IOUnit* unit)
{
	World *world = unit->mWorld;
	unit->m_fbusChannel = -1.;

	if (unit->mCalcRate == calc_FullRate) {
		SETCALC(ClearUnitOutputs);
		ClearUnitOutputs(unit, 1);
	} else {
		SETCALC(InTrig_next_k);
		unit->m_bus = world->mControlBus;
		unit->m_busTouched = world->mControlBusTouched;
		InTrig_next_k(unit, 1);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////


void ReplaceOut_next_a(IOUnit *unit, int inNumSamples)
{
	World *world = unit->mWorld;
	int bufLength = world->mBufLength;
	int numChannels = unit->mNumInputs - 1;

	float fbusChannel = ZIN0(0);
	if (fbusChannel != unit->m_fbusChannel) {
		unit->m_fbusChannel = fbusChannel;
		int busChannel = (int)fbusChannel;
		int lastChannel = busChannel + numChannels;
		
		if (!(busChannel < 0 || lastChannel > world->mNumAudioBusChannels)) {
			unit->m_bus = world->mAudioBus + (busChannel * bufLength);
			unit->m_busTouched = world->mAudioBusTouched + busChannel;
		}
	}
	
	float *out = unit->m_bus;
	int32 *touched = unit->m_busTouched;
	int32 bufCounter = unit->mWorld->mBufCounter;
	for (int i=0; i<numChannels; ++i, out+=bufLength) {
		float *in = IN(i+1);
		Copy(inNumSamples, out, in);
		touched[i] = bufCounter;
	}
}

void ReplaceOut_next_k(IOUnit *unit, int inNumSamples)
{
	World *world = unit->mWorld;
	int numChannels = unit->mNumInputs - 1;

	float fbusChannel = ZIN0(0);
	if (fbusChannel != unit->m_fbusChannel) {
		unit->m_fbusChannel = fbusChannel;
		int busChannel = (int)fbusChannel;
		int lastChannel = busChannel + numChannels;
		
		if (!(busChannel < 0 || lastChannel > world->mNumControlBusChannels)) {
			unit->m_bus = world->mControlBus + busChannel;
			unit->m_busTouched = world->mControlBusTouched + busChannel;
		}
	}
	
	float *out = unit->m_bus;
	int32 *touched = unit->m_busTouched;
	int32 bufCounter = unit->mWorld->mBufCounter;
	for (int i=0; i<numChannels; ++i, out++) {
		float *in = IN(i+1);
		*out = *in;
		touched[i] = bufCounter;
	}
}

void ReplaceOut_Ctor(IOUnit* unit)
{
	World *world = unit->mWorld;
	if (unit->mCalcRate == calc_FullRate) {
		SETCALC(ReplaceOut_next_a);
		unit->m_bus = world->mAudioBus;
		unit->m_busTouched = world->mAudioBusTouched;
	} else {
		SETCALC(ReplaceOut_next_k);
		unit->m_bus = world->mControlBus;
		unit->m_busTouched = world->mControlBusTouched;
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void Out_next_a(IOUnit *unit, int inNumSamples)
{
	//Print("Out_next_a %d\n", unit->mNumInputs);
	World *world = unit->mWorld;
	int bufLength = world->mBufLength;
	int numChannels = unit->mNumInputs - 1;

	float fbusChannel = ZIN0(0);
	if (fbusChannel != unit->m_fbusChannel) {
		unit->m_fbusChannel = fbusChannel;
		int busChannel = (int)fbusChannel;
		int lastChannel = busChannel + numChannels;
		
		if (!(busChannel < 0 || lastChannel > world->mNumAudioBusChannels)) {
			unit->m_bus = world->mAudioBus + (busChannel * bufLength);
			unit->m_busTouched = world->mAudioBusTouched + busChannel;
		}
	}
	
	float *out = unit->m_bus;
	int32 *touched = unit->m_busTouched;
	int32 bufCounter = unit->mWorld->mBufCounter;
	for (int i=0; i<numChannels; ++i, out+=bufLength) {
		float *in = IN(i+1);
		if (touched[i] == bufCounter) Accum(inNumSamples, out, in);
		else {
			Copy(inNumSamples, out, in);
			touched[i] = bufCounter;
		}
		//Print("out %d %g %g\n", i, in[0], out[0]);
	}
}

void Out_next_k(IOUnit *unit, int inNumSamples)
{
	World *world = unit->mWorld;
	int numChannels = unit->mNumInputs - 1;

	float fbusChannel = ZIN0(0);
	if (fbusChannel != unit->m_fbusChannel) {
		unit->m_fbusChannel = fbusChannel;
		int busChannel = (int)fbusChannel;
		int lastChannel = busChannel + numChannels;
		
		if (!(busChannel < 0 || lastChannel > world->mNumControlBusChannels)) {
			unit->m_bus = world->mControlBus + busChannel;
			unit->m_busTouched = world->mControlBusTouched + busChannel;
		}
	}
	float *out = unit->m_bus;
	int32 *touched = unit->m_busTouched;
	int32 bufCounter = unit->mWorld->mBufCounter;
	for (int i=0; i<numChannels; ++i, out++) {
		float *in = IN(i+1);
		if (touched[i] == bufCounter) *out += *in;
		else {
			*out = *in;
			touched[i] = bufCounter;
		}
	}
}

void Out_Ctor(IOUnit* unit)
{
	//Print("->Out_Ctor\n");
	World *world = unit->mWorld;
	unit->m_fbusChannel = -1.;
	
	if (unit->mCalcRate == calc_FullRate) {
		SETCALC(Out_next_a);
		unit->m_bus = world->mAudioBus;
		unit->m_busTouched = world->mAudioBusTouched;
	} else {
		SETCALC(Out_next_k);
		unit->m_bus = world->mControlBus;
		unit->m_busTouched = world->mControlBusTouched;
	}
	//Print("<-Out_Ctor\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

void XOut_next_a(XOut *unit, int inNumSamples)
{
	World *world = unit->mWorld;
	int bufLength = world->mBufLength;
	int numChannels = unit->mNumInputs - 2;

	float fbusChannel = ZIN0(0);
	if (fbusChannel != unit->m_fbusChannel) {
		unit->m_fbusChannel = fbusChannel;
		int busChannel = (int)fbusChannel;
		int lastChannel = busChannel + numChannels;
		
		if (!(busChannel < 0 || lastChannel > world->mNumAudioBusChannels)) {
			unit->m_bus = world->mAudioBus + (busChannel * bufLength);
			unit->m_busTouched = world->mAudioBusTouched + busChannel;
		}
	}
	
	float next_xfade = ZIN0(1);
	float xfade0 = unit->m_xfade;
	float *out = unit->m_bus;
	int32 *touched = unit->m_busTouched;
	int32 bufCounter = unit->mWorld->mBufCounter;
	if (xfade0 != next_xfade) {
		float slope = CALCSLOPE(next_xfade, xfade0);
		for (int i=0; i<numChannels; ++i) {
			float xfade = xfade0;
			float *in = IN(i+2);
			if (touched[i] == bufCounter) {
				LOOP(inNumSamples,
					float zin = *in;
					float zout = *out;
					*out = zout + xfade * (zin - zout);
					//if (xxi==0) Print("x %d %d %g %g %g %g\n", bufCounter, i, zin, zout, xfade, *out);
					xfade += slope;
					++out; ++in;
				);
			} else {
				LOOP(inNumSamples,
					float zin = *in;
					*out = xfade * zin;
					xfade += slope;
					++out; ++in;
				);
				touched[i] = bufCounter;
			}
		}
	} else if (xfade0 == 1.f) {
		for (int i=0; i<numChannels; ++i, out+=bufLength) {
			float *in = IN(i+2);
			Copy(inNumSamples, out, in);
			touched[i] = bufCounter;
		}
	} else if (xfade0 == 0.f) {
		// do nothing.
	} else {
		for (int i=0; i<numChannels; ++i) {
			float *in = IN(i+2);
			if (touched[i] == bufCounter) {
				LOOP(inNumSamples,
					float zin = *in;
					float zout = *out;
					*out = zout + xfade0 * (zin - zout);
					++out; ++in;
				);
			} else {
				LOOP(inNumSamples,
					float zin = *in;
					*out = xfade0 * zin;
					++out; ++in;
				);
				touched[i] = bufCounter;
			}
		}
	}
	unit->m_xfade = next_xfade;
}

void XOut_next_k(XOut *unit, int inNumSamples)
{
	World *world = unit->mWorld;
	int numChannels = unit->mNumInputs - 2;

	float fbusChannel = ZIN0(0);
	if (fbusChannel != unit->m_fbusChannel) {
		unit->m_fbusChannel = fbusChannel;
		int busChannel = (int)fbusChannel;
		int lastChannel = busChannel + numChannels;
		
		if (!(busChannel < 0 || lastChannel > world->mNumControlBusChannels)) {
			unit->m_bus = world->mControlBus + busChannel;
			unit->m_busTouched = world->mControlBusTouched + busChannel;
		}
	}

	float xfade = ZIN0(1);
	float *out = unit->m_bus;
	int32 *touched = unit->m_busTouched;
	int32 bufCounter = unit->mWorld->mBufCounter;
	for (int i=0; i<numChannels; ++i, out++) {
		float *in = IN(i+2);
		if (touched[i] == bufCounter) {
			float zin = *in;
			float zout = *out;
			*out = zout + xfade * (zin - zout);
		} else {
			*out = xfade * *in;
			touched[i] = bufCounter;
		}
	}
}

void XOut_Ctor(XOut* unit)
{
	//Print("->Out_Ctor\n");
	World *world = unit->mWorld;
	unit->m_fbusChannel = -1.;
	unit->m_xfade = ZIN0(1);
	if (unit->mCalcRate == calc_FullRate) {
		SETCALC(XOut_next_a);
		unit->m_bus = world->mAudioBus;
		unit->m_busTouched = world->mAudioBusTouched;
	} else {
		SETCALC(XOut_next_k);
		unit->m_bus = world->mControlBus;
		unit->m_busTouched = world->mControlBusTouched;
	}
	//Print("<-Out_Ctor\n");
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void OffsetOut_next_a(OffsetOut *unit, int inNumSamples)
{
	World *world = unit->mWorld;
	int bufLength = world->mBufLength;
	int numChannels = unit->mNumInputs - 1;

	float fbusChannel = ZIN0(0);
	if (fbusChannel != unit->m_fbusChannel) {
		unit->m_fbusChannel = fbusChannel;
		int busChannel = (int)fbusChannel;
		int lastChannel = busChannel + numChannels;
		
		if (!(busChannel < 0 || lastChannel > world->mNumAudioBusChannels)) {
			unit->m_bus = world->mAudioBus + (busChannel * bufLength);
			unit->m_busTouched = world->mAudioBusTouched + busChannel;
		}
	}

	int32 offset = unit->mParent->mSampleOffset;
	int32 remain = BUFLENGTH - offset;
	
	float *out = unit->m_bus;
	float *saved = unit->m_saved;
	int32 *touched = unit->m_busTouched;
	int32 bufCounter = unit->mWorld->mBufCounter;
	for (int i=0; i<numChannels; ++i, out+=bufLength, saved += offset) {
		float *in = IN(i+1);
		if (touched[i] == bufCounter) {
			Accum(offset, out, saved);
			Accum(remain, out + offset, in);
		} else {
			Copy(offset, out, saved);
			Copy(remain, out + offset, in);
			touched[i] = bufCounter;
		}
		Copy(offset, saved, in + remain);
		//Print("out %d %g %g\n", i, in[0], out[0]);
	}
}

void OffsetOut_Ctor(OffsetOut* unit)
{
	//Print("->Out_Ctor\n");
	World *world = unit->mWorld;
	unit->m_fbusChannel = -1.;

	SETCALC(OffsetOut_next_a);
	unit->m_bus = world->mAudioBus;
	unit->m_busTouched = world->mAudioBusTouched;
	int32 offset = unit->mParent->mSampleOffset;
	int numChannels = unit->mNumInputs - 1;

	unit->m_saved = (float*)RTAlloc(unit->mWorld, offset * numChannels * sizeof(float));
	//Print("<-Out_Ctor\n");
}

void OffsetOut_Dtor(OffsetOut* unit)
{
	RTFree(unit->mWorld, unit->m_saved);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineDtorUnit(OffsetOut);
	DefineSimpleUnit(XOut);
	DefineUnit("Control", sizeof(Unit), (UnitCtorFunc)&Control_Ctor, 0);
	DefineUnit("ReplaceOut", sizeof(IOUnit), (UnitCtorFunc)&ReplaceOut_Ctor, 0);
	DefineUnit("Out", sizeof(IOUnit), (UnitCtorFunc)&Out_Ctor, 0);
	DefineUnit("In", sizeof(IOUnit), (UnitCtorFunc)&In_Ctor, 0);
	DefineUnit("InTrig", sizeof(IOUnit), (UnitCtorFunc)&InTrig_Ctor, 0);
}
