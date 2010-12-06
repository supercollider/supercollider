//  templated arithmetic simd functions
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

#ifndef SIMD_TERNARY_ARITHMETIC_HPP
#define SIMD_TERNARY_ARITHMETIC_HPP

#include <algorithm>

#include "vec.hpp"

#include "detail/define_macros.hpp"

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif


namespace nova
{
namespace detail
{

template<typename float_type>
struct clip
{
    float_type operator()(float_type value, float_type low, float_type high) const
    {
        return max_(min_(value, high),
                    low);
    }

    typedef vec<float_type> vec_type;

    vec_type operator()(vec_type value, vec_type low, vec_type high) const
    {
        return clip<vec_type>()(value, low, high);
    }
};

template<typename float_type>
struct muladd
{
    float_type operator()(float_type value, float_type mul, float_type add) const
    {
        return value * mul + add;
    }

    typedef vec<float_type> vec_type;

    vec_type operator()(vec_type value, vec_type mul, vec_type add) const
    {
        return muladd<vec_type>()(value, mul, add);
    }
};

template<typename float_type>
struct ampmod
{
    float_type operator()(float_type signal, float_type modulator, float_type amount) const
    {
        float_type one = 1.f;
        return signal * (one + modulator * amount);
    }

    typedef vec<float_type> vec_type;

    vec_type operator()(vec_type signal, vec_type modulator, vec_type amount) const
    {
        return ampmod<vec_type>()(signal, modulator, amount);
    }
};

}


NOVA_SIMD_DEFINE_TERNARY_OPERATION(clip, detail::clip)
NOVA_SIMD_DEFINE_TERNARY_OPERATION(muladd, detail::muladd)
NOVA_SIMD_DEFINE_TERNARY_OPERATION(ampmod, detail::ampmod)

}

#undef always_inline

#endif /* SIMD_TERNARY_ARITHMETIC_HPP */
