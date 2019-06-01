//  basic arithmetic objects
//  Copyright (C) 2005, 2006, 2007, 2008  Tim Blechmann
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

#pragma once

#include <cmath>
#include <algorithm> // for std::min & std::max

#include <boost/math/special_functions/fpclassify.hpp>

#include "force_inline.hpp"

namespace nova {
template <typename T> always_inline T identity(T arg) { return arg; }

template <typename T> always_inline T plus(T lhs, T rhs) { return lhs + rhs; }

template <typename T> always_inline T minus(T lhs, T rhs) { return lhs - rhs; }

template <typename F> always_inline F inv_minus(F lhs, F rhs) { return rhs - lhs; }

template <typename T> always_inline T times(T lhs, T rhs) { return lhs * rhs; }

template <typename T> always_inline T over(T lhs, T rhs) { return lhs / rhs; }

template <typename F> always_inline F inv_over(F lhs, F rhs) { return rhs / lhs; }

template <typename F> always_inline F fmod(F lhs, F rhs) { return std::fmod(lhs, rhs); }


template <typename F> always_inline F less(F lhs, F rhs) { return F(lhs < rhs); }

template <typename F> always_inline F less_equal(F lhs, F rhs) { return F(lhs <= rhs); }

template <typename F> always_inline F equal(F lhs, F rhs) { return F(lhs == rhs); }

template <typename F> always_inline F notequal(F lhs, F rhs) { return F(lhs != rhs); }

template <typename F> always_inline F greater(F lhs, F rhs) { return F(lhs > rhs); }

template <typename F> always_inline F greater_equal(F lhs, F rhs) { return F(lhs >= rhs); }

using std::max;
using std::min;


template <typename F> always_inline F nova_max(F lhs, F rhs) { return max(lhs, rhs); }

template <typename F> always_inline F nova_min(F lhs, F rhs) { return min(lhs, rhs); }

template <typename F> always_inline F sign(F arg) {
    if (arg > 0)
        return F(1);
    if (arg < 0)
        return F(-1);
    return F(0);
}

template <typename F> always_inline F and_bool(F lhs, F rhs) { return F(bool(lhs) and bool(rhs)); }

template <typename F> always_inline F and_bitwise(F lhs, F rhs) { return F(long(lhs) & long(rhs)); }

template <typename F> always_inline F or_bool(F lhs, F rhs) { return F(bool(lhs) or bool(rhs)); }

template <typename F> always_inline F or_bitwise(F lhs, F rhs) { return F(long(lhs) | long(rhs)); }

template <typename F> always_inline F shift_left(F lhs, F rhs) { return F(long(lhs) << long(rhs)); }

template <typename F> always_inline F shift_right(F lhs, F rhs) { return F(long(lhs) >> long(rhs)); }

template <typename F> always_inline F inv_pow(F lhs, F rhs) { return std::pow(rhs, lhs); }

template <typename F> always_inline F square(F arg) { return arg * arg; }

template <typename T> always_inline T pow2(T const& arg) { return square(arg); }

template <typename T> always_inline T cube(T arg) { return arg * arg * arg; }

template <typename T> always_inline T pow3(T const& arg) { return cube(arg); }

using std::sqrt;

template <typename F> always_inline F rsqrt(F arg) { return F(1) / std::sqrt(arg); }

template <typename F> always_inline F rcp(F arg) { return F(1) / arg; }

using std::abs;


template <typename F> always_inline F isnormal(F arg) {
#ifdef __INTEL_COMPILER /* intel compiler workaround */
    return boost::math::isnormal(arg);
#else
    return std::isnormal(arg);
#endif
}


template <typename F1, typename F2, typename F3> always_inline F1 clip(F1 t, F2 low, F3 hi) {
    if (t < low)
        return low;
    else if (t > hi)
        return hi;
    else
        return t;
}

template <typename F> always_inline F modulo(F lhs, F rhs) {
    F ret = fmod(lhs, rhs);
    if (ret < 0)
        ret += rhs;
    return ret;
}

template <typename I> always_inline I wrap_optimistic(I val, const I mod) {
    if (unlikely(val >= mod))
        do
            val -= mod;
        while (unlikely(val >= mod));
    else
        while (unlikely(val < I(0)))
            val += mod;

    return val;
}

template <typename I>
always_inline I scale_linear(I in, I const& low_in, I const& high_in, I const& low_out, I const& high_out) {
    return (in - low_in) / (high_in - low_in) * (high_out - low_out) + low_out;
}

template <typename I>
always_inline I scale_linear_clipped(I in, I const& low_in, I const& high_in, I const& low_out, I const& high_out) {
    return (in - low_in) / (high_in - low_in) * (high_out - low_out) + low_out;
}


} /* namespace nova */
