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

#ifndef EXPF4_H
#define EXPF4_H

#include "simdconst.h"
#include "fabsf4.h"
#include "floorf4.h"

DEF_CONST(CF4_LOG2EF, 1.44269504088896341f)
DEF_CONST(CF4_LOG2EA, 0.44269504088896340736F)



/*

Base polynomial from cephes library, decomposed into quadratic functions 
for parallel execution.

y = (a1*x+a0)*(x^2+b1*x+b0)*x^2+x+1

Performance (Playstation 3, YDL 6):

Interval -2 .... 2

time: 3.90489e-08s
rms abs: 7.01217e-08, rms rel: 3.14763e-08
max abs: 3.01923e-07, max rel: 1.18811e-07

libmath:
time: 1.23597e-06s
rms abs: 6.33267e-08, rms rel: 2.52695e-08
max abs: 2.38557e-07, max rel: 5.92675e-08

ps3-simdmath:
time: 4.8605e-08s
rms abs: 1.5881e-07, rms rel: 5.20215e-08
max abs: 1.24296e-06, max rel: 2.54791e-07

*/

#ifdef __SSE__
DEF_CONST(CF4_EXPC1, 0.693359375f)
DEF_CONST(CF4_EXPC2, -2.12194440e-4f)

DEF_CONST(CF4_EXP0, 1.9875691500E-4f)
DEF_CONST(CF4_EXP1, 1.3981999507E-3f)
DEF_CONST(CF4_EXP2, 8.3334519073E-3f)
DEF_CONST(CF4_EXP3, 4.1665795894E-2f)
DEF_CONST(CF4_EXP4, 1.6666665459E-1f)
DEF_CONST(CF4_EXP5, 5.0000001201E-1f)


static inline vec_float4  __attribute__((__always_inline__))
      _expf4( vec_float4 x ){

      vec_float4 z = _floorf4( CF4_LOG2EF * x + CF4_05 ); 
      x -= z * CF4_EXPC1;
      x -= z * CF4_EXPC2;
      vec_int4 n;
      VEC_F2I(n, z);

      z = x * x;

      z =
      ((((( CF4_EXP0  * x
            + CF4_EXP1) * x
            + CF4_EXP2) * x
            + CF4_EXP3) * x
            + CF4_EXP4) * x
            + CF4_EXP5) * z
            + x
            + CF4_1;


      x = _ldexpf4( z, n );

      return( x );
}
#else // __SSE__
static inline vec_float4 _expf4( vec_float4 x ){
      // All constants are provided as vector elements 
      // for fast access
      vec_float4 ce0 ={       1.9875691500E-4f,             // a1
                                    9.32692057443480e-04f,        // a0
                                    -2.92860199031852f,           // b1
                                    24.42440763841549f};          // b0
      vec_float4 ce1 ={       5.27069858231140f,                  // c1
                                    21.94864411350581f,                 // c0
                                    1.44269504088896341f,         // 1/log(2)
                                    0.693147180559945f};          // log(2)
      vec_float4 ce2 ={       0.693359375f,                       // log(2), extended precision mult
                                    0.0f,             
                                    0.5f,             
                                    1.0f};            

      vec_float4 a1 = vec_splat(ce0,0);
      vec_float4 a0 = vec_splat(ce0,1);
      vec_float4 b1 = vec_splat(ce0,2);
      vec_float4 b0 = vec_splat(ce0,3);
      vec_float4 c1 = vec_splat(ce1,0);
      vec_float4 c0 = vec_splat(ce1,1);
      vec_float4 l1 = vec_splat(ce1,2);
      vec_float4 l2 = vec_splat(ce1,3);
      vec_float4 v0 = vec_splat(ce2,1);
      vec_float4 v05= vec_splat(ce2,2);
      vec_float4 v1 = vec_splat(ce2,3);



      vec_float4 z = _floorf4(vec_madd(l1,x,v05)); 
#ifdef __SPU__
      vec_float4 l2l= vec_splat(ce2,0);
      vec_float4 l2s = spu_splats( -2.12194440e-4f );
      x = vec_nmsub(z,l2s,x);
      x = vec_nmsub(z,l2l,x);
#else
      x = vec_nmsub(z,l2,x);
#endif

      vec_int4 n;
      VEC_F2I(n, z);

      z = vec_madd(x, x, v0);


      vec_float4 z2 = vec_add(x , v1);
      vec_float4 y  = vec_madd(x, a1, a0);
      vec_float4 z0 = vec_add(x , b1);
      vec_float4 z1 = vec_add(x , c1);
      vec_float4 y0 = vec_madd(z0, x, b0);
      vec_float4 y1 = vec_madd(z1, x, c0);
      y   = vec_madd(y, z ,v0);
      y0  = vec_madd(y0,y1,v0);
      y   = vec_madd(y, y0, z2);

      y = _ldexpf4( y, n );
#ifdef __SPU__
      y *= spu_splats(1.0000002f); 
#endif      
      return( y );
}
#endif // __SSE__
#endif
