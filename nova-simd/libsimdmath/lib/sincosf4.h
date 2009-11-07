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
#ifndef SINCOSF4_H
#define SINCOSF4_H

#include "simdconst.h"
#include "fabsf4.h"


/*
sin / cos:
Base polynomial from cephes library

must be compiled with  --param max-inline-insns-single=1200 !

Performance (Playstation 3, YDL 6):

sin:
Interval -12.5664 .... 12.5664
libsimdmath:
time: 5.53222e-08s
rms abs: 2.09016e-08, rms rel: 3.17429e-08
max abs: 8.35695e-08, max rel: 1.14062e-07
sum: 171.393
libmath:
time: 8.99497e-07s
rms abs: 1.78311e-08, rms rel: 2.60236e-08
max abs: 7.43757e-08, max rel: 9.87253e-08
sum: 171.393
ps3:
time: 5.94029e-08s
rms abs: 2.16034e-08, rms rel: 1.89525e-07
max abs: 9.38151e-08, max rel: 5.86425e-05
sum: 171.393

cos:
Interval -12.5664 .... 12.5664
libsimdmath:
time: 6.09851e-08s
rms abs: 2.11238e-08, rms rel: 3.31267e-08
max abs: 9.15287e-08, max rel: 1.27717e-07
sum: -6099.05
libmath:
time: 9.0185e-07s
rms abs: 1.76233e-08, rms rel: 2.57262e-08
max abs: 7.21546e-08, max rel: 9.78021e-08
sum: -6099.05
ps3:
time: 6.03609e-08s
rms abs: 2.16609e-08, rms rel: 3.98844e-08
max abs: 9.0456e-08, max rel: 5.09086e-06
sum: -6099.04




*/

#ifdef __SSE__

DEF_CONST(CF4_FOPI, 1.27323954473516f);

DEF_CONST(DP1, 0.78515625f)
DEF_CONST(DP2, 2.4187564849853515625e-4f)
DEF_CONST(DP3, 3.77489497744594108e-8f)

DEF_CONST(sf0, -1.9515295891E-4f)
DEF_CONST(sf1,  8.3321608736E-3f)
DEF_CONST(sf2, -1.6666654611E-1f)

DEF_CONST(cf0,  2.443315711809948E-005f)
DEF_CONST(cf1, -1.388731625493765E-003f)
DEF_CONST(cf2,  4.166664568298827E-002)


static inline void  __attribute__((__always_inline__))
      _sincosf4( vec_float4 x, vec_float4 *s, vec_float4 *c ){
      vec_float4 x1,y,y0,y1;


#define fpi  CF4_FOPI
#define dp1  DP1
#define dp2  DP2
#define dp3  DP3
#define v05  CF4_05
#define v1   CF4_1

#define   i0   CI4_0
#define   i1   CI4_1
#define   i3   CI4_3
#define   i7   CI4_7
#define   is   CI4_SIGN

      vec_int4 is = vec_sign_mask();

      vec_int4 sign_s = vec_and((vec_int4)x, is);
      VEC_NAND3(x, x,is);
      vec_int4   j;
      x1 = fpi * x;
      VEC_F2I(j, x1); /* integer part of x/(PI/4) */
      j += j & i1;
      VEC_I2F(y, j );


      j &= i7;

      vec_int4 sign_c  = -(vec_int4)VEC_SR(vec_and(j, CI4_4),2) & is;

      sign_s  ^=  sign_c; // revert sign if j=4,6
      j         &=  i3;   // j= 0,2

      vec_int4 j_larger_1 = -VEC_SR(vec_and(j, CI4_2),1);
      sign_c ^=   j_larger_1 & is;


      x = vec_nmsub(y, dp1,x);
      x = vec_nmsub(y, dp2,x);
      x = vec_nmsub(y, dp3,x);

      vec_float4 z = x * x;

      vec_int4 j_equals_1_or_2 = - VEC_SR( vec_and(j, CI4_2), 1 );

      y0  = (( cf0 * z + cf1 ) * z + cf2) * z * z - v05 * z + v1;
      y1  = (( sf0 * z + sf1 ) * z + sf2) * z * x + x;

      y = vec_sel(y1,y0,(vec_int4)j_equals_1_or_2);

      VEC_XOR(y, sign_s);
      *s = y;

      y = vec_sel(y0,y1,(vec_int4)j_equals_1_or_2);
      VEC_XOR(y, sign_c);
      *c = y;
}

#else // !__SSE__

static  inline void _sincosf4( vec_float4 x, vec_float4 *s, vec_float4 *c ){
      // All constants are provided as vector elements
      // for fast access
      vec_float4 c0 ={  1.27323954473516f,            // 4/pi
                        -1.9515295891E-4f,            // sf0
                        8.3321608736E-3f,             // sf1
                        -1.6666654611E-1f};           // sf2
      vec_float4 c1 ={  2.443315711809948E-005f,      // cf0
                        -1.388731625493765E-003f,     // cf1
                        4.166664568298827E-002f,      // cf2
                        0.78515625f};                 // dp1
      vec_float4 c2 ={  2.4187564849853515625e-4f,    // dp2
                        3.77489497744594108e-8f,      // dp3
                        0.5f,
                        1.0f};
      vec_int4  iv0 = { 0, 1, 3, 7};
      vec_int4  iv1 = { 0x80000000u, 1, 3, 7};

      vec_float4 fpi = vec_splat(c0,0);
      vec_float4 sf0 = vec_splat(c0,1);
      vec_float4 sf1 = vec_splat(c0,2);
      vec_float4 sf2 = vec_splat(c0,3);
      vec_float4 cf0 = vec_splat(c1,0);
      vec_float4 cf1 = vec_splat(c1,1);
      vec_float4 cf2 = vec_splat(c1,2);
      vec_float4 dp1 = vec_splat(c1,3);
      vec_float4 dp2 = vec_splat(c2,0);
      vec_float4 dp3 = vec_splat(c2,1);
      vec_float4 v05 = vec_splat(c2,2);
      vec_float4 v1  = vec_splat(c2,3);

      vec_int4   i0  = vec_splat(iv0,0);
      vec_int4   i1  = vec_splat(iv0,1);
      vec_int4   i3  = vec_splat(iv0,2);
      vec_int4   i7  = vec_splat(iv0,3);
      vec_int4   is  = vec_splat(iv1,0);

      vec_float4 x1,y,y0,y1;


      vec_int4 sign_s = vec_and((vec_int4)x, is);
      VEC_NAND3(x, x, is);
      vec_int4   j;
      x1 = vec_madd(fpi,x,(vec_float4)i0);
      VEC_F2I(j, x1); /* integer part of x/(PI/4) */
      j += j & i1;
      VEC_I2F(y, j );

      j &= i7; // j=0,2,4,6

      /* reflect in x axis */
      vec_int4 sign_c  = (vec_int4)VEC_GT(j, i3) & is;

      sign_s  ^=  sign_c; // revert sign if j=4,6
      j         &=  i3; // j= 0,2



      vec_int4 j_larger_1 = (vec_int4)VEC_GT(j, i1);
      sign_c ^=   j_larger_1 & is;


      x = vec_nmsub(y, dp1,x);
      x = vec_nmsub(y, dp2,x);
      x = vec_nmsub(y, dp3,x);


      vec_int4 j_equals_1_or_2 = (vec_int4)VEC_GT(j, i0);


      vec_float4 z =  vec_madd(x, x,(vec_float4)i0);
      vec_float4 zx = vec_madd(z, x,(vec_float4)i0);
      vec_float4 zz = vec_madd(z, z,(vec_float4)i0);
      vec_float4 zc = vec_nmsub( v05,  z , v1);

      y0  = vec_madd(cf0 , z , cf1);
      y1  = vec_madd(sf0 , z , sf1);
      y0  = vec_madd(y0,   z , cf2);
      y1  = vec_madd(y1,   z , sf2);
      y0  = vec_madd(y0 ,  zz , zc);
      y1  = vec_madd(y1,   zx , x);

      y = vec_sel(y1,y0,(vec_int4)j_equals_1_or_2);

      VEC_XOR(y, sign_s);
      *s = y;

      y = vec_sel(y0,y1,(vec_int4)j_equals_1_or_2);

      VEC_XOR(y, sign_c);
      *c = y;
}

#endif // __SSE__

static inline vec_float4  __attribute__((__always_inline__))
      _sinf4( vec_float4 x ){
      vec_float4 s,c;
      _sincosf4(x, &s, &c);
      return s;
}

static inline vec_float4  __attribute__((__always_inline__))
      _cosf4( vec_float4 x ){
      vec_float4 s,c;
      _sincosf4(x, &s, &c);
      return c;
}


/*
Base polynomial from cephes library,
decomposed into quadratic functions
for parallel execution.

y = (a1*x+a0)*(x^2+b1*x+b0)*(x^2+d1*x+d0)

must be compiled with  --param max-inline-insns-single=1200 !

Performance (Playstation 3, YDL 6):
tan:
Interval -1.5698 .... 1.5698
libsimdmath:
time: 7.111e-08s
rms abs: 9.95818e-07, rms rel: 4.18408e-08
max abs: 8.35369e-05, max rel: 1.64889e-07
sum: -770064
libmath:
time: 7.79234e-07s
rms abs: 5.09711e-07, rms rel: 2.57097e-08
max abs: 2.83545e-05, max rel: 6.87513e-08
sum: -770064
ps3:
time: 1.21019e-07s
rms abs: 1.27446e-06, rms rel: 5.6e-08
max abs: 0.000178149, max rel: 2.5475e-07
sum: -770064

*/

#ifdef __SSE__
DEF_CONST(tf0, 9.38540185543E-3f)
DEF_CONST(tf1, 3.11992232697E-3f)
DEF_CONST(tf2, 2.44301354525E-2f)
DEF_CONST(tf3, 5.34112807005E-2f)
DEF_CONST(tf4, 1.33387994085E-1f)
DEF_CONST(tf5, 3.33331568548E-1f)

static inline vec_float4  __attribute__((__always_inline__))
      _tanf4( vec_float4 x ){
#define fpi  CF4_FOPI
#define dp1  DP1
#define dp2  DP2
#define dp3  DP3
#define v05  CF4_05
#define v1   CF4_1

#define   i0   CI4_0
#define   i1   CI4_1
#define   i3   CI4_3
#define   i7   CI4_7

      vec_int4 sign = vec_and((vec_int4)x, (vec_int4)vec_sign_mask());
      VEC_XOR(x, sign);

      vec_int4   j;
      VEC_F2I(j, fpi * x);
      j += j & i1;
      vec_float4 y;
      VEC_I2F(y, j );

      vec_float4 z = vec_nmsub(y,dp1,x);
      z = vec_nmsub(y, dp2,z);
      z = vec_nmsub(y, dp3,z);

      vec_float4 zz = z * z;

      vec_int4 x_larger_1e_4 = (vec_int4)VEC_GT(x, CF4_00004);
      y =   (((((( tf0 * zz) + tf1) * zz + tf2) * zz + tf3) * zz
                              + tf4) * zz + tf5) * zz * z + z;
      VEC_AND(y, x_larger_1e_4);
      VEC_NAND(x_larger_1e_4, z);
      VEC_OR(y, x_larger_1e_4);

      vec_int4 j_and_2 = -VEC_SR(vec_and(j, CI4_2),1);
      z = CF4__1/(y+CF4_SMALL);
      z = (vec_float4) vec_sel(y,z,(vec_int4)j_and_2);
      VEC_XOR(z, sign);

      return z;

}
#else // __SSE__

static inline vec_float4 _tanf4( vec_float4 x ){
      // All constants are provided as vector elements
      // for fast access
      vec_float4 c0 ={  9.38540185543E-3f,            // a1
                        0.0165565478411853f,          // a0
                        -2.83261451482425f,           // b1
                        5.29383139560134f};           // b0
      vec_float4 c1 ={  1.40096292214989f,            // d1
                        3.80308955552993f,            // d0
                        1.27323954473516f,            // 4/pi
                        0.78515625f};                 // dp1
      vec_float4 c2 ={  2.4187564849853515625e-4f,    // dp2
                        3.77489497744594108e-8f,      // dp3
                        0.5f,
                        1.0f};
      vec_int4  iv0 = { 0, 1, 3, 0x80000000u};

      vec_float4 a1  = vec_splat(c0,0);
      vec_float4 a0  = vec_splat(c0,1);
      vec_float4 b1  = vec_splat(c0,2);
      vec_float4 b0  = vec_splat(c0,3);
      vec_float4 d1  = vec_splat(c1,0);
      vec_float4 d0  = vec_splat(c1,1);
      vec_float4 fpi = vec_splat(c1,2);
      vec_float4 dp1 = vec_splat(c1,3);
      vec_float4 dp2 = vec_splat(c2,0);
      vec_float4 dp3 = vec_splat(c2,1);
      vec_float4 v05 = vec_splat(c2,2);
      vec_float4 v1  = vec_splat(c2,3);

      vec_int4   i0  = vec_splat(iv0,0);
      vec_int4   i1  = vec_splat(iv0,1);
      vec_int4   i3  = vec_splat(iv0,2);
      vec_int4   is  = vec_splat(iv0,3);


      /* make argument positive and save the sign */
      vec_int4 sign = vec_and((vec_int4)x, is);
      VEC_XOR(x, sign);

      vec_int4   j;
      VEC_F2I(j, fpi * x); /* integer part of x/(PI/4) */
      j += j & i1;
      vec_float4 y;
      VEC_I2F(y, j );

      vec_float4 z = vec_nmsub(y,dp1,x);
      z = vec_nmsub(y, dp2,z);
      z = vec_nmsub(y, dp3,z);


      vec_float4 zz = vec_madd(z, z, (vec_float4)i0);
      vec_float4 z3 = vec_madd(z, zz,(vec_float4)i0);

      y  = vec_madd(zz, a1, a0);
      vec_float4 z0 = vec_add(zz, b1);
      vec_float4 z1 = vec_add(zz, d1);
      vec_float4 y0 = vec_madd(z0, zz, b0);
      vec_float4 y1 = vec_madd(z1, zz, d0);
      y  = vec_madd(y, z3,(vec_float4)i0);
      y0 = vec_madd(y0,y1,(vec_float4)i0);
      y  = vec_madd(y ,y0,z);


      vec_int4 j_and_2 = (vec_int4)VEC_GT(j, i0);
      z = -_recipf4(y);
      z = (vec_float4) vec_sel(y,z,(vec_int4)j_and_2);
      VEC_XOR(z, sign);
      return z;

}
#endif // __SSE__
#endif
