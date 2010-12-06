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

#ifndef SIMD_UNIT_CONVERSION_HPP
#define SIMD_UNIT_CONVERSION_HPP

#include "simd_unary_arithmetic.hpp"
#include "simd_math.hpp"

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

namespace nova
{
namespace detail
{

template <typename FloatType>
struct abs_log2
{
    always_inline FloatType operator()(const FloatType & arg) const
    {
        return log2_<FloatType>()(fabs_<FloatType>()(arg));
    }
};

template <typename FloatType>
struct midi2freq
{
    always_inline FloatType operator()(const FloatType & midi) const
    {
        return (FloatType)440. * pow_<FloatType>()(FloatType(2.),
                                                     (midi - FloatType(69.)) * FloatType(0.083333333333));
    }
};

template <typename FloatType>
struct freq2midi
{
    always_inline FloatType operator()(const FloatType & freq) const
    {
        return abs_log2<FloatType>()(freq * FloatType(0.0022727272727))  * FloatType(12.) + FloatType(69.);
    }
};

template <typename FloatType>
struct midi2ratio
{
    always_inline FloatType operator()(const FloatType & midi) const
    {
        return pow_<FloatType>()(FloatType(2.), midi * FloatType(0.083333333333));
    }
};

template <typename FloatType>
struct ratio2midi
{
    always_inline FloatType operator()(const FloatType & ratio) const
    {
        return FloatType(12.) * log2_<FloatType>()(ratio);
    }
};

template <typename FloatType>
struct oct2freq
{
    always_inline FloatType operator()(const FloatType & note) const
    {
        return FloatType(440.) * pow_<FloatType>()(FloatType(2.), note - FloatType(4.75));
    }
};


template <typename FloatType>
struct freq2oct
{
    always_inline FloatType operator()(const FloatType & freq) const
    {
        return abs_log2<FloatType>()(freq * FloatType(0.0022727272727)) + FloatType(4.75);
    }
};

template <typename FloatType>
struct amp2db
{
    always_inline FloatType operator()(const FloatType & amp) const
    {
        return log10_<FloatType>()(fabs_<FloatType>()(amp)) * FloatType(20.);
    }
};

template <typename FloatType>
struct db2amp
{
    always_inline FloatType operator()(const FloatType & db) const
    {
        return pow_<FloatType>()(FloatType(10.), db * FloatType(0.05));
    }
};

}


NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(midi2freq, detail::midi2freq)
NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(freq2midi, detail::freq2midi)

NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(midi2ratio, detail::midi2ratio)
NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(ratio2midi, detail::ratio2midi)

NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(oct2freq, detail::oct2freq)
NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(freq2oct, detail::freq2oct)

NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(amp2db, detail::amp2db)
NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(db2amp, detail::db2amp)

}

#undef always_inline

#endif /* SIMD_UNIT_CONVERSION_HPP */
