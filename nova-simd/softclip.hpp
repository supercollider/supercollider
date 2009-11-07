//  softclip
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

#ifndef SIMD_SOFTCLIP_HPP
#define SIMD_SOFTCLIP_HPP

#include <cassert>
#include <cmath>

#ifdef __SSE__
#include <xmmintrin.h>
#include "simd_utils.hpp"
#endif

namespace nova
{

template <typename float_type>
inline void softclip(float_type * out, const float_type * in, unsigned int n)
{
    for (unsigned int i = 0; i != n; ++i)
    {
        float_type x = in[i];
        float_type abs_x = std::abs(x);
        if (abs_x <= float_type(0.5))
            out[i] = x;
        else
            out[i] = (abs_x - (float_type)0.25) / x;
    }
}

template <typename float_type>
inline void softclip4(float_type * out, const float_type * in, unsigned int n)
{
    softclip(out, in, n);
}

#ifdef __SSE__
/* this computes both parts of the branch
 *
 * benchmarks (core2) showed:
 * 6.5 seconds for simdfied code
 * 5.3 seconds for non-simd code for samples with abs(sample) < 0.5
 * 17 seconds                    when 50% of the samples have abs(sample) < 0.5
 * 26 seconds                    for samples with abs(sample) > 0.5
 *
 * */
template <>
inline void softclip4(float * out, const float * in, unsigned int n)
{
    n = n >> 2;
    const __m128 abs_mask = (__m128)detail::gen_abs_mask();

    const __m128 const05  = detail::gen_05();
    const __m128 const025 = _mm_set1_ps(0.25);

    do
    {
        __m128 x = _mm_load_ps(in);
        in += 4;
        __m128 abs_x = _mm_and_ps(x, abs_mask);
        __m128 selecter = _mm_cmplt_ps(abs_x, const05);
        __m128 alt_ret = _mm_div_ps(_mm_sub_ps(abs_x, const025),
                                    x);
        __m128 result = detail::select_vector(alt_ret, x, selecter);
        _mm_store_ps(out, result);
        out += 4;
    }
    while (--n);
}

#endif /* __SSE__ */


} /* namespace nova */

#endif /* SIMD_MATH_HPP */
