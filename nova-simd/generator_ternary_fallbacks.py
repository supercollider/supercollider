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

def gen_fn_arg(float_type, index, arg):
    if arg == 0:
        return "const %s * arg%d" % (float_type, index)
    if arg == 1:
        return "const %s arg%d" % (float_type, index)
    if arg == 2:
        return "%s arg%d, const %s arg%d_slope" % (float_type, index, float_type, index)

def gen_fn_args(float_type, a1, a2, a3):
    ret = "%s, %s, %s" % (gen_fn_arg(float_type, 1, a1),
                          gen_fn_arg(float_type, 2, a2),
                          gen_fn_arg(float_type, 3, a3))
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


def gen_fn_arg(float_type, index, arg):
    if arg == 0:
        return "const %s * arg%d" % (float_type, index)
    if arg == 1:
        return "const %s arg%d" % (float_type, index)
    if arg == 2:
        return "%s arg%d, const %s arg%d_slope" % (float_type, index, float_type, index)

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

def gen_initial_mp_arg(index, arg):
    if (arg == 0):
        return "arg" + str(index)
    if (arg == 1):
        return "arg%d" % index
    if (arg == 2):
        return "arg%d, arg%d_slope" % (index, index)

def gen_initial_mp_args(arg1_type, arg2_type, arg3_type):
    args = (gen_initial_mp_arg(1, arg1_type),
            gen_initial_mp_arg(2, arg2_type),
            gen_initial_mp_arg(3, arg3_type))

    mp_args = "out, %s, %s, %s"%args

    return mp_args

def gen_template(float_type, a1, a2, a3):
    fn_args = gen_fn_args(float_type, a1, a2, a3)
    call_args = gen_call_args(a1, a2, a3)
    inc_slopes = gen_inc_slope(a1, a2, a3)
    fn_suffix = gen_fn_suffix(a1, a2, a3)
    fmpargs = gen_initial_mp_args(a1, a2, a3)

    ret = Template("""
template <unsigned int n>
inline void $${label}_vec_simd$fnsuffix($float_type * out, $fnargs)
{
    $${label}_vec_simd$fnsuffix($fmpargs, n);
}

""")

    return ret.substitute( fnargs = fn_args, callargs = call_args, increments = inc_slopes, fnsuffix = fn_suffix,
                           float_type = float_type, fmpargs = fmpargs)

float_template_string = ""
double_template_string = ""

for a1 in range(3):
    for a2 in range(3):
        for a3 in range(3):
            float_template_string += gen_template("float", a1, a2, a3)
            double_template_string += gen_template("double", a1, a2, a3)


float_template = Template(float_template_string)
double_template = Template(double_template_string)

float_body = float_template.substitute(label='clip', operation = 'detail::clip<float>') + \
             float_template.substitute(label='muladd',  operation = 'detail::muladd<float>')

double_body = double_template.substitute(label='clip', operation = 'detail::clip<double>') + \
              double_template.substitute(label='muladd',  operation = 'detail::muladd<double>')

float_string = generate_file(float_body, "templated arithmetic simd functions",
                             "SIMD_TERNARY_ARITHMETIC_FALLBACKS_FLOAT_HPP", [],
                             'nova')
double_string = generate_file(double_body, "templated arithmetic simd functions",
                              "SIMD_TERNARY_ARITHMETIC_FALLBACKS_DOUBLE_HPP", [],
                              'nova')

if (len(sys.argv) > 1):
    open(sys.argv[1], 'w').write(float_string)
    open(sys.argv[2], 'w').write(double_string)
else:
    print float_string
