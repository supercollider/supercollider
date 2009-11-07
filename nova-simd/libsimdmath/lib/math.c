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

#include "simdmath.h"

#include "fabsf4.h"
#include "floorf4.h"
#include "atanf4.h"
#include "sqrtf4.h"
#include "sincosf4.h"
#include "asinf4.h"
#include "expf4.h"
#include "logf4.h"
#include "powf4.h"

#define VFUNC1(f) vec_float4 f(vec_float4 x){ return _ ## f(x);}
#define VFUNC2(f) vec_float4 f(vec_float4 x, vec_float4 y)\
                                            { return _ ## f(x,y);}
#define VFUNC3(f) vec_float4 f(vec_float4 x, vec_int4 y)\
                                            { return _ ## f(x,y);}
#define VFUNC4(f) vec_float4 f(vec_float4 x, vec_int4 *y)\
                                            { return _ ## f(x,y);}


VFUNC1(atanf4)
VFUNC2(atan2f4)
VFUNC1(sqrtf4)
VFUNC1(sinf4)
VFUNC1(cosf4)
VFUNC1(tanf4)
VFUNC1(asinf4)
VFUNC1(acosf4)
VFUNC1(fabsf4)
VFUNC1(floorf4)
VFUNC1(expf4)
VFUNC1(logf4)
VFUNC2(powf4)
VFUNC1(roundf4)
VFUNC1(ceilf4)
VFUNC3(ldexpf4)
VFUNC4(frexpf4)


