//  utilities for the simd implementation
//  Copyright (C) 2008, 2009 Tim Blechmann
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

#ifndef SIMD_UTILS_HPP
#define SIMD_UTILS_HPP

#include <xmmintrin.h>

#ifdef __SSE2__
#include <emmintrin.h>
#endif /* __SSE2__ */

namespace nova
{
namespace detail
{

#ifdef __SSE2__
inline __m128 gen_sign_mask(void)
{
    __m128i x = _mm_setzero_si128();
    __m128i ones = _mm_cmpeq_epi32(x, x);
    return (__m128)_mm_slli_epi32 (_mm_srli_epi32(ones, 31), 31);
}

inline __m128 gen_abs_mask(void)
{
    __m128i x = _mm_setzero_si128();
    __m128i ones = _mm_cmpeq_epi32(x, x);
    return (__m128)_mm_srli_epi32 (_mm_slli_epi32(ones, 1), 1);
}

inline __m128 gen_one(void)
{
    __m128i x = _mm_setzero_si128();
    __m128i ones = _mm_cmpeq_epi32(x, x);
    return (__m128)_mm_slli_epi32 (_mm_srli_epi32(ones, 25), 23);
}

inline __m128 gen_05(void)
{
    __m128i x = _mm_setzero_si128();
    __m128i ones = _mm_cmpeq_epi32(x, x);
    return (__m128)_mm_slli_epi32 (_mm_srli_epi32(ones, 26), 24);
}

inline __m128 gen_025(void)
{
    __m128i x = _mm_setzero_si128();
    __m128i ones = _mm_cmpeq_epi32(x, x);
    return (__m128)_mm_slli_epi32 (_mm_srli_epi32(ones, 27), 25);
}

#else

/* SSE fallback */

inline __m128 gen_sign_mask(void)
{
    static const int sign_mask = 0x80000000;
    float * casted = (float*)(&sign_mask);
    return _mm_set_ps1(*casted);
}

inline __m128 gen_abs_mask(void)
{
    static const int abs_mask = 0x7fffffff;
    float * casted = (float*)(&abs_mask);
    return _mm_set_ps1(*casted);
}

inline __m128 gen_one(void)
{
    return _mm_set_ps1(1.f);
}

inline __m128 gen_05(void)
{
    return _mm_set_ps1(0.5f);
}

inline __m128 gen_025(void)
{
    return _mm_set_ps1(0.25f);
}
#endif


inline __m128 select_vector(__m128 val0, __m128 val1, __m128 sel)
{
    /* if bitmask is set, return value in val1, else value in val0 */
    return _mm_or_ps(_mm_andnot_ps(sel, val0),
                     _mm_and_ps(val1, sel));
}


} /* namespace detail */
} /* namespace nova */

#endif /* SIMD_UTILS_HPP */
