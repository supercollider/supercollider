/*
 *    SuperCollider real time audio synthesis system
 *    Copyright (c) 2002 James McCartney. All rights reserved.
 *    Copyright (c) 2011 Tim Blechmann.
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */

#pragma once

#include "SC_PlugIn.hpp"

#include "simd_memory.hpp"
#include "simd_binary_arithmetic.hpp"
#include "simd_ternary_arithmetic.hpp"
#include "simd_mix.hpp"

using nova::slope_argument;

#if defined(__GNUC__) && !defined(__clang__)
#    define inline_functions __attribute__((flatten))
#else
#    define inline_functions
#endif

struct SIMD_Unit : SCUnit {
    enum { scalar, unrolled, unrolled_64 };

    bool canUseSIMD(void) const { return (mBufLength & (nova::vec<float>::objects_per_cacheline - 1)) == 0; }

    template <int index> struct ControlRateInput {
        float value;

        void init(const SIMD_Unit* parent) { value = parent->in0(index); }

        bool changed(const SIMD_Unit* parent) const { return value != parent->in0(index); }

#if __cplusplus <= 199711L
        nova::detail::scalar_ramp_argument<float> slope(const SIMD_Unit* parent)
#else
        decltype(nova::slope_argument(0.f, 0.f)) slope(const SIMD_Unit* parent)
#endif
        {
            float next = parent->in0(index);
            float current = value;
            float slope = parent->calcSlope(next, current);
            value = next;
            return slope_argument(current, slope);
        }

        operator float(void) { return value; }
    };

    template <typename UnitType, void (UnitType::*UnrolledCalcFunc)(int), void (UnitType::*VectorCalcFunc)(int),
              void (UnitType::*ScalarCalcFunc)(int)>
    void set_unrolled_calc_function(void) {
        if (bufferSize() == 64)
            SCUnit::set_vector_calc_function<UnitType, UnrolledCalcFunc, ScalarCalcFunc>();
        else
            set_vector_calc_function<UnitType, VectorCalcFunc, ScalarCalcFunc>();
    }

    template <typename UnitType, void (UnitType::*VectorCalcFunc)(int), void (UnitType::*ScalarCalcFunc)(int)>
    void set_vector_calc_function(void) {
        if (canUseSIMD())
            SCUnit::set_vector_calc_function<UnitType, VectorCalcFunc, ScalarCalcFunc>();
        else
            SCUnit::set_calc_function<UnitType, ScalarCalcFunc>();
    }

    template <int type, typename Arg1, typename Arg2, typename Arg3>
    static void muladd(float* out, Arg1 const& arg1, Arg2 const& arg2, Arg3 const& arg3, int inNumSamples) {
        if (type == scalar)
            nova::muladd_vec(out, arg1, arg2, arg3, inNumSamples);
        if (type == unrolled)
            nova::muladd_vec_simd(out, arg1, arg2, arg3, inNumSamples);
        if (type == unrolled_64)
            nova::muladd_vec_simd<64>(out, arg1, arg2, arg3);
    }

    template <int type, typename Arg1, typename Arg2>
    static void plus_vec(float* out, Arg1 const& arg1, Arg2 const& arg2, int inNumSamples) {
        if (type == scalar)
            nova::plus_vec(out, arg1, arg2, inNumSamples);
        if (type == unrolled)
            nova::plus_vec_simd(out, arg1, arg2, inNumSamples);
        if (type == unrolled_64)
            nova::plus_vec_simd<64>(out, arg1, arg2);
    }

    template <int type, typename Arg1, typename Arg2>
    static void times_vec(float* out, Arg1 const& arg1, Arg2 const& arg2, int inNumSamples) {
        if (type == scalar)
            nova::times_vec(out, arg1, arg2, inNumSamples);
        if (type == unrolled)
            nova::times_vec_simd(out, arg1, arg2, inNumSamples);
        if (type == unrolled_64)
            nova::times_vec_simd<64>(out, arg1, arg2);
    }

    template <int type, typename Arg1, typename Arg2>
    static void slope_vec(float* out, Arg1 const& base, Arg2 const& slope, int inNumSamples) {
        if (type == scalar)
            nova::set_slope_vec(out, base, slope, inNumSamples);
        else
            nova::set_slope_vec_simd(out, base, slope, inNumSamples);
    }

    template <int type, typename Arg1> static void slope_vec(float* out, Arg1 const& slope, int inNumSamples) {
        if (type == scalar)
            nova::set_slope_vec(out, slope.data, slope.slope_, inNumSamples);
        else
            nova::set_slope_vec_simd(out, slope.data, slope.slope_, inNumSamples);
    }

    template <int type> static void copy_vec(float* out, const float* in, int inNumSamples) {
        if (in == out)
            return;

        if (type == scalar)
            nova::copyvec(out, in, inNumSamples);
        if (type == unrolled)
            nova::copyvec_simd(out, in, inNumSamples);
        if (type == unrolled_64)
            nova::copyvec_simd<64>(out, in);
    }

    template <int type> static void set_vec(float* out, float value, int inNumSamples) {
        if (type == scalar)
            nova::setvec(out, value, inNumSamples);
        if (type == unrolled)
            nova::setvec_simd(out, value, inNumSamples);
        if (type == unrolled_64)
            nova::setvec_simd<64>(out, value);
    }

    template <int type> static void zero_vec(float* out, int inNumSamples) {
        if (type == scalar)
            nova::zerovec(out, inNumSamples);
        if (type == unrolled)
            nova::zerovec_simd(out, inNumSamples);
        if (type == unrolled_64)
            nova::zerovec_simd<64>(out);
    }
};
