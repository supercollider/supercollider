//  generic vector class
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

#ifndef VEC_GENERIC_HPP
#define VEC_GENERIC_HPP

#include <cmath>
#include <algorithm>

#include "wrap_arguments.hpp"
#include "detail/math.hpp"

namespace nova
{

template <typename FloatType>
struct vec
{
public:
    typedef FloatType float_type;

    static const int size = 4;
    static const int objects_per_cacheline = 64/sizeof(float_type);

    typedef float_type (*unary_fn)(float_type);
    typedef float_type (*binary_fn)(float_type, float_type);

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
        set_vec(f);
    }

    vec(vec const & rhs)
    {
        for (int i = 0; i != size; ++i)
            data_[i] = rhs.data_[i];
    }
    /* @} */

    /* @{ */
    /** io */
    void load(const float_type * data)
    {
        for (int i = 0; i != size; ++i)
            data_[i] = data[i];
    }

    void load_first(const float_type * data)
    {
        data_[0] = *data;

        for (int i = 1; i != size; ++i)
            data_[i] = 0;
    }

    void load_aligned(const float_type * data)
    {
        load(data);
    }

    void store(float_type * dest) const
    {
        for (int i = 0; i != size; ++i)
            dest[i] = data_[i];
    }

    void store_aligned(float_type * dest) const
    {
        store(dest);
    }

    void store_aligned_stream(float_type * dest) const
    {
        store(dest);
    }

    void clear(void)
    {
        set_vec(0);
    }
    /* @} */

    /* @{ */
    /** element access */
    void set (std::size_t index, float_type value)
    {
        data_[index] = value;
    }

    void set_vec (float_type value)
    {
        for (int i = 0; i != size; ++i)
            data_[i] = value;
    }

    float_type set_slope(float_type start, float_type slope)
    {
        float_type diff = 0;
        for (int i = 0; i != size; ++i)
        {
            data_[i] = start + diff;
            diff += slope;
        }
        return diff;
    }

    float_type set_exp(float_type start, float_type curve)
    {
        float_type value = start;
        for (int i = 0; i != size; ++i)
        {
            data_[i] = value;
            value *= curve;
        }
        return value;
    }

    float_type get (std::size_t index)
    {
        return data_[index];
    }
    /* @} */

    /* @{ */
    /** arithmetic operators */
#define OPERATOR_ASSIGNMENT(op) \
    vec & operator op(vec const & rhs) \
    { \
        for (int i = 0; i != size; ++i) \
            data_[i] op rhs.data_[i]; \
        return *this; \
    }

    OPERATOR_ASSIGNMENT(+=)
    OPERATOR_ASSIGNMENT(-=)
    OPERATOR_ASSIGNMENT(*=)
    OPERATOR_ASSIGNMENT(/=)

#define ARITHMETIC_OPERATOR(op) \
    vec operator op(vec const & rhs) const \
    { \
        vec ret; \
        for (int i = 0; i != size; ++i) \
            ret.data_[i] = data_[i] op rhs.data_[i]; \
        return ret; \
    }

    ARITHMETIC_OPERATOR(+)
    ARITHMETIC_OPERATOR(-)
    ARITHMETIC_OPERATOR(*)
    ARITHMETIC_OPERATOR(/)

    ARITHMETIC_OPERATOR(<)
    ARITHMETIC_OPERATOR(<=)
    ARITHMETIC_OPERATOR(>)
    ARITHMETIC_OPERATOR(>=)
    ARITHMETIC_OPERATOR(==)
    ARITHMETIC_OPERATOR(!=)

    /* @} */

    /* @{ */
    /** unary functions */
#define APPLY_UNARY(NAME, FUNCTION)                 \
    friend inline vec NAME(vec const & arg)         \
    {                                               \
        vec ret;                                    \
        detail::apply_on_vector<float_type, size> (ret.data_, wrap_argument(arg.data_),                \
                                                   FUNCTION);    \
        return ret;                                 \
    }

    APPLY_UNARY(abs, detail::fabs<float_type>)
    APPLY_UNARY(sign, detail::sign<float_type>)
    APPLY_UNARY(square, detail::square<float_type>)
    APPLY_UNARY(cube, detail::cube<float_type>)
    /* @} */

    /* @{ */

#define APPLY_BINARY(NAME, FUNCTION)                            \
    friend inline vec NAME(vec const & lhs, vec const & rhs)    \
    {                                                           \
        vec ret;                                                \
        detail::apply_on_vector<float_type, size> (ret.data_, wrap_argument(lhs.data_), wrap_argument(rhs.data_),  \
                                                   FUNCTION);   \
        return ret;                                 \
    }

    APPLY_BINARY(max_, detail::max<float_type>)
    APPLY_BINARY(min_, detail::min<float_type>)
    /* @} */


    /* @{ */
    /** rounding functions */
    APPLY_UNARY(round, detail::round<float_type>)
    APPLY_UNARY(frac, detail::frac<float_type>)
    APPLY_UNARY(floor, detail::floor<float_type>)
    APPLY_UNARY(ceil, detail::ceil<float_type>)
    /* @} */

    /* @{ */
    /** mathematical functions */
    APPLY_UNARY(sin, detail::sin<float_type>)
    APPLY_UNARY(cos, detail::cos<float_type>)
    APPLY_UNARY(tan, detail::tan<float_type>)
    APPLY_UNARY(asin, detail::asin<float_type>)
    APPLY_UNARY(acos, detail::acos<float_type>)
    APPLY_UNARY(atan, detail::atan<float_type>)

    APPLY_UNARY(tanh, detail::tanh<float_type>)

    APPLY_UNARY(log, detail::log<float_type>)
    APPLY_UNARY(log2, detail::log2<float_type>)
    APPLY_UNARY(log10, detail::log10<float_type>)
    APPLY_UNARY(exp, detail::exp<float_type>)

    APPLY_UNARY(signed_sqrt, detail::signed_sqrt<float_type>)

    APPLY_BINARY(pow, detail::pow<float_type>)
    APPLY_BINARY(signed_pow, detail::signed_pow<float_type>)
    /* @} */

    /* @{ */
    /** horizontal functions */
    inline float horizontal_min(void) const
    {
        return *std::min_element(data_, data_ + size);
    }

    inline float horizontal_max(void) const
    {
        return *std::max_element(data_, data_ + size);
    }
    /* @} */

private:
    float_type data_[size];
};

} /* namespace nova */

#undef OPERATOR_ASSIGNMENT
#undef ARITHMETIC_OPERATOR
#undef APPLY_UNARY
#undef APPLY_BINARY

#endif /* VEC_GENERIC_HPP */
