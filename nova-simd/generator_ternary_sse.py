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

ternary_sse_prolog = """
#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

#define samples_per_loop nova::unroll_constraints<float>::samples_per_loop

"""

ternary_sse_epilog = """
#undef always_inline
#undef samples_per_loop
"""

def gen_mp_name_suffix(arg):
    if (arg == 0):
        return 'v'
    if (arg == 1):
        return 's'
    if (arg == 2):
        return 'r'

def gen_mp_name(arg1_type, arg2_type, arg3_type):
    function_name = "${label}_vec_simd_mp_"
    function_name += gen_mp_name_suffix(arg1_type)
    function_name += gen_mp_name_suffix(arg2_type)
    function_name += gen_mp_name_suffix(arg3_type)

    return function_name

def gen_mp_arg(index, arg):
    if (arg == 0):
        return "const float * arg" + str(index)
    if (arg == 1):
        return "const __m128 arg" + str(index)
    if (arg == 2):
        return "__m128 & arg%d, const __m128 arg%d_slope"%(index, index)

def gen_mp_args(arg1_type, arg2_type, arg3_type):
    args = (gen_mp_arg(1, arg1_type),
            gen_mp_arg(2, arg2_type),
            gen_mp_arg(3, arg3_type))

    mp_args = "float * out, %s, %s, %s"%args

    return mp_args

def gen_mp_call_arg(index, arg):
    if (arg == 0):
        return "arg%d+4" % index
    if (arg == 1):
        return "arg%d" % index
    if (arg == 2):
        return "arg%d, arg%d_slope"%(index, index)

def gen_mp_call_args(arg1_type, arg2_type, arg3_type):
    args = (gen_mp_call_arg(1, arg1_type),
            gen_mp_call_arg(2, arg2_type),
            gen_mp_call_arg(3, arg3_type))

    mp_args = "out+4, %s, %s, %s"%args

    return mp_args

def gen_operation_arg(index, arg):
    arg_name = "    __m128 a%d" % index

    if arg == 0:
        load_fn = " = _mm_load_ps(arg%d);\n"%index
    if arg == 1 or arg == 2:
        load_fn = " = arg%d;\n"%index

    return arg_name + load_fn;

def gen_operation_args(a1, a2, a3):
    ret = gen_operation_arg(1, a1) + \
          gen_operation_arg(2, a2) + \
          gen_operation_arg(3, a3)

    return ret


def gen_inc(index, arg):
    if arg != 2:
        return ""

    return 4*" " + "arg%d = _mm_add_ps(arg%d, arg%d_slope);\n" % ((index,)*3)

def gen_inc_slope(a1, a2, a3):
    ret = gen_inc(1, a1) + \
          gen_inc(2, a2) + \
          gen_inc(3, a3)
    return ret.rstrip()

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

def gen_namespace_detail(arg1_type, arg2_type, arg3_type):
    """arg coding:
    0: vector
    1: scalar
    2: sloping scalar
    """

    function_name = gen_mp_name(arg1_type, arg2_type, arg3_type)
    function_args = gen_mp_args(arg1_type, arg2_type, arg3_type)
    function_call_args = gen_mp_call_args(arg1_type, arg2_type, arg3_type)

    operation_arguments = gen_operation_args(arg1_type, arg2_type, arg3_type)

    inc_slope = gen_inc_slope(arg1_type, arg2_type, arg3_type)

    ns_detail = Template("""
namespace detail
{

template <int n>
always_inline void $fname($fargs)
{
$op_args
    __m128 ret = $$operation(a1, a2, a3);
    _mm_store_ps(out, ret);
$inc_slope

    $fname<n-4>($fcargs);
}

template <>
always_inline void $fname<0>($fargs)
{}

} /* namespace detail */

""")

    return ns_detail.substitute(fname=function_name, fargs = function_args,
                                fcargs = function_call_args,
                                op_args = operation_arguments,
                                inc_slope = inc_slope)

def gen_main_arg(index, arg):
    if arg == 0:
        return "const float * arg%d" % index
    if arg == 1:
        return "const float arg%d" % index
    if arg == 2:
        return "const float arg%d, const float arg%d_slope" % (index, index)

def gen_main_args(a1, a2, a3):
    return "%s, %s, %s" % (gen_main_arg(1, a1),
                           gen_main_arg(2, a2),
                           gen_main_arg(3, a3))

def gen_initial_mp_arg(index, arg):
    if (arg == 0):
        return "arg" + str(index)
    if (arg == 1):
        return "a%d" % index
    if (arg == 2):
        return "a%d, a%d_slope" % (index, index)

def gen_initial_mp_args(arg1_type, arg2_type, arg3_type):
    args = (gen_initial_mp_arg(1, arg1_type),
            gen_initial_mp_arg(2, arg2_type),
            gen_initial_mp_arg(3, arg3_type))

    mp_args = "out, %s, %s, %s"%args

    return mp_args

def gen_increments(a1, a2, a3):
    ret = ""

    if a1 == 0:
        ret += " " * 8 + "arg1 += samples_per_loop;\n"
    if a2 == 0:
        ret += " " * 8 + "arg2 += samples_per_loop;\n"
    if a3 == 0:
        ret += " " * 8 + "arg3 += samples_per_loop;\n"
    return ret.rstrip()


def gen_prefix_(index, arg):
    if arg == 0:
        return ""
    if arg == 1:
        return 4*" " + "__m128 a%d = _mm_set_ps1(arg%d);\n" % (index, index)
    if arg == 2:
        ret = Template("""
    __m128 a$i = _mm_setr_ps(arg$i, arg$i + arg${i}_slope,
                            arg$i + arg${i}_slope + arg${i}_slope,
                            arg$i + arg${i}_slope + arg${i}_slope + arg${i}_slope);
    __m128 a${i}_slope = _mm_set_ps1(arg${i}_slope + arg${i}_slope + arg${i}_slope + arg${i}_slope);
   """)

        return ret.substitute( i = index ).rstrip()

def gen_prefix(a1, a2, a3):
    ret = gen_prefix_(1, a1) + \
          gen_prefix_(2, a2) + \
          gen_prefix_(3, a3)
    return ret.rstrip()


def gen_function(arg1_type, arg2_type, arg3_type):

    fargs = "float * out, " + gen_main_args(arg1_type, arg2_type, arg3_type)
    fargs_mp = fargs.replace("float_type", "float")

    fname = gen_mp_name(arg1_type, arg2_type, arg3_type)
    fmpargs = gen_initial_mp_args(arg1_type, arg2_type, arg3_type)
    fn_suffix = gen_fn_suffix(arg1_type, arg2_type, arg3_type)
    increments = gen_increments(arg1_type, arg2_type, arg3_type)

    prefix = gen_prefix(arg1_type, arg2_type, arg3_type)

    function = Template("""
template <>
inline void $${label}_vec_simd$fnsuffix($fargs, unsigned int n)
{
    unsigned int loops = n / samples_per_loop;
$prefix

    do {
        detail::${fname}<samples_per_loop>($fmpargs);
        out += samples_per_loop;
$increments
    } while (--loops);
}

template<unsigned int n>
inline void $${label}_vec_simd$fnsuffix($fargs_mp)
{
$prefix

    detail::${fname}<n>($fmpargs);
}

""")

    return function.substitute(fargs = fargs, fname = fname, fmpargs = fmpargs, increments = increments, prefix = prefix,
                               fnsuffix = fn_suffix, fargs_mp = fargs_mp)


def gen_template(a1, a2, a3):
    detail = gen_namespace_detail(a1, a2, a3)
    main = gen_function(a1, a2, a3)

    return detail + main



ternary_template = ""

for a1 in range(3):
    for a2 in range(3):
        for a3 in range(3):
            ternary_template += gen_template(a1, a2, a3)


ternary_template = Template(ternary_template)

functions = """
namespace detail
{

template <>
inline __m128 clip(__m128 value, __m128 low, __m128 high)
{
    return _mm_max_ps(_mm_min_ps(value, high),
                      low);
}

template <>
inline __m128 muladd(__m128 value, __m128 mul, __m128 add)
{
    return _mm_add_ps(_mm_mul_ps(value, mul),
                      add);
}

} /* namespace detail */
"""

body = ternary_sse_prolog + functions + \
       ternary_template.substitute(label='clip', operation = 'detail::clip<__m128>') + \
       ternary_template.substitute(label='muladd',  operation = 'detail::muladd<__m128>') + \
       ternary_sse_epilog

ret = generate_file(body, "templated arithmetic simd functions",
                    "SIMD_TERNARY_ARITHMETIC_SSE_HPP", ["<xmmintrin.h>"],
                    'nova')

if (len(sys.argv) > 1):
    open(sys.argv[1], 'w').write(ret)
else:
    print ret

