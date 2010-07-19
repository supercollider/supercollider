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

#include <cmath>

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

template <typename VecType>
always_inline VecType vec_tanh_float(VecType const & arg)
{
    /* this order of computation (large->small->medium) seems to be the most efficient on sse*/

    const VecType sign_arg = arg & VecType::gen_sign_mask();
    const VecType abs_arg  = arg ^ sign_arg;
    const VecType one      = VecType::gen_one();
    const VecType two (2);
    const VecType maxlogf_2 (22);
    const VecType limit_small (0.625f);

    /* large values */
    const VecType abs_big          = mask_gt(abs_arg, maxlogf_2);
    const VecType result_limit_abs = one;

    /* small values */
    const VecType f1(-5.70498872745e-3);
    const VecType f2( 2.06390887954e-2);
    const VecType f3(-5.37397155531e-2);
    const VecType f4( 1.33314422036e-1);
    const VecType f5(-3.33332819422e-1);

    const VecType arg_sqr = abs_arg * abs_arg;
    const VecType result_small = ((((f1 * arg_sqr
                                     + f2) * arg_sqr
                                    + f3) * arg_sqr
                                   + f4) * arg_sqr
                                  + f5) * arg_sqr * arg
        + arg;

    const VecType abs_small = mask_lt(abs_arg, limit_small);

    /* medium values */
    const VecType result_medium_abs = one - two / (exp(abs_arg + abs_arg) + one);

    /* select from large and medium branches and set sign */
    const VecType result_lm_abs = select(result_medium_abs, result_limit_abs, abs_big);
    const VecType result_lm = result_lm_abs | sign_arg;

    const VecType result = select(result_lm, result_small, abs_small);

    return result;
}

template <typename VecType>
always_inline VecType vec_signed_pow(VecType arg1, VecType arg2)
{
    const VecType sign_arg1 = arg1 & VecType::gen_sign_mask();
    const VecType abs_arg1  = arg1 ^ sign_arg1;

    const VecType result = pow(abs_arg1, arg2);

    return sign_arg1 | result;
}

template <typename VecType>
always_inline VecType vec_signed_sqrt(VecType arg)
{
    const VecType sign_arg1 = arg & VecType::gen_sign_mask();
    const VecType abs_arg1  = arg ^ sign_arg1;

    const VecType result = sqrt(abs_arg1);

    return sign_arg1 | result;
}

template <typename VecType>
always_inline VecType vec_log2(VecType arg)
{
    const double rlog2 = 1.0/std::log(2.0);
    return log(arg) * VecType(rlog2);
}

template <typename VecType>
always_inline VecType vec_log10(VecType arg)
{
    const double rlog10 = 1.0/std::log(10.0);
    return log(arg) * VecType(rlog10);
}

}
}

#undef always_inline

#endif /* NOVA_SIMD_DETAIL_VECMATH_HPP */
