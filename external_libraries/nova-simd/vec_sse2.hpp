//  sse2 vector class
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

#ifndef VEC_SSE2_HPP
#define VEC_SSE2_HPP

#include <algorithm>

#include <xmmintrin.h>
#include <emmintrin.h>

#ifdef __SSE4_1__
#include <smmintrin.h>
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
struct vec<double>
{
    typedef double float_type;
    typedef __m128d internal_vector_type;

    static inline __m128d gen_sign_mask(void)
    {
        __m128i x = _mm_setzero_si128();
#ifdef __SSE4_1__
        __m128i ones = _mm_cmpeq_epi64(x, x);
#else
        __m128i ones = _mm_cmpeq_epi32(x, x);
#endif
        return (__m128d)_mm_slli_epi64 (_mm_srli_epi64(ones, 63), 63);
    }

    static inline __m128d gen_abs_mask(void)
    {
        __m128i x = _mm_setzero_si128();
#ifdef __SSE4_1__
        __m128i ones = _mm_cmpeq_epi64(x, x);
#else
        __m128i ones = _mm_cmpeq_epi32(x, x);
#endif
        return (__m128d)_mm_srli_epi64 (_mm_slli_epi64(ones, 1), 1);
    }

    static inline __m128d gen_one(void)
    {
        return _mm_set1_pd(1.f);
    }

    static inline __m128d gen_05(void)
    {
        return _mm_set1_pd(0.5f);
    }

    static inline __m128d gen_zero(void)
    {
        volatile __m128d x;
        return _mm_xor_pd(x, x);
    }

    vec(__m128d const & arg):
        data_(arg)
    {}

public:
    static const int size = 2;
    static const int objects_per_cacheline = 64/sizeof(double);

    /* @{ */
    /** constructors */
    vec(void)
    {}

    vec(double f)
    {
        set_vec(f);
    }

    vec(float f)
    {
        set_vec((double)f);
    }

    vec(vec const & rhs)
    {
        data_ = rhs.data_;
    }
    /* @} */

    /* @{ */
    /** io */
    void load(const double * data)
    {
        data_ = _mm_loadu_pd(data);
    }

    void load_aligned(const double * data)
    {
        data_ = _mm_load_pd(data);
    }

    void load_first(const double * data)
    {
        data_ = _mm_load_sd(data);
    }

    void store(double * dest) const
    {
        _mm_storeu_pd(dest, data_);
    }

    void store_aligned(double * dest) const
    {
        _mm_store_pd(dest, data_);
    }

    void store_aligned_stream(double * dest) const
    {
        _mm_stream_pd(dest, data_);
    }

    void clear(void)
    {
        data_ = gen_zero();
    }

    /* @} */

    /* @{ */
    /** element access */
    void set (std::size_t index, double value)
    {
        double * data = (double*)&data_;
        data[index] = value;
    }

    void set_vec (double value)
    {
        data_ = _mm_set1_pd(value);
    }

    double set_slope(double start, double slope)
    {
        double v1 = start + slope;
        data_ = _mm_set_pd(v1, start);
        return slope + slope;
    }

    double set_exp(double start, double curve)
    {
        double v1 = start * curve;
        data_ = _mm_set_pd(v1, start);
        return v1 * curve;
    }

    double get (std::size_t index) const
    {
        __m128d ret;
        switch (index)
        {
        case 0:
            ret = data_;
            break;

        case 1:
            ret = _mm_shuffle_pd(data_, data_, _MM_SHUFFLE2(1, 1));
            break;
        }

        return _mm_cvtsd_f64(ret);
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

    OPERATOR_ASSIGNMENT(+=, _mm_add_pd)
    OPERATOR_ASSIGNMENT(-=, _mm_sub_pd)
    OPERATOR_ASSIGNMENT(*=, _mm_mul_pd)
    OPERATOR_ASSIGNMENT(/=, _mm_div_pd)

#define ARITHMETIC_OPERATOR(op, opcode) \
    vec operator op(vec const & rhs) const \
    { \
        return opcode(data_, rhs.data_); \
    }

    ARITHMETIC_OPERATOR(+, _mm_add_pd)
    ARITHMETIC_OPERATOR(-, _mm_sub_pd)
    ARITHMETIC_OPERATOR(*, _mm_mul_pd)
    ARITHMETIC_OPERATOR(/, _mm_div_pd)

#define RELATIONAL_OPERATOR(op, opcode) \
    vec operator op(vec const & rhs) const \
    { \
        const __m128d one = gen_one(); \
        return _mm_and_pd(opcode(data_, rhs.data_), one); \
    }

    RELATIONAL_OPERATOR(<, _mm_cmplt_pd)
    RELATIONAL_OPERATOR(<=, _mm_cmple_pd)
    RELATIONAL_OPERATOR(>, _mm_cmpgt_pd)
    RELATIONAL_OPERATOR(>=, _mm_cmpge_pd)
    RELATIONAL_OPERATOR(==, _mm_cmpeq_pd)
    RELATIONAL_OPERATOR(!=, _mm_cmpneq_pd)

    /* @{ */
#define BITWISE_OPERATOR(op, opcode) \
    vec operator op(vec const & rhs) const \
    { \
        return opcode(data_, rhs.data_); \
    }

    BITWISE_OPERATOR(&, _mm_and_pd)
    BITWISE_OPERATOR(|, _mm_or_pd)
    BITWISE_OPERATOR(^, _mm_xor_pd)

    #define RELATIONAL_MASK_OPERATOR(op, opcode) \
    friend vec mask_##op(vec const & lhs, vec const & rhs) \
    { \
        return opcode(lhs.data_, rhs.data_); \
    }

    RELATIONAL_MASK_OPERATOR(lt, _mm_cmplt_pd)
    RELATIONAL_MASK_OPERATOR(le, _mm_cmple_pd)
    RELATIONAL_MASK_OPERATOR(gt, _mm_cmpgt_pd)
    RELATIONAL_MASK_OPERATOR(ge, _mm_cmpge_pd)
    RELATIONAL_MASK_OPERATOR(eq, _mm_cmpeq_pd)
    RELATIONAL_MASK_OPERATOR(neq, _mm_cmpneq_pd)

    #undef RELATIONAL_MASK_OPERATOR

    friend inline vec select(vec lhs, vec rhs, vec bitmask)
    {
        /* if bitmask is set, return value in rhs, else value in lhs */
#ifdef __SSE4_1__
        return _mm_blendv_pd(lhs.data_, rhs.data_, bitmask.data_);
#else
        return _mm_or_pd(_mm_andnot_pd(bitmask.data_, lhs.data_),
                        _mm_and_pd(rhs.data_, bitmask.data_));
#endif
    }

    /* @} */

    /* @{ */
    /** unary functions */
    friend inline vec abs(vec const & arg)
    {
        return _mm_and_pd(gen_abs_mask(), arg.data_);
    }

    friend always_inline vec sign(vec const & arg)
    {
        return detail::vec_sign(arg);
    }

    friend inline vec square(vec const & arg)
    {
        return _mm_mul_pd(arg.data_, arg.data_);
    }

    friend inline vec sqrt(vec const & arg)
    {
        return _mm_sqrt_pd(arg.data_);
    }

    friend inline vec cube(vec const & arg)
    {
        return _mm_mul_pd(arg.data_, _mm_mul_pd(arg.data_, arg.data_));
    }
    /* @} */

    /* @{ */
    /** binary functions */
    friend inline vec max_(vec const & lhs, vec const & rhs)
    {
        return _mm_max_pd(lhs.data_, rhs.data_);
    }

    friend inline vec min_(vec const & lhs, vec const & rhs)
    {
        return _mm_min_pd(lhs.data_, rhs.data_);
    }
    /* @} */

    /* @{ */
    /** rounding functions */
    friend inline vec round(vec const & arg)
    {
#ifdef __SSE4_1__
        return _mm_round_pd(arg.data_, _MM_FROUND_TO_NEAREST_INT);
#else
        vec ret;
        detail::apply_on_vector<double, size> ((double*)&ret.data_,
                                              wrap_argument((double*)&arg.data_),
                                              (double(*)(double const &))detail::round<double>);
        return ret;
#endif
    }

    friend inline vec frac(vec const & arg)
    {
        return arg - floor(arg);
    }

    friend inline vec floor(vec const & arg)
    {
#ifdef __SSE4_1__
        return _mm_round_pd(arg.data_, _MM_FROUND_TO_NEG_INF);
#else
        vec ret;
        detail::apply_on_vector<double, size> ((double*)&ret.data_,
                                              wrap_argument((double*)&arg.data_),
                                              (double(*)(double const &))detail::floor<double>);
        return ret;
#endif
    }

    friend inline vec ceil(vec const & arg)
    {
#ifdef __SSE4_1__
        return _mm_round_pd(arg.data_, _MM_FROUND_TO_POS_INF);
#else
        vec ret;
        detail::apply_on_vector<double, size> ((double*)&ret.data_,
                                              wrap_argument((double*)&arg.data_),
                                              (double(*)(double const &))detail::ceil<double>);
        return ret;
#endif
    }
    /* @} */


    /* @{ */
    /** mathematical functions */
#define APPLY_UNARY(NAME, FUNCTION)                 \
    friend inline vec NAME(vec const & arg)         \
    {                                               \
        vec ret;                                    \
        detail::apply_on_vector<double, size> ((double*)&ret.data_, \
                                               wrap_argument((double*)&arg.data_),    \
                                               FUNCTION);           \
        return ret;                                 \
    }

#define APPLY_BINARY(NAME, FUNCTION)                            \
    friend inline vec NAME(vec const & lhs, vec const & rhs)    \
    {                                                           \
        vec ret;                                                \
        detail::apply_on_vector<double, size> ((double*)&ret.data_, \
                                              wrap_argument((double*)&lhs.data_), \
                                              wrap_argument((double*)&rhs.data_),  \
                                              FUNCTION);        \
        return ret;                                 \
    }

    APPLY_UNARY(sin, detail::sin<double>)
    APPLY_UNARY(cos, detail::cos<double>)
    APPLY_UNARY(tan, detail::tan<double>)
    APPLY_UNARY(asin, detail::asin<double>)
    APPLY_UNARY(acos, detail::acos<double>)
    APPLY_UNARY(atan, detail::atan<double>)

    APPLY_UNARY(log, detail::log<double>)
    APPLY_UNARY(exp, detail::exp<double>)

    APPLY_BINARY(pow, detail::pow<double>)

    APPLY_UNARY(tanh, detail::tanh<double>)


#undef APPLY_UNARY
#undef APPLY_BINARY

    friend inline vec signed_pow(vec const & lhs, vec const & rhs)
    {
        return detail::vec_signed_pow(lhs, rhs);
    }

    friend inline vec signed_sqrt(vec const & arg)
    {
        return detail::vec_signed_sqrt(arg);
    }

    friend inline vec log2(vec const & arg)
    {
        return detail::vec_log2(arg);
    }

    friend inline vec log10(vec const & arg)
    {
        return detail::vec_log10(arg);
    }
    /* @} */

    /* @{ */
    /** horizontal functions */
    inline double horizontal_min(void) const
    {
        return std::min(get(0), get(1));
    }

    inline double horizontal_max(void) const
    {
        return std::max(get(0), get(1));
    }
    /* @} */

private:
    typedef union
    {
        double f[2];
        __m128d m;
    } cast_union;

    __m128d data_;
};

} /* namespace nova */


#undef OPERATOR_ASSIGNMENT
#undef ARITHMETIC_OPERATOR
#undef RELATIONAL_OPERATOR
#undef always_inline

#endif /* VEC_SSE2_HPP */
