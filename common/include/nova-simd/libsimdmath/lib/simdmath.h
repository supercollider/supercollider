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

#ifndef SIMDMATH_H
#define SIMDMATH_H

#ifdef __SPU__
      #include <spu_intrinsics.h>
      #include <spu_mfcio.h>
#endif // __SPU__


#ifdef __PPU__
      #include <altivec.h>
      typedef vector float         vec_float4;
      typedef vector signed int    vec_int4;
      typedef vector unsigned int  vec_uint4;
#endif // __PPU__

#ifdef __SSE__
      typedef float           vec_float4 __attribute__ ((vector_size (16)));
      typedef int             vec_int4   __attribute__ ((vector_size (16)));
      typedef unsigned int    vec_uint4  __attribute__ ((vector_size (16)));
#endif // __SSE__

extern vec_float4 atanf4(  vec_float4 x );
extern vec_float4 atan2f4( vec_float4 y, vec_float4 x );
extern vec_float4 sqrtf4(  vec_float4 x );
extern vec_float4 sinf4(   vec_float4 x );
extern vec_float4 cosf4(   vec_float4 x );
extern vec_float4 tanf4(   vec_float4 x );
extern vec_float4 asinf4(  vec_float4 x );
extern vec_float4 acosf4(  vec_float4 x );
extern vec_float4 fabsf4(  vec_float4 x );
extern vec_float4 floorf4( vec_float4 x );
extern vec_float4 expf4(   vec_float4 x );
extern vec_float4 logf4(   vec_float4 x );
extern vec_float4 powf4(   vec_float4 x, vec_float4 y );

#endif // SIMDMATH_H
