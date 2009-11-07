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

#ifndef POWF4_H
#define POWF4_H

#include "simdconst.h"
#include "fabsf4.h"
#include "floorf4.h"
#include "expf4.h"
#include "logf4.h"

#define FAST_POWF4
#ifndef FAST_POWF4
/*
      A somewhat more accurate, but much slower version of powf4
      Performs similar to Apple's routine, which is slower than
      straight libmath.
*/
static inline int _vec_or( vec_int4 x ){
      union{
            vec_int4 v;
            int i[4];
      }y;
      y.v = x;
      return y.i[0] | y.i[1] | y.i[2] | y.i[3];
}

 static inline vec_float4 _vec_array( float *f, vec_int4 x ){
      union{
            vec_int4 v;
            int i[4];
      }idx;
      idx.v = x;
      return (vec_float4){f[idx.i[0]], f[idx.i[1]],f[idx.i[2]],f[idx.i[3]]};
}



static inline vec_float4 _powif4( vec_float4 x, vec_int4 n ){
      vec_int4 sign_x = _signf4( x );
      VEC_XOR(x, sign_x);

      vec_int4 sign_n = _signi4( n );
      n *= sign_n;


      vec_float4 n_oddf, nf;
      vec_int4   n_odd = CI4_1;
      VEC_AND( n_odd, n );
      VEC_I2F( n_oddf, n_odd );
      nf = n_oddf;

      vec_float4 y = n_oddf * x + CF4_1 - n_oddf;

      vec_float4 w = x;
      n = VEC_SR(n,1);

      while( _vec_or( n ) ){
            w *= w;
            n_odd = CI4_1;
            VEC_AND( n_odd, n );
            VEC_I2F( n_oddf, n_odd );
            y *= n_oddf * w + CF4_1 - n_oddf;
            n = VEC_SR(n,1);
      }


      w = y;
      VEC_XOR(w, sign_x);
      y = nf * w + (CF4_1-nf) * y;

      w = CF4_1/y;
      VEC_I2F(x, VEC_SR(sign_n + CI4_1,1));  // 1 if positiv, else 0
      return x * y + (CF4_1-x) * w;
}

static float A[] = {
  1.00000000000000000000E0,
 9.57603275775909423828125E-1,
 9.17004048824310302734375E-1,
 8.78126084804534912109375E-1,
 8.40896427631378173828125E-1,
 8.05245161056518554687500E-1,
 7.71105408668518066406250E-1,
 7.38413095474243164062500E-1,
 7.07106769084930419921875E-1,
 6.77127778530120849609375E-1,
 6.48419797420501708984375E-1,
 6.20928883552551269531250E-1,
 5.94603538513183593750000E-1,
 5.69394290447235107421875E-1,
 5.45253872871398925781250E-1,
 5.22136867046356201171875E-1,
  5.00000000000000000000E-1
};

static float B[] = {
 0.00000000000000000000E0,
-5.61963907099083340520586E-9,
-1.23776636307969995237668E-8,
 4.03545234539989593104537E-9,
 1.21016171044789693621048E-8,
-2.00949968760174979411038E-8,
 1.89881769396087499852802E-8,
-6.53877009617774467211965E-9,
 0.00000000000000000000E0
};


static float Ainv[] = {
 1.00000000000000000000000E0,
 1.04427378242741384032197E0,
 1.09050773266525765920701E0,
 1.13878863475669165370383E0,
 1.18920711500272106671750E0,
 1.24185781207348404859368E0,
 1.29683955465100966593375E0,
 1.35425554693689272829801E0,
 1.41421356237309504880169E0,
 1.47682614593949931138691E0,
 1.54221082540794082361229E0,
 1.61049033194925430817952E0,
 1.68179283050742908606225E0,
 1.75625216037329948311216E0,
 1.83400808640934246348708E0,
 1.91520656139714729387261E0,
 2.00000000000000000000000E0
};

DEF_CONST(CF4_00625, 0.0625f)
DEF_CONST(CF4_16, 16.f)
DEF_CONST(CF4_32768, 32768.f)

DEF_CONST(CF4_POW0, -0.1663883081054895f)
DEF_CONST(CF4_POW1,  0.2003770364206271f)
DEF_CONST(CF4_POW2, -0.2500006373383951f)
DEF_CONST(CF4_POW3,  0.3333331095506474f)
DEF_CONST(CF4_POW4,  9.416993633606397E-003f)
DEF_CONST(CF4_POW5,  5.549356188719141E-002f)
DEF_CONST(CF4_POW6,  2.402262883964191E-001f)
DEF_CONST(CF4_POW7,  6.931471791490764E-001f)
DEF_CONST(CF4_POW8,  6.77127778530120849609375E-1f)  //A[9])
DEF_CONST(CF4_POW9,  9.57603275775909423828125E-1f)  // A[1])




#define reduc(x)  CF4_00625 * _floorf4( CF4_16 * (x) )


static inline vec_float4 _powf4( vec_float4 x, vec_float4 y ){
      vec_float4 w;// = _floorf4(y);
      vec_float4 z;// = _fabsf4(y);

      vec_int4 sign_x = _signf4(x);
      vec_int4 x_equals_0 = (vec_int4)VEC_EQ(CF4_0,x);
      vec_int4 y_equals_0 = (vec_int4)VEC_EQ(CF4_0,y);
      x_equals_0 = ~(x_equals_0 & ~y_equals_0);

      VEC_XOR(x, sign_x);
/*
      if( _vec_or( __builtin_ia32_cmpeqps(w,y) ) ){ // all components zero
            return _powif4(x, _vec_float_2_int(w)); // NUR POS Y!!!
      }
*/
      vec_int4 e;


      x = _frexpf4( x, &e );


      vec_int4 i = CI4_1;
      i += (VEC_GE( CF4_POW8, x) & CI4_8);
      i += (VEC_GE( _vec_array(A,i+CI4_4), x) & CI4_4);
      i += (VEC_GE( _vec_array(A,i+CI4_2), x) & CI4_2);
      i =  (vec_int4) VEC_LT( x, CF4_POW9) & (i+CI4_1);



      x -= _vec_array(A,i);
      x -= _vec_array(B, VEC_SR(i,1) );
      x *= _vec_array(Ainv,i);

      z = x*x;
      w = (((CF4_POW0  * x
      + CF4_POW1) * x
      + CF4_POW2) * x
      + CF4_POW3) * x * z;
      w -= CF4_05 * z;


      w = w + CF4_LOG2EA * w;

      z = w + CF4_LOG2EA * x;
      z = z + x;


      VEC_I2F(w, i);
      w *= -CF4_00625;
      vec_float4 w_add;
      VEC_I2F(w_add, e);
      w += w_add;


      vec_float4 ya = reduc(y);
      vec_float4 yb = y - ya;


      vec_float4 F = z * y  +  w * yb;
      vec_float4 Fa = reduc(F);
      vec_float4 Fb = F - Fa;

      vec_float4 G = Fa + w * ya;
      vec_float4 Ga = reduc(G);
      vec_float4 Gb = G - Ga;

      vec_float4 H = Fb + Gb;
      vec_float4 Ha = reduc(H);
      w = CF4_16 * (Ga + Ha);

      VEC_F2I(e, w);
      vec_float4 Hb = H - Ha;

      vec_int4 Hb_larger_0 = (vec_int4)VEC_GT(Hb, CF4_0);
      e  += (Hb_larger_0 & CI4_1);
      vec_float4 Hb_add = CF4_00625;
      VEC_AND( Hb_add, Hb_larger_0 );
      Hb -= Hb_add;

      z = ((( CF4_POW4 * Hb
      + CF4_POW5) * Hb
      + CF4_POW6) * Hb
      + CF4_POW7) * Hb;

      vec_int4 e_negativ = -VEC_SR(e & CI4_SIGN,31);
      i =  (-VEC_SR(-e,4))      &  e_negativ |
             ( VEC_SR(e,4)+CI4_1) & ~e_negativ;
      e = VEC_SL(i,4)-e;
      w = _vec_array(A,e);
      z = w + w * z;

      z = _ldexpf4( z, i );

      w = CF4_2 * _floorf4( CF4_05 * y ); // Error in cephes
      vec_int4 w_odd = (vec_int4)VEC_NE(w,y) & sign_x;
      VEC_XOR( z, w_odd);
      VEC_AND( z, x_equals_0);
      return z ;
}

#else // FAST_POWF4

static inline vec_float4 _powf4( vec_float4 x, vec_float4 y ){
#ifdef __SSE__
      #define v0 CF4_0
      #define v1 CF4_1
#else // __SSE__
      vec_float4 c0 ={  0.0f,
                        1.0f,
                        0.0f,
                        0.0f};
      vec_float4 v0  = vec_splat(c0,0);
      vec_float4 v1  = vec_splat(c0,1);
#endif // __SSE__
      vec_int4 x_0 = (vec_int4)VEC_EQ(x,v0);
      vec_int4 y_0 = (vec_int4)VEC_EQ(y,v0);
      return vec_sel( _expf4(_logf4(x)*y), vec_sel( v0, v1, (vec_int4)y_0),
                                    (vec_int4)x_0 );
}
#endif
#endif
