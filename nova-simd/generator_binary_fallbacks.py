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

binary_fallbacks_template = Template("""
/* vector/vector */
template <int n>
inline void ${label}_vec_simd_mp(${float} * out, const ${float} * src1, const ${float} * src2)
{
    ${label}_vec_simd(out, src1, src2, n);
}

template <int n>
inline void ${label}_vec_simd(${float} * out, const ${float} * src1, const ${float} * src2)
{
    ${label}_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void ${label}_vec_simd_mp(${float} * out, const ${float} * src1, const ${float} src2)
{
    ${label}_vec_simd(out, src1, src2, n);
}

template <int n>
inline void ${label}_vec_simd(${float} * out, const ${float} * src1, const ${float} src2)
{
    ${label}_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void ${label}_vec_simd_mp(${float} * out, const ${float} src1, const ${float} * src2)
{
    ${label}_vec_simd(out, src1, src2, n);
}

template <int n>
inline void ${label}_vec_simd(${float} * out, const ${float} src1, const ${float} * src2)
{
    ${label}_vec_simd_mp<n>(out, src1, src2);
}

""")

def generate_binary_fallbacks(file, float_type):
    body = binary_fallbacks_template.substitute(label='plus', float=float_type) + \
        binary_fallbacks_template.substitute(label='minus', float=float_type) + \
        binary_fallbacks_template.substitute(label='times', float=float_type) + \
        binary_fallbacks_template.substitute(label='over', float=float_type) + \
        binary_fallbacks_template.substitute(label='min', float=float_type) + \
        binary_fallbacks_template.substitute(label='max', float=float_type) + \
        binary_fallbacks_template.substitute(label='less', float=float_type) + \
        binary_fallbacks_template.substitute(label='less_equal', float=float_type) + \
        binary_fallbacks_template.substitute(label='greater', float=float_type) + \
        binary_fallbacks_template.substitute(label='greater_equal', float=float_type) + \
        binary_fallbacks_template.substitute(label='equal', float=float_type) + \
        binary_fallbacks_template.substitute(label='notequal', float=float_type)

    comment = "binary arithmetic simd function fallbacks, %s version" % float_type

    guard = "SIMD_BINARY_ARITHMETIC_FALLBACKS_%s_HPP" % float_type.upper()

    ret = generate_file(body, comment, guard, ["\"simd_binary_arithmetic.hpp\""],
                        'nova')
    file.write(ret)


if (len(sys.argv) > 1):
    generate_binary_fallbacks(open(sys.argv[1], 'w'), 'float')
    generate_binary_fallbacks(open(sys.argv[2], 'w'), 'double')
else:
    generate_binary_fallbacks(sys.stdout, 'float')
