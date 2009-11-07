# -*- coding: utf-8 -*-
#
# code generation system for simd framework
# Copyright (C) 2008 Tim Blechmann
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
# Boston, MA 02111-1307, USA.


from string import Template
import sys

from generator_common import *

binary_template = Template("""
template <typename float_type>
inline void ${label}_vec(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = $operation(*arg1++, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void ${label}_vec(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    do {
        *out++ = $operation(*arg1++, arg2);
    }
    while (--n);
}

template <typename float_type>
inline void ${label}_vec(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    do {
        *out++ = $operation(arg1, *arg2++);
    }
    while (--n);
}

template <typename float_type>
inline void ${label}_vec(float_type * out, const float_type * arg1, float_type arg2,
                  const float_type arg2_slope, unsigned int n)
{
    do {
        *out++ = $operation(*arg1++, arg2);
        arg2 += arg2_slope;
    }
    while (--n);
}

template <typename float_type>
inline void ${label}_vec(float_type * out, float_type arg1, const float_type arg1_slope,
                  const float_type * arg2, unsigned int n)
{
    do {
        *out++ = $operation(arg1, *arg2++);
        arg1 += arg1_slope;
    }
    while (--n);
}

template <typename float_type>
inline void ${label}_vec_simd(float_type * out, const float_type * arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = $operation(arg1[0], arg2[0]);
        float_type out1 = $operation(arg1[1], arg2[1]);
        float_type out2 = $operation(arg1[2], arg2[2]);
        float_type out3 = $operation(arg1[3], arg2[3]);
        float_type out4 = $operation(arg1[4], arg2[4]);
        float_type out5 = $operation(arg1[5], arg2[5]);
        float_type out6 = $operation(arg1[6], arg2[6]);
        float_type out7 = $operation(arg1[7], arg2[7]);
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
inline void ${label}_vec_simd(float_type * out, const float_type * arg1, const float_type arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = $operation(arg1[0], arg2);
        float_type out1 = $operation(arg1[1], arg2);
        float_type out2 = $operation(arg1[2], arg2);
        float_type out3 = $operation(arg1[3], arg2);
        float_type out4 = $operation(arg1[4], arg2);
        float_type out5 = $operation(arg1[5], arg2);
        float_type out6 = $operation(arg1[6], arg2);
        float_type out7 = $operation(arg1[7], arg2);
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
inline void ${label}_vec_simd(float_type * out, const float_type arg1, const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = $operation(arg1, arg2[0]);
        float_type out1 = $operation(arg1, arg2[1]);
        float_type out2 = $operation(arg1, arg2[2]);
        float_type out3 = $operation(arg1, arg2[3]);
        float_type out4 = $operation(arg1, arg2[4]);
        float_type out5 = $operation(arg1, arg2[5]);
        float_type out6 = $operation(arg1, arg2[6]);
        float_type out7 = $operation(arg1, arg2[7]);
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
inline void ${label}_vec_simd(float_type * out, const float_type * arg1, float_type arg2,
                              float_type arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = $operation(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = $operation(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = $operation(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = $operation(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = $operation(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = $operation(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = $operation(*arg1++, arg2); arg2 += arg2_slope;
        *out++ = $operation(*arg1++, arg2); arg2 += arg2_slope;
    }
    while (--loops);
}

template <typename float_type>
inline void ${label}_vec_simd(float_type * out, float_type arg1, const float_type arg1_slope,
                              const float_type * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        *out++ = $operation(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = $operation(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = $operation(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = $operation(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = $operation(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = $operation(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = $operation(arg1, *arg2++); arg1 += arg1_slope;
        *out++ = $operation(arg1, *arg2++); arg1 += arg1_slope;
    }
    while (--loops);
}
""")

body = binary_template.substitute(label='plus',  operation = 'std::plus<float_type>()') + \
    binary_template.substitute(label='minus', operation = 'std::minus<float_type>()') + \
    binary_template.substitute(label='times', operation = 'std::multiplies<float_type>()') + \
    binary_template.substitute(label='over', operation = 'std::divides<float_type>()') + \
    binary_template.substitute(label='min', operation = 'std::min<float_type>') + \
    binary_template.substitute(label='max', operation = 'std::max<float_type>') + \
    binary_template.substitute(label='less', operation = 'std::less<float_type>()') + \
    binary_template.substitute(label='less_equal', operation = 'std::less_equal<float_type>()') + \
    binary_template.substitute(label='greater', operation = 'std::greater<float_type>()') + \
    binary_template.substitute(label='greater_equal', operation = 'std::greater_equal<float_type>()') + \
    binary_template.substitute(label='equal', operation = 'std::equal_to<float_type>()') + \
    binary_template.substitute(label='notequal', operation = 'std::not_equal_to<float_type>()')

ret = generate_file(body, "templated arithmetic simd functions",
                    "SIMD_BINARY_ARITHMETIC_GENERIC_HPP", ["<functional>", "<algorithm>"],
                    'nova')

if (len(sys.argv) > 1):
    open(sys.argv[1], 'w').write(ret)
else:
    print ret
