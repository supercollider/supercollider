//  sse vector class
//
//  Copyright (C) 2010 Tim Blechmann
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

#ifndef VEC_SSE_HPP
#define VEC_SSE_HPP

#include <xmmintrin.h>

#ifdef __SSE2__
#include <emmintrin.h>
#endif

#ifdef __SSE4_1__
#include <smmintrin.h>
#endif

#ifndef NO_GPL3_CODE
#include "libsimdmath/lib/sincosf4.h"
#include "libsimdmath/lib/asinf4.h"
#include "libsimdmath/lib/atanf4.h"
#include "libsimdmath/lib/logf4.h"
#include "libsimdmath/lib/expf4.h"
#include "libsimdmath/lib/powf4.h"

namespace {

inline vec_float4 _log2f4(vec_float4 arg)
{
    const float rlog2 = 1.f/std::log(2.f);
    const vec_float4 rlog2v= {rlog2, rlog2, rlog2, rlog2};
    return _logf4(arg) * rlog2v;
}

inline vec_float4 _log10f4(vec_float4 arg)
{
    const float rlog10 = 1.f/std::log(10.f);
    const vec_float4 rlog10v= {rlog10, rlog10, rlog10, rlog10};
    return _logf4(arg) * rlog10v;
}

#define vec_xor(a,b)           ((a)^(b))

inline vec_float4 _signed_sqrtf4(vec_float4 arg)
{
    const vec_int4   sign_in = _signf4(arg);
    const vec_float4 abs_in = (vec_float4)vec_xor((vec_int4)arg, sign_in);

    vec_float4 result = _sqrtf4(abs_in);

    return (vec_float4)(sign_in | (vec_int4)result);
}

inline vec_float4 _tanhf4(vec_float4 arg)
{
    /* this order of computation (large->small->medium) seems to be the most efficient */

    const vec_int4   sign_arg = _signf4(arg);
    const vec_float4 abs_arg = (vec_float4)vec_xor((vec_int4)arg, sign_arg);
    const vec_float4 one = {1.f, 1.f, 1.f, 1.f};
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

    return result;
}

inline vec_float4 _signed_powf4(vec_float4 arg1, vec_float4 arg2)
{
    const vec_int4   sign_in1 = _signf4(arg1);
    const vec_float4 abs_in1 = (vec_float4)vec_xor((vec_int4)arg1, sign_in1);

    vec_float4 result = _powf4(abs_in1, arg2);

    return (vec_float4)(sign_in1 | (vec_int4)result);
}

#undef vec_xor

}

#endif

#include "detail/vec_math.hpp"

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

namespace nova
{

template <>
struct vec<float>
{
    typedef __m128 internal_vector_type;

#ifdef __SSE2__
    static inline __m128 gen_sign_mask(void)
    {
        __m128i x = _mm_setzero_si128();
        __m128i ones = _mm_cmpeq_epi32(x, x);
        return (__m128)_mm_slli_epi32 (_mm_srli_epi32(ones, 31), 31);
    }

    static inline __m128 gen_abs_mask(void)
    {
        __m128i x = _mm_setzero_si128();
        __m128i ones = _mm_cmpeq_epi32(x, x);
        return (__m128)_mm_srli_epi32 (_mm_slli_epi32(ones, 1), 1);
    }

    static inline __m128 gen_one(void)
    {
        __m128i x = _mm_setzero_si128();
        __m128i ones = _mm_cmpeq_epi32(x, x);
        return (__m128)_mm_slli_epi32 (_mm_srli_epi32(ones, 25), 23);
    }

    static inline __m128 gen_05(void)
    {
        __m128i x = _mm_setzero_si128();
        __m128i ones = _mm_cmpeq_epi32(x, x);
        return (__m128)_mm_slli_epi32 (_mm_srli_epi32(ones, 26), 24);
    }
#else
    /* SSE fallback */
    static inline __m128 gen_sign_mask(void)
    {
        static const int sign_mask = 0x80000000;
        float * casted = (float*)(&sign_mask);
        return _mm_set_ps1(*casted);
    }

    static inline __m128 gen_abs_mask(void)
    {
        static const int abs_mask = 0x7fffffff;
        float * casted = (float*)(&abs_mask);
        return _mm_set_ps1(*casted);
    }

    static inline __m128 gen_one(void)
    {
        return _mm_set_ps1(1.f);
    }

    static inline __m128 gen_05(void)
    {
        return _mm_set_ps1(0.5f);
    }
#endif

    static inline __m128 gen_zero(void)
    {
        volatile __m128 x;
        return _mm_xor_ps(x, x);
    }

    vec(__m128 const & arg):
        data_(arg)
    {}

public:
    static const int size = 4;
    static const int objects_per_cacheline = 64/sizeof(float);

    /* @{ */
    /** constructors */
    vec(void)
    {}

    explicit vec(float f)
    {
        set_vec(f);
    }

    vec(vec const & rhs)
    {
        data_ = rhs.data_;
    }
    /* @} */

    /* @{ */
    /** io */
    void load(const float * data)
    {
        data_ = _mm_loadu_ps(data);
    }

    void load_aligned(const float * data)
    {
        data_ = _mm_load_ps(data);
    }

    void load_first(const float * data)
    {
        data_ = _mm_load_ss(data);
    }

    void store(float * dest) const
    {
        _mm_storeu_ps(dest, data_);
    }

    void store_aligned(float * dest) const
    {
        _mm_store_ps(dest, data_);
    }

    void store_aligned_stream(float * dest) const
    {
        _mm_stream_ps(dest, data_);
    }

    void clear(void)
    {
        data_ = _mm_setzero_ps();
    }

    /* @} */

    /* @{ */
    /** element access */
    void set (std::size_t index, float value)
    {
#ifdef __SSE4_1__
        __m128 tmp = _mm_set_ss(value);

        switch (index)
        {
        case 0:
            data_ = _mm_insert_ps(data_, tmp, 0 << 4);
            break;

        case 1:
            data_ = _mm_insert_ps(data_, tmp, 1 << 4);
            break;

        case 2:
            data_ = _mm_insert_ps(data_, tmp, 2 << 4);
            break;

        case 3:
            data_ = _mm_insert_ps(data_, tmp, 3 << 4);
            break;
        }
#else
        float * data = (float*)&data_;
        data[index] = value;
#endif
    }

    void set_vec (float value)
    {
        data_ = _mm_set_ps1(value);
    }

    float set_slope(float start, float slope)
    {
        float v1 = start + slope;
        float v2 = start + slope + slope;
        float v3 = start + slope + slope + slope;
        data_ = _mm_set_ps(v3, v2, v1, start);
        return slope + slope + slope + slope;
    }

    float set_exp(float start, float curve)
    {
        float v1 = start * curve;
        float v2 = v1 * curve;
        float v3 = v2 * curve;
        data_ = _mm_set_ps(v3, v2, v1, start);
        return v3 * curve;
    }

    float get (std::size_t index)
    {
#ifdef __SSE4_1__
        union {
            int32_t i;
            float f;
        } cu;

        switch (index)
        {
        case 0:
            cu.i = _mm_extract_ps(data_, 0);
            break;

        case 1:
            cu.i = _mm_extract_ps(data_, 1);
            break;

        case 2:
            cu.i = _mm_extract_ps(data_, 2);
            break;

        case 3:
            cu.i = _mm_extract_ps(data_, 3);
            break;
        }

        return cu.f;
#else
        __m128 ret;
        switch (index)
        {
        case 0:
            ret = data_;
            break;

        case 1:
            ret = _mm_shuffle_ps(data_, data_, _MM_SHUFFLE(1, 1, 1, 1));
            break;

        case 2:
            ret = _mm_shuffle_ps(data_, data_, _MM_SHUFFLE(2, 2, 2, 2));
            break;

        case 3:
            ret = _mm_shuffle_ps(data_, data_, _MM_SHUFFLE(3, 3, 3, 3));
            break;
        }

        return _mm_cvtss_f32(ret);
#endif
    }
    /* @} */

    /* @{ */
    /** arithmetic operators */
#define OPERATOR_ASSIGNMENT(op, opcode) \
    vec & operator op(vec const & rhs) \
    { \
        data_ = opcode(data_, rhs.data_);\
        return *this;\
    }

    OPERATOR_ASSIGNMENT(+=, _mm_add_ps)
    OPERATOR_ASSIGNMENT(-=, _mm_sub_ps)
    OPERATOR_ASSIGNMENT(*=, _mm_mul_ps)
    OPERATOR_ASSIGNMENT(/=, _mm_div_ps)

#define ARITHMETIC_OPERATOR(op, opcode) \
    vec operator op(vec const & rhs) const \
    { \
        return opcode(data_, rhs.data_); \
    }

    ARITHMETIC_OPERATOR(+, _mm_add_ps)
    ARITHMETIC_OPERATOR(-, _mm_sub_ps)
    ARITHMETIC_OPERATOR(*, _mm_mul_ps)
    ARITHMETIC_OPERATOR(/, _mm_div_ps)

#define RELATIONAL_OPERATOR(op, opcode) \
    vec operator op(vec const & rhs) const \
    { \
        const __m128 one = gen_one(); \
        return _mm_and_ps(opcode(data_, rhs.data_), one); \
    }

    RELATIONAL_OPERATOR(<, _mm_cmplt_ps)
    RELATIONAL_OPERATOR(<=, _mm_cmple_ps)
    RELATIONAL_OPERATOR(>, _mm_cmpgt_ps)
    RELATIONAL_OPERATOR(>=, _mm_cmpge_ps)
    RELATIONAL_OPERATOR(==, _mm_cmpeq_ps)
    RELATIONAL_OPERATOR(!=, _mm_cmpneq_ps)

    /* @{ */
#define BITWISE_OPERATOR(op, opcode) \
    vec operator op(vec const & rhs) const \
    { \
        return opcode(data_, rhs.data_); \
    }

    BITWISE_OPERATOR(&, _mm_and_ps)
    BITWISE_OPERATOR(|, _mm_or_ps)
    BITWISE_OPERATOR(^, _mm_xor_ps)

    #define RELATIONAL_MASK_OPERATOR(op, opcode) \
    friend vec mask_##op(vec const & lhs, vec const & rhs) \
    { \
        return opcode(lhs.data_, rhs.data_); \
    }

    RELATIONAL_MASK_OPERATOR(lt, _mm_cmplt_ps)
    RELATIONAL_MASK_OPERATOR(le, _mm_cmple_ps)
    RELATIONAL_MASK_OPERATOR(gt, _mm_cmpgt_ps)
    RELATIONAL_MASK_OPERATOR(ge, _mm_cmpge_ps)
    RELATIONAL_MASK_OPERATOR(eq, _mm_cmpeq_ps)
    RELATIONAL_MASK_OPERATOR(neq, _mm_cmpneq_ps)

    friend inline vec select(vec lhs, vec rhs, vec bitmask)
    {
        /* if bitmask is set, return value in rhs, else value in lhs */
#ifdef __SSE4_1__
        return _mm_blendv_ps(rhs.data_, lhs.data_, bitmask.data_);
#else
        return _mm_or_ps(_mm_andnot_ps(bitmask.data_, lhs.data_),
                        _mm_and_ps(rhs.data_, bitmask.data_));
#endif
    }

    /* @} */

    /* @{ */
    /** unary functions */
    friend inline vec abs(vec const & arg)
    {
        return _mm_and_ps(gen_abs_mask(), arg.data_);
    }

    friend always_inline vec sign(vec const & arg)
    {
        return detail::vec_sign(arg);
    }

    friend inline vec square(vec const & arg)
    {
        return _mm_mul_ps(arg.data_, arg.data_);
    }

    friend inline vec cube(vec const & arg)
    {
        return _mm_mul_ps(arg.data_, _mm_mul_ps(arg.data_, arg.data_));
    }
    /* @} */

    /* @{ */
    /** binary functions */
    friend inline vec max_(vec const & lhs, vec const & rhs)
    {
        return _mm_max_ps(lhs.data_, rhs.data_);
    }

    friend inline vec min_(vec const & lhs, vec const & rhs)
    {
        return _mm_min_ps(lhs.data_, rhs.data_);
    }
    /* @} */

    /* @{ */
    /** rounding functions */
    friend inline vec round(vec const & arg)
    {
#ifdef __SSE4_1__
        return _mm_round_ps(arg.data_, _MM_FROUND_TO_NEAREST_INT);
#else
        return detail::vec_round_float(arg);
#endif
    }

    friend inline vec frac(vec const & arg)
    {
        return arg - floor(arg);
    }

    friend inline vec floor(vec const & arg)
    {
#ifdef __SSE4_1__
        return _mm_round_ps(arg.data_, _MM_FROUND_TO_NEG_INF);
#else
        return detail::vec_floor_float(arg);
#endif
    }

    friend inline vec ceil(vec const & arg)
    {
#ifdef __SSE4_1__
        return _mm_round_ps(arg.data_, _MM_FROUND_TO_POS_INF);
#else
        return detail::vec_ceil_float(arg);
#endif
    }
    /* @} */


    /* @{ */
    /** mathematical functions */
#ifndef NO_GPL3_CODE

#define LIBSIMDMATH_WRAPPER_UNARY(NAME)       \
    friend inline vec NAME(vec const & arg) \
    {                                   \
        return _##NAME##f4(arg.data_);  \
    }

    LIBSIMDMATH_WRAPPER_UNARY(sin)
    LIBSIMDMATH_WRAPPER_UNARY(cos)
    LIBSIMDMATH_WRAPPER_UNARY(tan)
    LIBSIMDMATH_WRAPPER_UNARY(asin)
    LIBSIMDMATH_WRAPPER_UNARY(acos)
    LIBSIMDMATH_WRAPPER_UNARY(atan)

    LIBSIMDMATH_WRAPPER_UNARY(tanh)

    LIBSIMDMATH_WRAPPER_UNARY(log)
    LIBSIMDMATH_WRAPPER_UNARY(log2)
    LIBSIMDMATH_WRAPPER_UNARY(log10)
    LIBSIMDMATH_WRAPPER_UNARY(exp)

    LIBSIMDMATH_WRAPPER_UNARY(signed_sqrt)

#define LIBSIMDMATH_WRAPPER_BINARY(NAME)                    \
    friend inline vec NAME(vec const & lhs, vec const & rhs)\
    {                                                       \
        return _##NAME##f4(lhs.data_, rhs.data_);           \
    }

    LIBSIMDMATH_WRAPPER_BINARY(pow)
    LIBSIMDMATH_WRAPPER_BINARY(signed_pow)

#undef LIBSIMDMATH_WRAPPER_UNARY
#undef LIBSIMDMATH_WRAPPER_BINARY

#else

#define APPLY_UNARY(NAME, FUNCTION)                 \
    friend inline vec NAME(vec const & arg)         \
    {                                               \
        vec ret;                                    \
        detail::apply_on_vector<float, size> ((float*)&ret.data_, (float*)&arg.data_,                \
                                                   FUNCTION);    \
        return ret;                                 \
    }

#define APPLY_BINARY(NAME, FUNCTION)                            \
    friend inline vec NAME(vec const & lhs, vec const & rhs)    \
    {                                                           \
        vec ret;                                                \
        detail::apply_on_vector<float, size> ((float*)&ret.data_,\
                                              wrap_arg_signal((float*)&lhs.data_), \
                                              wrap_arg_signal((float*)&rhs.data_),  \
                                              FUNCTION);   \
        return ret;                                 \
    }

    APPLY_UNARY(sin, detail::sin<float>)
    APPLY_UNARY(cos, detail::cos<float>)
    APPLY_UNARY(tan, detail::tan<float>)
    APPLY_UNARY(asin, detail::asin<float>)
    APPLY_UNARY(acos, detail::acos<float>)
    APPLY_UNARY(atan, detail::atan<float>)

    APPLY_UNARY(tanh, detail::tanh<float>)

    APPLY_UNARY(log, detail::log<float>)
    APPLY_UNARY(log2, detail::log2<float>)
    APPLY_UNARY(log10, detail::log10<float>)
    APPLY_UNARY(exp, detail::exp<float>)

    APPLY_UNARY(signed_sqrt, detail::signed_sqrt<float>)

    APPLY_BINARY(pow, detail::pow<float>)
    APPLY_BINARY(signed_pow, detail::signed_pow<float>)

#undef APPLY_UNARY
#undef APPLY_BINARY

#endif

    /* @} */

    /* @{ */
    /** horizontal functions */
    inline float horizontal_min(void) const
    {
        __m128 xmm0, xmm1;
        xmm0 = data_;
        xmm1 = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(2,2,2,2));
        xmm0 = _mm_min_ps(xmm0, xmm1);
        xmm1 = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(1,1,1,1));
        xmm0 = _mm_min_ss(xmm0, xmm1);
        return _mm_cvtss_f32(xmm0);
    }

    inline float horizontal_max(void) const
    {
        __m128 xmm0, xmm1;
        xmm0 = data_;
        xmm1 = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(2,2,2,2));
        xmm0 = _mm_max_ps(xmm0, xmm1);
        xmm1 = _mm_shuffle_ps(xmm0, xmm0, _MM_SHUFFLE(1,1,1,1));
        xmm0 = _mm_max_ss(xmm0, xmm1);
        return _mm_cvtss_f32(xmm0);
    }
    /* @} */

private:
    typedef union
    {
        float f[4];
        __m128 m;
    } cast_union;

    __m128 data_;
};

} /* namespace nova */

#undef OPERATOR_ASSIGNMENT
#undef ARITHMETIC_OPERATOR
#undef RELATIONAL_OPERATOR
#undef always_inline

#endif /* VEC_SSE_HPP */
