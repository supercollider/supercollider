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
#ifndef FABSF4_H
#define FABSF4_H

#include "simdconst.h"


static inline vec_float4  __attribute__((__always_inline__))
      _fabsf4( vec_float4 x ){
#ifdef __SSE__
      VEC_AND(x, vec_rsign_mask());
#else
      VEC_AND(x, CI4_RSIGN);
#endif

      return x;
}


static inline vec_int4  __attribute__((__always_inline__))
      _signf4( vec_float4 x ){
#ifdef __SSE__
      vec_int4 a = vec_sign_mask();
#else
      vec_int4 a = CI4_SIGN;
#endif
      VEC_AND(a, x);
      return a;
}

static inline vec_int4  __attribute__((__always_inline__))
      _signi4( vec_int4 x ){
#ifdef __SSE__
      vec_int4 a = vec_sign_mask();
#else
      vec_int4 a = CI4_SIGN;
#endif
      VEC_AND( a, x);
      return CI4_1 - (vec_int4)VEC_SR( a, 30 ); // +/- 1
}

#endif
