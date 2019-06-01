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

#include <boost/align/is_aligned.hpp>

#ifdef _MSC_VER
// hypotf is c99, but not c++
#    define hypotf _hypotf
#endif

#ifdef NOVA_SIMD
#    include "simd_binary_arithmetic.hpp"
#    include "simd_math.hpp"
#    include "simd_memory.hpp"


#    include "function_attributes.h"
using nova::slope_argument;

#    define NOVA_BINARY_WRAPPER(SCNAME, NOVANAME)                                                                      \
        FLATTEN void SCNAME##_aa_nova(BinaryOpUGen* unit, int inNumSamples) {                                          \
            nova::NOVANAME##_vec_simd(OUT(0), IN(0), IN(1), inNumSamples);                                             \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_aa_nova_64(BinaryOpUGen* unit, int inNumSamples) {                                       \
            nova::NOVANAME##_vec_simd<64>(OUT(0), IN(0), IN(1));                                                       \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ia_nova(BinaryOpUGen* unit, int inNumSamples) {                                          \
            float xa = ZIN0(0);                                                                                        \
                                                                                                                       \
            nova::NOVANAME##_vec_simd(OUT(0), xa, IN(1), inNumSamples);                                                \
            unit->mPrevA = xa;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ia_nova_64(BinaryOpUGen* unit, int inNumSamples) {                                       \
            float xa = ZIN0(0);                                                                                        \
                                                                                                                       \
            nova::NOVANAME##_vec_simd<64>(OUT(0), xa, IN(1));                                                          \
            unit->mPrevA = xa;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ai_nova(BinaryOpUGen* unit, int inNumSamples) {                                          \
            float xb = ZIN0(1);                                                                                        \
                                                                                                                       \
            nova::NOVANAME##_vec_simd(OUT(0), IN(0), xb, inNumSamples);                                                \
            unit->mPrevB = xb;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ai_nova_64(BinaryOpUGen* unit, int inNumSamples) {                                       \
            float xb = ZIN0(1);                                                                                        \
                                                                                                                       \
            nova::NOVANAME##_vec_simd<64>(OUT(0), IN(0), xb);                                                          \
            unit->mPrevB = xb;                                                                                         \
        }

#    define NOVA_BINARY_WRAPPER_K(SCNAME, NOVANAME)                                                                    \
        FLATTEN void SCNAME##_aa_nova(BinaryOpUGen* unit, int inNumSamples) {                                          \
            nova::NOVANAME##_vec_simd(OUT(0), IN(0), IN(1), inNumSamples);                                             \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_aa_nova_64(BinaryOpUGen* unit, int inNumSamples) {                                       \
            nova::NOVANAME##_vec_simd<64>(OUT(0), IN(0), IN(1));                                                       \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ia_nova(BinaryOpUGen* unit, int inNumSamples) {                                          \
            float xa = ZIN0(0);                                                                                        \
                                                                                                                       \
            nova::NOVANAME##_vec_simd(OUT(0), xa, IN(1), inNumSamples);                                                \
            unit->mPrevA = xa;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ia_nova_64(BinaryOpUGen* unit, int inNumSamples) {                                       \
            float xa = ZIN0(0);                                                                                        \
                                                                                                                       \
            nova::NOVANAME##_vec_simd<64>(OUT(0), xa, IN(1));                                                          \
            unit->mPrevA = xa;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ai_nova(BinaryOpUGen* unit, int inNumSamples) {                                          \
            float xb = ZIN0(1);                                                                                        \
                                                                                                                       \
            nova::NOVANAME##_vec_simd(OUT(0), IN(0), xb, inNumSamples);                                                \
            unit->mPrevB = xb;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ai_nova_64(BinaryOpUGen* unit, int inNumSamples) {                                       \
            float xb = ZIN0(1);                                                                                        \
                                                                                                                       \
            nova::NOVANAME##_vec_simd<64>(OUT(0), IN(0), xb);                                                          \
            unit->mPrevB = xb;                                                                                         \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ak_nova(BinaryOpUGen* unit, int inNumSamples) {                                          \
            float xb = unit->mPrevB;                                                                                   \
            float next_b = ZIN0(1);                                                                                    \
                                                                                                                       \
            if (xb == next_b) {                                                                                        \
                nova::NOVANAME##_vec_simd(OUT(0), IN(0), xb, inNumSamples);                                            \
            } else {                                                                                                   \
                float slope = CALCSLOPE(next_b, xb);                                                                   \
                nova::NOVANAME##_vec_simd(OUT(0), IN(0), slope_argument(xb, slope), inNumSamples);                     \
                unit->mPrevB = next_b;                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ak_nova_64(BinaryOpUGen* unit, int inNumSamples) {                                       \
            float xb = unit->mPrevB;                                                                                   \
            float next_b = ZIN0(1);                                                                                    \
                                                                                                                       \
            if (xb == next_b) {                                                                                        \
                nova::NOVANAME##_vec_simd<64>(OUT(0), IN(0), xb);                                                      \
            } else {                                                                                                   \
                float slope = CALCSLOPE(next_b, xb);                                                                   \
                nova::NOVANAME##_vec_simd(OUT(0), IN(0), slope_argument(xb, slope), inNumSamples);                     \
                unit->mPrevB = next_b;                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
                                                                                                                       \
        FLATTEN void SCNAME##_ka_nova(BinaryOpUGen* unit, int inNumSamples) {                                          \
            float xa = unit->mPrevA;                                                                                   \
            float next_a = ZIN0(0);                                                                                    \
                                                                                                                       \
            if (xa == next_a) {                                                                                        \
                nova::NOVANAME##_vec_simd(OUT(0), xa, IN(1), inNumSamples);                                            \
            } else {                                                                                                   \
                float slope = CALCSLOPE(next_a, xa);                                                                   \
                nova::NOVANAME##_vec_simd(OUT(0), slope_argument(xa, slope), IN(1), inNumSamples);                     \
                unit->mPrevA = next_a;                                                                                 \
            }                                                                                                          \
        }                                                                                                              \
        FLATTEN void SCNAME##_ka_nova_64(BinaryOpUGen* unit, int inNumSamples) {                                       \
            float xa = unit->mPrevA;                                                                                   \
            float next_a = ZIN0(0);                                                                                    \
                                                                                                                       \
            if (xa == next_a) {                                                                                        \
                nova::NOVANAME##_vec_simd<64>(OUT(0), xa, IN(1));                                                      \
            } else {                                                                                                   \
                float slope = CALCSLOPE(next_a, xa);                                                                   \
                nova::NOVANAME##_vec_simd(OUT(0), slope_argument(xa, slope), IN(1), inNumSamples);                     \
                unit->mPrevA = next_a;                                                                                 \
            }                                                                                                          \
        }


#    define DEFINE_TEMPLATE_FUNCTOR(NAME)                                                                              \
        struct NAME##_functor {                                                                                        \
            template <typename FloatType> inline FloatType operator()(FloatType a, FloatType b) const {                \
                return NAME(a, b);                                                                                     \
            }                                                                                                          \
                                                                                                                       \
            template <typename FloatType>                                                                              \
            inline nova::vec<FloatType> operator()(nova::vec<FloatType> a, nova::vec<FloatType> b) const {             \
                return NAME(a, b);                                                                                     \
            }                                                                                                          \
        };

DEFINE_TEMPLATE_FUNCTOR(sc_ring1)
DEFINE_TEMPLATE_FUNCTOR(sc_ring2)
DEFINE_TEMPLATE_FUNCTOR(sc_ring3)
DEFINE_TEMPLATE_FUNCTOR(sc_ring4)

DEFINE_TEMPLATE_FUNCTOR(sc_difsqr)
DEFINE_TEMPLATE_FUNCTOR(sc_sumsqr)
DEFINE_TEMPLATE_FUNCTOR(sc_sqrsum)
DEFINE_TEMPLATE_FUNCTOR(sc_sqrdif)

namespace nova {
NOVA_SIMD_DEFINE_BINARY_WRAPPER(sc_ring1, sc_ring1_functor)
NOVA_SIMD_DEFINE_BINARY_WRAPPER(sc_ring2, sc_ring2_functor)
NOVA_SIMD_DEFINE_BINARY_WRAPPER(sc_ring3, sc_ring3_functor)
NOVA_SIMD_DEFINE_BINARY_WRAPPER(sc_ring4, sc_ring4_functor)

NOVA_SIMD_DEFINE_BINARY_WRAPPER(sc_difsqr, sc_difsqr_functor)
NOVA_SIMD_DEFINE_BINARY_WRAPPER(sc_sumsqr, sc_sumsqr_functor)
NOVA_SIMD_DEFINE_BINARY_WRAPPER(sc_sqrsum, sc_sqrsum_functor)
NOVA_SIMD_DEFINE_BINARY_WRAPPER(sc_sqrdif, sc_sqrdif_functor)
}

#endif

using namespace std; // for math functions

static InterfaceTable* ft;

//////////////////////////////////////////////////////////////////////////////////////////////////


/* special binary math operators */
enum {
    opAdd,
    opSub,
    opMul,
    opIDiv,
    opFDiv,
    opMod,
    opEQ,
    opNE,
    opLT,
    opGT,
    opLE,
    opGE,
    // opIdentical,
    // opNotIdentical,

    opMin,
    opMax,
    opBitAnd,
    opBitOr,
    opBitXor,
    opLCM,
    opGCD,
    opRound,
    opRoundUp,
    opTrunc,
    opAtan2,
    opHypot,
    opHypotx,
    opPow,
    opShiftLeft, //
    opShiftRight, //
    opUnsignedShift, //
    opFill, //
    opRing1, // a * (b + 1) == a * b + a
    opRing2, // a * b + a + b
    opRing3, // a*a*b
    opRing4, // a*a*b - a*b*b
    opDifSqr, // a*a - b*b
    opSumSqr, // a*a + b*b
    opSqrSum, // (a + b)^2
    opSqrDif, // (a - b)^2
    opAbsDif, // |a - b|
    opThresh,
    opAMClip,
    opScaleNeg,
    opClip2,
    opExcess,
    opFold2,
    opWrap2,
    opFirstArg,
    opRandRange,
    opExpRandRange,

    opNumBinarySelectors
};


inline float sc_andt(float a, float b) { return int(a) & int(b); }

inline float sc_ort(float a, float b) { return int(a) | int(b); }

inline float sc_xort(float a, float b) { return int(a) ^ int(b); }

inline float sc_rst(float a, float b) { return int(a) >> int(b); }

inline float sc_lst(float a, float b) { return int(a) << int(b); }

struct BinaryOpUGen : public Unit {
    float mPrevA, mPrevB;
};

typedef void (*BinaryOpFunc)(BinaryOpUGen* unit, int inNumSamples);

extern "C" {

void BinaryOpUGen_Ctor(BinaryOpUGen* unit);

// void zero_d(BinaryOpUGen *unit, int inNumSamples);
void zero_1(BinaryOpUGen* unit, int inNumSamples);
void zero_aa(BinaryOpUGen* unit, int inNumSamples);
void firstarg_d(BinaryOpUGen* unit, int inNumSamples);
void firstarg_1(BinaryOpUGen* unit, int inNumSamples);
void firstarg_aa(BinaryOpUGen* unit, int inNumSamples);
void secondarg_d(BinaryOpUGen* unit, int inNumSamples);
void secondarg_1(BinaryOpUGen* unit, int inNumSamples);
void secondarg_aa(BinaryOpUGen* unit, int inNumSamples);
void add_d(BinaryOpUGen* unit, int inNumSamples);
void add_1(BinaryOpUGen* unit, int inNumSamples);
void add_aa(BinaryOpUGen* unit, int inNumSamples);
void add_ak(BinaryOpUGen* unit, int inNumSamples);
void add_ka(BinaryOpUGen* unit, int inNumSamples);
void add_ai(BinaryOpUGen* unit, int inNumSamples);
void add_ia(BinaryOpUGen* unit, int inNumSamples);
void sub_d(BinaryOpUGen* unit, int inNumSamples);
void sub_1(BinaryOpUGen* unit, int inNumSamples);
void sub_aa(BinaryOpUGen* unit, int inNumSamples);
void sub_ak(BinaryOpUGen* unit, int inNumSamples);
void sub_ka(BinaryOpUGen* unit, int inNumSamples);
void sub_ai(BinaryOpUGen* unit, int inNumSamples);
void sub_ia(BinaryOpUGen* unit, int inNumSamples);
void mul_d(BinaryOpUGen* unit, int inNumSamples);
void mul_1(BinaryOpUGen* unit, int inNumSamples);
void mul_aa(BinaryOpUGen* unit, int inNumSamples);
void mul_ak(BinaryOpUGen* unit, int inNumSamples);
void mul_ka(BinaryOpUGen* unit, int inNumSamples);
void mul_ai(BinaryOpUGen* unit, int inNumSamples);
void mul_ia(BinaryOpUGen* unit, int inNumSamples);
void div_d(BinaryOpUGen* unit, int inNumSamples);
void div_1(BinaryOpUGen* unit, int inNumSamples);
void div_aa(BinaryOpUGen* unit, int inNumSamples);
void div_ak(BinaryOpUGen* unit, int inNumSamples);
void div_ka(BinaryOpUGen* unit, int inNumSamples);
void div_ai(BinaryOpUGen* unit, int inNumSamples);
void div_ia(BinaryOpUGen* unit, int inNumSamples);
void mod_d(BinaryOpUGen* unit, int inNumSamples);
void mod_1(BinaryOpUGen* unit, int inNumSamples);
void mod_aa(BinaryOpUGen* unit, int inNumSamples);
void mod_ak(BinaryOpUGen* unit, int inNumSamples);
void mod_ka(BinaryOpUGen* unit, int inNumSamples);
void mod_ai(BinaryOpUGen* unit, int inNumSamples);
void mod_ia(BinaryOpUGen* unit, int inNumSamples);
void max_d(BinaryOpUGen* unit, int inNumSamples);
void max_1(BinaryOpUGen* unit, int inNumSamples);
void max_aa(BinaryOpUGen* unit, int inNumSamples);
void max_ak(BinaryOpUGen* unit, int inNumSamples);
void max_ka(BinaryOpUGen* unit, int inNumSamples);
void max_ai(BinaryOpUGen* unit, int inNumSamples);
void max_ia(BinaryOpUGen* unit, int inNumSamples);
void min_d(BinaryOpUGen* unit, int inNumSamples);
void min_1(BinaryOpUGen* unit, int inNumSamples);
void min_aa(BinaryOpUGen* unit, int inNumSamples);
void min_ak(BinaryOpUGen* unit, int inNumSamples);
void min_ka(BinaryOpUGen* unit, int inNumSamples);
void min_ai(BinaryOpUGen* unit, int inNumSamples);
void min_ia(BinaryOpUGen* unit, int inNumSamples);

void and_d(BinaryOpUGen* unit, int inNumSamples);
void and_1(BinaryOpUGen* unit, int inNumSamples);
void and_aa(BinaryOpUGen* unit, int inNumSamples);
void and_ak(BinaryOpUGen* unit, int inNumSamples);
void and_ka(BinaryOpUGen* unit, int inNumSamples);
void and_ai(BinaryOpUGen* unit, int inNumSamples);
void and_ia(BinaryOpUGen* unit, int inNumSamples);

void or_d(BinaryOpUGen* unit, int inNumSamples);
void or_1(BinaryOpUGen* unit, int inNumSamples);
void or_aa(BinaryOpUGen* unit, int inNumSamples);
void or_ak(BinaryOpUGen* unit, int inNumSamples);
void or_ka(BinaryOpUGen* unit, int inNumSamples);
void or_ai(BinaryOpUGen* unit, int inNumSamples);
void or_ia(BinaryOpUGen* unit, int inNumSamples);

void xor_d(BinaryOpUGen* unit, int inNumSamples);
void xor_1(BinaryOpUGen* unit, int inNumSamples);
void xor_aa(BinaryOpUGen* unit, int inNumSamples);
void xor_ak(BinaryOpUGen* unit, int inNumSamples);
void xor_ka(BinaryOpUGen* unit, int inNumSamples);
void xor_ai(BinaryOpUGen* unit, int inNumSamples);
void xor_ia(BinaryOpUGen* unit, int inNumSamples);

void amclip_d(BinaryOpUGen* unit, int inNumSamples);
void amclip_1(BinaryOpUGen* unit, int inNumSamples);
void amclip_aa(BinaryOpUGen* unit, int inNumSamples);
void amclip_ak(BinaryOpUGen* unit, int inNumSamples);
void amclip_ka(BinaryOpUGen* unit, int inNumSamples);
void amclip_ai(BinaryOpUGen* unit, int inNumSamples);
void amclip_ia(BinaryOpUGen* unit, int inNumSamples);
void scaleneg_d(BinaryOpUGen* unit, int inNumSamples);
void scaleneg_1(BinaryOpUGen* unit, int inNumSamples);
void scaleneg_aa(BinaryOpUGen* unit, int inNumSamples);
void scaleneg_ak(BinaryOpUGen* unit, int inNumSamples);
void scaleneg_ka(BinaryOpUGen* unit, int inNumSamples);
void scaleneg_ai(BinaryOpUGen* unit, int inNumSamples);
void scaleneg_ia(BinaryOpUGen* unit, int inNumSamples);
void pow_d(BinaryOpUGen* unit, int inNumSamples);
void pow_1(BinaryOpUGen* unit, int inNumSamples);
void pow_aa(BinaryOpUGen* unit, int inNumSamples);
void pow_ak(BinaryOpUGen* unit, int inNumSamples);
void pow_ka(BinaryOpUGen* unit, int inNumSamples);
void pow_ai(BinaryOpUGen* unit, int inNumSamples);
void pow_ia(BinaryOpUGen* unit, int inNumSamples);
void ring1_d(BinaryOpUGen* unit, int inNumSamples);
void ring1_1(BinaryOpUGen* unit, int inNumSamples);
void ring1_aa(BinaryOpUGen* unit, int inNumSamples);
void ring1_ak(BinaryOpUGen* unit, int inNumSamples);
void ring1_ka(BinaryOpUGen* unit, int inNumSamples);
void ring1_ai(BinaryOpUGen* unit, int inNumSamples);
void ring1_ia(BinaryOpUGen* unit, int inNumSamples);
void ring2_d(BinaryOpUGen* unit, int inNumSamples);
void ring2_1(BinaryOpUGen* unit, int inNumSamples);
void ring2_aa(BinaryOpUGen* unit, int inNumSamples);
void ring2_ak(BinaryOpUGen* unit, int inNumSamples);
void ring2_ka(BinaryOpUGen* unit, int inNumSamples);
void ring2_ai(BinaryOpUGen* unit, int inNumSamples);
void ring2_ia(BinaryOpUGen* unit, int inNumSamples);
void ring3_d(BinaryOpUGen* unit, int inNumSamples);
void ring3_1(BinaryOpUGen* unit, int inNumSamples);
void ring3_aa(BinaryOpUGen* unit, int inNumSamples);
void ring3_ak(BinaryOpUGen* unit, int inNumSamples);
void ring3_ka(BinaryOpUGen* unit, int inNumSamples);
void ring3_ai(BinaryOpUGen* unit, int inNumSamples);
void ring3_ia(BinaryOpUGen* unit, int inNumSamples);
void ring4_d(BinaryOpUGen* unit, int inNumSamples);
void ring4_1(BinaryOpUGen* unit, int inNumSamples);
void ring4_aa(BinaryOpUGen* unit, int inNumSamples);
void ring4_ak(BinaryOpUGen* unit, int inNumSamples);
void ring4_ka(BinaryOpUGen* unit, int inNumSamples);
void ring4_ai(BinaryOpUGen* unit, int inNumSamples);
void ring4_ia(BinaryOpUGen* unit, int inNumSamples);
void thresh_d(BinaryOpUGen* unit, int inNumSamples);
void thresh_1(BinaryOpUGen* unit, int inNumSamples);
void thresh_aa(BinaryOpUGen* unit, int inNumSamples);
void thresh_ak(BinaryOpUGen* unit, int inNumSamples);
void thresh_ka(BinaryOpUGen* unit, int inNumSamples);
void thresh_ai(BinaryOpUGen* unit, int inNumSamples);
void thresh_ia(BinaryOpUGen* unit, int inNumSamples);
void clip2_d(BinaryOpUGen* unit, int inNumSamples);
void clip2_1(BinaryOpUGen* unit, int inNumSamples);
void clip2_aa(BinaryOpUGen* unit, int inNumSamples);
void clip2_ak(BinaryOpUGen* unit, int inNumSamples);
void clip2_ka(BinaryOpUGen* unit, int inNumSamples);
void clip2_ai(BinaryOpUGen* unit, int inNumSamples);
void clip2_ia(BinaryOpUGen* unit, int inNumSamples);
void fold2_d(BinaryOpUGen* unit, int inNumSamples);
void fold2_1(BinaryOpUGen* unit, int inNumSamples);
void fold2_aa(BinaryOpUGen* unit, int inNumSamples);
void fold2_ak(BinaryOpUGen* unit, int inNumSamples);
void fold2_ka(BinaryOpUGen* unit, int inNumSamples);
void fold2_ai(BinaryOpUGen* unit, int inNumSamples);
void fold2_ia(BinaryOpUGen* unit, int inNumSamples);
void wrap2_d(BinaryOpUGen* unit, int inNumSamples);
void wrap2_1(BinaryOpUGen* unit, int inNumSamples);
void wrap2_aa(BinaryOpUGen* unit, int inNumSamples);
void wrap2_ak(BinaryOpUGen* unit, int inNumSamples);
void wrap2_ka(BinaryOpUGen* unit, int inNumSamples);
void wrap2_ai(BinaryOpUGen* unit, int inNumSamples);
void wrap2_ia(BinaryOpUGen* unit, int inNumSamples);
void excess_d(BinaryOpUGen* unit, int inNumSamples);
void excess_1(BinaryOpUGen* unit, int inNumSamples);
void excess_aa(BinaryOpUGen* unit, int inNumSamples);
void excess_ak(BinaryOpUGen* unit, int inNumSamples);
void excess_ka(BinaryOpUGen* unit, int inNumSamples);
void excess_ai(BinaryOpUGen* unit, int inNumSamples);
void excess_ia(BinaryOpUGen* unit, int inNumSamples);
void rrand_d(BinaryOpUGen* unit, int inNumSamples);
void rrand_1(BinaryOpUGen* unit, int inNumSamples);
void rrand_aa(BinaryOpUGen* unit, int inNumSamples);
void rrand_ak(BinaryOpUGen* unit, int inNumSamples);
void rrand_ka(BinaryOpUGen* unit, int inNumSamples);
void rrand_ai(BinaryOpUGen* unit, int inNumSamples);
void rrand_ia(BinaryOpUGen* unit, int inNumSamples);
void exprand_d(BinaryOpUGen* unit, int inNumSamples);
void exprand_1(BinaryOpUGen* unit, int inNumSamples);
void exprand_aa(BinaryOpUGen* unit, int inNumSamples);
void exprand_ak(BinaryOpUGen* unit, int inNumSamples);
void exprand_ka(BinaryOpUGen* unit, int inNumSamples);
void exprand_ai(BinaryOpUGen* unit, int inNumSamples);
void exprand_ia(BinaryOpUGen* unit, int inNumSamples);
void lt_d(BinaryOpUGen* unit, int inNumSamples);
void lt_1(BinaryOpUGen* unit, int inNumSamples);
void lt_aa(BinaryOpUGen* unit, int inNumSamples);
void lt_ak(BinaryOpUGen* unit, int inNumSamples);
void lt_ka(BinaryOpUGen* unit, int inNumSamples);
void lt_ai(BinaryOpUGen* unit, int inNumSamples);
void lt_ia(BinaryOpUGen* unit, int inNumSamples);
void le_d(BinaryOpUGen* unit, int inNumSamples);
void le_1(BinaryOpUGen* unit, int inNumSamples);
void le_aa(BinaryOpUGen* unit, int inNumSamples);
void le_ak(BinaryOpUGen* unit, int inNumSamples);
void le_ka(BinaryOpUGen* unit, int inNumSamples);
void le_ai(BinaryOpUGen* unit, int inNumSamples);
void le_ia(BinaryOpUGen* unit, int inNumSamples);
void lcm_d(BinaryOpUGen* unit, int inNumSamples);
void lcm_1(BinaryOpUGen* unit, int inNumSamples);
void lcm_aa(BinaryOpUGen* unit, int inNumSamples);
void lcm_ak(BinaryOpUGen* unit, int inNumSamples);
void lcm_ka(BinaryOpUGen* unit, int inNumSamples);
void lcm_ai(BinaryOpUGen* unit, int inNumSamples);
void lcm_ia(BinaryOpUGen* unit, int inNumSamples);
void gcd_d(BinaryOpUGen* unit, int inNumSamples);
void gcd_1(BinaryOpUGen* unit, int inNumSamples);
void gcd_aa(BinaryOpUGen* unit, int inNumSamples);
void gcd_ak(BinaryOpUGen* unit, int inNumSamples);
void gcd_ka(BinaryOpUGen* unit, int inNumSamples);
void gcd_ai(BinaryOpUGen* unit, int inNumSamples);
void gcd_ia(BinaryOpUGen* unit, int inNumSamples);
void gt_d(BinaryOpUGen* unit, int inNumSamples);
void gt_1(BinaryOpUGen* unit, int inNumSamples);
void gt_aa(BinaryOpUGen* unit, int inNumSamples);
void gt_ak(BinaryOpUGen* unit, int inNumSamples);
void gt_ka(BinaryOpUGen* unit, int inNumSamples);
void gt_ai(BinaryOpUGen* unit, int inNumSamples);
void gt_ia(BinaryOpUGen* unit, int inNumSamples);
void ge_d(BinaryOpUGen* unit, int inNumSamples);
void ge_1(BinaryOpUGen* unit, int inNumSamples);
void ge_aa(BinaryOpUGen* unit, int inNumSamples);
void ge_ak(BinaryOpUGen* unit, int inNumSamples);
void ge_ka(BinaryOpUGen* unit, int inNumSamples);
void ge_ai(BinaryOpUGen* unit, int inNumSamples);
void ge_ia(BinaryOpUGen* unit, int inNumSamples);
void eq_d(BinaryOpUGen* unit, int inNumSamples);
void eq_1(BinaryOpUGen* unit, int inNumSamples);
void eq_aa(BinaryOpUGen* unit, int inNumSamples);
void eq_ak(BinaryOpUGen* unit, int inNumSamples);
void eq_ka(BinaryOpUGen* unit, int inNumSamples);
void eq_ai(BinaryOpUGen* unit, int inNumSamples);
void eq_ia(BinaryOpUGen* unit, int inNumSamples);
void neq_d(BinaryOpUGen* unit, int inNumSamples);
void neq_1(BinaryOpUGen* unit, int inNumSamples);
void neq_aa(BinaryOpUGen* unit, int inNumSamples);
void neq_ak(BinaryOpUGen* unit, int inNumSamples);
void neq_ka(BinaryOpUGen* unit, int inNumSamples);
void neq_ai(BinaryOpUGen* unit, int inNumSamples);
void neq_ia(BinaryOpUGen* unit, int inNumSamples);
void sumsqr_d(BinaryOpUGen* unit, int inNumSamples);
void sumsqr_1(BinaryOpUGen* unit, int inNumSamples);
void sumsqr_aa(BinaryOpUGen* unit, int inNumSamples);
void sumsqr_ak(BinaryOpUGen* unit, int inNumSamples);
void sumsqr_ka(BinaryOpUGen* unit, int inNumSamples);
void sumsqr_ai(BinaryOpUGen* unit, int inNumSamples);
void sumsqr_ia(BinaryOpUGen* unit, int inNumSamples);
void difsqr_d(BinaryOpUGen* unit, int inNumSamples);
void difsqr_1(BinaryOpUGen* unit, int inNumSamples);
void difsqr_aa(BinaryOpUGen* unit, int inNumSamples);
void difsqr_ak(BinaryOpUGen* unit, int inNumSamples);
void difsqr_ka(BinaryOpUGen* unit, int inNumSamples);
void difsqr_ai(BinaryOpUGen* unit, int inNumSamples);
void difsqr_ia(BinaryOpUGen* unit, int inNumSamples);
void sqrsum_d(BinaryOpUGen* unit, int inNumSamples);
void sqrsum_1(BinaryOpUGen* unit, int inNumSamples);
void sqrsum_aa(BinaryOpUGen* unit, int inNumSamples);
void sqrsum_ak(BinaryOpUGen* unit, int inNumSamples);
void sqrsum_ka(BinaryOpUGen* unit, int inNumSamples);
void sqrsum_ai(BinaryOpUGen* unit, int inNumSamples);
void sqrsum_ia(BinaryOpUGen* unit, int inNumSamples);
void sqrdif_d(BinaryOpUGen* unit, int inNumSamples);
void sqrdif_1(BinaryOpUGen* unit, int inNumSamples);
void sqrdif_aa(BinaryOpUGen* unit, int inNumSamples);
void sqrdif_ak(BinaryOpUGen* unit, int inNumSamples);
void sqrdif_ka(BinaryOpUGen* unit, int inNumSamples);
void sqrdif_ai(BinaryOpUGen* unit, int inNumSamples);
void sqrdif_ia(BinaryOpUGen* unit, int inNumSamples);
void absdif_d(BinaryOpUGen* unit, int inNumSamples);
void absdif_1(BinaryOpUGen* unit, int inNumSamples);
void absdif_aa(BinaryOpUGen* unit, int inNumSamples);
void absdif_ak(BinaryOpUGen* unit, int inNumSamples);
void absdif_ka(BinaryOpUGen* unit, int inNumSamples);
void absdif_ai(BinaryOpUGen* unit, int inNumSamples);
void absdif_ia(BinaryOpUGen* unit, int inNumSamples);
void round_d(BinaryOpUGen* unit, int inNumSamples);
void round_1(BinaryOpUGen* unit, int inNumSamples);
void round_aa(BinaryOpUGen* unit, int inNumSamples);
void round_ak(BinaryOpUGen* unit, int inNumSamples);
void round_ka(BinaryOpUGen* unit, int inNumSamples);
void round_ai(BinaryOpUGen* unit, int inNumSamples);
void round_ia(BinaryOpUGen* unit, int inNumSamples);
void roundUp_d(BinaryOpUGen* unit, int inNumSamples);
void roundUp_1(BinaryOpUGen* unit, int inNumSamples);
void roundUp_aa(BinaryOpUGen* unit, int inNumSamples);
void roundUp_ak(BinaryOpUGen* unit, int inNumSamples);
void roundUp_ka(BinaryOpUGen* unit, int inNumSamples);
void roundUp_ai(BinaryOpUGen* unit, int inNumSamples);
void roundUp_ia(BinaryOpUGen* unit, int inNumSamples);
void trunc_d(BinaryOpUGen* unit, int inNumSamples);
void trunc_1(BinaryOpUGen* unit, int inNumSamples);
void trunc_aa(BinaryOpUGen* unit, int inNumSamples);
void trunc_ak(BinaryOpUGen* unit, int inNumSamples);
void trunc_ka(BinaryOpUGen* unit, int inNumSamples);
void trunc_ai(BinaryOpUGen* unit, int inNumSamples);
void trunc_ia(BinaryOpUGen* unit, int inNumSamples);
void atan2_d(BinaryOpUGen* unit, int inNumSamples);
void atan2_1(BinaryOpUGen* unit, int inNumSamples);
void atan2_aa(BinaryOpUGen* unit, int inNumSamples);
void atan2_ak(BinaryOpUGen* unit, int inNumSamples);
void atan2_ka(BinaryOpUGen* unit, int inNumSamples);
void atan2_ai(BinaryOpUGen* unit, int inNumSamples);
void atan2_ia(BinaryOpUGen* unit, int inNumSamples);
void hypot_d(BinaryOpUGen* unit, int inNumSamples);
void hypot_1(BinaryOpUGen* unit, int inNumSamples);
void hypot_aa(BinaryOpUGen* unit, int inNumSamples);
void hypot_ak(BinaryOpUGen* unit, int inNumSamples);
void hypot_ka(BinaryOpUGen* unit, int inNumSamples);
void hypot_ai(BinaryOpUGen* unit, int inNumSamples);
void hypot_ia(BinaryOpUGen* unit, int inNumSamples);
void hypotx_d(BinaryOpUGen* unit, int inNumSamples);
void hypotx_1(BinaryOpUGen* unit, int inNumSamples);
void hypotx_aa(BinaryOpUGen* unit, int inNumSamples);
void hypotx_ak(BinaryOpUGen* unit, int inNumSamples);
void hypotx_ka(BinaryOpUGen* unit, int inNumSamples);
void hypotx_ai(BinaryOpUGen* unit, int inNumSamples);
void hypotx_ia(BinaryOpUGen* unit, int inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool ChooseOperatorFunc(BinaryOpUGen* unit);

void BinaryOpUGen_Ctor(BinaryOpUGen* unit) {
    unit->mPrevA = ZIN0(0);
    unit->mPrevB = ZIN0(1);
    bool initialized = ChooseOperatorFunc(unit);
    if (unit->mCalcRate == calc_DemandRate) {
        OUT0(0) = 0.f;
    } else {
        if (!initialized)
            (unit->mCalcFunc)(unit, 1);
    }
}

/*
void zero_d(BinaryOpUGen *unit, int inNumSamples)
{
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : 0.f;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}
*/

void firstarg_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void secondarg_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : b;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void add_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a + b;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void sub_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a - b;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void mul_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a * b;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void div_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a / b;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void idiv_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : floor(a / b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void mod_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_mod(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void max_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_max(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void min_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_min(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void and_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_andt(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void or_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_ort(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void xor_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_xort(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void rightShift_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_rst(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void leftShift_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_lst(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void lcm_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_lcm(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void gcd_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_gcd(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void amclip_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_amclip(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void scaleneg_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_scaleneg(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void pow_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : (a < 0.f ? -pow(-a, b) : pow(a, b));
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void ring1_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a * b + a;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void ring2_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a * b + a + b;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void ring3_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a * a * b;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void ring4_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a * a * b - a * b * b;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void thresh_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_thresh(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void clip2_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_clip2(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void excess_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_excess(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void lt_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : (a < b ? 1.f : 0.f);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void gt_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : (a > b ? 1.f : 0.f);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void le_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : (a <= b ? 1.f : 0.f);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void ge_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : (a >= b ? 1.f : 0.f);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void eq_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : (a == b ? 1.f : 0.f);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void neq_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : (a != b ? 1.f : 0.f);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}


void sumsqr_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a * a + b * b;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void difsqr_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : a * a - b * b;
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void sqrsum_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        float z;
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : (z = a + b, z * z);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void sqrdif_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        float z;
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : (z = a - b, z * z);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void absdif_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : fabs(a - b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void round_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_round(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void roundUp_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_roundUp(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void trunc_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_trunc(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void fold2_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_fold2(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void wrap2_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_wrap2(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void atan2_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : atan2(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void hypot_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : hypot(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void hypotx_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float a = DEMANDINPUT_A(0, inNumSamples);
        float b = DEMANDINPUT_A(1, inNumSamples);
        OUT0(0) = sc_isnan(a) || sc_isnan(b) ? NAN : sc_hypotx(a, b);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void rrand_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float xa = DEMANDINPUT_A(0, inNumSamples);
        float xb = DEMANDINPUT_A(1, inNumSamples);
        RGen& rgen = *unit->mParent->mRGen;
        OUT0(0) = sc_isnan(xa) || sc_isnan(xb)
            ? NAN
            : xb > xa ? xa + rgen.frand() * (xb - xa) : (xb + rgen.frand() * (xa - xb));
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}

void exprand_d(BinaryOpUGen* unit, int inNumSamples) {
    if (inNumSamples) {
        float xa = DEMANDINPUT_A(0, inNumSamples);
        float xb = DEMANDINPUT_A(1, inNumSamples);
        RGen& rgen = *unit->mParent->mRGen;
        OUT0(0) = sc_isnan(xa) || sc_isnan(xb) ? NAN : xb > xa ? rgen.exprandrng(xa, xb) : rgen.exprandrng(xb, xa);
    } else {
        RESETINPUT(0);
        RESETINPUT(1);
    }
}


void zero_1(BinaryOpUGen* unit, int inNumSamples) { ZOUT0(0) = 0.f; }

void firstarg_1(BinaryOpUGen* unit, int inNumSamples) { ZOUT0(0) = ZIN0(0); }

void secondarg_1(BinaryOpUGen* unit, int inNumSamples) { ZOUT0(0) = ZIN0(1); }

void add_1(BinaryOpUGen* unit, int inNumSamples) { ZOUT0(0) = ZIN0(0) + ZIN0(1); }

void sub_1(BinaryOpUGen* unit, int inNumSamples) { ZOUT0(0) = ZIN0(0) - ZIN0(1); }

void mul_1(BinaryOpUGen* unit, int inNumSamples) { ZOUT0(0) = ZIN0(0) * ZIN0(1); }

void div_1(BinaryOpUGen* unit, int inNumSamples) { ZOUT0(0) = ZIN0(0) / ZIN0(1); }

void idiv_1(BinaryOpUGen* unit, int inNumSamples) { ZOUT0(0) = floor(ZIN0(0) / ZIN0(1)); }

void mod_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_mod(xa, xb);
}

void max_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_max(xa, xb);
}

void min_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_min(xa, xb);
}

void and_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_andt(xa, xb);
}

void or_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_ort(xa, xb);
}

void xor_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_xort(xa, xb);
}

void rightShift_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_rst(xa, xb);
}

void leftShift_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_lst(xa, xb);
}

void lcm_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_lcm(xa, xb);
}

void gcd_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_gcd(xa, xb);
}

void amclip_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_amclip(xa, xb);
}

void scaleneg_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa >= 0.f ? xa : xa * xb;
}

void pow_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa >= 0.f ? pow(xa, xb) : -pow(-xa, xb);
}


void ring1_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa * xb + xa;
}

void ring2_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa * xb + xa + xb;
}

void ring3_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa * xa * xb;
}

void ring4_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa * xa * xb - xa * xb * xb;
}

void thresh_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa < xb ? 0.f : xa;
}

void clip2_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa > xb ? xb : (xa < -xb ? -xb : xa);
}

void excess_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa > xb ? xa - xb : (xa < -xb ? xa + xb : 0.f);
}

void lt_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa < xb ? 1.f : 0.f;
}

void le_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa <= xb ? 1.f : 0.f;
}

void gt_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa > xb ? 1.f : 0.f;
}

void ge_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa >= xb ? 1.f : 0.f;
}

void eq_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa == xb ? 1.f : 0.f;
}

void neq_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa != xb ? 1.f : 0.f;
}


void sumsqr_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa * xa + xb * xb;
}

void difsqr_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = xa * xa - xb * xb;
}

void sqrsum_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    float sum = xa + xb;
    ZOUT0(0) = sum * sum;
}

void sqrdif_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    float dif = xa - xb;
    ZOUT0(0) = dif * dif;
}

void absdif_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = fabs(xa - xb);
}

void round_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_round(xa, xb);
}

void roundUp_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_roundUp(xa, xb);
}

void trunc_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_trunc(xa, xb);
}

void fold2_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_fold(xa, -xb, xb);
}

void wrap2_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_wrap(xa, -xb, xb);
}

void atan2_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = atan2(xa, xb);
}

void hypot_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = hypot(xa, xb);
}

void hypotx_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    ZOUT0(0) = sc_hypotx(xa, xb);
}

void rrand_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    RGen& rgen = *unit->mParent->mRGen;
    ZOUT0(0) = xb > xa ? xa + rgen.frand() * (xb - xa) : (xb + rgen.frand() * (xa - xb));
}

void exprand_1(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    float xb = ZIN0(1);
    RGen& rgen = *unit->mParent->mRGen;
    ZOUT0(0) = xb > xa ? rgen.exprandrng(xa, xb) : rgen.exprandrng(xb, xa);
}

void zero_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);

    ZClear(inNumSamples, out);
}

void firstarg_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);

    ZCopy(inNumSamples, out, a);
}

#ifdef NOVA_SIMD
FLATTEN void firstarg_aa_nova(BinaryOpUGen* unit, int inNumSamples) { nova::copyvec_simd(OUT(0), IN(0), inNumSamples); }
#endif

void secondarg_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* b = ZIN(1);

    ZCopy(inNumSamples, out, b);
}

#ifdef NOVA_SIMD
FLATTEN void secondarg_aa_nova(BinaryOpUGen* unit, int inNumSamples) {
    nova::copyvec_simd(OUT(0), IN(1), inNumSamples);
}
#endif

void add_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = ZXP(a) + ZXP(b););
}

void add_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f) {
            ZCopy(inNumSamples, out, a);
        } else {
            float* out = ZOUT(0);
            float* a = ZIN(0);
            LOOP1(inNumSamples, ZXP(out) = ZXP(a) + xb;);
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, ZXP(out) = ZXP(a) + xb; xb += slope;);
        unit->mPrevB = xb;
    }
}


void add_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f) {
            ZCopy(inNumSamples, out, b);
        } else {
            LOOP1(inNumSamples, ZXP(out) = xa + ZXP(b););
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, ZXP(out) = xa + ZXP(b); xa += slope;);
        unit->mPrevA = xa;
    }
}

void add_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = xa + ZXP(b););
    unit->mPrevA = xa;
}

void add_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, ZXP(out) = ZXP(a) + xb;);
    unit->mPrevB = xb;
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER(add, plus)

FLATTEN void add_ak_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f)
            nova::copyvec_simd(OUT(0), IN(0), inNumSamples);
        else
            nova::plus_vec_simd(OUT(0), IN(0), xb, inNumSamples);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        nova::plus_vec_simd(OUT(0), IN(0), slope_argument(xb, slope), inNumSamples);
        unit->mPrevB = next_b;
    }
}

FLATTEN void add_ak_nova_64(BinaryOpUGen* unit, int inNumSamples) {
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f)
            nova::copyvec_simd<64>(OUT(0), IN(0));
        else
            nova::plus_vec_simd<64>(OUT(0), IN(0), xb);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        nova::plus_vec_simd(OUT(0), IN(0), slope_argument(xb, slope), inNumSamples);
        unit->mPrevB = next_b;
    }
}


FLATTEN void add_ka_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xa = unit->mPrevA;
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f)
            nova::copyvec_simd(OUT(0), IN(1), inNumSamples);
        else
            nova::plus_vec_simd(OUT(0), IN(1), xa, inNumSamples);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        nova::plus_vec_simd(OUT(0), IN(1), slope_argument(xa, slope), inNumSamples);
        unit->mPrevA = next_a;
    }
}

FLATTEN void add_ka_nova_64(BinaryOpUGen* unit, int inNumSamples) {
    float xa = unit->mPrevA;
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f)
            nova::copyvec_simd<64>(OUT(0), IN(1));
        else
            nova::plus_vec_simd<64>(OUT(0), IN(1), xa);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        nova::plus_vec_simd(OUT(0), IN(1), slope_argument(xa, slope), inNumSamples);
        unit->mPrevA = next_a;
    }
}

#endif


/////////////////////////


void sub_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = ZXP(a) - ZXP(b););
}

void sub_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f) {
            ZCopy(inNumSamples, out, a);
        } else {
            LOOP1(inNumSamples, ZXP(out) = ZXP(a) - xb;);
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, ZXP(out) = ZXP(a) - xb; xb += slope;);
        unit->mPrevB = xb;
    }
}

void sub_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f) {
            ZCopy(inNumSamples, out, b);
        } else {
            LOOP1(inNumSamples, ZXP(out) = xa - ZXP(b););
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, ZXP(out) = xa - ZXP(b); xa += slope;);
        unit->mPrevA = xa;
    }
}


void sub_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = xa - ZXP(b););
    unit->mPrevA = xa;
}

void sub_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, ZXP(out) = ZXP(a) - xb;);
    unit->mPrevB = xb;
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER(sub, minus)

FLATTEN void sub_ak_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f)
            nova::copyvec_simd(OUT(0), IN(0), inNumSamples);
        else
            nova::minus_vec_simd(OUT(0), IN(0), xb, inNumSamples);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        nova::minus_vec_simd(OUT(0), IN(0), slope_argument(xb, slope), inNumSamples);
        unit->mPrevB = next_b;
    }
}

FLATTEN void sub_ak_nova_64(BinaryOpUGen* unit, int inNumSamples) {
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f)
            nova::copyvec_aa_simd<64>(OUT(0), IN(0));
        else
            nova::minus_vec_simd<64>(OUT(0), IN(0), xb);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        nova::minus_vec_simd(OUT(0), IN(0), slope_argument(xb, slope), inNumSamples);
        unit->mPrevB = next_b;
    }
}

FLATTEN void sub_ka_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xa = unit->mPrevA;
    float next_a = ZIN0(0);

    if (xa == next_a) {
        nova::minus_vec_simd(OUT(0), xa, IN(1), inNumSamples);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        nova::minus_vec_simd(OUT(0), slope_argument(xa, slope), IN(1), inNumSamples);
        unit->mPrevA = next_a;
    }
}

FLATTEN void sub_ka_nova_64(BinaryOpUGen* unit, int inNumSamples) {
    float xa = unit->mPrevA;
    float next_a = ZIN0(0);

    if (xa == next_a) {
        nova::minus_vec_simd<64>(OUT(0), xa, IN(1));
    } else {
        float slope = CALCSLOPE(next_a, xa);
        nova::minus_vec_simd(OUT(0), slope_argument(xa, slope), IN(1), inNumSamples);
        unit->mPrevA = next_a;
    }
}

#endif


void mul_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = ZXP(a) * ZXP(b););
}

void mul_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f) {
            ZClear(inNumSamples, out);
        } else if (xb == 1.f) {
            ZCopy(inNumSamples, out, a);
        } else {
            LOOP1(inNumSamples, ZXP(out) = ZXP(a) * xb;);
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, ZXP(out) = ZXP(a) * xb; xb += slope;);
        unit->mPrevB = xb;
    }
}

void mul_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f) {
            ZClear(inNumSamples, out);
        } else if (xa == 1.f) {
            ZCopy(inNumSamples, out, b);
        } else {
            LOOP1(inNumSamples, ZXP(out) = xa * ZXP(b););
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, ZXP(out) = xa * ZXP(b); xa += slope;);
        unit->mPrevA = xa;
    }
}


void mul_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, ZXP(out) = ZXP(a) * xb;);
    unit->mPrevB = xb;
}

void mul_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = xa * ZXP(b););
    unit->mPrevA = xa;
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER(mul, times)

FLATTEN void mul_ka_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xa = unit->mPrevA;
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f)
            nova::zerovec_simd(OUT(0), inNumSamples);
        else if (xa == 1.f)
            nova::copyvec_simd(OUT(0), IN(1), inNumSamples);
        else
            nova::times_vec_simd(OUT(0), IN(1), xa, inNumSamples);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        unit->mPrevA = next_a;

        nova::times_vec_simd(OUT(0), IN(1), slope_argument(xa, slope), inNumSamples);
    }
}

FLATTEN void mul_ka_nova_64(BinaryOpUGen* unit, int inNumSamples) {
    float xa = unit->mPrevA;
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f)
            nova::zerovec_simd<64>(OUT(0));
        else if (xa == 1.f)
            nova::copyvec_simd<64>(OUT(0), IN(1));
        else
            nova::times_vec_simd<64>(OUT(0), IN(1), xa);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        unit->mPrevA = next_a;

        nova::times_vec_simd(OUT(0), IN(1), slope_argument(xa, slope), inNumSamples);
    }
}

FLATTEN void mul_ak_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f)
            nova::zerovec_simd(OUT(0), inNumSamples);
        else if (xb == 1.f)
            nova::copyvec_simd(OUT(0), IN(0), inNumSamples);
        else
            nova::times_vec_simd(OUT(0), IN(0), xb, inNumSamples);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        unit->mPrevB = next_b;

        nova::times_vec_simd(OUT(0), IN(0), slope_argument(xb, slope), inNumSamples);
    }
}

FLATTEN void mul_ak_nova_64(BinaryOpUGen* unit, int inNumSamples) {
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f)
            nova::zerovec_simd<64>(OUT(0));
        else if (xb == 1.f)
            nova::copyvec_simd<64>(OUT(0), IN(0));
        else
            nova::times_vec_simd<64>(OUT(0), IN(0), xb);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        unit->mPrevB = next_b;

        nova::times_vec_simd(OUT(0), IN(0), slope_argument(xb, slope), inNumSamples);
    }
}

#endif


void div_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = ZXP(a) / ZXP(b););
}

void div_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f) {
            ZClear(inNumSamples, out);
        } else if (xb == 1.f) {
            ZCopy(inNumSamples, out, a);
        } else {
            float recip = 1.f / xb;
            LOOP1(inNumSamples, ZXP(out) = ZXP(a) * recip;);
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, ZXP(out) = ZXP(a) / xb; xb += slope;);
        unit->mPrevB = xb;
    }
}

void div_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f) {
            ZClear(inNumSamples, out);
        } else {
            LOOP1(inNumSamples, ZXP(out) = xa / ZXP(b););
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, ZXP(out) = xa / ZXP(b); xa += slope;);
        unit->mPrevA = xa;
    }
}

void div_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = xa / ZXP(b););
    unit->mPrevA = xa;
}


void div_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    float rxb = 1.f / xb;
    LOOP1(inNumSamples, ZXP(out) = ZXP(a) * rxb;);
    unit->mPrevB = xb;
}

#ifdef NOVA_SIMD
FLATTEN void div_aa_nova(BinaryOpUGen* unit, int inNumSamples) {
    nova::over_vec_simd(OUT(0), IN(0), IN(1), inNumSamples);
}

FLATTEN void div_ia_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);

    nova::over_vec_simd(OUT(0), xa, IN(1), inNumSamples);
    unit->mPrevA = xa;
}

FLATTEN void div_ai_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xb = ZIN0(1);

    nova::times_vec_simd(OUT(0), IN(0), sc_reciprocal(xb), inNumSamples);
    unit->mPrevB = xb;
}

FLATTEN void div_ak_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f)
            nova::zerovec_simd(OUT(0), inNumSamples);
        else if (xb == 1.f)
            nova::copyvec_simd(OUT(0), IN(0), inNumSamples);
        else {
            float recip = 1.f / xb;
            nova::times_vec_simd(OUT(0), IN(0), recip, inNumSamples);
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        nova::over_vec_simd(OUT(0), IN(0), slope_argument(xb, slope), inNumSamples);
        unit->mPrevB = next_b;
    }
}

FLATTEN void div_ka_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xa = unit->mPrevA;
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f)
            nova::zerovec_simd(OUT(0), inNumSamples);
        else
            nova::over_vec_simd(OUT(0), xa, IN(1), inNumSamples);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        nova::over_vec_simd(OUT(0), slope_argument(xa, slope), IN(1), inNumSamples);
        unit->mPrevA = next_a;
    }
}

#endif


void idiv_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = floor(ZXP(a) / ZXP(b)););
}

void idiv_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        ZXP(out) = floor(ZXP(a) / xb);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, ZXP(out) = floor(ZXP(a) / xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void idiv_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f) {
            ZClear(inNumSamples, out);
        } else {
            LOOP1(inNumSamples, ZXP(out) = floor(xa / ZXP(b)););
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, ZXP(out) = floor(xa / ZXP(b)); xa += slope;);
        unit->mPrevA = xa;
    }
}

void idiv_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = floor(xa / ZXP(b)););
}


void idiv_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, ZXP(out) = floor(ZXP(a) / xb););
}


void mod_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_mod(xa, xb););
}

void mod_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f) {
            ZCopy(inNumSamples, out, a);
        } else {
            LOOP1(inNumSamples, ZXP(out) = sc_mod(ZXP(a), xb););
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, ZXP(out) = sc_mod(ZXP(a), xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void mod_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f) {
            ZClear(inNumSamples, out);
        } else {
            LOOP1(inNumSamples, ZXP(out) = sc_mod(xa, ZXP(b)););
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, ZXP(out) = sc_mod(xa, ZXP(b)); xa += slope;);
        unit->mPrevA = xa;
    }
}


void mod_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, ZXP(out) = sc_mod(xa, ZXP(b)););
    unit->mPrevA = xa;
}


void mod_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, ZXP(out) = sc_mod(ZXP(a), xb););
    unit->mPrevB = xb;
}


void max_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_max(xa, xb););
}

void max_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_max(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_max(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void max_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_max(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_max(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void max_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_max(xa, xb););
    unit->mPrevA = xa;
}


void max_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_max(xa, xb););
    unit->mPrevB = xb;
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER_K(max, max)
#endif


void min_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_min(xa, xb););
}

void min_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_min(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_min(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void min_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_min(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_min(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void min_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_min(xa, xb););
    unit->mPrevA = xa;
}


void min_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_min(xa, xb););
    unit->mPrevB = xb;
}


#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER_K(min, min)
#endif


void and_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_andt(xa, xb););
}

void and_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_andt(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_andt(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void and_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_andt(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_andt(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void and_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_andt(xa, xb););
    unit->mPrevA = xa;
}


void and_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_andt(xa, xb););
    unit->mPrevB = xb;
}


void or_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_ort(xa, xb););
}

void or_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_ort(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_ort(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void or_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_ort(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_ort(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void or_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_ort(xa, xb););
    unit->mPrevA = xa;
}


void or_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_ort(xa, xb););
    unit->mPrevB = xb;
}


void xor_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_xort(xa, xb););
}

void xor_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_xort(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_xort(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void xor_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_xort(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_xort(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void xor_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_xort(xa, xb););
    unit->mPrevA = xa;
}


void xor_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_xort(xa, xb););
    unit->mPrevB = xb;
}


void rightShift_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_rst(xa, xb););
}

void rightShift_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_rst(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_rst(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void rightShift_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_rst(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_rst(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void rightShift_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_rst(xa, xb););
    unit->mPrevA = xa;
}


void rightShift_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_rst(xa, xb););
    unit->mPrevB = xb;
}


void leftShift_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_lst(xa, xb););
}

void leftShift_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_lst(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_lst(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void leftShift_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_lst(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_lst(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void leftShift_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_lst(xa, xb););
    unit->mPrevA = xa;
}


void leftShift_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_lst(xa, xb););
    unit->mPrevB = xb;
}


void lcm_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_lcm(xa, xb););
}

void lcm_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_lcm(xa, xb););
        unit->mPrevB = xb;
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_lcm(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void lcm_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_lcm(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_lcm(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void lcm_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_lcm(xa, xb););
    unit->mPrevA = xa;
}


void lcm_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_lcm(xa, xb););
    unit->mPrevB = xb;
}


void gcd_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_gcd(xa, xb););
}

void gcd_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_gcd(xa, xb););
        unit->mPrevB = xb;
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_gcd(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void gcd_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_gcd(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_gcd(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void gcd_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_gcd(xa, xb););
    unit->mPrevA = xa;
}


void gcd_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_gcd(xa, xb););
    unit->mPrevB = xb;
}


void amclip_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_amclip(xa, xb););
}

void amclip_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb > 0.f) {
            LOOP1(inNumSamples, ZXP(out) = ZXP(a) * xb;);
        } else {
            ZClear(inNumSamples, out);
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_amclip(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void amclip_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_amclip(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_amclip(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void amclip_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_amclip(xa, xb););
    unit->mPrevA = xa;
}


void amclip_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_amclip(xa, xb););
    unit->mPrevB = xb;
}


void scaleneg_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa >= 0.f ? xa : xa * xb;);
}

void scaleneg_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa >= 0.f ? xa : xa * xb;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa >= 0.f ? xa : xa * xb; xb += slope;);
        unit->mPrevB = xb;
    }
}

void scaleneg_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa >= 0.f) {
            LOOP1(inNumSamples, ZXP(out) = xa;);
        } else {
            LOOP1(inNumSamples, ZXP(out) = xa * ZXP(b););
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa >= 0.f ? xa : xa * xb; xa += slope;);
        unit->mPrevA = xa;
    }
}

void scaleneg_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa >= 0.f ? xa : xa * xb;);
    unit->mPrevA = xa;
}


void scaleneg_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa >= 0.f ? xa : xa * xb;);
    unit->mPrevB = xb;
}


void pow_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa >= 0.f ? pow(xa, xb) : -pow(-xa, xb););
}


void pow_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa >= 0.f ? pow(xa, xb) : -pow(-xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa >= 0.f ? pow(xa, xb) : -pow(-xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void pow_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa >= 0.f) {
            LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = pow(xa, xb););
        } else {
            LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = -pow(-xa, xb););
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa >= 0.f ? pow(xa, xb) : -pow(-xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void pow_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa >= 0.f ? pow(xa, xb) : -pow(-xa, xb););
    unit->mPrevA = xa;
}


void pow_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa >= 0.f ? pow(xa, xb) : -pow(-xa, xb););
    unit->mPrevB = xb;
}

#ifdef NOVA_SIMD
FLATTEN void pow_aa_nova(BinaryOpUGen* unit, int inNumSamples) {
    nova::spow_vec_simd(OUT(0), IN(0), IN(1), inNumSamples);
}

FLATTEN void pow_ak_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b)
        nova::spow_vec_simd(OUT(0), IN(0), xb, inNumSamples);
    else {
        float slope = CALCSLOPE(next_b, xb);
        nova::spow_vec_simd(OUT(0), IN(0), slope_argument(xb, slope), inNumSamples);
        unit->mPrevB = next_b;
    }
}

FLATTEN void pow_ka_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xa = unit->mPrevA;
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa >= 0.f)
            nova::pow_vec_simd(OUT(0), xa, IN(1), inNumSamples);
        else
            nova::spow_vec_simd(OUT(0), xa, IN(1), inNumSamples);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        nova::spow_vec_simd(OUT(0), slope_argument(xa, slope), IN(1), inNumSamples);
        unit->mPrevA = next_a;
    }
}


FLATTEN void pow_ia_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xa = ZIN0(0);
    if (xa > 0.f)
        nova::pow_vec_simd(OUT(0), xa, IN(1), inNumSamples);
    else
        nova::spow_vec_simd(OUT(0), xa, IN(1), inNumSamples);
    unit->mPrevA = xa;
}


FLATTEN void pow_ai_nova(BinaryOpUGen* unit, int inNumSamples) {
    float xb = ZIN0(1);
    nova::spow_vec_simd(OUT(0), IN(0), xb, inNumSamples);
}
#endif

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER(ring1, sc_ring1)
NOVA_BINARY_WRAPPER(ring2, sc_ring2)
NOVA_BINARY_WRAPPER(ring3, sc_ring3)
NOVA_BINARY_WRAPPER(ring4, sc_ring4)
#endif


void ring1_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa * xb + xa;);
}

void ring1_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f) {
            ZCopy(inNumSamples, out, a);
        } else if (xb == 1.f) {
            LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa + xa;);
        } else {
            LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xb + xa;);
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xb + xa; xb += slope;);
        unit->mPrevB = xb;
    }
}

void ring1_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f) {
            LOOP1(inNumSamples, ZXP(out) = 0.f;);
        } else {
            LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xb + xa;);
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xb + xa; xa += slope;);
        unit->mPrevA = xa;
    }
}

void ring1_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xb + xa;);
    unit->mPrevA = xa;
}


void ring1_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xb + xa;);
    unit->mPrevB = xb;
}


void ring2_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa * xb + xa + xb;);
}

void ring2_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f) {
            ZCopy(inNumSamples, out, a);
        } else {
            LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xb + xa + xb;);
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xb + xa + xb; xb += slope;);
        unit->mPrevB = xb;
    }
}

void ring2_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f) {
            ZCopy(inNumSamples, out, b);
        } else {
            LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xb + xa + xb;);
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xb + xa + xb; xa += slope;);
        unit->mPrevA = xa;
    }
}

void ring2_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xb + xa + xb;);
    unit->mPrevA = xa;
}


void ring2_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xb + xa + xb;);
    unit->mPrevB = xb;
}


void ring3_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa * xa * xb;);
}

void ring3_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f) {
            ZClear(inNumSamples, out);
        } else if (xb == 1.f) {
            LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa;);
        } else {
            LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa * xb;);
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa * xb; xb += slope;);
        unit->mPrevB = xb;
    }
}

void ring3_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f) {
            ZClear(inNumSamples, out);
        } else if (xa == 1.f) {
            ZCopy(inNumSamples, out, b);
        } else {
            LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa * xb;);
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa * xb; xa += slope;);
        unit->mPrevA = xa;
    }
}

void ring3_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa * xb;);
    unit->mPrevA = xa;
}


void ring3_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa * xb;);
    unit->mPrevB = xb;
}


void ring4_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa * xa * xb - xa * xb * xb;);
}

void ring4_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        if (xb == 0.f) {
            ZClear(inNumSamples, out);
        } else if (xb == 1.f) {
            LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa - xa;);
        } else {
            LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa * xb - xa * xb * xb;);
        }
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa * xb - xa * xb * xb; xb += slope;);
        unit->mPrevB = xb;
    }
}

void ring4_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        if (xa == 0.f) {
            ZClear(inNumSamples, out);
        } else if (xa == 1.f) {
            LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xb - xb * xb;);
        } else {
            LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa * xb - xa * xb * xb;);
        }
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa * xb - xa * xb * xb; xa += slope;);
        unit->mPrevA = xa;
    }
}

void ring4_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa * xb - xa * xb * xb;);
    unit->mPrevA = xa;
}


void ring4_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa * xb - xa * xb * xb;);
    unit->mPrevB = xb;
}


void thresh_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa < xb ? 0.f : xa;);
}

void thresh_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa < xb ? 0.f : xa;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa < xb ? 0.f : xa; xb += slope;);
        unit->mPrevB = xb;
    }
}

void thresh_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa < xb ? 0.f : xa;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa < xb ? 0.f : xa; xa += slope;);
        unit->mPrevA = xa;
    }
}

void thresh_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa < xb ? 0.f : xa;);
    unit->mPrevA = xa;
}


void thresh_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa < xb ? 0.f : xa;);
    unit->mPrevB = xb;
}


void clip2_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa > xb ? xb : (xa < -xb ? -xb : xa););
}

void clip2_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa > xb ? xb : (xa < -xb ? -xb : xa););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa > xb ? xb : (xa < -xb ? -xb : xa); xb += slope;);
        unit->mPrevB = xb;
    }
}

void clip2_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa > xb ? xb : (xa < -xb ? -xb : xa););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa > xb ? xb : (xa < -xb ? -xb : xa); xa += slope;);
        unit->mPrevA = xa;
    }
}

void clip2_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa > xb ? xb : (xa < -xb ? -xb : xa););
    unit->mPrevA = xa;
}


void clip2_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa > xb ? xb : (xa < -xb ? -xb : xa););
    unit->mPrevB = xb;
}


#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER_K(clip2, clip2)
#endif


void excess_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b);
          ZXP(out) = xa > xb ? xa - xb : (xa < -xb ? xa + xb : 0.f););
}

void excess_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa > xb ? xa - xb : (xa < -xb ? xa + xb : 0.f););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa > xb ? xa - xb : (xa < -xb ? xa + xb : 0.f); xb += slope;);
        unit->mPrevB = xb;
    }
}

void excess_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa > xb ? xa - xb : (xa < -xb ? xa + xb : 0.f););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa > xb ? xa - xb : (xa < -xb ? xa + xb : 0.f); xa += slope;);
        unit->mPrevA = xa;
    }
}

void excess_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa > xb ? xa - xb : (xa < -xb ? xa + xb : 0.f););
    unit->mPrevA = xa;
}

void excess_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa > xb ? xa - xb : (xa < -xb ? xa + xb : 0.f););
    unit->mPrevB = xb;
}

void rrand_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    RGET

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b);
          ZXP(out) = xb > xa ? xa + frand2(s1, s2, s3) * (xb - xa) : (xb + frand2(s1, s2, s3) * (xa - xb)););

    RPUT
}

void rrand_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    RGET

        if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a);
              ZXP(out) = xb > xa ? xa + frand2(s1, s2, s3) * (xb - xa) : (xb + frand2(s1, s2, s3) * (xa - xb)););
    }
    else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a);
              ZXP(out) = xb > xa ? xa + frand2(s1, s2, s3) * (xb - xa) : (xb + frand2(s1, s2, s3) * (xa - xb));
              xb += slope;);
        unit->mPrevB = xb;
    }

    RPUT
}

void rrand_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    RGET

        if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b);
              ZXP(out) = xb > xa ? xa + frand2(s1, s2, s3) * (xb - xa) : (xb + frand2(s1, s2, s3) * (xa - xb)););
    }
    else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b);
              ZXP(out) = xb > xa ? xa + frand2(s1, s2, s3) * (xb - xa) : (xb + frand2(s1, s2, s3) * (xa - xb));
              xa += slope;);
        unit->mPrevA = xa;
    }

    RPUT
}

void rrand_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    RGET

    LOOP1(inNumSamples, float xb = ZXP(b);
          ZXP(out) = xb > xa ? xa + frand2(s1, s2, s3) * (xb - xa) : (xb + frand2(s1, s2, s3) * (xa - xb)););
    unit->mPrevA = xa;

    RPUT
}


void rrand_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    RGET

    LOOP1(inNumSamples, float xa = ZXP(a);
          ZXP(out) = xb > xa ? xa + frand2(s1, s2, s3) * (xb - xa) : (xb + frand2(s1, s2, s3) * (xa - xb)););

    RPUT

        unit->mPrevB = xb;
}


void exprand_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); RGen& rgen = *unit->mParent->mRGen;
          ZXP(out) = xb > xa ? rgen.exprandrng(xa, xb) : rgen.exprandrng(xb, xa););
}

void exprand_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);
    RGen& rgen = *unit->mParent->mRGen;

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xb > xa ? rgen.exprandrng(xa, xb) : rgen.exprandrng(xb, xa););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xb > xa ? rgen.exprandrng(xa, xb) : rgen.exprandrng(xb, xa);
              xb += slope;);
        unit->mPrevB = xb;
    }
}

void exprand_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);
    RGen& rgen = *unit->mParent->mRGen;

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xb > xa ? rgen.exprandrng(xa, xb) : rgen.exprandrng(xb, xa););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xb > xa ? rgen.exprandrng(xa, xb) : rgen.exprandrng(xb, xa);
              xa += slope;);
        unit->mPrevA = xa;
    }
}

void exprand_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); RGen& rgen = *unit->mParent->mRGen;
          ZXP(out) = xb > xa ? rgen.exprandrng(xa, xb) : rgen.exprandrng(xb, xa););
    unit->mPrevA = xa;
}


void exprand_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); RGen& rgen = *unit->mParent->mRGen;
          ZXP(out) = xb > xa ? rgen.exprandrng(xa, xb) : rgen.exprandrng(xb, xa););
    unit->mPrevB = xb;
}


void lt_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa < xb ? 1.f : 0.f;);
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER_K(lt, less)
#endif


void lt_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa < xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa < xb ? 1.f : 0.f; xb += slope;);
        unit->mPrevB = xb;
    }
}

void lt_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa < xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa < xb ? 1.f : 0.f; xa += slope;);
        unit->mPrevA = xa;
    }
}

void lt_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa < xb ? 1.f : 0.f;);
    unit->mPrevA = xa;
}


void lt_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa < xb ? 1.f : 0.f;);
    unit->mPrevB = xb;
}


void le_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa <= xb ? 1.f : 0.f;);
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER_K(le, less_equal)
#endif

void le_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa <= xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa <= xb ? 1.f : 0.f; xb += slope;);
        unit->mPrevB = xb;
    }
}

void le_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa <= xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa <= xb ? 1.f : 0.f; xa += slope;);
        unit->mPrevA = xa;
    }
}

void le_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa <= xb ? 1.f : 0.f;);
    unit->mPrevA = xa;
}


void le_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa <= xb ? 1.f : 0.f;);
    unit->mPrevB = xb;
}


void gt_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa > xb ? 1.f : 0.f;);
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER_K(gt, greater)
#endif


void gt_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa > xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa > xb ? 1.f : 0.f; xb += slope;);
        unit->mPrevB = xb;
    }
}

void gt_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa > xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa > xb ? 1.f : 0.f; xa += slope;);
        unit->mPrevA = xa;
    }
}

void gt_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa > xb ? 1.f : 0.f;);
    unit->mPrevA = xa;
}


void gt_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa > xb ? 1.f : 0.f;);
    unit->mPrevB = xb;
}


void ge_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa >= xb ? 1.f : 0.f;);
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER_K(ge, greater_equal)
#endif


void ge_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa >= xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa >= xb ? 1.f : 0.f; xb += slope;);
        unit->mPrevB = xb;
    }
}

void ge_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa >= xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa >= xb ? 1.f : 0.f; xa += slope;);
        unit->mPrevA = xa;
    }
}

void ge_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa >= xb ? 1.f : 0.f;);
    unit->mPrevA = xa;
}


void ge_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa >= xb ? 1.f : 0.f;);
    unit->mPrevB = xb;
}


void eq_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa == xb ? 1.f : 0.f;);
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER_K(eq, equal)
#endif

void eq_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa == xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa == xb ? 1.f : 0.f; xb += slope;);
        unit->mPrevB = xb;
    }
}

void eq_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa == xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa == xb ? 1.f : 0.f; xa += slope;);
        unit->mPrevA = xa;
    }
}

void eq_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa == xb ? 1.f : 0.f;);
    unit->mPrevA = xa;
}


void eq_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa == xb ? 1.f : 0.f;);
    unit->mPrevB = xb;
}


void neq_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa != xb ? 1.f : 0.f;);
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER_K(neq, notequal)
#endif


void neq_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa != xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa != xb ? 1.f : 0.f; xb += slope;);
        unit->mPrevB = xb;
    }
}

void neq_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa != xb ? 1.f : 0.f;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa != xb ? 1.f : 0.f; xa += slope;);
        unit->mPrevA = xa;
    }
}

void neq_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa != xb ? 1.f : 0.f;);
    unit->mPrevA = xa;
}


void neq_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa != xb ? 1.f : 0.f;);
    unit->mPrevB = xb;
}

#ifdef NOVA_SIMD
NOVA_BINARY_WRAPPER_K(sumsqr, sc_sumsqr)
NOVA_BINARY_WRAPPER_K(difsqr, sc_difsqr)
NOVA_BINARY_WRAPPER_K(sqrsum, sc_sqrsum)
NOVA_BINARY_WRAPPER_K(sqrdif, sc_sqrdif)
#endif

void sumsqr_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa * xa + xb * xb;);
}

void sumsqr_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa + xb * xb;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa + xb * xb; xb += slope;);
        unit->mPrevB = xb;
    }
}

void sumsqr_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa + xb * xb;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa + xb * xb; xa += slope;);
        unit->mPrevA = xa;
    }
}


void sumsqr_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa + xb * xb;);
    unit->mPrevA = xa;
}


void sumsqr_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa + xb * xb;);
    unit->mPrevB = xb;
}


void difsqr_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = xa * xa - xb * xb;);
}

void difsqr_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa - xb * xb;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa - xb * xb; xb += slope;);
        unit->mPrevB = xb;
    }
}

void difsqr_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa - xb * xb;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa - xb * xb; xa += slope;);
        unit->mPrevA = xa;
    }
}

void difsqr_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = xa * xa - xb * xb;);
    unit->mPrevA = xa;
}


void difsqr_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = xa * xa - xb * xb;);
    unit->mPrevB = xb;
}


void sqrsum_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float sum = ZXP(a) + ZXP(b); ZXP(out) = sum * sum;);
}

void sqrsum_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); float sum = xa + xb; ZXP(out) = sum * sum;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); float sum = xa + xb; ZXP(out) = sum * sum; xb += slope;);
        unit->mPrevB = xb;
    }
}

void sqrsum_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); float sum = xa + xb; ZXP(out) = sum * sum;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); float sum = xa + xb; ZXP(out) = sum * sum; xa += slope;);
        unit->mPrevA = xa;
    }
}

void sqrsum_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); float sum = xa + xb; ZXP(out) = sum * sum;);
    unit->mPrevA = xa;
}


void sqrsum_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float sum = xa + xb; ZXP(out) = sum * sum;);
    unit->mPrevB = xb;
}


void sqrdif_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float dif = ZXP(a) - ZXP(b); ZXP(out) = dif * dif;);
}

void sqrdif_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); float dif = xa - xb; ZXP(out) = dif * dif;);
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); float dif = xa - xb; ZXP(out) = dif * dif; xb += slope;);
        unit->mPrevB = xb;
    }
}

void sqrdif_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); float dif = xa - xb; ZXP(out) = dif * dif;);
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); float dif = xa - xb; ZXP(out) = dif * dif; xa += slope;);
        unit->mPrevA = xa;
    }
}


void sqrdif_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); float dif = xa - xb; ZXP(out) = dif * dif;);
    unit->mPrevA = xa;
}


void sqrdif_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float dif = xa - xb; ZXP(out) = dif * dif;);
    unit->mPrevB = xb;
}


void absdif_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float dif = ZXP(a) - ZXP(b); ZXP(out) = fabs(dif););
}

void absdif_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); float dif = xa - xb; ZXP(out) = fabs(dif););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); float dif = xa - xb; ZXP(out) = fabs(dif); xb += slope;);
        unit->mPrevB = xb;
    }
}

void absdif_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); float dif = xa - xb; ZXP(out) = fabs(dif););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); float dif = xa - xb; ZXP(out) = fabs(dif); xa += slope;);
        unit->mPrevA = xa;
    }
}

void absdif_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); float dif = xa - xb; ZXP(out) = fabs(dif););
    unit->mPrevA = xa;
}


void absdif_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float dif = xa - xb; ZXP(out) = fabs(dif););
    unit->mPrevB = xb;
}


void round_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_round(xa, xb););
}

void round_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_round(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_round(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void round_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_round(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_round(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}


void round_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_round(xa, xb););
    unit->mPrevA = xa;
}


void round_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_round(xa, xb););
    unit->mPrevB = xb;
}


void roundUp_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_roundUp(xa, xb););
}

void roundUp_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_roundUp(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_roundUp(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void roundUp_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_roundUp(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_roundUp(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}


void roundUp_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_roundUp(xa, xb););
    unit->mPrevA = xa;
}


void roundUp_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_roundUp(xa, xb););
    unit->mPrevB = xb;
}


void trunc_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_trunc(xa, xb););
}

void trunc_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_trunc(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_trunc(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void trunc_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_trunc(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_trunc(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void trunc_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_trunc(xa, xb););
    unit->mPrevA = xa;
}


void trunc_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_trunc(xa, xb););
    unit->mPrevB = xb;
}


void fold2_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_fold(xa, -xb, xb););
}

void fold2_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_fold(xa, -xb, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_fold(xa, -xb, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void fold2_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_fold(xa, -xb, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_fold(xa, -xb, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void fold2_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_fold(xa, -xb, xb););
    unit->mPrevA = xa;
}


void fold2_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_fold(xa, -xb, xb););
    unit->mPrevB = xb;
}


void wrap2_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_wrap(xa, -xb, xb););
}

void wrap2_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_wrap(xa, -xb, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_wrap(xa, -xb, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void wrap2_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_wrap(xa, -xb, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_wrap(xa, -xb, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void wrap2_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_wrap(xa, -xb, xb););
    unit->mPrevA = xa;
}


void wrap2_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_wrap(xa, -xb, xb););
    unit->mPrevB = xb;
}


void atan2_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = atan2(xa, xb););
}

void atan2_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = atan2(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = atan2(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void atan2_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = atan2(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = atan2(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void atan2_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = atan2(xa, xb););
    unit->mPrevA = xa;
}


void atan2_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = atan2(xa, xb););
    unit->mPrevB = xb;
}


void hypot_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = hypotf(xa, xb););
}

void hypot_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = hypotf(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = hypotf(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void hypot_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = hypotf(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = hypotf(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void hypot_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = hypotf(xa, xb););
    unit->mPrevA = xa;
}


void hypot_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = hypotf(xa, xb););
    unit->mPrevB = xb;
}


void hypotx_aa(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xa = ZXP(a); float xb = ZXP(b); ZXP(out) = sc_hypotx(xa, xb););
}

void hypotx_ak(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = unit->mPrevB;
    float next_b = ZIN0(1);

    if (xb == next_b) {
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_hypotx(xa, xb););
    } else {
        float slope = CALCSLOPE(next_b, xb);
        LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_hypotx(xa, xb); xb += slope;);
        unit->mPrevB = xb;
    }
}

void hypotx_ka(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = unit->mPrevA;
    float* b = ZIN(1);
    float next_a = ZIN0(0);

    if (xa == next_a) {
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_hypotx(xa, xb););
    } else {
        float slope = CALCSLOPE(next_a, xa);
        LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_hypotx(xa, xb); xa += slope;);
        unit->mPrevA = xa;
    }
}

void hypotx_ia(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float xa = ZIN0(0);
    float* b = ZIN(1);

    LOOP1(inNumSamples, float xb = ZXP(b); ZXP(out) = sc_hypotx(xa, xb););
    unit->mPrevA = xa;
}


void hypotx_ai(BinaryOpUGen* unit, int inNumSamples) {
    float* out = ZOUT(0);
    float* a = ZIN(0);
    float xb = ZIN0(1);

    LOOP1(inNumSamples, float xa = ZXP(a); ZXP(out) = sc_hypotx(xa, xb););
    unit->mPrevB = xb;
}

static BinaryOpFunc ChooseOneSampleFunc(BinaryOpUGen* unit) {
    BinaryOpFunc func = &zero_1;

    switch (unit->mSpecialIndex) {
    // case opSilence2 : func = &zero_1; break;
    case opAdd:
        func = &add_1;
        break;
    case opSub:
        func = &sub_1;
        break;
    case opMul:
        func = &mul_1;
        break;
    case opFDiv:
        func = &div_1;
        break;
    case opIDiv:
        func = &idiv_1;
        break;
    case opMod:
        func = &mod_1;
        break;
    case opEQ:
        func = &eq_1;
        break;
    case opNE:
        func = &neq_1;
        break;
    case opLT:
        func = &lt_1;
        break;
    case opGT:
        func = &gt_1;
        break;
    case opLE:
        func = &le_1;
        break;
    case opGE:
        func = &ge_1;
        break;
    case opMin:
        func = &min_1;
        break;
    case opMax:
        func = &max_1;
        break;
    case opBitAnd:
        func = &and_1;
        break;
    case opBitOr:
        func = &or_1;
        break;
    case opBitXor:
        func = &xor_1;
        break;
    case opShiftRight:
        func = &rightShift_1;
        break;
    case opShiftLeft:
        func = &leftShift_1;
        break;
    case opLCM:
        func = &lcm_1;
        break;
    case opGCD:
        func = &gcd_1;
        break;
    case opRound:
        func = &round_1;
        break;
    case opRoundUp:
        func = &roundUp_1;
        break;
    case opTrunc:
        func = &trunc_1;
        break;
    case opAtan2:
        func = &atan2_1;
        break;
    case opHypot:
        func = &hypot_1;
        break;
    case opHypotx:
        func = &hypotx_1;
        break;
    case opPow:
        func = &pow_1;
        break;
    case opRing1:
        func = &ring1_1;
        break;
    case opRing2:
        func = &ring2_1;
        break;
    case opRing3:
        func = &ring3_1;
        break;
    case opRing4:
        func = &ring4_1;
        break;
    case opDifSqr:
        func = &difsqr_1;
        break;
    case opSumSqr:
        func = &sumsqr_1;
        break;
    case opSqrSum:
        func = &sqrsum_1;
        break;
    case opSqrDif:
        func = &sqrdif_1;
        break;
    case opAbsDif:
        func = &absdif_1;
        break;
    case opThresh:
        func = &thresh_1;
        break;
    case opAMClip:
        func = &amclip_1;
        break;
    case opScaleNeg:
        func = &scaleneg_1;
        break;
    case opClip2:
        func = &clip2_1;
        break;
    case opFold2:
        func = &fold2_1;
        break;
    case opWrap2:
        func = &wrap2_1;
        break;
    case opExcess:
        func = &excess_1;
        break;
    case opFirstArg:
        func = &firstarg_1;
        break;
    case opRandRange:
        func = &rrand_1;
        break;
    case opExpRandRange:
        func = &exprand_1;
        break;
    // case opSecondArg : func = &secondarg_1; break;
    default:
        func = &add_1;
        break;
    }
    return func;
}


static BinaryOpFunc ChooseDemandFunc(BinaryOpUGen* unit) {
    BinaryOpFunc func = &zero_1;

    switch (unit->mSpecialIndex) {
    // case opSilence2 : func = &zero_d; break;
    case opAdd:
        func = &add_d;
        break;
    case opSub:
        func = &sub_d;
        break;
    case opMul:
        func = &mul_d;
        break;
    case opFDiv:
        func = &div_d;
        break;
    case opIDiv:
        func = &idiv_d;
        break;
    case opMod:
        func = &mod_d;
        break;
    case opEQ:
        func = &eq_d;
        break;
    case opNE:
        func = &neq_d;
        break;
    case opLT:
        func = &lt_d;
        break;
    case opGT:
        func = &gt_d;
        break;
    case opLE:
        func = &le_d;
        break;
    case opGE:
        func = &ge_d;
        break;
    case opMin:
        func = &min_d;
        break;
    case opMax:
        func = &max_d;
        break;
    case opBitAnd:
        func = &and_d;
        break;
    case opBitOr:
        func = &or_d;
        break;
    case opBitXor:
        func = &xor_d;
        break;
    case opShiftRight:
        func = &rightShift_d;
        break;
    case opShiftLeft:
        func = &leftShift_d;
        break;
    case opLCM:
        func = &lcm_d;
        break;
    case opGCD:
        func = &gcd_d;
        break;
    case opRound:
        func = &round_d;
        break;
    case opRoundUp:
        func = &roundUp_d;
        break;
    case opTrunc:
        func = &trunc_d;
        break;
    case opAtan2:
        func = &atan2_d;
        break;
    case opHypot:
        func = &hypot_d;
        break;
    case opHypotx:
        func = &hypotx_d;
        break;
    case opPow:
        func = &pow_d;
        break;
    case opRing1:
        func = &ring1_d;
        break;
    case opRing2:
        func = &ring2_d;
        break;
    case opRing3:
        func = &ring3_d;
        break;
    case opRing4:
        func = &ring4_d;
        break;
    case opDifSqr:
        func = &difsqr_d;
        break;
    case opSumSqr:
        func = &sumsqr_d;
        break;
    case opSqrSum:
        func = &sqrsum_d;
        break;
    case opSqrDif:
        func = &sqrdif_d;
        break;
    case opAbsDif:
        func = &absdif_d;
        break;
    case opThresh:
        func = &thresh_d;
        break;
    case opAMClip:
        func = &amclip_d;
        break;
    case opScaleNeg:
        func = &scaleneg_d;
        break;
    case opClip2:
        func = &clip2_d;
        break;
    case opFold2:
        func = &fold2_d;
        break;
    case opWrap2:
        func = &wrap2_d;
        break;
    case opExcess:
        func = &excess_d;
        break;
    case opFirstArg:
        func = &firstarg_d;
        break;
    case opRandRange:
        func = &rrand_d;
        break;
    case opExpRandRange:
        func = &exprand_d;
        break;

    // case opSecondArg : func = &secondarg_d; break;
    default:
        func = &add_d;
        break;
    }
    return func;
}


static BinaryOpFunc ChooseNormalFunc(BinaryOpUGen* unit) {
    BinaryOpFunc func = &zero_1;

    int rateA = INRATE(0);
    int rateB = INRATE(1);

    switch (rateA) {
    case calc_FullRate:
        switch (rateB) {
        case calc_FullRate:
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_aa;
                break;
            case opSub:
                func = &sub_aa;
                break;
            case opMul:
                func = &mul_aa;
                break;
            case opFDiv:
                func = &div_aa;
                break;
            case opIDiv:
                func = &idiv_aa;
                break;
            case opMod:
                func = &mod_aa;
                break;
            case opEQ:
                func = &eq_aa;
                break;
            case opNE:
                func = &neq_aa;
                break;
            case opLT:
                func = &lt_aa;
                break;
            case opGT:
                func = &gt_aa;
                break;
            case opLE:
                func = &le_aa;
                break;
            case opGE:
                func = &ge_aa;
                break;
            case opMin:
                func = &min_aa;
                break;
            case opMax:
                func = &max_aa;
                break;
            case opBitAnd:
                func = &and_aa;
                break;
            case opBitOr:
                func = &or_aa;
                break;
            case opBitXor:
                func = &xor_aa;
                break;
            case opShiftRight:
                func = &rightShift_aa;
                break;
            case opShiftLeft:
                func = &leftShift_aa;
                break;
            case opLCM:
                func = &lcm_aa;
                break;
            case opGCD:
                func = &gcd_aa;
                break;
            case opRound:
                func = &round_aa;
                break;
            case opRoundUp:
                func = &roundUp_aa;
                break;
            case opTrunc:
                func = &trunc_aa;
                break;
            case opAtan2:
                func = &atan2_aa;
                break;
            case opHypot:
                func = &hypot_aa;
                break;
            case opHypotx:
                func = &hypotx_aa;
                break;
            case opPow:
                func = &pow_aa;
                break;
            case opRing1:
                func = &ring1_aa;
                break;
            case opRing2:
                func = &ring2_aa;
                break;
            case opRing3:
                func = &ring3_aa;
                break;
            case opRing4:
                func = &ring4_aa;
                break;
            case opDifSqr:
                func = &difsqr_aa;
                break;
            case opSumSqr:
                func = &sumsqr_aa;
                break;
            case opSqrSum:
                func = &sqrsum_aa;
                break;
            case opSqrDif:
                func = &sqrdif_aa;
                break;
            case opAbsDif:
                func = &absdif_aa;
                break;
            case opThresh:
                func = &thresh_aa;
                break;
            case opAMClip:
                func = &amclip_aa;
                break;
            case opScaleNeg:
                func = &scaleneg_aa;
                break;
            case opClip2:
                func = &clip2_aa;
                break;
            case opFold2:
                func = &fold2_aa;
                break;
            case opWrap2:
                func = &wrap2_aa;
                break;
            case opExcess:
                func = &excess_aa;
                break;
            case opRandRange:
                func = &rrand_aa;
                break;
            case opExpRandRange:
                func = &exprand_aa;
                break;
            case opFirstArg:
                func = &firstarg_aa;
                break;
                // case opSecondArg : func = &secondarg_aa; break;


            default:
                func = &add_aa;
                break;
            }
            break;
        case calc_BufRate:
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ak;
                break;
            case opSub:
                func = &sub_ak;
                break;
            case opMul:
                func = &mul_ak;
                break;
            case opFDiv:
                func = &div_ak;
                break;
            case opIDiv:
                func = &idiv_ak;
                break;
            case opMod:
                func = &mod_ak;
                break;
            case opEQ:
                func = &eq_ak;
                break;
            case opNE:
                func = &neq_ak;
                break;
            case opLT:
                func = &lt_ak;
                break;
            case opGT:
                func = &gt_ak;
                break;
            case opLE:
                func = &le_ak;
                break;
            case opGE:
                func = &ge_ak;
                break;
            case opMin:
                func = &min_ak;
                break;
            case opMax:
                func = &max_ak;
                break;
            case opBitAnd:
                func = &and_ak;
                break;
            case opBitOr:
                func = &or_ak;
                break;
            case opBitXor:
                func = &xor_ak;
                break;
            case opShiftRight:
                func = &rightShift_ak;
                break;
            case opShiftLeft:
                func = &leftShift_ak;
                break;
            case opLCM:
                func = &lcm_ak;
                break;
            case opGCD:
                func = &gcd_ak;
                break;
            case opRound:
                func = &round_ak;
                break;
            case opRoundUp:
                func = &roundUp_ak;
                break;
            case opTrunc:
                func = &trunc_ak;
                break;
            case opAtan2:
                func = &atan2_ak;
                break;
            case opHypot:
                func = &hypot_ak;
                break;
            case opHypotx:
                func = &hypotx_ak;
                break;
            case opPow:
                func = &pow_ak;
                break;
            case opRing1:
                func = &ring1_ak;
                break;
            case opRing2:
                func = &ring2_ak;
                break;
            case opRing3:
                func = &ring3_ak;
                break;
            case opRing4:
                func = &ring4_ak;
                break;
            case opDifSqr:
                func = &difsqr_ak;
                break;
            case opSumSqr:
                func = &sumsqr_ak;
                break;
            case opSqrSum:
                func = &sqrsum_ak;
                break;
            case opSqrDif:
                func = &sqrdif_ak;
                break;
            case opAbsDif:
                func = &absdif_ak;
                break;
            case opThresh:
                func = &thresh_ak;
                break;
            case opAMClip:
                func = &amclip_ak;
                break;
            case opScaleNeg:
                func = &scaleneg_ak;
                break;
            case opClip2:
                func = &clip2_ak;
                break;
            case opFold2:
                func = &fold2_ak;
                break;
            case opWrap2:
                func = &wrap2_ak;
                break;
            case opExcess:
                func = &excess_ak;
                break;
            case opRandRange:
                func = &rrand_ak;
                break;
            case opExpRandRange:
                func = &exprand_ak;
                break;
            case opFirstArg:
                func = &firstarg_aa;
                break;
                // case opSecondArg : func = &secondarg_aa; break;


            default:
                func = &add_ak;
                break;
            }
            break;
        case calc_ScalarRate:
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ai;
                break;
            case opSub:
                func = &sub_ai;
                break;
            case opMul:
                func = &mul_ai;
                break;
            case opFDiv:
                func = &div_ai;
                break;
            case opIDiv:
                func = &idiv_ai;
                break;
            case opMod:
                func = &mod_ai;
                break;
            case opEQ:
                func = &eq_ai;
                break;
            case opNE:
                func = &neq_ai;
                break;
            case opLT:
                func = &lt_ai;
                break;
            case opGT:
                func = &gt_ai;
                break;
            case opLE:
                func = &le_ai;
                break;
            case opGE:
                func = &ge_ai;
                break;
            case opMin:
                func = &min_ai;
                break;
            case opMax:
                func = &max_ai;
                break;
            case opBitAnd:
                func = &and_ai;
                break;
            case opBitOr:
                func = &or_ai;
                break;
            case opBitXor:
                func = &xor_ai;
                break;
            case opShiftRight:
                func = &rightShift_ai;
                break;
            case opShiftLeft:
                func = &leftShift_ai;
                break;
            case opLCM:
                func = &lcm_ai;
                break;
            case opGCD:
                func = &gcd_ai;
                break;
            case opRound:
                func = &round_ai;
                break;
            case opRoundUp:
                func = &roundUp_ai;
                break;
            case opTrunc:
                func = &trunc_ai;
                break;
            case opAtan2:
                func = &atan2_ai;
                break;
            case opHypot:
                func = &hypot_ai;
                break;
            case opHypotx:
                func = &hypotx_ai;
                break;
            case opPow:
                func = &pow_ai;
                break;
            case opRing1:
                func = &ring1_ai;
                break;
            case opRing2:
                func = &ring2_ai;
                break;
            case opRing3:
                func = &ring3_ai;
                break;
            case opRing4:
                func = &ring4_ai;
                break;
            case opDifSqr:
                func = &difsqr_ai;
                break;
            case opSumSqr:
                func = &sumsqr_ai;
                break;
            case opSqrSum:
                func = &sqrsum_ai;
                break;
            case opSqrDif:
                func = &sqrdif_ai;
                break;
            case opAbsDif:
                func = &absdif_ai;
                break;
            case opThresh:
                func = &thresh_ai;
                break;
            case opAMClip:
                func = &amclip_ai;
                break;
            case opScaleNeg:
                func = &scaleneg_ai;
                break;
            case opClip2:
                func = &clip2_ai;
                break;
            case opFold2:
                func = &fold2_ai;
                break;
            case opWrap2:
                func = &wrap2_ai;
                break;
            case opExcess:
                func = &excess_ai;
                break;
            case opRandRange:
                func = &rrand_ai;
                break;
            case opExpRandRange:
                func = &exprand_ai;
                break;
            case opFirstArg:
                func = &firstarg_aa;
                break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ai;
                break;
            }
        }
        break;
    case calc_BufRate:
        if (rateB == calc_FullRate) {
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ka;
                break;
            case opSub:
                func = &sub_ka;
                break;
            case opMul:
                func = &mul_ka;
                break;
            case opFDiv:
                func = &div_ka;
                break;
            case opIDiv:
                func = &idiv_ka;
                break;
            case opMod:
                func = &mod_ka;
                break;
            case opEQ:
                func = &eq_ka;
                break;
            case opNE:
                func = &neq_ka;
                break;
            case opLT:
                func = &lt_ka;
                break;
            case opGT:
                func = &gt_ka;
                break;
            case opLE:
                func = &le_ka;
                break;
            case opGE:
                func = &ge_ka;
                break;
            case opMin:
                func = &min_ka;
                break;
            case opMax:
                func = &max_ka;
                break;
            case opBitAnd:
                func = &and_ka;
                break;
            case opBitOr:
                func = &or_ka;
                break;
            case opBitXor:
                func = &xor_ka;
                break;
            case opShiftRight:
                func = &rightShift_ka;
                break;
            case opShiftLeft:
                func = &leftShift_ka;
                break;
            case opLCM:
                func = &lcm_ka;
                break;
            case opGCD:
                func = &gcd_ka;
                break;
            case opRound:
                func = &round_ka;
                break;
            case opRoundUp:
                func = &roundUp_ka;
                break;
            case opTrunc:
                func = &trunc_ka;
                break;
            case opAtan2:
                func = &atan2_ka;
                break;
            case opHypot:
                func = &hypot_ka;
                break;
            case opHypotx:
                func = &hypotx_ka;
                break;
            case opPow:
                func = &pow_ka;
                break;
            case opRing1:
                func = &ring1_ka;
                break;
            case opRing2:
                func = &ring2_ka;
                break;
            case opRing3:
                func = &ring3_ka;
                break;
            case opRing4:
                func = &ring4_ka;
                break;
            case opDifSqr:
                func = &difsqr_ka;
                break;
            case opSumSqr:
                func = &sumsqr_ka;
                break;
            case opSqrSum:
                func = &sqrsum_ka;
                break;
            case opSqrDif:
                func = &sqrdif_ka;
                break;
            case opAbsDif:
                func = &absdif_ka;
                break;
            case opThresh:
                func = &thresh_ka;
                break;
            case opAMClip:
                func = &amclip_ka;
                break;
            case opScaleNeg:
                func = &scaleneg_ka;
                break;
            case opClip2:
                func = &clip2_ka;
                break;
            case opFold2:
                func = &fold2_ka;
                break;
            case opWrap2:
                func = &wrap2_ka;
                break;
            case opExcess:
                func = &excess_ka;
                break;
            case opRandRange:
                func = &rrand_ka;
                break;
            case opExpRandRange:
                func = &exprand_ka;
                break;
            // case opFirstArg : func = &firstarg_aa; break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ka;
                break;
            }
        } else {
            // this should have been caught by mBufLength == 1
            func = &zero_aa;
        }
        break;
    case calc_ScalarRate:
        if (rateB == calc_FullRate) {
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ia;
                break;
            case opSub:
                func = &sub_ia;
                break;
            case opMul:
                func = &mul_ia;
                break;
            case opFDiv:
                func = &div_ia;
                break;
            case opIDiv:
                func = &idiv_ia;
                break;
            case opMod:
                func = &mod_ia;
                break;
            case opEQ:
                func = &eq_ia;
                break;
            case opNE:
                func = &neq_ia;
                break;
            case opLT:
                func = &lt_ia;
                break;
            case opGT:
                func = &gt_ia;
                break;
            case opLE:
                func = &le_ia;
                break;
            case opGE:
                func = &ge_ia;
                break;
            case opMin:
                func = &min_ia;
                break;
            case opMax:
                func = &max_ia;
                break;
            case opBitAnd:
                func = &and_ia;
                break;
            case opBitOr:
                func = &or_ia;
                break;
            case opBitXor:
                func = &xor_ia;
                break;
            case opShiftRight:
                func = &rightShift_ia;
                break;
            case opShiftLeft:
                func = &leftShift_ia;
                break;
            case opLCM:
                func = &lcm_ia;
                break;
            case opGCD:
                func = &gcd_ia;
                break;
            case opRound:
                func = &round_ia;
                break;
            case opRoundUp:
                func = &roundUp_ia;
                break;
            case opTrunc:
                func = &trunc_ia;
                break;
            case opAtan2:
                func = &atan2_ia;
                break;
            case opHypot:
                func = &hypot_ia;
                break;
            case opHypotx:
                func = &hypotx_ia;
                break;
            case opPow:
                func = &pow_ia;
                break;
            case opRing1:
                func = &ring1_ia;
                break;
            case opRing2:
                func = &ring2_ia;
                break;
            case opRing3:
                func = &ring3_ia;
                break;
            case opRing4:
                func = &ring4_ia;
                break;
            case opDifSqr:
                func = &difsqr_ia;
                break;
            case opSumSqr:
                func = &sumsqr_ia;
                break;
            case opSqrSum:
                func = &sqrsum_ia;
                break;
            case opSqrDif:
                func = &sqrdif_ia;
                break;
            case opAbsDif:
                func = &absdif_ia;
                break;
            case opThresh:
                func = &thresh_ia;
                break;
            case opAMClip:
                func = &amclip_ia;
                break;
            case opScaleNeg:
                func = &scaleneg_ia;
                break;
            case opClip2:
                func = &clip2_ia;
                break;
            case opFold2:
                func = &fold2_ia;
                break;
            case opWrap2:
                func = &wrap2_ia;
                break;
            case opExcess:
                func = &excess_ia;
                break;
            case opRandRange:
                func = &rrand_ia;
                break;
            case opExpRandRange:
                func = &exprand_ia;
                break;
            // case opFirstArg : func = &firstarg_aa; break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ia;
                break;
            }
        } else {
            // this should have been caught by mBufLength == 1
            func = &zero_aa;
        }
        break;
    }

    return func;
}

#ifdef NOVA_SIMD
static BinaryOpFunc ChooseNovaSimdFunc_64(BinaryOpUGen* unit) {
    BinaryOpFunc func = &zero_1;

    int rateA = INRATE(0);
    int rateB = INRATE(1);

    switch (rateA) {
    case calc_FullRate:
        switch (rateB) {
        case calc_FullRate:
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_aa_nova_64;
                break;
            case opSub:
                func = &sub_aa_nova_64;
                break;
            case opMul:
                func = &mul_aa_nova_64;
                break;
            case opFDiv:
                func = &div_aa_nova;
                break;
            case opIDiv:
                func = &idiv_aa;
                break;
            case opMod:
                func = &mod_aa;
                break;
            case opEQ:
                func = &eq_aa_nova_64;
                break;
            case opNE:
                func = &neq_aa_nova_64;
                break;
            case opLT:
                func = &lt_aa_nova_64;
                break;
            case opGT:
                func = &gt_aa_nova_64;
                break;
            case opLE:
                func = &le_aa_nova_64;
                break;
            case opGE:
                func = &ge_aa_nova_64;
                break;
            case opMin:
                func = &min_aa_nova_64;
                break;
            case opMax:
                func = &max_aa_nova_64;
                break;
            case opBitAnd:
                func = &and_aa;
                break;
            case opBitOr:
                func = &or_aa;
                break;
            case opBitXor:
                func = &xor_aa;
                break;
            case opShiftRight:
                func = &rightShift_aa;
                break;
            case opShiftLeft:
                func = &leftShift_aa;
                break;
            case opLCM:
                func = &lcm_aa;
                break;
            case opGCD:
                func = &gcd_aa;
                break;
            case opRound:
                func = &round_aa;
                break;
            case opRoundUp:
                func = &roundUp_aa;
                break;
            case opTrunc:
                func = &trunc_aa;
                break;
            case opAtan2:
                func = &atan2_aa;
                break;
            case opHypot:
                func = &hypot_aa;
                break;
            case opHypotx:
                func = &hypotx_aa;
                break;
            case opPow:
                func = &pow_aa_nova;
                break;
            case opRing1:
                func = &ring1_aa_nova_64;
                break;
            case opRing2:
                func = &ring2_aa_nova_64;
                break;
            case opRing3:
                func = &ring3_aa_nova_64;
                break;
            case opRing4:
                func = &ring4_aa_nova_64;
                break;
            case opDifSqr:
                func = &difsqr_aa_nova_64;
                break;
            case opSumSqr:
                func = &sumsqr_aa_nova_64;
                break;
            case opSqrSum:
                func = &sqrsum_aa_nova_64;
                break;
            case opSqrDif:
                func = &sqrdif_aa_nova_64;
                break;
            case opAbsDif:
                func = &absdif_aa;
                break;
            case opThresh:
                func = &thresh_aa;
                break;
            case opAMClip:
                func = &amclip_aa;
                break;
            case opScaleNeg:
                func = &scaleneg_aa;
                break;
            case opClip2:
                func = &clip2_aa_nova_64;
                break;
            case opFold2:
                func = &fold2_aa;
                break;
            case opWrap2:
                func = &wrap2_aa;
                break;
            case opExcess:
                func = &excess_aa;
                break;
            case opRandRange:
                func = &rrand_aa;
                break;
            case opExpRandRange:
                func = &exprand_aa;
                break;
            case opFirstArg:
                func = &firstarg_aa_nova;
                break;
            // case opSecondArg : func = &secondarg_aa_nova; break;
            default:
                func = &add_aa;
                break;
            }
            break;
        case calc_BufRate:
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ak_nova_64;
                break;
            case opSub:
                func = &sub_ak_nova_64;
                break;
            case opMul:
                func = &mul_ak_nova_64;
                break;
            case opFDiv:
                func = &div_ak_nova;
                break;
            case opIDiv:
                func = &idiv_ak;
                break;
            case opMod:
                func = &mod_ak;
                break;
            case opEQ:
                func = &eq_ak_nova_64;
                break;
            case opNE:
                func = &neq_ak_nova_64;
                break;
            case opLT:
                func = &lt_ak_nova_64;
                break;
            case opGT:
                func = &gt_ak_nova_64;
                break;
            case opLE:
                func = &le_ak_nova_64;
                break;
            case opGE:
                func = &ge_ak_nova_64;
                break;
            case opMin:
                func = &min_ak_nova_64;
                break;
            case opMax:
                func = &max_ak_nova_64;
                break;
            case opBitAnd:
                func = &and_ak;
                break;
            case opBitOr:
                func = &or_ak;
                break;
            case opBitXor:
                func = &xor_ak;
                break;
            case opShiftRight:
                func = &rightShift_ak;
                break;
            case opShiftLeft:
                func = &leftShift_ak;
                break;
            case opLCM:
                func = &lcm_ak;
                break;
            case opGCD:
                func = &gcd_ak;
                break;
            case opRound:
                func = &round_ak;
                break;
            case opRoundUp:
                func = &roundUp_ak;
                break;
            case opTrunc:
                func = &trunc_ak;
                break;
            case opAtan2:
                func = &atan2_ak;
                break;
            case opHypot:
                func = &hypot_ak;
                break;
            case opHypotx:
                func = &hypotx_ak;
                break;
            case opPow:
                func = &pow_ak_nova;
                break;
            case opRing1:
                func = &ring1_ak;
                break;
            case opRing2:
                func = &ring2_ak;
                break;
            case opRing3:
                func = &ring3_ak;
                break;
            case opRing4:
                func = &ring4_ak;
                break;
            case opDifSqr:
                func = &difsqr_ak_nova_64;
                break;
            case opSumSqr:
                func = &sumsqr_ak_nova_64;
                break;
            case opSqrSum:
                func = &sqrsum_ak_nova_64;
                break;
            case opSqrDif:
                func = &sqrdif_ak_nova_64;
                break;
            case opAbsDif:
                func = &absdif_ak;
                break;
            case opThresh:
                func = &thresh_ak;
                break;
            case opAMClip:
                func = &amclip_ak;
                break;
            case opScaleNeg:
                func = &scaleneg_ak;
                break;
            case opClip2:
                func = &clip2_ak_nova_64;
                break;
            case opFold2:
                func = &fold2_ak;
                break;
            case opWrap2:
                func = &wrap2_ak;
                break;
            case opExcess:
                func = &excess_ak;
                break;
            case opRandRange:
                func = &rrand_ak;
                break;
            case opExpRandRange:
                func = &exprand_ak;
                break;
            case opFirstArg:
                func = &firstarg_aa;
                break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ak;
                break;
            }
            break;
        case calc_ScalarRate:
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ai_nova_64;
                break;
            case opSub:
                func = &sub_ai_nova_64;
                break;
            case opMul:
                func = &mul_ai_nova_64;
                break;
            case opFDiv:
                func = &div_ai_nova;
                break;
            case opIDiv:
                func = &idiv_ai;
                break;
            case opMod:
                func = &mod_ai;
                break;
            case opEQ:
                func = &eq_ai_nova_64;
                break;
            case opNE:
                func = &neq_ai_nova_64;
                break;
            case opLT:
                func = &lt_ai_nova_64;
                break;
            case opGT:
                func = &gt_ai_nova_64;
                break;
            case opLE:
                func = &le_ai_nova_64;
                break;
            case opGE:
                func = &ge_ai_nova_64;
                break;
            case opMin:
                func = &min_ai_nova_64;
                break;
            case opMax:
                func = &max_ai_nova_64;
                break;
            case opBitAnd:
                func = &and_ai;
                break;
            case opBitOr:
                func = &or_ai;
                break;
            case opBitXor:
                func = &xor_ai;
                break;
            case opShiftRight:
                func = &rightShift_ai;
                break;
            case opShiftLeft:
                func = &leftShift_ai;
                break;
            case opLCM:
                func = &lcm_ai;
                break;
            case opGCD:
                func = &gcd_ai;
                break;
            case opRound:
                func = &round_ai;
                break;
            case opRoundUp:
                func = &roundUp_ai;
                break;
            case opTrunc:
                func = &trunc_ai;
                break;
            case opAtan2:
                func = &atan2_ai;
                break;
            case opHypot:
                func = &hypot_ai;
                break;
            case opHypotx:
                func = &hypotx_ai;
                break;
            case opPow:
                func = &pow_ai_nova;
                break;
            case opRing1:
                func = &ring1_ai_nova_64;
                break;
            case opRing2:
                func = &ring2_ai_nova_64;
                break;
            case opRing3:
                func = &ring3_ai_nova_64;
                break;
            case opRing4:
                func = &ring4_ai_nova_64;
                break;
            case opDifSqr:
                func = &difsqr_ai_nova_64;
                break;
            case opSumSqr:
                func = &sumsqr_ai_nova_64;
                break;
            case opSqrSum:
                func = &sqrsum_ai_nova_64;
                break;
            case opSqrDif:
                func = &sqrdif_ai_nova_64;
                break;
            case opAbsDif:
                func = &absdif_ai;
                break;
            case opThresh:
                func = &thresh_ai;
                break;
            case opAMClip:
                func = &amclip_ai;
                break;
            case opScaleNeg:
                func = &scaleneg_ai;
                break;
            case opClip2:
                func = &clip2_ai_nova_64;
                break;
            case opFold2:
                func = &fold2_ai;
                break;
            case opWrap2:
                func = &wrap2_ai;
                break;
            case opExcess:
                func = &excess_ai;
                break;
            case opRandRange:
                func = &rrand_ai;
                break;
            case opExpRandRange:
                func = &exprand_ai;
                break;
            case opFirstArg:
                func = &firstarg_aa;
                break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ai;
                break;
            }
        }
        break;
    case calc_BufRate:
        if (rateB == calc_FullRate) {
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ka_nova_64;
                break;
            case opSub:
                func = &sub_ka_nova_64;
                break;
            case opMul:
                func = &mul_ka_nova_64;
                break;
            case opFDiv:
                func = &div_ka_nova;
                break;
            case opIDiv:
                func = &idiv_ka;
                break;
            case opMod:
                func = &mod_ka;
                break;
            case opEQ:
                func = &eq_ka_nova_64;
                break;
            case opNE:
                func = &neq_ka_nova_64;
                break;
            case opLT:
                func = &lt_ka_nova_64;
                break;
            case opGT:
                func = &gt_ka_nova_64;
                break;
            case opLE:
                func = &le_ka_nova_64;
                break;
            case opGE:
                func = &ge_ka_nova_64;
                break;
            case opMin:
                func = &min_ka_nova_64;
                break;
            case opMax:
                func = &max_ka_nova_64;
                break;
            case opBitAnd:
                func = &and_ka;
                break;
            case opBitOr:
                func = &or_ka;
                break;
            case opBitXor:
                func = &xor_ka;
                break;
            case opShiftRight:
                func = &rightShift_ka;
                break;
            case opShiftLeft:
                func = &leftShift_ka;
                break;
            case opLCM:
                func = &lcm_ka;
                break;
            case opGCD:
                func = &gcd_ka;
                break;
            case opRound:
                func = &round_ka;
                break;
            case opRoundUp:
                func = &roundUp_ka;
                break;
            case opTrunc:
                func = &trunc_ka;
                break;
            case opAtan2:
                func = &atan2_ka;
                break;
            case opHypot:
                func = &hypot_ka;
                break;
            case opHypotx:
                func = &hypotx_ka;
                break;
            case opPow:
                func = &pow_ka_nova;
                break;
            case opRing1:
                func = &ring1_ka;
                break;
            case opRing2:
                func = &ring2_ka;
                break;
            case opRing3:
                func = &ring3_ka;
                break;
            case opRing4:
                func = &ring4_ka;
                break;
            case opDifSqr:
                func = &difsqr_ka_nova_64;
                break;
            case opSumSqr:
                func = &sumsqr_ka_nova_64;
                break;
            case opSqrSum:
                func = &sqrsum_ka_nova_64;
                break;
            case opSqrDif:
                func = &sqrdif_ka_nova_64;
                break;
            case opAbsDif:
                func = &absdif_ka;
                break;
            case opThresh:
                func = &thresh_ka;
                break;
            case opAMClip:
                func = &amclip_ka;
                break;
            case opScaleNeg:
                func = &scaleneg_ka;
                break;
            case opClip2:
                func = &clip2_ka_nova_64;
                break;
            case opFold2:
                func = &fold2_ka;
                break;
            case opWrap2:
                func = &wrap2_ka;
                break;
            case opExcess:
                func = &excess_ka;
                break;
            case opRandRange:
                func = &rrand_ka;
                break;
            case opExpRandRange:
                func = &exprand_ka;
                break;
            // case opFirstArg : func = &firstarg_aa; break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ka;
                break;
            }
        } else {
            // this should have been caught by mBufLength == 1
            func = &zero_aa;
        }
        break;
    case calc_ScalarRate:
        if (rateB == calc_FullRate) {
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ia_nova_64;
                break;
            case opSub:
                func = &sub_ia_nova_64;
                break;
            case opMul:
                func = &mul_ia_nova_64;
                break;
            case opFDiv:
                func = &div_ia_nova;
                break;
            case opIDiv:
                func = &idiv_ia;
                break;
            case opMod:
                func = &mod_ia;
                break;
            case opEQ:
                func = &eq_ia_nova_64;
                break;
            case opNE:
                func = &neq_ia_nova_64;
                break;
            case opLT:
                func = &lt_ia_nova_64;
                break;
            case opGT:
                func = &gt_ia_nova_64;
                break;
            case opLE:
                func = &le_ia_nova_64;
                break;
            case opGE:
                func = &ge_ia_nova_64;
                break;
            case opMin:
                func = &min_ia_nova_64;
                break;
            case opMax:
                func = &max_ia_nova_64;
                break;
            case opBitAnd:
                func = &and_ia;
                break;
            case opBitOr:
                func = &or_ia;
                break;
            case opBitXor:
                func = &xor_ia;
                break;
            case opShiftRight:
                func = &rightShift_ia;
                break;
            case opShiftLeft:
                func = &leftShift_ia;
                break;
            case opLCM:
                func = &lcm_ia;
                break;
            case opGCD:
                func = &gcd_ia;
                break;
            case opRound:
                func = &round_ia;
                break;
            case opRoundUp:
                func = &roundUp_ia;
                break;
            case opTrunc:
                func = &trunc_ia;
                break;
            case opAtan2:
                func = &atan2_ia;
                break;
            case opHypot:
                func = &hypot_ia;
                break;
            case opHypotx:
                func = &hypotx_ia;
                break;
            case opPow:
                func = &pow_ia_nova;
                break;
            case opRing1:
                func = &ring1_ia_nova_64;
                break;
            case opRing2:
                func = &ring2_ia_nova_64;
                break;
            case opRing3:
                func = &ring3_ia_nova_64;
                break;
            case opRing4:
                func = &ring4_ia_nova_64;
                break;
            case opDifSqr:
                func = &difsqr_ia_nova_64;
                break;
            case opSumSqr:
                func = &sumsqr_ia_nova_64;
                break;
            case opSqrSum:
                func = &sqrsum_ia_nova_64;
                break;
            case opSqrDif:
                func = &sqrdif_ia_nova_64;
                break;
            case opAbsDif:
                func = &absdif_ia;
                break;
            case opThresh:
                func = &thresh_ia;
                break;
            case opAMClip:
                func = &amclip_ia;
                break;
            case opScaleNeg:
                func = &scaleneg_ia;
                break;
            case opClip2:
                func = &clip2_ia_nova_64;
                break;
            case opFold2:
                func = &fold2_ia;
                break;
            case opWrap2:
                func = &wrap2_ia;
                break;
            case opExcess:
                func = &excess_ia;
                break;
            case opRandRange:
                func = &rrand_ia;
                break;
            case opExpRandRange:
                func = &exprand_ia;
                break;
            // case opFirstArg : func = &firstarg_aa; break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ia;
                break;
            }
        } else {
            // this should have been caught by mBufLength == 1
            func = &zero_aa;
        }
        break;
    }

    return func;
}


static BinaryOpFunc ChooseNovaSimdFunc(BinaryOpUGen* unit) {
    if (BUFLENGTH == 64)
        return ChooseNovaSimdFunc_64(unit);

    BinaryOpFunc func = &zero_1;

    int rateA = INRATE(0);
    int rateB = INRATE(1);

    switch (rateA) {
    case calc_FullRate:
        switch (rateB) {
        case calc_FullRate:
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_aa_nova;
                break;
            case opSub:
                func = &sub_aa_nova;
                break;
            case opMul:
                func = &mul_aa_nova;
                break;
            case opFDiv:
                func = &div_aa_nova;
                break;
            case opIDiv:
                func = &idiv_aa;
                break;
            case opMod:
                func = &mod_aa;
                break;
            case opEQ:
                func = &eq_aa_nova;
                break;
            case opNE:
                func = &neq_aa_nova;
                break;
            case opLT:
                func = &lt_aa_nova;
                break;
            case opGT:
                func = &gt_aa_nova;
                break;
            case opLE:
                func = &le_aa_nova;
                break;
            case opGE:
                func = &ge_aa_nova;
                break;
            case opMin:
                func = &min_aa_nova;
                break;
            case opMax:
                func = &max_aa_nova;
                break;
            case opBitAnd:
                func = &and_aa;
                break;
            case opBitOr:
                func = &or_aa;
                break;
            case opBitXor:
                func = &xor_aa;
                break;
            case opShiftRight:
                func = &rightShift_aa;
                break;
            case opShiftLeft:
                func = &leftShift_aa;
                break;
            case opLCM:
                func = &lcm_aa;
                break;
            case opGCD:
                func = &gcd_aa;
                break;
            case opRound:
                func = &round_aa;
                break;
            case opRoundUp:
                func = &roundUp_aa;
                break;
            case opTrunc:
                func = &trunc_aa;
                break;
            case opAtan2:
                func = &atan2_aa;
                break;
            case opHypot:
                func = &hypot_aa;
                break;
            case opHypotx:
                func = &hypotx_aa;
                break;
            case opPow:
                func = &pow_aa_nova;
                break;
            case opRing1:
                func = &ring1_aa_nova;
                break;
            case opRing2:
                func = &ring2_aa_nova;
                break;
            case opRing3:
                func = &ring3_aa_nova;
                break;
            case opRing4:
                func = &ring4_aa_nova;
                break;
            case opDifSqr:
                func = &difsqr_aa_nova;
                break;
            case opSumSqr:
                func = &sumsqr_aa_nova;
                break;
            case opSqrSum:
                func = &sqrsum_aa_nova;
                break;
            case opSqrDif:
                func = &sqrdif_aa_nova;
                break;
            case opAbsDif:
                func = &absdif_aa;
                break;
            case opThresh:
                func = &thresh_aa;
                break;
            case opAMClip:
                func = &amclip_aa;
                break;
            case opScaleNeg:
                func = &scaleneg_aa;
                break;
            case opClip2:
                func = &clip2_aa_nova;
                break;
            case opFold2:
                func = &fold2_aa;
                break;
            case opWrap2:
                func = &wrap2_aa;
                break;
            case opExcess:
                func = &excess_aa;
                break;
            case opRandRange:
                func = &rrand_aa;
                break;
            case opExpRandRange:
                func = &exprand_aa;
                break;
            case opFirstArg:
                func = &firstarg_aa_nova;
                break;
            // case opSecondArg : func = &secondarg_aa_nova; break;
            default:
                func = &add_aa;
                break;
            }
            break;
        case calc_BufRate:
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ak_nova;
                break;
            case opSub:
                func = &sub_ak_nova;
                break;
            case opMul:
                func = &mul_ak_nova;
                break;
            case opFDiv:
                func = &div_ak_nova;
                break;
            case opIDiv:
                func = &idiv_ak;
                break;
            case opMod:
                func = &mod_ak;
                break;
            case opEQ:
                func = &eq_ak_nova;
                break;
            case opNE:
                func = &neq_ak_nova;
                break;
            case opLT:
                func = &lt_ak_nova;
                break;
            case opGT:
                func = &gt_ak_nova;
                break;
            case opLE:
                func = &le_ak_nova;
                break;
            case opGE:
                func = &ge_ak_nova;
                break;
            case opMin:
                func = &min_ak_nova;
                break;
            case opMax:
                func = &max_ak_nova;
                break;
            case opBitAnd:
                func = &and_ak;
                break;
            case opBitOr:
                func = &or_ak;
                break;
            case opBitXor:
                func = &xor_ak;
                break;
            case opShiftRight:
                func = &rightShift_ak;
                break;
            case opShiftLeft:
                func = &leftShift_ak;
                break;
            case opLCM:
                func = &lcm_ak;
                break;
            case opGCD:
                func = &gcd_ak;
                break;
            case opRound:
                func = &round_ak;
                break;
            case opRoundUp:
                func = &roundUp_ak;
                break;
            case opTrunc:
                func = &trunc_ak;
                break;
            case opAtan2:
                func = &atan2_ak;
                break;
            case opHypot:
                func = &hypot_ak;
                break;
            case opHypotx:
                func = &hypotx_ak;
                break;
            case opPow:
                func = &pow_ak_nova;
                break;
            case opRing1:
                func = &ring1_ak;
                break;
            case opRing2:
                func = &ring2_ak;
                break;
            case opRing3:
                func = &ring3_ak;
                break;
            case opRing4:
                func = &ring4_ak;
                break;
            case opDifSqr:
                func = &difsqr_ak_nova;
                break;
            case opSumSqr:
                func = &sumsqr_ak_nova;
                break;
            case opSqrSum:
                func = &sqrsum_ak_nova;
                break;
            case opSqrDif:
                func = &sqrdif_ak_nova;
                break;
            case opAbsDif:
                func = &absdif_ak;
                break;
            case opThresh:
                func = &thresh_ak;
                break;
            case opAMClip:
                func = &amclip_ak;
                break;
            case opScaleNeg:
                func = &scaleneg_ak;
                break;
            case opClip2:
                func = &clip2_ak_nova;
                break;
            case opFold2:
                func = &fold2_ak;
                break;
            case opWrap2:
                func = &wrap2_ak;
                break;
            case opExcess:
                func = &excess_ak;
                break;
            case opRandRange:
                func = &rrand_ak;
                break;
            case opExpRandRange:
                func = &exprand_ak;
                break;
            case opFirstArg:
                func = &firstarg_aa;
                break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ak;
                break;
            }
            break;
        case calc_ScalarRate:
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ai_nova;
                break;
            case opSub:
                func = &sub_ai_nova;
                break;
            case opMul:
                func = &mul_ai_nova;
                break;
            case opFDiv:
                func = &div_ai_nova;
                break;
            case opIDiv:
                func = &idiv_ai;
                break;
            case opMod:
                func = &mod_ai;
                break;
            case opEQ:
                func = &eq_ai_nova;
                break;
            case opNE:
                func = &neq_ai_nova;
                break;
            case opLT:
                func = &lt_ai_nova;
                break;
            case opGT:
                func = &gt_ai_nova;
                break;
            case opLE:
                func = &le_ai_nova;
                break;
            case opGE:
                func = &ge_ai_nova;
                break;
            case opMin:
                func = &min_ai_nova;
                break;
            case opMax:
                func = &max_ai_nova;
                break;
            case opBitAnd:
                func = &and_ai;
                break;
            case opBitOr:
                func = &or_ai;
                break;
            case opBitXor:
                func = &xor_ai;
                break;
            case opShiftRight:
                func = &rightShift_ai;
                break;
            case opShiftLeft:
                func = &leftShift_ai;
                break;
            case opLCM:
                func = &lcm_ai;
                break;
            case opGCD:
                func = &gcd_ai;
                break;
            case opRound:
                func = &round_ai;
                break;
            case opRoundUp:
                func = &roundUp_ai;
                break;
            case opTrunc:
                func = &trunc_ai;
                break;
            case opAtan2:
                func = &atan2_ai;
                break;
            case opHypot:
                func = &hypot_ai;
                break;
            case opHypotx:
                func = &hypotx_ai;
                break;
            case opPow:
                func = &pow_ai_nova;
                break;
            case opRing1:
                func = &ring1_ai_nova;
                break;
            case opRing2:
                func = &ring2_ai_nova;
                break;
            case opRing3:
                func = &ring3_ai_nova;
                break;
            case opRing4:
                func = &ring4_ai_nova;
                break;
            case opDifSqr:
                func = &difsqr_ai_nova;
                break;
            case opSumSqr:
                func = &sumsqr_ai_nova;
                break;
            case opSqrSum:
                func = &sqrsum_ai_nova;
                break;
            case opSqrDif:
                func = &sqrdif_ai_nova;
                break;
            case opAbsDif:
                func = &absdif_ai;
                break;
            case opThresh:
                func = &thresh_ai;
                break;
            case opAMClip:
                func = &amclip_ai;
                break;
            case opScaleNeg:
                func = &scaleneg_ai;
                break;
            case opClip2:
                func = &clip2_ai_nova;
                break;
            case opFold2:
                func = &fold2_ai;
                break;
            case opWrap2:
                func = &wrap2_ai;
                break;
            case opExcess:
                func = &excess_ai;
                break;
            case opRandRange:
                func = &rrand_ai;
                break;
            case opExpRandRange:
                func = &exprand_ai;
                break;
            case opFirstArg:
                func = &firstarg_aa;
                break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ai;
                break;
            }
        }
        break;
    case calc_BufRate:
        if (rateB == calc_FullRate) {
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ka_nova;
                break;
            case opSub:
                func = &sub_ka_nova;
                break;
            case opMul:
                func = &mul_ka_nova;
                break;
            case opFDiv:
                func = &div_ka_nova;
                break;
            case opIDiv:
                func = &idiv_ka;
                break;
            case opMod:
                func = &mod_ka;
                break;
            case opEQ:
                func = &eq_ka_nova;
                break;
            case opNE:
                func = &neq_ka_nova;
                break;
            case opLT:
                func = &lt_ka_nova;
                break;
            case opGT:
                func = &gt_ka_nova;
                break;
            case opLE:
                func = &le_ka_nova;
                break;
            case opGE:
                func = &ge_ka_nova;
                break;
            case opMin:
                func = &min_ka_nova;
                break;
            case opMax:
                func = &max_ka_nova;
                break;
            case opBitAnd:
                func = &and_ka;
                break;
            case opBitOr:
                func = &or_ka;
                break;
            case opBitXor:
                func = &xor_ka;
                break;
            case opShiftRight:
                func = &rightShift_ka;
                break;
            case opShiftLeft:
                func = &leftShift_ka;
                break;
            case opLCM:
                func = &lcm_ka;
                break;
            case opGCD:
                func = &gcd_ka;
                break;
            case opRound:
                func = &round_ka;
                break;
            case opRoundUp:
                func = &roundUp_ka;
                break;
            case opTrunc:
                func = &trunc_ka;
                break;
            case opAtan2:
                func = &atan2_ka;
                break;
            case opHypot:
                func = &hypot_ka;
                break;
            case opHypotx:
                func = &hypotx_ka;
                break;
            case opPow:
                func = &pow_ka_nova;
                break;
            case opRing1:
                func = &ring1_ka;
                break;
            case opRing2:
                func = &ring2_ka;
                break;
            case opRing3:
                func = &ring3_ka;
                break;
            case opRing4:
                func = &ring4_ka;
                break;
            case opDifSqr:
                func = &difsqr_ka_nova;
                break;
            case opSumSqr:
                func = &sumsqr_ka_nova;
                break;
            case opSqrSum:
                func = &sqrsum_ka_nova;
                break;
            case opSqrDif:
                func = &sqrdif_ka_nova;
                break;
            case opAbsDif:
                func = &absdif_ka;
                break;
            case opThresh:
                func = &thresh_ka;
                break;
            case opAMClip:
                func = &amclip_ka;
                break;
            case opScaleNeg:
                func = &scaleneg_ka;
                break;
            case opClip2:
                func = &clip2_ka_nova;
                break;
            case opFold2:
                func = &fold2_ka;
                break;
            case opWrap2:
                func = &wrap2_ka;
                break;
            case opExcess:
                func = &excess_ka;
                break;
            case opRandRange:
                func = &rrand_ka;
                break;
            case opExpRandRange:
                func = &exprand_ka;
                break;
            // case opFirstArg : func = &firstarg_aa; break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ka;
                break;
            }
        } else {
            // this should have been caught by mBufLength == 1
            func = &zero_aa;
        }
        break;
    case calc_ScalarRate:
        if (rateB == calc_FullRate) {
            switch (unit->mSpecialIndex) {
            // case opSilence2 : func = &zero_aa; break;
            case opAdd:
                func = &add_ia_nova;
                break;
            case opSub:
                func = &sub_ia_nova;
                break;
            case opMul:
                func = &mul_ia_nova;
                break;
            case opFDiv:
                func = &div_ia_nova;
                break;
            case opIDiv:
                func = &idiv_ia;
                break;
            case opMod:
                func = &mod_ia;
                break;
            case opEQ:
                func = &eq_ia_nova;
                break;
            case opNE:
                func = &neq_ia_nova;
                break;
            case opLT:
                func = &lt_ia_nova;
                break;
            case opGT:
                func = &gt_ia_nova;
                break;
            case opLE:
                func = &le_ia_nova;
                break;
            case opGE:
                func = &ge_ia_nova;
                break;
            case opMin:
                func = &min_ia_nova;
                break;
            case opMax:
                func = &max_ia_nova;
                break;
            case opBitAnd:
                func = &and_ia;
                break;
            case opBitOr:
                func = &or_ia;
                break;
            case opBitXor:
                func = &xor_ia;
                break;
            case opShiftRight:
                func = &rightShift_ia;
                break;
            case opShiftLeft:
                func = &leftShift_ia;
                break;
            case opLCM:
                func = &lcm_ia;
                break;
            case opGCD:
                func = &gcd_ia;
                break;
            case opRound:
                func = &round_ia;
                break;
            case opRoundUp:
                func = &roundUp_ia;
                break;
            case opTrunc:
                func = &trunc_ia;
                break;
            case opAtan2:
                func = &atan2_ia;
                break;
            case opHypot:
                func = &hypot_ia;
                break;
            case opHypotx:
                func = &hypotx_ia;
                break;
            case opPow:
                func = &pow_ia_nova;
                break;
            case opRing1:
                func = &ring1_ia_nova;
                break;
            case opRing2:
                func = &ring2_ia_nova;
                break;
            case opRing3:
                func = &ring3_ia_nova;
                break;
            case opRing4:
                func = &ring4_ia_nova;
                break;
            case opDifSqr:
                func = &difsqr_ia_nova;
                break;
            case opSumSqr:
                func = &sumsqr_ia_nova;
                break;
            case opSqrSum:
                func = &sqrsum_ia_nova;
                break;
            case opSqrDif:
                func = &sqrdif_ia_nova;
                break;
            case opAbsDif:
                func = &absdif_ia;
                break;
            case opThresh:
                func = &thresh_ia;
                break;
            case opAMClip:
                func = &amclip_ia;
                break;
            case opScaleNeg:
                func = &scaleneg_ia;
                break;
            case opClip2:
                func = &clip2_ia_nova;
                break;
            case opFold2:
                func = &fold2_ia;
                break;
            case opWrap2:
                func = &wrap2_ia;
                break;
            case opExcess:
                func = &excess_ia;
                break;
            case opRandRange:
                func = &rrand_ia;
                break;
            case opExpRandRange:
                func = &rrand_ia;
                break;
            // case opFirstArg : func = &firstarg_aa; break;
            // case opSecondArg : func = &secondarg_aa; break;
            default:
                func = &add_ia;
                break;
            }
        } else {
            // this should have been caught by mBufLength == 1
            func = &zero_aa;
        }
        break;
    }

    return func;
}

#endif

bool ChooseOperatorFunc(BinaryOpUGen* unit) {
    // Print("->ChooseOperatorFunc %d\n", unit->mSpecialIndex);
    BinaryOpFunc func = &zero_aa;
    bool ret = false;

    if (BUFLENGTH == 1) {
        if (unit->mCalcRate == calc_DemandRate) {
            func = ChooseDemandFunc(unit);
        } else {
            func = ChooseOneSampleFunc(unit);
        }
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


PluginLoad(BinaryOp) {
    ft = inTable;

    DefineSimpleUnit(BinaryOpUGen);
}
