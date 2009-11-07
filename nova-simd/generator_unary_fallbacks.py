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

unary_fallbacks_template = Template("""
template <int n>
inline void ${label}_vec_simd(${float} * out, const ${float} * src)
{
    ${label}_vec_simd(out, src, n);
}
""")

def generate_unary_fallbacks(file, float_type):
    body = unary_fallbacks_template.substitute(label='abs', float=float_type) + \
        unary_fallbacks_template.substitute(label='sgn', float=float_type) + \
        unary_fallbacks_template.substitute(label='square', float=float_type) + \
        unary_fallbacks_template.substitute(label='cube', float=float_type)

    comment = "unary arithmetic simd function fallbacks, %s version" % float_type

    guard = "SIMD_UNARY_ARITHMETIC_FALLBACKS_%s_HPP" % float_type.upper()

    ret = generate_file(body, comment, guard, ["\"simd_unary_arithmetic.hpp\""],
                        'nova')
    file.write(ret)


if (len(sys.argv) > 1):
    generate_unary_fallbacks(open(sys.argv[1], 'w'), 'float')
    generate_unary_fallbacks(open(sys.argv[2], 'w'), 'double')
else:
    generate_unary_fallbacks(sys.stdout, 'float')
