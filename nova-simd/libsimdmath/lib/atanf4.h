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

#ifndef ATANF4_H
#define ATANF4_H

#include "simdconst.h"
#include "fabsf4.h"



/*

atan:
Interval -10 .... 10
libsimdmath:
time: 3.9783e-08s
rms abs: 6.20469e-08, rms rel: 4.80861e-08
max abs: 1.92718e-07, max rel: 1.79545e-07
sum: 1885.87
libmath:
time: 4.84865e-07s
rms abs: 3.33867e-08, rms rel: 2.63467e-08
max abs: 8.87853e-08, max rel: 7.82277e-08
sum: 1885.86
ps3:
time: 4.231e-08s
rms abs: 6.86579e-08, rms rel: 5.41484e-08
max abs: 2.44287e-07, max rel: 2.31609e-07
sum: 1885.87

*/

#ifdef __SSE__

DEF_CONST(CF4_2414213562373095, 2.414213562373095f) 
DEF_CONST(CF4_04142135623730950, 0.4142135623730950f) 
DEF_CONST(CF4_805374449538e_2,      8.05374449538e-2f) 
DEF_CONST(CF4_138776856032E_1,      1.38776856032E-1f) 
DEF_CONST(CF4_199777106478E_1,      1.99777106478E-1f) 
DEF_CONST(CF4_333329491539E_1,      3.33329491539E-1f) 

static inline vec_float4 __attribute__((__always_inline__))
            _atanf4( vec_float4 x ){
      vec_float4 y, z,z1,z2;
      vec_int4 a1, a2, a3;
      /* make argument positive and save the sign */
      vec_int4 sign = _signf4( x );
      VEC_XOR(x, sign);
      
      /* range reduction */
      a1 = (vec_int4)VEC_GT (x , CF4_2414213562373095 );
      a2 = (vec_int4)VEC_GT (x , CF4_04142135623730950 );
      a3 = ~a2; 
      a2 ^= a1;

      z1 = CF4__1 / (x+CF4_SMALL);
      z2 = (x-CF4_1)/(x+CF4_1);
      VEC_AND(z1, a1);
      VEC_AND(z2, a2);
      VEC_AND(x, a3);
      VEC_OR(x, z1);
      VEC_OR(x, z2);
      
      y = CF4_PIO2F;
      z1 = CF4_PIO4F;
      VEC_AND(y, a1);
      VEC_AND(z1, a2);
      VEC_OR(y, z1);

      z = x * x;
      y +=
            ((( CF4_805374449538e_2 * z
            - CF4_138776856032E_1) * z
            + CF4_199777106478E_1) * z
            - CF4_333329491539E_1) * z * x
            + x;

      VEC_XOR(y, sign);
      return y;
}

#else // __SSE__

static inline vec_float4 _atanf4(vec_float4 x){
      vec_float4 c0 ={  0.0028662257f,                      // a1
                                    -0.00718684420434729f,        // a0
                                    -3.83195178589435f,           // b1
                                    5.50314831143450f};                 // b0
      vec_float4 c1 ={  -1.00488174720983f,                 // d1
                                    3.76840794390466f,                  // d0
                                    1.70417985002100f,                  // e1
                                    2.23650017675129f};                 // e0
      vec_float4 c2 ={  2.4187564849853515625e-4f,    // dp2
                                    1.570796326794896619f,      // pi/2
                                    0.5f,             
                                    1.0f};      
      vec_int4  iv0 = { 0, 1, 3, 0x80000000u};  

      vec_float4 a1  = vec_splat(c0,0);
      vec_float4 a0  = vec_splat(c0,1);
      vec_float4 b1  = vec_splat(c0,2);
      vec_float4 b0  = vec_splat(c0,3);
      vec_float4 d1  = vec_splat(c1,0);
      vec_float4 d0  = vec_splat(c1,1);
      vec_float4 e1  = vec_splat(c1,2);
      vec_float4 e0  = vec_splat(c1,3);
      vec_float4 pi2 = vec_splat(c2,1);
      vec_float4 v05 = vec_splat(c2,2);
      vec_float4 v1  = vec_splat(c2,3);

      vec_int4   i0  = vec_splat(iv0,0);  
      vec_int4   i1  = vec_splat(iv0,1);  
      vec_int4   i3  = vec_splat(iv0,2);  
      vec_int4   is  = vec_splat(iv0,3);  

      vec_float4 bias, inv_x, xabs;
      vec_uint4 sign, select;
 
      sign = (vec_uint4)is;
      VEC_AND(sign, x);
      VEC_NAND3(xabs, x, is);
    

      inv_x = _recipf4(x);
      VEC_XOR(inv_x, is);


      select = (vec_uint4)VEC_GT(xabs, v1);

      VEC_OR(sign, pi2);
      VEC_AND(sign, select);
      bias = (vec_float4)sign;


      x = vec_sel(x, inv_x, (vec_uint4)select);

      vec_float4  z  = vec_madd(x,x,(vec_float4)i0);
      vec_float4  zm = vec_madd(z,x,(vec_float4)i0);
      bias = vec_add(bias,x);

      vec_float4 y  = vec_madd(z, a1, a0);
      vec_float4 z0 = vec_add(z, b1);
      vec_float4 z1 = vec_add(z, d1);
      vec_float4 z2 = vec_add(z, e1); 
      vec_float4 y0 = vec_madd(z0, z,b0);
      vec_float4 y1 = vec_madd(z1, z,d0); 
      vec_float4 y2 = vec_madd(z2, z,e0); 
      y   = vec_madd(zm,y,(vec_float4)i0);
      y0  = vec_madd(y1,y0,(vec_float4)i0);
      y2  = vec_madd(y,y2,(vec_float4)i0);
      y0  = vec_madd(y0,y2,bias);
      return y0;  
}

#endif
/*

atan2:
Interval x: -5 .... 5
Interval y: -5 .... 5
libsimdmath:
time: 7.48889e-08s
rms abs: 7.84672e-08, rms rel: 4.5569e-08
max abs: 3.2948e-07, max rel: 2.01993e-07
sum: -7.05909
libmath:
time: 1.13799e-06s
rms abs: 5.49829e-08, rms rel: 3.14913e-08
max abs: 2.43593e-07, max rel: 1.20541e-07
sum: -7.09837

*/

#ifdef __SSE__
static inline vec_float4  __attribute__((__always_inline__))
      _atan2f4( vec_float4 y, vec_float4 x ){
      vec_float4 z, w;
      vec_float4 x_neg_PI    = CF4_PIF;
      VEC_AND(x_neg_PI, VEC_GT( CF4_0, x ));
      vec_float4 y_negativ_2 = CF4_2;
      VEC_AND(y_negativ_2, VEC_GT( CF4_0, y ));

      vec_int4 i_x_zero  = (vec_int4)VEC_EQ ( CF4_0, x );
      vec_int4 i_y_zero  = (vec_int4)VEC_EQ ( CF4_0, y );
      vec_float4 x_zero_PIO2 = CF4_PIO2F;
      VEC_AND(x_zero_PIO2, i_x_zero);
      vec_float4 y_zero    = CF4_1;
      VEC_AND(y_zero, i_y_zero);


      w = x_neg_PI *  ( CF4_1  - y_negativ_2 );

      z = _atanf4( y / (x+x_zero_PIO2));
      VEC_AND(z, ~(i_x_zero|i_y_zero));

      return w + z + x_zero_PIO2 * ( CF4_1 - y_zero - y_negativ_2 )
      + y_zero *  x_neg_PI;
}
#else // __SSE__
static inline vec_float4 _atan2f4( vec_float4 y, vec_float4 x ){
      // All constants are provided as vector elements 
      // for fast access
      vec_float4 c0 ={  3.14159265358979323846f,      // pi
                                    1.570796326794896619f,        // pi/2
                                    1.0f,                 
                                    0.0f};                  
      vec_float4 pi  = vec_splat(c0,0);
      vec_float4 pi2 = vec_splat(c0,1);
      vec_float4 v1  = vec_splat(c0,2);
      vec_float4 v0  = vec_splat(c0,3);
      vec_float4 v2  = vec_add(v1,v1);

      vec_float4 z, w;
      vec_float4 x_neg_PI    = pi;
      VEC_AND(x_neg_PI, VEC_GT( v0, x ));
      vec_float4 y_negativ_2 = v2;
      VEC_AND(y_negativ_2, VEC_GT( v0, y ));

      vec_int4 i_x_zero  = (vec_int4)VEC_EQ ( v0, x );
      vec_int4 i_y_zero  = (vec_int4)VEC_EQ ( v0, y );
      vec_float4 x_zero_PIO2 = pi2;
      VEC_AND(x_zero_PIO2, i_x_zero);
      vec_float4 y_zero    = v1;
      VEC_AND(y_zero, i_y_zero);


      w = x_neg_PI *  ( v1  - y_negativ_2 );

      z = _atanf4( y * _recipf4(x));
      VEC_AND(z, ~(i_x_zero|i_y_zero));

      return w + z + x_zero_PIO2 * ( v1 - y_zero - y_negativ_2 )
      + y_zero *  x_neg_PI;
}
#endif //__SSE__
#endif


