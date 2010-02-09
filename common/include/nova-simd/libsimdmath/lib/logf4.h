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
#ifndef LOGF4_H
#define LOGF4_H

#include "simdconst.h"
#include "fabsf4.h"
#include "floorf4.h"

#ifdef __SSE__
DEF_CONST(CF4_SQRTHF, 0.707106781186547524f)
DEF_CONST(CF4_LOG0, 7.0376836292E-2f)
DEF_CONST(CF4_LOG1, - 1.1514610310E-1f)
DEF_CONST(CF4_LOG2, 1.1676998740E-1f)
DEF_CONST(CF4_LOG3, - 1.2420140846E-1f)
DEF_CONST(CF4_LOG4, 1.4249322787E-1f)
DEF_CONST(CF4_LOG5, - 1.6668057665E-1f)
DEF_CONST(CF4_LOG6, 2.0000714765E-1f)
DEF_CONST(CF4_LOG7, - 2.4999993993E-1f)
DEF_CONST(CF4_LOG8, 3.3333331174E-1f)
DEF_CONST(CF4_LOG9, -2.12194440e-4)
DEF_CONST(CF4_LOGA, 0.693359375)



static inline vec_float4  __attribute__((__always_inline__))
      _logf4( vec_float4 x ){
      vec_int4 e;
      x = _frexpf4( x, &e );

      vec_int4 x_smaller_SQRTHF = (vec_int4)VEC_GT(CF4_SQRTHF, x);
      e += x_smaller_SQRTHF;
      vec_float4 x_add = x;
      VEC_AND(x_add, x_smaller_SQRTHF);
      x += x_add + CF4__1;

      vec_float4 z = x * x;

      vec_float4 y =
            (((((((( CF4_LOG0 * x
                  + CF4_LOG1) * x
                  + CF4_LOG2) * x
                  + CF4_LOG3) * x
                  + CF4_LOG4) * x
                  + CF4_LOG5) * x
                  + CF4_LOG6) * x
                  + CF4_LOG7) * x
                  + CF4_LOG8) * x * z;

      vec_float4 fe;
      VEC_I2F(fe, e); 
      y += fe*CF4_LOG9;

      y -=  CF4_05 * z;  /* y - 0.5 x^2 */
      z  = x + y;        /* ... + x  */

      return z + CF4_LOGA * fe;
}
#else // __SSE__


/*

Base polynomial from cephes library, decomposed into quadratic functions 
for parallel execution.

log(1+x) = (a2*x^2+a1*x+a0)*(x^2+b1*x+b0)*(x^2+c1*x*c0) *(x^2+d1*x+d0)*x^3-0.5*x^2+x





Performance (Playstation 3, YDL 6):
Interval a = 0.001f;    b = 1000.0f;

time: 4.5908e-08s
rms abs: 1.35258e-07, rms rel: 2.32583e-08
max abs: 2.83842e-07, max rel: 8.55807e-08

libmath:
time: 8.22383e-07s
rms abs: 1.35052e-07, rms rel: 2.32082e-08
max abs: 2.59424e-07, max rel: 7.60729e-08

ps3-libsimdmath:
time: 4.71961e-08s
rms abs: 2.21681e-07, rms rel: 4.6647e-08
max abs: 6.70667e-07, max rel: 5.62086e-06



*/

static inline vec_float4 _logf4( vec_float4 x ){
      // All constants are provided as vector elements 
      // for fast access
      vec_float4 cl0 ={ 7.0376836292E-2f,             // a2
                        0.1272005596232987f,          // a1
                        0.0945159719438509f,          // a0
                        0.350754689537964f};          // b1
      vec_float4 cl1 ={ 1.386218787509749f,           // b0
                        -1.34222384332336f,           // c1
                        1.52207708547017f,            // c0
                        -2.45208806606148f};          // d1
      vec_float4 cl2 ={ 1.67149500867827f,            // d0
                        0.5f,
                        0.693147180559945f,           // log(2)
                        0.707106781186547524f};       // sqrt(0.5)
      vec_float4 cl3 ={ 0.693359375,                  // log(2), extended precision mult.
                        -2.12194440e-4f,              // log(2), extended precision mult.
                        -1.0f,
                        0.0f};

      vec_float4 cf_0=vec_splat(cl3,3);               // zero, needed often

      vec_int4 e;                                                       
      
      x = _frexpf4( x, &e );  
      // 0.5 < x < 1                                  

      vec_int4 x_smaller_SQRTHF = (vec_int4)VEC_GT(vec_splat(cl2,3),x);
      e += x_smaller_SQRTHF;
      vec_float4 fe,fe1;
      VEC_I2F(fe, e); 

      vec_float4 x_add = x;
      VEC_AND(x_add, x_smaller_SQRTHF);
      x += x_add + vec_splat(cl3,2);
      
      // -0.293 < x < 0.414
      
      vec_float4 z  = x * x;
      vec_float4 zx = z * x;

      fe1 = vec_madd(fe ,vec_splat(cl3,1),x);
      vec_float4 z0 = vec_madd(vec_splat(cl0,0),x,vec_splat(cl0,1));
      vec_float4 z1 = vec_add(x,vec_splat(cl0,3));
      vec_float4 z2 = vec_add(x,vec_splat(cl1,1));
      vec_float4 z3 = vec_add(x,vec_splat(cl1,3));
      vec_float4 z4 = vec_nmsub(vec_splat(cl2,1),z,fe1);

      vec_float4 y0 = vec_madd(z0, x, vec_splat(cl0,2));
      vec_float4 y1 = vec_madd(z1, x, vec_splat(cl1,0));
      vec_float4 y2 = vec_madd(z2, x, vec_splat(cl1,2));
      vec_float4 y3 = vec_madd(z3, x, vec_splat(cl2,0));
      y0 = vec_madd(y0,zx,cf_0);
      y1 = vec_madd(y1,y2,cf_0);
      y0 = vec_madd(y0,y3,cf_0);
      
      vec_float4 y = vec_madd(y0,y1,z4);

      return y + vec_splat(cl3,0)*fe;
}

#endif // __SSE__
#endif
