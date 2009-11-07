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
#include "simd_unary_arithmetic.hpp"
#include "simd_binary_arithmetic.hpp"
#include "simd_ternary_arithmetic.hpp"
#include "simd_math.hpp"
#include "simd_memory.hpp"
#include "simd_round.hpp"
#include "softclip.hpp"

#define NOVA_WRAPPER(NAME, NOVANAME)                        \
	void NAME##_nova(UnaryOpUGen *unit, int inNumSamples)   \
	{                                                       \
		nova::NOVANAME##_vec_simd(OUT(0), IN(0), inNumSamples); \
	}


#endif


static InterfaceTable *ft;

//////////////////////////////////////////////////////////////////////////////////////////////////


/* special unary math operators */
enum {
	opNeg,
	opNot,
	opIsNil,
	opNotNil,
	opBitNot,
	opAbs,
	opAsFloat,
	opAsInt,
	opCeil,
	opFloor,
	opFrac,
	opSign,
	opSquared,
	opCubed,
	opSqrt,
	opExp,
	opRecip,
	opMIDICPS,
	opCPSMIDI,

	opMIDIRatio,
	opRatioMIDI,
	opDbAmp,
	opAmpDb,
	opOctCPS,
	opCPSOct,
	opLog,
	opLog2,
	opLog10,
	opSin,
	opCos,
	opTan,
	opArcSin,
	opArcCos,
	opArcTan,
	opSinH,
	opCosH,
	opTanH,
	opRand,
	opRand2,
	opLinRand,
	opBiLinRand,

	opSum3Rand,

	opDistort,
	opSoftClip,
	opCoin,

	opDigitValue,
	opSilence,
	opThru,
	opRectWindow,
	opHanWindow,
	opWelchWindow,
	opTriWindow,

	opRamp,
	opSCurve,

	opNumUnarySelectors
};


struct UnaryOpUGen : public Unit
{
};

typedef void (*UnaryOpFunc)(UnaryOpUGen *unit, int inNumSamples);

extern "C"
{
	void load(InterfaceTable *inTable);

	void UnaryOpUGen_Ctor(UnaryOpUGen *unit);

	void invert_a(UnaryOpUGen *unit, int inNumSamples);
	void not_a(UnaryOpUGen *unit, int inNumSamples);
	void zero_a(UnaryOpUGen *unit, int inNumSamples);
	void thru_a(UnaryOpUGen *unit, int inNumSamples);
	void abs_a(UnaryOpUGen *unit, int inNumSamples);
	void recip_a(UnaryOpUGen *unit, int inNumSamples);
	void floor_a(UnaryOpUGen *unit, int inNumSamples);
	void ceil_a(UnaryOpUGen *unit, int inNumSamples);
	void sin_a(UnaryOpUGen *unit, int inNumSamples);
	void cos_a(UnaryOpUGen *unit, int inNumSamples);
	void tan_a(UnaryOpUGen *unit, int inNumSamples);
	void asin_a(UnaryOpUGen *unit, int inNumSamples);
	void acos_a(UnaryOpUGen *unit, int inNumSamples);
	void atan_a(UnaryOpUGen *unit, int inNumSamples);
	void sinh_a(UnaryOpUGen *unit, int inNumSamples);
	void cosh_a(UnaryOpUGen *unit, int inNumSamples);
	void tanh_a(UnaryOpUGen *unit, int inNumSamples);
	void log_a(UnaryOpUGen *unit, int inNumSamples);
	void log2_a(UnaryOpUGen *unit, int inNumSamples);
	void log10_a(UnaryOpUGen *unit, int inNumSamples);
	void exp_a(UnaryOpUGen *unit, int inNumSamples);
	void sqrt_a(UnaryOpUGen *unit, int inNumSamples);
	void ampdb_a(UnaryOpUGen *unit, int inNumSamples);
	void dbamp_a(UnaryOpUGen *unit, int inNumSamples);
	void midicps_a(UnaryOpUGen *unit, int inNumSamples);
	void cpsmidi_a(UnaryOpUGen *unit, int inNumSamples);
	void midiratio_a(UnaryOpUGen *unit, int inNumSamples);
	void ratiomidi_a(UnaryOpUGen *unit, int inNumSamples);
	void cpsoct_a(UnaryOpUGen *unit, int inNumSamples);
	void octcps_a(UnaryOpUGen *unit, int inNumSamples);
	void frac_a(UnaryOpUGen *unit, int inNumSamples);
	void squared_a(UnaryOpUGen *unit, int inNumSamples);
	void cubed_a(UnaryOpUGen *unit, int inNumSamples);
	void sign_a(UnaryOpUGen *unit, int inNumSamples);
	void distort_a(UnaryOpUGen *unit, int inNumSamples);
	void distortneg_a(UnaryOpUGen *unit, int inNumSamples);
	void softclip_a(UnaryOpUGen *unit, int inNumSamples);
	void rectwindow_a(UnaryOpUGen *unit, int inNumSamples);
	void hanwindow_a(UnaryOpUGen *unit, int inNumSamples);
	void welwindow_a(UnaryOpUGen *unit, int inNumSamples);
	void triwindow_a(UnaryOpUGen *unit, int inNumSamples);
	void ramp_a(UnaryOpUGen *unit, int inNumSamples);
	void scurve_a(UnaryOpUGen *unit, int inNumSamples);

	void invert_d(UnaryOpUGen *unit, int inNumSamples);
	void not_d(UnaryOpUGen *unit, int inNumSamples);
	void zero_d(UnaryOpUGen *unit, int inNumSamples);
	void thru_d(UnaryOpUGen *unit, int inNumSamples);
	void abs_d(UnaryOpUGen *unit, int inNumSamples);
	void recip_d(UnaryOpUGen *unit, int inNumSamples);
	void floor_d(UnaryOpUGen *unit, int inNumSamples);
	void ceil_d(UnaryOpUGen *unit, int inNumSamples);
	void sin_d(UnaryOpUGen *unit, int inNumSamples);
	void cos_d(UnaryOpUGen *unit, int inNumSamples);
	void tan_d(UnaryOpUGen *unit, int inNumSamples);
	void asin_d(UnaryOpUGen *unit, int inNumSamples);
	void acos_d(UnaryOpUGen *unit, int inNumSamples);
	void atan_d(UnaryOpUGen *unit, int inNumSamples);
	void sinh_d(UnaryOpUGen *unit, int inNumSamples);
	void cosh_d(UnaryOpUGen *unit, int inNumSamples);
	void tanh_d(UnaryOpUGen *unit, int inNumSamples);
	void log_d(UnaryOpUGen *unit, int inNumSamples);
	void log2_d(UnaryOpUGen *unit, int inNumSamples);
	void log10_d(UnaryOpUGen *unit, int inNumSamples);
	void exp_d(UnaryOpUGen *unit, int inNumSamples);
	void sqrt_d(UnaryOpUGen *unit, int inNumSamples);
	void ampdb_d(UnaryOpUGen *unit, int inNumSamples);
	void dbamp_d(UnaryOpUGen *unit, int inNumSamples);
	void midicps_d(UnaryOpUGen *unit, int inNumSamples);
	void cpsmidi_d(UnaryOpUGen *unit, int inNumSamples);
	void midiratio_d(UnaryOpUGen *unit, int inNumSamples);
	void ratiomidi_d(UnaryOpUGen *unit, int inNumSamples);
	void cpsoct_d(UnaryOpUGen *unit, int inNumSamples);
	void octcps_d(UnaryOpUGen *unit, int inNumSamples);
	void frac_d(UnaryOpUGen *unit, int inNumSamples);
	void squared_d(UnaryOpUGen *unit, int inNumSamples);
	void cubed_d(UnaryOpUGen *unit, int inNumSamples);
	void sign_d(UnaryOpUGen *unit, int inNumSamples);
	void distort_d(UnaryOpUGen *unit, int inNumSamples);
	void distortneg_d(UnaryOpUGen *unit, int inNumSamples);
	void softclip_d(UnaryOpUGen *unit, int inNumSamples);
	void rectwindow_d(UnaryOpUGen *unit, int inNumSamples);
	void hanwindow_d(UnaryOpUGen *unit, int inNumSamples);
	void welwindow_d(UnaryOpUGen *unit, int inNumSamples);
	void triwindow_d(UnaryOpUGen *unit, int inNumSamples);
	void ramp_d(UnaryOpUGen *unit, int inNumSamples);
	void scurve_d(UnaryOpUGen *unit, int inNumSamples);

}

bool ChooseOperatorFunc(UnaryOpUGen *unit);

void UnaryOpUGen_Ctor(UnaryOpUGen *unit)
{	
	bool initialized = ChooseOperatorFunc(unit);
	if (!initialized)
		(unit->mCalcFunc)(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void invert_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = -ZXP(a);
	);
}

#ifdef NOVA_SIMD
void invert_nova(UnaryOpUGen *unit, int inNumSamples)
{
	nova::minus_vec_simd(OUT(0), 0.f, IN(0), inNumSamples);
}
#endif

#if __VEC__
void vinvert_a(UnaryOpUGen *unit, int inNumSamples)
{
	vfloat32 *out = (vfloat32*)OUT(0);
	vfloat32 *a = (vfloat32*)IN(0);

	vint32 neg = (vint32)vinit(0x80000000);

	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		vec_st((vfloat32)vec_xor(neg, (vint32)vec_ld(i, a)), i, out);
	}
}
#endif // __VEC__

void not_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = ZXP(a) > 0.f ? 0.f : 1.f;
	);
}

void zero_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);

	ZClear(inNumSamples, out);
}

void thru_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	ZCopy(inNumSamples, out, a);
}

void abs_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = fabs(ZXP(a));
	);
}

#ifdef NOVA_SIMD
void zero_nova(UnaryOpUGen *unit, int inNumSamples)
{
	nova::zerovec_simd(OUT(0), inNumSamples);
}

void thru_nova(UnaryOpUGen *unit, int inNumSamples)
{
	nova::copyvec_simd(OUT(0), IN(0), inNumSamples);
}

void abs_nova(UnaryOpUGen *unit, int inNumSamples)
{
	nova::abs_vec_simd(OUT(0), IN(0), inNumSamples);
}
#endif

#if __VEC__
void vabs_a(UnaryOpUGen *unit, int inNumSamples)
{
	vfloat32 *out = (vfloat32*)OUT(0);
	vfloat32 *a = (vfloat32*)IN(0);

	vint32 mask = (vint32)vinit(0x7fffffff);

	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		vec_st((vfloat32)vec_and(mask, (vint32)vec_ld(i, a)), i, out);
	}
}
#endif // __VEC__

void recip_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = 1.f / ZXP(a);
	);
}

#ifdef NOVA_SIMD
void recip_nova(UnaryOpUGen *unit, int inNumSamples)
{
	nova::over_vec_simd(OUT(0), 1.f, IN(0), inNumSamples);
}
#endif


#if __VEC__
void vrecip_a(UnaryOpUGen *unit, int inNumSamples)
{
	vfloat32 *out = (vfloat32*)OUT(0);
	vfloat32 *a = (vfloat32*)IN(0);

	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		vec_st(vec_reciprocal(vec_ld(i, a)), i, out);
	}
}
#endif // __VEC__

void floor_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = floor(ZXP(a));
	);
}

#if __VEC__
void vfloor_a(UnaryOpUGen *unit, int inNumSamples)
{
	vfloat32 *out = (vfloat32*)OUT(0);
	vfloat32 *a = (vfloat32*)IN(0);

	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		vec_st(vec_floor(vec_ld(i, a)), i, out);
	}
}
#endif // __VEC__

void ceil_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = ceil(ZXP(a));
	);
}

#if __VEC__
void vceil_a(UnaryOpUGen *unit, int inNumSamples)
{
	vfloat32 *out = (vfloat32*)OUT(0);
	vfloat32 *a = (vfloat32*)IN(0);

	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		vec_st(vec_ceil(vec_ld(i, a)), i, out);
	}
}
#endif // __VEC__

#ifdef NOVA_SIMD
NOVA_WRAPPER(floor, floor)
NOVA_WRAPPER(ceil, ceil)
#endif

void sin_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::sin(ZXP(a));
	);
}

void cos_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::cos(ZXP(a));
	);
}

void tan_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::tan(ZXP(a));
	);
}

void asin_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::asin(ZXP(a));
	);
}

void acos_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::acos(ZXP(a));
	);
}

void atan_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::atan(ZXP(a));
	);
}

void sinh_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::sinh(ZXP(a));
	);
}

void cosh_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::cosh(ZXP(a));
	);
}

void tanh_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::tanh(ZXP(a));
	);
}

#ifdef NOVA_SIMD
NOVA_WRAPPER(sin, sin)
NOVA_WRAPPER(cos, cos)
NOVA_WRAPPER(tan, tan)
NOVA_WRAPPER(asin, asin)
NOVA_WRAPPER(acos, acos)
NOVA_WRAPPER(atan, atan)
NOVA_WRAPPER(tanh, tanh)
#endif



void log_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::log(ZXP(a));
	);
}

void log2_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_log2(ZXP(a));
	);
}

void log10_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_log10(ZXP(a));
	);
}

void exp_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = std::exp(ZXP(a));
	);
}


#ifdef NOVA_SIMD
NOVA_WRAPPER(log, log)
NOVA_WRAPPER(log2, log2)
NOVA_WRAPPER(log10, log10)
NOVA_WRAPPER(exp, exp)
#endif

void sqrt_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_sqrt(ZXP(a));
	);
}

#ifdef NOVA_SIMD
NOVA_WRAPPER(sqrt, ssqrt)
#endif

void ampdb_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_ampdb(ZXP(a));
	);
}

void dbamp_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_dbamp(ZXP(a));
	);
}

void midicps_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_midicps(ZXP(a));
	);
}

void cpsmidi_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_cpsmidi(ZXP(a));
	);
}

void midiratio_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_midiratio(ZXP(a));
	);
}

void ratiomidi_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_ratiomidi(ZXP(a));
	);
}

void cpsoct_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_cpsoct(ZXP(a));
	);
}

void octcps_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_octcps(ZXP(a));
	);
}

void frac_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		float xa = ZXP(a);
		ZXP(out) = xa - std::floor(xa);
	);
}

#ifdef NOVA_SIMD
NOVA_WRAPPER(frac, frac)
#endif

#if __VEC__
void vfrac_a(UnaryOpUGen *unit, int inNumSamples)
{
	vfloat32 *out = (vfloat32*)OUT(0);
	vfloat32 *a = (vfloat32*)IN(0);

	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		vfloat32 z = vec_ld(i, a);
		vec_st(vec_sub(z, vec_floor(z)), i, out);
	}
}
#endif // __VEC__

void squared_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		float xa = ZXP(a);
		ZXP(out) = xa * xa;
	);
}

#ifdef NOVA_SIMD
void squared_nova(UnaryOpUGen *unit, int inNumSamples)
{
	nova::square_vec_simd(OUT(0), IN(0), inNumSamples);
}
#endif

#if __VEC__
void vsquared_a(UnaryOpUGen *unit, int inNumSamples)
{
	vfloat32 *out = (vfloat32*)OUT(0);
	vfloat32 *a = (vfloat32*)IN(0);
	define_vzero;

	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		vfloat32 z = vec_ld(i, a);
		vec_st(vec_mul(z, z), i, out);
	}
}
#endif // __VEC__

void cubed_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		float xa = ZXP(a);
		ZXP(out) = xa * xa * xa;
	);
}

#if __VEC__
void vcubed_a(UnaryOpUGen *unit, int inNumSamples)
{
	vfloat32 *out = (vfloat32*)OUT(0);
	vfloat32 *a = (vfloat32*)IN(0);
	define_vzero;

	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		vfloat32 z = vec_ld(i, a);
		vec_st(vec_mul(z, vec_mul(z, z)), i, out);
	}
}
#endif // __VEC__

#ifdef NOVA_SIMD
void cubed_nova(UnaryOpUGen *unit, int inNumSamples)
{
	nova::cube_vec_simd(OUT(0), IN(0), inNumSamples);
}
#endif

void sign_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		float xa = ZXP(a);
		ZXP(out) = xa < 0.f ? -1.f : (xa > 0.f ? 1.f : 0.f);
	);
}

#ifdef NOVA_SIMD
void sign_nova(UnaryOpUGen *unit, int inNumSamples)
{
	nova::sgn_vec_simd(OUT(0), IN(0), inNumSamples);
}
#endif

void distort_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	/*LOOP(inNumSamples,
		float z = ZXP(a);
		if (z < 0.f) ZXP(out) = z/(1. - z);
		else ZXP(out) = z/(1. + z);
	);*/

	LOOP(inNumSamples,
		float z = ZXP(a);
		ZXP(out) = z/(1.f + std::abs(z));
	);
}

#if __VEC__
void vdistort_a(UnaryOpUGen *unit, int inNumSamples)
{
	vfloat32 *out = (vfloat32*)OUT(0);
	vfloat32 *a = (vfloat32*)IN(0);
	define_vzero;
	define_vones;
	vint32 mask = (vint32)vinit(0x7fffffff);

	int len = inNumSamples << 2;
	for (int i=0; i<len; i+=16) {
		vfloat32 z = vec_ld(i, a);
		vec_st(vec_div(z, vec_add(vones, (vfloat32)vec_and(mask, (vint32)z))), i, out);
	}
}
#endif // __VEC__

void distortneg_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		float z = ZXP(a);
		if (z < 0.f) ZXP(out) = z/(1.f - z);
		else ZXP(out) = z;
	);
}

void softclip_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	/*LOOP(inNumSamples,
		float z = ZXP(a);
		if (z < -0.5) ZXP(out) = (-z - .25)/z;
		else if (z > 0.5) ZXP(out) = (z - .25)/z;
		else ZXP(out) = z;
	);*/

	LOOP(inNumSamples,
		float z = ZXP(a);
		float az = fabs(z);
		if (az > 0.5f) ZXP(out) = (az - .25f)/z;
		else ZXP(out) = z;
	);
}

#ifdef NOVA_SIMD
NOVA_WRAPPER(softclip, softclip)
#endif

void rectwindow_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_rectwindow(ZXP(a));
	);
}

void hanwindow_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_hanwindow(ZXP(a));
	);
}

void welwindow_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_welwindow(ZXP(a));
	);
}

void triwindow_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		float x = ZXP(a);
		float z;
		if (x < 0.f || x > 1.f) z = 0.f;
		if (x < .5f) z = 2.f * x;
		else z = 2.f - 2.f * x;
		ZXP(out) = z;
	);
}


void scurve_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_scurve(ZXP(a));
	);
}

void ramp_a(UnaryOpUGen *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *a = ZIN(0);

	LOOP(inNumSamples,
		ZXP(out) = sc_ramp(ZXP(a));
	);
}

#ifdef NOVA_SIMD
void ramp_nova(UnaryOpUGen *unit, int inNumSamples)
{
	nova::clip_vec_simd(OUT(0), IN(0), 0.f, 1.f, inNumSamples);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////


void invert_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : -(x);
	} else {
		RESETINPUT(0);
	}
}

void not_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : (x > 0.f ? 0.f : 1.f);
	} else {
		RESETINPUT(0);
	}
}

void zero_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : 0.f;
	} else {
		RESETINPUT(0);
	}
}

void thru_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : (x);
	} else {
		RESETINPUT(0);
	}
}

void abs_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::abs(x);
	} else {
		RESETINPUT(0);
	}
}

void recip_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : 1.f / x;
	} else {
		RESETINPUT(0);
	}
}

void floor_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::floor(x);
	} else {
		RESETINPUT(0);
	}
}

void ceil_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::ceil(x);
	} else {
		RESETINPUT(0);
	}
}

void sin_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::sin(x);
	} else {
		RESETINPUT(0);
	}
}

void cos_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::cos(x);
	} else {
		RESETINPUT(0);
	}
}

void tan_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::tan(x);
	} else {
		RESETINPUT(0);
	}
}

void asin_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::asin(x);
	} else {
		RESETINPUT(0);
	}
}

void acos_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::acos(x);
	} else {
		RESETINPUT(0);
	}
}

void atan_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : atan(x);
	} else {
		RESETINPUT(0);
	}
}

void sinh_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::sinh(x);
	} else {
		RESETINPUT(0);
	}
}

void cosh_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::cosh(x);
	} else {
		RESETINPUT(0);
	}
}

void tanh_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::tanh(x);
	} else {
		RESETINPUT(0);
	}
}

void log_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::log(x);
	} else {
		RESETINPUT(0);
	}
}

void log2_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_log2(x);
	} else {
		RESETINPUT(0);
	}
}

void log10_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_log10(x);
	} else {
		RESETINPUT(0);
	}
}

void exp_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : std::exp(x);
	} else {
		RESETINPUT(0);
	}
}

void sqrt_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_sqrt(x);
	} else {
		RESETINPUT(0);
	}
}

void ampdb_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_ampdb(x);
	} else {
		RESETINPUT(0);
	}
}

void dbamp_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_dbamp(x);
	} else {
		RESETINPUT(0);
	}
}

void midicps_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_midicps(x);
	} else {
		RESETINPUT(0);
	}
}

void cpsmidi_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_cpsmidi(x);
	} else {
		RESETINPUT(0);
	}
}

void midiratio_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_midiratio(x);
	} else {
		RESETINPUT(0);
	}
}

void ratiomidi_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_ratiomidi(x);
	} else {
		RESETINPUT(0);
	}
}

void cpsoct_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_cpsoct(x);
	} else {
		RESETINPUT(0);
	}
}

void octcps_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_octcps(x);
	} else {
		RESETINPUT(0);
	}
}

void frac_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_frac(x);
	} else {
		RESETINPUT(0);
	}
}

void squared_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : x*x;
	} else {
		RESETINPUT(0);
	}
}

void cubed_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : x*x*x;
	} else {
		RESETINPUT(0);
	}
}

void sign_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : (x < 0.f ? -1.f : (x > 0.f ? 1.f : 0.f));
	} else {
		RESETINPUT(0);
	}
}

void distort_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_distort(x);
	} else {
		RESETINPUT(0);
	}
}

void distortneg_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : (x < 0.f ? x/(1.f-x) : x);
	} else {
		RESETINPUT(0);
	}
}

void softclip_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_softclip(x);
	} else {
		RESETINPUT(0);
	}
}

void rectwindow_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_rectwindow(x);
	} else {
		RESETINPUT(0);
	}
}

void hanwindow_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_hanwindow(x);
	} else {
		RESETINPUT(0);
	}
}

void welwindow_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_welwindow(x);
	} else {
		RESETINPUT(0);
	}
}

void triwindow_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_triwindow(x);
	} else {
		RESETINPUT(0);
	}
}

void ramp_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_ramp(x);
	} else {
		RESETINPUT(0);
	}
}

void scurve_d(UnaryOpUGen *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		OUT0(0) = sc_isnan(x) ? NAN : sc_scurve(x);
	} else {
		RESETINPUT(0);
	}
}



////////////////////////////////////////////////////////////////////////////////////////////////////////


UnaryOpFunc ChooseNormalFunc(UnaryOpUGen *unit)
{
	void (*func)(UnaryOpUGen *unit, int inNumSamples);

	switch (unit->mSpecialIndex) {
		case opSilence : func = &zero_a; break;
		case opThru : func = &thru_a; break;
		case opNeg : func = &invert_a; break;
		case opNot : func = &not_a; break;
		case opAbs : func = &abs_a; break;
		case opCeil : func = &ceil_a; break;
		case opFloor : func = &floor_a; break;
		case opFrac : func = &frac_a; break;
		case opSign : func = &sign_a; break;
		case opSquared : func = &squared_a; break;
		case opCubed : func = &cubed_a; break;
		case opSqrt : 		func = &sqrt_a; break;
		case opExp : func = &exp_a; break;
		case opRecip : func = &recip_a; break;
		case opMIDICPS : func = &midicps_a; break;
		case opCPSMIDI : func = &cpsmidi_a; break;

		case opMIDIRatio : func = &midiratio_a; break;
		case opRatioMIDI : func = &ratiomidi_a; break;
		case opDbAmp : func = &dbamp_a; break;
		case opAmpDb : 	func = &ampdb_a; break;
		case opOctCPS : func = &octcps_a; break;
		case opCPSOct : func = &cpsoct_a; break;
		case opLog : func = &log_a; break;
		case opLog2 : func = &log2_a; break;
		case opLog10 : func = &log10_a; break;
		case opSin : func = &sin_a; break;
		case opCos : func = &cos_a; break;
		case opTan : func = &tan_a; break;
		case opArcSin : func = &asin_a; break;
		case opArcCos : func = &acos_a; break;
		case opArcTan : func = &atan_a; break;
		case opSinH : func = &sinh_a; break;
		case opCosH : func = &cosh_a; break;
		case opTanH : func = &tanh_a; break;

		case opDistort : func = &distort_a; break;
		case opSoftClip : func = &softclip_a; break;

		case opRectWindow : func = &rectwindow_a; break;
		case opHanWindow : func = &hanwindow_a; break;
		case opWelchWindow : func = &welwindow_a; break;
		case opTriWindow : func = &triwindow_a; break;

		case opSCurve : func = &scurve_a; break;
		case opRamp : func = &ramp_a; break;

		default : func = &thru_a; break;
	}
	return func;
}


UnaryOpFunc ChooseDemandFunc(UnaryOpUGen *unit)
{
	void (*func)(UnaryOpUGen *unit, int inNumSamples);

	switch (unit->mSpecialIndex) {
		case opSilence : func = &zero_d; break;
		case opThru : func = &thru_d; break;
		case opNeg : func = &invert_d; break;
		case opNot : func = &not_d; break;
		case opAbs : func = &abs_d; break;
		case opCeil : func = &ceil_d; break;
		case opFloor : func = &floor_d; break;
		case opFrac : func = &frac_d; break;
		case opSign : func = &sign_d; break;
		case opSquared : func = &squared_d; break;
		case opCubed : func = &cubed_d; break;
		case opSqrt : 		func = &sqrt_d; break;
		case opExp : func = &exp_d; break;
		case opRecip : func = &recip_d; break;
		case opMIDICPS : func = &midicps_d; break;
		case opCPSMIDI : func = &cpsmidi_d; break;

		case opMIDIRatio : func = &midiratio_d; break;
		case opRatioMIDI : func = &ratiomidi_d; break;
		case opDbAmp : func = &dbamp_d; break;
		case opAmpDb : 	func = &ampdb_d; break;
		case opOctCPS : func = &octcps_d; break;
		case opCPSOct : func = &cpsoct_d; break;
		case opLog : func = &log_d; break;
		case opLog2 : func = &log2_d; break;
		case opLog10 : func = &log10_d; break;
		case opSin : func = &sin_d; break;
		case opCos : func = &cos_d; break;
		case opTan : func = &tan_d; break;
		case opArcSin : func = &asin_d; break;
		case opArcCos : func = &acos_d; break;
		case opArcTan : func = &atan_d; break;
		case opSinH : func = &sinh_d; break;
		case opCosH : func = &cosh_d; break;
		case opTanH : func = &tanh_d; break;

		case opDistort : func = &distort_d; break;
		case opSoftClip : func = &softclip_d; break;

		case opRectWindow : func = &rectwindow_d; break;
		case opHanWindow : func = &hanwindow_d; break;
		case opWelchWindow : func = &welwindow_d; break;
		case opTriWindow : func = &triwindow_d; break;

		case opSCurve : func = &scurve_d; break;
		case opRamp : func = &ramp_d; break;

		default : func = &thru_d; break;
	}
	return func;
}

#if __VEC__
UnaryOpFunc ChooseVectorFunc(UnaryOpUGen *unit)
{
	void (*func)(UnaryOpUGen *unit, int inNumSamples);

	switch (unit->mSpecialIndex) {
		case opSilence : func = &zero_a; break;
		case opThru : func = &thru_a; break;
		case opNeg : func = &vinvert_a; break;
		case opNot : func = &not_a; break;
		case opAbs : func = &vabs_a; break;
		case opCeil : func = &vceil_a; break;
		case opFloor : func = &vfloor_a; break;
		case opFrac : func = &vfrac_a; break;
		case opSign : func = &sign_a; break;
		case opSquared : func = &vsquared_a; break;
		case opCubed : func = &vcubed_a; break;
		case opSqrt : 		func = &sqrt_a; break;
		case opExp : func = &exp_a; break;
		case opRecip : func = &vrecip_a; break;
		case opMIDICPS : func = &midicps_a; break;
		case opCPSMIDI : func = &cpsmidi_a; break;

		case opMIDIRatio : func = &midiratio_a; break;
		case opRatioMIDI : func = &ratiomidi_a; break;
		case opDbAmp : func = &dbamp_a; break;
		case opAmpDb : 	func = &ampdb_a; break;
		case opOctCPS : func = &octcps_a; break;
		case opCPSOct : func = &cpsoct_a; break;
		case opLog : func = &log_a; break;
		case opLog2 : func = &log2_a; break;
		case opLog10 : func = &log10_a; break;
		case opSin : func = &sin_a; break;
		case opCos : func = &cos_a; break;
		case opTan : func = &tan_a; break;
		case opArcSin : func = &asin_a; break;
		case opArcCos : func = &acos_a; break;
		case opArcTan : func = &atan_a; break;
		case opSinH : func = &sinh_a; break;
		case opCosH : func = &cosh_a; break;
		case opTanH : func = &tanh_a; break;

		case opDistort : func = &vdistort_a; break;
		case opSoftClip : func = &softclip_a; break;

		case opRectWindow : func = &rectwindow_a; break;
		case opHanWindow : func = &hanwindow_a; break;
		case opWelchWindow : func = &welwindow_a; break;
		case opTriWindow : func = &triwindow_a; break;

		case opSCurve : func = &scurve_a; break;
		case opRamp : func = &ramp_a; break;

		default : func = &thru_a; break;
	}
	return func;
}
#endif // __VEC__

#ifdef NOVA_SIMD
UnaryOpFunc ChooseNovaSimdFunc(UnaryOpUGen *unit)
{
	void (*func)(UnaryOpUGen *unit, int inNumSamples);

	switch (unit->mSpecialIndex) {
		case opSilence : func = &zero_nova; break;
		case opThru : func = &thru_nova; break;
		case opNeg : func = &invert_nova; break;
		case opNot : func = &not_a; break;
		case opAbs : func = &abs_nova; break;
		case opCeil : func = &ceil_nova; break;
		case opFloor : func = &floor_nova; break;
		case opFrac : func = &frac_nova; break;
		case opSign : func = &sign_nova; break;
		case opSquared : func = &squared_nova; break;
		case opCubed : func = &cubed_nova; break;
		case opSqrt : func = &sqrt_nova; break;
		case opExp : func = &exp_nova; break;
		case opRecip : func = &recip_nova; break;
		case opMIDICPS : func = &midicps_a; break;
		case opCPSMIDI : func = &cpsmidi_a; break;

		case opMIDIRatio : func = &midiratio_a; break;
		case opRatioMIDI : func = &ratiomidi_a; break;	
		case opDbAmp : func = &dbamp_a; break;
		case opAmpDb : 	func = &ampdb_a; break;
		case opOctCPS : func = &octcps_a; break;
		case opCPSOct : func = &cpsoct_a; break;
		case opLog : func = &log_nova; break;
		case opLog2 : func = &log2_nova; break;
		case opLog10 : func = &log10_nova; break;
		case opSin : func = &sin_nova; break;
		case opCos : func = &cos_nova; break;
		case opTan : func = &tan_nova; break;
		case opArcSin : func = &asin_nova; break;
		case opArcCos : func = &acos_nova; break;
		case opArcTan : func = &atan_nova; break;
		case opSinH : func = &sinh_a; break;
		case opCosH : func = &cosh_a; break;
		case opTanH : func = &tanh_nova; break;

		case opDistort : func = &distort_a; break;
		case opSoftClip : func = &softclip_nova; break;

		case opRectWindow : func = &rectwindow_a; break;
		case opHanWindow : func = &hanwindow_a; break;
		case opWelchWindow : func = &welwindow_a; break;
		case opTriWindow : func = &triwindow_a; break;

		case opSCurve : func = &scurve_a; break;
		case opRamp : func = &ramp_nova; break;

		default : func = &thru_nova; break;
	}
	return func;
}
#endif

bool ChooseOperatorFunc(UnaryOpUGen *unit)
{
	//Print("->ChooseOperatorFunc %d\n", unit->mSpecialIndex);
	UnaryOpFunc func;
	bool ret = false;
	if (unit->mCalcRate == calc_DemandRate) {
		func = ChooseDemandFunc(unit);
	} else if (BUFLENGTH == 1) {
		func = ChooseNormalFunc(unit);
#if __VEC__
	} else if (USEVEC) {
		func = ChooseVectorFunc(unit);
#elif defined(NOVA_SIMD)
	} else if (!(BUFLENGTH & 15)) {
		/* select normal function for initialization */
		func = ChooseNormalFunc(unit);
		func(unit, 1);

		/* select simd function */
		func = ChooseNovaSimdFunc(unit);
		ret = true;
#endif
	} else {
		func = ChooseNormalFunc(unit);
	}
	unit->mCalcFunc = (UnitCalcFunc)func;
	//Print("<-ChooseOperatorFunc %08X\n", func);
	//Print("calc %d\n", unit->mCalcRate);
	return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(UnaryOp)
{
	ft = inTable;

	DefineSimpleUnit(UnaryOpUGen);
}
