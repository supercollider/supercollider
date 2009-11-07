//  templated unary arithmetic simd functions
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

#ifndef SIMD_UNARY_ARITHMETIC_GENERIC_HPP
#define SIMD_UNARY_ARITHMETIC_GENERIC_HPP

#include <cmath>

namespace nova {

namespace detail
{
template <typename float_type>
inline float_type sgn(float_type arg)
{
    if (arg > 0)
        return float_type(1);
    if (arg == 0)
        return float_type(0);
    else
        return float_type(-1);
}

template <typename float_type>
inline float_type square(float_type arg)
{
    return arg * arg;
}

template <typename float_type>
inline float_type cube(float_type arg)
{
    return arg * arg * arg;
}

} /* namespace detail */


template <typename float_type>
inline void abs_vec(float_type * out, const float_type * arg, unsigned int n)
{
    do {
        *out++ = std::abs(*arg++);
    }
    while (--n);
}

template <typename float_type>
inline void abs_vec_simd(float_type * out, const float_type * arg, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::abs(arg[0]);
        float_type out1 = std::abs(arg[1]);
        float_type out2 = std::abs(arg[2]);
        float_type out3 = std::abs(arg[3]);
        float_type out4 = std::abs(arg[4]);
        float_type out5 = std::abs(arg[5]);
        float_type out6 = std::abs(arg[6]);
        float_type out7 = std::abs(arg[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void sgn_vec(float_type * out, const float_type * arg, unsigned int n)
{
    do {
        *out++ = detail::sgn<float_type>(*arg++);
    }
    while (--n);
}

template <typename float_type>
inline void sgn_vec_simd(float_type * out, const float_type * arg, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::sgn<float_type>(arg[0]);
        float_type out1 = detail::sgn<float_type>(arg[1]);
        float_type out2 = detail::sgn<float_type>(arg[2]);
        float_type out3 = detail::sgn<float_type>(arg[3]);
        float_type out4 = detail::sgn<float_type>(arg[4]);
        float_type out5 = detail::sgn<float_type>(arg[5]);
        float_type out6 = detail::sgn<float_type>(arg[6]);
        float_type out7 = detail::sgn<float_type>(arg[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void square_vec(float_type * out, const float_type * arg, unsigned int n)
{
    do {
        *out++ = detail::square<float_type>(*arg++);
    }
    while (--n);
}

template <typename float_type>
inline void square_vec_simd(float_type * out, const float_type * arg, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::square<float_type>(arg[0]);
        float_type out1 = detail::square<float_type>(arg[1]);
        float_type out2 = detail::square<float_type>(arg[2]);
        float_type out3 = detail::square<float_type>(arg[3]);
        float_type out4 = detail::square<float_type>(arg[4]);
        float_type out5 = detail::square<float_type>(arg[5]);
        float_type out6 = detail::square<float_type>(arg[6]);
        float_type out7 = detail::square<float_type>(arg[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void cube_vec(float_type * out, const float_type * arg, unsigned int n)
{
    do {
        *out++ = detail::cube<float_type>(*arg++);
    }
    while (--n);
}

template <typename float_type>
inline void cube_vec_simd(float_type * out, const float_type * arg, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::cube<float_type>(arg[0]);
        float_type out1 = detail::cube<float_type>(arg[1]);
        float_type out2 = detail::cube<float_type>(arg[2]);
        float_type out3 = detail::cube<float_type>(arg[3]);
        float_type out4 = detail::cube<float_type>(arg[4]);
        float_type out5 = detail::cube<float_type>(arg[5]);
        float_type out6 = detail::cube<float_type>(arg[6]);
        float_type out7 = detail::cube<float_type>(arg[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg += 8;
    }
    while (--loops);
}

} /* namespace nova */

#endif /* SIMD_UNARY_ARITHMETIC_GENERIC_HPP */
