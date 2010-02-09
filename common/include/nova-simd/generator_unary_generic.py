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

unary_template = Template("""
template <typename float_type>
inline void ${label}_vec(float_type * out, const float_type * arg, unsigned int n)
{
    do {
        *out++ = $operation(*arg++);
    }
    while (--n);
}

template <typename float_type>
inline void ${label}_vec_simd(float_type * out, const float_type * arg, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = $operation(arg[0]);
        float_type out1 = $operation(arg[1]);
        float_type out2 = $operation(arg[2]);
        float_type out3 = $operation(arg[3]);
        float_type out4 = $operation(arg[4]);
        float_type out5 = $operation(arg[5]);
        float_type out6 = $operation(arg[6]);
        float_type out7 = $operation(arg[7]);
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
""")

functions = """
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

"""


body = unary_template.substitute(label='abs',  operation = 'std::abs') + \
       unary_template.substitute(label='sgn', operation = 'detail::sgn<float_type>') + \
       unary_template.substitute(label='square', operation = 'detail::square<float_type>') + \
       unary_template.substitute(label='cube', operation = 'detail::cube<float_type>')

body = functions + body

ret = generate_file(body, "templated unary arithmetic simd functions",
                    "SIMD_UNARY_ARITHMETIC_GENERIC_HPP", ["<cmath>"],
                    'nova')

if (len(sys.argv) > 1):
    open(sys.argv[1], 'w').write(ret)
else:
    print ret
