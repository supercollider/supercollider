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

#ifndef SIMDCONST_H
#define SIMDCONST_H

#include "simdmath.h"

#ifdef __SPU__

#define VEC_F2I(a,b)          a = spu_convts(b, 0)
#define VEC_I2F(a,b)          a = spu_convtf(b, 0)

#define VEC_NAND3(a,b,c)      a=(typeof(a))spu_andc((vec_uint4)(b), (vec_uint4)(c));
#define VEC_AND(a,b)          a=(typeof(a))spu_and((vec_uint4)(a), (vec_uint4)(b));
#define VEC_NAND(a,b)         a=(typeof(a))spu_andc((vec_uint4)(b), (vec_uint4)(a));
#define VEC_OR(a,b)           a=(typeof(a))spu_or((vec_uint4)(a), (vec_uint4)(b));
#define VEC_XOR(a,b)          a=(typeof(a))spu_xor((vec_uint4)(a), (vec_uint4)(b));

// Map spu to altivec intrinsics

#define vec_re(x)             spu_re(x)
#define vec_sr(x,y)           spu_rlmask(x,-(y))
#define vec_sl(x,y)           spu_sl(x,y)
#define vec_or(a,b)           spu_or(a,b)
#define vec_and(a,b)          spu_and(a,b)
#define vec_add(a,b)          spu_add(a,b)
#define vec_madd(a,b,c)       spu_madd(a,b,c)
#define vec_nmsub(a,b,c)      spu_nmsub(a,b,c)
#define vec_splat(a,b)        spu_splats(spu_extract(a,b))
#define vec_sel(a,b,c)        spu_sel(a,b,c)

#define VEC_SR(v,n)           spu_rlmask(v, -(n))
#define VEC_SL(v,n)           spu_sl(v, n)
#define VEC_GT(a,b)           spu_cmpgt(a,b)
#define VEC_LT(a,b)           spu_cmpgt(b,a)
#define VEC_EQ(a,b)           spu_cmpeq(a,b)
#define VEC_NE(a,b)           (~spu_cmpeq(a,b))
#define VEC_GE(a,b)           spu_or(spu_cmpgt(a,b),spu_cmpeq(a,b))

#endif // __SPU__


#ifdef __PPU__

#define VEC_F2I(a,b)   a = vec_cts(b, 0)
#define VEC_I2F(a,b)   a = vec_ctf(b, 0)


/* definitions for a &= b; etc
gcc generates very slow code for corresponding
vec_float4 C-style expressions
*/

#define VEC_NAND3(a,b,c) a=(typeof(a))vec_andc((vector unsigned int)(b), (vector unsigned int)(c));

#define VEC_AND(a,b)  a=(typeof(a))vec_and((vector unsigned int)(a), (vector unsigned int)(b));
#define VEC_NAND(a,b) a=(typeof(a))vec_andc((vector unsigned int)(b), (vector unsigned int)(a));
#define VEC_OR(a,b)   a=(typeof(a))vec_or((vector unsigned int)(a), (vector unsigned int)(b));
#define VEC_XOR(a,b)  a=(typeof(a))vec_xor((vector unsigned int)(a), (vector unsigned int)(b));

#define VEC_GT(a,b)     vec_cmpgt(a,b)
#define VEC_LT(a,b)     vec_cmplt(a,b)
#define VEC_EQ(a,b)     vec_cmpeq(a,b)
#define VEC_NE(a,b)     (~vec_cmpeq(a,b))
#define VEC_GE(a,b)     vec_cmpge(a,b)
#define VEC_SR(v,n)    _vec_sr(v, n)
#define VEC_SL(v,n)    _vec_sl(v, n)

static inline vec_int4 _vec_sr(vec_int4 x, int n){
      union{
            vector unsigned int v;
            int i[4];
      }y;
      y.i[0] = n;
      return vec_sr(x, vec_splat(y.v, 0));
}

static inline vec_int4 _vec_sl(vec_int4 x, int n){
      union{
            vector unsigned int v;
            int i[4];
      }y;
      y.i[0] = n;
      return vec_sl(x, vec_splat(y.v, 0));
}
#endif // __PPU__

#ifdef __SSE__
#include <xmmintrin.h>
#include <emmintrin.h>

#define VEC_F2I(a,b)   asm("cvttps2dq %1, %0":"=x" (a) :"x" (b))
#define VEC_I2F(a,b)   asm("cvtdq2ps  %1, %0":"=x" (a) :"x" (b))
/* definitions for a &= b; etc
gcc generates very slow code for corresponding
vec_float4 C-style expressions
*/
#define VEC_AND(a,b)   asm ("andps %2, %1":"=x" (a) :"0" (a),"x" (b))
#define VEC_NAND(a,b)  asm ("andnps %2, %1":"=x" (a) :"0" (a),"x" (b))
#define VEC_NAND3(a,b,c)  a=(typeof(a))_mm_andnot_ps((vec_float4)(c),(vec_float4)(b))
#define VEC_OR(a,b)    asm ("orps  %2, %1":"=x" (a) :"0" (a),"x" (b))
#define VEC_XOR(a,b)   asm ("xorps %2, %1":"=x" (a) :"0" (a),"x" (b))
#define VEC_SUB(a,b)   asm ("subps %2, %1":"=x" (a) :"0" (a),"x" (b))

#define VEC_GT(a,b)     __builtin_ia32_cmpgtps((vec_float4)a,(vec_float4)b)
#define VEC_LT(a,b)     __builtin_ia32_cmpltps(a,b)
#define VEC_EQ(a,b)     __builtin_ia32_cmpeqps(a,b)
#define VEC_NE(a,b)     __builtin_ia32_cmpneqps(a,b)
#define VEC_GE(a,b)     __builtin_ia32_cmpgeps(a,b)
#define VEC_SR(v,n)     (typeof(v))_mm_srli_epi32((__m128i)v,n)
#define VEC_SL(v,n)     (typeof(v))_mm_slli_epi32((__m128i)v,n)


#define vec_re(x)             _mm_rcp_ps(x)
#define vec_sr(x,y)           (typeof(x))_mm_srli_epi32((__m128i)x,y)
#define vec_sl(x,y)           (typeof(x))_mm_slli_epi32((__m128i)x,y)
#define vec_or(a,b)           ((a)|(b))
#define vec_and(a,b)          ((a)&(b))
#define vec_add(a,b)          ((a)+(b))
#define vec_madd(a,b,c)       ((a)*(b)+(c))
#define vec_nmsub(a,b,c)      ((c)-(a)*(b))
#define vec_splat(x,n)        (typeof(x))_mm_shuffle_ps(x,x,_MM_SHUFFLE(n,n,n,n))

#ifdef __SSE4_1__
#include <smmintrin.h>

#define vec_sel(a,b,mask)     _mm_blendv_ps(a,b,(__m128)mask)

#else

static vec_float4 __attribute__((__always_inline__)) vec_sel( vec_float4 a, vec_float4 b, vec_int4 sel ){
      VEC_AND(b, sel);
      VEC_NAND(sel,a);
      VEC_OR(b,sel);
      return b;
}

#endif

static inline vec_int4 __attribute__((__always_inline__)) vec_sign_mask(void)
{
    __m128i x;
    __m128i ones = _mm_cmpeq_epi32(x, x);
    return (vec_int4)_mm_slli_epi32 (_mm_srli_epi32(ones, 31), 31);
}

static inline vec_int4 __attribute__((__always_inline__)) vec_rsign_mask(void)
{
    __m128i x;
    __m128i ones = _mm_cmpeq_epi32(x, x);
    return (vec_int4)_mm_srli_epi32 (_mm_slli_epi32(ones, 1), 1);
}



#endif // __SSE__

#define DEF_CONST(a,b) static  const vec_float4 a = {b,b,b,b};
#define DEI_CONST(a,b) static  const vec_int4   a = {b,b,b,b};

DEF_CONST(CF4_0,        0.0f)
DEF_CONST(CF4_0125,     0.125f)
DEF_CONST(CF4_05,       0.5f)
DEF_CONST(CF4_1,        1.0f)
DEF_CONST(CF4_2,        2.0f)
DEF_CONST(CF4_3,        3.0f)
DEF_CONST(CF4_4,        4.0f)
DEF_CONST(CF4_8,        8.0f)
DEF_CONST(CF4__1,       -1.0f)
DEF_CONST(CF4_00004,    1.0e-4f);
DEF_CONST(CF4_SMALL,    1.0E-35f)

DEF_CONST(CF4_PIF,      3.14159265358979323846f)
DEF_CONST(CF4_PIO2F,    1.570796326794896619f)
DEF_CONST(CF4_PIO4F,    0.7853981633974483096f)

DEI_CONST(CI4_SIGN,     0x80000000u)
DEI_CONST(CI4_RSIGN,    0x7FFFFFFFu);
DEF_CONST(CF4_LI,       0x1.0p23f)
DEI_CONST(CI4_LI,       0x4B000000u)


DEI_CONST(CI4_0,        0);
DEI_CONST(CI4_1,        1);
DEI_CONST(CI4_2,        2);
DEI_CONST(CI4_3,        3);
DEI_CONST(CI4_4,        4);
DEI_CONST(CI4_7,        7);
DEI_CONST(CI4_8,        8);


static inline vec_float4 _recipf4(vec_float4 a){
      vec_float4 x0;
      x0 = vec_re(a);
      return vec_nmsub(a, x0, CF4_1)*x0+x0;
}


#endif
