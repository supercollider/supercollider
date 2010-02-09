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

#ifndef SIMD_UNIT_CONVERSION_VEC_HPP
#define SIMD_UNIT_CONVERSION_VEC_HPP

#include <cassert>

#include "libsimdmath/lib/simdmath.h"
#include "libsimdmath/lib/simdconst.h"
#include "libsimdmath/lib/powf4.h"
#include "libsimdmath/lib/logf4.h"

namespace nova
{

namespace detail
{

inline vec_float4 midi2freq(const vec_float4 midi)
{
    DEF_CONST(CF4_69, 69);
    DEF_CONST(CF4_0083, 0.083333333333f);
    DEF_CONST(CF4_440, 440);
    return CF4_440 * _powf4(CF4_2,
                            (midi - CF4_69) * CF4_0083);
}

inline vec_float4 abs_log2(const vec_float4 arg)
{
    const vec_float4 absarg = _fabsf4(arg);
    DEF_CONST(CF4_RLOG2, 1./std::log(2.));

    return _logf4(absarg) * CF4_RLOG2;
}

inline vec_float4 freq2midi(const vec_float4 freq)
{
    DEF_CONST(CF4_00227, 0.0022727272727);
    DEF_CONST(CF4_12, 12);
    DEF_CONST(CF4_69, 69);

    return abs_log2(freq * CF4_00227)  * CF4_12 + CF4_69;
}

inline vec_float4 midi2ratio(const vec_float4 midi)
{
    DEF_CONST(CF4_0083, 0.083333333333f);
    return _powf4(CF4_2, midi * CF4_0083);
}

inline vec_float4 ratio2midi(const vec_float4 ratio)
{
    DEF_CONST(CF4_12, 12);
    return CF4_12 * abs_log2(ratio);
}

inline vec_float4 oct2freq(vec_float4 note)
{
    DEF_CONST(CF4_440, 440);
    DEF_CONST(CF4_475, 4.75);
    return CF4_440 * _powf4(CF4_2, note - CF4_475);
}

inline vec_float4 freq2oct(vec_float4 freq)
{
    DEF_CONST(CF4_475, 4.75);
    DEF_CONST(CF4_00227, 0.0022727272727);
    return abs_log2(freq * CF4_00227) + CF4_475;
}

inline vec_float4 log10(const vec_float4 arg)
{
    const vec_float4 absarg = _fabsf4(arg);
    DEF_CONST(CF4_RLOG10, 1./std::log(10.));

    return _logf4(absarg) * CF4_RLOG10;
}

inline vec_float4 amp2db(vec_float4 amp)
{
    DEF_CONST(CF4_20, 20);
    return log10(amp) * CF4_20;
}

inline vec_float4 db2amp(vec_float4 db)
{
    DEF_CONST(CF4_10, 10);
    DEF_CONST(CF4_005, 0.05);
    return _powf4(CF4_10, db * CF4_005);
}

} /* namespace detail */

#define DECLARE_UNIT_CONVERSION(name, function)                         \
template <>                                                             \
inline void name##_vec_simd<float>(float * out, const float * in, unsigned int n) \
{                                                                       \
    unsigned int loops = n/4;                                           \
    const vec_float4 * inv = (const vec_float4 *)in;                    \
    vec_float4 * outv = (vec_float4 *)out;                              \
                                                                        \
    do {                                                                \
        *outv++ = function(*inv++);                                     \
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
#endif /* SIMD_UNIT_CONVERSION_VEC_HPP */
