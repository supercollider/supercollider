# -*- coding: utf-8 -*-
#
# code generation system for simd framework
# Copyright (C) 2008, 2009 Tim Blechmann
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

binary_sse_prolog = """
#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

#define samples_per_loop nova::unroll_constraints<float>::samples_per_loop

"""

binary_sse_epilog = """
#undef always_inline
#undef samples_per_loop
"""


binary_sse_template = Template("""
namespace detail {

template <int n>
always_inline void ${label}_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    const __m128 in1 = _mm_load_ps(src1);
    const __m128 in2 = _mm_load_ps(src2);

    const __m128 result = ${opcode}(in1, in2);

    _mm_store_ps(out, result);

    ${label}_vec_simd_mp<n-4>(out+4, src1+4, src2+4);
}

template <>
always_inline void ${label}_vec_simd_mp<0>(float * out, const float * src1, const float * src2)
{}

template <int n>
always_inline void ${label}_vec_simd_mp_iteration(float * out, const float * src1, const __m128 & src2)
{
    const __m128 in1 = _mm_load_ps(src1);
    const __m128 in2 = _mm_load_ps(src1+4);

    const __m128 result1 = ${opcode}(in1, src2);
    const __m128 result2 = ${opcode}(in2, src2);

    _mm_store_ps(out, result1);
    _mm_store_ps(out+4, result2);

    ${label}_vec_simd_mp_iteration<n-8>(out+8, src1+8, src2);
}

template <>
always_inline void ${label}_vec_simd_mp_iteration<0>(float * out, const float * src1, const __m128 & src2)
{}

template <int n>
always_inline void ${label}_vec_simd_mp_iteration(float * out, const __m128 & src1, const float * src2)
{
    const __m128 in1 = _mm_load_ps(src2);
    const __m128 in2 = _mm_load_ps(src2+4);

    const __m128 result1 = ${opcode}(src1, in1);
    const __m128 result2 = ${opcode}(src1, in2);

    _mm_store_ps(out, result1);
    _mm_store_ps(out+4, result2);

    ${label}_vec_simd_mp_iteration<n-8>(out+8, src1, src2+8);
}

template <>
always_inline void ${label}_vec_simd_mp_iteration<0>(float * out, const __m128 & src1, const float * src2)
{}

} /* namespace detail */


/* vector/vector */
using detail::${label}_vec_simd_mp;

template <int n>
void ${label}_vec_simd(float * out, const float * src1, const float * src2)
{
    ${label}_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
always_inline void ${label}_vec_simd_mp(float * out, const float * src1, const float src2)
{
    __m128 in2 = _mm_set_ps1(src2);
    detail::${label}_vec_simd_mp_iteration<n>(out, src1, in2);
}

template <int n>
void ${label}_vec_simd(float * out, const float * src1, const float src2)
{
    ${label}_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
always_inline void ${label}_vec_simd_mp(float * out, const float src1, const float * src2)
{
    __m128 in1 = _mm_set_ps1(src1);
    detail::${label}_vec_simd_mp_iteration<n>(out, in1, src2);
}

template <int n>
void ${label}_vec_simd(float * out, const float src1, const float * src2)
{
    ${label}_vec_simd_mp<n>(out, src1, src2);
}


template <>
inline void ${label}_vec_simd(float * out, const float * arg1, const float * arg2, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    do {
        detail::${label}_vec_simd_mp<samples_per_loop>(out, arg1, arg2);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg2 += samples_per_loop;
    }
    while (--loops);
}

template <>
inline void ${label}_vec_simd(float * out, const float * arg1, const float arg2, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    const __m128 in2 = _mm_set_ps1(arg2);
    do {
        detail::${label}_vec_simd_mp_iteration<samples_per_loop>(out, arg1, in2);
        out += samples_per_loop;
        arg1 += samples_per_loop;
    }
    while (--loops);
}

template <>
inline void ${label}_vec_simd(float * out, const float arg1, const float * arg2, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    const __m128 in1 = _mm_set_ps1(arg1);
    do {
        detail::${label}_vec_simd_mp_iteration<samples_per_loop>(out, in1, arg2);
        out += samples_per_loop;
        arg2 += samples_per_loop;
    }
    while (--loops);
}

template <>
inline void ${label}_vec_simd(float * out, const float * arg1, float arg2,
                              float arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    __m128 in2   = _mm_setr_ps(arg2,
                               arg2 + arg2_slope,
                               arg2 + arg2_slope + arg2_slope,
                               arg2 + arg2_slope + arg2_slope + arg2_slope);
    __m128 vslope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);

    do {
        __m128 in1 = _mm_load_ps(arg1);
        __m128 result = ${opcode}(in1, in2);

        _mm_store_ps(out, result);
        in2 = _mm_add_ps(in2, vslope);

        in1 = _mm_load_ps(arg1+4);
        result = ${opcode}(in1, in2);

        _mm_store_ps(out+4, result);
        in2 = _mm_add_ps(in2, vslope);

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <>
inline void ${label}_vec_simd(float * out, float arg1, const float arg1_slope,
                              const float * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    __m128 in1   = _mm_setr_ps(arg1,
                               arg1 + arg1_slope,
                               arg1 + arg1_slope + arg1_slope,
                               arg1 + arg1_slope + arg1_slope + arg1_slope);
    __m128 vslope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);

    do {
        __m128 in2 = _mm_load_ps(arg2);
        __m128 result = ${opcode}(in1, in2);

        _mm_store_ps(out, result);
        in1 = _mm_add_ps(in1, vslope);

        in2 = _mm_load_ps(arg2+4);
        result = ${opcode}(in1, in2);

        _mm_store_ps(out+4, result);
        in1 = _mm_add_ps(in1, vslope);

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}
""")


binary_sse_compare = Template("""
namespace detail {

template <int n>
always_inline void ${label}_vec_simd_mp(float * out, const float * src1, const float * src2, const __m128 one)
{
    const __m128 in1 = _mm_load_ps(src1);
    const __m128 in2 = _mm_load_ps(src2);

    const __m128 result = _mm_and_ps(${opcode}(in1, in2), one);

    _mm_store_ps(out, result);

    ${label}_vec_simd_mp<n-4>(out+4, src1+4, src2+4, one);
}

template <>
always_inline void ${label}_vec_simd_mp<0>(float * out, const float * src1, const float * src2, const __m128 one)
{}

template <int n>
always_inline void ${label}_vec_simd_mp_iteration(float * out, const float * src1, const __m128 & src2, const __m128 one)
{
    const __m128 in1 = _mm_load_ps(src1);
    const __m128 in2 = _mm_load_ps(src1+4);

    const __m128 result1 = _mm_and_ps(${opcode}(in1, src2), one);
    const __m128 result2 = _mm_and_ps(${opcode}(in2, src2), one);

    _mm_store_ps(out, result1);
    _mm_store_ps(out+4, result2);

    ${label}_vec_simd_mp_iteration<n-8>(out+8, src1+8, src2, one);
}

template <>
always_inline void ${label}_vec_simd_mp_iteration<0>(float * out, const float * src1, const __m128 & src2, const __m128 one)
{}

template <int n>
always_inline void ${label}_vec_simd_mp_iteration(float * out, const __m128 & src1, const float * src2, const __m128 one)
{
    const __m128 in1 = _mm_load_ps(src2);
    const __m128 in2 = _mm_load_ps(src2+4);

    const __m128 result1 = _mm_and_ps(${opcode}(src1, in1), one);
    const __m128 result2 = _mm_and_ps(${opcode}(src1, in2), one);

    _mm_store_ps(out, result1);
    _mm_store_ps(out+4, result2);

    ${label}_vec_simd_mp_iteration<n-8>(out+8, src1, src2+8, one);
}

template <>
always_inline void ${label}_vec_simd_mp_iteration<0>(float * out, const __m128 & src1, const float * src2, const __m128 one)
{}

} /* namespace detail */


/* vector/vector */
template <int n>
always_inline void ${label}_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    detail::${label}_vec_simd_mp<n>(out, src1, src2, detail::gen_one());
}

template <int n>
void ${label}_vec_simd(float * out, const float * src1, const float * src2)
{
    ${label}_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
always_inline void ${label}_vec_simd_mp(float * out, const float * src1, const float src2)
{
    __m128 in2 = _mm_set_ps1(src2);
    detail::${label}_vec_simd_mp_iteration<n>(out, src1, in2, detail::gen_one());
}

template <int n>
void ${label}_vec_simd(float * out, const float * src1, const float src2)
{
    ${label}_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
always_inline void ${label}_vec_simd_mp(float * out, const float src1, const float * src2)
{
    __m128 in1 = _mm_set_ps1(src1);
    detail::${label}_vec_simd_mp_iteration<n>(out, in1, src2, detail::gen_one());
}

template <int n>
void ${label}_vec_simd(float * out, const float src1, const float * src2)
{
    ${label}_vec_simd_mp<n>(out, src1, src2);
}


template <>
inline void ${label}_vec_simd(float * out, const float * arg1, const float * arg2, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    __m128 one = detail::gen_one();
    do {
        detail::${label}_vec_simd_mp<samples_per_loop>(out, arg1, arg2, one);
        out += samples_per_loop;
        arg1 += samples_per_loop;
        arg2 += samples_per_loop;
    }
    while (--loops);
}

template <>
inline void ${label}_vec_simd(float * out, const float * arg1, const float arg2, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    const __m128 in2 = _mm_set_ps1(arg2);
    __m128 one = detail::gen_one();
    do {
        detail::${label}_vec_simd_mp_iteration<samples_per_loop>(out, arg1, in2, one);
        out += samples_per_loop;
        arg1 += samples_per_loop;
    }
    while (--loops);
}

template <>
inline void ${label}_vec_simd(float * out, const float arg1, const float * arg2, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    const __m128 in1 = _mm_set_ps1(arg1);
    __m128 one = detail::gen_one();
    do {
        detail::${label}_vec_simd_mp_iteration<samples_per_loop>(out, in1, arg2, one);
        out += samples_per_loop;
        arg2 += samples_per_loop;
    }
    while (--loops);
}

template <>
inline void ${label}_vec_simd(float * out, const float * arg1, float arg2,
                              float arg2_slope, unsigned int n)
{
    unsigned int loops = n / 8;
    __m128 in2   = _mm_setr_ps(arg2,
                               arg2 + arg2_slope,
                               arg2 + arg2_slope + arg2_slope,
                               arg2 + arg2_slope + arg2_slope + arg2_slope);
    const __m128 vslope = _mm_set_ps1(arg2_slope + arg2_slope + arg2_slope + arg2_slope);
    const __m128 one = detail::gen_one();

    do {
        __m128 in1 = _mm_load_ps(arg1);
        __m128 result = _mm_and_ps(${opcode}(in1, in2), one);

        _mm_store_ps(out, result);
        in2 = _mm_add_ps(in2, vslope);

        in1 = _mm_load_ps(arg1+4);
        result = _mm_and_ps(${opcode}(in1, in2), one);

        _mm_store_ps(out+4, result);
        in2 = _mm_add_ps(in2, vslope);

        out += 8;
        arg1 += 8;
    }
    while (--loops);
}

template <>
inline void ${label}_vec_simd(float * out, float arg1, const float arg1_slope,
                              const float * arg2, unsigned int n)
{
    unsigned int loops = n / 8;
    __m128 in1   = _mm_setr_ps(arg1,
                               arg1 + arg1_slope,
                               arg1 + arg1_slope + arg1_slope,
                               arg1 + arg1_slope + arg1_slope + arg1_slope);
    const __m128 vslope = _mm_set_ps1(arg1_slope + arg1_slope + arg1_slope + arg1_slope);
    const __m128 one = detail::gen_one();

    do {
        __m128 in2 = _mm_load_ps(arg2);
        __m128 result = _mm_and_ps(${opcode}(in1, in2), one);

        _mm_store_ps(out, result);
        in1 = _mm_add_ps(in1, vslope);

        in2 = _mm_load_ps(arg2+4);
        result = _mm_and_ps(${opcode}(in1, in2), one);

        _mm_store_ps(out+4, result);
        in1 = _mm_add_ps(in1, vslope);

        out += 8;
        arg2 += 8;
    }
    while (--loops);
}
""")


body = binary_sse_template.substitute(label='plus', opcode='_mm_add_ps') + \
    binary_sse_template.substitute(label='minus', opcode='_mm_sub_ps') + \
    binary_sse_template.substitute(label='times', opcode='_mm_mul_ps') + \
    binary_sse_template.substitute(label='over', opcode='_mm_div_ps') + \
    binary_sse_template.substitute(label='min', opcode='_mm_min_ps') + \
    binary_sse_template.substitute(label='max', opcode='_mm_max_ps') + \
    binary_sse_compare.substitute(label='less', opcode='_mm_cmplt_ps') + \
    binary_sse_compare.substitute(label='less_equal', opcode='_mm_cmple_ps') + \
    binary_sse_compare.substitute(label='greater', opcode='_mm_cmpgt_ps') + \
    binary_sse_compare.substitute(label='greater_equal', opcode='_mm_cmpge_ps') + \
    binary_sse_compare.substitute(label='equal', opcode='_mm_cmpeq_ps') + \
    binary_sse_compare.substitute(label='notequal', opcode='_mm_cmpneq_ps')


body = binary_sse_prolog + body + binary_sse_epilog

ret = generate_file(body, "binary arithmetic simd functions for sse",
                    "SIMD_BINARY_ARITHMETIC_SSE_HPP", ["<xmmintrin.h>", "\"simd_utils.hpp\""],
                    'nova')

if (len(sys.argv) > 1):
    open(sys.argv[1], 'w').write(ret)
else:
    print ret
