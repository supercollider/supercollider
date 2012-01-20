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


#include "SC_PlugIn.h"



#ifdef NOVA_SIMD
#include "simd_memory.hpp"
#include "simd_binary_arithmetic.hpp"
#include "simd_ternary_arithmetic.hpp"

using nova::slope_argument;

#include "function_attributes.h"


#endif



static InterfaceTable *ft;

//////////////////////////////////////////////////////////////////////////////////////////////////

#define MULIN IN(1)
#define ADDIN IN(2)

struct MulAdd : public Unit
{
	float mPrevMul, mPrevAdd;
};

extern "C"
{
	void MulAdd_Ctor(MulAdd *unit);

	// mul add functions for every occasion:
	void ampmix_k(MulAdd *unit, int inNumSamples);

	void ampmix_aa(MulAdd *unit, int inNumSamples);
	void ampmix_ak(MulAdd *unit, int inNumSamples);
	void ampmix_ai(MulAdd *unit, int inNumSamples);

	void ampmix_ka(MulAdd *unit, int inNumSamples);
	void ampmix_kk(MulAdd *unit, int inNumSamples);
	void ampmix_ki(MulAdd *unit, int inNumSamples);

	void ampmix_ia(MulAdd *unit, int inNumSamples);
	void ampmix_ik(MulAdd *unit, int inNumSamples);
	void ampmix_ii(MulAdd *unit, int inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void ampmix_k(MulAdd *unit, int inNumSamples);
void ampmix_k(MulAdd *unit, int inNumSamples)
{
	ZOUT0(0) = ZIN0(0) * MULIN[0] + ADDIN[0];
}

void ampmix_aa(MulAdd *unit, int inNumSamples);
void ampmix_aa(MulAdd *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float *amp = MULIN - ZOFF;
	float *mix = ADDIN - ZOFF;

	LOOP1(inNumSamples, ZXP(out) = ZXP(amp) * ZXP(in) + ZXP(mix); );
}

void ampmix_ak(MulAdd *unit, int inNumSamples);
void ampmix_ak(MulAdd *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float *amp = MULIN - ZOFF;

	float mix_cur = unit->mPrevAdd;
	float nextMix = ADDIN[0];
	float mix_slope = CALCSLOPE(nextMix, mix_cur);
	if (mix_slope == 0.f) {
		if (mix_cur == 0.f) {
			LOOP1(inNumSamples, ZXP(out) = ZXP(amp) * ZXP(in); );
		} else {
			LOOP1(inNumSamples, ZXP(out) = ZXP(amp) * ZXP(in) + mix_cur; );
		}
	} else {
		LOOP1(inNumSamples, ZXP(out) = ZXP(amp) * ZXP(in) + mix_cur; mix_cur += mix_slope; );
		unit->mPrevAdd = nextMix;
	}
}

void ampmix_ai(MulAdd *unit, int inNumSamples);
void ampmix_ai(MulAdd *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float *amp = MULIN - ZOFF;
	float mix_cur = unit->mPrevAdd;

	LOOP1(inNumSamples, ZXP(out) = ZXP(amp) * ZXP(in) + mix_cur; );
}

void ampmix_ka(MulAdd *unit, int inNumSamples);
void ampmix_ka(MulAdd *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float *mix = ADDIN - ZOFF;
	float amp_cur = unit->mPrevMul;
	float nextAmp = MULIN[0];
	float amp_slope = CALCSLOPE(nextAmp, amp_cur);

	if (amp_slope == 0.f) {
		if (amp_cur == 0.f) {
			ZCopy(inNumSamples, out, mix);
		} else if (amp_cur == 1.f) {
			LOOP1(inNumSamples, ZXP(out) = ZXP(in) + ZXP(mix); );
		} else {
			LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + ZXP(mix); );
		}
	} else {
		LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + ZXP(mix); amp_cur += amp_slope; );
		unit->mPrevMul = nextAmp;
	}
}

void ampmix_kk(MulAdd *unit, int inNumSamples);
void ampmix_kk(MulAdd *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);

	float amp_cur = unit->mPrevMul;
	float nextAmp = MULIN[0];
	float amp_slope = CALCSLOPE(nextAmp, amp_cur);
	float mix_cur = unit->mPrevAdd;
	float nextMix = ADDIN[0];
	float mix_slope = CALCSLOPE(nextMix, mix_cur);

	if (amp_slope == 0.f) {
		if (mix_slope == 0.f) {
			if (mix_cur == 0.f) {
				if (amp_cur == 1.f) {
					if (out != in)
						ZCopy(inNumSamples, out, in);
				} else if (amp_cur == 0.f) {
					ZClear(inNumSamples, out);
				} else {
					LOOP1(inNumSamples, ZXP(out) = ZXP(in) * amp_cur;);
				}
			} else {
				if (amp_cur == 1.f) {
					LOOP1(inNumSamples, ZXP(out) = ZXP(in) + mix_cur;);
				} else if (amp_cur == 0.f) {
					LOOP1(inNumSamples, ZXP(out) = mix_cur;);
				} else {
					LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + mix_cur; );
				}
			}
		} else {
			if (amp_cur == 1.f) {
				LOOP1(inNumSamples, ZXP(out) = ZXP(in) + mix_cur; mix_cur += mix_slope;);
			} else if (amp_cur == 0.f) {
				LOOP1(inNumSamples, ZXP(out) = mix_cur; mix_cur += mix_slope;);
			} else {
				LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + mix_cur; mix_cur += mix_slope; );
			}
			unit->mPrevAdd = nextMix;
		}
	} else {
		if (mix_slope == 0.f) {
			if (mix_cur == 0.f) {
				LOOP1(inNumSamples, ZXP(out) = ZXP(in) * amp_cur; amp_cur += amp_slope; );
			} else {
				LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + mix_cur; amp_cur += amp_slope; );
			}
		} else {
			LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + mix_cur; amp_cur += amp_slope; mix_cur += mix_slope; );
			unit->mPrevAdd = nextMix;
		}
		unit->mPrevMul = nextAmp;
	}
}

void ampmix_ki(MulAdd *unit, int inNumSamples);
void ampmix_ki(MulAdd *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);

	float amp_cur = unit->mPrevMul;
	float nextAmp = MULIN[0];
	float amp_slope = CALCSLOPE(nextAmp, amp_cur);
	float mix_cur = unit->mPrevAdd;
	//postbuf("ampmix_ki %p %g %g\n", out, amp_cur, mix_cur);

	if (amp_slope == 0.f) {
		if (amp_cur == 1.f) {
			LOOP1(inNumSamples, ZXP(out) = ZXP(in) + mix_cur;);
		} else if (amp_cur == 0.f) {
			LOOP1(inNumSamples, ZXP(out) = mix_cur;);
		} else {
			LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + mix_cur; );
		}
	} else {
		LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + mix_cur; amp_cur += amp_slope; );
		unit->mPrevMul = nextAmp;
	}
}

void ampmix_ia(MulAdd *unit, int inNumSamples);
void ampmix_ia(MulAdd *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float *mix = ADDIN - ZOFF;
	float amp_cur = unit->mPrevMul;

#ifdef IPHONE_VEC
	in++;
	out++;
	mix++;
	vscalarmul(in, amp_cur, in, inNumSamples);
	vadd(out, in, mix, inNumSamples);
#else
	LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + ZXP(mix); );
#endif
}


void ampmix_ik(MulAdd *unit, int inNumSamples);
void ampmix_ik(MulAdd *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);

	float amp_cur = unit->mPrevMul;
	float mix_cur = unit->mPrevAdd;
	float nextMix = ADDIN[0];
	float mix_slope = CALCSLOPE(nextMix, mix_cur);

	if (mix_slope == 0.f) {
		if (mix_cur == 0.f) {
			LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in); );
		} else {
			LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + mix_cur; );
		}
	} else {
		LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + mix_cur; mix_cur += mix_slope; );
		unit->mPrevAdd = nextMix;
	}
}

void ampmix_ii(MulAdd *unit, int inNumSamples);
void ampmix_ii(MulAdd *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);

	float amp_cur = unit->mPrevMul;
	float mix_cur = unit->mPrevAdd;
	LOOP1(inNumSamples, ZXP(out) = amp_cur * ZXP(in) + mix_cur; );
}

#ifdef NOVA_SIMD
FLATTEN void ampmix_aa_nova(MulAdd *unit, int inNumSamples)
{
	nova::muladd_vec_simd(OUT(0), IN(0), MULIN,
						  ADDIN, inNumSamples);
}

FLATTEN void ampmix_aa_nova_64(MulAdd *unit, int inNumSamples)
{
	nova::muladd_vec_simd<64>(OUT(0), IN(0), MULIN, ADDIN);
}

FLATTEN void ampmix_ak_nova(MulAdd *unit, int inNumSamples)
{
	float mix_cur = unit->mPrevAdd;
	float nextMix = ADDIN[0];
	if (nextMix == mix_cur) {
		if (mix_cur == 0.f)
			nova::times_vec_simd(OUT(0), IN(0), MULIN, inNumSamples);
		else
			nova::muladd_vec_simd(OUT(0), IN(0), MULIN, mix_cur, inNumSamples);
	} else {
		float mix_slope = CALCSLOPE(nextMix, mix_cur);
		unit->mPrevAdd = nextMix;
		nova::muladd_vec_simd(OUT(0), IN(0), MULIN,
							  slope_argument(mix_cur, mix_slope), inNumSamples);
	}
}

FLATTEN void ampmix_ak_nova_64(MulAdd *unit, int inNumSamples)
{
	float mix_cur = unit->mPrevAdd;
	float nextMix = ADDIN[0];
	if (nextMix == mix_cur) {
		if (mix_cur == 0.f)
			nova::times_vec_simd<64>(OUT(0), IN(0), MULIN);
		else
			nova::muladd_vec_simd<64>(OUT(0), IN(0), MULIN,
										 mix_cur);
	} else {
		float mix_slope = CALCSLOPE(nextMix, mix_cur);
		unit->mPrevAdd = nextMix;
		nova::muladd_vec_simd<64>(OUT(0), IN(0), MULIN,
								  slope_argument(mix_cur, mix_slope));
	}
}

FLATTEN void ampmix_ai_nova(MulAdd *unit, int inNumSamples)
{
	nova::muladd_vec_simd(OUT(0), IN(0), MULIN,
						  unit->mPrevAdd, inNumSamples);
}

FLATTEN void ampmix_ai_nova_64(MulAdd *unit, int inNumSamples)
{
	nova::muladd_vec_simd<64>(OUT(0), IN(0), MULIN,
							  unit->mPrevAdd);
}

FLATTEN void ampmix_ka_nova(MulAdd *unit, int inNumSamples)
{
	float amp_cur = unit->mPrevMul;
	float nextAmp = MULIN[0];

	if (amp_cur == nextAmp) {
		if (amp_cur == 0.f)
			nova::copyvec_simd(OUT(0), ADDIN, inNumSamples);
		else if (amp_cur == 1.f)
			nova::plus_vec_simd(OUT(0), IN(0), ADDIN, inNumSamples);
		else
			nova::muladd_vec_simd(OUT(0), IN(0), amp_cur,
								  ADDIN, inNumSamples);
	} else {
		float amp_slope = CALCSLOPE(nextAmp, amp_cur);
		unit->mPrevMul = nextAmp;
		nova::muladd_vec_simd(OUT(0), IN(0), slope_argument(amp_cur, amp_slope),
							  ADDIN, inNumSamples);
	}
}

FLATTEN void ampmix_ka_nova_64(MulAdd *unit, int inNumSamples)
{
	float amp_cur = unit->mPrevMul;
	float nextAmp = MULIN[0];

	if (amp_cur == nextAmp) {
		if (amp_cur == 0.f)
			nova::copyvec_simd<64>(OUT(0), ADDIN);
		else if (amp_cur == 1.f)
			nova::plus_vec_simd<64>(OUT(0), IN(0), ADDIN);
		else
			nova::muladd_vec_simd<64>(OUT(0), IN(0), amp_cur,
										 ADDIN);
	} else {
		float amp_slope = CALCSLOPE(nextAmp, amp_cur);
		unit->mPrevMul = nextAmp;
		nova::muladd_vec_simd<64>(OUT(0), IN(0), slope_argument(amp_cur, amp_slope),
								  ADDIN);
	}
}

FLATTEN void ampmix_kk_nova(MulAdd *unit, int inNumSamples)
{
	float amp_cur = unit->mPrevMul;
	float nextAmp = MULIN[0];
	float mix_cur = unit->mPrevAdd;
	float nextMix = ADDIN[0];

	if (nextAmp == amp_cur) {
		if (nextMix == mix_cur) {
			if (mix_cur == 0.f) {
					if (amp_cur == 1.f) {
						if (ZOUT(0) != ZIN(0))
							ZCopy(inNumSamples, ZOUT(0), ZIN(0));
					} else if (amp_cur == 0.f)
						nova::zerovec_simd(OUT(0), inNumSamples);
					else
						nova::times_vec_simd(OUT(0), IN(0), amp_cur, inNumSamples);
				} else {
					if (amp_cur == 1.f)
						nova::plus_vec_simd(OUT(0), IN(0), mix_cur, inNumSamples);
					else if (amp_cur == 0.f)
						nova::setvec_simd(OUT(0), mix_cur, inNumSamples);
					else
						nova::muladd_vec_simd(OUT(0), IN(0), amp_cur,
											  mix_cur, inNumSamples);
				}
		} else {
			float mix_slope = CALCSLOPE(nextMix, mix_cur);
			if (amp_cur == 1.f)
				nova::plus_vec_simd(OUT(0), IN(0), slope_argument(mix_cur, mix_slope), inNumSamples);
			else if (amp_cur == 0.f)
				nova::set_slope_vec_simd(OUT(0), mix_cur, mix_slope, inNumSamples);
			else
				nova::muladd_vec_simd(OUT(0), IN(0), amp_cur,
									  slope_argument(mix_cur, mix_slope), inNumSamples);
			unit->mPrevAdd = nextMix;
		}
	} else {
		float amp_slope = CALCSLOPE(nextAmp, amp_cur);
		if (nextMix == mix_cur) {
			if (mix_cur == 0.f)
				nova::times_vec_simd(OUT(0), IN(0), slope_argument(amp_cur, amp_slope), inNumSamples);
			else
				nova::muladd_vec_simd(OUT(0), IN(0), slope_argument(amp_cur, amp_slope),
									  mix_cur, inNumSamples);
		} else {
			float mix_slope = CALCSLOPE(nextMix, mix_cur);
			nova::muladd_vec_simd(OUT(0), IN(0), slope_argument(amp_cur, amp_slope),
								  slope_argument(mix_cur, mix_slope), inNumSamples);
			unit->mPrevAdd = nextMix;
		}
		unit->mPrevMul = nextAmp;
	}
}

FLATTEN void ampmix_ki_nova(MulAdd *unit, int inNumSamples)
{
	float amp_cur = unit->mPrevMul;
	float nextAmp = MULIN[0];
	float mix_cur = unit->mPrevAdd;

	if (nextAmp == amp_cur) {
		if (amp_cur == 1.f)
			nova::plus_vec_simd(OUT(0), IN(0), mix_cur, inNumSamples);
		else if (amp_cur == 0.f)
			nova::setvec_simd(OUT(0), mix_cur, inNumSamples);
		else
			nova::muladd_vec_simd(OUT(0), IN(0), amp_cur,
								  mix_cur, inNumSamples);
	} else {
		float amp_slope = CALCSLOPE(nextAmp, amp_cur);
		nova::muladd_vec_simd(OUT(0), IN(0), slope_argument(amp_cur, amp_slope),
							  mix_cur, inNumSamples);
		unit->mPrevMul = nextAmp;
	}
}

FLATTEN void ampmix_ki_nova_64(MulAdd *unit, int inNumSamples)
{
	float amp_cur = unit->mPrevMul;
	float nextAmp = MULIN[0];
	float mix_cur = unit->mPrevAdd;

	if (nextAmp == amp_cur) {
		if (amp_cur == 1.f)
			nova::plus_vec_simd<64>(OUT(0), IN(0), mix_cur);
		else if (amp_cur == 0.f)
			nova::setvec_simd<64>(OUT(0), mix_cur);
		else
			nova::muladd_vec_simd<64>(OUT(0), IN(0), amp_cur,
										 mix_cur);
	} else {
		float amp_slope = CALCSLOPE(nextAmp, amp_cur);
		nova::muladd_vec_simd<64>(OUT(0), IN(0), slope_argument(amp_cur, amp_slope),
								  mix_cur);
		unit->mPrevMul = nextAmp;
	}
}

FLATTEN void ampmix_ia_nova(MulAdd *unit, int inNumSamples)
{
	nova::muladd_vec_simd(OUT(0), IN(0), unit->mPrevMul,
						  ADDIN, inNumSamples);
}

FLATTEN void ampmix_ia_nova_64(MulAdd *unit, int inNumSamples)
{
	nova::muladd_vec_simd<64>(OUT(0), IN(0), unit->mPrevMul,
							  ADDIN);
}

FLATTEN void ampmix_ik_nova(MulAdd *unit, int inNumSamples)
{
	float amp_cur = unit->mPrevMul;
	float mix_cur = unit->mPrevAdd;
	float nextMix = ADDIN[0];

	if (nextMix == mix_cur) {
		if (mix_cur == 0.f)
			nova::times_vec_simd(OUT(0), IN(0), amp_cur, inNumSamples);
		else
			nova::muladd_vec_simd(OUT(0), IN(0), amp_cur,
								  mix_cur, inNumSamples);
	} else {
		float mix_slope = CALCSLOPE(nextMix, mix_cur);
		nova::muladd_vec_simd(OUT(0), IN(0), amp_cur,
							  slope_argument(mix_cur, mix_slope), inNumSamples);
		unit->mPrevAdd = nextMix;
	}
}

FLATTEN void ampmix_ik_nova_64(MulAdd *unit, int inNumSamples)
{
	float amp_cur = unit->mPrevMul;
	float mix_cur = unit->mPrevAdd;
	float nextMix = ADDIN[0];

	if (nextMix == mix_cur) {
		if (mix_cur == 0.f)
			nova::times_vec_simd<64>(OUT(0), IN(0), amp_cur);
		else
			nova::muladd_vec_simd<64>(OUT(0), IN(0), amp_cur, mix_cur);
	} else {
		float mix_slope = CALCSLOPE(nextMix, mix_cur);
		nova::muladd_vec_simd<64>(OUT(0), IN(0), amp_cur,
								  slope_argument(mix_cur, mix_slope));
		unit->mPrevAdd = nextMix;
	}
}

FLATTEN void ampmix_ii_nova(MulAdd *unit, int inNumSamples)
{
	nova::muladd_vec_simd(OUT(0), IN(0), unit->mPrevMul,
						  unit->mPrevAdd, inNumSamples);
}

FLATTEN void ampmix_ii_nova_64(MulAdd *unit, int inNumSamples)
{
	nova::muladd_vec_simd<64>(OUT(0), IN(0), unit->mPrevMul,
							  unit->mPrevAdd);
}

#endif




////////////////////////////////////////////////////////////////////////////////////////////////////////

void MulAdd_Ctor(MulAdd *unit)
{
	if (unit->mCalcRate != calc_FullRate) {
		unit->mCalcFunc = (UnitCalcFunc)&ampmix_k;
		ampmix_k(unit, 1);
		return;
	}

	unit->mPrevMul = ZIN0(1);
	unit->mPrevAdd = ZIN0(2);

	int mulRate = INRATE(1);
	int addRate = INRATE(2);

	//Print("muladd %d %d %g %g\n", mulRate, addRate, unit->mPrevMul, unit->mPrevAdd);
	//Print("**** %p %p %p    %p\n", IN(0), IN(1), IN(2), OUT(0));


#if defined (NOVA_SIMD)
	if ((BUFLENGTH & 15))
	{
#endif
		switch (mulRate) {
		case calc_FullRate :
			switch (addRate) {
				case calc_FullRate :
					unit->mCalcFunc = (UnitCalcFunc)&ampmix_aa;
					break;
				case calc_BufRate :
					unit->mCalcFunc = (UnitCalcFunc)&ampmix_ak;
					break;
				case calc_ScalarRate :
					unit->mCalcFunc = (UnitCalcFunc)&ampmix_ai;
					break;
			}
			break;
		case calc_BufRate :
			switch (addRate) {
				case calc_FullRate :
					unit->mCalcFunc = (UnitCalcFunc)&ampmix_ka;
					break;
				case calc_BufRate :
					unit->mCalcFunc = (UnitCalcFunc)&ampmix_kk;
					break;
				case calc_ScalarRate :
					unit->mCalcFunc = (UnitCalcFunc)&ampmix_ki;
					break;
			}
			break;
		case calc_ScalarRate :
			switch (addRate) {
				case calc_FullRate :
					unit->mCalcFunc = (UnitCalcFunc)&ampmix_ia;
					break;
				case calc_BufRate :
					unit->mCalcFunc = (UnitCalcFunc)&ampmix_ik;
					break;
				case calc_ScalarRate :
					unit->mCalcFunc = (UnitCalcFunc)&ampmix_ii;
					break;
			}
			break;
		}

#if defined (NOVA_SIMD)
	} else {
		if (BUFLENGTH == 64) {
		switch (mulRate) {
			case calc_FullRate :
				switch (addRate) {
					case calc_FullRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_aa_nova_64;
						break;
					case calc_BufRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ak_nova_64;
						break;
					case calc_ScalarRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ai_nova_64;
						break;
				}
				break;
			case calc_BufRate :
				switch (addRate) {
					case calc_FullRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ka_nova_64;
						break;
					case calc_BufRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_kk_nova;
						break;
					case calc_ScalarRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ki_nova_64;
						break;
				}
				break;
			case calc_ScalarRate :
				switch (addRate) {
					case calc_FullRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ia_nova_64;
						break;
					case calc_BufRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ik_nova_64;
						break;
					case calc_ScalarRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ii_nova_64;
						break;
				}
				break;
		}
		} else {
		switch (mulRate) {
			case calc_FullRate :
				switch (addRate) {
					case calc_FullRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_aa_nova;
						break;
					case calc_BufRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ak_nova;
						break;
					case calc_ScalarRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ai_nova;
						break;
				}
				break;
			case calc_BufRate :
				switch (addRate) {
					case calc_FullRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ka_nova;
						break;
					case calc_BufRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_kk_nova;
						break;
					case calc_ScalarRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ki_nova;
						break;
				}
				break;
			case calc_ScalarRate :
				switch (addRate) {
					case calc_FullRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ia_nova;
						break;
					case calc_BufRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ik_nova;
						break;
					case calc_ScalarRate :
						unit->mCalcFunc = (UnitCalcFunc)&ampmix_ii_nova;
						break;
				}
				break;
		}
		}
    }
#endif
	ampmix_k(unit, 1);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(MulAdd)
{
	ft = inTable;

	DefineSimpleUnit(MulAdd);
}
