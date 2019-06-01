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

#include <boost/utility/enable_if.hpp>
#include <boost/align/is_aligned.hpp>

#ifdef NOVA_SIMD
#    include "simd_unary_arithmetic.hpp"
#    include "simd_binary_arithmetic.hpp"
#    include "simd_ternary_arithmetic.hpp"
#    include "simd_math.hpp"
#    include "simd_memory.hpp"
#    include "softclip.hpp"
#    include "simd_unit_conversion.hpp"

#    include "function_attributes.h"

using nova::wrap_argument;

#    define NOVA_WRAPPER(NAME, NOVANAME)                                                                               \
        FLATTEN void NAME##_nova(UnaryOpUGen* unit, int inNumSamples) {                                                \
            nova::NOVANAME##_vec_simd(OUT(0), IN(0), inNumSamples);                                                    \
        }

#    define NOVA_WRAPPER_CT_UNROLL(NAME, NOVANAME)                                                                     \
        FLATTEN void NAME##_nova(UnaryOpUGen* unit, int inNumSamples) {                                                \
            nova::NOVANAME##_vec_simd(OUT(0), IN(0), inNumSamples);                                                    \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void NAME##_nova_64(UnaryOpUGen* unit, int inNumSamples) {                                             \
            nova::NOVANAME##_vec_simd<64>(OUT(0), IN(0));                                                              \
        }

struct sc_distort_functor {
    template <typename FloatType> inline FloatType operator()(FloatType arg) const { return sc_distort(arg); }

    template <typename FloatType> inline nova::vec<FloatType> operator()(nova::vec<FloatType> arg) const {
        nova::vec<FloatType> one(1.f);
        return arg * reciprocal(one + abs(arg));
    }
};

struct sc_scurve_functor {
    template <typename FloatType> inline FloatType operator()(FloatType arg) const { return sc_scurve(arg); }

    template <typename FloatType> inline nova::vec<FloatType> operator()(nova::vec<FloatType> arg) const {
        return perform(arg);
    }

    template <typename VecType>
    inline typename boost::disable_if_c<VecType::has_compare_bitmask, VecType>::type perform(VecType arg) const {
        typedef VecType vec;

        vec result;
        for (int i = 0; i != result.size; ++i)
            result.set(i, sc_scurve(arg.get(i)));
        return result;
    }

    template <typename VecType>
    inline typename boost::enable_if_c<VecType::has_compare_bitmask, VecType>::type perform(VecType arg) const {
        typedef VecType vec;
        vec one(1.f);
        vec zero(0.f);
        vec two(2.f);
        vec three(3.f);

        vec result = (arg * arg) * (three - (two * arg));

        vec boundLow = mask_lt(arg, zero);
        vec boundHigh = mask_gt(arg, one);

        result = select(result, zero, boundLow);
        result = select(result, one, boundHigh);

        return result;
    }
};


namespace nova {
NOVA_SIMD_DEFINE_UNARY_WRAPPER(distort, sc_distort_functor)
NOVA_SIMD_DEFINE_UNARY_WRAPPER(scurve, sc_scurve_functor)
}
#endif

using namespace std; // for math functions

static InterfaceTable* ft;

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


struct UnaryOpUGen : public Unit {};

typedef void (*UnaryOpFunc)(UnaryOpUGen* unit, int inNumSamples);

extern "C" {

void UnaryOpUGen_Ctor(UnaryOpUGen* unit);
}

bool ChooseOperatorFunc(UnaryOpUGen* unit);

void UnaryOpUGen_Ctor(UnaryOpUGen* unit) {
    bool initialized = ChooseOperatorFunc(unit);
    if (!initialized)
        (unit->mCalcFunc)(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFINE_UNARY_OP_FUNCS(name, function)                                                                          \
    extern "C" void name##_a(UnaryOpUGen* unit, int inNumSamples) {                                                    \
        float* out = ZOUT(0);                                                                                          \
        float* a = ZIN(0);                                                                                             \
                                                                                                                       \
        LOOP1(inNumSamples, ZXP(out) = function(ZXP(a)););                                                             \
    }                                                                                                                  \
                                                                                                                       \
    extern "C" void name##_1(UnaryOpUGen* unit, int inNumSamples) { ZOUT0(0) = function(ZIN0(0)); }                    \
                                                                                                                       \
    extern "C" void name##_d(UnaryOpUGen* unit, int inNumSamples) {                                                    \
        if (inNumSamples) {                                                                                            \
            float x = DEMANDINPUT_A(0, inNumSamples);                                                                  \
            OUT0(0) = sc_isnan(x) ? NAN : function(x);                                                                 \
        } else {                                                                                                       \
            RESETINPUT(0);                                                                                             \
        }                                                                                                              \
    }

template <typename F> inline F sc_invert(F x) { return -x; }

DEFINE_UNARY_OP_FUNCS(invert, sc_invert)

#ifdef NOVA_SIMD
FLATTEN void invert_nova(UnaryOpUGen* unit, int inNumSamples) {
    nova::minus_vec_simd(OUT(0), 0.f, IN(0), inNumSamples);
}

FLATTEN void invert_nova_64(UnaryOpUGen* unit, int inNumSamples) { nova::minus_vec_simd<64>(OUT(0), 0.f, IN(0)); }

#endif


template <typename F> inline F sc_not(F x) { return x > 0.f ? 0.f : 1.f; }

DEFINE_UNARY_OP_FUNCS(not, sc_not)
DEFINE_UNARY_OP_FUNCS(bitNot, sc_bitNot)


void zero_a(UnaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);

    ZClear(inNumSamples, out);
}

void thru_a(UnaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);

    ZCopy(inNumSamples, out, a);
}

DEFINE_UNARY_OP_FUNCS(abs, std::abs)

#ifdef NOVA_SIMD
FLATTEN void zero_nova(UnaryOpUGen* unit, int inNumSamples) { nova::zerovec_simd(OUT(0), inNumSamples); }

FLATTEN void zero_nova_64(UnaryOpUGen* unit, int inNumSamples) { nova::zerovec_simd<64>(OUT(0)); }

FLATTEN void thru_nova(UnaryOpUGen* unit, int inNumSamples) { nova::copyvec_simd(OUT(0), IN(0), inNumSamples); }

FLATTEN void thru_nova_64(UnaryOpUGen* unit, int inNumSamples) { nova::copyvec_simd<64>(OUT(0), IN(0)); }

FLATTEN void abs_nova(UnaryOpUGen* unit, int inNumSamples) { nova::abs_vec_simd(OUT(0), IN(0), inNumSamples); }

FLATTEN void abs_nova_64(UnaryOpUGen* unit, int inNumSamples) { nova::abs_vec_simd<64>(OUT(0), IN(0)); }
#endif

DEFINE_UNARY_OP_FUNCS(recip, sc_reciprocal)

#ifdef NOVA_SIMD
FLATTEN void recip_nova(UnaryOpUGen* unit, int inNumSamples) { nova::reciprocal_vec_simd(OUT(0), IN(0), inNumSamples); }

FLATTEN void recip_nova_64(UnaryOpUGen* unit, int inNumSamples) { nova::reciprocal_vec_simd<64>(OUT(0), IN(0)); }
#endif


DEFINE_UNARY_OP_FUNCS(floor, floor)
DEFINE_UNARY_OP_FUNCS(ceil, ceil)

#ifdef NOVA_SIMD
NOVA_WRAPPER_CT_UNROLL(floor, floor)
NOVA_WRAPPER_CT_UNROLL(ceil, ceil)
#endif

DEFINE_UNARY_OP_FUNCS(sin, sin)
DEFINE_UNARY_OP_FUNCS(cos, cos)
DEFINE_UNARY_OP_FUNCS(tan, tan)

DEFINE_UNARY_OP_FUNCS(asin, asin)
DEFINE_UNARY_OP_FUNCS(acos, acos)
DEFINE_UNARY_OP_FUNCS(atan, atan)

DEFINE_UNARY_OP_FUNCS(sinh, sinh)
DEFINE_UNARY_OP_FUNCS(cosh, cosh)
DEFINE_UNARY_OP_FUNCS(tanh, tanh)


#ifdef NOVA_SIMD
NOVA_WRAPPER(sin, sin)
NOVA_WRAPPER(cos, cos)
NOVA_WRAPPER(tan, tan)
NOVA_WRAPPER(asin, asin)
NOVA_WRAPPER(acos, acos)
NOVA_WRAPPER(atan, atan)
NOVA_WRAPPER(tanh, tanh)
#endif


DEFINE_UNARY_OP_FUNCS(log, std::log)
DEFINE_UNARY_OP_FUNCS(log2, sc_log2)
DEFINE_UNARY_OP_FUNCS(log10, sc_log10)
DEFINE_UNARY_OP_FUNCS(exp, exp)

#ifdef NOVA_SIMD
NOVA_WRAPPER(log, log)
NOVA_WRAPPER(log2, log2)
NOVA_WRAPPER(log10, log10)
NOVA_WRAPPER(exp, exp)
#endif

DEFINE_UNARY_OP_FUNCS(sqrt, sc_sqrt)

#ifdef NOVA_SIMD
NOVA_WRAPPER_CT_UNROLL(sqrt, signed_sqrt)
#endif

DEFINE_UNARY_OP_FUNCS(ampdb, sc_ampdb)
DEFINE_UNARY_OP_FUNCS(dbamp, sc_dbamp)
DEFINE_UNARY_OP_FUNCS(midicps, sc_midicps)
DEFINE_UNARY_OP_FUNCS(cpsmidi, sc_cpsmidi)
DEFINE_UNARY_OP_FUNCS(midiratio, sc_midiratio)
DEFINE_UNARY_OP_FUNCS(ratiomidi, sc_ratiomidi)
DEFINE_UNARY_OP_FUNCS(cpsoct, sc_cpsoct)
DEFINE_UNARY_OP_FUNCS(octcps, sc_octcps)

#ifdef NOVA_SIMD
NOVA_WRAPPER(ampdb, amp2db)
NOVA_WRAPPER(dbamp, db2amp)
NOVA_WRAPPER(midicps, midi2freq)
NOVA_WRAPPER(cpsmidi, freq2midi)
NOVA_WRAPPER(midiratio, midi2ratio)
NOVA_WRAPPER(ratiomidi, ratio2midi)
NOVA_WRAPPER(cpsoct, freq2oct)
NOVA_WRAPPER(octcps, oct2freq)
#endif


DEFINE_UNARY_OP_FUNCS(frac, sc_frac)
#ifdef NOVA_SIMD
NOVA_WRAPPER_CT_UNROLL(frac, frac)
#endif


DEFINE_UNARY_OP_FUNCS(squared, sc_squared)
#ifdef NOVA_SIMD
NOVA_WRAPPER_CT_UNROLL(squared, square)
#endif


DEFINE_UNARY_OP_FUNCS(cubed, sc_cubed)
#ifdef NOVA_SIMD
NOVA_WRAPPER_CT_UNROLL(cubed, cube)
#endif

DEFINE_UNARY_OP_FUNCS(sign, sc_sign)

#ifdef NOVA_SIMD
NOVA_WRAPPER_CT_UNROLL(sign, sgn)
#endif

DEFINE_UNARY_OP_FUNCS(distort, sc_distort)

#ifdef NOVA_SIMD
NOVA_WRAPPER_CT_UNROLL(distort, distort)
#endif

DEFINE_UNARY_OP_FUNCS(distortneg, sc_distortneg)
DEFINE_UNARY_OP_FUNCS(softclip, sc_softclip)

#ifdef NOVA_SIMD
NOVA_WRAPPER_CT_UNROLL(softclip, softclip)
#endif

DEFINE_UNARY_OP_FUNCS(rectwindow, sc_rectwindow)
DEFINE_UNARY_OP_FUNCS(hanwindow, sc_hanwindow)
DEFINE_UNARY_OP_FUNCS(welwindow, sc_welwindow)
DEFINE_UNARY_OP_FUNCS(triwindow, sc_triwindow)

DEFINE_UNARY_OP_FUNCS(scurve, sc_scurve)
#ifdef NOVA_SIMD
NOVA_WRAPPER_CT_UNROLL(scurve, scurve)
#endif

DEFINE_UNARY_OP_FUNCS(ramp, sc_ramp)


#ifdef NOVA_SIMD
FLATTEN void ramp_nova(UnaryOpUGen* unit, int inNumSamples) {
    nova::clip_vec_simd(OUT(0), wrap_argument(IN(0)), wrap_argument(0.f), wrap_argument(1.f), inNumSamples);
}

FLATTEN void ramp_nova_64(UnaryOpUGen* unit, int inNumSamples) {
    nova::clip_vec_simd<64>(OUT(0), wrap_argument(IN(0)), wrap_argument(0.f), wrap_argument(1.f));
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////

void zero_d(UnaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float x = DEMANDINPUT_A(0, inNumSamples);
        OUT0(0) = sc_isnan(x) ? NAN : 0.f;
    } else {
        RESETINPUT(0);
    }
}

void thru_d(UnaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float x = DEMANDINPUT_A(0, inNumSamples);
        OUT0(0) = sc_isnan(x) ? NAN : (x);
    } else {
        RESETINPUT(0);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

#define DEFINE_UNARY_OP_RANDOM_FUNCS(name, function)                                                                   \
    extern "C" void name##_a(UnaryOpUGen* unit, int inNumSamples) {                                                    \
        float* out = ZOUT(0);                                                                                          \
        float* a = ZIN(0);                                                                                             \
        RGen& rgen = *unit->mParent->mRGen;                                                                            \
        LOOP1(inNumSamples, ZXP(out) = rgen.function() * ZXP(a););                                                     \
    }                                                                                                                  \
                                                                                                                       \
    extern "C" void name##_1(UnaryOpUGen* unit, int inNumSamples) {                                                    \
        RGen& rgen = *unit->mParent->mRGen;                                                                            \
        ZOUT0(0) = rgen.function() * ZIN0(0);                                                                          \
    }                                                                                                                  \
                                                                                                                       \
    extern "C" void name##_d(UnaryOpUGen* unit, int inNumSamples) {                                                    \
        if (inNumSamples) {                                                                                            \
            float x = DEMANDINPUT_A(0, inNumSamples);                                                                  \
            RGen& rgen = *unit->mParent->mRGen;                                                                        \
            OUT0(0) = sc_isnan(x) ? NAN : (rgen.function() * x);                                                       \
        } else {                                                                                                       \
            RESETINPUT(0);                                                                                             \
        }                                                                                                              \
    }

DEFINE_UNARY_OP_RANDOM_FUNCS(rand, frand);
DEFINE_UNARY_OP_RANDOM_FUNCS(rand2, frand2);
DEFINE_UNARY_OP_RANDOM_FUNCS(linrand, flinrand);
DEFINE_UNARY_OP_RANDOM_FUNCS(bilinrand, fbilinrand);
DEFINE_UNARY_OP_RANDOM_FUNCS(sum3rand, fsum3rand);


void coin_a(UnaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    RGen& rgen = *unit->mParent->mRGen;
    LOOP1(inNumSamples, ZXP(out) = (rgen.frand() < ZXP(a)) ? 1.f : 0.f;);
}

void coin_1(UnaryOpUGen* unit, int inNumSamples) {
    RGen& rgen = *unit->mParent->mRGen;
    float x = ZIN0(0);
    ZOUT0(0) = (rgen.frand() < x) ? 1.f : 0.f;
}

void coin_d(UnaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float x = DEMANDINPUT_A(0, inNumSamples);
        RGen& rgen = *unit->mParent->mRGen;
        float val = (rgen.frand() < x) ? 1.f : 0.f;
        OUT0(0) = sc_isnan(x) ? NAN : val;
    } else {
        RESETINPUT(0);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

static UnaryOpFunc ChooseNormalFunc(UnaryOpUGen* unit) {
    void (*func)(UnaryOpUGen * unit, int inNumSamples);

    switch (unit->mSpecialIndex) {
    case opSilence:
        func = &zero_a;
        break;
    case opThru:
        func = &thru_a;
        break;
    case opNeg:
        func = &invert_a;
        break;
    case opNot:
        func = &not_a;
        break;
    case opBitNot:
        func = &bitNot_a;
        break;
    case opAbs:
        func = &abs_a;
        break;
    case opCeil:
        func = &ceil_a;
        break;
    case opFloor:
        func = &floor_a;
        break;
    case opFrac:
        func = &frac_a;
        break;
    case opSign:
        func = &sign_a;
        break;
    case opSquared:
        func = &squared_a;
        break;
    case opCubed:
        func = &cubed_a;
        break;
    case opSqrt:
        func = &sqrt_a;
        break;
    case opExp:
        func = &exp_a;
        break;
    case opRecip:
        func = &recip_a;
        break;
    case opMIDICPS:
        func = &midicps_a;
        break;
    case opCPSMIDI:
        func = &cpsmidi_a;
        break;

    case opMIDIRatio:
        func = &midiratio_a;
        break;
    case opRatioMIDI:
        func = &ratiomidi_a;
        break;
    case opDbAmp:
        func = &dbamp_a;
        break;
    case opAmpDb:
        func = &ampdb_a;
        break;
    case opOctCPS:
        func = &octcps_a;
        break;
    case opCPSOct:
        func = &cpsoct_a;
        break;
    case opLog:
        func = &log_a;
        break;
    case opLog2:
        func = &log2_a;
        break;
    case opLog10:
        func = &log10_a;
        break;
    case opSin:
        func = &sin_a;
        break;
    case opCos:
        func = &cos_a;
        break;
    case opTan:
        func = &tan_a;
        break;
    case opArcSin:
        func = &asin_a;
        break;
    case opArcCos:
        func = &acos_a;
        break;
    case opArcTan:
        func = &atan_a;
        break;
    case opSinH:
        func = &sinh_a;
        break;
    case opCosH:
        func = &cosh_a;
        break;
    case opTanH:
        func = &tanh_a;
        break;

    case opRand:
        func = &rand_a;
        break;
    case opRand2:
        func = &rand2_a;
        break;
    case opLinRand:
        func = &linrand_a;
        break;
    case opBiLinRand:
        func = &bilinrand_a;
        break;
    case opSum3Rand:
        func = &sum3rand_a;
        break;
    case opCoin:
        func = &coin_a;
        break;

    case opDistort:
        func = &distort_a;
        break;
    case opSoftClip:
        func = &softclip_a;
        break;

    case opRectWindow:
        func = &rectwindow_a;
        break;
    case opHanWindow:
        func = &hanwindow_a;
        break;
    case opWelchWindow:
        func = &welwindow_a;
        break;
    case opTriWindow:
        func = &triwindow_a;
        break;

    case opSCurve:
        func = &scurve_a;
        break;
    case opRamp:
        func = &ramp_a;
        break;

    default:
        func = &thru_a;
        break;
    }
    return func;
}

static UnaryOpFunc ChooseOneFunc(UnaryOpUGen* unit) {
    void (*func)(UnaryOpUGen * unit, int inNumSamples);

    switch (unit->mSpecialIndex) {
    case opSilence:
        func = &zero_a;
        break;
    case opThru:
        func = &thru_a;
        break;
    case opNeg:
        func = &invert_1;
        break;
    case opNot:
        func = &not_1;
        break;
    case opBitNot:
        func = &bitNot_1;
        break;
    case opAbs:
        func = &abs_1;
        break;
    case opCeil:
        func = &ceil_1;
        break;
    case opFloor:
        func = &floor_1;
        break;
    case opFrac:
        func = &frac_1;
        break;
    case opSign:
        func = &sign_1;
        break;
    case opSquared:
        func = &squared_1;
        break;
    case opCubed:
        func = &cubed_1;
        break;
    case opSqrt:
        func = &sqrt_1;
        break;
    case opExp:
        func = &exp_1;
        break;
    case opRecip:
        func = &recip_1;
        break;
    case opMIDICPS:
        func = &midicps_1;
        break;
    case opCPSMIDI:
        func = &cpsmidi_1;
        break;

    case opMIDIRatio:
        func = &midiratio_1;
        break;
    case opRatioMIDI:
        func = &ratiomidi_1;
        break;
    case opDbAmp:
        func = &dbamp_1;
        break;
    case opAmpDb:
        func = &ampdb_1;
        break;
    case opOctCPS:
        func = &octcps_1;
        break;
    case opCPSOct:
        func = &cpsoct_1;
        break;
    case opLog:
        func = &log_1;
        break;
    case opLog2:
        func = &log2_1;
        break;
    case opLog10:
        func = &log10_1;
        break;
    case opSin:
        func = &sin_1;
        break;
    case opCos:
        func = &cos_1;
        break;
    case opTan:
        func = &tan_1;
        break;
    case opArcSin:
        func = &asin_1;
        break;
    case opArcCos:
        func = &acos_1;
        break;
    case opArcTan:
        func = &atan_1;
        break;
    case opSinH:
        func = &sinh_1;
        break;
    case opCosH:
        func = &cosh_1;
        break;
    case opTanH:
        func = &tanh_1;
        break;

    case opRand:
        func = &rand_1;
        break;
    case opRand2:
        func = &rand2_1;
        break;
    case opLinRand:
        func = &linrand_1;
        break;
    case opBiLinRand:
        func = &bilinrand_1;
        break;
    case opSum3Rand:
        func = &sum3rand_1;
        break;
    case opCoin:
        func = &coin_1;
        break;

    case opDistort:
        func = &distort_1;
        break;
    case opSoftClip:
        func = &softclip_1;
        break;

    case opRectWindow:
        func = &rectwindow_1;
        break;
    case opHanWindow:
        func = &hanwindow_1;
        break;
    case opWelchWindow:
        func = &welwindow_1;
        break;
    case opTriWindow:
        func = &triwindow_1;
        break;

    case opSCurve:
        func = &scurve_1;
        break;
    case opRamp:
        func = &ramp_1;
        break;

    default:
        func = &thru_a;
        break;
    }
    return func;
}


static UnaryOpFunc ChooseDemandFunc(UnaryOpUGen* unit) {
    void (*func)(UnaryOpUGen * unit, int inNumSamples);

    switch (unit->mSpecialIndex) {
    case opSilence:
        func = &zero_d;
        break;
    case opThru:
        func = &thru_d;
        break;
    case opNeg:
        func = &invert_d;
        break;
    case opNot:
        func = &not_d;
        break;
    case opBitNot:
        func = &bitNot_d;
        break;
    case opAbs:
        func = &abs_d;
        break;
    case opCeil:
        func = &ceil_d;
        break;
    case opFloor:
        func = &floor_d;
        break;
    case opFrac:
        func = &frac_d;
        break;
    case opSign:
        func = &sign_d;
        break;
    case opSquared:
        func = &squared_d;
        break;
    case opCubed:
        func = &cubed_d;
        break;
    case opSqrt:
        func = &sqrt_d;
        break;
    case opExp:
        func = &exp_d;
        break;
    case opRecip:
        func = &recip_d;
        break;
    case opMIDICPS:
        func = &midicps_d;
        break;
    case opCPSMIDI:
        func = &cpsmidi_d;
        break;

    case opMIDIRatio:
        func = &midiratio_d;
        break;
    case opRatioMIDI:
        func = &ratiomidi_d;
        break;
    case opDbAmp:
        func = &dbamp_d;
        break;
    case opAmpDb:
        func = &ampdb_d;
        break;
    case opOctCPS:
        func = &octcps_d;
        break;
    case opCPSOct:
        func = &cpsoct_d;
        break;
    case opLog:
        func = &log_d;
        break;
    case opLog2:
        func = &log2_d;
        break;
    case opLog10:
        func = &log10_d;
        break;
    case opSin:
        func = &sin_d;
        break;
    case opCos:
        func = &cos_d;
        break;
    case opTan:
        func = &tan_d;
        break;
    case opArcSin:
        func = &asin_d;
        break;
    case opArcCos:
        func = &acos_d;
        break;
    case opArcTan:
        func = &atan_d;
        break;
    case opSinH:
        func = &sinh_d;
        break;
    case opCosH:
        func = &cosh_d;
        break;
    case opTanH:
        func = &tanh_d;
        break;

    case opRand:
        func = &rand_d;
        break;
    case opRand2:
        func = &rand2_d;
        break;
    case opLinRand:
        func = &linrand_d;
        break;
    case opBiLinRand:
        func = &bilinrand_d;
        break;
    case opSum3Rand:
        func = &sum3rand_d;
        break;
    case opCoin:
        func = &coin_d;
        break;

    case opDistort:
        func = &distort_d;
        break;
    case opSoftClip:
        func = &softclip_d;
        break;

    case opRectWindow:
        func = &rectwindow_d;
        break;
    case opHanWindow:
        func = &hanwindow_d;
        break;
    case opWelchWindow:
        func = &welwindow_d;
        break;
    case opTriWindow:
        func = &triwindow_d;
        break;

    case opSCurve:
        func = &scurve_d;
        break;
    case opRamp:
        func = &ramp_d;
        break;


    default:
        func = &thru_d;
        break;
    }
    return func;
}

#ifdef NOVA_SIMD

static UnaryOpFunc ChooseNovaSimdFunc(UnaryOpUGen* unit) {
    void (*func)(UnaryOpUGen * unit, int inNumSamples);

    if (BUFLENGTH == 64) {
        switch (unit->mSpecialIndex) {
        case opSilence:
            return &zero_nova_64;
        case opThru:
            func = &thru_nova;
            break;
        case opNeg:
            return &invert_nova_64;
        case opNot:
            func = &not_a;
            break;
        case opBitNot:
            func = &bitNot_a;
            break;
        case opAbs:
            return &abs_nova_64;
        case opCeil:
            func = &ceil_nova_64;
            break;
        case opFloor:
            func = &floor_nova_64;
            break;
        case opFrac:
            func = &frac_nova_64;
            break;
        case opSign:
            return &sign_nova_64;
        case opSquared:
            return &squared_nova_64;
        case opCubed:
            return &cubed_nova_64;
        case opSqrt:
            func = &sqrt_nova_64;
            break;
        case opExp:
            func = &exp_nova;
            break;
        case opRecip:
            return &recip_nova_64;
        case opMIDICPS:
            func = &midicps_nova;
            break;
        case opCPSMIDI:
            func = &cpsmidi_nova;
            break;
        case opMIDIRatio:
            func = &midiratio_nova;
            break;
        case opRatioMIDI:
            func = &ratiomidi_nova;
            break;
        case opDbAmp:
            func = &dbamp_nova;
            break;
        case opAmpDb:
            func = &ampdb_nova;
            break;
        case opOctCPS:
            func = &octcps_nova;
            break;
        case opCPSOct:
            func = &cpsoct_nova;
            break;
        case opLog:
            func = &log_nova;
            break;
        case opLog2:
            func = &log2_nova;
            break;
        case opLog10:
            func = &log10_nova;
            break;
        case opSin:
            func = &sin_nova;
            break;
        case opCos:
            func = &cos_nova;
            break;
        case opTan:
            func = &tan_nova;
            break;
        case opArcSin:
            func = &asin_nova;
            break;
        case opArcCos:
            func = &acos_nova;
            break;
        case opArcTan:
            func = &atan_nova;
            break;
        case opSinH:
            func = &sinh_a;
            break;
        case opCosH:
            func = &cosh_a;
            break;
        case opTanH:
            func = &tanh_nova;
            break;

        case opRand:
            func = &rand_a;
            break;
        case opRand2:
            func = &rand2_a;
            break;
        case opLinRand:
            func = &linrand_a;
            break;
        case opBiLinRand:
            func = &bilinrand_a;
            break;
        case opSum3Rand:
            func = &sum3rand_a;
            break;
        case opCoin:
            func = &coin_a;
            break;

        case opDistort:
            func = &distort_nova_64;
            break;
        case opSoftClip:
            func = &softclip_nova_64;
            break;

        case opRectWindow:
            func = &rectwindow_a;
            break;
        case opHanWindow:
            func = &hanwindow_a;
            break;
        case opWelchWindow:
            func = &welwindow_a;
            break;
        case opTriWindow:
            func = &triwindow_a;
            break;

        case opSCurve:
            func = &scurve_nova_64;
            break;
        case opRamp:
            return &ramp_nova_64;

        default:
            return &thru_nova_64;
        }
        return func;
    }
    switch (unit->mSpecialIndex) {
    case opSilence:
        func = &zero_nova;
        break;
    case opThru:
        func = &thru_nova;
        break;
    case opNeg:
        func = &invert_nova;
        break;
    case opNot:
        func = &not_a;
        break;
    case opBitNot:
        func = &bitNot_a;
        break;
    case opAbs:
        func = &abs_nova;
        break;
    case opCeil:
        func = &ceil_nova;
        break;
    case opFloor:
        func = &floor_nova;
        break;
    case opFrac:
        func = &frac_nova;
        break;
    case opSign:
        func = &sign_nova;
        break;
    case opSquared:
        func = &squared_nova;
        break;
    case opCubed:
        func = &cubed_nova;
        break;
    case opSqrt:
        func = &sqrt_nova;
        break;
    case opExp:
        func = &exp_nova;
        break;
    case opRecip:
        func = &recip_nova;
        break;
    case opMIDICPS:
        func = &midicps_nova;
        break;
    case opCPSMIDI:
        func = &cpsmidi_nova;
        break;
    case opMIDIRatio:
        func = &midiratio_nova;
        break;
    case opRatioMIDI:
        func = &ratiomidi_nova;
        break;
    case opDbAmp:
        func = &dbamp_nova;
        break;
    case opAmpDb:
        func = &ampdb_nova;
        break;
    case opOctCPS:
        func = &octcps_nova;
        break;
    case opCPSOct:
        func = &cpsoct_nova;
        break;
    case opLog:
        func = &log_nova;
        break;
    case opLog2:
        func = &log2_nova;
        break;
    case opLog10:
        func = &log10_nova;
        break;
    case opSin:
        func = &sin_nova;
        break;
    case opCos:
        func = &cos_nova;
        break;
    case opTan:
        func = &tan_nova;
        break;
    case opArcSin:
        func = &asin_nova;
        break;
    case opArcCos:
        func = &acos_nova;
        break;
    case opArcTan:
        func = &atan_nova;
        break;
    case opSinH:
        func = &sinh_a;
        break;
    case opCosH:
        func = &cosh_a;
        break;
    case opTanH:
        func = &tanh_nova;
        break;

    case opRand:
        func = &rand_a;
        break;
    case opRand2:
        func = &rand2_a;
        break;
    case opLinRand:
        func = &linrand_a;
        break;
    case opBiLinRand:
        func = &bilinrand_a;
        break;
    case opSum3Rand:
        func = &sum3rand_a;
        break;
    case opCoin:
        func = &coin_a;
        break;

    case opDistort:
        func = &distort_nova;
        break;
    case opSoftClip:
        func = &softclip_nova;
        break;

    case opRectWindow:
        func = &rectwindow_a;
        break;
    case opHanWindow:
        func = &hanwindow_a;
        break;
    case opWelchWindow:
        func = &welwindow_a;
        break;
    case opTriWindow:
        func = &triwindow_a;
        break;

    case opSCurve:
        func = &scurve_nova;
        break;
    case opRamp:
        func = &ramp_nova;
        break;

    default:
        func = &thru_nova;
        break;
    }
    return func;
}
#endif

bool ChooseOperatorFunc(UnaryOpUGen* unit) {
    // Print("->ChooseOperatorFunc %d\n", unit->mSpecialIndex);
    UnaryOpFunc func;
    bool ret = false;
    if (unit->mCalcRate == calc_DemandRate) {
        func = ChooseDemandFunc(unit);
    } else if (BUFLENGTH == 1) {
        func = ChooseOneFunc(unit);
#if defined(NOVA_SIMD)
    } else if (boost::alignment::is_aligned(BUFLENGTH, 16)) {
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
    // Print("<-ChooseOperatorFunc %p\n", func);
    // Print("calc %d\n", unit->mCalcRate);
    return ret;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(UnaryOp) {
    ft = inTable;

    DefineSimpleUnit(UnaryOpUGen);
}
