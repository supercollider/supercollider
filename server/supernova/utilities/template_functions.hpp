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

#include "function_attributes.h"

namespace nova {
template <typename T> FORCE_INLINE T identity(T arg) { return arg; }

template <typename T> FORCE_INLINE T plus(T lhs, T rhs) { return lhs + rhs; }

template <typename T> FORCE_INLINE T minus(T lhs, T rhs) { return lhs - rhs; }

template <typename F> FORCE_INLINE F inv_minus(F lhs, F rhs) { return rhs - lhs; }

template <typename T> FORCE_INLINE T times(T lhs, T rhs) { return lhs * rhs; }

template <typename T> FORCE_INLINE T over(T lhs, T rhs) { return lhs / rhs; }

template <typename F> FORCE_INLINE F inv_over(F lhs, F rhs) { return rhs / lhs; }

template <typename F> FORCE_INLINE F fmod(F lhs, F rhs) { return std::fmod(lhs, rhs); }


template <typename F> FORCE_INLINE F less(F lhs, F rhs) { return F(lhs < rhs); }

template <typename F> FORCE_INLINE F less_equal(F lhs, F rhs) { return F(lhs <= rhs); }

template <typename F> FORCE_INLINE F equal(F lhs, F rhs) { return F(lhs == rhs); }

template <typename F> FORCE_INLINE F notequal(F lhs, F rhs) { return F(lhs != rhs); }

template <typename F> FORCE_INLINE F greater(F lhs, F rhs) { return F(lhs > rhs); }

template <typename F> FORCE_INLINE F greater_equal(F lhs, F rhs) { return F(lhs >= rhs); }

using std::max;
using std::min;


template <typename F> FORCE_INLINE F nova_max(F lhs, F rhs) { return max(lhs, rhs); }

template <typename F> FORCE_INLINE F nova_min(F lhs, F rhs) { return min(lhs, rhs); }

template <typename F> FORCE_INLINE F sign(F arg) {
    if (arg > 0)
        return F(1);
    if (arg < 0)
        return F(-1);
    return F(0);
}

template <typename F> FORCE_INLINE F and_bool(F lhs, F rhs) { return F(bool(lhs) and bool(rhs)); }

template <typename F> FORCE_INLINE F and_bitwise(F lhs, F rhs) { return F(long(lhs) & long(rhs)); }

template <typename F> FORCE_INLINE F or_bool(F lhs, F rhs) { return F(bool(lhs) or bool(rhs)); }

template <typename F> FORCE_INLINE F or_bitwise(F lhs, F rhs) { return F(long(lhs) | long(rhs)); }

template <typename F> FORCE_INLINE F shift_left(F lhs, F rhs) { return F(long(lhs) << long(rhs)); }

template <typename F> FORCE_INLINE F shift_right(F lhs, F rhs) { return F(long(lhs) >> long(rhs)); }

template <typename F> FORCE_INLINE F inv_pow(F lhs, F rhs) { return std::pow(rhs, lhs); }

template <typename F> FORCE_INLINE F square(F arg) { return arg * arg; }

template <typename T> FORCE_INLINE T pow2(T const& arg) { return square(arg); }

template <typename T> FORCE_INLINE T cube(T arg) { return arg * arg * arg; }

template <typename T> FORCE_INLINE T pow3(T const& arg) { return cube(arg); }

using std::sqrt;

template <typename F> FORCE_INLINE F rsqrt(F arg) { return F(1) / std::sqrt(arg); }

template <typename F> FORCE_INLINE F rcp(F arg) { return F(1) / arg; }

using std::abs;


template <typename F> FORCE_INLINE F isnormal(F arg) { return std::isnormal(arg); }


template <typename F1, typename F2, typename F3> FORCE_INLINE F1 clip(F1 t, F2 low, F3 hi) {
    if (t < low)
        return low;
    else if (t > hi)
        return hi;
    else
        return t;
}

template <typename F> FORCE_INLINE F modulo(F lhs, F rhs) {
    F ret = fmod(lhs, rhs);
    if (ret < 0)
        ret += rhs;
    return ret;
}

template <typename I> FORCE_INLINE I wrap_optimistic(I val, const I mod) {
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
FORCE_INLINE I scale_linear(I in, I const& low_in, I const& high_in, I const& low_out, I const& high_out) {
    return (in - low_in) / (high_in - low_in) * (high_out - low_out) + low_out;
}

template <typename I>
FORCE_INLINE I scale_linear_clipped(I in, I const& low_in, I const& high_in, I const& low_out, I const& high_out) {
    return (in - low_in) / (high_in - low_in) * (high_out - low_out) + low_out;
}


} /* namespace nova */
