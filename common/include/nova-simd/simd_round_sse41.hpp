//  simdfied round functions, sse4.1 implementation
//  Copyright (C) 2008, 2009 Tim Blechmann <tim@klingt.org>
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
//
//  implemented as part of nova

#ifndef SIMD_ROUND_SSE41_HPP
#define SIMD_ROUND_SSE41_HPP

#include <smmintrin.h>

namespace nova {

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

#define samples_per_loop nova::unroll_constraints<float>::samples_per_loop


namespace detail
{

always_inline __m128 round(__m128 v)
{
    return _mm_round_ps(v, _MM_FROUND_TO_NEAREST_INT);
}

always_inline __m128 frac(__m128 v)
{
    return _mm_sub_ps(v,
                      _mm_round_ps(v, _MM_FROUND_TO_NEAREST_INT));
}

} /* namespace detail */


namespace detail {

template <int n>
always_inline void round_vec_simd_mp(float * out, const float * src)
{
    const __m128 in = _mm_load_ps(src);
    const __m128 result = detail::round(in);
    _mm_store_ps(out, result);

    round_vec_simd_mp<n-4>(out+4, src+4);
}

template <>
always_inline void round_vec_simd_mp<0>(float * out, const float * src)
{}

} /* namespace detail */

using detail::round_vec_simd_mp;

template <int n>
void round_vec_simd(float * out, const float * src)
{
    round_vec_simd_mp<n>(out, src);
}

template <>
inline void round_vec_simd(float * out, const float * arg, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    do {
        detail::round_vec_simd_mp<samples_per_loop>(out, arg);
        out += samples_per_loop;
        arg += samples_per_loop;
    }
    while (--loops);
}

namespace detail {

template <int n>
always_inline void ceil_vec_simd_mp(float * out, const float * src)
{
    const __m128 in = _mm_load_ps(src);
    const __m128 result = _mm_ceil_ps(in);
    _mm_store_ps(out, result);

    ceil_vec_simd_mp<n-4>(out+4, src+4);
}

template <>
always_inline void ceil_vec_simd_mp<0>(float * out, const float * src)
{}

} /* namespace detail */

using detail::ceil_vec_simd_mp;

template <int n>
void ceil_vec_simd(float * out, const float * src)
{
    ceil_vec_simd_mp<n>(out, src);
}

template <>
inline void ceil_vec_simd(float * out, const float * arg, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    do {
        detail::ceil_vec_simd_mp<samples_per_loop>(out, arg);
        out += samples_per_loop;
        arg += samples_per_loop;
    }
    while (--loops);
}

namespace detail {

template <int n>
always_inline void floor_vec_simd_mp(float * out, const float * src)
{
    const __m128 in = _mm_load_ps(src);
    const __m128 result = _mm_floor_ps(in);
    _mm_store_ps(out, result);

    floor_vec_simd_mp<n-4>(out+4, src+4);
}

template <>
always_inline void floor_vec_simd_mp<0>(float * out, const float * src)
{}

} /* namespace detail */

using detail::floor_vec_simd_mp;

template <int n>
void floor_vec_simd(float * out, const float * src)
{
    floor_vec_simd_mp<n>(out, src);
}

template <>
inline void floor_vec_simd(float * out, const float * arg, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    do {
        detail::floor_vec_simd_mp<samples_per_loop>(out, arg);
        out += samples_per_loop;
        arg += samples_per_loop;
    }
    while (--loops);
}

namespace detail {

template <int n>
always_inline void frac_vec_simd_mp(float * out, const float * src)
{
    const __m128 in = _mm_load_ps(src);
    const __m128 result = detail::frac(in);
    _mm_store_ps(out, result);

    frac_vec_simd_mp<n-4>(out+4, src+4);
}

template <>
always_inline void frac_vec_simd_mp<0>(float * out, const float * src)
{}

} /* namespace detail */

using detail::frac_vec_simd_mp;

template <int n>
void frac_vec_simd(float * out, const float * src)
{
    frac_vec_simd_mp<n>(out, src);
}

template <>
inline void frac_vec_simd(float * out, const float * arg, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    do {
        detail::frac_vec_simd_mp<samples_per_loop>(out, arg);
        out += samples_per_loop;
        arg += samples_per_loop;
    }
    while (--loops);
}

#undef always_inline
#undef samples_per_loop

} /* namespace nova */

#endif /* SIMD_ROUND_SSE41_HPP */
