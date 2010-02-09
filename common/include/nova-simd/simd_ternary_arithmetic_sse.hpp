//  templated arithmetic simd functions
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

#ifndef SIMD_TERNARY_ARITHMETIC_SSE_HPP
#define SIMD_TERNARY_ARITHMETIC_SSE_HPP

#include <xmmintrin.h>

namespace nova {

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

#define samples_per_loop nova::unroll_constraints<float>::samples_per_loop


namespace detail
{

template <>
inline __m128 clip(__m128 value, __m128 low, __m128 high)
{
    return _mm_max_ps(_mm_min_ps(value, high),
                      low);
}

template <>
inline __m128 muladd(__m128 value, __m128 mul, __m128 add)
{
    return _mm_add_ps(_mm_mul_ps(value, mul),
                      add);
}

} /* namespace detail */

namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_vvv(float * out, const float * arg1, const float * arg2, const float * arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    clip_vec_simd_mp_vvv<n-4>(out+4, arg1+4, arg2+4, arg3+4);
}

template <>
always_inline void clip_vec_simd_mp_vvv<0>(float * out, const float * arg1, const float * arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd(float * out, const float * arg1, const float * arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;


    do {
        detail::clip_vec_simd_mp_vvv<samples_per_loop>(out, arg1, arg2, arg3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg2 += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_mp(float * out, const float * arg1, const float * arg2, const float * arg3)
{


    detail::clip_vec_simd_mp_vvv<n>(out, arg1, arg2, arg3);
}

template<unsigned int n>
inline void clip_vec_simd(float * out, const float * arg1, const float * arg2, const float * arg3)
{


    detail::clip_vec_simd_mp_vvv<n>(out, arg1, arg2, arg3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_vvs(float * out, const float * arg1, const float * arg2, const __m128 arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    clip_vec_simd_mp_vvs<n-4>(out+4, arg1+4, arg2+4, arg3);
}

template <>
always_inline void clip_vec_simd_mp_vvs<0>(float * out, const float * arg1, const float * arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd(float * out, const float * arg1, const float * arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::clip_vec_simd_mp_vvs<samples_per_loop>(out, arg1, arg2, a3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_mp(float * out, const float * arg1, const float * arg2, const float arg3)
{
    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_vvs<n>(out, arg1, arg2, a3);
}

template<unsigned int n>
inline void clip_vec_simd(float * out, const float * arg1, const float * arg2, const float arg3)
{
    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_vvs<n>(out, arg1, arg2, a3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_vvr(float * out, const float * arg1, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    clip_vec_simd_mp_vvr<n-4>(out+4, arg1+4, arg2+4, arg3, arg3_slope);
}

template <>
always_inline void clip_vec_simd_mp_vvr<0>(float * out, const float * arg1, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r3(float * out, const float * arg1, const float * arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::clip_vec_simd_mp_vvr<samples_per_loop>(out, arg1, arg2, a3, a3_slope);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r3_mp(float * out, const float * arg1, const float * arg2, const float arg3, const float arg3_slope)
{

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_vvr<n>(out, arg1, arg2, a3, a3_slope);
}

template<unsigned int n>
inline void clip_vec_simd_r3(float * out, const float * arg1, const float * arg2, const float arg3, const float arg3_slope)
{

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_vvr<n>(out, arg1, arg2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_vsv(float * out, const float * arg1, const __m128 arg2, const float * arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    clip_vec_simd_mp_vsv<n-4>(out+4, arg1+4, arg2, arg3+4);
}

template <>
always_inline void clip_vec_simd_mp_vsv<0>(float * out, const float * arg1, const __m128 arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd(float * out, const float * arg1, const float arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a2 = _mm_set_ps1(arg2);

    do {
        detail::clip_vec_simd_mp_vsv<samples_per_loop>(out, arg1, a2, arg3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_mp(float * out, const float * arg1, const float arg2, const float * arg3)
{
    __m128 a2 = _mm_set_ps1(arg2);

    detail::clip_vec_simd_mp_vsv<n>(out, arg1, a2, arg3);
}

template<unsigned int n>
inline void clip_vec_simd(float * out, const float * arg1, const float arg2, const float * arg3)
{
    __m128 a2 = _mm_set_ps1(arg2);

    detail::clip_vec_simd_mp_vsv<n>(out, arg1, a2, arg3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_vss(float * out, const float * arg1, const __m128 arg2, const __m128 arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    clip_vec_simd_mp_vss<n-4>(out+4, arg1+4, arg2, arg3);
}

template <>
always_inline void clip_vec_simd_mp_vss<0>(float * out, const float * arg1, const __m128 arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd(float * out, const float * arg1, const float arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::clip_vec_simd_mp_vss<samples_per_loop>(out, arg1, a2, a3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_mp(float * out, const float * arg1, const float arg2, const float arg3)
{
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_vss<n>(out, arg1, a2, a3);
}

template<unsigned int n>
inline void clip_vec_simd(float * out, const float * arg1, const float arg2, const float arg3)
{
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_vss<n>(out, arg1, a2, a3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_vsr(float * out, const float * arg1, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    clip_vec_simd_mp_vsr<n-4>(out+4, arg1+4, arg2, arg3, arg3_slope);
}

template <>
always_inline void clip_vec_simd_mp_vsr<0>(float * out, const float * arg1, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r3(float * out, const float * arg1, const float arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::clip_vec_simd_mp_vsr<samples_per_loop>(out, arg1, a2, a3, a3_slope);
        out += samples_per_loop;
        arg1 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r3_mp(float * out, const float * arg1, const float arg2, const float arg3, const float arg3_slope)
{
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_vsr<n>(out, arg1, a2, a3, a3_slope);
}

template<unsigned int n>
inline void clip_vec_simd_r3(float * out, const float * arg1, const float arg2, const float arg3, const float arg3_slope)
{
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_vsr<n>(out, arg1, a2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_vrv(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    clip_vec_simd_mp_vrv<n-4>(out+4, arg1+4, arg2, arg2_slope, arg3+4);
}

template <>
always_inline void clip_vec_simd_mp_vrv<0>(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r2(float * out, const float * arg1, const float arg2, const float arg2_slope, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    do {
        detail::clip_vec_simd_mp_vrv<samples_per_loop>(out, arg1, a2, a2_slope, arg3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r2_mp(float * out, const float * arg1, const float arg2, const float arg2_slope, const float * arg3)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::clip_vec_simd_mp_vrv<n>(out, arg1, a2, a2_slope, arg3);
}

template<unsigned int n>
inline void clip_vec_simd_r2(float * out, const float * arg1, const float arg2, const float arg2_slope, const float * arg3)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::clip_vec_simd_mp_vrv<n>(out, arg1, a2, a2_slope, arg3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_vrs(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    clip_vec_simd_mp_vrs<n-4>(out+4, arg1+4, arg2, arg2_slope, arg3);
}

template <>
always_inline void clip_vec_simd_mp_vrs<0>(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r2(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::clip_vec_simd_mp_vrs<samples_per_loop>(out, arg1, a2, a2_slope, a3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r2_mp(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_vrs<n>(out, arg1, a2, a2_slope, a3);
}

template<unsigned int n>
inline void clip_vec_simd_r2(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_vrs<n>(out, arg1, a2, a2_slope, a3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_vrr(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    clip_vec_simd_mp_vrr<n-4>(out+4, arg1+4, arg2, arg2_slope, arg3, arg3_slope);
}

template <>
always_inline void clip_vec_simd_mp_vrr<0>(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r2r3(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::clip_vec_simd_mp_vrr<samples_per_loop>(out, arg1, a2, a2_slope, a3, a3_slope);
        out += samples_per_loop;
        arg1 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r2r3_mp(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_vrr<n>(out, arg1, a2, a2_slope, a3, a3_slope);
}

template<unsigned int n>
inline void clip_vec_simd_r2r3(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_vrr<n>(out, arg1, a2, a2_slope, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_svv(float * out, const __m128 arg1, const float * arg2, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    clip_vec_simd_mp_svv<n-4>(out+4, arg1, arg2+4, arg3+4);
}

template <>
always_inline void clip_vec_simd_mp_svv<0>(float * out, const __m128 arg1, const float * arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd(float * out, const float arg1, const float * arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);

    do {
        detail::clip_vec_simd_mp_svv<samples_per_loop>(out, a1, arg2, arg3);
        out += samples_per_loop;
        arg2 += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_mp(float * out, const float arg1, const float * arg2, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    detail::clip_vec_simd_mp_svv<n>(out, a1, arg2, arg3);
}

template<unsigned int n>
inline void clip_vec_simd(float * out, const float arg1, const float * arg2, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    detail::clip_vec_simd_mp_svv<n>(out, a1, arg2, arg3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_svs(float * out, const __m128 arg1, const float * arg2, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    clip_vec_simd_mp_svs<n-4>(out+4, arg1, arg2+4, arg3);
}

template <>
always_inline void clip_vec_simd_mp_svs<0>(float * out, const __m128 arg1, const float * arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd(float * out, const float arg1, const float * arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::clip_vec_simd_mp_svs<samples_per_loop>(out, a1, arg2, a3);
        out += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_mp(float * out, const float arg1, const float * arg2, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_svs<n>(out, a1, arg2, a3);
}

template<unsigned int n>
inline void clip_vec_simd(float * out, const float arg1, const float * arg2, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_svs<n>(out, a1, arg2, a3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_svr(float * out, const __m128 arg1, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    clip_vec_simd_mp_svr<n-4>(out+4, arg1, arg2+4, arg3, arg3_slope);
}

template <>
always_inline void clip_vec_simd_mp_svr<0>(float * out, const __m128 arg1, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r3(float * out, const float arg1, const float * arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::clip_vec_simd_mp_svr<samples_per_loop>(out, a1, arg2, a3, a3_slope);
        out += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r3_mp(float * out, const float arg1, const float * arg2, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_svr<n>(out, a1, arg2, a3, a3_slope);
}

template<unsigned int n>
inline void clip_vec_simd_r3(float * out, const float arg1, const float * arg2, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_svr<n>(out, a1, arg2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_ssv(float * out, const __m128 arg1, const __m128 arg2, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    clip_vec_simd_mp_ssv<n-4>(out+4, arg1, arg2, arg3+4);
}

template <>
always_inline void clip_vec_simd_mp_ssv<0>(float * out, const __m128 arg1, const __m128 arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd(float * out, const float arg1, const float arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    do {
        detail::clip_vec_simd_mp_ssv<samples_per_loop>(out, a1, a2, arg3);
        out += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_mp(float * out, const float arg1, const float arg2, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    detail::clip_vec_simd_mp_ssv<n>(out, a1, a2, arg3);
}

template<unsigned int n>
inline void clip_vec_simd(float * out, const float arg1, const float arg2, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    detail::clip_vec_simd_mp_ssv<n>(out, a1, a2, arg3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_sss(float * out, const __m128 arg1, const __m128 arg2, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    clip_vec_simd_mp_sss<n-4>(out+4, arg1, arg2, arg3);
}

template <>
always_inline void clip_vec_simd_mp_sss<0>(float * out, const __m128 arg1, const __m128 arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd(float * out, const float arg1, const float arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::clip_vec_simd_mp_sss<samples_per_loop>(out, a1, a2, a3);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_mp(float * out, const float arg1, const float arg2, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_sss<n>(out, a1, a2, a3);
}

template<unsigned int n>
inline void clip_vec_simd(float * out, const float arg1, const float arg2, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_sss<n>(out, a1, a2, a3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_ssr(float * out, const __m128 arg1, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    clip_vec_simd_mp_ssr<n-4>(out+4, arg1, arg2, arg3, arg3_slope);
}

template <>
always_inline void clip_vec_simd_mp_ssr<0>(float * out, const __m128 arg1, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r3(float * out, const float arg1, const float arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::clip_vec_simd_mp_ssr<samples_per_loop>(out, a1, a2, a3, a3_slope);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r3_mp(float * out, const float arg1, const float arg2, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_ssr<n>(out, a1, a2, a3, a3_slope);
}

template<unsigned int n>
inline void clip_vec_simd_r3(float * out, const float arg1, const float arg2, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_ssr<n>(out, a1, a2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_srv(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    clip_vec_simd_mp_srv<n-4>(out+4, arg1, arg2, arg2_slope, arg3+4);
}

template <>
always_inline void clip_vec_simd_mp_srv<0>(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r2(float * out, const float arg1, const float arg2, const float arg2_slope, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    do {
        detail::clip_vec_simd_mp_srv<samples_per_loop>(out, a1, a2, a2_slope, arg3);
        out += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r2_mp(float * out, const float arg1, const float arg2, const float arg2_slope, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::clip_vec_simd_mp_srv<n>(out, a1, a2, a2_slope, arg3);
}

template<unsigned int n>
inline void clip_vec_simd_r2(float * out, const float arg1, const float arg2, const float arg2_slope, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::clip_vec_simd_mp_srv<n>(out, a1, a2, a2_slope, arg3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_srs(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    clip_vec_simd_mp_srs<n-4>(out+4, arg1, arg2, arg2_slope, arg3);
}

template <>
always_inline void clip_vec_simd_mp_srs<0>(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r2(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::clip_vec_simd_mp_srs<samples_per_loop>(out, a1, a2, a2_slope, a3);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r2_mp(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_srs<n>(out, a1, a2, a2_slope, a3);
}

template<unsigned int n>
inline void clip_vec_simd_r2(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_srs<n>(out, a1, a2, a2_slope, a3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_srr(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    clip_vec_simd_mp_srr<n-4>(out+4, arg1, arg2, arg2_slope, arg3, arg3_slope);
}

template <>
always_inline void clip_vec_simd_mp_srr<0>(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r2r3(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::clip_vec_simd_mp_srr<samples_per_loop>(out, a1, a2, a2_slope, a3, a3_slope);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r2r3_mp(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_srr<n>(out, a1, a2, a2_slope, a3, a3_slope);
}

template<unsigned int n>
inline void clip_vec_simd_r2r3(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_srr<n>(out, a1, a2, a2_slope, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_rvv(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);

    clip_vec_simd_mp_rvv<n-4>(out+4, arg1, arg1_slope, arg2+4, arg3+4);
}

template <>
always_inline void clip_vec_simd_mp_rvv<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float * arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);

    do {
        detail::clip_vec_simd_mp_rvv<samples_per_loop>(out, a1, a1_slope, arg2, arg3);
        out += samples_per_loop;
        arg2 += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r1_mp(float * out, const float arg1, const float arg1_slope, const float * arg2, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);

    detail::clip_vec_simd_mp_rvv<n>(out, a1, a1_slope, arg2, arg3);
}

template<unsigned int n>
inline void clip_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float * arg2, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);

    detail::clip_vec_simd_mp_rvv<n>(out, a1, a1_slope, arg2, arg3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_rvs(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);

    clip_vec_simd_mp_rvs<n-4>(out+4, arg1, arg1_slope, arg2+4, arg3);
}

template <>
always_inline void clip_vec_simd_mp_rvs<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::clip_vec_simd_mp_rvs<samples_per_loop>(out, a1, a1_slope, arg2, a3);
        out += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r1_mp(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_rvs<n>(out, a1, a1_slope, arg2, a3);
}

template<unsigned int n>
inline void clip_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_rvs<n>(out, a1, a1_slope, arg2, a3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_rvr(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    clip_vec_simd_mp_rvr<n-4>(out+4, arg1, arg1_slope, arg2+4, arg3, arg3_slope);
}

template <>
always_inline void clip_vec_simd_mp_rvr<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r1r3(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::clip_vec_simd_mp_rvr<samples_per_loop>(out, a1, a1_slope, arg2, a3, a3_slope);
        out += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r1r3_mp(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_rvr<n>(out, a1, a1_slope, arg2, a3, a3_slope);
}

template<unsigned int n>
inline void clip_vec_simd_r1r3(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_rvr<n>(out, a1, a1_slope, arg2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_rsv(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);

    clip_vec_simd_mp_rsv<n-4>(out+4, arg1, arg1_slope, arg2, arg3+4);
}

template <>
always_inline void clip_vec_simd_mp_rsv<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    do {
        detail::clip_vec_simd_mp_rsv<samples_per_loop>(out, a1, a1_slope, a2, arg3);
        out += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r1_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    detail::clip_vec_simd_mp_rsv<n>(out, a1, a1_slope, a2, arg3);
}

template<unsigned int n>
inline void clip_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float arg2, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    detail::clip_vec_simd_mp_rsv<n>(out, a1, a1_slope, a2, arg3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_rss(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);

    clip_vec_simd_mp_rss<n-4>(out+4, arg1, arg1_slope, arg2, arg3);
}

template <>
always_inline void clip_vec_simd_mp_rss<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::clip_vec_simd_mp_rss<samples_per_loop>(out, a1, a1_slope, a2, a3);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r1_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_rss<n>(out, a1, a1_slope, a2, a3);
}

template<unsigned int n>
inline void clip_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_rss<n>(out, a1, a1_slope, a2, a3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_rsr(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    clip_vec_simd_mp_rsr<n-4>(out+4, arg1, arg1_slope, arg2, arg3, arg3_slope);
}

template <>
always_inline void clip_vec_simd_mp_rsr<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r1r3(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::clip_vec_simd_mp_rsr<samples_per_loop>(out, a1, a1_slope, a2, a3, a3_slope);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r1r3_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_rsr<n>(out, a1, a1_slope, a2, a3, a3_slope);
}

template<unsigned int n>
inline void clip_vec_simd_r1r3(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_rsr<n>(out, a1, a1_slope, a2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_rrv(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    clip_vec_simd_mp_rrv<n-4>(out+4, arg1, arg1_slope, arg2, arg2_slope, arg3+4);
}

template <>
always_inline void clip_vec_simd_mp_rrv<0>(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r1r2(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    do {
        detail::clip_vec_simd_mp_rrv<samples_per_loop>(out, a1, a1_slope, a2, a2_slope, arg3);
        out += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r1r2_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::clip_vec_simd_mp_rrv<n>(out, a1, a1_slope, a2, a2_slope, arg3);
}

template<unsigned int n>
inline void clip_vec_simd_r1r2(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::clip_vec_simd_mp_rrv<n>(out, a1, a1_slope, a2, a2_slope, arg3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_rrs(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    clip_vec_simd_mp_rrs<n-4>(out+4, arg1, arg1_slope, arg2, arg2_slope, arg3);
}

template <>
always_inline void clip_vec_simd_mp_rrs<0>(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r1r2(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::clip_vec_simd_mp_rrs<samples_per_loop>(out, a1, a1_slope, a2, a2_slope, a3);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r1r2_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_rrs<n>(out, a1, a1_slope, a2, a2_slope, a3);
}

template<unsigned int n>
inline void clip_vec_simd_r1r2(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::clip_vec_simd_mp_rrs<n>(out, a1, a1_slope, a2, a2_slope, a3);
}


namespace detail
{

template <int n>
always_inline void clip_vec_simd_mp_rrr(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::clip<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);
    arg2 = _mm_add_ps(arg2, arg2_slope);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    clip_vec_simd_mp_rrr<n-4>(out+4, arg1, arg1_slope, arg2, arg2_slope, arg3, arg3_slope);
}

template <>
always_inline void clip_vec_simd_mp_rrr<0>(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void clip_vec_simd_r1r2r3(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::clip_vec_simd_mp_rrr<samples_per_loop>(out, a1, a1_slope, a2, a2_slope, a3, a3_slope);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void clip_vec_simd_r1r2r3_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_rrr<n>(out, a1, a1_slope, a2, a2_slope, a3, a3_slope);
}

template<unsigned int n>
inline void clip_vec_simd_r1r2r3(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::clip_vec_simd_mp_rrr<n>(out, a1, a1_slope, a2, a2_slope, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_vvv(float * out, const float * arg1, const float * arg2, const float * arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    muladd_vec_simd_mp_vvv<n-4>(out+4, arg1+4, arg2+4, arg3+4);
}

template <>
always_inline void muladd_vec_simd_mp_vvv<0>(float * out, const float * arg1, const float * arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd(float * out, const float * arg1, const float * arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;


    do {
        detail::muladd_vec_simd_mp_vvv<samples_per_loop>(out, arg1, arg2, arg3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg2 += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_mp(float * out, const float * arg1, const float * arg2, const float * arg3)
{


    detail::muladd_vec_simd_mp_vvv<n>(out, arg1, arg2, arg3);
}

template<unsigned int n>
inline void muladd_vec_simd(float * out, const float * arg1, const float * arg2, const float * arg3)
{


    detail::muladd_vec_simd_mp_vvv<n>(out, arg1, arg2, arg3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_vvs(float * out, const float * arg1, const float * arg2, const __m128 arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    muladd_vec_simd_mp_vvs<n-4>(out+4, arg1+4, arg2+4, arg3);
}

template <>
always_inline void muladd_vec_simd_mp_vvs<0>(float * out, const float * arg1, const float * arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd(float * out, const float * arg1, const float * arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::muladd_vec_simd_mp_vvs<samples_per_loop>(out, arg1, arg2, a3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_mp(float * out, const float * arg1, const float * arg2, const float arg3)
{
    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_vvs<n>(out, arg1, arg2, a3);
}

template<unsigned int n>
inline void muladd_vec_simd(float * out, const float * arg1, const float * arg2, const float arg3)
{
    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_vvs<n>(out, arg1, arg2, a3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_vvr(float * out, const float * arg1, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    muladd_vec_simd_mp_vvr<n-4>(out+4, arg1+4, arg2+4, arg3, arg3_slope);
}

template <>
always_inline void muladd_vec_simd_mp_vvr<0>(float * out, const float * arg1, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r3(float * out, const float * arg1, const float * arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::muladd_vec_simd_mp_vvr<samples_per_loop>(out, arg1, arg2, a3, a3_slope);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r3_mp(float * out, const float * arg1, const float * arg2, const float arg3, const float arg3_slope)
{

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_vvr<n>(out, arg1, arg2, a3, a3_slope);
}

template<unsigned int n>
inline void muladd_vec_simd_r3(float * out, const float * arg1, const float * arg2, const float arg3, const float arg3_slope)
{

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_vvr<n>(out, arg1, arg2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_vsv(float * out, const float * arg1, const __m128 arg2, const float * arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    muladd_vec_simd_mp_vsv<n-4>(out+4, arg1+4, arg2, arg3+4);
}

template <>
always_inline void muladd_vec_simd_mp_vsv<0>(float * out, const float * arg1, const __m128 arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd(float * out, const float * arg1, const float arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a2 = _mm_set_ps1(arg2);

    do {
        detail::muladd_vec_simd_mp_vsv<samples_per_loop>(out, arg1, a2, arg3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_mp(float * out, const float * arg1, const float arg2, const float * arg3)
{
    __m128 a2 = _mm_set_ps1(arg2);

    detail::muladd_vec_simd_mp_vsv<n>(out, arg1, a2, arg3);
}

template<unsigned int n>
inline void muladd_vec_simd(float * out, const float * arg1, const float arg2, const float * arg3)
{
    __m128 a2 = _mm_set_ps1(arg2);

    detail::muladd_vec_simd_mp_vsv<n>(out, arg1, a2, arg3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_vss(float * out, const float * arg1, const __m128 arg2, const __m128 arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    muladd_vec_simd_mp_vss<n-4>(out+4, arg1+4, arg2, arg3);
}

template <>
always_inline void muladd_vec_simd_mp_vss<0>(float * out, const float * arg1, const __m128 arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd(float * out, const float * arg1, const float arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::muladd_vec_simd_mp_vss<samples_per_loop>(out, arg1, a2, a3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_mp(float * out, const float * arg1, const float arg2, const float arg3)
{
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_vss<n>(out, arg1, a2, a3);
}

template<unsigned int n>
inline void muladd_vec_simd(float * out, const float * arg1, const float arg2, const float arg3)
{
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_vss<n>(out, arg1, a2, a3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_vsr(float * out, const float * arg1, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    muladd_vec_simd_mp_vsr<n-4>(out+4, arg1+4, arg2, arg3, arg3_slope);
}

template <>
always_inline void muladd_vec_simd_mp_vsr<0>(float * out, const float * arg1, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r3(float * out, const float * arg1, const float arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::muladd_vec_simd_mp_vsr<samples_per_loop>(out, arg1, a2, a3, a3_slope);
        out += samples_per_loop;
        arg1 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r3_mp(float * out, const float * arg1, const float arg2, const float arg3, const float arg3_slope)
{
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_vsr<n>(out, arg1, a2, a3, a3_slope);
}

template<unsigned int n>
inline void muladd_vec_simd_r3(float * out, const float * arg1, const float arg2, const float arg3, const float arg3_slope)
{
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_vsr<n>(out, arg1, a2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_vrv(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    muladd_vec_simd_mp_vrv<n-4>(out+4, arg1+4, arg2, arg2_slope, arg3+4);
}

template <>
always_inline void muladd_vec_simd_mp_vrv<0>(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r2(float * out, const float * arg1, const float arg2, const float arg2_slope, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    do {
        detail::muladd_vec_simd_mp_vrv<samples_per_loop>(out, arg1, a2, a2_slope, arg3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r2_mp(float * out, const float * arg1, const float arg2, const float arg2_slope, const float * arg3)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::muladd_vec_simd_mp_vrv<n>(out, arg1, a2, a2_slope, arg3);
}

template<unsigned int n>
inline void muladd_vec_simd_r2(float * out, const float * arg1, const float arg2, const float arg2_slope, const float * arg3)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::muladd_vec_simd_mp_vrv<n>(out, arg1, a2, a2_slope, arg3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_vrs(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    muladd_vec_simd_mp_vrs<n-4>(out+4, arg1+4, arg2, arg2_slope, arg3);
}

template <>
always_inline void muladd_vec_simd_mp_vrs<0>(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r2(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::muladd_vec_simd_mp_vrs<samples_per_loop>(out, arg1, a2, a2_slope, a3);
        out += samples_per_loop;
        arg1 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r2_mp(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_vrs<n>(out, arg1, a2, a2_slope, a3);
}

template<unsigned int n>
inline void muladd_vec_simd_r2(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_vrs<n>(out, arg1, a2, a2_slope, a3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_vrr(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = _mm_load_ps(arg1);
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    muladd_vec_simd_mp_vrr<n-4>(out+4, arg1+4, arg2, arg2_slope, arg3, arg3_slope);
}

template <>
always_inline void muladd_vec_simd_mp_vrr<0>(float * out, const float * arg1, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r2r3(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::muladd_vec_simd_mp_vrr<samples_per_loop>(out, arg1, a2, a2_slope, a3, a3_slope);
        out += samples_per_loop;
        arg1 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r2r3_mp(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_vrr<n>(out, arg1, a2, a2_slope, a3, a3_slope);
}

template<unsigned int n>
inline void muladd_vec_simd_r2r3(float * out, const float * arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_vrr<n>(out, arg1, a2, a2_slope, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_svv(float * out, const __m128 arg1, const float * arg2, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    muladd_vec_simd_mp_svv<n-4>(out+4, arg1, arg2+4, arg3+4);
}

template <>
always_inline void muladd_vec_simd_mp_svv<0>(float * out, const __m128 arg1, const float * arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd(float * out, const float arg1, const float * arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);

    do {
        detail::muladd_vec_simd_mp_svv<samples_per_loop>(out, a1, arg2, arg3);
        out += samples_per_loop;
        arg2 += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_mp(float * out, const float arg1, const float * arg2, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    detail::muladd_vec_simd_mp_svv<n>(out, a1, arg2, arg3);
}

template<unsigned int n>
inline void muladd_vec_simd(float * out, const float arg1, const float * arg2, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    detail::muladd_vec_simd_mp_svv<n>(out, a1, arg2, arg3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_svs(float * out, const __m128 arg1, const float * arg2, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    muladd_vec_simd_mp_svs<n-4>(out+4, arg1, arg2+4, arg3);
}

template <>
always_inline void muladd_vec_simd_mp_svs<0>(float * out, const __m128 arg1, const float * arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd(float * out, const float arg1, const float * arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::muladd_vec_simd_mp_svs<samples_per_loop>(out, a1, arg2, a3);
        out += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_mp(float * out, const float arg1, const float * arg2, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_svs<n>(out, a1, arg2, a3);
}

template<unsigned int n>
inline void muladd_vec_simd(float * out, const float arg1, const float * arg2, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_svs<n>(out, a1, arg2, a3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_svr(float * out, const __m128 arg1, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    muladd_vec_simd_mp_svr<n-4>(out+4, arg1, arg2+4, arg3, arg3_slope);
}

template <>
always_inline void muladd_vec_simd_mp_svr<0>(float * out, const __m128 arg1, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r3(float * out, const float arg1, const float * arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::muladd_vec_simd_mp_svr<samples_per_loop>(out, a1, arg2, a3, a3_slope);
        out += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r3_mp(float * out, const float arg1, const float * arg2, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_svr<n>(out, a1, arg2, a3, a3_slope);
}

template<unsigned int n>
inline void muladd_vec_simd_r3(float * out, const float arg1, const float * arg2, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_svr<n>(out, a1, arg2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_ssv(float * out, const __m128 arg1, const __m128 arg2, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    muladd_vec_simd_mp_ssv<n-4>(out+4, arg1, arg2, arg3+4);
}

template <>
always_inline void muladd_vec_simd_mp_ssv<0>(float * out, const __m128 arg1, const __m128 arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd(float * out, const float arg1, const float arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    do {
        detail::muladd_vec_simd_mp_ssv<samples_per_loop>(out, a1, a2, arg3);
        out += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_mp(float * out, const float arg1, const float arg2, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    detail::muladd_vec_simd_mp_ssv<n>(out, a1, a2, arg3);
}

template<unsigned int n>
inline void muladd_vec_simd(float * out, const float arg1, const float arg2, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    detail::muladd_vec_simd_mp_ssv<n>(out, a1, a2, arg3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_sss(float * out, const __m128 arg1, const __m128 arg2, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);


    muladd_vec_simd_mp_sss<n-4>(out+4, arg1, arg2, arg3);
}

template <>
always_inline void muladd_vec_simd_mp_sss<0>(float * out, const __m128 arg1, const __m128 arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd(float * out, const float arg1, const float arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::muladd_vec_simd_mp_sss<samples_per_loop>(out, a1, a2, a3);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_mp(float * out, const float arg1, const float arg2, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_sss<n>(out, a1, a2, a3);
}

template<unsigned int n>
inline void muladd_vec_simd(float * out, const float arg1, const float arg2, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_sss<n>(out, a1, a2, a3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_ssr(float * out, const __m128 arg1, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    muladd_vec_simd_mp_ssr<n-4>(out+4, arg1, arg2, arg3, arg3_slope);
}

template <>
always_inline void muladd_vec_simd_mp_ssr<0>(float * out, const __m128 arg1, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r3(float * out, const float arg1, const float arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::muladd_vec_simd_mp_ssr<samples_per_loop>(out, a1, a2, a3, a3_slope);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r3_mp(float * out, const float arg1, const float arg2, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_ssr<n>(out, a1, a2, a3, a3_slope);
}

template<unsigned int n>
inline void muladd_vec_simd_r3(float * out, const float arg1, const float arg2, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);
    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_ssr<n>(out, a1, a2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_srv(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    muladd_vec_simd_mp_srv<n-4>(out+4, arg1, arg2, arg2_slope, arg3+4);
}

template <>
always_inline void muladd_vec_simd_mp_srv<0>(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r2(float * out, const float arg1, const float arg2, const float arg2_slope, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    do {
        detail::muladd_vec_simd_mp_srv<samples_per_loop>(out, a1, a2, a2_slope, arg3);
        out += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r2_mp(float * out, const float arg1, const float arg2, const float arg2_slope, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::muladd_vec_simd_mp_srv<n>(out, a1, a2, a2_slope, arg3);
}

template<unsigned int n>
inline void muladd_vec_simd_r2(float * out, const float arg1, const float arg2, const float arg2_slope, const float * arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::muladd_vec_simd_mp_srv<n>(out, a1, a2, a2_slope, arg3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_srs(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    muladd_vec_simd_mp_srs<n-4>(out+4, arg1, arg2, arg2_slope, arg3);
}

template <>
always_inline void muladd_vec_simd_mp_srs<0>(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r2(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::muladd_vec_simd_mp_srs<samples_per_loop>(out, a1, a2, a2_slope, a3);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r2_mp(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_srs<n>(out, a1, a2, a2_slope, a3);
}

template<unsigned int n>
inline void muladd_vec_simd_r2(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_srs<n>(out, a1, a2, a2_slope, a3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_srr(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg2 = _mm_add_ps(arg2, arg2_slope);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    muladd_vec_simd_mp_srr<n-4>(out+4, arg1, arg2, arg2_slope, arg3, arg3_slope);
}

template <>
always_inline void muladd_vec_simd_mp_srr<0>(float * out, const __m128 arg1, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r2r3(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::muladd_vec_simd_mp_srr<samples_per_loop>(out, a1, a2, a2_slope, a3, a3_slope);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r2r3_mp(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_srr<n>(out, a1, a2, a2_slope, a3, a3_slope);
}

template<unsigned int n>
inline void muladd_vec_simd_r2r3(float * out, const float arg1, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{
    __m128 a1 = _mm_set_ps1(arg1);

    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_srr<n>(out, a1, a2, a2_slope, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_rvv(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);

    muladd_vec_simd_mp_rvv<n-4>(out+4, arg1, arg1_slope, arg2+4, arg3+4);
}

template <>
always_inline void muladd_vec_simd_mp_rvv<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float * arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);

    do {
        detail::muladd_vec_simd_mp_rvv<samples_per_loop>(out, a1, a1_slope, arg2, arg3);
        out += samples_per_loop;
        arg2 += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r1_mp(float * out, const float arg1, const float arg1_slope, const float * arg2, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);

    detail::muladd_vec_simd_mp_rvv<n>(out, a1, a1_slope, arg2, arg3);
}

template<unsigned int n>
inline void muladd_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float * arg2, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);

    detail::muladd_vec_simd_mp_rvv<n>(out, a1, a1_slope, arg2, arg3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_rvs(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);

    muladd_vec_simd_mp_rvs<n-4>(out+4, arg1, arg1_slope, arg2+4, arg3);
}

template <>
always_inline void muladd_vec_simd_mp_rvs<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::muladd_vec_simd_mp_rvs<samples_per_loop>(out, a1, a1_slope, arg2, a3);
        out += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r1_mp(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_rvs<n>(out, a1, a1_slope, arg2, a3);
}

template<unsigned int n>
inline void muladd_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_rvs<n>(out, a1, a1_slope, arg2, a3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_rvr(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = _mm_load_ps(arg2);
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    muladd_vec_simd_mp_rvr<n-4>(out+4, arg1, arg1_slope, arg2+4, arg3, arg3_slope);
}

template <>
always_inline void muladd_vec_simd_mp_rvr<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const float * arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r1r3(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::muladd_vec_simd_mp_rvr<samples_per_loop>(out, a1, a1_slope, arg2, a3, a3_slope);
        out += samples_per_loop;
        arg2 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r1r3_mp(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_rvr<n>(out, a1, a1_slope, arg2, a3, a3_slope);
}

template<unsigned int n>
inline void muladd_vec_simd_r1r3(float * out, const float arg1, const float arg1_slope, const float * arg2, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_rvr<n>(out, a1, a1_slope, arg2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_rsv(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);

    muladd_vec_simd_mp_rsv<n-4>(out+4, arg1, arg1_slope, arg2, arg3+4);
}

template <>
always_inline void muladd_vec_simd_mp_rsv<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, const float * arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float arg2, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    do {
        detail::muladd_vec_simd_mp_rsv<samples_per_loop>(out, a1, a1_slope, a2, arg3);
        out += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r1_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    detail::muladd_vec_simd_mp_rsv<n>(out, a1, a1_slope, a2, arg3);
}

template<unsigned int n>
inline void muladd_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float arg2, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    detail::muladd_vec_simd_mp_rsv<n>(out, a1, a1_slope, a2, arg3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_rss(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);

    muladd_vec_simd_mp_rss<n-4>(out+4, arg1, arg1_slope, arg2, arg3);
}

template <>
always_inline void muladd_vec_simd_mp_rss<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::muladd_vec_simd_mp_rss<samples_per_loop>(out, a1, a1_slope, a2, a3);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r1_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_rss<n>(out, a1, a1_slope, a2, a3);
}

template<unsigned int n>
inline void muladd_vec_simd_r1(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);
    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_rss<n>(out, a1, a1_slope, a2, a3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_rsr(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    muladd_vec_simd_mp_rsr<n-4>(out+4, arg1, arg1_slope, arg2, arg3, arg3_slope);
}

template <>
always_inline void muladd_vec_simd_mp_rsr<0>(float * out, __m128 & arg1, const __m128 arg1_slope, const __m128 arg2, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r1r3(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::muladd_vec_simd_mp_rsr<samples_per_loop>(out, a1, a1_slope, a2, a3, a3_slope);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r1r3_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_rsr<n>(out, a1, a1_slope, a2, a3, a3_slope);
}

template<unsigned int n>
inline void muladd_vec_simd_r1r3(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);    __m128 a2 = _mm_set_ps1(arg2);

    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_rsr<n>(out, a1, a1_slope, a2, a3, a3_slope);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_rrv(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = _mm_load_ps(arg3);

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    muladd_vec_simd_mp_rrv<n-4>(out+4, arg1, arg1_slope, arg2, arg2_slope, arg3+4);
}

template <>
always_inline void muladd_vec_simd_mp_rrv<0>(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, const float * arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r1r2(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float * arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    do {
        detail::muladd_vec_simd_mp_rrv<samples_per_loop>(out, a1, a1_slope, a2, a2_slope, arg3);
        out += samples_per_loop;
        arg3 += samples_per_loop;
    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r1r2_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::muladd_vec_simd_mp_rrv<n>(out, a1, a1_slope, a2, a2_slope, arg3);
}

template<unsigned int n>
inline void muladd_vec_simd_r1r2(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float * arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    detail::muladd_vec_simd_mp_rrv<n>(out, a1, a1_slope, a2, a2_slope, arg3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_rrs(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);
    arg2 = _mm_add_ps(arg2, arg2_slope);

    muladd_vec_simd_mp_rrs<n-4>(out+4, arg1, arg1_slope, arg2, arg2_slope, arg3);
}

template <>
always_inline void muladd_vec_simd_mp_rrs<0>(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, const __m128 arg3)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r1r2(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    do {
        detail::muladd_vec_simd_mp_rrs<samples_per_loop>(out, a1, a1_slope, a2, a2_slope, a3);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r1r2_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_rrs<n>(out, a1, a1_slope, a2, a2_slope, a3);
}

template<unsigned int n>
inline void muladd_vec_simd_r1r2(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);    __m128 a3 = _mm_set_ps1(arg3);

    detail::muladd_vec_simd_mp_rrs<n>(out, a1, a1_slope, a2, a2_slope, a3);
}


namespace detail
{

template <int n>
always_inline void muladd_vec_simd_mp_rrr(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{
    __m128 a1 = arg1;
    __m128 a2 = arg2;
    __m128 a3 = arg3;

    __m128 ret = detail::muladd<__m128>(a1, a2, a3);
    _mm_store_ps(out, ret);
    arg1 = _mm_add_ps(arg1, arg1_slope);
    arg2 = _mm_add_ps(arg2, arg2_slope);
    arg3 = _mm_add_ps(arg3, arg3_slope);

    muladd_vec_simd_mp_rrr<n-4>(out+4, arg1, arg1_slope, arg2, arg2_slope, arg3, arg3_slope);
}

template <>
always_inline void muladd_vec_simd_mp_rrr<0>(float * out, __m128 & arg1, const __m128 arg1_slope, __m128 & arg2, const __m128 arg2_slope, __m128 & arg3, const __m128 arg3_slope)
{}

} /* namespace detail */


template <>
inline void muladd_vec_simd_r1r2r3(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    do {
        detail::muladd_vec_simd_mp_rrr<samples_per_loop>(out, a1, a1_slope, a2, a2_slope, a3, a3_slope);
        out += samples_per_loop;

    } while (--loops);
}

template<unsigned int n>
always_inline void muladd_vec_simd_r1r2r3_mp(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_rrr<n>(out, a1, a1_slope, a2, a2_slope, a3, a3_slope);
}

template<unsigned int n>
inline void muladd_vec_simd_r1r2r3(float * out, const float arg1, const float arg1_slope, const float arg2, const float arg2_slope, const float arg3, const float arg3_slope)
{

    __m128 a1 = _mm_setr_ps(arg1, arg1 + arg1_slope,
                            arg1 + arg1_slope + arg1_slope,
                            arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 a1_slope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    __m128 a2 = _mm_setr_ps(arg2, arg2 + arg2_slope,
                            arg2 + arg2_slope + arg2_slope,
                            arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 a2_slope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    __m128 a3 = _mm_setr_ps(arg3, arg3 + arg3_slope,
                            arg3 + arg3_slope + arg3_slope,
                            arg3 + arg3_slope + arg3_slope + arg3_slope);
    __m128 a3_slope = _mm_set_ps1(arg3_slope + arg3_slope + arg3_slope + arg3_slope);

    detail::muladd_vec_simd_mp_rrr<n>(out, a1, a1_slope, a2, a2_slope, a3, a3_slope);
}


#undef always_inline
#undef samples_per_loop

} /* namespace nova */

#endif /* SIMD_TERNARY_ARITHMETIC_SSE_HPP */
