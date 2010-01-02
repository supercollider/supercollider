//  simdfied mathematical functions, double fallback implementation
//  Copyright (C) 2009 Tim Blechmann
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

#ifndef SIMD_MATH_FALLBACK_DOUBLE_HPP
#define SIMD_MATH_FALLBACK_DOUBLE_HPP

#include <cmath>

namespace nova
{

#define WRAP_MATH_FUNCTION(name)                                \
    template <unsigned int n>                                   \
    inline void name##_vec_simd_mp(double * out, const double * in)\
    {                                                           \
        name##_vec_simd(out, in, n);                            \
    }                                                           \
    template <unsigned int n>                                   \
    inline void name##_vec_simd(double * out, const double * in)\
    {                                                           \
        name##_vec_simd(out, in, n);                            \
    }

#define WRAP_MATH_FUNCTION_BINARY(name)                                 \
    template <unsigned int n>                                           \
    inline void name##_vec_simd(double * out, const double * in0, const double * in1) \
    {                                                                   \
        name##_vec_simd(out, in0, in1, n);                              \
    }                                                                   \
                                                                        \
    template <unsigned int n>                                           \
    inline void name##_vec_simd(double * out, const double * in0, const double in1) \
    {                                                                   \
        name##_vec_simd(out, in0, in1, n);                              \
    }                                                                   \
                                                                        \
    template <unsigned int n>                                           \
    inline void name##_vec_simd(double * out, const double in0, const double * in1) \
    {                                                                   \
        name##_vec_simd(out, in0, in1, n);                              \
    }


WRAP_MATH_FUNCTION(sin)
WRAP_MATH_FUNCTION(cos)
WRAP_MATH_FUNCTION(tan)
WRAP_MATH_FUNCTION(asin)
WRAP_MATH_FUNCTION(acos)
WRAP_MATH_FUNCTION(atan)
WRAP_MATH_FUNCTION(log)
WRAP_MATH_FUNCTION(log2)
WRAP_MATH_FUNCTION(log10)
WRAP_MATH_FUNCTION(exp)
WRAP_MATH_FUNCTION(ssqrt)
WRAP_MATH_FUNCTION(tanh)

WRAP_MATH_FUNCTION_BINARY(pow)
WRAP_MATH_FUNCTION_BINARY(spow)

} /* namespace nova */

#undef WRAP_MATH_FUNCTION
#undef WRAP_MATH_FUNCTION_BINARY


#endif /* SIMD_MATH_FALLBACK_DOUBLE_HPP */
