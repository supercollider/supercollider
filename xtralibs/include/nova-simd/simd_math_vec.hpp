//  simdfied mathematical functions
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

#ifndef SIMD_MATH_VEC_HPP
#define SIMD_MATH_VEC_HPP

#include <cmath>

#include "libsimdmath/lib/sincosf4.h"
#include "libsimdmath/lib/asinf4.h"
#include "libsimdmath/lib/atanf4.h"
#include "libsimdmath/lib/powf4.h"

#ifdef __SSE__
#include "simd_utils.hpp"
#endif

#define vec_xor(a,b)           ((a)^(b))

#define WRAP_MATH_FUNCTION(name, function)                              \
    template <>                                                         \
    inline void name##_vec_simd<float>(float * out, const float * in, unsigned int n) \
    {                                                                   \
        unsigned int loops = n/4;                                       \
        do {                                                            \
            function(out, in);                                          \
            in += 4;                                                    \
            out += 4;                                                   \
        } while (--loops);                                              \
    }                                                                   \
    template <unsigned int n>                                           \
    inline void name##_vec_simd_mp(float * out, const float * in)       \
    {                                                                   \
        name##_vec_simd(out, in, n);                                    \
    }                                                                   \
    template <unsigned int n>                                           \
    inline void name##_vec_simd(float * out, const float * in)          \
    {                                                                   \
        name##_vec_simd(out, in, n);                                    \
    }

#define WRAP_MATH_FUNCTION_BINARY(name, function)                       \
    template <>                                                         \
    inline void name##_vec_simd<float>(float * out, const float * in0, const float * in1, unsigned int n) \
    {                                                                   \
        unsigned int loops = n/4;                                       \
        do {                                                            \
            function(out, in0, in1);                                    \
            in0 += 4;                                                   \
            in1 += 4;                                                   \
            out += 4;                                                   \
        } while (--loops);                                              \
    }                                                                   \
                                                                        \
    template <>                                                         \
    inline void name##_vec_simd<float>(float * out, const float * in0, const float in1, unsigned int n) \
    {                                                                   \
        unsigned int loops = n/4;                                       \
        do {                                                            \
            function(out, in0, in1);                                    \
            in0 += 4;                                                   \
            out += 4;                                                   \
        } while (--loops);                                              \
    }                                                                   \
                                                                        \
    template <>                                                         \
    inline void name##_vec_simd<float>(float * out, const float in0, const float * in1, unsigned int n) \
    {                                                                   \
        unsigned int loops = n/4;                                       \
        do {                                                            \
            function(out, in0, in1);                                    \
            in1 += 4;                                                   \
            out += 4;                                                   \
        } while (--loops);                                              \
    }

namespace nova
{

namespace detail
{

inline void sin4(float * out, const float * in)
{
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _sinf4(*ip);
}

inline void cos4(float * out, const float * in)
{
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _cosf4(*ip);
}

inline void tan4(float * out, const float * in)
{
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _tanf4(*ip);
}

inline void asin4(float * out, const float * in)
{
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _asinf4(*ip);
}

inline void acos4(float * out, const float * in)
{
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _acosf4(*ip);
}

inline void atan4(float * out, const float * in)
{
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _atanf4(*ip);
}

inline void log4(float * out, const float * in)
{
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _logf4(*ip);
}

inline void log2_4(float * out, const float * in)
{
    const float rlog2 = 1.f/std::log(2.f);
    const vec_float4 rlog2v= {rlog2, rlog2, rlog2, rlog2};
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _logf4(*ip) * rlog2v;
}

inline void log10_4(float * out, const float * in)
{
    const float rlog10 = 1.f/std::log(10.f);
    const vec_float4 rlog10v= {rlog10, rlog10, rlog10, rlog10};
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _logf4(*ip) * rlog10v;
}

inline void exp4(float * out, const float * in)
{
    const vec_float4 * ip = (const vec_float4*)in;
    vec_float4 * op = (vec_float4*)out;
    *op = _expf4(*ip);
}

/** signed sqrt
 *  if (in > 0)
 *      return sqrt(in);
 *  else
 *      return -sqrt(-in);
 *
 */
inline void ssqrt4(float * out, const float * in)
{
    const vec_float4 * ip = (const vec_float4*)in;

    const vec_int4   sign_in = _signf4(*ip);
    const vec_float4 abs_in = (vec_float4)vec_xor((vec_int4)*ip, sign_in);

    vec_float4 * op = (vec_float4*)out;

    vec_float4 result = _sqrtf4(abs_in);

    *op = (vec_float4)(sign_in | (vec_int4)result);
}


inline void tanh4(float * out, const float * in)
{
    /* this order of computation (large->small->medium) seems to be the most efficient */

    const vec_float4 arg = *(const vec_float4*)in;
    const vec_int4   sign_arg = _signf4(arg);
    const vec_float4 abs_arg = (vec_float4)vec_xor((vec_int4)arg, sign_arg);
#ifdef __SSE__
    const vec_float4 one = detail::gen_one();
#else
    const vec_float4 one = {1.f, 1.f, 1.f, 1.f};
#endif
    const vec_float4 two = {2.f, 2.f, 2.f, 2.f};
    const vec_float4 maxlogf_2 = {22.f, 22.f, 22.f, 22.f};
    const vec_float4 limit_small = {0.625f, 0.625f, 0.625f, 0.625f};

    /* large values */
    const vec_int4   abs_big = (vec_int4)VEC_GT(abs_arg, maxlogf_2);
    const vec_float4 result_limit_abs = one;

    /* small values */
    const vec_float4 f1 = {-5.70498872745e-3, -5.70498872745e-3, -5.70498872745e-3, -5.70498872745e-3};
    const vec_float4 f2 = { 2.06390887954e-2,  2.06390887954e-2,  2.06390887954e-2,  2.06390887954e-2};
    const vec_float4 f3 = {-5.37397155531e-2, -5.37397155531e-2, -5.37397155531e-2, -5.37397155531e-2};
    const vec_float4 f4 = { 1.33314422036e-1,  1.33314422036e-1,  1.33314422036e-1,  1.33314422036e-1};
    const vec_float4 f5 = {-3.33332819422e-1, -3.33332819422e-1, -3.33332819422e-1, -3.33332819422e-1};

    const vec_float4 arg_sqr = abs_arg * abs_arg;
    const vec_float4 result_small = ((((f1 * arg_sqr
                                        + f2) * arg_sqr
                                       + f3) * arg_sqr
                                      + f4) * arg_sqr
                                     + f5) * arg_sqr * arg
        + arg;

    const vec_int4 abs_small = (vec_int4)VEC_LT(abs_arg, limit_small);

    /* medium values */
    const vec_float4 result_medium_abs = one - two / (_expf4(abs_arg + abs_arg) + one);

    /* select from large and medium branches and set sign */
    const vec_float4 result_lm_abs = vec_sel(result_medium_abs, result_limit_abs, abs_big);
    const vec_float4 result_lm = (vec_float4) vec_or((vec_int4)result_lm_abs, sign_arg);

    const vec_float4 result = vec_sel(result_lm, result_small, abs_small);

    vec_float4 * op = (vec_float4*)out;
    *op = result;
}

} /* namespace detail */

WRAP_MATH_FUNCTION(sin, detail::sin4)
WRAP_MATH_FUNCTION(cos, detail::cos4)
WRAP_MATH_FUNCTION(tan, detail::tan4)
WRAP_MATH_FUNCTION(asin, detail::asin4)
WRAP_MATH_FUNCTION(acos, detail::acos4)
WRAP_MATH_FUNCTION(atan, detail::atan4)
WRAP_MATH_FUNCTION(log, detail::log4)
WRAP_MATH_FUNCTION(log2, detail::log2_4)
WRAP_MATH_FUNCTION(log10, detail::log10_4)
WRAP_MATH_FUNCTION(exp, detail::exp4)
WRAP_MATH_FUNCTION(ssqrt, detail::ssqrt4)
WRAP_MATH_FUNCTION(tanh, detail::tanh4)

namespace detail
{

/** pow */
/* @{ */
inline void pow4(float * out, const float * in1, const float * in2)
{
    const vec_float4 * ip1 = (const vec_float4*)in1;
    const vec_float4 * ip2 = (const vec_float4*)in2;
    vec_float4 * op = (vec_float4*)out;
    *op = _powf4(*ip1, *ip2);
}

inline void pow4(float * out, const float * in1, const float in2)
{
    const vec_float4 ip2 = {in2, in2, in2, in2};
    pow4(out, in1, (float*)&ip2);
}

inline void pow4(float * out, const float in1, const float * in2)
{
    const vec_float4 ip1 = {in1, in1, in1, in1};
    pow4(out, (float*)&ip1, in2);
}
/* @} */


/** signed pow:
 *
 *  if (in1 > 0)
 *      return std::pow(in1, in2);
 *  else
 *      return -std::pow(-in1, in2);
 *
 * */
/* @{ */
inline void spow4(float * out, const float * in1, const float * in2)
{
    const vec_float4 * ip1 = (const vec_float4*)in1;
    const vec_float4 * ip2 = (const vec_float4*)in2;

    const vec_int4   sign_in1 = _signf4(*ip1);
    const vec_float4 abs_in1 = (vec_float4)vec_xor((vec_int4)*ip1, sign_in1);

    vec_float4 * op = (vec_float4*)out;

    vec_float4 result = _powf4(abs_in1, *ip2);

    *op = (vec_float4)(sign_in1 | (vec_int4)result);
}

inline void spow4(float * out, const float * in1, const float in2)
{
    const vec_float4 ip2 = {in2, in2, in2, in2};
    spow4(out, in1, (float*)&ip2);
}

inline void spow4(float * out, const float in1, const float * in2)
{
    const vec_float4 ip1 = {in1, in1, in1, in1};
    spow4(out, (float*)&ip1, in2);
}
/* @} */

} /* namespace detail */

WRAP_MATH_FUNCTION_BINARY(pow, detail::pow4)
WRAP_MATH_FUNCTION_BINARY(spow, detail::spow4)


} /* namespace nova */

#undef WRAP_MATH_FUNCTION
#undef WRAP_MATH_FUNCTION_BINARY

#endif /* SIMD_MATH_VEC_HPP */
