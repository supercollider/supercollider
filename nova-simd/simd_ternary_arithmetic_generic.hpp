//  templated arithmetic simd functions
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

#ifndef SIMD_TERNARY_ARITHMETIC_GENERIC_HPP
#define SIMD_TERNARY_ARITHMETIC_GENERIC_HPP

#include <algorithm>

namespace nova {

namespace detail
{

template <typename float_type>
inline float_type clip(float_type value, float_type low, float_type high)
{
    return std::max<float_type>(std::min<float_type>(value, high),
                                low);
}

template <typename float_type>
inline float_type muladd(float_type value, float_type mul, float_type add)
{
    return value * mul + add;
}

} /* namespace detail */

template <typename float_type>
inline void clip_vec(float_type * out, const float_type * arg1, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(*arg1++, *arg2++, *arg3++);
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out1 = detail::clip<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out2 = detail::clip<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out3 = detail::clip<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out4 = detail::clip<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out5 = detail::clip<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out6 = detail::clip<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out7 = detail::clip<float_type>(*arg1++, *arg2++, *arg3++);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec(float_type * out, const float_type * arg1, const float_type * arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(*arg1++, *arg2++, arg3);
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        float_type out1 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        float_type out2 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        float_type out3 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        float_type out4 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        float_type out5 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        float_type out6 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        float_type out7 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r3(float_type * out, const float_type * arg1, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r3(float_type * out, const float_type * arg1, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out1 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out2 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out3 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out4 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out5 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out6 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out7 = detail::clip<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec(float_type * out, const float_type * arg1, const float_type arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(*arg1++, arg2, *arg3++);
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        float_type out1 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        float_type out2 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        float_type out3 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        float_type out4 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        float_type out5 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        float_type out6 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        float_type out7 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec(float_type * out, const float_type * arg1, const float_type arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(*arg1++, arg2, arg3);
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(*arg1++, arg2, arg3);
        float_type out1 = detail::clip<float_type>(*arg1++, arg2, arg3);
        float_type out2 = detail::clip<float_type>(*arg1++, arg2, arg3);
        float_type out3 = detail::clip<float_type>(*arg1++, arg2, arg3);
        float_type out4 = detail::clip<float_type>(*arg1++, arg2, arg3);
        float_type out5 = detail::clip<float_type>(*arg1++, arg2, arg3);
        float_type out6 = detail::clip<float_type>(*arg1++, arg2, arg3);
        float_type out7 = detail::clip<float_type>(*arg1++, arg2, arg3);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r3(float_type * out, const float_type * arg1, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r3(float_type * out, const float_type * arg1, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out1 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out2 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out3 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out4 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out5 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out6 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out7 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r2(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r2(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out1 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out2 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out3 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out4 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out5 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out6 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out7 = detail::clip<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r2(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r2(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out1 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out2 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out3 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out4 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out5 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out6 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out7 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r2r3(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r2r3(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out1 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out2 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out3 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out4 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out5 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out6 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out7 = detail::clip<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec(float_type * out, const float_type arg1, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, *arg2++, *arg3++);
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        float_type out1 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        float_type out2 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        float_type out3 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        float_type out4 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        float_type out5 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        float_type out6 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        float_type out7 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec(float_type * out, const float_type arg1, const float_type * arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, *arg2++, arg3);
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, *arg2++, arg3);
        float_type out1 = detail::clip<float_type>(arg1, *arg2++, arg3);
        float_type out2 = detail::clip<float_type>(arg1, *arg2++, arg3);
        float_type out3 = detail::clip<float_type>(arg1, *arg2++, arg3);
        float_type out4 = detail::clip<float_type>(arg1, *arg2++, arg3);
        float_type out5 = detail::clip<float_type>(arg1, *arg2++, arg3);
        float_type out6 = detail::clip<float_type>(arg1, *arg2++, arg3);
        float_type out7 = detail::clip<float_type>(arg1, *arg2++, arg3);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r3(float_type * out, const float_type arg1, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r3(float_type * out, const float_type arg1, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out1 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out2 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out3 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out4 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out5 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out6 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out7 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec(float_type * out, const float_type arg1, const float_type arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, *arg3++);
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd(float_type * out, const float_type arg1, const float_type arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, *arg3++);
        float_type out1 = detail::clip<float_type>(arg1, arg2, *arg3++);
        float_type out2 = detail::clip<float_type>(arg1, arg2, *arg3++);
        float_type out3 = detail::clip<float_type>(arg1, arg2, *arg3++);
        float_type out4 = detail::clip<float_type>(arg1, arg2, *arg3++);
        float_type out5 = detail::clip<float_type>(arg1, arg2, *arg3++);
        float_type out6 = detail::clip<float_type>(arg1, arg2, *arg3++);
        float_type out7 = detail::clip<float_type>(arg1, arg2, *arg3++);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec(float_type * out, const float_type arg1, const float_type arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, arg3);
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd(float_type * out, const float_type arg1, const float_type arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, arg3);
        float_type out1 = detail::clip<float_type>(arg1, arg2, arg3);
        float_type out2 = detail::clip<float_type>(arg1, arg2, arg3);
        float_type out3 = detail::clip<float_type>(arg1, arg2, arg3);
        float_type out4 = detail::clip<float_type>(arg1, arg2, arg3);
        float_type out5 = detail::clip<float_type>(arg1, arg2, arg3);
        float_type out6 = detail::clip<float_type>(arg1, arg2, arg3);
        float_type out7 = detail::clip<float_type>(arg1, arg2, arg3);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r3(float_type * out, const float_type arg1, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r3(float_type * out, const float_type arg1, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out1 = detail::clip<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out2 = detail::clip<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out3 = detail::clip<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out4 = detail::clip<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out5 = detail::clip<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out6 = detail::clip<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out7 = detail::clip<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r2(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r2(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out1 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out2 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out3 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out4 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out5 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out6 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out7 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r2(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r2(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out1 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out2 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out3 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out4 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out5 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out6 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out7 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r2r3(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r2r3(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out1 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out2 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out3 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out4 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out5 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out6 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out7 = detail::clip<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out1 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out2 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out3 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out4 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out5 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out6 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out7 = detail::clip<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out1 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out2 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out3 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out4 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out5 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out6 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out7 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r1r3(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r1r3(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out1 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out2 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out3 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out4 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out5 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out6 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out7 = detail::clip<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out1 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out2 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out3 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out4 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out5 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out6 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out7 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out1 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out2 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out3 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out4 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out5 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out6 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out7 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r1r3(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r1r3(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out1 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out2 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out3 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out4 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out5 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out6 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out7 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r1r2(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r1r2(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out1 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out2 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out3 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out4 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out5 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out6 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out7 = detail::clip<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r1r2(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r1r2(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out1 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out2 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out3 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out4 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out5 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out6 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out7 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void clip_vec_r1r2r3(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void clip_vec_simd_r1r2r3(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out1 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out2 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out3 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out4 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out5 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out6 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out7 = detail::clip<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec(float_type * out, const float_type * arg1, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(*arg1++, *arg2++, *arg3++);
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out1 = detail::muladd<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out2 = detail::muladd<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out3 = detail::muladd<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out4 = detail::muladd<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out5 = detail::muladd<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out6 = detail::muladd<float_type>(*arg1++, *arg2++, *arg3++);
        float_type out7 = detail::muladd<float_type>(*arg1++, *arg2++, *arg3++);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec(float_type * out, const float_type * arg1, const float_type * arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        float_type out1 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        float_type out2 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        float_type out3 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        float_type out4 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        float_type out5 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        float_type out6 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        float_type out7 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r3(float_type * out, const float_type * arg1, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r3(float_type * out, const float_type * arg1, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out1 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out2 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out3 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out4 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out5 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out6 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out7 = detail::muladd<float_type>(*arg1++, *arg2++, arg3);
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec(float_type * out, const float_type * arg1, const float_type arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        float_type out1 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        float_type out2 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        float_type out3 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        float_type out4 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        float_type out5 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        float_type out6 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        float_type out7 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec(float_type * out, const float_type * arg1, const float_type arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(*arg1++, arg2, arg3);
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        float_type out1 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        float_type out2 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        float_type out3 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        float_type out4 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        float_type out5 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        float_type out6 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        float_type out7 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r3(float_type * out, const float_type * arg1, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r3(float_type * out, const float_type * arg1, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out1 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out2 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out3 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out4 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out5 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out6 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        float_type out7 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r2(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r2(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out1 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out2 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out3 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out4 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out5 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out6 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out7 = detail::muladd<float_type>(*arg1++, arg2, *arg3++);
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r2(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r2(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out1 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out2 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out3 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out4 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out5 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out6 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        float_type out7 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r2r3(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r2r3(float_type * out, const float_type * arg1, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out1 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out2 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out3 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out4 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out5 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out6 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out7 = detail::muladd<float_type>(*arg1++, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec(float_type * out, const float_type arg1, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        float_type out1 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        float_type out2 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        float_type out3 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        float_type out4 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        float_type out5 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        float_type out6 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        float_type out7 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec(float_type * out, const float_type arg1, const float_type * arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, *arg2++, arg3);
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        float_type out1 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        float_type out2 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        float_type out3 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        float_type out4 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        float_type out5 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        float_type out6 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        float_type out7 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r3(float_type * out, const float_type arg1, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r3(float_type * out, const float_type arg1, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out1 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out2 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out3 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out4 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out5 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out6 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        float_type out7 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec(float_type * out, const float_type arg1, const float_type arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, *arg3++);
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd(float_type * out, const float_type arg1, const float_type arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        float_type out1 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        float_type out2 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        float_type out3 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        float_type out4 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        float_type out5 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        float_type out6 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        float_type out7 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec(float_type * out, const float_type arg1, const float_type arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, arg3);
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd(float_type * out, const float_type arg1, const float_type arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, arg3);
        float_type out1 = detail::muladd<float_type>(arg1, arg2, arg3);
        float_type out2 = detail::muladd<float_type>(arg1, arg2, arg3);
        float_type out3 = detail::muladd<float_type>(arg1, arg2, arg3);
        float_type out4 = detail::muladd<float_type>(arg1, arg2, arg3);
        float_type out5 = detail::muladd<float_type>(arg1, arg2, arg3);
        float_type out6 = detail::muladd<float_type>(arg1, arg2, arg3);
        float_type out7 = detail::muladd<float_type>(arg1, arg2, arg3);
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r3(float_type * out, const float_type arg1, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r3(float_type * out, const float_type arg1, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out1 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out2 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out3 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out4 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out5 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out6 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        float_type out7 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r2(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r2(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out1 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out2 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out3 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out4 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out5 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out6 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        float_type out7 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r2(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r2(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out1 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out2 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out3 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out4 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out5 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out6 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        float_type out7 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r2r3(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r2r3(float_type * out, const float_type arg1, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out1 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out2 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out3 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out4 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out5 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out6 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out7 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out1 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out2 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out3 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out4 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out5 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out6 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        float_type out7 = detail::muladd<float_type>(arg1, *arg2++, *arg3++);
        arg1 += arg1_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out1 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out2 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out3 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out4 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out5 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out6 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        float_type out7 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r1r3(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r1r3(float_type * out, float_type arg1, const float_type arg1_slope, const float_type * arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out1 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out2 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out3 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out4 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out5 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out6 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out7 = detail::muladd<float_type>(arg1, *arg2++, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out1 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out2 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out3 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out4 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out5 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out6 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        float_type out7 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r1(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out1 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out2 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out3 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out4 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out5 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out6 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        float_type out7 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r1r3(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r1r3(float_type * out, float_type arg1, const float_type arg1_slope, const float_type arg2, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out1 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out2 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out3 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out4 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out5 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out6 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        float_type out7 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r1r2(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r1r2(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, const float_type * arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out1 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out2 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out3 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out4 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out5 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out6 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out7 = detail::muladd<float_type>(arg1, arg2, *arg3++);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r1r2(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r1r2(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, const float_type arg3, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out1 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out2 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out3 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out4 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out5 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out6 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        float_type out7 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

template <typename float_type>
inline void muladd_vec_r1r2r3(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    do {
        *out++ = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
    }
    while (--n);
}

template <typename float_type>
inline void muladd_vec_simd_r1r2r3(float_type * out, float_type arg1, const float_type arg1_slope, float_type arg2, const float_type arg2_slope, float_type arg3, const float_type arg3_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out1 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out2 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out3 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out4 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out5 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out6 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        float_type out7 = detail::muladd<float_type>(arg1, arg2, arg3);
        arg1 += arg1_slope;
        arg2 += arg2_slope;
        arg3 += arg3_slope;
        *out++ = out0;
        *out++ = out1;
        *out++ = out2;
        *out++ = out3;
        *out++ = out4;
        *out++ = out5;
        *out++ = out6;
        *out++ = out7;
    }
    while (--loops);
}

} /* namespace nova */

#endif /* SIMD_TERNARY_ARITHMETIC_GENERIC_HPP */
