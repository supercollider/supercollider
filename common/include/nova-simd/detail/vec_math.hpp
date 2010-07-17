//  genertic vectorized math functions
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

#ifndef NOVA_SIMD_DETAIL_VECMATH_HPP
#define NOVA_SIMD_DETAIL_VECMATH_HPP

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

namespace nova
{
namespace detail
{

template <typename VecType>
always_inline VecType vec_sign(VecType const & arg)
{
    typedef VecType vec;
    const vec zero       = vec::gen_zero();
    const vec one        = vec::gen_one();
    const vec sign_mask  = vec::gen_sign_mask();

    const vec nonzero    = mask_neq(arg, zero);
    const vec sign       = arg & sign_mask;

    const vec abs_ret    = nonzero & one;
    return sign | abs_ret;
}

template <typename VecType>
always_inline VecType vec_round_float(VecType const & arg)
{
    typedef VecType vec;

    const vec sign    = arg & vec::gen_sign_mask();
    const vec abs_arg = sign ^ arg;
    const vec two_to_23_ps (0x1.0p23f);
    const vec rounded = (abs_arg + two_to_23_ps) - two_to_23_ps;

    return sign ^ rounded;
}

template <typename VecType>
always_inline VecType vec_floor_float(VecType const & arg)
{
    typedef VecType vec;

    const vec rounded = vec_round_float(arg);

    const vec rounded_larger = mask_gt(rounded, arg);
    const vec add            = rounded_larger & vec::gen_one();
    return rounded - add;
}

template <typename VecType>
always_inline VecType vec_ceil_float(VecType const & arg)
{
    typedef VecType vec;

    const vec rounded = vec_round_float(arg);

    const vec rounded_smaller = mask_lt(rounded, arg);
    const vec add             = rounded_smaller & vec::gen_one();
    return rounded + add;
}

}
}

#undef always_inline

#endif /* NOVA_SIMD_DETAIL_VECMATH_HPP */
