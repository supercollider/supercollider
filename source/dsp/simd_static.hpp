//  non-inline wrapper functions for compile-time loop unrolled funtions
//  Copyright (C) 2006, 2007, 2008 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.

#ifndef SIMD_STATIC_HPP
#define SIMD_STATIC_HPP

#ifdef __SSE__
#include "simd_sse.hpp"
#include "simd_sse_mp.hpp"
#else /* __SSE__ */
#include "simd_fallbacks.hpp"
#endif /* __SSE__ */


namespace nova
{

template <int n>
void addvec_simd(float *dst, const float *src)
{
    addvec_simd_mp<n>(dst, src);
}
template <int n>
void addvec_simd(float *dst, const float *src1, const float *src2)
{
    addvec_simd_mp<n>(dst, src1, src2);
}

template <int n>
void addvec_simd(float *dst, const float *src1, const float *src2, const float *src3)
{
    addvec_simd_mp<n>(dst, src1, src2, src3);
}

template <int n>
void testcopyvec_simd(float *dst, const float *src)
{
    testcopyvec_simd_mp<n>(dst, src);
}

template <int n>
void testaddvec_simd(float *dst, const float *src)
{
    testaddvec_simd_mp<n>(dst, src);
}

template <int n>
inline void plus_vec_simd(float * dest, const float * src1, const float * src2, const float * src3, const float * src4)
{
    plus_vec_simd_mp<n>(dest, src1, src2, src3, src4);
}


/***********************************************************************
 * basic arithmetic functions
 ***********************************************************************/


#define UNARY_SIMD_WRAPPER(PREFIX)                          \
    template <int n>                                        \
    void PREFIX##_vec_simd(float * dest, const float * src) \
    {                                                       \
        PREFIX##_vec_simd_mp<n>(dest, src);                 \
    }

UNARY_SIMD_WRAPPER(square);
UNARY_SIMD_WRAPPER(cube);
UNARY_SIMD_WRAPPER(sqrt);
UNARY_SIMD_WRAPPER(rsqrt);
UNARY_SIMD_WRAPPER(rcp);
UNARY_SIMD_WRAPPER(abs);
UNARY_SIMD_WRAPPER(sgn);

template <int n>
void clip_vec_simd(float * dest, const float * src, const float * low, const float * high)
{
    clip_vec_simd_mp<n>(dest, src, low, high);
}

template <int n>
void clip_vec_simd(float * dest, const float * src, float low, const float * high)
{
    clip_vec_simd_mp<n>(dest, src, low, high);
}

template <int n>
void clip_vec_simd(float * dest, const float * src, const float * low, float high)
{
    clip_vec_simd_mp<n>(dest, src, low, high);
}

template <int n>
void clip_vec_simd(float * dest, const float * src, float low, float high)
{
    clip_vec_simd_mp<n>(dest, src, low, high);
}



/***********************************************************************
 * muladd operations
 ***********************************************************************/

template <int n>
void muladd_vec_simd(float * dest, const float * src, const float mul, const float add)
{
    muladd_vec_simd_mp<n>(dest, src, mul, add);
}

template <int n>
void muladd_vec_simd(float * dest, const float * src, const float * mul, const float add)
{
    muladd_vec_simd_mp<n>(dest, src, mul, add);
}

template <int n>
void muladd_vec_simd(float * dest, const float * src, const float mul, const float * add)
{
    muladd_vec_simd_mp<n>(dest, src, mul, add);
}

template <int n>
void muladd_vec_simd(float * dest, const float * src, const float * mul, const float * add)
{
    muladd_vec_simd_mp<n>(dest, src, mul, add);
}

/***********************************************************************
 * mixing operations
 ***********************************************************************/

template <int n>
void mix_vec_simd(float * dest, const float * src0, const float mul0,
                  const float * src1, const float mul1)
{
    mix_vec_simd_mp<n>(dest, src0, mul0, src1, mul1);
}


} /* namespace nova */

#undef UNARY_SIMD_WRAPPER
#endif /* SIMD_STATIC_HPP */
