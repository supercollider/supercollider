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

#ifndef ASINF4_H
#define ASINF4_H

#include "simdconst.h"
#include "fabsf4.h"
#include "sqrtf4.h"

DEF_CONST(CF4_42163199048E_2,   4.2163199048E-2f)
DEF_CONST(CF4_24181311049E_2,   2.4181311049E-2f)
DEF_CONST(CF4_45470025998E_2,   4.5470025998E-2f)
DEF_CONST(CF4_74953002686E_2,   7.4953002686E-2f)
DEF_CONST(CF4_16666752422E_1,   1.6666752422E-1f)


/*

asin:
Interval -0.95 .... 0.95
libsimdmath:
time: 5.85089e-08s
rms abs: 4.30785e-08, rms rel: 5.8654e-08
max abs: 1.9724e-07, max rel: 3.48274e-07
sum: 4217.95
libmath:
time: 7.47557e-07s
rms abs: 1.77496e-08, rms rel: 2.68284e-08
max abs: 7.76105e-08, max rel: 9.48021e-08
sum: 4217.96
ps3:
time: 1.47138e-07s
rms abs: 2.57339e-08, rms rel: 3.26383e-08
max abs: 1.92584e-07, max rel: 2.11566e-07
sum: 4217.96



*/
static inline vec_float4  __attribute__((__always_inline__))
      _asinf4( vec_float4 x ){
      vec_float4 a,b,z;
      vec_int4 sign;

      sign  = _signf4( x );
      VEC_XOR(x, sign);

      vec_int4 x_smaller_1e_4 = (vec_int4)VEC_GT(CF4_00004, x);
      vec_int4 x_larger_05    = (vec_int4)VEC_GT(x, CF4_05);
      vec_int4 x_else         = ~( x_smaller_1e_4 | x_larger_05);

      a = x;
      VEC_AND(a, x_smaller_1e_4);
      b = CF4_05*(CF4_1-x);
      VEC_AND(b, x_larger_05);
      z = x * x;
      VEC_AND(z, x_else);

      VEC_OR(z, a);
      VEC_OR(z, b);

      VEC_AND(x, x_else);
      a = _sqrtf4(z);
      VEC_AND(a, x_larger_05 );
      VEC_OR(x, a);

      vec_float4 z1 =
            (((( CF4_42163199048E_2 * z
                  + CF4_24181311049E_2) * z
                  + CF4_45470025998E_2) * z
                  + CF4_74953002686E_2) * z
                  + CF4_16666752422E_1) * z * x
                  + x;

      z = vec_sel(z1,z,(vec_int4)x_smaller_1e_4);

      z1 = z+z;
      z1 = CF4_PIO2F-z1;

      z = vec_sel(z,z1,(vec_int4)x_larger_05);
      VEC_XOR(z,sign);
      return z ;
}

static inline vec_float4  __attribute__((__always_inline__))
      _acosf4( vec_float4 x ){
      vec_float4 a=x,x2,z;
      vec_int4   sign;
      sign  = _signf4( x );
      VEC_XOR(a, sign);
      sign    = -VEC_SR(sign,31); // 0xffff.. if negativ

      vec_int4 a_larger_05    = (vec_int4)VEC_GT(a, CF4_05);
      x = vec_sel( x, _sqrtf4(vec_nmsub(a,CF4_05,CF4_05)), (vec_int4)a_larger_05);

      x = _asinf4( x );
      x2 = x+x;

      z = vec_sel(x2, CF4_PIF-x2, (vec_int4)sign);
      z = vec_sel(CF4_PIO2F - x, z, (vec_int4)a_larger_05);
#ifdef __SPU__
      z *= spu_splats(1.0000002f);
#endif

      return z;
}

#endif

