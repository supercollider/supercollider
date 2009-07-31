//  fallback functions for non-sse code
//
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

#ifndef SIMD_FALLBACKS_HPP
#define SIMD_FALLBACKS_HPP

#include "arithmetic.hpp"

namespace nova
{

/** \brief same as copyvec_8 */
template <int n>
inline void copyvec_simd_mp(float *dest, const float *src)
{
    copyvec(dest, src, n);
}

inline void copyvec_simd(float *dest, const float *src, uint n)
{
    copyvec(dest, src, n);
}

template <int n>
inline void copyvec_alignedUnaligned_simd_mp(float *dest, const float *src)
{
    copyvec(dest, src, n);
}

inline void copyvec_alignedUnaligned_simd(float *dest, const float *src, uint n)
{
    copyvec(dest, src, n);
}

template <int n>
inline void copyvec_unalignedAligned_simd_mp(float *dest, const float *src)
{
    copyvec(dest, src, n);
}

inline void copyvec_unalignedAligned_simd(float *dest, const float *src, uint n)
{
    copyvec(dest, src, n);
}

template <int n>
inline void copyvec_unaligned_simd_mp(float *dest, const float *src)
{
    copyvec(dest, src, n);
}

inline void copyvec_unaligned_simd(float *dest, const float *src, uint n)
{
    copyvec(dest, src, n);
}

template <int n>
inline void addvec_simd_mp(float *dest, const float *src)
{
    addvec(dest, src, n);
}

inline void addvec_simd(float *dest, const float *src, uint n)
{
    addvec(dest, src, n);
}

template <int n>
inline void addvec_simd_mp(float *dest, const float *src1, const float *src2)
{
    addvec(dest, src1, src2, n);
}

inline void addvec_simd(float *dest, const float *src1, const float *src2, uint n)
{
    addvec(dest, src1, src2, n);
}

template <int n>
inline void addvec_simd_mp(float *dest, const float *src1, const float *src2, const float *src3)
{
    addvec(dest, src1, src2, src3, n);
}

inline void addvec_simd(float *dest, const float *src1, const float *src2, const float *src3, uint n)
{
    addvec(dest, src1, src2, src3, n);
}

/***********************************************************************
 * denormal bashing copy/add functions methods
 ***********************************************************************/

template <int n>
void testcopyvec_simd_mp(float *dest, const float *src)
{
    testcopyvec(dest, src, n);
}

inline void testcopyvec_simd(float *dest, const float *src, uint n)
{
    testcopyvec(dest, src, n);
}

template <int n>
inline void testaddvec_simd_mp(float *dest, const float *src)
{
    testaddvec(dest, src, n);
}

inline void testaddvec_simd(float *dest, const float *src, uint n)
{
    testaddvec(dest, src, n);
}

/***********************************************************************
 * basic arithmetic functions (vector/vector)
 ***********************************************************************/
#define TERNARY_ARITHMETIC_SIMD_WRAPPER(PREFIX)                         \
    template <int n, typename F, typename G, typename H>                \
    inline void PREFIX##_vec_simd_mp(float * dest, F src1, G src2, H src3) \
    {                                                                   \
        PREFIX##_vec_8(dest, src1, src2, src3, n);                      \
    }                                                                   \
                                                                        \
    template <typename F, typename G, typename H>                       \
    inline void PREFIX##_vec_simd(float * dest, F src1, G src2, H src3, uint n) \
    {                                                                   \
        PREFIX##_vec_8(dest, src1, src2, src3, n);                      \
    }

TERNARY_ARITHMETIC_SIMD_WRAPPER(plus);

template <int n>
inline void plus_vec_simd_mp(float * dest, const float * src1, const float * src2, const float * src3, const float * src4)
{
    plus_vec(dest, src1, src2, src3, src4, n);
}

inline void plus_vec_simd(float * dest, const float * src1, const float * src2, const float * src3, const float * src4, uint n)
{
    plus_vec(dest, src1, src2, src3, src4, n);
}


/***********************************************************************
 * unary vector operations
 ***********************************************************************/

#define UNARY_SIMD_WRAPPER(PREFIX)                              \
    template <int n, typename F>                                \
    inline void PREFIX##_vec_simd_mp(float * dest, F src)       \
    {                                                           \
        PREFIX##_vec_8(dest, src, n);                           \
    }                                                           \
                                                                \
    template <typename F>                                       \
    inline void PREFIX##_vec_simd(float * dest, F src, uint n)  \
    {                                                           \
        PREFIX##_vec_8(dest, src, n);                           \
    }


UNARY_SIMD_WRAPPER(square);
UNARY_SIMD_WRAPPER(cube);
UNARY_SIMD_WRAPPER(sqrt);
UNARY_SIMD_WRAPPER(rsqrt);
UNARY_SIMD_WRAPPER(rcp);

UNARY_SIMD_WRAPPER(abs);
UNARY_SIMD_WRAPPER(sgn);


/***********************************************************************
 * clipping operations
 ***********************************************************************/

TERNARY_ARITHMETIC_SIMD_WRAPPER(clip);


/***********************************************************************
 * peak finding operations
 ***********************************************************************/
template <int n>
inline void abspeak_simd_mp(const float * src, float & peak)
{
    abspeak(src, peak, n);
}

inline void abspeak_simd(const float * src, float & peak, uint n)
{
    abspeak(src, peak, n);
}

template <int n>
inline void rms_simd_mp(const float * src, float & rms_value)
{
    rms(src, rms_value, n);
}

inline void rms_simd(const float * src, float & rms_value, uint n)
{
    rms(src, rms_value, n);
}

template <int n>
inline void peak_rms_simd_mp(const float * src, float & peak, float & rms)
{
    peak_rms(src, peak, rms, n);
}

inline void peak_rms_simd(const float * src, float & peak, float & rms, uint n)
{
    peak_rms(src, peak, rms, n);
}

/***********************************************************************
 * ramp
 ***********************************************************************/
template <int n>
inline void ramp_simd_mp(float * dest, const float * last, float * increment)
{
    assert(false);
}

template <int n>
inline void vol_ramp_simd_mp(float * dest, const float * src, const float * last, float * increment)
{
    assert(false);
}

inline float ramp_simd(float * dst, float current, float * m_slopes, float m_slope_step, uint n)
{
    assert(n % 8 == 0);

    float slope = m_slopes[1];
    for (uint i = 0; i != n; ++i)
    {
        dst[i] = current;
        current += slope;
    }

    return current;
}


/***********************************************************************
 * muladd operations
 ***********************************************************************/

TERNARY_ARITHMETIC_SIMD_WRAPPER(muladd);

/***********************************************************************
 * mixing operations
 ***********************************************************************/

template <int n>
inline void mix_vec_simd_mp(float * dest, const float * src0, const float mul0,
                            const float * src1, const float mul1)
{
    for (uint i = 0; i != n; ++i)
        dest[i] = src0[i] * mul0 +
            src1[i] * mul1;
}


} /* namespace nova */

#undef TERNARY_ARITHMETIC_SIMD_WRAPPER
#undef UNARY_SIMD_WRAPPER

#endif /* SIMD_FALLBACKS_HPP */
