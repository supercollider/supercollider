//  fundamental arithmetic functions, coded in sse with intrinsics and
//  using template metaprogramming
//  make sure to compile with loop unrolling or it may be extremely slow!
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

#ifndef _SIMD_SSE_MP_HPP
#define _SIMD_SSE_MP_HPP

#include <cassert>
#include <xmmintrin.h>

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

#include "../simd/simd_utils.hpp"

namespace nova
{

namespace detail
{
typedef union
{
    int l;
    float f;
} bitmask_union;
} /* namespace nova */

/***********************************************************************
 * basic add methods
 ***********************************************************************/

/** \brief same as addvec_8 */
template <int n>
always_inline void addvec_simd_mp(float *dst, const float *src)
{
    __m128 *out = (__m128*) dst;
    const __m128 *in = (__m128*) src;
    *out = _mm_add_ps(*in, *out);

    addvec_simd_mp<n-4>((float*)++out, (const float*)++in);
}

template <>
always_inline void addvec_simd_mp<0>(float *dst, const float *src)
{}

template <int n>
always_inline void addvec_simd_mp(float *dst, const float *src, const float *src2)
{
    __m128 *out = (__m128*) dst;
    const __m128 *in = (__m128*) src;
    const __m128 *in2 = (__m128*) src2;
    *out = _mm_add_ps(_mm_add_ps(*in, *in2), *out);

    addvec_simd_mp<n-4>((float*)++out, (const float*)++in, (const float*)++in2);
}

template <>
always_inline void addvec_simd_mp<0>(float *dst, const float *src, const float *src2)
{}

template <int n>
always_inline void addvec_simd_mp(float *dst, const float *src, const float *src2, const float *src3)
{
    __m128 *out = (__m128*) dst;
    const __m128 *in = (__m128*) src;
    const __m128 *in2 = (__m128*) src2;
    const __m128 *in3 = (__m128*) src3;
    *out = _mm_add_ps(_mm_add_ps(*in, *in2), _mm_add_ps(*out,*in3));

    addvec_simd_mp<n-4>((float*)++out, (const float*)++in, (const float*)++in2, (const float*)++in3);
}

template <>
always_inline void addvec_simd_mp<0>(float *dst, const float *src, const float *src2, const float *src3)
{}

/***********************************************************************
 * denormal bashing copy/add functions methods
 ***********************************************************************/

namespace detail
{
template <int n>
always_inline void testcopyvec_simd_mp_iteration(float *dst, const float *src, __m128 const & bitmask, __m128 const & zero)
{
    __m128 *out = (__m128*) dst;
    const __m128 *in = (__m128*) src;

    __m128 xmm0 = *in;
    __m128 xmm1 = xmm0;

    xmm1 = _mm_and_ps(xmm1, bitmask);
    __m128 xmm2 = xmm1;
    xmm1 = _mm_cmpneq_ps(xmm1, bitmask);
    xmm2 = _mm_cmpneq_ps(xmm2, zero);

    xmm1 = _mm_and_ps(xmm1, xmm2);
    xmm0 = _mm_and_ps(xmm1, xmm0);

    *out = xmm0;

    testcopyvec_simd_mp_iteration<n-4>((float*)++out, (const float*)++in, bitmask, zero);
}

template <>
always_inline void testcopyvec_simd_mp_iteration<0>(float *dst, const float *src, __m128 const & bitmask, __m128 const & zero)
{}
}

template <int n>
always_inline void testcopyvec_simd_mp(float *dst, const float *src)
{
    detail::bitmask_union m;
    m.l = 0x60000000;
    const __m128 bitmask = _mm_set1_ps(m.f);
    const __m128 zero = _mm_setzero_ps();

    detail::testcopyvec_simd_mp_iteration<n>(dst, src, bitmask, zero);
}

namespace detail
{

template <int n>
always_inline void testaddvec_simd_mp_iteration(float *dst, const float *src, __m128 const & bitmask, __m128 const & zero)
{
    __m128 *out = (__m128*) dst;
    const __m128 *in = (__m128*) src;

    __m128 data = *in;
    __m128 bitmasked = _mm_and_ps(data, bitmask);
    __m128 bitmask1 = _mm_cmpneq_ps(bitmasked, bitmask);
    __m128 bitmask2 = _mm_cmpneq_ps(bitmasked, zero);
    __m128 bitmask3 = _mm_and_ps(bitmask1, bitmask2);
    __m128 result = _mm_and_ps(bitmask3, data);
    *out = _mm_add_ps(result, out[0]);

    testaddvec_simd_mp_iteration<n-4>((float *)++out, (const float *)++in, bitmask, zero);
}

template <>
always_inline void testaddvec_simd_mp_iteration<0>(float *dst, const float *src, __m128 const & bitmask, __m128 const & zero)
{}

} /* namespace detail */

template <int n>
always_inline void testaddvec_simd_mp(float *dst, const float *src)
{
    detail::bitmask_union m;
    m.l = 0x60000000;
    const __m128 bitmask = _mm_set1_ps(m.f);
    const __m128 zero = _mm_setzero_ps();

    detail::testaddvec_simd_mp_iteration<n>(dst, src, bitmask, zero);
}

template <>
always_inline void testaddvec_simd_mp<0>(float *dst, const float *src)
{}

/***********************************************************************
 * basic arithmetic functions (vector/vector)
 ***********************************************************************/

template <int n>
always_inline void plus_vec_simd_mp(float * dest, const float * src1, const float * src2, const float * src3)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in1 = (__m128*) src1;
    const __m128 *in2 = (__m128*) src2;
    const __m128 *in3 = (__m128*) src3;

    *out = _mm_add_ps(_mm_add_ps(*in1, *in2), *in3);

    plus_vec_simd_mp<n-4>((float*) ++out, (const float *)++in1, (const float *)++in2, (const float *)++in3);
}

template <>
always_inline void plus_vec_simd_mp<0>(float * dest, const float * src1, const float * src2, const float * src3)
{}

template <int n>
always_inline void plus_vec_simd_mp(float * dest, const float * src1, const float * src2, const float * src3, const float * src4)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in1 = (__m128*) src1;
    const __m128 *in2 = (__m128*) src2;
    const __m128 *in3 = (__m128*) src3;
    const __m128 *in4 = (__m128*) src4;

    *out = _mm_add_ps(_mm_add_ps(*in1, *in2), _mm_add_ps(*in3, *in4));

    plus_vec_simd_mp<n-4>((float*) ++out, (const float *)++in1, (const float *)++in2,
                          (const float *)++in3, (const float *)++in4);
}

template <>
always_inline void plus_vec_simd_mp<0>(float * dest, const float * src1, const float * src2, const float * src3, const float * src4)
{}



/***********************************************************************
 * unary vector operations
 ***********************************************************************/


/** \brief squares content of src to dest */
template <int n>
always_inline void square_vec_simd_mp(float * dest, const float * src)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;

    *out = _mm_mul_ps(*in, *in);

    square_vec_simd_mp<n-4>((float*)++out, (const float *)++in);
}

template <>
always_inline void square_vec_simd_mp<0>(float * dest, const float * src)
{}


/** \brief cubes content of src to dest */
template <int n>
always_inline void cube_vec_simd_mp(float * dest, const float * src)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;

    *out = _mm_mul_ps(*in, _mm_mul_ps(*in, *in));

    cube_vec_simd_mp<n-4>((float*)++out, (const float *)++in);
}

template <>
always_inline void cube_vec_simd_mp<0>(float * dest, const float * src)
{}


/** \brief returns the square root of src to dest */
template <int n>
always_inline void sqrt_vec_simd_mp(float * dest, const float * src)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;

    *out = _mm_sqrt_ps(*in);

    sqrt_vec_simd_mp<n-4>((float *) ++out, (const float *) ++in);
}

template <>
always_inline void sqrt_vec_simd_mp<0>(float * dest, const float * src)
{}


/** \brief returns the reciprocal square root of src to dest, n has to be a multiple of 8 */
template <int n>
always_inline void rsqrt_vec_simd_mp(float * dest, const float * src)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;

    *out = _mm_rsqrt_ps(*in);

    rsqrt_vec_simd_mp<n-4>((float *)++out , (const float *)++in);
}

template <>
always_inline void rsqrt_vec_simd_mp<0>(float * dest, const float * src)
{}


/** \brief returns the reciprocal of src to dest, n has to be a multiple of 8 */
template <int n>
always_inline void rcp_vec_simd_mp(float * dest, const float * src)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;

    *out = _mm_rcp_ps(*in);

    rcp_vec_simd_mp<n-4>((float *) ++out, (const float *) ++in);
}

template <>
always_inline void rcp_vec_simd_mp<0>(float * dest, const float * src)
{}

namespace detail
{
template <int n>
always_inline void abs_vec_simd_mp_iteration(float * dest, const float * src, __m128 const & bitmask)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;

    *out = _mm_and_ps(*in, bitmask);

    abs_vec_simd_mp_iteration<n-4>((float *) ++out, (const float *) ++in, bitmask);
}

template <>
always_inline void abs_vec_simd_mp_iteration<0>(float * dest, const float * src, __m128 const & bitmask)
{}

} /* namespace detail */

template <int n>
always_inline void abs_vec_simd_mp(float * dest, const float * src)
{
    const __m128 bitmask = (__m128)detail::gen_abs_mask();

    detail::abs_vec_simd_mp_iteration<n>(dest, src, bitmask);
}

/** \brief return the sign of src to dest
 *  */
namespace detail
{
template <int n>
always_inline void sgn_vec_simd_mp_iteration(float * dest, const float * src, __m128 const & bitmask, __m128 const & zero, __m128 const & one)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;

    __m128 data = *in;
    __m128 nonzero = _mm_cmpneq_ps(data, zero);
    __m128 sign = _mm_and_ps(data, bitmask);
    __m128 abs = _mm_and_ps(nonzero, one);
    *out = _mm_or_ps(sign, abs);

    sgn_vec_simd_mp_iteration<n-4>((float*)++out, (const float *) ++in, bitmask, zero, one);
}

template <>
always_inline void sgn_vec_simd_mp_iteration<0>(float * dest, const float * src, __m128 const & bitmask, __m128 const & zero, __m128 const & one)
{}

} /* namespace detail */

template <int n>
always_inline void sgn_vec_simd_mp(float * dest, const float * src)
{
    const __m128 bitmask = (__m128)detail::gen_sign_mask();
    const __m128 zero = _mm_setzero_ps();
    const __m128 one = (__m128)detail::gen_one();

    detail::sgn_vec_simd_mp_iteration<n>(dest, src, bitmask, zero, one);
}


/***********************************************************************
 * clipping operations
 ***********************************************************************/

/** \brief clip */
template <int n>
always_inline void clip_vec_simd_mp(float * dest, const float * src, const float * low, const float * high)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;
    const __m128 *lo = (__m128*) low;
    const __m128 *hi = (__m128*) high;

    *out = _mm_min_ps(_mm_max_ps(*in, *lo), *hi) ;

    clip_vec_simd_mp<n-4>(dest + 4, src + 4, low + 4, high + 4);
}

template <>
always_inline void clip_vec_simd_mp<0>(float * dest, const float * src, const float * low, const float * high)
{}

template <int n>
always_inline void clip_vec_simd_mp(float * dest, const float * src, float low, const float * high)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;
    const __m128 lo = _mm_set1_ps(low);
    const __m128 *hi = (__m128*) high;

    *out = _mm_min_ps(_mm_max_ps(*in, lo), *hi) ;

    clip_vec_simd_mp<n-4>(dest + 4, src + 4, low + 4, high + 4);
}

template <>
always_inline void clip_vec_simd_mp<0>(float * dest, const float * src, float low, const float * high)
{}

template <int n>
always_inline void clip_vec_simd_mp(float * dest, const float * src, const float * low, float high)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;
    const __m128 *lo = (__m128*) low;
    const __m128 hi = _mm_set1_ps(high);

    *out = _mm_min_ps(_mm_max_ps(*in, *lo), hi) ;

    clip_vec_simd_mp<n-4>(dest + 4, src + 4, low + 4, high + 4);
}

template <>
always_inline void clip_vec_simd_mp<0>(float * dest, const float * src, const float * low, float high)
{}

/** \brief clip */
template <int n>
always_inline void clip_vec_simd_mp(float * dest, const float * src, float low, float high)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;
    const __m128 lo = _mm_set1_ps(low);
    const __m128 hi = _mm_set1_ps(high);

    *out = _mm_min_ps(_mm_max_ps(*in, lo), hi);

    clip_vec_simd_mp<n-4>((float *) ++out, (const float *) ++in, low, high);
}

template <>
always_inline void clip_vec_simd_mp<0>(float * dest, const float * src, float low, float high)
{}


/***********************************************************************
 * peak finding operations
 ***********************************************************************/

namespace detail
{
template <int n>
always_inline void abspeak_simd_mp_iteration(const float * src, float * accum_peak, __m128 const & bitmask)
{
    const __m128 *in = (__m128*) src;
    __m128 * peak = (__m128*) accum_peak;

    *peak = _mm_max_ps(_mm_and_ps(*in, bitmask), *peak);

    abspeak_simd_mp_iteration<n-4>((const float*)++in, accum_peak, bitmask);
}

template <>
always_inline void abspeak_simd_mp_iteration<0>(const float * src, float * accum_peak, __m128 const & bitmask)
{}

} /* namespace detail */

template <int n>
always_inline void abspeak_simd_mp(const float * src, float & peak)
{
    __m128 maximum = _mm_load_ss(&peak);

    const __m128 bitmask = (__m128)detail::gen_abs_mask();

    detail::abspeak_simd_mp_iteration<n>(src, (float*) &maximum, bitmask);

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

namespace detail
{
template <int n>
always_inline void rms_simd_mp_iteration(const float * src, float * accum_rms)
{
    const __m128 *in = (__m128*) src;
    __m128 *sum = (__m128*) accum_rms;

    *sum = _mm_add_ps(*sum, _mm_mul_ps(*in, *in));

    rms_simd_mp_iteration<n-4>((const float*)++in, accum_rms);
}

template <>
always_inline void rms_simd_mp_iteration<0>(const float * src, float * accum_rms)
{}

} /* namespace detail */

template <int n>
always_inline void rms_simd_mp(const float * src, float & rms)
{
    __m128 sum = _mm_load_ss(&rms);

    detail::rms_simd_mp_iteration<n>(src, (float*) &sum);


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

namespace detail
{
template <int n>
always_inline void peak_rms_simd_mp_iteration(const float * src, float * accum_peak, float * accum_rms, __m128 const & bitmask)
{
    const __m128 *in = (__m128*) src;
    __m128 *sum = (__m128*) accum_rms;
    __m128 *peak = (__m128*) accum_peak;

    *peak = _mm_max_ps(_mm_and_ps(*in, bitmask), *peak);
    *sum = _mm_add_ps(*sum, _mm_mul_ps(*in, *in));

    peak_rms_simd_mp_iteration<n-4>(src + 4, accum_peak, accum_rms, bitmask);
}

template <>
always_inline void peak_rms_simd_mp_iteration<0>(const float * src, float * accum_peak, float * accum_rms, __m128 const & bitmask)
{}

} /* namespace detail */

template <int n>
always_inline void peak_rms_simd_mp(const float * src, float & peak, float & rms)
{
    __m128 maximum = _mm_load_ss(&peak);
    __m128 sum = _mm_load_ss(&rms);

    const __m128 bitmask = (__m128)detail::gen_abs_mask();

    detail::peak_rms_simd_mp_iteration<n>(src, (float*) &maximum, (float*) &sum, bitmask);

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

template <int n>
always_inline void ramp_simd_mp(float * dst, const float * last, float * increment)
{
    __m128 *out = (__m128*) dst;
    const __m128 *prev = (__m128*) last;

    const __m128 inc = _mm_set1_ps( *increment );

    __m128 next = _mm_add_ps(*prev, inc);
    *out = next;

    ramp_simd_mp<n-4>((float*)++out, &next, increment);
}

template <>
always_inline void ramp_simd_mp<0>(float * dst, const float * last, float * increment)
{}

template <int n>
always_inline void vol_ramp_simd_mp(float * dst, const float * src, const float * last, float * increment)
{
    __m128 *out = (__m128*) dst;
    const __m128 *prev = (__m128*) last;
    const __m128 *in = (__m128*) src;

    const __m128 inc = _mm_set1_ps( *increment );

    __m128 next_factor = _mm_add_ps(*prev, inc);
    *out = _mm_mul_ps(next_factor, *in);

    ramp_simd_mp<n-4>((float*)++in, (const float*)++out, &next_factor, increment);
}

template <>
always_inline void vol_ramp_simd_mp<0>(float * dst, const float * src, const float * last, float * increment)
{}


/***********************************************************************
 * muladd operations
 ***********************************************************************/

template <int n>
always_inline void muladd_vec_simd_mp(float * dest, const float * src, const float mul, const float add)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;

    *out = _mm_add_ps(_mm_mul_ps(*in, _mm_set_ps1(mul)),  _mm_set_ps1(add));

    muladd_vec_simd_mp<n-4>((float*)++out, (const float*)++in, mul, add);
}

template <>
always_inline void muladd_vec_simd_mp<0>(float * dest, const float * src, const float mul, const float add)
{}

template <int n>
always_inline void muladd_vec_simd_mp(float * dest, const float * src, const float * mul, const float add)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;
    const __m128 *ml = (__m128*) mul;

    *out = _mm_add_ps(_mm_mul_ps(*in, *ml), _mm_set_ps1(add));

    muladd_vec_simd_mp<n-4>((float*)++out, (const float*)++in, (const float*)++ml, add);
}

template <>
always_inline void muladd_vec_simd_mp<0>(float * dest, const float * src, const float * mul, const float add)
{}

template <int n>
always_inline void muladd_vec_simd_mp(float * dest, const float * src, const float mul, const float * add)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;
    const __m128 *ad = (__m128*) add;

    *out = _mm_add_ps(_mm_mul_ps(*in, _mm_set_ps1(mul)), *ad);

    muladd_vec_simd_mp<n-4>((float*)++out, (const float*)++in, mul, (const float*)++ad);
}

template <>
always_inline void muladd_vec_simd_mp<0>(float * dest, const float * src, const float mul, const float * add)
{}

template <int n>
always_inline void muladd_vec_simd_mp(float * dest, const float * src, const float * mul, const float * add)
{
    __m128 *out = (__m128*) dest;
    const __m128 *in = (__m128*) src;
    const __m128 *ad = (__m128*) add;
    const __m128 *ml = (__m128*) mul;

    *out = _mm_add_ps(_mm_mul_ps(*in, *ml), *ad);

    muladd_vec_simd_mp<n-4>((float*)++out, (const float*)++in, (const float*)++ml, (const float*)++ad);
}

template <>
always_inline void muladd_vec_simd_mp<0>(float * dest, const float * src, const float * mul, const float * add)
{}


/***********************************************************************
 * mixing operations
 ***********************************************************************/

namespace detail
{
template <int n>
always_inline void mix_vec_simd_mp_iteration(float *dst, const float * src0, __m128 const & mul0,
                                             const float * src1, __m128 const & mul1)
{
    __m128 *out = (__m128*) dst;
    const __m128 *in0 = (__m128*) src0;
    const __m128 *in1 = (__m128*) src1;

    *out = _mm_add_ps(_mm_mul_ps(*in0, mul0), _mm_mul_ps(*in1, mul1));
    mix_vec_simd_mp_iteration<n-4>((float*)++out, (float*)++in0, mul0, (float*)++in1, mul1);
}

template <>
always_inline void mix_vec_simd_mp_iteration<0>(float *dst, const float * src0, __m128 const & mul0,
                                                 const float * src1, __m128 const & mul1)
{}

} /* namespace detail */

template <int n>
always_inline void mix_vec_simd_mp(float * dest, const float * src0, const float mul0,
                                   const float * src1, const float mul1)
{
    detail::mix_vec_simd_mp_iteration<n>(dest, src0, _mm_set_ps1(mul0), src1, _mm_set_ps1(mul1));
}



} /* namespace nova */

#undef BINARY_COMPARING_SIMD_WRAPPER
#undef always_inline
#endif /* _SIMD_SSE_MP_HPP */

