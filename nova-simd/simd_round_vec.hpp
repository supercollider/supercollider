//  simdfied round functions, libsimdmath wrapper
//  Copyright (C) 2009 Tim Blechmann <tim@klingt.org>
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.
//
//  implemented as part of nova

#ifndef SIMD_ROUND_VEC_HPP
#define SIMD_ROUND_VEC_HPP

#include "libsimdmath/lib/floorf4.h"

namespace nova {

#define LIBSIMDMATH_WRAPPER(NAME)                                       \
template <>                                                             \
inline void NAME##_vec_simd(float * out, const float * arg, unsigned int n) \
{                                                                       \
    unsigned int loops = n / 8;                                         \
    do {                                                                \
        const vec_float4 * in0 = (const vec_float4*)arg;                \
        const vec_float4 * in1 = (const vec_float4*)(arg + 4);          \
        vec_float4 * out0 = (vec_float4*)out;                           \
        vec_float4 * out1 = (vec_float4*)(out+4);                       \
        *out0 = _##NAME##f4(*in0);                                      \
        *out1 = _##NAME##f4(*in1);                                      \
                                                                        \
        out += 8;                                                       \
        arg += 8;                                                       \
    }                                                                   \
    while (--loops);                                                    \
}                                                                       \
                                                                        \
template <unsigned int n>                                               \
inline void NAME##_vec_simd_mp(float * out, const float * arg)          \
{                                                                       \
    unsigned int loops = n / 8;                                         \
    do {                                                                \
        const vec_float4 * in0 = (const vec_float4*)arg;                \
        const vec_float4 * in1 = (const vec_float4*)(arg + 4);          \
        vec_float4 * out0 = (vec_float4*)out;                           \
        vec_float4 * out1 = (vec_float4*)(out+4);                       \
        *out0 = _##NAME##f4(*in0);                                      \
        *out1 = _##NAME##f4(*in1);                                      \
                                                                        \
        out += 8;                                                       \
        arg += 8;                                                       \
    }                                                                   \
    while (--loops);                                                    \
}                                                                       \
template <unsigned int n>                                               \
inline void NAME##_vec_simd(float * out, const float * arg)             \
{                                                                       \
    NAME##_vec_simd_mp<n>(out, arg);                                    \
}


LIBSIMDMATH_WRAPPER(round)
LIBSIMDMATH_WRAPPER(floor)
LIBSIMDMATH_WRAPPER(ceil)

template <>
inline void frac_vec_simd(float * out, const float * arg, unsigned int n)
{
    unsigned int loops = n / 8;
    do {
        const vec_float4 * in0 = (const vec_float4*)arg;
        const vec_float4 * in1 = (const vec_float4*)(arg + 4);
        vec_float4 * out0 = (vec_float4*)out;
        vec_float4 * out1 = (vec_float4*)(out+4);
        *out0 = *in0 - _roundf4(*in0);
        *out1 = *in1 - _roundf4(*in1);

        out += 8;
        arg += 8;
    }
    while (--loops);
}

template <unsigned int n>
inline void frac_vec_simd(float * out, const float * arg)
{
    unsigned int loops = n / 8;
    do {
        const vec_float4 * in0 = (const vec_float4*)arg;
        const vec_float4 * in1 = (const vec_float4*)(arg + 4);
        vec_float4 * out0 = (vec_float4*)out;
        vec_float4 * out1 = (vec_float4*)(out+4);
        *out0 = *in0 - _roundf4(*in0);
        *out1 = *in1 - _roundf4(*in1);

        out += 8;
        arg += 8;
    }
    while (--loops);
}

template <unsigned int n>
inline void frac_vec_simd_mp(float * out, const float * arg)
{
    frac_vec_simd<n>(out, arg);
}

} /* namespace nova */

#endif /* SIMD_ROUND_VEC_HPP */
