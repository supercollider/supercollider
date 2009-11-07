//  arithmetic simd functions for sse
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


#ifndef SIMD_MEMORY_SSE_HPP
#define SIMD_MEMORY_SSE_HPP

#include <xmmintrin.h>

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

#define samples_per_loop nova::unroll_constraints<float>::samples_per_loop

namespace nova {

namespace detail {

template <unsigned int n>
always_inline void setvec_simd_mp_iteration(float *dst, __m128 const & val)
{
    _mm_store_ps(dst, val);

    setvec_simd_mp_iteration<n-4>(dst+4, val);
}

template <>
always_inline void setvec_simd_mp_iteration<0>(float *dst, __m128 const & val)
{}

template <unsigned int n>
always_inline void setvec_na_simd_mp_iteration(float *dst, __m128 const & val)
{
    _mm_storeu_ps(dst, val);

    setvec_na_simd_mp_iteration<n-4>(dst+4, val);
}

template <>
always_inline void setvec_na_simd_mp_iteration<0>(float *dst, __m128 const & val)
{}

} /* namespace detail */

template <unsigned int n>
always_inline void zerovec_simd_mp(float *dest)
{
    const __m128 zero = _mm_setzero_ps();
    detail::setvec_simd_mp_iteration<n>(dest, zero);
}

inline void zerovec_simd(float *dest, uint n)
{
    n = n / samples_per_loop;
    const __m128 zero = _mm_setzero_ps();

    do
    {
        detail::setvec_simd_mp_iteration<samples_per_loop>(dest, zero);
        dest += samples_per_loop;
    }
    while (--n);
}

template <unsigned int n>
always_inline void zerovec_simd_mp(double *dest)
{
    const __m128 zero = _mm_setzero_ps();
    detail::setvec_simd_mp_iteration<n*2>((float*)dest, zero);
}

template <unsigned int n>
void zerovec_simd(float *dest)
{
    zerovec_simd_mp<n>(dest);
}

template <unsigned int n>
void zerovec_simd(double *dest)
{
    zerovec_simd_mp<n>(dest);
}


inline void zerovec_simd(double *dest, uint n)
{
    zerovec_simd((float*)dest, n*2);
}

/* memset seems to be more efficient than nonaligned set operations, so don't implement */

template <unsigned int n>
always_inline void setvec_simd_mp(float *dest, float v)
{
    __m128 val =_mm_set_ps1(v);
    detail::setvec_simd_mp_iteration<n>(dest, val);
}

template <unsigned int n>
void setvec_simd(float *dest, float v)
{
    setvec_simd_mp<n>(dest, v);
}

inline void setvec_simd(float *dest, float v, uint n)
{
    n = n / samples_per_loop;

    __m128 val =_mm_set_ps1(v);
    do
    {
        detail::setvec_simd_mp_iteration<samples_per_loop>(dest, val);
        dest += samples_per_loop;
    }
    while(--n);
}


template <>
inline void set_slope_vec_simd(float * dest, float v, float slope, uint n)
{
    n = n / 8;

    __m128 val = _mm_setr_ps(v, v + slope,
                             v + slope + slope,
                             v + slope + slope + slope);
    const __m128 vslope =_mm_set_ps1(slope + slope + slope + slope);

    do
    {
        _mm_store_ps(dest, val);
        val = _mm_add_ps(val, vslope);

        _mm_store_ps(dest+4, val);
        val = _mm_add_ps(val, vslope);

        dest += 8;
    }
    while (--n);
}

template <>
inline void set_exp_vec_simd(float * dest, float f, float curve, uint n)
{
    n = n / 8;

    __m128 val = _mm_setr_ps(f, f * curve, f * curve * curve,
                             f * curve * curve * curve);
    const __m128 vcurve =_mm_set_ps1(curve * curve * curve * curve);

    do
    {
        _mm_store_ps(dest, val);
        val = _mm_mul_ps(val, vcurve);

        _mm_store_ps(dest+4, val);
        val = _mm_mul_ps(val, vcurve);

        dest += 8;
    }
    while (--n);
}


namespace detail
{

template <unsigned int n>
always_inline void copyvec_simd_mp(float *dest, const float *src)
{
    __m128 data = _mm_load_ps(src);
    _mm_store_ps(dest, data);

    copyvec_simd_mp<n-4>(dest+4, src+4);
}

template <>
always_inline void copyvec_simd_mp<0>(float *dest, const float *src)
{}

} /* namespace detail */

using detail::copyvec_simd_mp;

template <unsigned int n>
void copyvec_simd(float *dest, const float *src)
{
    copyvec_simd_mp<n>(dest, src);
}

template <>
inline void copyvec_aa_simd(float * dest, const float * src, uint n)
{
    n = n / samples_per_loop;

    do
    {
        copyvec_simd_mp<samples_per_loop>(dest, src);
        dest += samples_per_loop;
        src += samples_per_loop;
    }
    while(--n);
}


template <>
inline void copyvec_aa_simd(double * dest, const double * src, uint n)
{
    n = n / samples_per_loop * 2;

    do
    {
        copyvec_simd_mp<samples_per_loop>((float*)dest, (float*)src);
        dest += samples_per_loop;
        src += samples_per_loop;
    }
    while(--n);
}


/* double fallback */
template <unsigned int n>
void setvec_simd(double *dest, double v)
{
    setvec(dest, v, n);
}


} /* namespace nova */

#undef samples_per_loop
#undef always_inline

#endif /* SIMD_MEMORY_SSE_HPP */
