//  simdfied round functions, generic implementation
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

#ifndef SIMD_ROUND_GENERIC_HPP
#define SIMD_ROUND_GENERIC_HPP

#include <cmath>

namespace nova {

namespace detail
{

template <typename float_type>
inline float_type round(float_type f)
{
    return std::floor(f + float_type(0.5));
}

template <typename float_type>
inline float_type frac(float_type f)
{
    return f - std::floor(f + float_type(0.5));
}

} /* namespace detail */

template <typename float_type>
inline void round_vec(float_type * out, const float_type * arg, unsigned int n)
{
    do {
        *out++ = detail::round<float_type>(*arg++);
    }
    while (--n);
}

template <typename float_type>
inline void round_vec_simd(float_type * out, const float_type * arg, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::round<float_type>(arg[0]);
        float_type out1 = detail::round<float_type>(arg[1]);
        float_type out2 = detail::round<float_type>(arg[2]);
        float_type out3 = detail::round<float_type>(arg[3]);
        float_type out4 = detail::round<float_type>(arg[4]);
        float_type out5 = detail::round<float_type>(arg[5]);
        float_type out6 = detail::round<float_type>(arg[6]);
        float_type out7 = detail::round<float_type>(arg[7]);
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
inline void ceil_vec(float_type * out, const float_type * arg, unsigned int n)
{
    do {
        *out++ = std::ceil(*arg++);
    }
    while (--n);
}

template <typename float_type>
inline void ceil_vec_simd(float_type * out, const float_type * arg, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::ceil(arg[0]);
        float_type out1 = std::ceil(arg[1]);
        float_type out2 = std::ceil(arg[2]);
        float_type out3 = std::ceil(arg[3]);
        float_type out4 = std::ceil(arg[4]);
        float_type out5 = std::ceil(arg[5]);
        float_type out6 = std::ceil(arg[6]);
        float_type out7 = std::ceil(arg[7]);
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
inline void floor_vec(float_type * out, const float_type * arg, unsigned int n)
{
    do {
        *out++ = std::floor(*arg++);
    }
    while (--n);
}

template <typename float_type>
inline void floor_vec_simd(float_type * out, const float_type * arg, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::floor(arg[0]);
        float_type out1 = std::floor(arg[1]);
        float_type out2 = std::floor(arg[2]);
        float_type out3 = std::floor(arg[3]);
        float_type out4 = std::floor(arg[4]);
        float_type out5 = std::floor(arg[5]);
        float_type out6 = std::floor(arg[6]);
        float_type out7 = std::floor(arg[7]);
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
inline void frac_vec(float_type * out, const float_type * arg, unsigned int n)
{
    do {
        *out++ = detail::frac<float_type>(*arg++);
    }
    while (--n);
}

template <typename float_type>
inline void frac_vec_simd(float_type * out, const float_type * arg, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::frac<float_type>(arg[0]);
        float_type out1 = detail::frac<float_type>(arg[1]);
        float_type out2 = detail::frac<float_type>(arg[2]);
        float_type out3 = detail::frac<float_type>(arg[3]);
        float_type out4 = detail::frac<float_type>(arg[4]);
        float_type out5 = detail::frac<float_type>(arg[5]);
        float_type out6 = detail::frac<float_type>(arg[6]);
        float_type out7 = detail::frac<float_type>(arg[7]);
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

#endif /* SIMD_ROUND_GENERIC_HPP */
