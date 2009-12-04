//  simd functions for mixing, sse implementation
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

#ifndef SIMD_MIX_SSE_HPP
#define SIMD_MIX_SSE_HPP

#include <xmmintrin.h>

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

namespace nova
{

namespace detail
{

template <unsigned int n>
void mix_vec_simd_mp(float * out, const float * in0, const __m128 factor0, const float * in1, const __m128 factor1)
{
    const __m128 sig0 = _mm_load_ps(in0);
    const __m128 sig1 = _mm_load_ps(in1);

    const __m128 mix = _mm_add_ps(_mm_mul_ps(sig0, factor0),
                                  _mm_mul_ps(sig1, factor1));
    _mm_store_ps(out, mix);

    mix_vec_simd_mp<n-4>(out+4, in0+4, factor0, in1+4, factor1);
}

template <>
void mix_vec_simd_mp<0>(float * out, const float * in0, const __m128 factor0, const float * in1, const __m128 factor1)
{}

} /* namespace detail */

template <unsigned int n>
void mix_vec_simd(float * out, const float * in0, float factor0, const float * in1, float factor1)
{
    const __m128 f0 = _mm_set_ps1(factor0);
    const __m128 f1 = _mm_set_ps1(factor1);

    detail::mix_vec_simd_mp<n>(out, in0, f0, in1, f1);
}


template <>
void mix_vec_simd(float * out, const float * in0, float factor0, const float * in1, float factor1, unsigned int n)
{
    const __m128 f0 = _mm_set_ps1(factor0);
    const __m128 f1 = _mm_set_ps1(factor1);

    n /= 8;
    do {
        detail::mix_vec_simd_mp<8>(out, in0, f0, in1, f1);

        out += 8;
        in0 += 8;
        in1 += 8;
    } while(--n);
}

template <>
void mix_vec_simd(float * out, const float * in0, float factor0, float slope0,
                  const float * in1, float factor1, float slope1, unsigned int n)
{
    __m128 f0 = _mm_setr_ps(factor0, factor0 + slope0,
                            factor0 + 2*slope0, factor0 + 3*slope0);
    __m128 f1 = _mm_setr_ps(factor1, factor1 + slope1,
                            factor1 + 2*slope1, factor1 + 3*slope1);

    const __m128 vslope0 = _mm_set_ps1(4*slope0);
    const __m128 vslope1 = _mm_set_ps1(4*slope1);

    n /= 8;
    do {
        __m128 sig00 = _mm_load_ps(in0);
        __m128 sig01 = _mm_load_ps(in1);
        __m128 sig10 = _mm_load_ps(in0+4);
        __m128 sig11 = _mm_load_ps(in1+4);
        __m128 mix0  = _mm_add_ps(_mm_mul_ps(sig00, f0),
                                  _mm_mul_ps(sig01, f1));
        _mm_store_ps(out, mix0);

        f0 = _mm_add_ps(f0, vslope0);
        f1 = _mm_add_ps(f1, vslope1);

        __m128 mix1  = _mm_add_ps(_mm_mul_ps(sig10, f0),
                                  _mm_mul_ps(sig11, f1));
        _mm_store_ps(out+4, mix1);

        f0 = _mm_add_ps(f0, vslope0);
        f1 = _mm_add_ps(f1, vslope1);

        out += 8;
        in0 += 8;
        in1 += 8;
    } while(--n);
}

template <unsigned int n>
void mix_vec_simd(float * out, const float * in0, float factor0, float slope0,
                  const float * in1, float factor1, float slope1)
{
    mix_vec_simd(out, in0, factor0, slope0, in1, factor1, slope1, n);
}


} /* namespace nova */

#undef always_inline

#endif /* SIMD_MIX_SSE_HPP */
