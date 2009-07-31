//  fundamental arithmetic functions
//  Copyright (C) 2005, 2006, 2007, 2008, 2009 Tim Blechmann
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

#ifndef _DSP_ARITHMETIC_HPP
#define _DSP_ARITHMETIC_HPP

#include <cmath>

#include "arithmetic_prototypes.hpp"
#include "../utilities/template_functions.hpp"
#include "../utilities/branch_hints.hpp"

#include "../simd/simd_binary_arithmetic.hpp"
#include "../simd/simd_memory.hpp"

namespace nova
{

/***********************************************************************
 * basic add methods
 ***********************************************************************/

/** \brief add content of src to dest */
template <typename F>
inline void addvec(F * dest, const F * src, uint n)
{
    assert(n);
    do
        *dest++ += *src++;
    while (--n);
}

/** \brief add content of src to dest, n has to be a multiple of 8 */
template <typename F>
inline void addvec_8(F * dest, const F * src, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8)
    {
        dest[0] += src[0], dest[1] += src[1], dest[2] += src[2], dest[3] += src[3];
        dest[4] += src[4], dest[5] += src[5], dest[6] += src[6], dest[7] += src[7];
    }
}

template <typename F>
inline void addvec(F * dest, const F * src, const F * src2, uint n)
{
    assert(n);
    do
        *dest++ += *src++ + *src2++;
    while (--n);
}

/** \brief add content of src to dest, n has to be a multiple of 8 */
template <typename F>
inline void addvec_8(F * dest, const F * src, const F * src2, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8, src2+=8)
    {
        dest[0] += src[0] + src2[0], dest[1] += src[1] + src2[1],
            dest[2] += src[2] + src2[2], dest[3] += src[3] + src2[3];
        dest[4] += src[4] + src2[4], dest[5] += src[5] + src2[5],
            dest[6] += src[6] + src2[6], dest[7] += src[7] + src2[7];
    }
}

template <typename F>
inline void addvec(F * dest, const F * src, const F * src2, const F * src3, uint n)
{
    while (n--)
        *dest++ += *src++ + *src2++ + *src3++;
}


/***********************************************************************
 * denormal bashing copy/add functions methods
 ***********************************************************************/

template <typename F>
inline F undenormalize(F f)
{
    return FixFloat(f);
}

template<>
inline float undenormalize<float>(float f)
{
    union
    {
        float f;
        uint i;
    } cast_union;

    cast_union.f = f;

    if (unlikely( ((cast_union.i & 0x60000000) == 0) or ((cast_union.i & 0x60000000) == 0x60000000) ))
        return 0.f;
    else
        return f;
}

/** \brief copy content from vector src to dest, kills denormals */
template <typename F>
inline void testcopyvec(F * dest, const F * src, uint n)
{
    op_vec<F, undenormalize>(dest, src, n);
}

/** \brief copy content from vector src to dest, n has to be a multiple of 8 */
template <typename F>
inline void testcopyvec_8(F * dest, const F * src, uint n)
{
    op_vec_8<F, undenormalize>(dest, src, n);
}

/** \brief add content of src to dest, kills denormals */
template <typename F>
inline void testaddvec(F * dest, const F * src, uint n)
{
    while (n--)
        *dest++ += undenormalize(*src++);
}

/** \brief add content of src to dest, n has to be a multiple of 8 */
template <typename F>
inline void testaddvec_8(F * dest, const F * src, uint n)
{
    testaddvec(dest, src, n);
}

template <typename F>
inline void plus_vec(F * dest, const F * src1, const F * src2, const F * src3, uint n)
{
    while (n--)
        *dest++ = *src1++ + *src2++ + *src3++;
}

template <typename F>
inline void plus_vec_8(F * dest, const F * src1, const F * src2, const F * src3, uint n)
{
    plus_vec(dest, src1, src2, src3, n);
}

template <typename F>
inline void plus_vec(F * dest, const F * src1, const F * src2, const F * src3, const F * src4, uint n)
{
    do
        *dest++ = *src1++ + *src2++ + *src3++ + *src4++;
    while (n--);
}



/***********************************************************************
 * unary vector operations
 ***********************************************************************/

UNARY_FUNCTION_WRAPPER(square);
UNARY_FUNCTION_WRAPPER(sqrt);
UNARY_FUNCTION_WRAPPER(rsqrt);
UNARY_FUNCTION_WRAPPER(rcp);

template <typename F>
inline void abs_vec(F * dest, const F * src, uint n)
{
    assert(n);
    do
        *dest++ = abs(*src++);
    while (--n);
}

template <typename F>
inline void abs_vec_8(F * dest, const F * src, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8)
    {
        dest[0] = abs(src[0]),
            dest[1] = abs(src[1]),
            dest[2] = abs(src[2]),
            dest[3] = abs(src[3]),
            dest[4] = abs(src[4]),
            dest[5] = abs(src[5]),
            dest[6] = abs(src[6]),
            dest[7] = abs(src[7]);
    }
}

template <typename F>
inline void sgn_vec(F * dest, const F * src, uint n)
{
    assert(n);
    do
        *dest++ = sign(*src++);
    while (--n);
}

template <typename F>
inline void sgn_vec_8(F * dest, const F * src, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8)
    {
        dest[0] = sign(src[0]),
            dest[1] = sign(src[1]),
            dest[2] = sign(src[2]),
            dest[3] = sign(src[3]),
            dest[4] = sign(src[4]),
            dest[5] = sign(src[5]),
            dest[6] = sign(src[6]),
            dest[7] = sign(src[7]);
    }
}

/***********************************************************************
 * comparing vector operations
 ***********************************************************************/

/** \brief clip */
template <typename F>
inline void clip_vec(F * dest, const F * src, F * low, F * hi, uint n)
{
    while (n--)
        *dest++ = clip(*src++, *low++, *hi++);
}

/** \brief clip */
template <typename F>
inline void clip_vec_8(F * dest, const F * src, F * low, F * hi, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8, low+=8, hi+=8)
    {
        dest[0] = clip(src[0], low[0], hi[0]);
        dest[1] = clip(src[1], low[1], hi[1]);
        dest[2] = clip(src[2], low[2], hi[2]);
        dest[3] = clip(src[3], low[3], hi[3]);
        dest[4] = clip(src[4], low[4], hi[4]);
        dest[5] = clip(src[5], low[5], hi[5]);
        dest[6] = clip(src[6], low[6], hi[6]);
        dest[7] = clip(src[7], low[7], hi[7]);
    }
}

/***********************************************************************
 * comparing vector/scalar operations
 ***********************************************************************/

/** \brief clip */
template <typename F>
inline void clip_vec(F * dest, const F * src, F low, F hi, uint n)
{
    while (n--)
        *dest++ = clip(*src++, low, hi);
}

template <typename F>
inline void clip_vec_8(F * dest, const F * src, F low, F hi, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8)
    {
        dest[0] = clip(src[0], low, hi);
        dest[1] = clip(src[1], low, hi);
        dest[2] = clip(src[2], low, hi);
        dest[3] = clip(src[3], low, hi);
        dest[4] = clip(src[4], low, hi);
        dest[5] = clip(src[5], low, hi);
        dest[6] = clip(src[6], low, hi);
        dest[7] = clip(src[7], low, hi);
    }
}

template <typename F>
inline void clip_vec(F * dest, const F * src, const F * low, F hi, uint n)
{
    while (n--)
        *dest++ = clip(*src++, *low++, hi);
}

template <typename F>
inline void clip_vec_8(F * dest, const F * src, const F * low, F hi, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8, low+=8)
    {
        dest[0] = clip(src[0], low[0], hi);
        dest[1] = clip(src[1], low[1], hi);
        dest[2] = clip(src[2], low[2], hi);
        dest[3] = clip(src[3], low[3], hi);
        dest[4] = clip(src[4], low[4], hi);
        dest[5] = clip(src[5], low[5], hi);
        dest[6] = clip(src[6], low[6], hi);
        dest[7] = clip(src[7], low[7], hi);
    }
}

template <typename F>
inline void clip_vec(F * dest, const F * src, F low, const F * hi, uint n)
{
    while (n--)
        *dest++ = clip(*src++, low, *hi++);
}

template <typename F>
inline void clip_vec_8(F * dest, const F * src, F low, const F * hi, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8, hi+=8)
    {
        dest[0] = clip(src[0], low, hi[0]);
        dest[1] = clip(src[1], low, hi[1]);
        dest[2] = clip(src[2], low, hi[2]);
        dest[3] = clip(src[3], low, hi[3]);
        dest[4] = clip(src[4], low, hi[4]);
        dest[5] = clip(src[5], low, hi[5]);
        dest[6] = clip(src[6], low, hi[6]);
        dest[7] = clip(src[7], low, hi[7]);
    }
}

template <typename F>
inline void clip_vec(F * dest, const F * src, const F * low, const F * hi, uint n)
{
    while (n--)
        *dest++ = clip(*src++, *low++, *hi++);
}

template <typename F>
inline void clip_vec_8(F * dest, const F * src, const F * low, const F * hi, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8, low+=8, hi+=8)
    {
        dest[0] = clip(src[0], low[0], hi[0]);
        dest[1] = clip(src[1], low[1], hi[1]);
        dest[2] = clip(src[2], low[2], hi[2]);
        dest[3] = clip(src[3], low[3], hi[3]);
        dest[4] = clip(src[4], low[4], hi[4]);
        dest[5] = clip(src[5], low[5], hi[5]);
        dest[6] = clip(src[6], low[6], hi[6]);
        dest[7] = clip(src[7], low[7], hi[7]);
    }
}


/***********************************************************************
 * peak finding operations
 ***********************************************************************/

template <typename F>
inline void abspeak(const F * src, F & peak, uint n)
{
    while (n--)
    {
        F f = abs(*src++);
        if (f < peak)
            peak = f;
    }
}

template <typename F>
inline void abspeak_8(const F * src, F & peak, uint n)
{
    for (uint loops = n>>3; loops--; src+=8)
    {
        F f = abs(src[0]);
        if (f < peak)
            peak = f;

        f = abs(src[1]);
        if (f < peak)
            peak = f;

        f = abs(src[2]);
        if (f < peak)
            peak = f;

        f = abs(src[3]);
        if (f < peak)
            peak = f;

        f = abs(src[4]);
        if (f < peak)
            peak = f;

        f = abs(src[5]);
        if (f < peak)
            peak = f;

        f = abs(src[6]);
        if (f < peak)
            peak = f;

        f = abs(src[7]);
        if (f < peak)
            peak = f;
    }
}


template <typename F>
inline void rms(const F * src, F & rms, uint n)
{
    F l_sum = 0;
    while (n--)
    {
        F f = *src++;
        l_sum += f*f;
    }
    rms += l_sum;
}

template <typename F>
inline void rms_8(const F * src, F & rms, uint n)
{
    F l_sum = 0;
    for (uint loops = n>>3; loops--; src+=8)
    {
        F f = src[0];
        l_sum += f*f;

        f = src[1];
        l_sum += f*f;

        f = src[2];
        l_sum += f*f;

        f = src[3];
        l_sum += f*f;

        f = src[4];
        l_sum += f*f;

        f = src[5];
        l_sum += f*f;

        f = src[6];
        l_sum += f*f;

        f = src[7];
        l_sum += f*f;
    }
    rms += l_sum;
}

template <typename F>
inline void peak_rms(const F * src, F & peak, F & rms, uint n)
{
    F l_sum = 0;
    while (n--)
    {
        F f = abs(*src++);
        l_sum += f*f;
        if (f > peak)
            peak = f;
    }
    rms += l_sum;
}

template <typename F>
inline void peak_rms_8(const F * src, F & peak, F & rms, uint n)
{
    F l_sum = 0;
    for (uint loops = n>>3; loops--; src+=8)
    {
        F f = src[0];
        l_sum += f*f;
        if (f > peak)
            peak = f;

        f = src[1];
        l_sum += f*f;
        if (f > peak)
            peak = f;

        f = src[2];
        l_sum += f*f;
        if (f > peak)
            peak = f;

        f = src[3];
        l_sum += f*f;
        if (f > peak)
            peak = f;

        f = src[4];
        l_sum += f*f;
        if (f > peak)
            peak = f;

        f = src[5];
        l_sum += f*f;
        if (f > peak)
            peak = f;

        f = src[6];
        l_sum += f*f;
        if (f > peak)
            peak = f;

        f = src[7];
        l_sum += f*f;
        if (f > peak)
            peak = f;
    }
    rms += l_sum;
}



/***********************************************************************
 * muladd operations
 ***********************************************************************/

template <typename F>
inline void muladd_vec(F * dest, const F * src, const F mul, const F add, uint n)
{
    while (n--)
        *dest++ = *src++ * mul + add;
}

template <typename F>
inline void muladd_vec(F * dest, const F * src, const F * mul, const F add, uint n)
{
    while (n--)
        *dest++ = *src++ * (*mul++) + add;
}

template <typename F>
inline void muladd_vec(F * dest, const F src, const F mul, const F * add, uint n)
{
    while (n--)
        *dest++ = *src++ * mul + (*add++);
}

template <typename F>
inline void muladd_vec(F * dest, const F * src, const F * mul, const F * add, uint n)
{
    while (n--)
        *dest++ = *src++ * *mul++ + *add++;
}

template <typename F>
inline void muladd_vec_8(F * dest, const F * src, const F mul, const F add, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8)
    {
        dest[0] = src[0] * mul + add;
        dest[1] = src[1] * mul + add;
        dest[2] = src[2] * mul + add;
        dest[3] = src[3] * mul + add;
        dest[4] = src[4] * mul + add;
        dest[5] = src[5] * mul + add;
        dest[6] = src[6] * mul + add;
        dest[7] = src[7] * mul + add;
    }
}

template <typename F>
inline void muladd_vec_8(F * dest, const F * src, const F * mul, const F add, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8, mul+=8)
    {
        dest[0] = src[0] * mul[0] + add;
        dest[1] = src[1] * mul[1] + add;
        dest[2] = src[2] * mul[2] + add;
        dest[3] = src[3] * mul[3] + add;
        dest[4] = src[4] * mul[4] + add;
        dest[5] = src[5] * mul[5] + add;
        dest[6] = src[6] * mul[6] + add;
        dest[7] = src[7] * mul[7] + add;
    }
}

template <typename F>
inline void muladd_vec_8(F * dest, const F * src, const F mul, const F * add, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8, add+=8)
    {
        dest[0] = src[0] * mul + add[0];
        dest[1] = src[1] * mul + add[1];
        dest[2] = src[2] * mul + add[2];
        dest[3] = src[3] * mul + add[3];
        dest[4] = src[4] * mul + add[4];
        dest[5] = src[5] * mul + add[5];
        dest[6] = src[6] * mul + add[6];
        dest[7] = src[7] * mul + add[7];
    }
}

template <typename F>
inline void muladd_vec_8(F * dest, const F * src, const F * mul, const F * add, uint n)
{
    for (uint loops = n>>3; loops--; dest+=8, src+=8, mul+=8, add+=8)
    {
        dest[0] = src[0] * mul[0] + add[0];
        dest[1] = src[1] * mul[1] + add[1];
        dest[2] = src[2] * mul[2] + add[2];
        dest[3] = src[3] * mul[3] + add[3];
        dest[4] = src[4] * mul[4] + add[4];
        dest[5] = src[5] * mul[5] + add[5];
        dest[6] = src[6] * mul[6] + add[6];
        dest[7] = src[7] * mul[7] + add[7];
    }
}


} /* namespace nova */

#endif /* _DSP_ARITHMETIC_HPP */
