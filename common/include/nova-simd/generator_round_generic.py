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
from generator_unary_generic import *


functions = """
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
"""


body = functions + \
       unary_template.substitute(label='round',  operation = 'detail::round<float_type>') + \
       unary_template.substitute(label='ceil', operation = 'std::ceil') + \
       unary_template.substitute(label='floor', operation = 'std::floor') + \
       unary_template.substitute(label='frac',  operation = 'detail::frac<float_type>')

ret = generate_file(body, "simdfied round functions, generic implementation",
                    "SIMD_ROUND_GENERIC_HPP", ["<cmath>"],
                    'nova')

if (len(sys.argv) > 1):
    open(sys.argv[1], 'w').write(ret)
else:
    print ret
