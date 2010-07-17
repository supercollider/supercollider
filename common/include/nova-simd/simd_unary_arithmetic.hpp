//  unary arithmetic simd functions
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

#ifndef SIMD_UNARY_ARITHMETIC_HPP
#define SIMD_UNARY_ARITHMETIC_HPP

#include "vec.hpp"

#include "detail/define_macros.hpp"

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

namespace nova
{

namespace detail
{

#define DEFINE_UNARY_FUNCTOR(NAME, VEC_NAME)                            \
template <typename FloatType>                                           \
struct NAME##_                                                          \
{                                                                       \
    always_inline FloatType operator()(FloatType arg) const             \
    {                                                                   \
        return NAME<FloatType>(arg);                                    \
    }                                                                   \
};                                                                      \
                                                                        \
template <typename FloatType>                                           \
struct NAME##_<vec<FloatType> >                                         \
{                                                                       \
    always_inline vec<FloatType> operator()(vec<FloatType> const & arg) const \
    {                                                                   \
        return VEC_NAME(arg);                                           \
    }                                                                   \
};

DEFINE_UNARY_FUNCTOR(fabs, abs)
DEFINE_UNARY_FUNCTOR(sign, sign)
DEFINE_UNARY_FUNCTOR(square, square)
DEFINE_UNARY_FUNCTOR(cube, cube)

DEFINE_UNARY_FUNCTOR(round, round)
DEFINE_UNARY_FUNCTOR(frac, frac)
DEFINE_UNARY_FUNCTOR(ceil, ceil)
DEFINE_UNARY_FUNCTOR(floor, floor)

} /* namespace detail */


NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(abs, detail::fabs_)
NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(sgn, detail::sign_)
NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(square, detail::square_)
NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(cube, detail::cube_)

NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(round, detail::round_)
NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(frac, detail::frac_)
NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(ceil, detail::ceil_)
NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(floor, detail::floor_)

} /* namespace nova */

#undef always_inline

#endif /* SIMD_UNARY_ARITHMETIC_HPP */
