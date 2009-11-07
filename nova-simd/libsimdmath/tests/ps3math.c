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
   Test  SIMD libraries in Cell SDK
   Must be compiled without optimization.
*/

#ifdef __SPU__
      #include <spu_intrinsics.h>
#else
      #define vector __vector
      #define pixel  __pixel
      #define bool   __bool

      #include <altivec.h>
#endif


#include "/usr/include/simdmath.h"
#include <simdmath/atanf4.h>
#include <simdmath/sqrtf4.h>
#include <simdmath/sinf4.h>
#include <simdmath/cosf4.h>
#include <simdmath/tanf4.h>
#include <simdmath/asinf4.h>
#include <simdmath/acosf4.h>
#include <simdmath/expf4.h>
#include <simdmath/logf4.h>
#include <simdmath/powf4.h>



vec_float4 ps3atanf4( vec_float4 x ){
      return _atanf4(  x );
}
#ifdef __SPU__
/*
      "cp_fatan-cbe-spu.c" function by Mike Acton 
      can be inserted here for reference. It is not included
      in the distribution and gave these results:
      // time: 6.26605e-08s
      rms abs: 0.000430534, rms rel: 0.000515888
      max abs: 0.00203829, max rel: 0.00337607

#include "cp_fatan-cbe-spu.h"
vec_float4 ps3atan2f4( vec_float4 y, vec_float4 x ){
      return (vec_float4)_cp_fatan2( (qword)y,(qword)x );
}
*/
#endif
vec_float4 ps3atan2f4( vec_float4 y, vec_float4 x ){
      return _atanf4( y/x );
}

vec_float4 ps3sqrtf4( vec_float4 x ){
      return _sqrtf4(  x );
}

vec_float4 ps3sinf4( vec_float4 x ){
      return _sinf4(  x );
}

vec_float4 ps3cosf4( vec_float4 x ){
      return _cosf4(  x );
}

vec_float4 ps3tanf4( vec_float4 x ){
      return _tanf4(  x );
}

vec_float4 ps3asinf4( vec_float4 x ){
      return _asinf4(  x );
}

vec_float4 ps3acosf4( vec_float4 x ){
      return _acosf4(  x );
}


vec_float4 ps3expf4( vec_float4 x ){
      return _expf4( x );
}

vec_float4 ps3logf4( vec_float4 x ){
      return _logf4( x );
}

vec_float4 ps3powf4( vec_float4 x, vec_float4 y ){
      return _powf4( x, y );
}

