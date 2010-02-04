//  simdfied unit conversion functions
//  Copyright (C) 2010 Tim Blechmann
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

#ifndef SIMD_UNIT_CONVERSION_GENERIC_HPP
#define SIMD_UNIT_CONVERSION_GENERIC_HPP

#include <cassert>
#include <cmath>

#include "simd_binary_arithmetic.hpp"
#include "simd_math.hpp"
#include "simd_math_generic.hpp" /* for log2 */

namespace nova
{

namespace detail
{

template <typename float_type>
inline float_type abs_log2(const float_type arg)
{
    return log2(std::abs(arg));
}

template <typename float_type>
inline float_type midi2freq(const float_type midi)
{
    return (float_type)440. * std::pow((float_type)2.,
                                       (midi - (float_type)69.) * (float_type)0.083333333333);
}

template <typename float_type>
inline float_type freq2midi(const float_type freq)
{
    return abs_log2(freq * (float_type)0.0022727272727)  * (float_type)12. + (float_type)69.;
}

template <typename float_type>
inline float_type midi2ratio(const float_type midi)
{
    return std::pow((float_type)2., midi * (float_type)0.083333333333);
}

template <typename float_type>
inline float_type ratio2midi(const float_type ratio)
{
    assert(ratio > 0);
    return (float_type)12. * log2(ratio);
}

template <typename float_type>
inline float_type oct2freq(float_type note)
{
    return (float_type)440. * std::pow((float_type)2., note - (float_type)4.75);
}

template <typename float_type>
inline float_type freq2oct(float_type freq)
{
    return abs_log2(freq * (float_type)0.0022727272727) + (float_type)4.75;
}

template <typename float_type>
inline float_type amp2db(float_type amp)
{
    return std::log10(std::abs(amp)) * (float_type)20.;
}

template <typename float_type>
inline float_type db2amp(float_type db)
{
    return std::pow((float_type)10., db * (float_type).05);
}

} /* namespace detail */

#define DECLARE_UNIT_CONVERSION(name, function)                         \
template <typename float_type>                                          \
inline void name##_vec(float_type * out, const float_type * in, unsigned int n) \
{                                                                       \
    do {                                                                \
        *out++ = function(*in++);                                       \
    }                                                                   \
    while (--n);                                                        \
}                                                                       \
                                                                        \
template <typename float_type>                                          \
inline void name##_vec_simd(float_type * out, const float_type * in, unsigned int n) \
{                                                                       \
    unsigned int loops = n/4;                                           \
    do {                                                                \
        *out++ = function(*in++);                                       \
        *out++ = function(*in++);                                       \
        *out++ = function(*in++);                                       \
        *out++ = function(*in++);                                       \
    }                                                                   \
    while (--loops);                                                    \
}

DECLARE_UNIT_CONVERSION(midi2freq, detail::midi2freq)
DECLARE_UNIT_CONVERSION(freq2midi, detail::freq2midi)
DECLARE_UNIT_CONVERSION(midi2ratio, detail::midi2ratio)
DECLARE_UNIT_CONVERSION(ratio2midi, detail::ratio2midi)
DECLARE_UNIT_CONVERSION(oct2freq, detail::oct2freq)
DECLARE_UNIT_CONVERSION(freq2oct, detail::freq2oct)
DECLARE_UNIT_CONVERSION(db2amp, detail::db2amp)
DECLARE_UNIT_CONVERSION(amp2db, detail::amp2db)

} /* namespace nova */

#undef DECLARE_UNIT_CONVERSION
#endif /* SIMD_UNIT_CONVERSION_GENERIC_HPP */
