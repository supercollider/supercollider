//  binary arithmetic simd functions
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

#ifndef SIMD_BINARY_ARITHMETIC_HPP
#define SIMD_BINARY_ARITHMETIC_HPP

#include <functional>
#include <algorithm>

#include "vec.hpp"

#include "detail/unroll_helpers.hpp"
#include "detail/define_macros.hpp"


#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif


namespace nova {

namespace detail
{

template<typename float_type>
struct clip2:
    public std::binary_function<float_type, float_type, float_type>
{
    float_type operator()(float_type const & f, float_type const & limit) const
    {
        float_type zero = 0.0;
        float_type neg = zero - float_type(limit);
        return max_(neg, min_(f, limit));
    }
};

template<typename float_type>
struct min_functor:
    public std::binary_function<float_type, float_type, float_type>
{
    float_type operator()(float_type const & x, float_type const & y) const
    {
        return min_(x, y);
    }
};

template<typename float_type>
struct max_functor:
    public std::binary_function<float_type, float_type, float_type>
{
    float_type operator()(float_type const & x, float_type const & y) const
    {
        return max_(x, y);
    }
};

template<typename float_type>
struct less:
    public std::binary_function<float_type, float_type, float_type>
{
    float_type operator()(float_type const & x, float_type const & y) const
    {
        return x < y;
    }
};

template<typename float_type>
struct less_equal:
    public std::binary_function<float_type, float_type, float_type>
{
    float_type operator()(float_type const & x, float_type const & y) const
    {
        return x <= y;
    }
};

template<typename float_type>
struct greater:
    public std::binary_function<float_type, float_type, float_type>
{
    float_type operator()(float_type const & x, float_type const & y) const
    {
        return x > y;
    }
};

template<typename float_type>
struct greater_equal:
    public std::binary_function<float_type, float_type, float_type>
{
    float_type operator()(float_type const & x, float_type const & y) const
    {
        return x >= y;
    }
};


template<typename float_type>
struct equal_to:
    public std::binary_function<float_type, float_type, float_type>
{
    float_type operator()(float_type const & x, float_type const & y) const
    {
        return x == y;
    }
};

template<typename float_type>
struct not_equal_to:
    public std::binary_function<float_type, float_type, float_type>
{
    float_type operator()(float_type const & x, float_type const & y) const
    {
        return x != y;
    }
};


} /* namespace detail */


NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(plus, std::plus)

NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(minus, std::minus)
NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(times, std::multiplies)
NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(over, std::divides)

NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(min, detail::min_functor)
NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(max, detail::max_functor)
NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(less, detail::less)
NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(less_equal, detail::less_equal)
NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(greater, detail::greater)
NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(greater_equal, detail::greater_equal)
NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(equal, detail::equal_to)
NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(notequal, detail::not_equal_to)

NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(clip2, detail::clip2)

} /* namespace nova */

#undef always_inline


#endif /* SIMD_BINARY_ARITHMETIC_HPP */
