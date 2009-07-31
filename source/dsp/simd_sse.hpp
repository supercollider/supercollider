//  fundamental arithmetic functions, coded in sse with intrinsics
//  Copyright (C) 2005, 2006, 2007, 2008 Tim Blechmann
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

#ifndef SIMD_SSE_HPP
#define SIMD_SSE_HPP

#include <cassert>
#include <xmmintrin.h>
#include "arithmetic.hpp"
#include "simd_sse_mp.hpp"

namespace nova
{
/***********************************************************************
 * basic templates
 ***********************************************************************/

#define VEC_VEC_SIMD_N(fn, base, dst, src, n)   \
    {                                           \
        n /= base;                              \
        do                                      \
        {                                       \
            fn<base>(dst, src);                 \
            dst += base;                        \
            src += base;                        \
        }                                       \
        while (--n);                            \
    }


#define VEC_VECVEC_SIMD_N(fn, base, dst, src1, src2, n) \
    {                                                   \
        n /= base;                                      \
        do                                              \
        {                                               \
            fn<base>(dst, src1, src2);                  \
            dst += base;                                \
            src1 += base;                               \
            src2 += base;                               \
        }                                               \
        while (--n);                                    \
    }

#define VEC_VECVECVEC_SIMD_N(fn, base, dst, src1, src2, src3, n)    \
    {                                                               \
        n /= base;                                                  \
        do                                                          \
        {                                                           \
            fn<base>(dst, src1, src2, src3);                        \
            dst += base;                                            \
            src1 += base;                                           \
            src2 += base;                                           \
            src3 += base;                                           \
        }                                                           \
        while (--n);                                                \
    }

#define VEC_VECVECSCAL_SIMD_N(fn, base, dst, src1, src2, src3, n)   \
    {                                                               \
        n /= base;                                                  \
        do                                                          \
        {                                                           \
            fn<base>(dst, src1, src2, src3);                        \
            dst += base;                                            \
            src1 += base;                                           \
            src2 += base;                                           \
        }                                                           \
        while (--n);                                                \
    }

#define VEC_VECSCALVEC_SIMD_N(fn, base, dst, src1, src2, src3, n)   \
    {                                                               \
        n /= base;                                                  \
        do                                                          \
        {                                                           \
            fn<base>(dst, src1, src2, src3);                        \
            dst += base;                                            \
            src1 += base;                                           \
            src3 += base;                                           \
        }                                                           \
        while (--n);                                                \
    }


#define VEC_VECSCAL_SIMD_N(fn, base, dst, src, f, n)    \
    {                                                   \
        n /= base;                                      \
        do                                              \
        {                                               \
            fn<base>(dst, src, f);                      \
            dst += base;                                \
            src += base;                                \
        }                                               \
        while (--n);                                    \
    }

#define VEC_VECSCALSCAL_SIMD_N(fn, base, dst, src, f1, f2, n)   \
    {                                                           \
        n /= base;                                              \
        do                                                      \
        {                                                       \
            fn<base>(dst, src, f1, f2);                         \
            dst += base;                                        \
            src += base;                                        \
        }                                                       \
        while (--n);                                            \
    }

#define VEC_SCALVEC_SIMD_N(fn, base, dst, f, src, n)    \
    {                                                   \
        n /= base;                                      \
        do                                              \
        {                                               \
            fn<base>(dst, f, src);                      \
            dst += base;                                \
            src += base;                                \
        }                                               \
        while (--n);                                    \
    }

const uint samples_per_loop = 8;

/***********************************************************************
 * basic add methods
 ***********************************************************************/

inline void addvec_simd(float *dst, const float *src, uint n)
{
    VEC_VEC_SIMD_N(addvec_simd_mp, samples_per_loop, dst, src, n);
}

inline void addvec_simd(float *dst, const float *src1, const float *src2, uint n)
{
    VEC_VECVEC_SIMD_N(addvec_simd_mp, samples_per_loop, dst, src1, src2, n);
}

inline void addvec_simd(float *dst, const float *src1, const float *src2, const float *src3, uint n)
{
    VEC_VECVECVEC_SIMD_N(addvec_simd_mp, samples_per_loop, dst, src1, src2, src3, n);
}

#define UNARY_ARITHMETIC_OPERATION_WRAPPER(OPERATION)                   \
    inline void OPERATION##_vec_simd(float * dest, const float * src, uint n) \
    {                                                                   \
        VEC_VEC_SIMD_N(OPERATION##_vec_simd_mp, samples_per_loop, dest, src, n); \
    }

#define COPY_OPERATION_WRAPPER(OPERATION)                               \
    inline void OPERATION##_simd(float * dest, const float * src, uint n) \
    {                                                                   \
        VEC_VEC_SIMD_N(OPERATION##_simd_mp, samples_per_loop, dest, src, n); \
    }


/***********************************************************************
 * copy operations
 ***********************************************************************/

COPY_OPERATION_WRAPPER(testcopyvec);
COPY_OPERATION_WRAPPER(testaddvec);

/***********************************************************************
 * binary arithmetic functions
 ***********************************************************************/

inline void times_vec2_simd(float * dest0, const float * src0, float f0,
                            float * dest1, const float * src1, float f1,
                            uint n)
{
    /* checked: faster than separate loops
     * */

    n /= samples_per_loop;
    do
    {
        times_vec_simd_mp<samples_per_loop>(dest0, src0, f0);
        times_vec_simd_mp<samples_per_loop>(dest1, src1, f1);
        dest0 += samples_per_loop;
        src0 += samples_per_loop;
        dest1 += samples_per_loop;
        src1 += samples_per_loop;
    }
    while (--n);
}


/***********************************************************************
 * unary vector operations
 ***********************************************************************/

UNARY_ARITHMETIC_OPERATION_WRAPPER(square);
UNARY_ARITHMETIC_OPERATION_WRAPPER(cube);
UNARY_ARITHMETIC_OPERATION_WRAPPER(sqrt);
UNARY_ARITHMETIC_OPERATION_WRAPPER(rsqrt);
UNARY_ARITHMETIC_OPERATION_WRAPPER(rcp);
UNARY_ARITHMETIC_OPERATION_WRAPPER(abs);
UNARY_ARITHMETIC_OPERATION_WRAPPER(sgn);


/***********************************************************************
 * clip operations
 ***********************************************************************/

inline void clip_vec_simd(float * dest, const float * src, const float mul, const float add, uint n)
{
    VEC_VECSCALSCAL_SIMD_N(clip_vec_simd_mp, samples_per_loop, dest, src, mul, add, n);
}

inline void clip_vec_simd(float * dest, const float * src, const float * mul, const float add, uint n)
{
    VEC_VECVECSCAL_SIMD_N(clip_vec_simd_mp, samples_per_loop, dest, src, mul, add, n);
}

inline void clip_vec_simd(float * dest, const float * src, const float mul, const float * add, uint n)
{
    VEC_VECSCALVEC_SIMD_N(clip_vec_simd_mp, samples_per_loop, dest, src, mul, add, n);
}

inline void clip_vec_simd(float * dest, const float * src, const float * mul, const float * add, uint n)
{
    VEC_VECVECVEC_SIMD_N(clip_vec_simd_mp, samples_per_loop, dest, src, mul, add, n);
}


/***********************************************************************
 * peak finding operations
 ***********************************************************************/

inline void abspeak_simd(const float * src, float & peak, uint n)
{
    const __m128 *in = (__m128*) src;
    detail::bitmask_union m;
    m.l = 0x7fffffff;
    const __m128 bitmask = _mm_set1_ps(m.f);
    __m128 maximum = _mm_load_ss(&peak);

    n = n>>4;

    do
    {
        maximum = _mm_max_ps(_mm_and_ps(in[0], bitmask), maximum);
        maximum = _mm_max_ps(_mm_and_ps(in[1], bitmask), maximum);
        maximum = _mm_max_ps(_mm_and_ps(in[2], bitmask), maximum);
        maximum = _mm_max_ps(_mm_and_ps(in[3], bitmask), maximum);

        in += 4;
    }
    while (--n);

    /* accumulate */
    __m128 accum1 = _mm_movehl_ps(maximum, maximum);
    __m128 accum2 = accum1;
    __m128 accum3 = maximum;
    accum2 = _mm_shuffle_ps(accum2, accum2, 81);
    accum3 = _mm_shuffle_ps(accum3, accum3, 81);

    __m128 maximum2 = _mm_max_ss(accum2, accum3);
    maximum = _mm_max_ss(maximum, accum1);
    maximum = _mm_max_ss(maximum, maximum2);

    _mm_store_ss(&peak, maximum);
}


inline void rms_simd(const float * src, float & rms, uint n)
{
    const __m128 *in = (__m128*) src;
    __m128 sum = _mm_load_ss(&rms);

    n = n>>4;

    do
    {
        sum = _mm_add_ps(sum, _mm_mul_ps(in[0], in[0]));
        sum = _mm_add_ps(sum, _mm_mul_ps(in[1], in[1]));
        sum = _mm_add_ps(sum, _mm_mul_ps(in[2], in[2]));
        sum = _mm_add_ps(sum, _mm_mul_ps(in[3], in[3]));

        in += 4;
    }
    while (--n);

    /* accumulate */
    __m128 accum1 = _mm_movehl_ps(sum, sum);
    __m128 accum2 = accum1;
    __m128 accum3 = sum;
    accum2 = _mm_shuffle_ps(accum2, accum2, 81);
    accum3 = _mm_shuffle_ps(accum3, accum3, 81);

    __m128 sum2 = _mm_add_ss(accum2, accum3);
    sum = _mm_add_ss(sum, accum1);
    sum = _mm_add_ss(sum, sum2);

    _mm_store_ss(&rms, sum);
}



inline void peak_rms_simd(const float * src, float & peak, float & rms, uint n)
{
    const __m128 *in = (__m128*) src;
    detail::bitmask_union m;
    m.l = 0x7fffffff;
    const __m128 bitmask = _mm_set1_ps(m.f);

    __m128 maximum = _mm_load_ss(&peak);
    __m128 sum = _mm_load_ss(&rms);

    n = n>>4;

    do
    {
        __m128 smp1 = in[0];
        sum = _mm_add_ps(sum, _mm_mul_ps(smp1, smp1));
        maximum = _mm_max_ps(_mm_and_ps(smp1, bitmask), maximum);
        sum = _mm_add_ps(sum, _mm_mul_ps(in[1], in[1]));
        maximum = _mm_max_ps(_mm_and_ps(in[1], bitmask), maximum);
        sum = _mm_add_ps(sum, _mm_mul_ps(in[2], in[2]));
        maximum = _mm_max_ps(_mm_and_ps(in[2], bitmask), maximum);
        sum = _mm_add_ps(sum, _mm_mul_ps(in[3], in[3]));
        maximum = _mm_max_ps(_mm_and_ps(in[3], bitmask), maximum);

        in += 4;
    }
    while (--n);

    /* accumulate */
    __m128 accum1 = _mm_movehl_ps(sum, sum);
    __m128 accum2 = accum1;
    __m128 accum3 = sum;
    accum2 = _mm_shuffle_ps(accum2, accum2, 81);
    accum3 = _mm_shuffle_ps(accum3, accum3, 81);

    __m128 sum2 = _mm_add_ss(accum2, accum3);
    sum = _mm_add_ss(sum, accum1);
    sum = _mm_add_ss(sum, sum2);

    _mm_store_ss(&rms, sum);

    /* accumulate */
    accum1 = _mm_movehl_ps(maximum, maximum);
    accum2 = accum1;
    accum3 = maximum;
    accum2 = _mm_shuffle_ps(accum2, accum2, 81);
    accum3 = _mm_shuffle_ps(accum3, accum3, 81);

    __m128 maximum2 = _mm_max_ss(accum2, accum3);
    maximum = _mm_max_ss(maximum, accum1);
    maximum = _mm_max_ss(maximum, maximum2);

    _mm_store_ss(&peak, maximum);
}

/***********************************************************************
 * ramp
 ***********************************************************************/

inline float ramp_simd(float * dst, float current, float * m_slopes, float m_slope_step, uint n)
{
    assert(n % 8 == 0);

    __m128 val =_mm_set_ps1(current);
    const __m128 slopes = *((__m128 *)m_slopes);
    const __m128 slope_step = _mm_set_ps1(m_slope_step);
    __m128 *out = (__m128*) dst;

    n /= 8;
    val = _mm_add_ps(val, slopes);
    for (uint i = 0; i != n; ++i)
    {
        *out = val;
        val = _mm_add_ps(val, slope_step);
        ++out;

        *out = val;
        val = _mm_add_ps(val, slope_step);
        ++out;
    }

    return current + 2 * n * m_slope_step;
}


/***********************************************************************
 * basic arithmetic functions, ramping
 ***********************************************************************/

inline void times_vec_ramp_simd(float * dest, const float * src1, float src2, float slope, uint n)
{
    __m128 factor = _mm_set_ps(src2, src2 + slope, src2 + slope+slope, src2 + slope+slope+slope);
    __m128 offset = _mm_set_ps1((slope+slope)+(slope+slope));

    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src1;

    n /= 8;
    for (uint i = 0; i != n; ++i)
    {
        *out++ = _mm_mul_ps(*in++, factor);
        factor = _mm_add_ps(factor, offset);
        *out++ = _mm_mul_ps(*in++, factor);
        factor = _mm_add_ps(factor, offset);
    }
}

inline void times_vec2_ramp_simd(float * dest0, const float * src01, float src02, float slope0,
                                 float * dest1, const float * src11, float src12, float slope1,
                                 uint n)
{
    /* checked: faster than separate loops (at least on x86_86, core2 duo)
     * */

    __m128 factor0 = _mm_set_ps(src02, src02 + slope0, src02 + slope0+slope0, src02 + slope0+slope0+slope0);
    __m128 offset0 = _mm_set_ps1((slope0+slope0)+(slope0+slope0));

    __m128 factor1 = _mm_set_ps(src12, src12 + slope1, src12 + slope1+slope1, src12 + slope1+slope1+slope1);
    __m128 offset1 = _mm_set_ps1((slope1+slope1)+(slope1+slope1));

    __m128 *out0 = (__m128*) dest0;
    const __m128 *in0 = (__m128*) src01;

    __m128 *out1 = (__m128*) dest1;
    const __m128 *in1 = (__m128*) src11;

    /* check: which unrolling makes sense? 8 seems to be more efficient on x86_64/core2 duo */
    n /= 8;
    do
    {
        *out0++ = _mm_mul_ps(*in0++, factor0);
        factor0 = _mm_add_ps(factor0, offset0);

        *out1++ = _mm_mul_ps(*in1++, factor1);
        factor1 = _mm_add_ps(factor1, offset1);

        *out0++ = _mm_mul_ps(*in0++, factor0);
        factor0 = _mm_add_ps(factor0, offset0);

        *out1++ = _mm_mul_ps(*in1++, factor1);
        factor1 = _mm_add_ps(factor1, offset1);
    }
    while (--n);
}


/***********************************************************************
 * muladd operations
 ***********************************************************************/

inline void muladd_vec_simd(float * dest, const float * src, const float mul, const float add, uint n)
{
    VEC_VECSCALSCAL_SIMD_N(muladd_vec_simd_mp, samples_per_loop, dest, src, mul, add, n);
}

inline void muladd_vec_simd(float * dest, const float * src, const float * mul, const float add, uint n)
{
    VEC_VECVECSCAL_SIMD_N(muladd_vec_simd_mp, samples_per_loop, dest, src, mul, add, n);
}

inline void muladd_vec_simd(float * dest, const float * src, const float mul, const float * add, uint n)
{
    VEC_VECSCALVEC_SIMD_N(muladd_vec_simd_mp, samples_per_loop, dest, src, mul, add, n);
}

inline void muladd_vec_simd(float * dest, const float * src, const float * mul, const float * add, uint n)
{
    VEC_VECVECVEC_SIMD_N(muladd_vec_simd_mp, samples_per_loop, dest, src, mul, add, n);
}


/***********************************************************************
 * mixing operations
 ***********************************************************************/

inline void mix_vec_simd(float * dest, const float * src0, const float mul0, const float * src1,
                         const float mul1, uint n)
{
    n = n / samples_per_loop;

    do
    {
        mix_vec_simd_mp<samples_per_loop>(dest, src0, mul0, src1, mul1);
        dest += samples_per_loop;
        src0 += samples_per_loop;
        src1 += samples_per_loop;
    }
    while (--n);
}



} /* namespace nova */


#undef VEC_VEC_SIMD_N

#undef VEC_VECVEC_SIMD_N
#undef VEC_VECSCAL_SIMD_N
#undef VEC_SCALVEC_SIMD_N

#undef VEC_VECVECVEC_SIMD_N
#undef VEC_VECVECSCAL_SIMD_N
#undef VEC_VECSCALVEC_SIMD_N
#undef VEC_VECSCALSCAL_SIMD_N

#undef UNARY_ARITHMETIC_OPERATION_WRAPPER
#undef COPY_OPERATION_WRAPPER

#endif /* SIMD_SSE_HPP */
