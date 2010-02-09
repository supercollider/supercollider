# -*- coding: utf-8 -*-
#
# code generation system for simd framework
# Copyright (C) 2009 Tim Blechmann
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

unary_sse_prolog = """
#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

#define samples_per_loop nova::unroll_constraints<float>::samples_per_loop

"""

unary_sse_epilog = """
#undef always_inline
#undef samples_per_loop
"""


unary_sse_template = Template("""
namespace detail {

template <int n>
always_inline void ${label}_vec_simd_mp(float * out, const float * src)
{
    const __m128 in = _mm_load_ps(src);
    const __m128 result = ${opcode}(in);
    _mm_store_ps(out, result);

    ${label}_vec_simd_mp<n-4>(out+4, src+4);
}

template <>
always_inline void ${label}_vec_simd_mp<0>(float * out, const float * src)
{}

} /* namespace detail */

using detail::${label}_vec_simd_mp;

template <int n>
void ${label}_vec_simd(float * out, const float * src)
{
    ${label}_vec_simd_mp<n>(out, src);
}

template <>
inline void ${label}_vec_simd(float * out, const float * arg, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
    do {
        detail::${label}_vec_simd_mp<samples_per_loop>(out, arg);
        out += samples_per_loop;
        arg += samples_per_loop;
    }
    while (--loops);
}
""")

functions = """
namespace detail
{

always_inline __m128 round(__m128 v)
{
    return _mm_round_ps(v, _MM_FROUND_TO_NEAREST_INT);
}

always_inline __m128 frac(__m128 v)
{
    return _mm_sub_ps(v,
                      _mm_round_ps(v, _MM_FROUND_TO_NEAREST_INT));
}

} /* namespace detail */

"""

body = functions + \
    unary_sse_template.substitute(label='round', opcode='detail::round') + \
    unary_sse_template.substitute(label='ceil', opcode='_mm_ceil_ps') + \
    unary_sse_template.substitute(label='floor', opcode='_mm_floor_ps') + \
    unary_sse_template.substitute(label='frac', opcode='detail::frac')


body = unary_sse_prolog + body + unary_sse_epilog

ret = generate_file(body, "simdfied round functions, sse4.1 implementation",
                    "SIMD_ROUND_SSE41_HPP", ["<smmintrin.h>"],
                    'nova')

if (len(sys.argv) > 1):
    open(sys.argv[1], 'w').write(ret)
else:
    print ret
