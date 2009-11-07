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

def gen_fn_arg(index, arg):
    if arg == 0:
        return "const float_type * arg%d" % index
    if arg == 1:
        return "const float_type arg%d" % index
    if arg == 2:
        return "float_type arg%d, const float_type arg%d_slope" % (index, index)

def gen_fn_args(a1, a2, a3):
    ret = "%s, %s, %s" % (gen_fn_arg(1, a1),
                          gen_fn_arg(2, a2),
                          gen_fn_arg(3, a3))
    return ret

def gen_call_arg(index, arg):
    if arg == 0:
        return "*arg%d++" % index
    if arg == 1 or arg == 2:
        return "arg%d" % index

def gen_call_args(a1, a2, a3):
    ret = "%s, %s, %s" % (gen_call_arg(1, a1),
                          gen_call_arg(2, a2),
                          gen_call_arg(3, a3))
    return ret


def gen_fn_arg(index, arg):
    if arg == 0:
        return "const float_type * arg%d" % index
    if arg == 1:
        return "const float_type arg%d" % index
    if arg == 2:
        return "float_type arg%d, const float_type arg%d_slope" % (index, index)


def gen_inc_slope_(index, arg):
    if arg != 2:
        return ""

    return 8 * ' ' + "arg%d += arg%d_slope;\n" % (index, index)


def gen_inc_slope(a1, a2, a3):
    ret = gen_inc_slope_(1, a1) + \
          gen_inc_slope_(2, a2) + \
          gen_inc_slope_(3, a3)

    if ret == "":
        return ret
    return "\n" + ret.rstrip()

def gen_fn_suffix(a1, a2, a3):
    ret = ""

    if a1 == 2:
        ret += "r1"
    if a2 == 2:
        ret += "r2"
    if a3 == 2:
        ret += "r3"

    if ret != "":
        ret = "_" + ret

    return ret

def gen_template(a1, a2, a3):
    fn_args = gen_fn_args(a1, a2, a3)
    call_args = gen_call_args(a1, a2, a3)
    inc_slopes = gen_inc_slope(a1, a2, a3)
    fn_suffix = gen_fn_suffix(a1, a2, a3)

    ret = Template("""
template <typename float_type>
inline void $${label}_vec$fnsuffix(float_type * out, $fnargs, unsigned int n)
{
    do {
        *out++ = $$operation($callargs);$increments
    }
    while (--n);
}

template <typename float_type>
inline void $${label}_vec_simd$fnsuffix(float_type * out, $fnargs, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        float_type out0 = $$operation($callargs);$increments
        float_type out1 = $$operation($callargs);$increments
        float_type out2 = $$operation($callargs);$increments
        float_type out3 = $$operation($callargs);$increments
        float_type out4 = $$operation($callargs);$increments
        float_type out5 = $$operation($callargs);$increments
        float_type out6 = $$operation($callargs);$increments
        float_type out7 = $$operation($callargs);$increments
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
""")

    return ret.substitute( fnargs = fn_args, callargs = call_args, increments = inc_slopes, fnsuffix = fn_suffix)

template_string = ""

for a1 in range(3):
    for a2 in range(3):
        for a3 in range(3):
            template_string += gen_template(a1, a2, a3)


ternary_template = Template(template_string)

functions = """
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
"""

body = functions + \
       ternary_template.substitute(label='clip', operation = 'detail::clip<float_type>') + \
       ternary_template.substitute(label='muladd',  operation = 'detail::muladd<float_type>')

ret = generate_file(body, "templated arithmetic simd functions",
                    "SIMD_TERNARY_ARITHMETIC_GENERIC_HPP", ["<algorithm>"],
                    'nova')

if (len(sys.argv) > 1):
    open(sys.argv[1], 'w').write(ret)
else:
    print ret
