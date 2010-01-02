/*
    Universal SIMD Mathlibrary
    Copyright 2008 Helmut Dersch
    der (at) fh-furtwangen.de

    This file is part of the Universal SIMD Mathlibrary.
    The Universal SIMD Mathlibrary is free software:
    you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
    For details of the algorithms see the documentation
*/
#ifndef FLOORF4_H
#define FLOORF4_H

#include "simdconst.h"
#include "fabsf4.h"


DEI_CONST(CI4_126,   126)

#ifdef __SSE__
static inline vec_int4 __attribute__((__always_inline__)) vec_exp()
{
    __m128i x;
    __m128i ones = _mm_cmpeq_epi32(x, x);
    __m128i ret = _mm_slli_epi32 (_mm_srli_epi32(ones, 24), 23);
    return (vec_int4)ret;
}

static inline vec_int4 __attribute__((__always_inline__)) vec_exp_1()
{
    __m128i x;
    __m128i ones = _mm_cmpeq_epi32(x, x);
    __m128i ret = _mm_slli_epi32 (_mm_srli_epi32(ones, 26), 24);
    return (vec_int4)ret;
}

/** \todo do some benchmarks, for now, this seems to be the m */
#define CI4_EXP_1 vec_exp_1()
//#define CI4_EXP vec_exp()

DEI_CONST(CI4_EXP,   0x7F800000u)
//DEI_CONST(CI4_EXP_1, 0x3F000000u)
#else
DEI_CONST(CI4_EXP,   0x7F800000u)
DEI_CONST(CI4_EXP_1, 0x3F000000u)
#endif

static inline vec_float4  __attribute__((__always_inline__))
      _roundf4(vec_float4 x){
      vec_int4 sign = _signf4( x ); // save sign
      VEC_XOR( x, sign );           // eliminate sign
#ifdef __SPU__
      x += CF4_05;
#endif
      x += CF4_LI;
      x -= CF4_LI;
      VEC_XOR( x, sign );
      return x;
}


static inline vec_float4  __attribute__((__always_inline__))
      _floorf4 ( vec_float4 x ){
#ifdef __PPU__
      return vec_floor(x);
#else
      vec_float4 y        = _roundf4( x );
      vec_int4 y_larger_x = (vec_int4)VEC_GT(y, x);
      vec_float4 y_add    = CF4__1;
      VEC_AND( y_add, y_larger_x );
      return y + y_add;
#endif
}

static inline vec_float4  __attribute__((__always_inline__))
      _ceilf4 ( vec_float4 x ){
      vec_float4 y             = _roundf4( x );
      vec_int4 y_smaller_x = (vec_int4)VEC_GT(x, y);
      vec_float4 y_add     = CF4_1;
      VEC_AND( y_add, y_smaller_x );
      return y + y_add;
}

#ifdef __SSE__

// ldexpf() multiplies x by 2**n.
static inline vec_float4 __attribute__((__always_inline__))
      _ldexpf4 ( vec_float4 x, vec_int4 n ){
      vec_int4 ex = CI4_EXP;              // exponent mask
      VEC_AND(ex, x);                     // extract exponent
      VEC_NAND3(x, x, CI4_EXP);           // clear exponent in x
//    VEC_XOR(x, ex);                     // clear exponent in x
      n = (vec_int4)VEC_SL(n,16+7) + ex;        // compute new exponent
      VEC_OR(x, n);                       // insert new exponent in x
      return x;
}

static inline vec_float4  __attribute__((__always_inline__))
      _frexpf4 ( vec_float4 x, vec_int4 *pw2 ){
      vec_int4 ex = CI4_EXP;              // exponent mask
      VEC_AND( ex, x);                    // extract exponent
      VEC_NAND3(x, x, CI4_EXP);           // clear exponent in x
//    VEC_XOR(x, ex);                     // clear exponent in x
      *pw2 = (vec_int4)VEC_SR(ex,16+7)-CI4_126; // compute exponent
      VEC_OR(x, CI4_EXP_1);               // insert exponent +1 in x
      return x;
}

#else // __SSE__

// ldexpf() multiplies x by 2**n.
static inline vec_float4 _ldexpf4( vec_float4 x, vec_int4 n ){
      vec_int4    ci ={ 0x7F800000u,            // exponent mask
                        -126,                   // exponent offset
                        0x3F000000u,            // exponent = -1
                        23};                    // exponent shift position
      vec_int4 e, ci_exp=vec_splat(ci,0);
      e = ci_exp;
      VEC_AND( e, x);                           // extract exponent
      VEC_NAND3(x, x, ci_exp);                  // clear exponent in x
#ifdef __PPU__
      n = vec_sl(n, vec_splat((vec_uint4)ci, 3)) +e; // compute exponent
#else // __PPU__
      n = vec_sl(n, 23) +e;                     // compute exponent
#endif // __PPU__
      VEC_OR(x, n);                             // insert new exponent in x
      return x;
}

static inline vec_float4 _frexpf4( vec_float4 x, vec_int4 *pw2 ){
      vec_int4    ci ={ 0x7F800000u,                        // exponent mask
                        -126,                               // exponent offset
                        0x3F000000u,                        // exponent = -1
                        23};                                // exponent shift position
      vec_int4 e, ci_exp=vec_splat(ci,0);
      e = ci_exp;
      VEC_AND( e, x);                     // extract exponent
      VEC_NAND3(x, x, ci_exp);            // clear exponent in x
#if defined(__SPU__) || defined(__SSE__)
      *pw2 = vec_sr(e,23) + vec_splat(ci,1); // compute exponent
#else
      *pw2 = vec_sr(e, (vec_uint4)vec_splat(ci, 3)) + vec_splat(ci,1); // compute exponent
#endif
      VEC_OR(x, vec_splat(ci,2));         // insert exponent +1 in x
      return x;
}
#endif // __SSE__
#endif
