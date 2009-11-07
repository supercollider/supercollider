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
#ifndef SQRTF4_H
#define SQRTF4_H

#include "simdconst.h"
#include "fabsf4.h"
#include "floorf4.h"

// Intrinsic for SSE
#ifdef __SSE__
static inline vec_float4  __attribute__((__always_inline__))
      _sqrtf4( vec_float4 x ){
      return _mm_sqrt_ps( x );
}
#else // ! __SSE__

#ifdef __PPU__

/*
sqrt:
Interval 0 .... 1000
libsimdmath:
time: 1.95282e-08s
rms abs: 7.99956e-07, rms rel: 3.73923e-08
max abs: 3.18666e-06, max rel: 1.70233e-07
sum: 2.10805e+07
libmath:
time: 3.86431e-07s
rms abs: 4.9226e-07, rms rel: 2.356e-08
max abs: 9.53671e-07, max rel: 5.96046e-08
sum: 2.10805e+07
ps3:
time: 1.98388e-08s
rms abs: 2.68302e-06, rms rel: 1.19424e-07
max abs: 7.61002e-06, max rel: 2.8155e-07
sum: 2.10805e+07
*/
static inline vec_float4 _sqrtf4(vec_float4 x ){
      // All constants are provided as vector elements 
      // for fast access
      vec_float4 c0 ={  1.17549e-38f,                       // cfm
                        1.5f,
                        0.5f,                 
                        0.0f};                  
      vec_float4 cfm  = vec_splat(c0,0);
      vec_float4 c15  = vec_splat(c0,1);
      vec_float4 c05  = vec_splat(c0,2);
      vec_float4 c00  = vec_splat(c0,3);

      vec_int4 x_pos = VEC_GT(x, cfm);
      vec_float4 x0  = vec_rsqrte( x );
      x  = vec_madd(x,x0,c00);
      x0 = vec_madd(x0,c05,c00);
      x  = vec_madd(x,vec_nmsub( x, x0, c15),c00);
      VEC_AND(x, x_pos);
      return x;
}

#else

#ifdef __SPU__

static inline vec_float4 _sqrtf4(vec_float4 x ){
      vec_float4 cfm  = spu_splats(1.17549e-38f);
      vec_float4 c15  = spu_splats(1.5f);
      vec_float4 c05  = spu_splats(0.5f);
      vec_float4 c00  = spu_splats(0.0f);
      vec_float4 c0e  = spu_splats(1.0000002f); 
      
      vec_int4 x_pos = (vec_int4)VEC_GT(x, cfm);
      
      vec_float4 x0  = spu_rsqrte( x );

      x  = vec_madd(x,x0,c00);
      x0 = vec_madd(x0,c05,c00);
      x  = vec_madd(x,vec_nmsub( x, x0, c15),c00);
      
      x *= c0e; // Compensate truncation errors

      VEC_AND(x, x_pos);
      return x;
}

#else
// Not used

DEF_CONST(CF4_SQRT2,    1.41421356237f)
DEF_CONST(CF4_SQRT05,   0.707106781187f)
DEF_CONST(CF4_SQ0,     -9.8843065718E-4f)
DEF_CONST(CF4_SQ1,     7.9479950957E-4f)
DEF_CONST(CF4_SQ2,     - 3.5890535377E-3f)
DEF_CONST(CF4_SQ3,     1.1028809744E-2f)
DEF_CONST(CF4_SQ4,     - 4.4195203560E-2f)
DEF_CONST(CF4_SQ5,     3.5355338194E-1f)
DEF_CONST(CF4_SQ6,     1.41421356237E0f)
DEF_CONST(CF4_SQ7,     1.35199291026E-2f)
DEF_CONST(CF4_SQ8,     - 2.26657767832E-2f)
DEF_CONST(CF4_SQ9,     2.78720776889E-2f)
DEF_CONST(CF4_SQA,     - 3.89582788321E-2f)
DEF_CONST(CF4_SQB,     6.24811144548E-2f)
DEF_CONST(CF4_SQC,     - 1.25001503933E-1f)
DEF_CONST(CF4_SQD,     -3.9495006054E-1f)
DEF_CONST(CF4_SQE,     5.1743034569E-1f)
DEF_CONST(CF4_SQF,     - 4.3214437330E-1f)
DEF_CONST(CF4_SQG,     3.5310730460E-1f)
DEF_CONST(CF4_SQH,     - 3.5354581892E-1f)
DEF_CONST(CF4_SQI,     7.0710676017E-1f)
DEF_CONST(CF4_SQJ,     7.07106781187E-1f)


static inline vec_float4 _sqrtf4( vec_float4 x ){
      vec_int4 e;
      x = _frexpf4( x, &e );
      
      vec_int4 e_odd = -(e & CI4_1);
      e += e_odd;
      vec_float4 x_add = x;
      VEC_AND(x, e_odd);
      x += x_add;
      
      e = VEC_SR(e,1);

      vec_int4 x_larger_sq2  = (vec_int4)VEC_GT(x, CF4_SQRT2);
      vec_int4 x_larger_sq05 = (vec_int4)VEC_GT(x, CF4_SQRT05);
      x_larger_sq05 -= x_larger_sq2; /* x is between sqrt(2)/2 and sqrt(2). */
      vec_int4 x_else = ~( x_larger_sq2 | x_larger_sq05);
      
      vec_float4 x1 = x - CF4_2;
      vec_float4 y1 = 
            ((((( CF4_SQ0 * x1
            + CF4_SQ1) * x1
            + CF4_SQ2) * x1
            + CF4_SQ3) * x1
            + CF4_SQ4) * x1
            + CF4_SQ5) * x1
            + CF4_SQ6;
      
      vec_float4 x2 = x - CF4_1;
      vec_float4 y2 = 
            ((((( CF4_SQ7 * x2
            + CF4_SQ8) * x2
            + CF4_SQ9) * x2
            + CF4_SQA) * x2
            + CF4_SQB) * x2
            + CF4_SQC) * x2 * x2
            + CF4_05 * x2
            + CF4_1;

      x -= CF4_05;
      vec_float4 y = 
            ((((( CF4_SQD * x
            + CF4_SQE) * x
            + CF4_SQF) * x
            + CF4_SQG) * x
            + CF4_SQH) * x
            + CF4_SQI) * x
            + CF4_SQJ;

      VEC_AND(y1, x_larger_sq2);
      VEC_AND(y2, x_larger_sq05);
      VEC_AND(y,  x_else);
      
      VEC_OR(y1, y2);
      VEC_OR(y, y1);

      return _ldexpf4(y, e);
      
}
#endif
#endif // ! __SSE__

#endif
#endif
