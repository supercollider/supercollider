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
   Make vec_float4 versions from standard C library functions
   for comparison
*/


#include "simdmath.h"
#include <math.h>


#define VEC_FUNC1( func )   vec_float4 func ## 4 (vec_float4 x){ \
                                    union{                        \
                                          vec_float4 v;           \
                                          float f[4];             \
                                    }y,z;                         \
                                    y.v = x;                      \
                                    z.f[0] = func( y.f[0] );      \
                                    z.f[1] = func( y.f[1] );      \
                                    z.f[2] = func( y.f[2] );      \
                                    z.f[3] = func( y.f[3] );      \
                                    return z.v;                   \
                              }
#define VEC_FUNC2( func )   vec_float4 func ## 4 (vec_float4 x, vec_float4 y){ \
                                    union{                              \
                                          vec_float4 v;                 \
                                          float f[4];                   \
                                    }x1,y1,z;                           \
                                    x1.v = x;                           \
                                    y1.v = y;                           \
                                    z.f[0] = func( x1.f[0], y1.f[0] );  \
                                    z.f[1] = func( x1.f[1], y1.f[1] );  \
                                    z.f[2] = func( x1.f[2], y1.f[2] );  \
                                    z.f[3] = func( x1.f[3], y1.f[3] );  \
                                    return z.v;                         \
                              }

VEC_FUNC1( sinf )
VEC_FUNC1( cosf )
VEC_FUNC1( tanf )
VEC_FUNC1( asinf )
VEC_FUNC1( acosf )
VEC_FUNC1( atanf )
VEC_FUNC1( sqrtf )
VEC_FUNC1( expf )
VEC_FUNC1( logf )
VEC_FUNC2( powf )
VEC_FUNC2( atan2f )
