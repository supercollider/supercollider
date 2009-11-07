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

#ifndef SIMD_BINARY_ARITHMETIC_GENERIC_HPP
#define SIMD_BINARY_ARITHMETIC_GENERIC_HPP

#include <functional>
#include <algorithm>

namespace nova {

template <typename float_type>
inline void plus_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::plus<float_type>()(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void plus_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::plus<float_type>()(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void plus_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::plus<float_type>()(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void plus_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::plus<float_type>()(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void plus_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::plus<float_type>()(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void plus_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::plus<float_type>()(arg1[0], arg2[0]);
        float_type out1 = std::plus<float_type>()(arg1[1], arg2[1]);
        float_type out2 = std::plus<float_type>()(arg1[2], arg2[2]);
        float_type out3 = std::plus<float_type>()(arg1[3], arg2[3]);
        float_type out4 = std::plus<float_type>()(arg1[4], arg2[4]);
        float_type out5 = std::plus<float_type>()(arg1[5], arg2[5]);
        float_type out6 = std::plus<float_type>()(arg1[6], arg2[6]);
        float_type out7 = std::plus<float_type>()(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void plus_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::plus<float_type>()(arg1[0], arg2);
        float_type out1 = std::plus<float_type>()(arg1[1], arg2);
        float_type out2 = std::plus<float_type>()(arg1[2], arg2);
        float_type out3 = std::plus<float_type>()(arg1[3], arg2);
        float_type out4 = std::plus<float_type>()(arg1[4], arg2);
        float_type out5 = std::plus<float_type>()(arg1[5], arg2);
        float_type out6 = std::plus<float_type>()(arg1[6], arg2);
        float_type out7 = std::plus<float_type>()(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void plus_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::plus<float_type>()(arg1, arg2[0]);
        float_type out1 = std::plus<float_type>()(arg1, arg2[1]);
        float_type out2 = std::plus<float_type>()(arg1, arg2[2]);
        float_type out3 = std::plus<float_type>()(arg1, arg2[3]);
        float_type out4 = std::plus<float_type>()(arg1, arg2[4]);
        float_type out5 = std::plus<float_type>()(arg1, arg2[5]);
        float_type out6 = std::plus<float_type>()(arg1, arg2[6]);
        float_type out7 = std::plus<float_type>()(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void plus_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::plus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::plus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::plus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::plus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::plus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::plus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::plus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::plus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void plus_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::plus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::plus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::plus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::plus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::plus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::plus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::plus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::plus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void minus_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::minus<float_type>()(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void minus_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::minus<float_type>()(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void minus_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::minus<float_type>()(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void minus_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::minus<float_type>()(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void minus_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::minus<float_type>()(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void minus_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::minus<float_type>()(arg1[0], arg2[0]);
        float_type out1 = std::minus<float_type>()(arg1[1], arg2[1]);
        float_type out2 = std::minus<float_type>()(arg1[2], arg2[2]);
        float_type out3 = std::minus<float_type>()(arg1[3], arg2[3]);
        float_type out4 = std::minus<float_type>()(arg1[4], arg2[4]);
        float_type out5 = std::minus<float_type>()(arg1[5], arg2[5]);
        float_type out6 = std::minus<float_type>()(arg1[6], arg2[6]);
        float_type out7 = std::minus<float_type>()(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void minus_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::minus<float_type>()(arg1[0], arg2);
        float_type out1 = std::minus<float_type>()(arg1[1], arg2);
        float_type out2 = std::minus<float_type>()(arg1[2], arg2);
        float_type out3 = std::minus<float_type>()(arg1[3], arg2);
        float_type out4 = std::minus<float_type>()(arg1[4], arg2);
        float_type out5 = std::minus<float_type>()(arg1[5], arg2);
        float_type out6 = std::minus<float_type>()(arg1[6], arg2);
        float_type out7 = std::minus<float_type>()(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void minus_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::minus<float_type>()(arg1, arg2[0]);
        float_type out1 = std::minus<float_type>()(arg1, arg2[1]);
        float_type out2 = std::minus<float_type>()(arg1, arg2[2]);
        float_type out3 = std::minus<float_type>()(arg1, arg2[3]);
        float_type out4 = std::minus<float_type>()(arg1, arg2[4]);
        float_type out5 = std::minus<float_type>()(arg1, arg2[5]);
        float_type out6 = std::minus<float_type>()(arg1, arg2[6]);
        float_type out7 = std::minus<float_type>()(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void minus_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::minus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::minus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::minus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::minus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::minus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::minus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::minus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::minus<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void minus_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::minus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::minus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::minus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::minus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::minus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::minus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::minus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::minus<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void times_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::multiplies<float_type>()(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void times_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::multiplies<float_type>()(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void times_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::multiplies<float_type>()(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void times_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::multiplies<float_type>()(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void times_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::multiplies<float_type>()(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void times_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::multiplies<float_type>()(arg1[0], arg2[0]);
        float_type out1 = std::multiplies<float_type>()(arg1[1], arg2[1]);
        float_type out2 = std::multiplies<float_type>()(arg1[2], arg2[2]);
        float_type out3 = std::multiplies<float_type>()(arg1[3], arg2[3]);
        float_type out4 = std::multiplies<float_type>()(arg1[4], arg2[4]);
        float_type out5 = std::multiplies<float_type>()(arg1[5], arg2[5]);
        float_type out6 = std::multiplies<float_type>()(arg1[6], arg2[6]);
        float_type out7 = std::multiplies<float_type>()(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void times_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::multiplies<float_type>()(arg1[0], arg2);
        float_type out1 = std::multiplies<float_type>()(arg1[1], arg2);
        float_type out2 = std::multiplies<float_type>()(arg1[2], arg2);
        float_type out3 = std::multiplies<float_type>()(arg1[3], arg2);
        float_type out4 = std::multiplies<float_type>()(arg1[4], arg2);
        float_type out5 = std::multiplies<float_type>()(arg1[5], arg2);
        float_type out6 = std::multiplies<float_type>()(arg1[6], arg2);
        float_type out7 = std::multiplies<float_type>()(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void times_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::multiplies<float_type>()(arg1, arg2[0]);
        float_type out1 = std::multiplies<float_type>()(arg1, arg2[1]);
        float_type out2 = std::multiplies<float_type>()(arg1, arg2[2]);
        float_type out3 = std::multiplies<float_type>()(arg1, arg2[3]);
        float_type out4 = std::multiplies<float_type>()(arg1, arg2[4]);
        float_type out5 = std::multiplies<float_type>()(arg1, arg2[5]);
        float_type out6 = std::multiplies<float_type>()(arg1, arg2[6]);
        float_type out7 = std::multiplies<float_type>()(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void times_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::multiplies<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::multiplies<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::multiplies<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::multiplies<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::multiplies<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::multiplies<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::multiplies<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::multiplies<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void times_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::multiplies<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::multiplies<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::multiplies<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::multiplies<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::multiplies<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::multiplies<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::multiplies<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::multiplies<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void over_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::divides<float_type>()(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void over_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::divides<float_type>()(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void over_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::divides<float_type>()(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void over_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::divides<float_type>()(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void over_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::divides<float_type>()(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void over_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::divides<float_type>()(arg1[0], arg2[0]);
        float_type out1 = std::divides<float_type>()(arg1[1], arg2[1]);
        float_type out2 = std::divides<float_type>()(arg1[2], arg2[2]);
        float_type out3 = std::divides<float_type>()(arg1[3], arg2[3]);
        float_type out4 = std::divides<float_type>()(arg1[4], arg2[4]);
        float_type out5 = std::divides<float_type>()(arg1[5], arg2[5]);
        float_type out6 = std::divides<float_type>()(arg1[6], arg2[6]);
        float_type out7 = std::divides<float_type>()(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void over_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::divides<float_type>()(arg1[0], arg2);
        float_type out1 = std::divides<float_type>()(arg1[1], arg2);
        float_type out2 = std::divides<float_type>()(arg1[2], arg2);
        float_type out3 = std::divides<float_type>()(arg1[3], arg2);
        float_type out4 = std::divides<float_type>()(arg1[4], arg2);
        float_type out5 = std::divides<float_type>()(arg1[5], arg2);
        float_type out6 = std::divides<float_type>()(arg1[6], arg2);
        float_type out7 = std::divides<float_type>()(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void over_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::divides<float_type>()(arg1, arg2[0]);
        float_type out1 = std::divides<float_type>()(arg1, arg2[1]);
        float_type out2 = std::divides<float_type>()(arg1, arg2[2]);
        float_type out3 = std::divides<float_type>()(arg1, arg2[3]);
        float_type out4 = std::divides<float_type>()(arg1, arg2[4]);
        float_type out5 = std::divides<float_type>()(arg1, arg2[5]);
        float_type out6 = std::divides<float_type>()(arg1, arg2[6]);
        float_type out7 = std::divides<float_type>()(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void over_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::divides<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::divides<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::divides<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::divides<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::divides<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::divides<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::divides<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::divides<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void over_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::divides<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::divides<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::divides<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::divides<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::divides<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::divides<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::divides<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::divides<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void min_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::min<float_type>(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void min_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::min<float_type>(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void min_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::min<float_type>(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void min_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::min<float_type>(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void min_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::min<float_type>(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void min_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::min<float_type>(arg1[0], arg2[0]);
        float_type out1 = std::min<float_type>(arg1[1], arg2[1]);
        float_type out2 = std::min<float_type>(arg1[2], arg2[2]);
        float_type out3 = std::min<float_type>(arg1[3], arg2[3]);
        float_type out4 = std::min<float_type>(arg1[4], arg2[4]);
        float_type out5 = std::min<float_type>(arg1[5], arg2[5]);
        float_type out6 = std::min<float_type>(arg1[6], arg2[6]);
        float_type out7 = std::min<float_type>(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void min_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::min<float_type>(arg1[0], arg2);
        float_type out1 = std::min<float_type>(arg1[1], arg2);
        float_type out2 = std::min<float_type>(arg1[2], arg2);
        float_type out3 = std::min<float_type>(arg1[3], arg2);
        float_type out4 = std::min<float_type>(arg1[4], arg2);
        float_type out5 = std::min<float_type>(arg1[5], arg2);
        float_type out6 = std::min<float_type>(arg1[6], arg2);
        float_type out7 = std::min<float_type>(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void min_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::min<float_type>(arg1, arg2[0]);
        float_type out1 = std::min<float_type>(arg1, arg2[1]);
        float_type out2 = std::min<float_type>(arg1, arg2[2]);
        float_type out3 = std::min<float_type>(arg1, arg2[3]);
        float_type out4 = std::min<float_type>(arg1, arg2[4]);
        float_type out5 = std::min<float_type>(arg1, arg2[5]);
        float_type out6 = std::min<float_type>(arg1, arg2[6]);
        float_type out7 = std::min<float_type>(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void min_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::min<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::min<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::min<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::min<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::min<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::min<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::min<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::min<float_type>(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void min_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::min<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::min<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::min<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::min<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::min<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::min<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::min<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::min<float_type>(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void max_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::max<float_type>(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void max_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::max<float_type>(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void max_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::max<float_type>(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void max_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::max<float_type>(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void max_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::max<float_type>(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void max_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::max<float_type>(arg1[0], arg2[0]);
        float_type out1 = std::max<float_type>(arg1[1], arg2[1]);
        float_type out2 = std::max<float_type>(arg1[2], arg2[2]);
        float_type out3 = std::max<float_type>(arg1[3], arg2[3]);
        float_type out4 = std::max<float_type>(arg1[4], arg2[4]);
        float_type out5 = std::max<float_type>(arg1[5], arg2[5]);
        float_type out6 = std::max<float_type>(arg1[6], arg2[6]);
        float_type out7 = std::max<float_type>(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void max_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::max<float_type>(arg1[0], arg2);
        float_type out1 = std::max<float_type>(arg1[1], arg2);
        float_type out2 = std::max<float_type>(arg1[2], arg2);
        float_type out3 = std::max<float_type>(arg1[3], arg2);
        float_type out4 = std::max<float_type>(arg1[4], arg2);
        float_type out5 = std::max<float_type>(arg1[5], arg2);
        float_type out6 = std::max<float_type>(arg1[6], arg2);
        float_type out7 = std::max<float_type>(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void max_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::max<float_type>(arg1, arg2[0]);
        float_type out1 = std::max<float_type>(arg1, arg2[1]);
        float_type out2 = std::max<float_type>(arg1, arg2[2]);
        float_type out3 = std::max<float_type>(arg1, arg2[3]);
        float_type out4 = std::max<float_type>(arg1, arg2[4]);
        float_type out5 = std::max<float_type>(arg1, arg2[5]);
        float_type out6 = std::max<float_type>(arg1, arg2[6]);
        float_type out7 = std::max<float_type>(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void max_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::max<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::max<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::max<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::max<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::max<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::max<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::max<float_type>(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::max<float_type>(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void max_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::max<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::max<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::max<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::max<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::max<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::max<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::max<float_type>(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::max<float_type>(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void less_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::less<float_type>()(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void less_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::less<float_type>()(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void less_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::less<float_type>()(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void less_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::less<float_type>()(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void less_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::less<float_type>()(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void less_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::less<float_type>()(arg1[0], arg2[0]);
        float_type out1 = std::less<float_type>()(arg1[1], arg2[1]);
        float_type out2 = std::less<float_type>()(arg1[2], arg2[2]);
        float_type out3 = std::less<float_type>()(arg1[3], arg2[3]);
        float_type out4 = std::less<float_type>()(arg1[4], arg2[4]);
        float_type out5 = std::less<float_type>()(arg1[5], arg2[5]);
        float_type out6 = std::less<float_type>()(arg1[6], arg2[6]);
        float_type out7 = std::less<float_type>()(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void less_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::less<float_type>()(arg1[0], arg2);
        float_type out1 = std::less<float_type>()(arg1[1], arg2);
        float_type out2 = std::less<float_type>()(arg1[2], arg2);
        float_type out3 = std::less<float_type>()(arg1[3], arg2);
        float_type out4 = std::less<float_type>()(arg1[4], arg2);
        float_type out5 = std::less<float_type>()(arg1[5], arg2);
        float_type out6 = std::less<float_type>()(arg1[6], arg2);
        float_type out7 = std::less<float_type>()(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void less_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::less<float_type>()(arg1, arg2[0]);
        float_type out1 = std::less<float_type>()(arg1, arg2[1]);
        float_type out2 = std::less<float_type>()(arg1, arg2[2]);
        float_type out3 = std::less<float_type>()(arg1, arg2[3]);
        float_type out4 = std::less<float_type>()(arg1, arg2[4]);
        float_type out5 = std::less<float_type>()(arg1, arg2[5]);
        float_type out6 = std::less<float_type>()(arg1, arg2[6]);
        float_type out7 = std::less<float_type>()(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void less_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::less<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void less_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::less<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void less_equal_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::less_equal<float_type>()(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void less_equal_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::less_equal<float_type>()(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void less_equal_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::less_equal<float_type>()(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void less_equal_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::less_equal<float_type>()(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void less_equal_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::less_equal<float_type>()(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void less_equal_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::less_equal<float_type>()(arg1[0], arg2[0]);
        float_type out1 = std::less_equal<float_type>()(arg1[1], arg2[1]);
        float_type out2 = std::less_equal<float_type>()(arg1[2], arg2[2]);
        float_type out3 = std::less_equal<float_type>()(arg1[3], arg2[3]);
        float_type out4 = std::less_equal<float_type>()(arg1[4], arg2[4]);
        float_type out5 = std::less_equal<float_type>()(arg1[5], arg2[5]);
        float_type out6 = std::less_equal<float_type>()(arg1[6], arg2[6]);
        float_type out7 = std::less_equal<float_type>()(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void less_equal_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::less_equal<float_type>()(arg1[0], arg2);
        float_type out1 = std::less_equal<float_type>()(arg1[1], arg2);
        float_type out2 = std::less_equal<float_type>()(arg1[2], arg2);
        float_type out3 = std::less_equal<float_type>()(arg1[3], arg2);
        float_type out4 = std::less_equal<float_type>()(arg1[4], arg2);
        float_type out5 = std::less_equal<float_type>()(arg1[5], arg2);
        float_type out6 = std::less_equal<float_type>()(arg1[6], arg2);
        float_type out7 = std::less_equal<float_type>()(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void less_equal_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::less_equal<float_type>()(arg1, arg2[0]);
        float_type out1 = std::less_equal<float_type>()(arg1, arg2[1]);
        float_type out2 = std::less_equal<float_type>()(arg1, arg2[2]);
        float_type out3 = std::less_equal<float_type>()(arg1, arg2[3]);
        float_type out4 = std::less_equal<float_type>()(arg1, arg2[4]);
        float_type out5 = std::less_equal<float_type>()(arg1, arg2[5]);
        float_type out6 = std::less_equal<float_type>()(arg1, arg2[6]);
        float_type out7 = std::less_equal<float_type>()(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void less_equal_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::less_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::less_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void less_equal_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::less_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::less_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void greater_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::greater<float_type>()(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void greater_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::greater<float_type>()(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void greater_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::greater<float_type>()(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void greater_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::greater<float_type>()(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void greater_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::greater<float_type>()(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void greater_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::greater<float_type>()(arg1[0], arg2[0]);
        float_type out1 = std::greater<float_type>()(arg1[1], arg2[1]);
        float_type out2 = std::greater<float_type>()(arg1[2], arg2[2]);
        float_type out3 = std::greater<float_type>()(arg1[3], arg2[3]);
        float_type out4 = std::greater<float_type>()(arg1[4], arg2[4]);
        float_type out5 = std::greater<float_type>()(arg1[5], arg2[5]);
        float_type out6 = std::greater<float_type>()(arg1[6], arg2[6]);
        float_type out7 = std::greater<float_type>()(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void greater_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::greater<float_type>()(arg1[0], arg2);
        float_type out1 = std::greater<float_type>()(arg1[1], arg2);
        float_type out2 = std::greater<float_type>()(arg1[2], arg2);
        float_type out3 = std::greater<float_type>()(arg1[3], arg2);
        float_type out4 = std::greater<float_type>()(arg1[4], arg2);
        float_type out5 = std::greater<float_type>()(arg1[5], arg2);
        float_type out6 = std::greater<float_type>()(arg1[6], arg2);
        float_type out7 = std::greater<float_type>()(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void greater_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::greater<float_type>()(arg1, arg2[0]);
        float_type out1 = std::greater<float_type>()(arg1, arg2[1]);
        float_type out2 = std::greater<float_type>()(arg1, arg2[2]);
        float_type out3 = std::greater<float_type>()(arg1, arg2[3]);
        float_type out4 = std::greater<float_type>()(arg1, arg2[4]);
        float_type out5 = std::greater<float_type>()(arg1, arg2[5]);
        float_type out6 = std::greater<float_type>()(arg1, arg2[6]);
        float_type out7 = std::greater<float_type>()(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void greater_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::greater<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void greater_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::greater<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void greater_equal_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::greater_equal<float_type>()(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void greater_equal_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::greater_equal<float_type>()(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void greater_equal_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::greater_equal<float_type>()(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void greater_equal_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::greater_equal<float_type>()(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void greater_equal_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::greater_equal<float_type>()(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void greater_equal_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::greater_equal<float_type>()(arg1[0], arg2[0]);
        float_type out1 = std::greater_equal<float_type>()(arg1[1], arg2[1]);
        float_type out2 = std::greater_equal<float_type>()(arg1[2], arg2[2]);
        float_type out3 = std::greater_equal<float_type>()(arg1[3], arg2[3]);
        float_type out4 = std::greater_equal<float_type>()(arg1[4], arg2[4]);
        float_type out5 = std::greater_equal<float_type>()(arg1[5], arg2[5]);
        float_type out6 = std::greater_equal<float_type>()(arg1[6], arg2[6]);
        float_type out7 = std::greater_equal<float_type>()(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void greater_equal_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::greater_equal<float_type>()(arg1[0], arg2);
        float_type out1 = std::greater_equal<float_type>()(arg1[1], arg2);
        float_type out2 = std::greater_equal<float_type>()(arg1[2], arg2);
        float_type out3 = std::greater_equal<float_type>()(arg1[3], arg2);
        float_type out4 = std::greater_equal<float_type>()(arg1[4], arg2);
        float_type out5 = std::greater_equal<float_type>()(arg1[5], arg2);
        float_type out6 = std::greater_equal<float_type>()(arg1[6], arg2);
        float_type out7 = std::greater_equal<float_type>()(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void greater_equal_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::greater_equal<float_type>()(arg1, arg2[0]);
        float_type out1 = std::greater_equal<float_type>()(arg1, arg2[1]);
        float_type out2 = std::greater_equal<float_type>()(arg1, arg2[2]);
        float_type out3 = std::greater_equal<float_type>()(arg1, arg2[3]);
        float_type out4 = std::greater_equal<float_type>()(arg1, arg2[4]);
        float_type out5 = std::greater_equal<float_type>()(arg1, arg2[5]);
        float_type out6 = std::greater_equal<float_type>()(arg1, arg2[6]);
        float_type out7 = std::greater_equal<float_type>()(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void greater_equal_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::greater_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::greater_equal<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void greater_equal_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::greater_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::greater_equal<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void equal_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::equal_to<float_type>()(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void equal_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::equal_to<float_type>()(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void equal_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::equal_to<float_type>()(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void equal_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::equal_to<float_type>()(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void equal_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::equal_to<float_type>()(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void equal_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::equal_to<float_type>()(arg1[0], arg2[0]);
        float_type out1 = std::equal_to<float_type>()(arg1[1], arg2[1]);
        float_type out2 = std::equal_to<float_type>()(arg1[2], arg2[2]);
        float_type out3 = std::equal_to<float_type>()(arg1[3], arg2[3]);
        float_type out4 = std::equal_to<float_type>()(arg1[4], arg2[4]);
        float_type out5 = std::equal_to<float_type>()(arg1[5], arg2[5]);
        float_type out6 = std::equal_to<float_type>()(arg1[6], arg2[6]);
        float_type out7 = std::equal_to<float_type>()(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void equal_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::equal_to<float_type>()(arg1[0], arg2);
        float_type out1 = std::equal_to<float_type>()(arg1[1], arg2);
        float_type out2 = std::equal_to<float_type>()(arg1[2], arg2);
        float_type out3 = std::equal_to<float_type>()(arg1[3], arg2);
        float_type out4 = std::equal_to<float_type>()(arg1[4], arg2);
        float_type out5 = std::equal_to<float_type>()(arg1[5], arg2);
        float_type out6 = std::equal_to<float_type>()(arg1[6], arg2);
        float_type out7 = std::equal_to<float_type>()(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void equal_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::equal_to<float_type>()(arg1, arg2[0]);
        float_type out1 = std::equal_to<float_type>()(arg1, arg2[1]);
        float_type out2 = std::equal_to<float_type>()(arg1, arg2[2]);
        float_type out3 = std::equal_to<float_type>()(arg1, arg2[3]);
        float_type out4 = std::equal_to<float_type>()(arg1, arg2[4]);
        float_type out5 = std::equal_to<float_type>()(arg1, arg2[5]);
        float_type out6 = std::equal_to<float_type>()(arg1, arg2[6]);
        float_type out7 = std::equal_to<float_type>()(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void equal_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void equal_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void notequal_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::not_equal_to<float_type>()(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void notequal_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = std::not_equal_to<float_type>()(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void notequal_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::not_equal_to<float_type>()(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void notequal_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = std::not_equal_to<float_type>()(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void notequal_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = std::not_equal_to<float_type>()(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void notequal_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::not_equal_to<float_type>()(arg1[0], arg2[0]);
        float_type out1 = std::not_equal_to<float_type>()(arg1[1], arg2[1]);
        float_type out2 = std::not_equal_to<float_type>()(arg1[2], arg2[2]);
        float_type out3 = std::not_equal_to<float_type>()(arg1[3], arg2[3]);
        float_type out4 = std::not_equal_to<float_type>()(arg1[4], arg2[4]);
        float_type out5 = std::not_equal_to<float_type>()(arg1[5], arg2[5]);
        float_type out6 = std::not_equal_to<float_type>()(arg1[6], arg2[6]);
        float_type out7 = std::not_equal_to<float_type>()(arg1[7], arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void notequal_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::not_equal_to<float_type>()(arg1[0], arg2);
        float_type out1 = std::not_equal_to<float_type>()(arg1[1], arg2);
        float_type out2 = std::not_equal_to<float_type>()(arg1[2], arg2);
        float_type out3 = std::not_equal_to<float_type>()(arg1[3], arg2);
        float_type out4 = std::not_equal_to<float_type>()(arg1[4], arg2);
        float_type out5 = std::not_equal_to<float_type>()(arg1[5], arg2);
        float_type out6 = std::not_equal_to<float_type>()(arg1[6], arg2);
        float_type out7 = std::not_equal_to<float_type>()(arg1[7], arg2);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void notequal_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = std::not_equal_to<float_type>()(arg1, arg2[0]);
        float_type out1 = std::not_equal_to<float_type>()(arg1, arg2[1]);
        float_type out2 = std::not_equal_to<float_type>()(arg1, arg2[2]);
        float_type out3 = std::not_equal_to<float_type>()(arg1, arg2[3]);
        float_type out4 = std::not_equal_to<float_type>()(arg1, arg2[4]);
        float_type out5 = std::not_equal_to<float_type>()(arg1, arg2[5]);
        float_type out6 = std::not_equal_to<float_type>()(arg1, arg2[6]);
        float_type out7 = std::not_equal_to<float_type>()(arg1, arg2[7]);
        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}

template <typename float_type>
inline void notequal_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::not_equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::not_equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::not_equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::not_equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::not_equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::not_equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::not_equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = std::not_equal_to<float_type>()(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void notequal_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = std::not_equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::not_equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::not_equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::not_equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::not_equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::not_equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::not_equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = std::not_equal_to<float_type>()(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}

} /* namespace nova */

#endif /* SIMD_BINARY_ARITHMETIC_GENERIC_HPP */
