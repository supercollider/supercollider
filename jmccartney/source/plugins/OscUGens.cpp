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
#include <limits.h>

static InterfaceTable *ft;

struct BufUnit : public Unit
{
	SndBuf *m_buf;
	float m_fbufnum;
};

struct TableLookup : public BufUnit
{
	double m_cpstoinc, m_radtoinc;
	int32 mTableSize;
	int32 m_lomask;
};

struct DegreeToKey : public BufUnit
{
	SndBuf *m_buf;
	float m_fbufnum;
	int32 mPrevIndex;
	float mPrevKey;
	int32 mOctave;
};

struct Select : public Unit
{
};

struct Index : public BufUnit
{
};

struct WrapIndex : public BufUnit
{
};

struct FoldIndex : public BufUnit
{
};

struct Shaper : public BufUnit
{
	float mOffset;
	float mPrevIn;
};

struct SigOsc : public BufUnit
{
	int32 mTableSize;
	double m_cpstoinc;
	float mPhase;
};

struct FSinOsc : public Unit
{
	double m_b1, m_y1, m_y2, m_freq;
};

struct PSinGrain : public Unit
{
	double m_b1, m_y1, m_y2;
	double m_level, m_slope, m_curve;
	int32 mCounter;
};

struct Osc : public TableLookup
{
	int32 m_phase, m_phaseoffset;
	float m_phasein;
};

struct SinOsc : public TableLookup
{
	int32 m_phase, m_phaseoffset;
	float m_phasein;
};

struct OscN : public TableLookup
{
	int32 m_phase, m_phaseoffset;
	float m_phasein;
};

struct COsc : public TableLookup
{
	int32 m_phase1, m_phase2;
};

#if 0
struct COsc2 : public TableLookup
{
	float *mTable2, *mTable3;
	int32 m_phase1, m_phase2;
};

struct OscX4 : public TableLookup
{
	float* mTables[8];
	int32 m_phase;
	float m_xpos, m_ypos;
};

struct OscX2 : public TableLookup
{
	float* mTables[4];
	int32 m_phase, m_counter;
	double m_xfadeslope, m_xfade;
};

struct PMOsc : public Unit
{
	int32 m_phase1, m_phase2, m_iphasemod;
	float m_pmindex;
	double m_cpstoinc, m_radtoinc;
};
#endif

struct Formant : public Unit
{
	int32 m_phase1, m_phase2, m_phase3;
	double m_cpstoinc;
};



struct Blip : public Unit
{
	int32 m_phase, m_numharm, m_N;
	float m_freqin, m_scale;
	double m_cpstoinc;
};

struct Saw : public Unit
{
	int32 m_phase, m_N;
	float m_freqin, m_scale, m_y1;
	double m_cpstoinc;
};

struct Pulse : public Unit
{
	int32 m_phase, m_phaseoff, m_N;
	float m_freqin, m_scale, m_y1;
	double m_cpstoinc;
};

struct Klang : public Unit
{
	float *m_coefs;
	int32 m_numpartials;
};

struct Klank : public Unit
{
	float *m_coefs;
	float m_x1, m_x2;
	int32 m_numpartials;
};



#define xlomask8  0x000003FC
#define xlomask9  0x000007FC
#define xlomask10 0x00000FFC
#define xlomask11 0x00001FFC
#define xlomask12 0x00003FFC
#define xlomask13 0x00007FFC

#define xlomask8i  0x000007F8
#define xlomask9i  0x00000FF8
#define xlomask10i 0x00001FF8
#define xlomask11i 0x00003FF8
#define xlomask12i 0x00007FF8
#define xlomask13i 0x0000FFF8

#define onecyc13 0x20000000



//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
void load(InterfaceTable *inTable);

void DegreeToKey_Ctor(DegreeToKey *unit);
void DegreeToKey_next_1(DegreeToKey *unit, int inNumSamples);
void DegreeToKey_next_k(DegreeToKey *unit, int inNumSamples);
void DegreeToKey_next_a(DegreeToKey *unit, int inNumSamples);

void Select_Ctor(Select *unit);
void Select_next_1(Select *unit, int inNumSamples);
void Select_next_k(Select *unit, int inNumSamples);
void Select_next_a(Select *unit, int inNumSamples);

void Index_Ctor(Index *unit);
void Index_next_1(Index *unit, int inNumSamples);
void Index_next_k(Index *unit, int inNumSamples);
void Index_next_a(Index *unit, int inNumSamples);

void FoldIndex_Ctor(FoldIndex *unit);
void FoldIndex_next_1(FoldIndex *unit, int inNumSamples);
void FoldIndex_next_k(FoldIndex *unit, int inNumSamples);
void FoldIndex_next_a(FoldIndex *unit, int inNumSamples);

void WrapIndex_Ctor(WrapIndex *unit);
void WrapIndex_next_1(WrapIndex *unit, int inNumSamples);
void WrapIndex_next_k(WrapIndex *unit, int inNumSamples);
void WrapIndex_next_a(WrapIndex *unit, int inNumSamples);

void Shaper_Ctor(Shaper *unit);
void Shaper_next_1(Shaper *unit, int inNumSamples);
void Shaper_next_k(Shaper *unit, int inNumSamples);
void Shaper_next_a(Shaper *unit, int inNumSamples);

void SigOsc_Ctor(SigOsc *unit);
void SigOsc_next_1(SigOsc *unit, int inNumSamples);
void SigOsc_next_k(SigOsc *unit, int inNumSamples);
void SigOsc_next_a(SigOsc *unit, int inNumSamples);

void FSinOsc_Ctor(FSinOsc *unit);
void FSinOsc_next(FSinOsc *unit, int inNumSamples);

void PSinGrain_Ctor(PSinGrain *unit);
void PSinGrain_next(PSinGrain *unit, int inNumSamples);

void SinOsc_Ctor(SinOsc *unit);
void SinOsc_next_ikk(SinOsc *unit, int inNumSamples);
void SinOsc_next_ika(SinOsc *unit, int inNumSamples);
void SinOsc_next_iak(SinOsc *unit, int inNumSamples);
void SinOsc_next_iaa(SinOsc *unit, int inNumSamples);

void Osc_Ctor(Osc *unit);
void Osc_next_ikk(Osc *unit, int inNumSamples);
void Osc_next_ika(Osc *unit, int inNumSamples);
void Osc_next_iak(Osc *unit, int inNumSamples);
void Osc_next_iaa(Osc *unit, int inNumSamples);

void OscN_Ctor(OscN *unit);
void OscN_next_nkk(OscN *unit, int inNumSamples);
void OscN_next_nka(OscN *unit, int inNumSamples);
void OscN_next_nak(OscN *unit, int inNumSamples);
void OscN_next_naa(OscN *unit, int inNumSamples);

void COsc_Ctor(COsc *unit);
void COsc_next(COsc *unit, int inNumSamples);

#if 0
void COsc2_Ctor(COsc2 *unit);
void COsc2_next(COsc2 *unit, int inNumSamples);

void PMOsc_Ctor(PMOsc *unit);
void PMOsc_next_kkkk(PMOsc *unit, int inNumSamples);

void OscX4_Ctor(OscX4 *unit);
void OscX4_SetTables(OscX4 *unit, int32 inSize, float* inTableA, float* inTableB, 
		float* inTableC, float* inTableD);
void OscX4_next(OscX4 *unit, int inNumSamples);

void OscX2_Ctor(OscX2 *unit);
void OscX2_SetTables(OscX2 *unit, int32 inSize, float* inTableA, float* inTableB);
void OscX2_next(OscX2 *unit, int inNumSamples);
#endif

void Formant_Ctor(Formant *unit);
void Formant_next(Formant *unit, int inNumSamples);

void Blip_Ctor(Blip *unit);
void Blip_next(Blip *unit, int inNumSamples);

void Saw_Ctor(Saw *unit);
void Saw_next(Saw *unit, int inNumSamples);

void Pulse_Ctor(Pulse *unit);
void Pulse_next(Pulse *unit, int inNumSamples);

void Klang_Dtor(Klang *unit);
void Klang_Ctor(Klang *unit);
float Klang_SetCoefs(Klang *unit);
void Klang_next(Klang *unit, int inNumSamples);

void Klank_Dtor(Klank *unit);
void Klank_Ctor(Klank *unit);
void Klank_SetCoefs(Klank *unit);
void Klank_next(Klank *unit, int inNumSamples);

}

//////////////////////////////////////////////////////////////////////////////////////////////////

#define GET_TABLE \
		float fbufnum = ZIN0(0); \
		if (fbufnum != unit->m_fbufnum) { \
			int bufnum = (int)fbufnum; \
			World *world = unit->mWorld; \
			if (bufnum < 0 || bufnum >= world->mNumSndBufs) bufnum = 0; \
			unit->m_buf = world->mSndBufs + bufnum; \
		} \
		SndBuf *buf = unit->m_buf; \
		int tableSize = buf->samples;

////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
void TableLookup_SetTable(TableLookup* unit, int32 inSize, float* inTable)
{
	unit->mTable0 = inTable;
	unit->mTable1 = inTable + 1;
	unit->mTableSize = inSize;
	unit->mMaxIndex = unit->mTableSize - 1;
	unit->mFMaxIndex = unit->mMaxIndex;
	unit->m_radtoinc = unit->mTableSize * (rtwopi * 65536.);
	unit->m_cpstoinc = unit->mTableSize * SAMPLEDUR * 65536.;
	//postbuf("TableLookup_SetTable unit->m_radtoinc %g %d %g\n", m_radtoinc, unit->mTableSize, rtwopi);
}
*/

////////////////////////////////////////////////////////////////////////////////////////////////////////

void DegreeToKey_Ctor(DegreeToKey *unit)
{
	if (BUFLENGTH == 1) {
		SETCALC(DegreeToKey_next_1);
	} else if (INRATE(0) == calc_FullRate) {
		SETCALC(DegreeToKey_next_a);
	} else {
		SETCALC(DegreeToKey_next_k);
	}
	unit->mOctave = (int32)ZIN0(1);
	unit->mPrevIndex = LONG_MIN;
	unit->mPrevKey = 0.;
	DegreeToKey_next_1(unit, 1);
}

void DegreeToKey_next_1(DegreeToKey *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;

	int32 key, oct;
	
	int32 octave = unit->mOctave;
	float val;

	int32 index = (int32)floor(ZIN0(1));
	if (index == unit->mPrevIndex) {
		val = unit->mPrevKey;
	} else if (index < 0) {
		unit->mPrevIndex = index;
		key = tableSize + index % tableSize;
		oct = (index + 1) / tableSize - 1;
		val = unit->mPrevKey = table[key] + octave * oct;
	} else if (index > maxindex) {
		unit->mPrevIndex = index;
		key = index % tableSize;
		oct = index / tableSize;
		val = unit->mPrevKey = table[key] + octave * oct;
	} else {
		unit->mPrevIndex = index;
		val = unit->mPrevKey = table[index];
	}
	ZOUT0(0) = val;

}

void DegreeToKey_next_k(DegreeToKey *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;

	float *out = ZOUT(0);

	int32 key, oct;
	float octave = unit->mOctave;
	float val;

	int32 index = (int32)floor(ZIN0(1));
	if (index == unit->mPrevIndex) {
		val = unit->mPrevKey;
	} else if (index < 0) {
		unit->mPrevIndex = index;
		key = tableSize + index % tableSize;
		oct = (index + 1) / tableSize - 1;
		val = unit->mPrevKey = table[key] + octave * oct;
	} else if (index > maxindex) {
		unit->mPrevIndex = index;
		key = index % tableSize;
		oct = index / tableSize;
		val = unit->mPrevKey = table[key] + octave * oct;
	} else {
		unit->mPrevIndex = index;
		val = unit->mPrevKey = table[index];
	}
	LOOP(inNumSamples,
		ZXP(out) = val;
	);

}


void DegreeToKey_next_a(DegreeToKey *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;


	float *out = ZOUT(0);
	float *in = ZIN(1);
	int32 previndex = unit->mPrevIndex;
	float prevkey = unit->mPrevKey;
	int32 key, oct;
	
	float octave = unit->mOctave;

	LOOP(inNumSamples,
		int32 index = (int32)floor(ZXP(in));
		if (index == previndex) {
			ZXP(out) = prevkey;
		} else if (index < 0) {
			previndex = index;
			key = tableSize + index % tableSize;
			oct = (index + 1) / tableSize - 1;
			ZXP(out) = prevkey = table[key] + octave * oct;
		} else if (index > maxindex) {
			previndex = index;
			key = index % tableSize;
			oct = index / tableSize;
			ZXP(out) = prevkey = table[key] + octave * oct;
		} else {
			previndex = index;
			ZXP(out) = prevkey = table[index];
		}
	);
	unit->mPrevIndex = previndex;
	unit->mPrevKey = prevkey;

}

////////////////////////////////////////////////////////////////////////////////////

void Select_Ctor(Select *unit)
{
	if (BUFLENGTH == 1) {
		SETCALC(Select_next_1);
	} else if (INRATE(0) == calc_FullRate) {
		SETCALC(Select_next_a);
	} else {
		SETCALC(Select_next_k);
	}
	Select_next_1(unit, 1);
}

void Select_next_1(Select *unit, int inNumSamples)
{
	int32 maxindex = unit->mNumInputs;
	int32 index = (int32)ZIN0(0) + 1;
	index = sc_clip(index, 1, maxindex);
	ZOUT0(0) = ZIN0(index);

}

void Select_next_k(Select *unit, int inNumSamples)
{
	int32 maxindex = unit->mNumInputs;
	int32 index = (int32)ZIN0(0) + 1;
	index = sc_clip(index, 1, maxindex);

	float *out = OUT(0);
	float *in = IN(index);
	
	Copy(inNumSamples, out, in);

}

void Select_next_a(Select *unit, int inNumSamples)
{
	int32 maxindex = unit->mNumInputs;

	float *out = ZOUT(0);
	float **in = unit->mInBuf;
	
	for (int i=0; i<inNumSamples; ++i) {
		int32 index = (int32)ZXP(in) + 1;
		index = sc_clip(index, 1, maxindex);
		ZXP(out) = in[index][i];
	}

}

////////////////////////////////////////////////////////////////////////////////////

void Index_Ctor(Index *unit)
{
	if (BUFLENGTH == 1) {
		SETCALC(Index_next_1);
	} else if (INRATE(0) == calc_FullRate) {
		SETCALC(Index_next_a);
	} else {
		SETCALC(Index_next_k);
	}
	Index_next_1(unit, 1);
}

void Index_next_1(Index *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;
	
	int32 index = (int32)ZIN0(0);
	index = sc_clip(index, 0, maxindex);
	ZOUT0(0) = table[index];

}

void Index_next_k(Index *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;

	float *out = ZOUT(0);

	int32 index = (int32)ZIN0(1);
	index = sc_clip(index, 0, maxindex);
	float val = table[index];
	LOOP(inNumSamples,
		ZXP(out) = val;
	);

}


void Index_next_a(Index *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;

	float *out = ZOUT(0);
	float *in = ZIN(1);
	
	LOOP(inNumSamples,
		int32 index = (int32)ZXP(in);
		index = sc_clip(index, 0, maxindex);
		ZXP(out) = table[index];
	);

}


////////////////////////////////////////////////////////////////////////////////////


void FoldIndex_Ctor(FoldIndex *unit)
{
	if (BUFLENGTH == 1) {
		SETCALC(FoldIndex_next_1);
	} else if (INRATE(0) == calc_FullRate) {
		SETCALC(FoldIndex_next_a);
	} else {
		SETCALC(FoldIndex_next_k);
	}
	FoldIndex_next_1(unit, 1);
}

void FoldIndex_next_1(FoldIndex *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;

	int32 index = (int32)ZIN0(1);
	index = sc_fold(index, 0, maxindex);
	ZOUT0(0) = table[index];

}

void FoldIndex_next_k(FoldIndex *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;

	int32 index = (int32)ZIN0(1);
	float *out = ZOUT(0);

	index = sc_fold(index, 0, maxindex);
	float val = table[index];
	LOOP(inNumSamples,
		ZXP(out) = val;
	);

}


void FoldIndex_next_a(FoldIndex *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;

	float *out = ZOUT(0);
	float *in = ZIN(1);

	LOOP(inNumSamples,
		int32 index = (int32)ZXP(in);
		index = sc_fold(index, 0, maxindex);
		ZXP(out) = table[index];
	);

}


////////////////////////////////////////////////////////////////////////////////////

void WrapIndex_Ctor(WrapIndex *unit)
{
	if (BUFLENGTH == 1) {
		SETCALC(WrapIndex_next_1);
	} else if (INRATE(0) == calc_FullRate) {
		SETCALC(WrapIndex_next_a);
	} else {
		SETCALC(WrapIndex_next_k);
	}
	WrapIndex_next_1(unit, 1);
}

void WrapIndex_next_1(WrapIndex *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;

	int32 index = (int32)floor(ZIN0(1));
	index = sc_wrap(index, 0, maxindex);
	ZOUT0(0) = table[index];

}

void WrapIndex_next_k(WrapIndex *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;

	float *out = ZOUT(0);

	int32 index = (int32)ZIN0(1);
	index = sc_wrap(index, 0, maxindex);
	float val = table[index];
	LOOP(inNumSamples,
		ZXP(out) = val;
	);

}


void WrapIndex_next_a(WrapIndex *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		int32 maxindex = tableSize - 1;

	float *out = ZOUT(0);
	float *in = ZIN(1);

	LOOP(inNumSamples,
		int32 index = (int32)ZXP(in);
		index = sc_wrap(index, 0, maxindex);
		ZXP(out) = table[index];
	);

}

////////////////////////////////////////////////////////////////////////////////////

void Shaper_Ctor(Shaper *unit)
{
	if (BUFLENGTH == 1) {
		SETCALC(Shaper_next_1);
	} else if (INRATE(0) == calc_FullRate) {
		SETCALC(Shaper_next_a);
	} else {
		SETCALC(Shaper_next_k);
	}
	unit->mPrevIn = ZIN0(0);
	Shaper_next_1(unit, 1);
}

void Shaper_next_1(Shaper *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		int32 maxindex = tableSize - 1;
		float offset = maxindex * 0.5;
	
	float val;

	float fin = ZIN0(1);
	if (fin < -1.f) val = *(float*)((char*)table0);
	else if (fin > 1.f) val = *(float*)((char*)table0  + maxindex);
	else {
		float findex = offset + fin * offset;
		int32 index = (int32)findex;
		float pfrac = findex - (index - 1);
		index <<= 3;
		float val1 = *(float*)((char*)table0 + index);
		float val2 = *(float*)((char*)table1 + index);
		val = val1 + val2 * pfrac;
	}
	ZOUT0(0) = val;

}

void Shaper_next_k(Shaper *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		int32 maxindex = tableSize - 1;
		float offset = maxindex * 0.5;

	float *out = ZOUT(0);
	
	float val;

	float fin = ZIN0(1);
	float phaseinc = (fin - unit->mPrevIn) * offset;
	unit->mPrevIn = fin;

	LOOP(inNumSamples,
		fin += phaseinc;
		if (fin < -1.f) val = *(float*)((char*)table0);
		else if (fin > 1.f) val = *(float*)((char*)table0  + maxindex);
		else {
			float findex = offset + fin * offset;
		
			int32 index = (int32)findex;
			float pfrac = findex - (index - 1);
			index <<= 3;
			float val1 = *(float*)((char*)table0 + index);
			float val2 = *(float*)((char*)table1 + index);
			val = val1 + val2 * pfrac;
		}
		ZXP(out) = val;
	);
	
}

void Shaper_next_a(Shaper *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		int32 maxindex = tableSize - 1;
		float offset = maxindex * 0.5;

	float *out = ZOUT(0);
	float *in = ZIN(1);
	float val;

	LOOP(inNumSamples,
		float fin = ZXP(in);
 		if (fin < -1.f) val = *(float*)((char*)table0);
 		else if (fin > 1.f) val = *(float*)((char*)table0  + maxindex);
		else {
			float findex = offset + fin * offset;
		
			int32 index = (int32)findex;
			float pfrac = findex - (index - 1);
			index <<= 3;
			float val1 = *(float*)((char*)table0 + index);
			float val2 = *(float*)((char*)table1 + index);
			val = val1 + val2 * pfrac;
		}
		ZXP(out) = val;
	);

}



////////////////////////////////////////////////////////////////////////////////////

void SigOsc_Ctor(SigOsc *unit)
{
	if (BUFLENGTH == 1) {
		SETCALC(SigOsc_next_1);
	} else if (INRATE(0) == calc_FullRate) {
		SETCALC(SigOsc_next_a);
	} else {
		SETCALC(SigOsc_next_k);
	}
	unit->mPhase = 0.f;
	SigOsc_next_1(unit, 1);
}


void SigOsc_next_1(SigOsc *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		int32 maxindex = tableSize - 1;
		float maxphase = (float)maxindex;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			unit->m_cpstoinc = tableSize * SAMPLEDUR * 65536.; 
		}

	float phase = unit->mPhase;
	
	while (phase < 0.f) phase += maxphase;
	while (phase >= maxphase) phase -= maxphase;
	int32 iphase = (int32)phase;
	float pfrac = phase - iphase;
	float val1 = *(float*)((char*)table0 + iphase);
	float val2 = *(float*)((char*)table1 + iphase);
	float val = lininterp(pfrac, val1, val2);
	phase += ZIN0(1) * unit->m_cpstoinc;

	ZOUT0(0) = val;

	unit->mPhase = phase;

}

void SigOsc_next_k(SigOsc *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		int32 maxindex = tableSize - 1;
		float maxphase = (float)maxindex;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			unit->m_cpstoinc = tableSize * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);

	float phase = unit->mPhase;
	
	float freq = ZIN0(1) * unit->m_cpstoinc;
	
	LOOP(inNumSamples, 
		while (phase < 0.f) phase += maxphase;
		while (phase >= maxphase) phase -= maxphase;
		int32 iphase = (int32)phase;
		float pfrac = phase - iphase;
		float val1 = *(float*)((char*)table0 + iphase);
		float val2 = *(float*)((char*)table1 + iphase);
		float val = lininterp(pfrac, val1, val2);
		phase += freq;

		ZXP(out) = val;
	);

	unit->mPhase = phase;
	
}

void SigOsc_next_a(SigOsc *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		int32 maxindex = tableSize - 1;
		float maxphase = (float)maxindex;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			unit->m_cpstoinc = tableSize * SAMPLEDUR * 65536.; 
		}
		
	float *out = ZOUT(0);
	float *freqin = ZIN(1);
	float phase = unit->mPhase;
	float cpstoinc = unit->m_cpstoinc;

	LOOP(inNumSamples,
		while (phase < 0.f) phase += maxphase;
		while (phase >= maxphase) phase -= maxphase;
		int32 iphase = (int32)phase;
		float pfrac = phase - iphase;
		float val1 = *(float*)((char*)table0 + iphase);
		float val2 = *(float*)((char*)table1 + iphase);
		float val = lininterp(pfrac, val1, val2);
		phase += ZXP(freqin) * cpstoinc;

		ZXP(out) = val;
	);
	
	unit->mPhase = phase;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void FSinOsc_Ctor(FSinOsc *unit)
{
	SETCALC(FSinOsc_next);
	unit->m_freq = ZIN0(0);
	float iphase = ZIN0(1);
	float w = unit->m_freq * unit->mRate->mRadiansPerSample;
	unit->m_b1 = 2. * cos(w);
	unit->m_y1 = sin(iphase);
	unit->m_y2 = sin(iphase - w);
	
	ZOUT0(0) = unit->m_y1;
}

void FSinOsc_next(FSinOsc *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	double freq = ZIN0(0);
	double b1;
	if (freq != unit->m_freq) {
		unit->m_freq = freq;
		double w = freq * unit->mRate->mRadiansPerSample;
		unit->m_b1 = b1 = 2.f * cos(w);
	} else {
		b1 = unit->m_b1;
	}
	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	//Print("y %g %g  b1 %g\n", y1, y2, b1);
	//Print("%d %d\n", unit->mRate->mFilterLoops, unit->mRate->mFilterRemain);
	LOOP(unit->mRate->mFilterLoops, 
		ZXP(out) = y0 = b1 * y1 - y2; 
		ZXP(out) = y2 = b1 * y0 - y1; 
		ZXP(out) = y1 = b1 * y2 - y0; 
	);
	LOOP(unit->mRate->mFilterRemain, 
		ZXP(out) = y0 = b1 * y1 - y2; 
		y2 = y1; 
		y1 = y0; 
	);
	//Print("y %g %g  b1 %g\n", y1, y2, b1);
	unit->m_y1 = y1;
	unit->m_y2 = y2;
	unit->m_b1 = b1;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void PSinGrain_Ctor(PSinGrain *unit)
{
	SETCALC(PSinGrain_next);
	float freq = ZIN0(0);
	float dur  = ZIN0(1);
	float amp  = ZIN0(2);

	float w = freq * unit->mRate->mRadiansPerSample;
	float sdur = SAMPLERATE * dur;

	float rdur = 1.f / sdur;
	float rdur2 = rdur * rdur;
	
	unit->m_level = 0.f;
	unit->m_slope = 4.0 * (rdur - rdur2);	// ampslope
	unit->m_curve = -8.0 * rdur2;			// ampcurve
	unit->mCounter = (int32)(sdur + .5);

	/* calc feedback param and initial conditions */
	unit->m_b1 = 2. * cos(w);
	unit->m_y1 = 0.f;
	unit->m_y2 = -sin(w) * amp;
	ZOUT0(0) = 0.f;
}


void PSinGrain_next(PSinGrain *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float y0;
	float y1 = unit->m_y1;
	float y2 = unit->m_y2;
	float b1 = unit->m_b1;
	float level = unit->m_level;
	float slope = unit->m_slope;
	float curve = unit->m_curve;
	int32 counter = unit->mCounter;
	int32 remain = inNumSamples;
	int32 nsmps;
	do {
		if (counter<=0) {
			nsmps = remain;
			remain = 0;
			LOOP(nsmps, ZXP(out) = 0.f;); // can't use Clear bcs might not be aligned
		} else {
			nsmps = sc_min(remain, counter);
			remain -= nsmps;
			counter -= nsmps;
			if (nsmps == inNumSamples) {
				nsmps = unit->mRate->mFilterLoops;
				LOOP(nsmps, 
					y0 = b1 * y1 - y2; 
					ZXP(out) = y0 * level;
					level += slope;
					slope += curve;
					y2 = b1 * y0 - y1; 
					ZXP(out) = y2 * level;
					level += slope;
					slope += curve;
					y1 = b1 * y2 - y0; 
					ZXP(out) = y1 * level;
					level += slope;
					slope += curve;
				); 
				nsmps = unit->mRate->mFilterRemain;
				LOOP(nsmps, 
					y0 = b1 * y1 - y2; 
					y2 = y1; 
					y1 = y0; 
					ZXP(out) = y0 * level;
					level += slope;
					slope += curve;
				); 
			} else {
				LOOP(nsmps, 
					y0 = b1 * y1 - y2; 
					y2 = y1; 
					y1 = y0; 
					ZXP(out) = y0 * level;
					level += slope;
					slope += curve;
				); 
			}
			if (counter == 0) {
				GraphEnd(unit->mParent);
			}
		}
	} while (remain>0);
	unit->m_level = level;
	unit->m_slope = slope;
	unit->m_y1 = y1;
	unit->m_y2 = y2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void SinOsc_Ctor(SinOsc *unit)
{
	if (INRATE(0) == calc_FullRate) {
		if (INRATE(1) == calc_FullRate) {
			//postbuf("next_iaa\n");
			SETCALC(SinOsc_next_iaa);
		} else {
			//postbuf("next_iak\n");
			SETCALC(SinOsc_next_iak);
		}
	} else {
		if (INRATE(1) == calc_FullRate) {
			//postbuf("next_ika\n");
			SETCALC(SinOsc_next_ika);
		} else {
			//postbuf("next_ikk\n");
			SETCALC(SinOsc_next_ikk);
		}
	}
	int tableSize2 = ft->mSineSize;
	unit->m_phasein = ZIN0(1);
	unit->m_phaseoffset = (int32)(unit->m_phasein * unit->m_radtoinc);
	unit->m_lomask = (tableSize2 - 1) << 3; 
	unit->m_radtoinc = tableSize2 * (rtwopi * 65536.); 
	unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 

	if (INRATE(0) == calc_FullRate) {
		unit->m_phase = 0;
	} else {
		unit->m_phase = unit->m_phaseoffset;
	}
	SinOsc_next_ikk(unit, 1);
}

//////////////!!!

void SinOsc_next_ikk(SinOsc *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float phasein = ZIN0(1);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	int32 phaseinc = freq + (int32)(CALCSLOPE(phasein, unit->m_phasein) * unit->m_radtoinc);
	unit->m_phasein = phasein;
	
	LOOP(inNumSamples,
		ZXP(out) = lookupi1(table0, table1, phase, lomask);
		phase += phaseinc;
	);
	unit->m_phase = phase;
		
}


void SinOsc_next_ika(SinOsc *unit, int inNumSamples)
{

	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float *phasein = ZIN(1);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	float radtoinc = unit->m_radtoinc;
	//postbuf("SinOsc_next_ika %d %g %d\n", inNumSamples, radtoinc, phase);
	LOOP(inNumSamples,
		int32 phaseoffset = phase + (int32)(radtoinc * ZXP(phasein));
		ZXP(out) = lookupi1(table0, table1, phaseoffset, lomask);
		phase += freq;
	);
	unit->m_phase = phase;
	//unit->m_phasein = phasein;
	
}

void SinOsc_next_iaa(SinOsc *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *freqin = ZIN(0);
	float *phasein = ZIN(1);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	float cpstoinc = unit->m_cpstoinc;
	float radtoinc = unit->m_radtoinc;
	//postbuf("SinOsc_next_iaa %d %g %g %d\n", inNumSamples, cpstoinc, radtoinc, phase);
	LOOP(inNumSamples,
		int32 phaseoffset = phase + (int32)(radtoinc * ZXP(phasein));
		float z = lookupi1(table0, table1, phaseoffset, lomask);
		phase += (int32)(cpstoinc * ZXP(freqin));
		ZXP(out) = z;
	);
	unit->m_phase = phase;
	//unit->m_phasein = ZX(phasein);
	
}


void SinOsc_next_iak(SinOsc *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *freqin = ZIN(0);
	float phasein = ZIN0(1);
	
	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;

	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	float cpstoinc = unit->m_cpstoinc;
	int32 phaseinc = (int32)(CALCSLOPE(phasein, unit->m_phasein) * unit->m_radtoinc);
	int32 phaseoffset = unit->m_phaseoffset;
	
	LOOP(inNumSamples,
		float z = lookupi1(table0, table1, phase + phaseoffset, lomask);
		phaseoffset += phaseinc;
		phase += (int32)(cpstoinc * ZXP(freqin));
		ZXP(out) = z;
	);
	unit->m_phase = phase;
	unit->m_phasein = phasein;
	unit->m_phaseoffset = phaseoffset;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void Osc_Ctor(Osc *unit)
{
	if (INRATE(0) == calc_FullRate) {
		if (INRATE(1) == calc_FullRate) {
			//postbuf("next_iaa\n");
			SETCALC(Osc_next_iaa);
		} else {
			//postbuf("next_iak\n");
			SETCALC(Osc_next_iak);
		}
	} else {
		if (INRATE(1) == calc_FullRate) {
			//postbuf("next_ika\n");
			SETCALC(Osc_next_ika);
		} else {
			//postbuf("next_ikk\n");
			SETCALC(Osc_next_ikk);
		}
	}
	unit->m_buf = unit->mWorld->mSndBufs;
	unit->mTableSize = -1;
	unit->m_phasein = ZIN0(1);
	unit->m_phaseoffset = (int32)(unit->m_phasein * unit->m_radtoinc);
	if (INRATE(1) == calc_FullRate) {
		unit->m_phase = 0;
	} else {
		unit->m_phase = unit->m_phaseoffset;
	}
	Osc_next_ikk(unit, 1);
}

//////////////!!!

void Osc_next_ikk(Osc *unit, int inNumSamples)
{
	// get table
	GET_TABLE

		float *table0 = buf->data;
		float *table1 = table0 + 1;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			int tableSize2 = tableSize >> 1;
			unit->m_lomask = (tableSize2 - 1) << 3; // Osc, OscN, COsc, COsc, COsc2, OscX4, OscX2
			unit->m_radtoinc = tableSize2 * (rtwopi * 65536.); // Osc, OscN, PMOsc
			// SigOsc, Osc, OscN, PMOsc, COsc, COsc2, OscX4, OscX2
			unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);
	float freqin = ZIN0(1);
	float phasein = ZIN0(2);
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	int32 phaseinc = freq + (int32)(CALCSLOPE(phasein, unit->m_phasein) * unit->m_radtoinc);
	unit->m_phasein = phasein;
	
	LOOP(inNumSamples,
		ZXP(out) = lookupi1(table0, table1, phase, lomask);
		phase += phaseinc;
	);
	unit->m_phase = phase;
		
}


void Osc_next_ika(Osc *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			int tableSize2 = tableSize >> 1;
			unit->m_lomask = (tableSize2 - 1) << 3; // Osc, OscN, COsc, COsc, COsc2, OscX4, OscX2
			unit->m_radtoinc = tableSize2 * (rtwopi * 65536.); // Osc, OscN, PMOsc
			// SigOsc, Osc, OscN, PMOsc, COsc, COsc2, OscX4, OscX2
			unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);
	float freqin = ZIN0(1);
	float *phasein = ZIN(2);
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	float radtoinc = unit->m_radtoinc;
	//postbuf("Osc_next_ika %d %g %d\n", inNumSamples, radtoinc, phase);
	LOOP(inNumSamples,
		int32 phaseoffset = phase + (int32)(radtoinc * ZXP(phasein));
		ZXP(out) = lookupi1(table0, table1, phaseoffset, lomask);
		phase += freq;
	);
	unit->m_phase = phase;
	//unit->m_phasein = phasein;
	
}

void Osc_next_iaa(Osc *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			int tableSize2 = tableSize >> 1;
			unit->m_lomask = (tableSize2 - 1) << 3; // Osc, OscN, COsc, COsc, COsc2, OscX4, OscX2
			unit->m_radtoinc = tableSize2 * (rtwopi * 65536.); // Osc, OscN, PMOsc
			// SigOsc, Osc, OscN, PMOsc, COsc, COsc2, OscX4, OscX2
			unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);
	float *freqin = ZIN(1);
	float *phasein = ZIN(2);
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	float cpstoinc = unit->m_cpstoinc;
	float radtoinc = unit->m_radtoinc;
	//postbuf("Osc_next_iaa %d %g %g %d\n", inNumSamples, cpstoinc, radtoinc, phase);
	LOOP(inNumSamples,
		int32 phaseoffset = phase + (int32)(radtoinc * ZXP(phasein));
		float z = lookupi1(table0, table1, phaseoffset, lomask);
		phase += (int32)(cpstoinc * ZXP(freqin));
		ZXP(out) = z;
	);
	unit->m_phase = phase;
	//unit->m_phasein = ZX(phasein);
	
}


void Osc_next_iak(Osc *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			int tableSize2 = tableSize >> 1;
			unit->m_lomask = (tableSize2 - 1) << 3; // Osc, OscN, COsc, COsc, COsc2, OscX4, OscX2
			unit->m_radtoinc = tableSize2 * (rtwopi * 65536.); // Osc, OscN, PMOsc
			// SigOsc, Osc, OscN, PMOsc, COsc, COsc2, OscX4, OscX2
			unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);
	float *freqin = ZIN(1);
	float phasein = ZIN0(2);
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	float cpstoinc = unit->m_cpstoinc;
	int32 phaseinc = (int32)(CALCSLOPE(phasein, unit->m_phasein) * unit->m_radtoinc);
	int32 phaseoffset = unit->m_phaseoffset;
	
	LOOP(inNumSamples,
		float z = lookupi1(table0, table1, phase + phaseoffset, lomask);
		phaseoffset += phaseinc;
		phase += (int32)(cpstoinc * ZXP(freqin));
		ZXP(out) = z;
	);
	unit->m_phase = phase;
	unit->m_phasein = phasein;
	unit->m_phaseoffset = phaseoffset;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////

void OscN_Ctor(OscN *unit)
{
	if (INRATE(0) == calc_FullRate) {
		if (INRATE(1) == calc_FullRate) {
			//postbuf("next_naa\n");
			SETCALC(OscN_next_naa);
		} else {
			//postbuf("next_nak\n");
			SETCALC(OscN_next_nak);
		}
	} else {
		if (INRATE(1) == calc_FullRate) {
			//postbuf("next_nka\n");
			SETCALC(OscN_next_nka);
		} else {
			//postbuf("next_nkk\n");
			SETCALC(OscN_next_nkk);
		}
	}
	unit->m_phase = 0;
	unit->m_phasein = 0;
	unit->m_phaseoffset = (int32)(ZIN0(1) * unit->m_radtoinc);
	OscN_next_nkk(unit, 1);
}


void OscN_next_nkk(OscN *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			int tableSize2 = tableSize >> 1;
			unit->m_lomask = (tableSize2 - 1) << 3; // Osc, OscN, COsc, COsc, COsc2, OscX4, OscX2
			unit->m_radtoinc = tableSize2 * (rtwopi * 65536.); // Osc, OscN, PMOsc
			// SigOsc, Osc, OscN, PMOsc, COsc, COsc2, OscX4, OscX2
			unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);
	float freqin = ZIN0(1);
	float phasein = ZIN0(2);
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	int32 phaseinc = freq + (int32)(CALCSLOPE(phasein, unit->m_phasein) * unit->m_radtoinc);
	LOOP(inNumSamples,
		ZXP(out) = *(float*)((char*)table + ((phase >> xlobits) & lomask));
		phase += phaseinc;
	);
	unit->m_phase = phase;
	
}



void OscN_next_nka(OscN *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			int tableSize2 = tableSize >> 1;
			unit->m_lomask = (tableSize2 - 1) << 3; // Osc, OscN, COsc, COsc, COsc2, OscX4, OscX2
			unit->m_radtoinc = tableSize2 * (rtwopi * 65536.); // Osc, OscN, PMOsc
			// SigOsc, Osc, OscN, PMOsc, COsc, COsc2, OscX4, OscX2
			unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);
	float freqin = ZIN0(1);
	float *phasein = ZIN(2);
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	float radtoinc = unit->m_radtoinc;
	LOOP(inNumSamples,
		int32 pphase = phase + (int32)(radtoinc * ZXP(phasein));
		ZXP(out) = *(float*)((char*)table + ((pphase >> xlobits) & lomask));
		phase += freq;
	);
	unit->m_phase = phase;
	
}

void OscN_next_naa(OscN *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			int tableSize2 = tableSize >> 1;
			unit->m_lomask = (tableSize2 - 1) << 3; // Osc, OscN, COsc, COsc, COsc2, OscX4, OscX2
			unit->m_radtoinc = tableSize2 * (rtwopi * 65536.); // Osc, OscN, PMOsc
			// SigOsc, Osc, OscN, PMOsc, COsc, COsc2, OscX4, OscX2
			unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);
	float *freqin = ZIN(1);
	float *phasein = ZIN(2);
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	float cpstoinc = unit->m_cpstoinc;
	float radtoinc = unit->m_radtoinc;
	LOOP(inNumSamples,
		int32 pphase = phase + (int32)(radtoinc * ZXP(phasein));
		float z = *(float*)((char*)table + ((pphase >> xlobits) & lomask));
		phase += (int32)(cpstoinc * ZXP(freqin));
		ZXP(out) = z;
	);
	unit->m_phase = phase;
	
}


void OscN_next_nak(OscN *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table = buf->data;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			int tableSize2 = tableSize >> 1;
			unit->m_lomask = (tableSize2 - 1) << 3; // Osc, OscN, COsc, COsc, COsc2, OscX4, OscX2
			unit->m_radtoinc = tableSize2 * (rtwopi * 65536.); // Osc, OscN, PMOsc
			// SigOsc, Osc, OscN, PMOsc, COsc, COsc2, OscX4, OscX2
			unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);
	float *freqin = ZIN(1);
	float phasein = ZIN0(2);
	
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	float cpstoinc = unit->m_cpstoinc;
	int32 phaseinc = (int32)(CALCSLOPE(phasein, unit->m_phasein) * unit->m_radtoinc);
	LOOP(inNumSamples,
		float z = *(float*)((char*)table + ((phase >> xlobits) & lomask));
		phase += (int32)(cpstoinc * ZXP(freqin)) + phaseinc;
		ZXP(out) = z;
	);
	unit->m_phase = phase;
	unit->m_phasein = phasein;
	//unit->m_phaseoffset = phaseoffset;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

#if 0

void PMOsc_Ctor(PMOsc *unit)
{
	SETCALC(PMOsc_next_kkkk);
	float phasemod = ZIN0(3);
	unit->m_cpstoinc = ft->mSineSize * SAMPLEDUR * 65536.;
	unit->m_radtoinc = ft->mSineSize * 65536. * rtwopi;
	unit->m_phase1 = 0;
	unit->m_phase2 = 0;
	unit->m_iphasemod = (int32)(unit->m_radtoinc * phasemod);
	PMOsc_next_kkkk(1);
}

void PMOsc_next_kkkk(PMOsc *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float carfreqin = ZIN0(0);
	float modfreqin = ZIN0(1);
	float next_pmindex = ZIN0(2);
	float phasemod = ZIN0(3);
	
	float* sine = ft->mSine;
	float* pmsine = gPMSine;
	int32 phase1 = unit->m_phase1;
	int32 phase2 = unit->m_phase2;
	float cpstoinc = unit->m_cpstoinc;
	float radtoinc = unit->m_radtoinc;
	int32 iphasemod = unit->m_iphasemod;
	
	float next_iphasemod = radtoinc * phasemod;
	int32 iphasemod_slope = (next_iphasemod - iphasemod) * unit->mRate->mSlopeFactor;
	int32 end_iphasemod = iphasemod + iphasemod_slope * inNumSamples;
	float pmindex = unit->m_pmindex;
	float pmindex_slope = (next_pmindex - pmindex) * unit->mRate->mSlopeFactor;
	
	int32 carfreq = (int32)(cpstoinc * carfreqin);
	int32 modfreq = (int32)(cpstoinc * modfreqin) + iphasemod_slope;

	if (pmindex_slope != 0.) {
		LOOP(inNumSamples,
			/* calculate modulator */
			float pmout = *(float*)((char*)pmsine + ((phase1 >> xlobits) & xlomask13)) * pmindex;
			phase1 += modfreq; 
			/* calculate carrier */
			*++out = *(float*)((char*)sine + (((phase2 + (int32)pmout) >> xlobits) & xlomask13));
			phase2 += carfreq;
			pmindex += pmindex_slope;
		);
	} else {
		LOOP(inNumSamples,
			/* calculate modulator */
			float pmout = *(float*)((char*)pmsine + ((phase1 >> xlobits) & xlomask13)) * pmindex;
			phase1 += modfreq; 
			/* calculate carrier */
			*++out = *(float*)((char*)sine + (((phase2 + (int32)pmout) >> xlobits) & xlomask13));
			phase2 += carfreq;
		);
	}
	unit->m_phase1 = phase1;
	unit->m_phase2 = phase2;
	unit->m_iphasemod = end_iphasemod;
	unit->m_pmindex = pmindex;
	
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////


void COsc_Ctor(COsc *unit)
{
	SETCALC(COsc_next);
	unit->m_phase1 = 0;
	unit->m_phase2 = 0;
	COsc_next(unit, 1);
}


void COsc_next(COsc *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			int tableSize2 = tableSize >> 1;
			unit->m_lomask = (tableSize2 - 1) << 3; // Osc, OscN, COsc, COsc, COsc2, OscX4, OscX2
			// SigOsc, Osc, OscN, PMOsc, COsc, COsc2, OscX4, OscX2
			unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);
	float freqin = ZIN0(1);
	float beats = ZIN0(2) * 0.5f;
	
	int32 phase1 = unit->m_phase1;
	int32 phase2 = unit->m_phase2;
	int32 lomask = unit->m_lomask;
	
	int32 cfreq = (int32)(unit->m_cpstoinc * freqin);
	int32 beatf = (int32)(unit->m_cpstoinc * beats);
	int32 freq1 = cfreq + beatf;
	int32 freq2 = cfreq - beatf;
	LOOP(inNumSamples,
		float a = lookupi1(table0, table1, phase1, lomask);
		float b = lookupi1(table0, table1, phase2, lomask);
		ZXP(out) = a + b;
		phase1 += freq1;
		phase2 += freq2;
	);
	unit->m_phase1 = phase1;
	unit->m_phase2 = phase2;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


#if 0
void COsc2_Ctor(COsc2 *unit)
{
	SETCALC(COsc2_next);
	unit->m_cpstoinc = unit->mTableSize * SAMPLEDUR * 65536.;
	unit->m_phase1 = 0;
	unit->m_phase2 = 0;
	COsc2_next(unit, 1);
}


void COsc2_SetTables(COsc2 *unit, int32 inSize, float* inTableA, float* inTableB)
{
	unit->mTable0 = inTableA;
	unit->mTable1 = inTableA + 1;
	unit->mTable2 = inTableB;
	unit->mTable3 = inTableB + 1;
	unit->mTableSize = inSize;
	unit->m_cpstoinc = inSize * SAMPLEDUR * 65536.;
	unit->m_lomask = (inSize - 1) << 3;
}


void COsc2_next(COsc2 *unit, int inNumSamples)
{
	// get table
	GET_TABLE
		int size = buf->samples;
		int tableSize2 = size >> 1;
		float *table0 = buf->data;
		float *table1 = table0 + 1;
		float *table2 = table0 + size2;
		float *table3 = table2 + 1;
		if (tableSize != unit->mTableSize) {
			unit->mTableSize = tableSize;
			unit->m_lomask = (tableSize2 - 1) << 3; // Osc, OscN, COsc, COsc, COsc2, OscX4, OscX2
			// SigOsc, Osc, OscN, PMOsc, COsc, COsc2, OscX4, OscX2
			unit->m_cpstoinc = tableSize2 * SAMPLEDUR * 65536.; 
		}

	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float beats = ZIN0(1);
	
	float *table0 = unit->mTable0;
	float *table1 = unit->mTable1;
	float *table2 = unit->mTable2;
	float *table3 = unit->mTable3;
	int32 phase1 = unit->m_phase1;
	int32 phase2 = unit->m_phase2;
	int32 lomask = unit->m_lomask;
	
	int32 cfreq = (int32)(unit->m_cpstoinc * freqin);
	int32 beatf = (int32)(unit->m_cpstoinc * beats);
	int32 freq1 = cfreq + beatf;
	int32 freq2 = cfreq - beatf;
	LOOP(inNumSamples,
		float a = lookupi1(table0, table1, phase1, lomask);
		float b = lookupi1(table2, table3, phase2, lomask);
		ZXP(out) = a + b;
		phase1 += freq1;
		phase2 += freq2;
	);
	unit->m_phase1 = phase1;
	unit->m_phase2 = phase2;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


void OscX4_Ctor(OscX4 *unit)
{
	SETCALC(OscX4_next);
	unit->m_cpstoinc = unit->mTableSize * SAMPLEDUR * 65536.;
	unit->m_xpos = ZIN0(1) * 0.5 + 0.5;
	unit->m_ypos = ZIN0(2) * 0.5 + 0.5;
	unit->m_phase = 0;
	OscX4_next(unit, 1);
}

void OscX4_SetTables(OscX4 *unit, int32 inSize, float* inTableA, float* inTableB, 
		float* inTableC, float* inTableD)
{
	unit->mTableSize = inSize;
	unit->mTables[0] = inTableA;
	unit->mTables[1] = inTableA + 1;
	unit->mTables[2] = inTableB;
	unit->mTables[3] = inTableB + 1;
	unit->mTables[4] = inTableC;
	unit->mTables[5] = inTableC + 1;
	unit->mTables[6] = inTableD;
	unit->mTables[7] = inTableD + 1;
	unit->m_cpstoinc = inSize * SAMPLEDUR * 65536.;
	unit->m_lomask = (inSize - 1) << 3;
}

void OscX4_next(OscX4 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float next_xpos = ZIN0(1) * 0.5 + 0.5;
	float next_ypos = ZIN0(2) * 0.5 + 0.5;
	
	float xpos = unit->m_xpos;
	float ypos = unit->m_ypos;
	float val1, val2;
	
	float **tables = unit->mTables;
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	float xpos_slope = CALCSLOPE(next_xpos, xpos);
	float ypos_slope = CALCSLOPE(next_ypos, ypos);
	LOOP(inNumSamples,
		float pfrac = PhaseFrac1(phase);
		int32 index = (phase >> xlobits1) & lomask;
			val1 = *(float*)((char*)tables[0] + index);
			val2 = *(float*)((char*)tables[1] + index);
		float out0 = val1 + val2 * pfrac;
			val1 = *(float*)((char*)tables[2] + index);
			val2 = *(float*)((char*)tables[3] + index);
		float out1 = val1 + val2 * pfrac;
			val1 = *(float*)((char*)tables[4] + index);
			val2 = *(float*)((char*)tables[5] + index);
		float out2 = val1 + val2 * pfrac;
			val1 = *(float*)((char*)tables[6] + index);
			val2 = *(float*)((char*)tables[7] + index);
		float out3 = val1 + val2 * pfrac;
		
		float out8 = lininterp(xpos, out0, out1);
		float out9 = lininterp(xpos, out2, out3);
		ZXP(out)  = lininterp(ypos, out9, out8);		
		
		phase += freq;
		xpos += xpos_slope;
		ypos += ypos_slope;
	);
	unit->m_xpos = xpos;
	unit->m_ypos = ypos;
	unit->m_phase = phase;
	
}

void OscX4_nextB(OscX4 *unit, int inNumSamples);
void OscX4_nextB(OscX4 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float next_xpos = ZIN0(1) * 0.5 + 0.5;
	float next_ypos = ZIN0(2) * 0.5 + 0.5;
	
	float xpos = unit->m_xpos;
	float ypos = unit->m_ypos;
	float val1, val2;
	
	float **tables = unit->mTables;
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	float xpos_slope = CALCSLOPE(next_xpos, xpos);
	float ypos_slope = CALCSLOPE(next_ypos, ypos);
	LOOP(inNumSamples,
		float pfrac = PhaseFrac1(phase);
		int32 index = (phase >> xlobits) & lomask;
			val1 = tables[0][index];
			val2 = tables[1][index];
		float out0 = val1 + val2 * pfrac;
			val1 = tables[2][index];
			val2 = tables[3][index];
		float out1 = val1 + val2 * pfrac;
			val1 = tables[4][index];
			val2 = tables[5][index];
		float out2 = val1 + val2 * pfrac;
			val1 = tables[6][index];
			val2 = tables[7][index];
		float out3 = val1 + val2 * pfrac;
		
		float out8 = lininterp(xpos, out0, out1);
		float out9 = lininterp(xpos, out2, out3);
		ZXP(out)  = lininterp(ypos, out9, out8);		
		
		phase += freq;
		xpos += xpos_slope;
		ypos += ypos_slope;
	);
	unit->m_xpos = xpos;
	unit->m_ypos = ypos;
	unit->m_phase = phase;
	
}

void OscX4_nextC(OscX4 *unit, int inNumSamples);
void OscX4_nextC(OscX4 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float next_xpos = ZIN0(1) * 0.5 + 0.5;
	float next_ypos = ZIN0(2) * 0.5 + 0.5;
	
	float xpos = unit->m_xpos;
	float ypos = unit->m_ypos;
	float val1, val2;
	
	float **tables = unit->mTables;
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	float xpos_slope = CALCSLOPE(next_xpos, xpos);
	float ypos_slope = CALCSLOPE(next_ypos, ypos);
	LOOP(inNumSamples,
		float pfrac = PhaseFrac(phase);
		int32 index = (phase >> xlobits) & lomask;
			val1 = tables[0][index];
			val2 = tables[1][index];
		float out0 = val1 + val2 * pfrac;
			val1 = tables[2][index];
			val2 = tables[3][index];
		float out1 = val1 + val2 * pfrac;
			val1 = tables[4][index];
			val2 = tables[5][index];
		float out2 = val1 + val2 * pfrac;
			val1 = tables[6][index];
			val2 = tables[7][index];
		float out3 = val1 + val2 * pfrac;
		
		float out8 = lininterp(xpos, out0, out1);
		float out9 = lininterp(xpos, out2, out3);
		ZXP(out)  = lininterp(ypos, out9, out8);		
		
		phase += freq;
		xpos += xpos_slope;
		ypos += ypos_slope;
	);
	unit->m_xpos = xpos;
	unit->m_ypos = ypos;
	unit->m_phase = phase;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////




void OscX2_Ctor(OscX2 *unit)
{
	SETCALC(OscX2_next);
	unit->m_cpstoinc = unit->mTableSize * SAMPLEDUR * 65536.;
	unit->m_phase = 0;
	unit->m_counter = 0;
	OscX2_next(unit, 1);
}

void OscX2_SetTables(OscX2 *unit, int32 inSize, float* inTableA, float* inTableB)
{
	unit->mTableSize = inSize;
	unit->mTables[0] = inTableA;
	unit->mTables[1] = inTableA + 1;
	unit->mTables[2] = inTableB;
	unit->mTables[3] = inTableB + 1;
	unit->m_cpstoinc = inSize * SAMPLEDUR * 65536.;
}


void OscX2_next(OscX2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float val1, val2;
	
	float **tables = unit->mTables;
	int32 phase = unit->m_phase;
	int32 lomask = unit->m_lomask;
	
	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	int32 counter = unit->m_counter;
	float xfade = unit->m_xfade;
	
	int remain = inNumSamples;
	while (remain) {		
		if (counter <= 0) {
			/*if (unit->m_newTableFunc) {
				(*unit->m_newTableFunc)(this);
				counter = unit->m_counter;
			} else {
				counter = remain;
			}*/
			xfade = 0.;
		}
		float xfadeslope = unit->m_xfadeslope;
		int nsmps = sc_min(remain, counter);
		LOOP(nsmps,
			float pfrac = PhaseFrac1(phase);
			int32 index = (phase >> xlobits1) & lomask;
				val1 = *(float*)((char*)tables[0] + index);
				val2 = *(float*)((char*)tables[1] + index);
			float out0 = val1 + val2 * pfrac;
				val1 = *(float*)((char*)tables[2] + index);
				val2 = *(float*)((char*)tables[3] + index);
			float out1 = val1 + val2 * pfrac;
			
			ZXP(out) = lininterp(xfade, out0, out1);		
			
			phase += freq;
			xfade += xfadeslope;
		);
		counter -= nsmps;
		remain -= nsmps;
	}
	unit->m_counter = counter;
	unit->m_phase = phase;
	unit->m_xfade = xfade;
	
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////


void Formant_Ctor(Formant *unit)
{
	SETCALC(Formant_next);
	unit->m_cpstoinc = ft->mSineSize * SAMPLEDUR * 65536.;
	unit->m_phase1 = 0;
	unit->m_phase2 = 0;
	unit->m_phase3 = 0;
	Formant_next(unit, 1);
}

#define tqcyc13 0x18000000

void Formant_next(Formant *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freq1in = ZIN0(0);
	float freq2in = ZIN0(1);
	float freq3in = ZIN0(2);

	int32 phase1 = unit->m_phase1;
	int32 phase2 = unit->m_phase2;
	int32 phase3 = unit->m_phase3;
	float cpstoinc = unit->m_cpstoinc;
	int32 freq1 = (int32)(cpstoinc * freq1in);
	int32 freq2 = (int32)(cpstoinc * freq2in);
	int32 freq3 = (int32)(cpstoinc * freq3in);
	float* sine = ft->mSine;
	int32 formfreq = sc_max(freq1, freq3);
	LOOP(inNumSamples,
		if (phase3 < onecyc13) {
			ZXP(out) = (*(float*)((char*)sine + (((phase3 + tqcyc13) >> xlobits) & xlomask13)) + 1.f)
			         *  *(float*)((char*)sine + ((phase2 >> xlobits) & xlomask13));
			phase3 += formfreq;
		} else {
			ZXP(out) = 0.f;
		}
		phase1 += freq1;
		phase2 += freq2;
		if (phase1 > onecyc13) {
			phase1 -= onecyc13;
			phase2 = phase1 * freq2 / freq1;
			phase3 = phase1 * freq3 / freq1;
		}
	);
	
	unit->m_phase1 = phase1;
	unit->m_phase2 = phase2;
	unit->m_phase3 = phase3;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

inline float lookup13(float* table, int32 pphase)
{
	float pfrac = PhaseFrac(pphase);
	float* tbl = (float*)((char*)table + ((pphase >> xlobits) & xlomask13));
	return lininterp(pfrac, tbl[0], tbl[1]);
}

void Blip_Ctor(Blip *unit)
{
	SETCALC(Blip_next);
	unit->m_freqin = ZIN0(0);
	unit->m_numharm = (int32)ZIN0(1);

	unit->m_cpstoinc = ft->mSineSize * SAMPLEDUR * 65536. * 0.5;
	int32 N = unit->m_numharm;
	int32 maxN = (int32)((SAMPLERATE * 0.5) / unit->m_freqin);
	if (N > maxN) N = maxN;
	unit->m_N = N;
	unit->m_scale = 0.5/N;
	unit->m_phase = 0;

	Blip_next(unit, 1);
}

void Blip_next(Blip *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	int numharm = (int32)ZIN0(1);
	
	int32 phase = unit->m_phase;
		
	float* numtbl = ft->mSine;
	float* dentbl = ft->mSecant;
	
	int32 freq, N, prevN;
	float scale, prevscale;
	bool crossfade;
	if (numharm != unit->m_numharm || freqin != unit->m_freqin) {
		N = numharm;
		int32 maxN = (int32)((SAMPLERATE * 0.5) / freqin);
		if (N > maxN) {
			float maxfreqin;
			N = maxN;
			maxfreqin = sc_max(unit->m_freqin, freqin);
			freq = (int32)(unit->m_cpstoinc * maxfreqin);
		} else {
			freq = (int32)(unit->m_cpstoinc * freqin);
		}
		crossfade = N != unit->m_N;
		prevN = unit->m_N;
		prevscale = unit->m_scale;
		unit->m_N = N;
		unit->m_scale = scale = 0.5/N;
	} else {
		N = unit->m_N;
		freq = (int32)(unit->m_cpstoinc * freqin);
		scale = unit->m_scale;
		crossfade = false;
	}
	int32 N2 = 2*N+1;
	
	if (crossfade) {
		int32 prevN2 = 2 * prevN + 1;
		float xfade_slope = unit->mRate->mSlopeFactor;
		float xfade = 0.f;
		LOOP(inNumSamples,
			float* tbl = (float*)((char*)dentbl + ((phase >> xlobits) & xlomask13));
			float t0 = tbl[0]; 
			float t1 = tbl[1];
			if (t0 == kBadValue || t1 == kBadValue) {
				tbl = (float*)((char*)numtbl + ((phase >> xlobits) & xlomask13));
				t0 = tbl[0]; 
				t1 = tbl[1];
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				if (fabs(denom) < 0.0005) {
					ZXP(out) = 1.f;
				} else {
					int32 rphase = phase * prevN2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					float numer = lininterp(pfrac, tbl[0], tbl[1]);
					float n1 = (numer / denom - 1.f) * prevscale;
					
					rphase = phase * N2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					numer = lininterp(pfrac, tbl[0], tbl[1]);
					float n2 = (numer / denom - 1.f) * scale;
					
					ZXP(out) = lininterp(xfade, n1, n2);
				}
			} else {
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				
				int32 rphase = phase * prevN2;
				pfrac = PhaseFrac(rphase);
				float* tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				float numer = lininterp(pfrac, tbl[0], tbl[1]);
				float n1 = (numer * denom - 1.f) * prevscale;
				
				rphase = phase * N2;
				pfrac = PhaseFrac(rphase);
				tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				numer = lininterp(pfrac, tbl[0], tbl[1]);
				float n2 = (numer * denom - 1.f) * scale;
				
				ZXP(out) = lininterp(xfade, n1, n2);
			}
			phase += freq;
			xfade += xfade_slope;
		);
	} else {
		// hmm, if freq is above sr/4 then revert to sine table osc w/ no interpolation ?
		// why bother, it isn't a common choice for a fundamental.
		LOOP(inNumSamples,
			float* tbl = (float*)((char*)dentbl + ((phase >> xlobits) & xlomask13));
			float t0 = tbl[0]; 
			float t1 = tbl[1];
			if (t0 == kBadValue || t1 == kBadValue) {
				tbl = (float*)((char*)numtbl + ((phase >> xlobits) & xlomask13));
				t0 = tbl[0]; 
				t1 = tbl[1];
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				if (fabs(denom) < 0.0005) {
					ZXP(out) = 1.f;
				} else {
					int32 rphase = phase * N2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					float numer = lininterp(pfrac, tbl[0], tbl[1]);
					ZXP(out) = (numer / denom - 1.f) * scale;
				}
			} else {
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				int32 rphase = phase * N2;
				pfrac = PhaseFrac(rphase);
				tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				float numer = lininterp(pfrac, tbl[0], tbl[1]);
				ZXP(out) = (numer * denom - 1.f) * scale;
			}
			phase += freq;
		);
	}
	
	unit->m_phase = phase;
	unit->m_freqin = freqin;
	unit->m_numharm = numharm;
	
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


void Saw_Ctor(Saw *unit)
{
	SETCALC(Saw_next);
	unit->m_freqin = ZIN0(0);

	unit->m_cpstoinc = ft->mSineSize * SAMPLEDUR * 65536. * 0.5;
	unit->m_N = (int32)((SAMPLERATE * 0.5) / unit->m_freqin);
	unit->m_scale = 0.5/unit->m_N;
	unit->m_phase = 0;
	unit->m_y1 = 1.f;

	ZOUT0(0) = 0.f;
}

void Saw_next(Saw *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	
	int32 phase = unit->m_phase;
	float y1 = unit->m_y1;
	
	float* numtbl = ft->mSine;
	float* dentbl = ft->mSecant;
	
	int32 freq, N, prevN;
	float scale, prevscale;
	bool crossfade;
	if (freqin != unit->m_freqin) {
		N = (int32)((SAMPLERATE * 0.5) / freqin);
		if (N != unit->m_N) {
			float maxfreqin;
			maxfreqin = sc_max(unit->m_freqin, freqin);
			freq = (int32)(unit->m_cpstoinc * maxfreqin);
			crossfade = true;
		} else {
			freq = (int32)(unit->m_cpstoinc * freqin);
			crossfade = false;
		}
		prevN = unit->m_N;
		prevscale = unit->m_scale;
		unit->m_N = N;
		unit->m_scale = scale = 0.5/N;
	} else {
		N = unit->m_N;
		freq = (int32)(unit->m_cpstoinc * freqin);
		scale = unit->m_scale;
		crossfade = false;
	}
	int32 N2 = 2*N+1;
	
	if (crossfade) {
		int32 prevN2 = 2 * prevN + 1;
		float xfade_slope = unit->mRate->mSlopeFactor;
		float xfade = 0.f;
		LOOP(inNumSamples,
			float* tbl = (float*)((char*)dentbl + ((phase >> xlobits) & xlomask13));
			float t0 = tbl[0]; 
			float t1 = tbl[1];
			if (t0 == kBadValue || t1 == kBadValue) {
				tbl = (float*)((char*)numtbl + ((phase >> xlobits) & xlomask13));
				t0 = tbl[0]; 
				t1 = tbl[1];
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				if (fabs(denom) < 0.0005) {
					ZXP(out) = y1 = 1.f + 0.999f * y1;
				} else {
					int32 rphase = phase * prevN2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					float numer = lininterp(pfrac, tbl[0], tbl[1]);
					float n1 = (numer / denom - 1.f) * prevscale;
					
					rphase = phase * N2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					numer = lininterp(pfrac, tbl[0], tbl[1]);
					float n2 = (numer / denom - 1.f) * scale;
					
					ZXP(out) = y1 = n1 + xfade * (n2 - n1) + 0.999f * y1;
				}

			} else {
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				
				int32 rphase = phase * prevN2;
				pfrac = PhaseFrac(rphase);
				tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				float numer = lininterp(pfrac, tbl[0], tbl[1]);
				float n1 = (numer * denom - 1.f) * prevscale;
				
				rphase = phase * N2;
				pfrac = PhaseFrac(rphase);
				tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				numer = lininterp(pfrac, tbl[0], tbl[1]);
				float n2 = (numer * denom - 1.f) * scale;
				
				ZXP(out) = y1 = n1 + xfade * (n2 - n1) + 0.999f * y1;
			}
			phase += freq;
			xfade += xfade_slope;
		);
	} else {
		// hmm, if freq is above sr/4 then revert to sine table osc ?
		// why bother, it isn't a common choice for a fundamental.
		LOOP(inNumSamples,
			float* tbl = (float*)((char*)dentbl + ((phase >> xlobits) & xlomask13));
			float t0 = tbl[0]; 
			float t1 = tbl[1];
			if (t0 == kBadValue || t1 == kBadValue) {
				tbl = (float*)((char*)numtbl + ((phase >> xlobits) & xlomask13));
				t0 = tbl[0]; 
				t1 = tbl[1];
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				if (fabs(denom) < 0.0005) {
					ZXP(out) = y1 = 1.f + 0.999f * y1;
				} else {
					int32 rphase = phase * N2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					float numer = lininterp(pfrac, tbl[0], tbl[1]);
					ZXP(out) = y1 = (numer / denom - 1.f) * scale + 0.999f * y1;
				}
			} else {
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				int32 rphase = phase * N2;
				pfrac = PhaseFrac(rphase);
				float* tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				float numer = lininterp(pfrac, tbl[0], tbl[1]);
				ZXP(out) = y1 = (numer * denom - 1.f) * scale + 0.999f * y1;
			}
			phase += freq;
		);
	}
	
	unit->m_y1 = y1;
	unit->m_phase = phase;
	unit->m_freqin = freqin;
	
}
	

////////////////////////////////////////////////////////////////////////////////////////////////////////


void Pulse_Ctor(Pulse *unit)
{
	SETCALC(Pulse_next);
	unit->m_freqin = ZIN0(0);

	unit->m_cpstoinc = ft->mSineSize * SAMPLEDUR * 65536. * 0.5;
	unit->m_N = (int32)((SAMPLERATE * 0.5) / unit->m_freqin);
	unit->m_scale = 0.5/unit->m_N;
	unit->m_phase = 0;
	unit->m_y1 = 1.f;
	ZOUT0(0) = 0.f;
}

void Pulse_next(Pulse *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float freqin = ZIN0(0);
	float duty = ZIN0(1);
	
	int32 phase = unit->m_phase;
	float y1 = unit->m_y1;
	
	float* numtbl = ft->mSine;
	float* dentbl = ft->mSecant;
	
	int32 freq, N, prevN;
	float scale, prevscale;
	bool crossfade;
	
	if (freqin != unit->m_freqin) {
		N = (int32)((SAMPLERATE * 0.5) / freqin);
		if (N != unit->m_N) {
			float maxfreqin;
			maxfreqin = sc_max(unit->m_freqin, freqin);
			freq = (int32)(unit->m_cpstoinc * maxfreqin);
			crossfade = true;
		} else {
			freq = (int32)(unit->m_cpstoinc * freqin);
			crossfade = false;
		}
		prevN = unit->m_N;
		prevscale = unit->m_scale;
		unit->m_N = N;
		unit->m_scale = scale = 0.5/N;
	} else {
		N = unit->m_N;
		freq = (int32)(unit->m_cpstoinc * freqin);
		scale = unit->m_scale;
		crossfade = false;
	}
	int32 N2 = 2*N+1;

	int32 phaseoff = unit->m_phaseoff;
	int32 next_phaseoff = (int32)(duty * (1L << 28));
	int32 phaseoff_slope = (int32)((next_phaseoff - phaseoff) * unit->mRate->mSlopeFactor);
	unit->m_phaseoff = next_phaseoff;
	float rscale = 1.f / scale + 1.f;
	float pul1, pul2;
	
	if (crossfade) {
		int32 prevN2 = 2 * prevN + 1;
		float xfade_slope = unit->mRate->mSlopeFactor;
		float xfade = 0.f;
		LOOP(inNumSamples,
			float* tbl = (float*)((char*)dentbl + ((phase >> xlobits) & xlomask13));
			float t0 = tbl[0]; 
			float t1 = tbl[1];
			if (t0 == kBadValue || t1 == kBadValue) {
				tbl = (float*)((char*)numtbl + ((phase >> xlobits) & xlomask13));
				t0 = tbl[0]; 
				t1 = tbl[1];
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				if (fabs(denom) < 0.0005) {
					pul1 = 1.f;
				} else {
					int32 rphase = phase * prevN2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					float numer = lininterp(pfrac, tbl[0], tbl[1]);
					float n1 = (numer / denom - 1.f) * prevscale;
					
					rphase = phase * N2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					numer = lininterp(pfrac, tbl[0], tbl[1]);
					float n2 = (numer / denom - 1.f) * scale;
					
					pul1 = lininterp(xfade, n1, n2);
				}
			} else {
				float pfrac = PhaseFrac(phase);
				float denom = lininterp(pfrac, t0, t1);
				
				int32 rphase = phase * prevN2;
				pfrac = PhaseFrac(rphase);
				tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				float numer = lininterp(pfrac, tbl[0], tbl[1]);
				float n1 = (numer * denom - 1.f) * prevscale;
				
				rphase = phase * N2;
				pfrac = PhaseFrac(rphase);
				tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				numer = lininterp(pfrac, tbl[0], tbl[1]);
				float n2 = (numer * denom - 1.f) * scale;
				
				pul1 = lininterp(xfade, n1, n2);
			}

			int32 phase2 = phase + phaseoff;
			tbl = (float*)((char*)dentbl + ((phase2 >> xlobits) & xlomask13));
			t0 = tbl[0]; 
			t1 = tbl[1];
			if (t0 == kBadValue || t1 == kBadValue) {
				tbl = (float*)((char*)numtbl + ((phase2 >> xlobits) & xlomask13));
				t0 = tbl[0]; 
				t1 = tbl[1];
				float pfrac = PhaseFrac(phase2);
				float denom = t0 + (t1 - t0) * pfrac;
				if (fabs(denom) < 0.0005) {
					pul2 = 1.f;
				} else {
					int32 rphase = phase2 * prevN2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					float numer = lininterp(pfrac, tbl[0], tbl[1]);
					float n1 = (numer / denom - 1.f) * prevscale;
					
					rphase = phase2 * N2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					numer = lininterp(pfrac, tbl[0], tbl[1]);
					float n2 = (numer / denom - 1.f) * scale;
					
					pul2 = lininterp(xfade, n1, n2);
				}
			} else {
				float pfrac = PhaseFrac(phase2);
				float denom = t0 + (t1 - t0) * pfrac;
				
				int32 rphase = phase2 * prevN2;
				pfrac = PhaseFrac(rphase);
				tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				float numer = lininterp(pfrac, tbl[0], tbl[1]);
				float n1 = (numer * denom - 1.f) * prevscale;
				
				rphase = phase2 * N2;
				pfrac = PhaseFrac(rphase);
				tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				numer = lininterp(pfrac, tbl[0], tbl[1]);
				float n2 = (numer * denom - 1.f) * scale;
				
				pul2 = lininterp(xfade, n1, n2);
			}

			ZXP(out) = y1 = pul1 - pul2 + 0.999f * y1;
			phase += freq;
			phaseoff += phaseoff_slope;
			xfade += xfade_slope;
		);
	} else {
		LOOP(inNumSamples,
			float* tbl = (float*)((char*)dentbl + ((phase >> xlobits) & xlomask13));
			float t0 = tbl[0]; 
			float t1 = tbl[1];
			if (t0 == kBadValue || t1 == kBadValue) {
				tbl = (float*)((char*)numtbl + ((phase >> xlobits) & xlomask13));
				t0 = tbl[0]; 
				t1 = tbl[1];
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				if (fabs(denom) < 0.0005) {
					pul1 = rscale;
				} else {
					int32 rphase = phase * N2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					float numer = lininterp(pfrac, tbl[0], tbl[1]);
					pul1 = numer / denom;
				}
			} else {
				float pfrac = PhaseFrac(phase);
				float denom = t0 + (t1 - t0) * pfrac;
				int32 rphase = phase * N2;
				pfrac = PhaseFrac(rphase);
				float* tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				float numer = lininterp(pfrac, tbl[0], tbl[1]);
				pul1 = (numer * denom);
			}

			int32 phase2 = phase + phaseoff;
			tbl = (float*)((char*)dentbl + ((phase2 >> xlobits) & xlomask13));
			t0 = tbl[0]; 
			t1 = tbl[1];
			if (t0 == kBadValue || t1 == kBadValue) {
				tbl = (float*)((char*)numtbl + ((phase2 >> xlobits) & xlomask13));
				t0 = tbl[0]; 
				t1 = tbl[1];
				float pfrac = PhaseFrac(phase2);
				float denom = t0 + (t1 - t0) * pfrac;
				if (fabs(denom) < 0.0005) {
					pul2 = rscale;
				} else {
					int32 rphase = phase2 * N2;
					pfrac = PhaseFrac(rphase);
					tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
					float numer = lininterp(pfrac, tbl[0], tbl[1]);
					pul2 = numer / denom;
				}
			} else {
				float pfrac = PhaseFrac(phase2);
				float denom = t0 + (t1 - t0) * pfrac;
				int32 rphase = phase2 * N2;
				pfrac = PhaseFrac(rphase);
				float* tbl = (float*)((char*)numtbl + ((rphase >> xlobits) & xlomask13));
				float numer = lininterp(pfrac, tbl[0], tbl[1]);
				
				pul2 = (numer * denom);
			}
			
			ZXP(out) = y1 = (pul1 - pul2) * scale + 0.999f * y1;
			phase += freq;
			phaseoff += phaseoff_slope;
		);
	}
	
	unit->m_y1 = y1;
	unit->m_phase = phase;
	unit->m_freqin = freqin;
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Klang_Dtor(Klang *unit)
{
	RTFree(unit->mWorld, unit->m_coefs);
}

void Klang_Ctor(Klang *unit)
{
	SETCALC(Klang_next);
	ZOUT0(0) = Klang_SetCoefs(unit);
}

float Klang_SetCoefs(Klang *unit)
{
	unit->m_numpartials = (unit->mNumInputs - 2)/3;

	//RTFree(unit->mWorld, unit->m_coefs);
	unit->m_coefs = (float*)RTAlloc(unit->mWorld, unit->m_numpartials * 3 * sizeof(float));

	float freqscale = ZIN0(0) * unit->mRate->mRadiansPerSample;
	float freqoffset = ZIN0(1) * unit->mRate->mRadiansPerSample;
	
	float outf = 0.;
	float* coefs = unit->m_coefs - 1;
		
	for (int i=0,j=2; i<unit->m_numpartials; ++i,j+=3) {
		float w = ZIN0(j) * freqscale + freqoffset;
		float level = ZIN0(j+1);
		float phase = ZIN0(j+2);

		if (phase != 0.f) {
			outf += *++coefs = level * sin(phase);		// y1
			        *++coefs = level * sin(phase - w);	// y2
		} else {
			outf += *++coefs = 0.f;				// y1
			        *++coefs = level * -sin(w);	// y2
		}
		*++coefs = 2. * cos(w);		// b1
	}
	return outf;
}

void Klang_next(Klang *unit, int inNumSamples)
{
	float *out0 = ZOUT(0);
	
	float* out;
	float y0_0, y1_0, y2_0, b1_0;
	float y0_1, y1_1, y2_1, b1_1;
	float y0_2, y1_2, y2_2, b1_2;
	float y0_3, y1_3, y2_3, b1_3;
	float outf;
		
	float* coefs = unit->m_coefs - 1;
	int32 numpartials = unit->m_numpartials;

	switch (numpartials & 3) {
		case 3 :
			y1_0 = *++coefs;	y2_0 = *++coefs;	b1_0 = *++coefs;
			y1_1 = *++coefs;	y2_1 = *++coefs;	b1_1 = *++coefs;
			y1_2 = *++coefs;	y2_2 = *++coefs;	b1_2 = *++coefs;

			out = out0;
			LOOP(unit->mRate->mFilterLoops, 
				outf  = y0_0 = b1_0 * y1_0 - y2_0; 
				outf += y0_1 = b1_1 * y1_1 - y2_1; 
				outf += y0_2 = b1_2 * y1_2 - y2_2; 
				ZXP(out) = outf;
				
				outf  = y2_0 = b1_0 * y0_0 - y1_0; 
				outf += y2_1 = b1_1 * y0_1 - y1_1; 
				outf += y2_2 = b1_2 * y0_2 - y1_2; 
				ZXP(out) = outf;
				
				outf  = y1_0 = b1_0 * y2_0 - y0_0; 
				outf += y1_1 = b1_1 * y2_1 - y0_1; 
				outf += y1_2 = b1_2 * y2_2 - y0_2; 
				ZXP(out) = outf;
			);
			LOOP(unit->mRate->mFilterRemain, 
				outf  = y0_0 = b1_0 * y1_0 - y2_0; 
				outf += y0_1 = b1_1 * y1_1 - y2_1; 
				outf += y0_2 = b1_2 * y1_2 - y2_2; 
				y2_0 = y1_0;	y1_0 = y0_0; 
				y2_1 = y1_1;	y1_1 = y0_1; 
				y2_2 = y1_2;	y1_2 = y0_2; 
				ZXP(out) = outf;
			);
			coefs -= 9;
			*++coefs = y1_0;	*++coefs = y2_0;	++coefs;	
			*++coefs = y1_1;	*++coefs = y2_1;	++coefs;	
			*++coefs = y1_2;	*++coefs = y2_2;	++coefs;	
			break;
		case 2 :
			y1_0 = *++coefs;	y2_0 = *++coefs;	b1_0 = *++coefs;
			y1_1 = *++coefs;	y2_1 = *++coefs;	b1_1 = *++coefs;

			out = out0;
			LOOP(unit->mRate->mFilterLoops, 
				outf  = y0_0 = b1_0 * y1_0 - y2_0; 
				outf += y0_1 = b1_1 * y1_1 - y2_1; 
				ZXP(out) = outf;
				
				outf  = y2_0 = b1_0 * y0_0 - y1_0; 
				outf += y2_1 = b1_1 * y0_1 - y1_1; 
				ZXP(out) = outf;
				
				outf  = y1_0 = b1_0 * y2_0 - y0_0; 
				outf += y1_1 = b1_1 * y2_1 - y0_1; 
				ZXP(out) = outf;
			);
			LOOP(unit->mRate->mFilterRemain, 
				outf  = y0_0 = b1_0 * y1_0 - y2_0; 
				outf += y0_1 = b1_1 * y1_1 - y2_1; 
				y2_0 = y1_0;	y1_0 = y0_0; 
				y2_1 = y1_1;	y1_1 = y0_1; 
				ZXP(out) = outf;
			);

			coefs -= 6;
			*++coefs = y1_0;	*++coefs = y2_0;	++coefs;	
			*++coefs = y1_1;	*++coefs = y2_1;	++coefs;	
			break;
		case 1 :
			y1_0 = *++coefs;	y2_0 = *++coefs;	b1_0 = *++coefs;

			out = out0;
			LOOP(unit->mRate->mFilterLoops, 
				ZXP(out) = y0_0 = b1_0 * y1_0 - y2_0; 
				
				ZXP(out) = y2_0 = b1_0 * y0_0 - y1_0; 
				
				ZXP(out) = y1_0 = b1_0 * y2_0 - y0_0; 
			);
			LOOP(unit->mRate->mFilterRemain, 
				ZXP(out) = y0_0 = b1_0 * y1_0 - y2_0; 
				y2_0 = y1_0;	y1_0 = y0_0; 
			);

			coefs -= 3;
			*++coefs = y1_0;	*++coefs = y2_0;	++coefs;	
			break;
		case 0 :
			out = out0;
			Clear(inNumSamples, out);
			break;
	}
	
	int32 imax = numpartials >> 2;
		
	for (int i=0; i<imax; ++i) {
		y1_0 = *++coefs;	y2_0 = *++coefs;	b1_0 = *++coefs;
		y1_1 = *++coefs;	y2_1 = *++coefs;	b1_1 = *++coefs;
		y1_2 = *++coefs;	y2_2 = *++coefs;	b1_2 = *++coefs;
		y1_3 = *++coefs;	y2_3 = *++coefs;	b1_3 = *++coefs;
		
		out = out0;
		LOOP(unit->mRate->mFilterLoops, 
			outf  = y0_0 = b1_0 * y1_0 - y2_0; 
			outf += y0_1 = b1_1 * y1_1 - y2_1; 
			outf += y0_2 = b1_2 * y1_2 - y2_2; 
			outf += y0_3 = b1_3 * y1_3 - y2_3; 
			ZXP(out) += outf;
			
			outf  = y2_0 = b1_0 * y0_0 - y1_0; 
			outf += y2_1 = b1_1 * y0_1 - y1_1; 
			outf += y2_2 = b1_2 * y0_2 - y1_2; 
			outf += y2_3 = b1_3 * y0_3 - y1_3; 
			ZXP(out) += outf;
			
			outf  = y1_0 = b1_0 * y2_0 - y0_0; 
			outf += y1_1 = b1_1 * y2_1 - y0_1; 
			outf += y1_2 = b1_2 * y2_2 - y0_2; 
			outf += y1_3 = b1_3 * y2_3 - y0_3; 
			ZXP(out) += outf;
		);
		LOOP(unit->mRate->mFilterRemain, 
			outf  = y0_0 = b1_0 * y1_0 - y2_0; 
			outf += y0_1 = b1_1 * y1_1 - y2_1; 
			outf += y0_2 = b1_2 * y1_2 - y2_2; 
			outf += y0_3 = b1_3 * y1_3 - y2_3; 
			y2_0 = y1_0;	y1_0 = y0_0; 
			y2_1 = y1_1;	y1_1 = y0_1; 
			y2_2 = y1_2;	y1_2 = y0_2; 
			y2_3 = y1_3;	y1_3 = y0_3; 
			ZXP(out) += outf;
		);
		coefs -= 12;
		*++coefs = y1_0;	*++coefs = y2_0;	++coefs;	
		*++coefs = y1_1;	*++coefs = y2_1;	++coefs;	
		*++coefs = y1_2;	*++coefs = y2_2;	++coefs;	
		*++coefs = y1_3;	*++coefs = y2_3;	++coefs;	
	}
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////




void Klank_Dtor(Klank *unit)
{
	RTFree(unit->mWorld, unit->m_coefs);
}

void Klank_Ctor(Klank *unit)
{
	SETCALC(Klank_next);
	unit->m_x1 = unit->m_x2 = 0.f;
	Klank_SetCoefs(unit);
	ZOUT0(0) = 0.f;
}

void Klank_SetCoefs(Klank *unit)
{
	unit->m_numpartials = (unit->mNumInputs - 4) / 3;
	
	//RTFree(unit->mWorld, unit->m_coefs);
	unit->m_coefs = (float*)RTAlloc(unit->mWorld, unit->m_numpartials * 5 * sizeof(float));

	float freqscale = ZIN0(1) * unit->mRate->mRadiansPerSample;
	float freqoffset = ZIN0(2) * unit->mRate->mRadiansPerSample;
	float decayscale = ZIN0(3);

	float* coefs = unit->m_coefs - 1;

	float sampleRate = SAMPLERATE;
		
	for (int i=0,j=4; i<unit->m_numpartials; ++i,j+=3) {
		float w = ZIN0(j) * freqscale + freqoffset;
		float level = ZIN0(j+1);
		float time = ZIN0(j+2) * decayscale;
				
		float R = time == 0.f ? 0.f : exp(log001/(time * sampleRate));
		float twoR = 2.f * R;
		float R2 = R * R;
		float cost = (twoR * cos(w)) / (1.f + R2);
		
		*++coefs = 0.f;					// y1
		*++coefs = 0.f;					// y2
		*++coefs = twoR * cost;			// b1
		*++coefs = -R2;					// b2
		*++coefs = level * 0.25;		// a0		
		//postbuf("coefs %d  %g %g %g\n", i, twoR * cost, -R2, ampf * 0.25);
	}
}


void Klank_next(Klank *unit, int inNumSamples)
{
	float *out0 = ZOUT(0);
	float *in0 = ZIN(0);
	
	float *in, *out;
	float inf;
	float y0_0, y1_0, y2_0, a0_0, b1_0, b2_0;
	float y0_1, y1_1, y2_1, a0_1, b1_1, b2_1;
	float y0_2, y1_2, y2_2, a0_2, b1_2, b2_2;
	float y0_3, y1_3, y2_3, a0_3, b1_3, b2_3;
		
	int32 numpartials = unit->m_numpartials;
	float* coefs = unit->m_coefs - 1;

	switch (numpartials & 3) {
		case 3 :
			y1_0 = *++coefs;	y2_0 = *++coefs;	b1_0 = *++coefs;	b2_0 = *++coefs;	a0_0 = *++coefs;
			y1_1 = *++coefs;	y2_1 = *++coefs;	b1_1 = *++coefs;	b2_1 = *++coefs;	a0_1 = *++coefs;
			y1_2 = *++coefs;	y2_2 = *++coefs;	b1_2 = *++coefs;	b2_2 = *++coefs;	a0_2 = *++coefs;
			
			in = in0;
			out = out0;
			LOOP(unit->mRate->mFilterLoops, 
				inf = *++in;
				y0_0 = inf + b1_0 * y1_0 + b2_0 * y2_0; 
				y0_1 = inf + b1_1 * y1_1 + b2_1 * y2_1; 
				y0_2 = inf + b1_2 * y1_2 + b2_2 * y2_2; 
				*++out = a0_0 * y0_0 + a0_1 * y0_1 + a0_2 * y0_2;
				
				inf = *++in;
				y2_0 = inf + b1_0 * y0_0 + b2_0 * y1_0; 
				y2_1 = inf + b1_1 * y0_1 + b2_1 * y1_1; 
				y2_2 = inf + b1_2 * y0_2 + b2_2 * y1_2; 
				*++out = a0_0 * y2_0 + a0_1 * y2_1 + a0_2 * y2_2;
				
				inf = *++in;
				y1_0 = inf + b1_0 * y2_0 + b2_0 * y0_0; 
				y1_1 = inf + b1_1 * y2_1 + b2_1 * y0_1; 
				y1_2 = inf + b1_2 * y2_2 + b2_2 * y0_2; 
				*++out = a0_0 * y1_0 + a0_1 * y1_1 + a0_2 * y1_2;
			);
			LOOP(unit->mRate->mFilterRemain, 
				inf = *++in;
				y0_0 = inf + b1_0 * y1_0 + b2_0 * y2_0; 
				y0_1 = inf + b1_1 * y1_1 + b2_1 * y2_1; 
				y0_2 = inf + b1_2 * y1_2 + b2_2 * y2_2; 
				*++out = a0_0 * y0_0 + a0_1 * y0_1 + a0_2 * y0_2;
				y2_0 = y1_0;	y1_0 = y0_0; 
				y2_1 = y1_1;	y1_1 = y0_1; 
				y2_2 = y1_2;	y1_2 = y0_2; 
			);
			coefs -= 15;
			*++coefs = y1_0;	*++coefs = y2_0;	coefs += 3;	
			*++coefs = y1_1;	*++coefs = y2_1;	coefs += 3;	
			*++coefs = y1_2;	*++coefs = y2_2;	coefs += 3;	
			break;
		case 2 :
			y1_0 = *++coefs;	y2_0 = *++coefs;	b1_0 = *++coefs;	b2_0 = *++coefs;	a0_0 = *++coefs;
			y1_1 = *++coefs;	y2_1 = *++coefs;	b1_1 = *++coefs;	b2_1 = *++coefs;	a0_1 = *++coefs;
			
			in = in0;
			out = out0;
			LOOP(unit->mRate->mFilterLoops, 
				inf = *++in;
				y0_0 = inf + b1_0 * y1_0 + b2_0 * y2_0; 
				y0_1 = inf + b1_1 * y1_1 + b2_1 * y2_1; 
				*++out = a0_0 * y0_0 + a0_1 * y0_1;
				
				inf = *++in;
				y2_0 = inf + b1_0 * y0_0 + b2_0 * y1_0; 
				y2_1 = inf + b1_1 * y0_1 + b2_1 * y1_1; 
				*++out = a0_0 * y2_0 + a0_1 * y2_1;
				
				inf = *++in;
				y1_0 = inf + b1_0 * y2_0 + b2_0 * y0_0; 
				y1_1 = inf + b1_1 * y2_1 + b2_1 * y0_1; 
				*++out = a0_0 * y1_0 + a0_1 * y1_1;
			);
			LOOP(unit->mRate->mFilterRemain, 
				inf = *++in;
				y0_0 = inf + b1_0 * y1_0 + b2_0 * y2_0; 
				y0_1 = inf + b1_1 * y1_1 + b2_1 * y2_1; 
				*++out = a0_0 * y0_0 + a0_1 * y0_1;
				y2_0 = y1_0;	y1_0 = y0_0; 
				y2_1 = y1_1;	y1_1 = y0_1; 
			);
			coefs -= 10;
			*++coefs = y1_0;	*++coefs = y2_0;	coefs += 3;	
			*++coefs = y1_1;	*++coefs = y2_1;	coefs += 3;	
			break;
		case 1 :
			y1_0 = *++coefs;	y2_0 = *++coefs;	b1_0 = *++coefs;	b2_0 = *++coefs;	a0_0 = *++coefs;
			
			//postbuf("rcoefs %g %g %g %g %g\n", y1_0, y2_0, b1_0, b2_0, a0_0);
			in = in0;
			out = out0;
			LOOP(unit->mRate->mFilterLoops, 
				inf = *++in;
				y0_0 = inf + b1_0 * y1_0 + b2_0 * y2_0; 
				*++out = a0_0 * y0_0;
				
				inf = *++in;
				y2_0 = inf + b1_0 * y0_0 + b2_0 * y1_0; 
				*++out = a0_0 * y2_0;
				
				inf = *++in;
				y1_0 = inf + b1_0 * y2_0 + b2_0 * y0_0; 
				*++out = a0_0 * y1_0;
				//postbuf("out %g %g %g\n", y0_0, y2_0, y1_0);
			);
			LOOP(unit->mRate->mFilterRemain, 
				inf = *++in;
				y0_0 = inf + b1_0 * y1_0 + b2_0 * y2_0; 
				*++out = a0_0 * y0_0;
				y2_0 = y1_0;	y1_0 = y0_0; 
				//postbuf("out %g\n", y0_0);
			);
			coefs -= 5;
			*++coefs = y1_0;	*++coefs = y2_0;	coefs += 3;	
			break;
		case 0 :
			out = out0;
			LOOP(inNumSamples, *++out = 0.f;);
			break;
	}

	int32 imax = numpartials >> 2;
	
	for (int i=0; i<imax; ++i) {
		y1_0 = *++coefs;	y2_0 = *++coefs;	b1_0 = *++coefs;	b2_0 = *++coefs;	a0_0 = *++coefs;
		y1_1 = *++coefs;	y2_1 = *++coefs;	b1_1 = *++coefs;	b2_1 = *++coefs;	a0_1 = *++coefs;
		y1_2 = *++coefs;	y2_2 = *++coefs;	b1_2 = *++coefs;	b2_2 = *++coefs;	a0_2 = *++coefs;
		y1_3 = *++coefs;	y2_3 = *++coefs;	b1_3 = *++coefs;	b2_3 = *++coefs;	a0_3 = *++coefs;
		
		in = in0;
		out = out0;
		LOOP(unit->mRate->mFilterLoops, 
			inf = *++in;
			y0_0 = inf + b1_0 * y1_0 + b2_0 * y2_0; 
			y0_1 = inf + b1_1 * y1_1 + b2_1 * y2_1; 
			y0_2 = inf + b1_2 * y1_2 + b2_2 * y2_2; 
			y0_3 = inf + b1_3 * y1_3 + b2_3 * y2_3; 
			*++out += a0_0 * y0_0 + a0_1 * y0_1 + a0_2 * y0_2 + a0_3 * y0_3;
			
			inf = *++in;
			y2_0 = inf + b1_0 * y0_0 + b2_0 * y1_0; 
			y2_1 = inf + b1_1 * y0_1 + b2_1 * y1_1; 
			y2_2 = inf + b1_2 * y0_2 + b2_2 * y1_2; 
			y2_3 = inf + b1_3 * y0_3 + b2_3 * y1_3; 
			*++out += a0_0 * y2_0 + a0_1 * y2_1 + a0_2 * y2_2 + a0_3 * y2_3;
			
			inf = *++in;
			y1_0 = inf + b1_0 * y2_0 + b2_0 * y0_0; 
			y1_1 = inf + b1_1 * y2_1 + b2_1 * y0_1; 
			y1_2 = inf + b1_2 * y2_2 + b2_2 * y0_2; 
			y1_3 = inf + b1_3 * y2_3 + b2_3 * y0_3; 
			*++out += a0_0 * y1_0 + a0_1 * y1_1 + a0_2 * y1_2 + a0_3 * y1_3;
		);
		LOOP(unit->mRate->mFilterRemain, 
			inf = *++in;
			y0_0 = inf + b1_0 * y1_0 + b2_0 * y2_0; 
			y0_1 = inf + b1_1 * y1_1 + b2_1 * y2_1; 
			y0_2 = inf + b1_2 * y1_2 + b2_2 * y2_2; 
			y0_3 = inf + b1_3 * y1_3 + b2_3 * y2_3; 
			*++out += a0_0 * y0_0 + a0_1 * y0_1 + a0_2 * y0_2 + a0_3 * y0_3;
			y2_0 = y1_0;	y1_0 = y0_0; 
			y2_1 = y1_1;	y1_1 = y0_1; 
			y2_2 = y1_2;	y1_2 = y0_2; 
			y2_3 = y1_3;	y1_3 = y0_3; 
		);
		coefs -= 20;
		*++coefs = y1_0;	*++coefs = y2_0;	coefs += 3;	
		*++coefs = y1_1;	*++coefs = y2_1;	coefs += 3;	
		*++coefs = y1_2;	*++coefs = y2_2;	coefs += 3;	
		*++coefs = y1_3;	*++coefs = y2_3;	coefs += 3;	
	}
	
	float x0;
	float x1 = unit->m_x1;
	float x2 = unit->m_x2;

	out = out0;
	LOOP(unit->mRate->mFilterLoops, 
		x0 = *++out;
		*out = x0 - x2;
		x2 = *++out;
		*out = x2 - x1;
		x1 = *++out;
		*out = x1 - x0;
	);
	LOOP(unit->mRate->mFilterRemain, 
		x0 = *++out;
		*out = x0 - x2;
		x2 = x1;	
		x1 = x0;
	);
	
	unit->m_x1 = x1;
	unit->m_x2 = x2;

}



////////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineSimpleUnit(DegreeToKey);
	DefineSimpleUnit(Select);
	DefineSimpleUnit(Index);
	DefineSimpleUnit(FoldIndex);
	DefineSimpleUnit(WrapIndex);
	DefineSimpleUnit(Shaper);
	DefineSimpleUnit(SigOsc);
	DefineSimpleUnit(FSinOsc);
	DefineSimpleUnit(SinOsc);
	DefineSimpleUnit(Osc);
	DefineSimpleUnit(OscN);
	DefineSimpleUnit(COsc);
	//DefineSimpleUnit(COsc2);
	//DefineSimpleUnit(OscX4);
	//DefineSimpleUnit(OscX2);
	DefineSimpleUnit(Formant);
	DefineSimpleUnit(Blip);
	DefineSimpleUnit(Saw);
	DefineSimpleUnit(Pulse);
	DefineDtorUnit(Klang);
	DefineDtorUnit(Klank);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
