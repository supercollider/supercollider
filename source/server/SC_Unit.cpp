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


#include "SC_Unit.h"
#include "SC_UnitSpec.h"
#include "SC_UnitDef.h"
#include "SC_World.h"
#include "SC_Wire.h"
#include "Unroll.h"
#include <stdio.h>
#include "SC_Prototypes.h"

void Unit_ChooseMulAddFunc(Unit* unit);

Unit* Unit_New(World *inWorld, UnitSpec *inUnitSpec, char*& memory)
{
	//printf("->Unit_New %08X\n", inUnitSpec);
	UnitDef *def = inUnitSpec->mUnitDef;
	//printf("def %s\n", def->mUnitDefName);
	//printf("def->mAllocSize %d\n", def->mAllocSize);
	
	Unit *unit = (Unit*)memory; 
	memory += def->mAllocSize;
	
	//printf("unit %08X\n", unit);
	unit->mWorld = inWorld;
	unit->mUnitDef = def;
	unit->mParent = 0;
	unit->mParentIndex = 0;
	unit->mNumInputs = inUnitSpec->mNumInputs;
	unit->mNumOutputs = inUnitSpec->mNumOutputs;
	//printf("->>World_Alloc\n");
	int numPorts = unit->mNumInputs + unit->mNumOutputs;

	unit->mInput = (Wire**)memory; 
	memory += numPorts * sizeof(Wire*);
	
	//printf("unit->mInput %08X\n", unit->mInput);
	unit->mOutput = unit->mInput + unit->mNumInputs;
	//printf("unit->mOutput %08X\n", unit->mOutput);

	unit->mInBuf = (float**)memory; 
	memory += numPorts * sizeof(float*);

	unit->mOutBuf = unit->mInBuf + unit->mNumInputs;
	

	unit->mCalcRate = inUnitSpec->mCalcRate;
	unit->mSpecialIndex = inUnitSpec->mSpecialIndex;
	
	unit->mRate = 0;
	unit->mDimension = 0;
	unit->mCalcFunc = 0;
	unit->mDone = false;
	
	//printf("<-Unit_New\n");
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

#if 0


// mul add functions for every occasion:
void ampmix_k(Unit *unit, int inNumSamples);

void ampmix_aa(Unit *unit, int inNumSamples);
void ampmix_ak(Unit *unit, int inNumSamples);
void ampmix_ai(Unit *unit, int inNumSamples);
void ampmix_a0(Unit *unit, int inNumSamples);

void ampmix_ka(Unit *unit, int inNumSamples);
void ampmix_kk(Unit *unit, int inNumSamples);
void ampmix_ki(Unit *unit, int inNumSamples);
void ampmix_k0(Unit *unit, int inNumSamples);

void ampmix_ia(Unit *unit, int inNumSamples);
void ampmix_ik(Unit *unit, int inNumSamples);
void ampmix_ii(Unit *unit, int inNumSamples);
void ampmix_i0(Unit *unit, int inNumSamples);

void ampmix_1a(Unit *unit, int inNumSamples);
void ampmix_1k(Unit *unit, int inNumSamples);
void ampmix_1i(Unit *unit, int inNumSamples);
void ampmix_10(Unit *unit, int inNumSamples);

void ampmix_0a(Unit *unit, int inNumSamples);
void ampmix_0k(Unit *unit, int inNumSamples);
void ampmix_0i(Unit *unit, int inNumSamples);
void ampmix_00(Unit *unit, int inNumSamples);


void ampmix_k(Unit *unit, int inNumSamples);
void ampmix_k(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = ZOUT0(0) * MULIN[0] + ADDIN[0];
}

void ampmix_aa(Unit *unit, int inNumSamples);
void ampmix_aa(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *amp = MULIN - ZOFF;
	float *mix = ADDIN - ZOFF;

	LOOP(inNumSamples, PZ(out); ZX(out) = ZXP(amp) * ZX(out) + ZXP(mix); ZP(out););
}

void ampmix_ak(Unit *unit, int inNumSamples);
void ampmix_ak(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *amp = MULIN - ZOFF;

	float mix_cur = unit->mPrevAdd;
	float mix_slope = CALCSLOPE(ADDIN[0], mix_cur);
	if (mix_slope == 0.f) {
		if (mix_cur == 0.f) {
			LOOP(inNumSamples, PZ(out); ZX(out) = ZXP(amp) * ZX(out); ZP(out););
		} else {
			LOOP(inNumSamples, PZ(out); ZX(out) = ZXP(amp) * ZX(out) + mix_cur; ZP(out););
		}
	} else {
		LOOP(inNumSamples, PZ(out); ZX(out) = ZXP(amp) * ZX(out) + mix_cur; mix_cur += mix_slope; ZP(out););
		unit->mPrevAdd = mix_cur;
	}
}

void ampmix_ai(Unit *unit, int inNumSamples);
void ampmix_ai(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *amp = MULIN - ZOFF;
	float mix_cur = unit->mPrevAdd;
	
	LOOP(inNumSamples, PZ(out); ZX(out) = ZXP(amp) * ZX(out) + mix_cur; ZP(out););
}

void ampmix_a0(Unit *unit, int inNumSamples);
void ampmix_a0(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *amp = MULIN - ZOFF;

	LOOP(inNumSamples, ZXP(out) *= ZXP(amp););
}

void ampmix_ka(Unit *unit, int inNumSamples);
void ampmix_ka(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *mix = ADDIN - ZOFF;
	float amp_cur = unit->mPrevMul;
	float amp_slope = CALCSLOPE(MULIN[0], amp_cur);
	
	if (amp_slope == 0.f) {
		if (amp_cur == 0.f) {
			ZCopy(inNumSamples, out, mix);
		} else if (amp_cur == 1.f) {
			ZAccum(inNumSamples, out, mix);
		} else {
			LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + ZXP(mix); ZP(out););
		}
	} else {
		LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + ZXP(mix); amp_cur += amp_slope; ZP(out););
		unit->mPrevMul = amp_cur;
	}
}

void ampmix_kk(Unit *unit, int inNumSamples);
void ampmix_kk(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	
	float amp_cur = unit->mPrevMul;
	float amp_slope = CALCSLOPE(MULIN[0], amp_cur);
	float mix_cur = unit->mPrevAdd;
	float mix_slope = CALCSLOPE(ADDIN[0], mix_cur);
	
	if (amp_slope == 0.f) {
		if (mix_slope == 0.f) {
			if (mix_cur == 0.f) {
				if (amp_cur == 1.f) {
					// do nothing!
				} else if (amp_cur == 0.f) {
					ZClear(inNumSamples, out);
				} else {
					LOOP(inNumSamples, ZXP(out) *= amp_cur;);
				}
			} else {
				if (amp_cur == 1.f) {
					LOOP(inNumSamples, ZXP(out) += mix_cur;);
				} else if (amp_cur == 0.f) {
					LOOP(inNumSamples, ZXP(out) = mix_cur;);
				} else {
					LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + mix_cur; ZP(out););
				}
			}
		} else {
			if (amp_cur == 1.f) {
				LOOP(inNumSamples, ZXP(out) += mix_cur; mix_cur += mix_slope;);
			} else if (amp_cur == 0.f) {
				LOOP(inNumSamples, ZXP(out) = mix_cur; mix_cur += mix_slope;);
			} else {
				LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + mix_cur; mix_cur += mix_slope; ZP(out););
			}
			unit->mPrevAdd = mix_cur;
		}
	} else {
		if (mix_slope == 0.f) {
			if (mix_cur == 0.f) {
				LOOP(inNumSamples, PZ(out); ZX(out) *= amp_cur; amp_cur += amp_slope; ZP(out););
			} else {
				LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + mix_cur; amp_cur += amp_slope; ZP(out););
			}
		} else {
			LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + mix_cur; amp_cur += amp_slope; mix_cur += mix_slope; ZP(out););
			unit->mPrevAdd = mix_cur;
		}
		unit->mPrevMul = amp_cur;
	}
}

void ampmix_ki(Unit *unit, int inNumSamples);
void ampmix_ki(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *mix = ADDIN - ZOFF;
	
	float amp_cur = unit->mPrevMul;
	float amp_slope = CALCSLOPE(MULIN[0], amp_cur);
	float mix_cur = unit->mPrevAdd;
	//postbuf("ampmix_ki %08X %g %g\n", out, amp_cur, mix_cur);
	
	if (amp_slope == 0.f) {
		if (amp_cur == 1.f) {
			LOOP(inNumSamples, ZXP(out) += mix_cur;);
		} else if (amp_cur == 0.f) {
			LOOP(inNumSamples, ZXP(out) = mix_cur;);
		} else {
			LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + mix_cur; ZP(out););
		}
	} else {
		LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + mix_cur; amp_cur += amp_slope; ZP(out););
		unit->mPrevMul = amp_cur;
	}
}

void ampmix_k0(Unit *unit, int inNumSamples);
void ampmix_k0(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float amp_cur = unit->mPrevMul;
	float amp_slope = CALCSLOPE(MULIN[0], amp_cur);
	
	if (amp_slope == 0.f) {
		if (amp_cur == 1.f) {
			// do nothing!
		} else if (amp_cur == 0.f) {
			ZClear(inNumSamples, out);
		} else {
			LOOP(inNumSamples, ZXP(out) *= amp_cur;);
		}
	} else {
		LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out); amp_cur += amp_slope; ZP(out););
		unit->mPrevMul = amp_cur;
	}

}

void ampmix_ia(Unit *unit, int inNumSamples);
void ampmix_ia(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *mix = ADDIN - ZOFF;
	float amp_cur = unit->mPrevMul;
	
	LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + ZXP(mix); ZP(out););
}

void ampmix_ik(Unit *unit, int inNumSamples);
void ampmix_ik(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	
	float amp_cur = unit->mPrevMul;
	float mix_cur = unit->mPrevAdd;
	float mix_slope = CALCSLOPE(ADDIN[0], mix_cur);
	
	if (mix_slope == 0.f) {
		if (mix_cur == 0.f) {
			LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out); ZP(out););
		} else {
			LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + mix_cur; ZP(out););
		}
	} else {
		LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + mix_cur; mix_cur += mix_slope; ZP(out););
		unit->mPrevAdd = mix_cur;
	}
}

void ampmix_ii(Unit *unit, int inNumSamples);
void ampmix_ii(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);

	float amp_cur = unit->mPrevMul;
	float mix_cur = unit->mPrevAdd;
	LOOP(inNumSamples, PZ(out); ZX(out) = amp_cur * ZX(out) + mix_cur; ZP(out););
}

void ampmix_i0(Unit *unit, int inNumSamples);
void ampmix_i0(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);

	float amp_cur = unit->mPrevMul;
	
	LOOP(inNumSamples, 
		//postbuf("ampmix_i0 in %g amp %g out %g\n", out[ZOFF], amp_cur, out[ZOFF] * amp_cur);
		ZXP(out) *= amp_cur;
	);
}

void ampmix_1a(Unit *unit, int inNumSamples);
void ampmix_1a(Unit *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *mix = ADDIN;
	Accum(inNumSamples, out, mix);
}

void ampmix_1k(Unit *unit, int inNumSamples);
void ampmix_1k(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);

	float mix_cur = unit->mPrevAdd;
	float mix_slope = CALCSLOPE(ADDIN[0], mix_cur);
	if (mix_slope == 0.f) {
		if (mix_cur == 0.f) {
			// do nothing!
		} else {
			LOOP(inNumSamples, ZXP(out) += mix_cur;);
		}
	} else {
		LOOP(inNumSamples, ZXP(out) += mix_cur; mix_cur += mix_slope;);
		unit->mPrevAdd = mix_cur;
	}
}

void ampmix_1i(Unit *unit, int inNumSamples);
void ampmix_1i(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);

	float mix_cur = unit->mPrevAdd;
	LOOP(inNumSamples, PZ(out); ZX(out) = ZX(out) + mix_cur; ZP(out););
}

void ampmix_10(Unit *unit, int inNumSamples);
void ampmix_10(Unit *unit, int inNumSamples)
{
	// this function is never even executed. The UGen tests for this case.
}

void ampmix_0a(Unit *unit, int inNumSamples);
void ampmix_0a(Unit *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *mix = ADDIN;
	Copy(inNumSamples, out, mix);
}

void ampmix_0k(Unit *unit, int inNumSamples);
void ampmix_0k(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	
	float mix_cur = unit->mPrevAdd;
	float mix_slope = CALCSLOPE(ADDIN[0], mix_cur);
	
	if (mix_slope == 0.f) {
		LOOP(inNumSamples, ZXP(out) = mix_cur;);
	} else {
		LOOP(inNumSamples, PZ(out); ZX(out) = mix_cur; mix_cur += mix_slope; ZP(out););
		unit->mPrevAdd = mix_cur;
	}
}

void ampmix_0i(Unit *unit, int inNumSamples);
void ampmix_0i(Unit *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float mix_cur = unit->mPrevAdd;
	LOOP(inNumSamples, ZXP(out) = mix_cur;);
}

void ampmix_00(Unit *unit, int inNumSamples);
void ampmix_00(Unit *unit, int inNumSamples)
{
	float *out = OUT(0);
	Clear(inNumSamples, out);
}

#if __VEC__

void v_ampmix_aa(Unit *unit, int inNumSamples);
void v_ampmix_aa(Unit *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *amp = MULIN;
	float *mix = ADDIN;

	int i, len = gKSAMPS << 2;
	vector float *vout = (vector float*)out;
	vector float *vamp = (vector float*)amp;
	//LOOP(inNumSamples, PZ(out); *out = ZXP(amp) * *out + ZXP(mix); ZP(out););
	vector float *vmix = (vector float*)mix;
	for (i=0; i<len; i+=16) {
		vec_st(vec_madd(vec_ld(i, vamp), vec_ld(i, vout), vec_ld(i, vmix)), i, vout);
	}
	//out_ptr->phyend = smax(smin(out_ptr->phyend, amp_ptr->phyend), mix_ptr->phyend);
	//out_ptr->durend = smax(smin(out_ptr->durend, amp_ptr->durend), mix_ptr->durend);
	
}

void v_ampmix_ak(Unit *unit, int inNumSamples);
void v_ampmix_ak(Unit *unit, int inNumSamples)
{
	int i, len = gKSAMPS << 2;
	define_vtemp

	float *out = ZOUT(0);
	float *amp = MULIN;

	float mix_cur = mPrevAdd;
	float mix_slope = CalcSlope(ADDIN[0], mix_cur);
	if (mix_slope == 0.f) {
		if (mix_cur == 0.f) {
			//LOOP(inNumSamples, PZ(out); *out = ZXP(amp) * *out; ZP(out););
			vector float *vout = (vector float*)out;
			vector float *vamp = (vector float*)amp;
			define_vzero 				
			for (i=0; i<len; i+=16) {
				vec_st(vec_mul(vec_ld(i, vamp), vec_ld(i, vout)), i, vout);
			}
		} else {
			//LOOP(inNumSamples, PZ(out); *out = ZXP(amp) * *out + mix_cur; ZP(out););
			vector float *vout = (vector float*)out;
			vector float *vamp = (vector float*)amp;
			vector float vmix = vload(*mix);
			for (i=0; i<len; i+=16) {
				vec_st(vec_madd(vec_ld(i, vamp), vec_ld(i, vout), vmix), i, vout);
			}
		}
	} else {
		//LOOP(inNumSamples, PZ(out); *out = ZXP(amp) * *out + mix_cur; mix_cur += mix_slope; ZP(out););
		vector float *vout = (vector float*)out;
		vector float *vamp = (vector float*)amp;
		vector float vmix_slope = vload(4.f * mix_slope);
		vector float vmix = vstart(mix_cur, vmix_slope);
		for (i=0; i<len; i+=16) {
			vec_st(vec_madd(vec_ld(i, vamp), vec_ld(i, vout), vmix), i, vout);
			vmix = vec_add(vmix, vmix_slope);
		}
		mPrevAdd = mix_cur;
	}
	
}

void v_ampmix_ai(Unit *unit, int inNumSamples);
void v_ampmix_ai(Unit *unit, int inNumSamples)
{
	float amp_cur, mix_cur, amp_slope, mix_slope;
	int i, len = gKSAMPS << 2;
	define_vtemp

	float *out = ZOUT(0);
	
	float *amp = MULIN;

	float mix_cur = mPrevAdd;
	//LOOP(inNumSamples, PZ(out); *out = ZXP(amp) * *out + mix_cur; ZP(out););
	vector float *vout = (vector float*)out;
	vector float *vamp = (vector float*)amp;
	vector float vmix = vload(mix_cur);
	for (i=0; i<len; i+=16) {
		vec_st(vec_madd(vec_ld(i, vamp), vec_ld(i, vout), vmix), i, vout);
	}
}

void v_ampmix_a0(Unit *unit, int inNumSamples);
void v_ampmix_a0(Unit *unit, int inNumSamples)
{
	float amp_cur, mix_cur, amp_slope, mix_slope;

	float *out = ZOUT(0);
	float *amp = MULIN;
	float *mix = ADDIN;

	//LOOP(inNumSamples, PZ(out); *out = ZXP(amp) * *out; ZP(out););
	int i, len = gKSAMPS << 2;
	vector float *vout = (vector float*)out;
	vector float *vamp = (vector float*)amp;
	define_vzero 				
	for (i=0; i<len; i+=16) {
		vec_st(vec_mul(vec_ld(i, vamp), vec_ld(i, vout)), i, vout);
	}

}

void v_ampmix_ka(Unit *unit, int inNumSamples);
void v_ampmix_ka(Unit *unit, int inNumSamples)
{
	float amp_cur, amp_slope;
	int i, len = gKSAMPS << 2;
	define_vtemp
	define_vzero

	float *out = ZOUT(0);
	float *mix = ADDIN;
	amp_cur = mPrevMul;
	amp_slope = CalcSlope(MULIN[0], amp_cur);

	if (amp_slope == 0.f) {
		if (amp_cur == 0.f) {
			out_ptr->bufptr = mix;
		} else if (amp_cur == 1.f) {
			vector float *vout = (vector float*)out;
			vector float *vmix = (vector float*)mix;
			for (i=0; i<len; i+=16) {
				vec_st(vec_add(vec_ld(i, vout), vec_ld(i, vmix)), i, vout);
			}
		} else {
			//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + ZXP(mix); ZP(out););
			vector float *vout = (vector float*)out;
			vector float vamp = vload(amp_cur);
			vector float *vmix = (vector float*)mix;
			for (i=0; i<len; i+=16) {
				vec_st(vec_madd(vamp, vec_ld(i, vout), vec_ld(i, vmix)), i, vout);
			}
			//mPrevMul = *amp;
		}
	} else {
		define_vzero
		vector float *vout = (vector float*)out;
		vector float vamp_slope = vload(4.f * amp_slope);
		vector float vamp = vstart(amp_cur, vamp_slope);
		vector float *vmix = (vector float*)mix;
		for (i=0; i<len; i+=16) {
			vec_st(vec_madd(vamp, vec_ld(i, vout), vec_ld(i, vmix)), i, vout);
			vamp = vec_add(vamp, vamp_slope);
		}
		mPrevMul = *amp;
	}

}

void v_ampmix_kk(Unit *unit, int inNumSamples);
void v_ampmix_kk(Unit *unit, int inNumSamples)
{
	float amp_cur, mix_cur, amp_slope, mix_slope;
	int i, len = gKSAMPS << 2;
	define_vtemp
	define_vzero

	float *out = ZOUT(0);

	amp_cur = mPrevMul;
	amp_slope = CalcSlope(MULIN[0], amp_cur);
	mix_cur = mPrevAdd;
	mix_slope = CalcSlope(ADDIN[0], mix_cur);

	if (amp_slope == 0.f) {
		if (mix_slope == 0.f) {
			if (mix_cur == 0.f) {
				if (amp_cur == 1.f) {
					// do nothing!
				} else if (amp_cur == 0.f) {
					for (i=0; i<len; i+=16) {
						vec_st(vzero, i, vout);
					}
				} else {
					//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out; ZP(out););
					vector float *vout = (vector float*)out;
					vector float vamp = vload(amp_cur);
					for (i=0; i<len; i+=16) {
						vec_st(vec_mul(vamp, vec_ld(i, vout)), i, vout);
					}
				}
			} else {
				if (amp_cur == 1.f) {
					//LOOP(inNumSamples, ZXP(out) += mix_cur;);
					vector float *vout = (vector float*)out;
					vector float vmix = vload(mix_cur);
					for (i=0; i<len; i+=16) {
						vec_st(vec_add(vmix, vec_ld(i, vout)), i, vout);
					}
				} else if (amp_cur == 0.f) {
					//LOOP(inNumSamples, ZXP(out) = mix_cur;);
					vector float *vout = (vector float*)out;
					vector float vmix = vload(mix_cur);
					for (i=0; i<len; i+=16) {
						vec_st(vmix, i, vout);
					}
				} else {
					//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + mix_cur; ZP(out););
					vector float *vout = (vector float*)out;
					vector float vamp = vload(amp_cur);
					vector float vmix = vload(mix_cur);
					for (i=0; i<len; i+=16) {
						vec_st(vec_madd(vamp, vec_ld(i, vout), vmix), i, vout);
					}
				}
			}
		} else {
			if (amp_cur == 1.f) {
				//LOOP(inNumSamples, ZXP(out) += mix_cur; mix_cur += mix_slope;);
				vector float *vout = (vector float*)out;
				vector float vmix_slope = vload(4.f * mix_slope);
				vector float vmix = vstart(mix_cur, vmix_slope);
				for (i=0; i<len; i+=16) {
					vec_st(vec_add(vmix, vec_ld(i, vout)), i, vout);
					vmix = vec_add(vmix, vmix_slope);
				}
			} else if (amp_cur == 0.f) {
				//LOOP(inNumSamples, ZXP(out) = mix_cur; mix_cur += mix_slope;);
				vector float *vout = (vector float*)out;
				vector float vmix_slope = vload(4.f * mix_slope);
				vector float vmix = vstart(mix_cur, vmix_slope);
				for (i=0; i<len; i+=16) {
					vec_st(vmix, i, vout);
					vmix = vec_add(vmix, vmix_slope);
				}
			} else {
				//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + mix_cur; mix_cur += mix_slope; ZP(out););
				vector float *vout = (vector float*)out;
				vector float vamp = vload(amp_cur);
				vector float vmix_slope = vload(4.f * mix_slope);
				vector float vmix = vstart(mix_cur, vmix_slope);
				for (i=0; i<len; i+=16) {
					vec_st(vec_madd(vamp, vec_ld(i, vout), vmix), i, vout);
					vmix = vec_add(vmix, vmix_slope);
				}
			}
			mPrevAdd = mix_cur;
		}
	} else {
		if (mix_slope == 0.f) {
			if (mix_cur == 0.f) {
				//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out; amp_cur += amp_slope; ZP(out););
				vector float *vout = (vector float*)out;
				vector float vamp_slope = vload(4.f * amp_slope);
				vector float vamp = vstart(amp_cur, vamp_slope);
				for (i=0; i<len; i+=16) {
					vec_st(vec_mul(vamp, vec_ld(i, vout)), i, vout);
					vamp = vec_add(vamp, vamp_slope);
				}
			} else {
				//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + mix_cur; amp_cur += amp_slope; ZP(out););
				vector float *vout = (vector float*)out;
				vector float vamp_slope = vload(4.f * amp_slope);
				vector float vamp = vstart(amp_cur, vamp_slope);
				vector float vmix = vload(mix_cur);
				for (i=0; i<len; i+=16) {
					vec_st(vec_madd(vamp, vec_ld(i, vout), vmix), i, vout);
					vamp = vec_add(vamp, vamp_slope);
				}
			}
		} else {
			//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + mix_cur; amp_cur += amp_slope; mix_cur += mix_slope; ZP(out););
			vector float *vout = (vector float*)out;
			vector float vamp_slope = vload(4.f * amp_slope);
			vector float vamp = vstart(amp_cur, vamp_slope);
			vector float vmix_slope = vload(4.f * mix_slope);
			vector float vmix = vstart(mix_cur, vmix_slope);
			for (i=0; i<len; i+=16) {
				vec_st(vec_madd(vamp, vec_ld(i, vout), vmix), i, vout);
				vamp = vec_add(vamp, vamp_slope);
				vmix = vec_add(vmix, vmix_slope);
			}
			mPrevAdd = *mix;
		}
		mPrevMul = *amp;
	}

}

void v_ampmix_ki(Unit *unit, int inNumSamples);
void v_ampmix_ki(Unit *unit, int inNumSamples)
{

	float amp_cur, mix_cur, amp_slope;
	int i, len = gKSAMPS << 2;
	define_vtemp

	float *out = ZOUT(0);
	float *mix = ADDIN;
	amp_cur = mPrevMul;
	amp_slope = CalcSlope(MULIN[0], amp_cur);
	mix_cur = mPrevAdd;
	if (amp_slope == 0.f) {
		if (amp_cur == 1.f) {
			//LOOP(inNumSamples, ZXP(out) += mix_cur;);
			vector float *vout = (vector float*)out;
			vector float vmix = vload(mix_cur);
			for (i=0; i<len; i+=16) {
				vec_st(vec_add(vmix, vec_ld(i, vout)), i, vout);
			}
		} else if (amp_cur == 0.f) {
			//LOOP(inNumSamples, ZXP(out) = mix_cur;);
			vector float *vout = (vector float*)out;
			vector float vmix = vload(mix_cur);
			for (i=0; i<len; i+=16) {
				vec_st(vmix, i, vout);
			}
		} else {
			//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + mix_cur; ZP(out););
			vector float *vout = (vector float*)out;
			vector float vamp = vload(amp_cur);
			vector float vmix = vload(mix_cur);
			for (i=0; i<len; i+=16) {
				vec_st(vec_madd(vamp, vec_ld(i, vout), vmix), i, vout);
			}
		}
	} else {
		//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + mix_cur; amp_cur += amp_slope; ZP(out););
		vector float *vout = (vector float*)out;
		vector float vamp_slope = vload(4.f * amp_slope);
		vector float vamp = vstart(amp_cur, vamp_slope);
		vector float vmix = vload(mix_cur);
		for (i=0; i<len; i+=16) {
			vec_st(vec_madd(vamp, vec_ld(i, vout), vmix), i, vout);
			vamp = vec_add(vamp, vamp_slope);
		}
		mPrevMul = amp_cur;
	}
}

void v_ampmix_k0(Unit *unit, int inNumSamples);
void v_ampmix_k0(Unit *unit, int inNumSamples)
{

	float amp_cur, mix_cur, amp_slope, mix_slope;
	int i, len = gKSAMPS << 2;
	define_vtemp
	define_vzero

	float *out = ZOUT(0);

	amp_cur = mPrevMul;
	amp_slope = CalcSlope(MULIN[0], amp_cur);

	if (amp_slope == 0.f) {
		if (amp_cur == 1.f) {
			// do nothing!
		} else if (amp_cur == 0.f) {
			for (i=0; i<len; i+=16) {
				vec_st(vzero, i, vout);
			}
		} else {
			//LOOP(inNumSamples, PZ(out); *out *= amp_cur; ZP(out););
			vector float *vout = (vector float*)out;
			vector float vamp = vload(amp_cur);
			for (i=0; i<len; i+=16) {
				vec_st(vec_mul(vamp, vec_ld(i, vout)), i, vout);
			}
		}
	} else {
		//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out; amp_cur += amp_slope; ZP(out););
		vector float *vout = (vector float*)out;
		vector float vamp_slope = vload(4.f * amp_slope);
		vector float vamp = vstart(amp_cur, vamp_slope);
		for (i=0; i<len; i+=16) {
			vec_st(vec_mul(vamp, vec_ld(i, vout)), i, vout);
			vamp = vec_add(vamp, vamp_slope);
		}
		mPrevMul = amp_cur;
	}

}

void v_ampmix_ia(Unit *unit, int inNumSamples);
void v_ampmix_ia(Unit *unit, int inNumSamples)
{
	float amp_cur, amp_slope;

	float *out = ZOUT(0);
	int i, len = gKSAMPS << 2;
	define_vtemp
	define_vzero

	float *amp = MULIN;
	float *mix = ADDIN;
	amp_cur = mPrevMul;
	//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + ZXP(mix); ZP(out););
	vector float *vout = (vector float*)out;
	vector float vamp = vload(amp_cur);
	vector float *vmix = (vector float*)mix;
	for (i=0; i<len; i+=16) {
		vec_st(vec_madd(vamp, vec_ld(i, vout), vec_ld(i, vmix)), i, vout);
	}
}

void v_ampmix_ik(Unit *unit, int inNumSamples);
void v_ampmix_ik(Unit *unit, int inNumSamples)
{

	float amp_cur, mix_cur, amp_slope, mix_slope;
	int i, len = gKSAMPS << 2;
	define_vtemp
	define_vzero

	float *out = ZOUT(0);
	float *amp = MULIN;

	amp_cur = mPrevMul;
	mix_cur = mPrevAdd;
	mix_slope = CalcSlope(ADDIN[0], mix_cur);
	if (mix_slope == 0.f) {
		if (mix_cur == 0.f) {
			//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out; ZP(out););
			vector float *vout = (vector float*)out;
			vector float vamp = vload(amp_cur);
			for (i=0; i<len; i+=16) {
				vec_st(vec_mul(vamp, vec_ld(i, vout)), i, vout);
			}
		} else {
			//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + mix_cur; ZP(out););
			vector float *vout = (vector float*)out;
			vector float vamp = vload(amp_cur);
			vector float vmix = vload(mix_cur);
			for (i=0; i<len; i+=16) {
				vec_st(vec_madd(vamp, vec_ld(i, vout), vmix), i, vout);
			}
		}
	} else {
		//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + mix_cur; mix_cur += mix_slope; ZP(out););
		vector float *vout = (vector float*)out;
		vector float vamp = vload(amp_cur);
		vector float vmix_slope = vload(4.f * mix_slope);
		vector float vmix = vstart(mix_cur, vmix_slope);
		for (i=0; i<len; i+=16) {
			vec_st(vec_madd(vamp, vec_ld(i, vout), vmix), i, vout);
			vmix = vec_add(vmix, vmix_slope);
		}
		mPrevAdd = mix_cur;
	}
}

void v_ampmix_ii(Unit *unit, int inNumSamples);
void v_ampmix_ii(Unit *unit, int inNumSamples)
{
	float *out, *endptr;
	float amp_cur, mix_cur;
	int i, len = gKSAMPS << 2;
	define_vtemp

	float *out = ZOUT(0);

	amp_cur = mPrevMul;
	mix_cur = mPrevAdd;

	//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out + mix_cur; ZP(out););
	vector float *vout = (vector float*)out;
	vector float vamp = vload(amp_cur);
	vector float vmix = vload(mix_cur);
	for (i=0; i<len; i+=16) {
		vec_st(vec_madd(vamp, vec_ld(i, vout), vmix), i, vout);
	}
}

void v_ampmix_i0(Unit *unit, int inNumSamples);
void v_ampmix_i0(Unit *unit, int inNumSamples)
{
	float *out, *endptr;
	float amp_cur;
	int i, len = gKSAMPS << 2;
	define_vtemp

	float *out = ZOUT(0);

	amp_cur = mPrevMul;
	//LOOP(inNumSamples, PZ(out); *out = amp_cur * *out; ZP(out););
	vector float *vout = (vector float*)out;
	vector float vamp = vload(amp_cur);
	define_vzero
	for (i=0; i<len; i+=16) {
		vec_st(vec_mul(vamp, vec_ld(i, vout)), i, vout);
	}
}

void v_ampmix_1a(Unit *unit, int inNumSamples);
void v_ampmix_1a(Unit *unit, int inNumSamples)
{
	float *out, *mix, *endptr;

	float *out = ZOUT(0);

	float *mix = ADDIN;
	int i, len = gKSAMPS << 2;
	vector float *vout = (vector float*)out;
	vector float *vmix = (vector float*)mix;
	for (i=0; i<len; i+=16) {
		vec_st(vec_add(vec_ld(i, vout), vec_ld(i, vmix)), i, vout);
	}
}

void v_ampmix_1k(Unit *unit, int inNumSamples);
void v_ampmix_1k(Unit *unit, int inNumSamples)
{
	float *out, *mix, *endptr;
	float mix_cur, mix_slope;
	int i, len = gKSAMPS << 2;
	define_vtemp

	float *out = ZOUT(0);

	mix_cur = mPrevAdd;
	mix_slope = CalcSlope(ADDIN[0], mix_cur);
	if (mix_slope == 0.f) {
		if (mix_cur == 0.f) {
			// do nothing!
		} else {
			//LOOP(inNumSamples, ZXP(out) += mix_cur;);
			vector float *vout = (vector float*)out;
			vector float vmix = vload(mix_cur);
			for (i=0; i<len; i+=16) {
				vec_st(vec_add(vmix, vec_ld(i, vout)), i, vout);
			}
		}
	} else {
		//LOOP(inNumSamples, ZXP(out) += mix_cur; mix_cur += mix_slope;);
		vector float *vout = (vector float*)out;
		vector float vmix_slope = vload(4.f * mix_slope);
		vector float vmix = vstart(mix_cur, vmix_slope);
		for (i=0; i<len; i+=16) {
			vec_st(vec_add(vmix, vec_ld(i, vout)), i, vout);
			vmix = vec_add(vmix, vmix_slope);
		}
		//mPrevAdd = mix_cur;
		mPrevAdd = *mix;
	}
}

void v_ampmix_1i(Unit *unit, int inNumSamples);
void v_ampmix_1i(Unit *unit, int inNumSamples)
{
	float *out, *endptr;
	float mix_cur;
	int i, len = gKSAMPS << 2;
	define_vtemp

	float *out = ZOUT(0);

	mix_cur = mPrevAdd;
	//LOOP(inNumSamples, ZXP(out) += mix_cur;);
	vector float *vout = (vector float*)out;
	vector float vmix = vload(mix_cur);
	for (i=0; i<len; i+=16) {
		vec_st(vec_add(vmix, vec_ld(i, vout)), i, vout);
	}
}

void v_ampmix_10(Unit *unit, int inNumSamples);
void v_ampmix_10(Unit *unit, int inNumSamples)
{
	// this function is never even executed. The UGen tests for this case.
	// out_ptr->bufptr = out;
}

void v_ampmix_0a(Unit *unit, int inNumSamples);
void v_ampmix_0a(Unit *unit, int inNumSamples)
{
	out_ptr->bufptr = mix_ptr->bufptr;
	out_ptr->phyend = mix_ptr->phyend;
	out_ptr->durend = mix_ptr->durend;
}

void v_ampmix_0k(Unit *unit, int inNumSamples);
void v_ampmix_0k(Unit *unit, int inNumSamples)
{
	float *out, *mix, *endptr;
	float mix_cur, mix_slope;
	int i, len = gKSAMPS << 2;
	define_vtemp

	float *out = ZOUT(0);
	
	mix_cur = mPrevAdd;
	mix_slope = CalcSlope(ADDIN[0], mix_cur);
	if (mix_slope == 0.f) {
		if (mix_cur == 0.f) {
			for (i=0; i<len; i+=16) {
				vec_st(vzero, i, vout);
			}
		} else {
			//LOOP(inNumSamples, ZXP(out) = mix_cur;);
			vector float *vout = (vector float*)out;
			vector float vmix = vload(mix_cur);
			for (i=0; i<len; i+=16) {
				vec_st(vmix, i, vout);
			}
		}
	} else {
		//LOOP(inNumSamples, ZXP(out) = mix_cur; mix_cur += mix_slope;);
		vector float *vout = (vector float*)out;
		vector float vmix_slope = vload(4.f * mix_slope);
		vector float vmix = vstart(mix_cur, vmix_slope);
		for (i=0; i<len; i+=16) {
			vec_st(vmix, i, vout);
			vmix = vec_add(vmix, vmix_slope);
		}
		mPrevAdd = mix_cur;
	}
	out_ptr->phyend = mix_ptr->phyend;
	out_ptr->durend = mix_ptr->durend;
}

void v_ampmix_0i(Unit *unit, int inNumSamples);
void v_ampmix_0i(Unit *unit, int inNumSamples)
{
	float *out, *endptr;
	float mix_cur;
	int i, len = gKSAMPS << 2;
	define_vtemp

	float *out = ZOUT(0);

	mix_cur = mPrevAdd;
	//LOOP(inNumSamples, ZXP(out) = mix_cur;);
	vector float *vout = (vector float*)out;
	vector float vmix = vload(mix_cur);
	for (i=0; i<len; i+=16) {
		vec_st(vmix, i, vout);
	}
	out_ptr->phyend = 0;
	//out_ptr->durend = 0;
}

void v_ampmix_00(Unit *unit, int inNumSamples);
void v_ampmix_00(Unit *unit, int inNumSamples)
{
	int i, len = gKSAMPS << 2;
	define_vzero;
	for (i=0; i<len; i+=16) {
		vec_st(vzero, i, vout);
	}
	out_ptr->phyend = 0;
	//out_ptr->durend = 0;
}

#endif



#include "SC_WireSpec.h"
#include "SC_GraphDef.h"

void ChooseMulAddFunc(GraphDef *graphDef, UnitSpec* unitSpec);
void ChooseMulAddFunc(GraphDef *graphDef, UnitSpec* unitSpec)
{
	//printf("Unit_ChooseMulAddFunc\n");
	
	if (!unitSpec->mUnitDef->mDoMulAdd) {
		unitSpec->mMulAddFunc = 0;
		return;
	}

	if (unitSpec->mCalcRate != calc_FullRate) {
		unitSpec->mMulAddFunc = &ampmix_k;
		return;
	}
	
	float mul, add;
	int numInputs = unitSpec->mNumInputs;
	
	int mulRate = unitSpec->mInputSpec[numInputs - 2].mCalcRate;
	int mulWireIndex = unitSpec->mInputSpec[numInputs - 2].mWireIndex;
	if (mulWireIndex < graphDef->mNumConstants) {
		mul = graphDef->mConstants[mulWireIndex];
	} else {
		mul = 99.; // just not 1 or 0.
	}
	
	int addRate = unitSpec->mInputSpec[numInputs - 1].mCalcRate;
	int addWireIndex = unitSpec->mInputSpec[numInputs - 1].mWireIndex;
	if (addWireIndex < graphDef->mNumConstants) {
		add = graphDef->mConstants[addWireIndex];
	} else {
		add = 99.; // just not 1 or 0.
	}
		
	//postbuf("numInputs %d  mulRate %d  addRate %d\n", numInputs, mulRate, addRate);
	//postbuf("mul %g   add %g\n", mul, add);
	
#if __VEC__	
	if (gKSAMPS & 3) {
#endif
		switch (mulRate) {
			case calc_FullRate :
				switch (addRate) {
					case calc_FullRate :
						unitSpec->mMulAddFunc = &ampmix_aa;
						break;
					case calc_BufRate :
						unitSpec->mMulAddFunc = &ampmix_ak;
						break;
					case calc_ScalarRate :
						if (add == 0.f) unitSpec->mMulAddFunc = &ampmix_a0;
						else unitSpec->mMulAddFunc = &ampmix_ai;
						break;
				}
				break;
			case calc_BufRate :
				switch (addRate) {
					case calc_FullRate :
						unitSpec->mMulAddFunc = &ampmix_ka;
						break;
					case calc_BufRate :
						unitSpec->mMulAddFunc = &ampmix_kk;
						break;
					case calc_ScalarRate :
						if (add == 0.f) unitSpec->mMulAddFunc = &ampmix_k0;
						else unitSpec->mMulAddFunc = &ampmix_ki;
						break;
				}
				break;
			case calc_ScalarRate :
				if (mul == 0.f) {
					switch (addRate) {
						case calc_FullRate :
							unitSpec->mMulAddFunc = &ampmix_0a;
							break;
						case calc_BufRate :
							unitSpec->mMulAddFunc = &ampmix_0k;
							break;
						case calc_ScalarRate :
							if (add == 0.f) unitSpec->mMulAddFunc = &ampmix_00;
							else unitSpec->mMulAddFunc = &ampmix_0i;
							break;
					}
				} else if (mul == 1.f) {
					switch (addRate) {
						case calc_FullRate :
							unitSpec->mMulAddFunc = &ampmix_1a;
							break;
						case calc_BufRate :
							unitSpec->mMulAddFunc = &ampmix_1k;
							break;
						case calc_ScalarRate :
							if (add == 0.f) unitSpec->mMulAddFunc = 0; // do nothing
							else unitSpec->mMulAddFunc = &ampmix_1i;
							break;
					}
				} else {
					switch (addRate) {
						case calc_FullRate :
							unitSpec->mMulAddFunc = &ampmix_ia;
							break;
						case calc_BufRate :
							unitSpec->mMulAddFunc = &ampmix_ik;
							break;
						case calc_ScalarRate :
							if (add == 0.f) unitSpec->mMulAddFunc = &ampmix_i0;
							else unitSpec->mMulAddFunc = &ampmix_ii;
							break;
					}
				}
				break;
		}
#if __VEC__
	} else {
		switch (mulRate) {
			case calc_FullRate :
				switch (addRate) {
					case calc_FullRate :
						unitSpec->mMulAddFunc = &v_ampmix_aa;
						break;
					case calc_BufRate :
						unitSpec->mMulAddFunc = &v_ampmix_ak;
						break;
					case calc_ScalarRate :
						if (add == 0.f) unitSpec->mMulAddFunc = &v_ampmix_a0;
						else unitSpec->mMulAddFunc = &v_ampmix_ai;
						break;
				}
				break;
			case calc_BufRate :
				switch (addRate) {
					case calc_FullRate :
						unitSpec->mMulAddFunc = &v_ampmix_ka;
						break;
					case calc_BufRate :
						unitSpec->mMulAddFunc = &v_ampmix_kk;
						break;
					case calc_ScalarRate :
						if (add == 0.f) unitSpec->mMulAddFunc = &v_ampmix_k0;
						else unitSpec->mMulAddFunc = &v_ampmix_ki;
						break;
				}
				break;
			case calc_ScalarRate :
				if (mul == 0.f) {
					switch (addRate) {
						case calc_FullRate :
							unitSpec->mMulAddFunc = &v_ampmix_0a;
							break;
						case calc_BufRate :
							unitSpec->mMulAddFunc = &v_ampmix_0k;
							break;
						case calc_ScalarRate :
							if (add == 0.f) unitSpec->mMulAddFunc = &v_ampmix_00;
							else unitSpec->mMulAddFunc = &v_ampmix_0i;
							break;
					}
				} else if (mul == 1.f) {
					switch (addRate) {
						case calc_FullRate :
							unitSpec->mMulAddFunc = &v_ampmix_1a;
							break;
						case calc_BufRate :
							unitSpec->mMulAddFunc = &v_ampmix_1k;
							break;
						case calc_ScalarRate :
							if (add == 0.f) unitSpec->mMulAddFunc = 0; // do nothing
							else unitSpec->mMulAddFunc = &v_ampmix_1i;
							break;
					}
				} else {
					switch (addRate) {
						case calc_FullRate :
							unitSpec->mMulAddFunc = &v_ampmix_ia;
							break;
						case calc_BufRate :
							unitSpec->mMulAddFunc = &v_ampmix_ik;
							break;
						case calc_ScalarRate :
							if (add == 0.f) unitSpec->mMulAddFunc = &v_ampmix_i0;
							else unitSpec->mMulAddFunc = &v_ampmix_ii;
							break;
					}
				}
				break;
		}
	}
#endif	
}

#endif