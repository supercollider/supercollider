//  templated unary arithmetic simd functions
//  Copyright (C) 2009 Tim Blechmann <tim@klingt.org>
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

#ifndef SIMD_UNARY_ARITHMETIC_SSE_HPP
#define SIMD_UNARY_ARITHMETIC_SSE_HPP

#include <xmmintrin.h>

#include "simd_unroll_constraints.hpp"
#include "simd_utils.hpp"


#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

#define samples_per_loop nova::unroll_constraints<float>::samples_per_loop

namespace nova {

namespace detail
{

always_inline __m128 abs(__m128 arg, __m128 const & abs_mask)
{
    return _mm_and_ps(arg, abs_mask);
}

always_inline __m128 sgn(__m128 arg, __m128 const & sign_mask, __m128 const & zero, __m128 const & one)
{
    const __m128 nonzero = _mm_cmpneq_ps(arg, zero);
    const __m128 sign = _mm_and_ps(arg, sign_mask);

    __m128 abs_ret = _mm_and_ps(nonzero, one);
    return _mm_or_ps(sign, abs_ret);
}

always_inline __m128 square(__m128 arg)
{
    return _mm_mul_ps(arg, arg);
}

always_inline __m128 cube(__m128 arg)
{
    return _mm_mul_ps(_mm_mul_ps(arg, arg),
                      arg);
}

} /* namespace detail */

namespace detail
{
template <int n>
always_inline void abs_vec_simd_mp(float * out, const float * arg, __m128 const & abs_mask)
{
    __m128 in = _mm_load_ps(arg);
    __m128 val = _mm_and_ps(in, abs_mask);
    _mm_store_ps(out, val);
    abs_vec_simd_mp<n-4>(out+4, arg+4, abs_mask);
}

template <>
always_inline void abs_vec_simd_mp<0>(float * out, const float * arg, __m128 const & abs_mask)
{}

} /* namespace detail */

template <>
inline void abs_vec_simd(float * out, const float * arg, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 abs_mask = detail::gen_abs_mask();

    do {
        detail::abs_vec_simd_mp<samples_per_loop>(out, arg, abs_mask);

        out += samples_per_loop;
        arg += samples_per_loop;
    }
    while (--loops);
}

template <unsigned int n>
always_inline void abs_vec_simd_mp(float * out, const float * arg)
{
    __m128 abs_mask = detail::gen_abs_mask();
    detail::abs_vec_simd_mp<n>(out, arg, abs_mask);
}

template <unsigned int n>
inline void abs_vec_simd(float * out, const float * arg)
{
    abs_vec_simd_mp<n>(out, arg);
}


namespace detail
{
template <int n>
always_inline void sgn_vec_simd_mp(float * out, const float * arg, __m128 const & sign_mask,
                              __m128 const & zero, __m128 const & one)
{
    __m128 in = _mm_load_ps(arg);
    __m128 val = sgn(in, sign_mask, zero, one);
    _mm_store_ps(out, val);
    sgn_vec_simd_mp<n-4>(out+4, arg+4, sign_mask, zero, one);
}

template <>
always_inline void sgn_vec_simd_mp<0>(float * out, const float * arg, __m128 const & sign_mask,
                                 __m128 const & zero, __m128 const & one)
{}

} /* namespace detail */

template <>
inline void sgn_vec_simd(float * out, const float * arg, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 sgn_mask = detail::gen_sign_mask();
    __m128 zero = _mm_setzero_ps();
    __m128 one = detail::gen_one();

    do {
        detail::sgn_vec_simd_mp<samples_per_loop>(out, arg, sgn_mask, zero, one);

        out += samples_per_loop;
        arg += samples_per_loop;
    }
    while (--loops);
}

template <unsigned int n>
always_inline void sgn_vec_simd_mp(float * out, const float * arg)
{
    __m128 sgn_mask = detail::gen_sign_mask();
    __m128 zero = _mm_setzero_ps();
    __m128 one = detail::gen_one();

    detail::sgn_vec_simd_mp<n>(out, arg, sgn_mask, zero, one);
}

template <unsigned int n>
void sgn_vec_simd(float * out, const float * arg)
{
    sgn_vec_simd_mp<n>(out, arg);
}

namespace detail
{
template <int n>
always_inline void square_vec_simd_mp(float * out, const float * arg)
{
    __m128 in = _mm_load_ps(arg);
    __m128 val = square(in);
    _mm_store_ps(out, val);
    square_vec_simd_mp<n-4>(out+4, arg+4);
}

template <>
always_inline void square_vec_simd_mp<0>(float * out, const float * arg)
{}

} /* namespace detail */

template <>
inline void square_vec_simd(float * out, const float * arg, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    do {
        detail::square_vec_simd_mp<samples_per_loop>(out, arg);

        out += samples_per_loop;
        arg += samples_per_loop;
    }
    while (--loops);
}

using detail::square_vec_simd_mp;

template <unsigned int n>
inline void square_vec_simd(float * out, const float * arg)
{
    detail::square_vec_simd_mp<n>(out, arg);
}

namespace detail
{
template <int n>
always_inline void cube_vec_simd_mp(float * out, const float * arg)
{
    __m128 in = _mm_load_ps(arg);
    __m128 val = cube(in);
    _mm_store_ps(out, val);
    cube_vec_simd_mp<n-4>(out+4, arg+4);
}

template <>
always_inline void cube_vec_simd_mp<0>(float * out, const float * arg)
{}

} /* namespace detail */

template <>
inline void cube_vec_simd(float * out, const float * arg, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    do {
        detail::cube_vec_simd_mp<samples_per_loop>(out, arg);

        out += samples_per_loop;
        arg += samples_per_loop;
    }
    while (--loops);
}

using detail::cube_vec_simd_mp;

template <unsigned int n>
inline void cube_vec_simd(float * out, const float * arg)
{
    detail::cube_vec_simd_mp<n>(out, arg);
}



} /* namespace nova */

#undef always_inline
#undef samples_per_loop

#endif /* SIMD_UNARY_ARITHMETIC_SSE_HPP */
