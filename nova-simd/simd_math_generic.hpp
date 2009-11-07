//  simdfied mathematical functions, generic implementation
//  Copyright (C) 2009 Tim Blechmann
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

#ifndef SIMD_MATH_GENERIC_HPP
#define SIMD_MATH_GENERIC_HPP

#include <cmath>

namespace nova
{

#define RUN_UNARY(in, out, fn) \
    F out0 = fn(in[0]);  \
    F out1 = fn(in[1]);  \
    F out2 = fn(in[2]);  \
    F out3 = fn(in[3]);  \
    out[0] = out0;       \
    out[1] = out1;       \
    out[2] = out2;       \
    out[3] = out3;       \

#define RUN_BINARY_VV(in0, in1, out, fn)        \
    F out0 = fn(in0[0], in1[0]);                \
    F out1 = fn(in0[1], in1[1]);                \
    F out2 = fn(in0[2], in1[2]);                \
    F out3 = fn(in0[3], in1[3]);                \
    out[0] = out0;                              \
    out[1] = out1;                              \
    out[2] = out2;                              \
    out[3] = out3;                              \

#define RUN_BINARY_VS(in0, in1, out, fn)        \
    F out0 = fn(in0[0], in1);                   \
    F out1 = fn(in0[1], in1);                   \
    F out2 = fn(in0[2], in1);                   \
    F out3 = fn(in0[3], in1);                   \
    out[0] = out0;                              \
    out[1] = out1;                              \
    out[2] = out2;                              \
    out[3] = out3;                              \

#define RUN_BINARY_SV(in0, in1, out, fn)        \
    F out0 = fn(in0, in1[0]);                   \
    F out1 = fn(in0, in1[1]);                   \
    F out2 = fn(in0, in1[2]);                   \
    F out3 = fn(in0, in1[3]);                   \
    out[0] = out0;                              \
    out[1] = out1;                              \
    out[2] = out2;                              \
    out[3] = out3;                              \

template<typename F>
inline void sin4(F * out, const F * in)
{
    RUN_UNARY(in, out, std::sin);
}

template<typename F>
inline void cos4(F * out, const F * in)
{
    RUN_UNARY(in, out, std::cos);
}

template<typename F>
inline void tan4(F * out, const F * in)
{
    RUN_UNARY(in, out, std::tan);
}

template<typename F>
inline void asin4(F * out, const F * in)
{
    RUN_UNARY(in, out, std::asin);
}

template<typename F>
inline void acos4(F * out, const F * in)
{
    RUN_UNARY(in, out, std::acos);
}

template<typename F>
inline void atan4(F * out, const F * in)
{
    RUN_UNARY(in, out, std::atan);
}

template<typename F>
inline void log4(F * out, const F * in)
{
    RUN_UNARY(in, out, std::log);
}

template<typename F>
inline void log2_4(F * out, const F * in)
{
#if defined(__GXX_EXPERIMENTAL_CXX0X__)
    RUN_UNARY(in, out, std::log2);
#else
    const float rlog2 = 1.f/std::log(2.f);
    F out0 = std::log(in[0]);
    F out1 = std::log(in[1]);
    F out2 = std::log(in[2]);
    F out3 = std::log(in[3]);
    out[0] = out0 * rlog2;
    out[1] = out1 * rlog2;
    out[2] = out2 * rlog2;
    out[3] = out3 * rlog2;
#endif
}

template<typename F>
inline void log10_4(F * out, const F * in)
{
    RUN_UNARY(in, out, std::log10);
}

template<typename F>
inline void exp4(F * out, const F * in)
{
    RUN_UNARY(in, out, std::exp);
}

/** pow */
/* @{ */
template<typename F>
inline void pow4(F * out, const F * in1, const F * in2)
{
    RUN_BINARY_VV(in1, in2, out, std::pow);
}

template<typename F>
inline void pow4(F * out, const F * in1, const F in2)
{
    RUN_BINARY_VS(in1, in2, out, std::pow);
}

template<typename F>
inline void pow4(F * out, const F in1, const F * in2)
{
    RUN_BINARY_SV(in1, in2, out, std::pow);
}
/* @} */


/* @{ */
/** signed pow */
namespace detail
{

template <typename F>
inline F spow(F in0, F in1)
{
    if (in0 > 0)
        return std::pow(in0, in1);
    else
        return -std::pow(-in0, in1);
}

} /* namespace detail */

template <typename F>
inline void spow4(F * out, const F * in1, const F * in2)
{
    RUN_BINARY_VV(in1, in2, out, detail::spow);
}

template <typename F>
inline void spow4(F * out, const F * in1, const F in2)
{
    RUN_BINARY_VS(in1, in2, out, detail::spow);
}

template <typename F>
inline void spow4(F * out, const F in1, const F * in2)
{
    RUN_BINARY_SV(in1, in2, out, detail::spow);
}
/* @} */

/** signed sqrt */

namespace detail
{

template <typename F>
inline F ssqrt(F in0)
{
    if (in0 >= 0)
        return std::sqrt(in0);
    else
        return -std::sqrt(-in0);
}

} /* namespace detail */

template <typename F>
inline void ssqrt4(F * out, const F * in)
{
    RUN_UNARY(in, out, detail::ssqrt);
}

template <typename F>
inline void tanh4(F * out, const F * in)
{
    RUN_UNARY(in, out, std::tanh);
}

} /* namespace nova */

#undef RUN_UNARY
#undef RUN_BINARY_VV
#undef RUN_BINARY_SV
#undef RUN_BINARY_VS

#endif /* SIMD_MATH_GENERIC_HPP */
