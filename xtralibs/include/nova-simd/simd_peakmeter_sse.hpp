//  simd functions for peak metering, sse implementation
//  Copyright (C) 2009 Tim Blechmann
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

#ifndef SIMD_PEAKMETER_SSE_HPP
#define SIMD_PEAKMETER_SSE_HPP

#include <xmmintrin.h>

#include "simd_utils.hpp"

namespace nova
{

template <>
inline float peak_vec_simd(const float * in, float * peak, unsigned int n)
{
    __m128 maximum = _mm_load_ss(peak); /* [*peak, 0, 0, 0] */
    const __m128 absmask = detail::gen_abs_mask();

    /* loop */
    __m128 abs3;
    n /= 16;
    do {
        const __m128 in0 = _mm_load_ps(in);
        const __m128 in1 = _mm_load_ps(in+4);
        const __m128 in2 = _mm_load_ps(in+8);
        const __m128 in3 = _mm_load_ps(in+12);

        const __m128 abs0 = _mm_and_ps(in0, absmask);
        const __m128 abs1 = _mm_and_ps(in1, absmask);
        const __m128 abs2 = _mm_and_ps(in2, absmask);
        abs3 = _mm_and_ps(in3, absmask);

        const __m128 max = _mm_max_ps(_mm_max_ps(abs0, abs1),
                                      _mm_max_ps(abs2, abs3));

        maximum = _mm_max_ps(maximum, max);
        in += 16;
    } while(--n);

    /* horizonal accumulation */
    *peak = detail::horizontal_max(maximum);

    /* return absolute of last sample */
    return detail::extract_3(abs3);
}

} /* namespace nova */


#endif /* SIMD_PEAKMETER_SSE_HPP */
