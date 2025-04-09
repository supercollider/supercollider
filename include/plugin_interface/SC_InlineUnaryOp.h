/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once

#include "SC_Types.h"
#include "SC_Constants.h"

#include <cmath>
#include <limits>

#ifdef __SSE__
#    include <xmmintrin.h>
#endif

#ifdef __SSE4_1__
#    include <smmintrin.h>
#endif


///////////////////////////////////////////////////////////////////////////////////////

/// Checks whether x is NaN. This is a legacy function, use std::isnan instead.
inline bool sc_isnan(float x) { return std::isnan(x); }

/// Checks whether x is finite. This is a legacy function, use std::isfinite instead.
inline bool sc_isnan(double x) { return std::isnan(x); }

inline bool sc_isfinite(float x) { return std::isfinite(x); }

inline bool sc_isfinite(double x) { return std::isfinite(x); }

///////////////////////////////////////////////////////////////////////////////////////

// versions provided for float32 and float64
// did not supply template because do not want to instantiate for integers.

///////////////////////////////////////////////////////////////////////////////////////


/*
 * Zap dangerous values (subnormals, infinities, nans) in feedback loops to zero.
 * Prevents pathological math operations in ugens and can be used at the end of a
 * block to fix any recirculating filter values.
 */
inline float32 zapgremlins(float32 x) {
    float32 absx = std::abs(x);
    // very small numbers fail the first test, eliminating denormalized numbers
    //    (zero also fails the first test, but that is OK since it returns zero.)
    // very large numbers fail the second test, eliminating infinities
    // Not-a-Numbers fail both tests and are eliminated.
    return (absx > 1e-15f && absx < 1e15f) ? x : 0.f;
}

inline float32 sc_log2(float32 x) { return std::log2(x); }

inline float32 sc_log10(float32 x) { return std::log10(std::abs(x)); }

/// Convert MIDI note to cycles per second
inline float32 sc_midicps(float32 note) { return 440.f * std::pow(2.f, (note - 69.f) / 12.f); }

/// Convert cycles per second to MIDI note.
inline float32 sc_cpsmidi(float32 freq) { return sc_log2(freq / 440.f) * 12.f + 69.f; }

/// Convert an interval in MIDI notes into a frequency ratio.
inline float32 sc_midiratio(float32 midi) { return std::pow(2.f, midi / 12.f); }

/// Convert a frequency ratio to an interval in MIDI notes.
inline float32 sc_ratiomidi(float32 ratio) { return 12.f * sc_log2(ratio); }

/// Convert decimal octaves to cycles per second.
inline float32 sc_octcps(float32 note) { return 440.f * std::pow(2.f, note - 4.75f); }

/// Convert cycles per second to decimal octaves.
inline float32 sc_cpsoct(float32 freq) { return sc_log2(freq / 440.f) + 4.75f; }

/// Convert linear amplitude to decibels.
inline float32 sc_ampdb(float32 amp) { return std::log10(amp) * 20.f; }

/// Convert decibels to linear amplitude.
inline float32 sc_dbamp(float32 db) { return std::pow(10.f, db * .05f); }

/// Squared value
inline float32 sc_squared(float32 x) { return x * x; }

/// Cubed value
inline float32 sc_cubed(float32 x) { return x * x * x; }

/*
 * Square root
 * The definition of square root is extended for signals so that sqrt(a) when a<0 returns -sqrt(-a).
 */
inline float32 sc_sqrt(float32 x) { return x < 0.f ? -sqrt(-x) : sqrt(x); }


/// A value for a hanning window function between 0 and 1.
inline float32 sc_hanwindow(float32 x) {
    if (x < 0.f || x > 1.f)
        return 0.f;
    return 0.5f - 0.5f * static_cast<float32>(cos(x * (float32)twopi));
}

/// A value for a welsh window function between 0 and 1.
inline float32 sc_welwindow(float32 x) {
    if (x < 0.f || x > 1.f)
        return 0.f;
    return static_cast<float32>(sin(x * pi));
}

/// a value for a triangle window function between 0 and 1.
inline float32 sc_triwindow(float32 x) {
    if (x < 0.f || x > 1.f)
        return 0.f;
    if (x < 0.5f)
        return 2.f * x;
    else
        return -2.f * x + 2.f;
}

/// a bilateral value for a triangle window function
inline float32 sc_bitriwindow(float32 x) {
    float32 ax = 1.f - std::abs(x);
    if (ax <= 0.f)
        return 0.f;
    return ax;
}

/// a value for a rectangular window function between 0 and 1.
inline float32 sc_rectwindow(float32 x) {
    if (x < 0.f || x > 1.f)
        return 0.f;
    return 1.f;
}

/// Map x onto an S-curve.
inline float32 sc_scurve(float32 x) {
    if (x <= 0.f)
        return 0.f;
    if (x >= 1.f)
        return 1.f;
    return x * x * (3.f - 2.f * x);
}

/*
 * Map x onto an S-curve.
 * Assumes that x is in range
 */
inline float32 sc_scurve0(float32 x) { return x * x * ((float32)3. - (float32)2. * x); }

/// Map x onto a ramp starting at 0.
inline float32 sc_ramp(float32 x) {
    if (x <= 0.f)
        return 0.f;
    if (x >= 1.f)
        return 1.f;
    return x;
}

/// Answer -1 if negative, +1 if positive or 0 if zero
inline float32 sc_sign(float32 x) { return x < 0.f ? -1.f : (x > 0.f ? 1.f : 0.f); }

/// A nonlinear distortion function.
inline float32 sc_distort(float32 x) { return x / ((float32)1. + std::abs(x)); }

inline float32 sc_distortneg(float32 x) {
    if (x < 0.f)
        return x / (1.f - x);
    else
        return x;
}

/// Distortion with a perfectly linear region from -0.5 to +0.5
inline float32 sc_softclip(float32 x) {
    float32 absx = std::abs(x);
    if (absx <= 0.5f)
        return x;
    else
        return (absx - 0.25f) / x;
}

/// Taylor expansion out to x**9/9! factored  into multiply-adds from Phil Burk.
inline float32 taylorsin(float32 x) {
    // valid range from -pi/2 to +3pi/2
    x = static_cast<float32>((float32)pi2 - std::abs(pi2 - x));
    float32 x2 = x * x;
    return static_cast<float32>(
        x * (x2 * (x2 * (x2 * (x2 * (1.0 / 362880.0) - (1.0 / 5040.0)) + (1.0 / 120.0)) - (1.0 / 6.0)) + 1.0));
}

/// Truncate to multiple of x (e.g. it rounds numbers down to a multiple of x).
inline float32 sc_trunc(float32 x) { return std::trunc(x); }

inline float32 sc_ceil(float32 x) {
#ifdef __SSE4_1__
    __m128 a = _mm_set_ss(x);
    __m128 b = _mm_round_ss(a, a, _MM_FROUND_TO_POS_INF);
    return _mm_cvtss_f32(b);
#else
    return std::ceil(x);
#endif
}

inline float32 sc_floor(float32 x) {
#ifdef __SSE4_1__
    __m128 a = _mm_set_ss(x);
    __m128 b = _mm_round_ss(a, a, _MM_FROUND_TO_NEG_INF);
    return _mm_cvtss_f32(b);
#else
    return std::floor(x);
#endif
}

/// 1 divided by x
inline float32 sc_reciprocal(float32 x) {
#ifdef __SSE__
    // adapted from AP-803 Newton-Raphson Method with Streaming SIMD Extensions
    // 23 bit accuracy (out of 24bit)
    const __m128 arg = _mm_set_ss(x);
    const __m128 approx = _mm_rcp_ss(arg);
    const __m128 muls = _mm_mul_ss(_mm_mul_ss(arg, approx), approx);
    const __m128 doubleApprox = _mm_add_ss(approx, approx);
    const __m128 result = _mm_sub_ss(doubleApprox, muls);
    return _mm_cvtss_f32(result);
#else
    return 1.f / x;
#endif
}

/// Return fractional part
inline float32 sc_frac(float32 x) { return x - sc_floor(x); }

////////////////////////////////

/// Returns ones complement
inline float32 sc_bitNot(float32 x) { return (float32) ~(int)x; }

/// Cubic lagrange interpolator
inline float32 sc_lg3interp(float32 x1, float32 a, float32 b, float32 c, float32 d) {
    float32 x0 = x1 + 1.f;
    float32 x2 = x1 - 1.f;
    float32 x3 = x1 - 2.f;

    float32 x03 = x0 * x3 * 0.5f;
    float32 x12 = x1 * x2 / 6.f;

    return x12 * (d * x0 - a * x3) + x03 * (b * x2 - c * x1);
}

/// Determines the feedback coefficient for a feedback comb filter with the given delay and decay times.
inline float32 sc_CalcFeedback(float32 delaytime, float32 decaytime) {
    if (delaytime == 0.f || decaytime == 0.f)
        return 0.f;

    float32 absret = static_cast<float32>(std::exp(log001 * delaytime / std::abs(decaytime)));
    float32 ret = std::copysign(absret, decaytime);
    return ret;
}

/// Wrap x around ±1, wrapping only once.
inline float32 sc_wrap1(float32 x) {
    if (x >= 1.f)
        return x + -2.f;
    if (x < -1.f)
        return x + 2.f;
    return x;
}

/// Fold x around ±1, folding only once.
inline float32 sc_fold1(float32 x) {
    if (x >= 1.f)
        return 2.f - x;
    if (x < -1.f)
        return -2.f - x;
    return x;
}

///////////////////////////////////////////////////////////////////////////////////////

/*
 * Zap dangerous values (subnormals, infinities, nans) in feedback loops to zero.
 * Prevents pathological math operations in ugens and can be used at the end of a
 * block to fix any recirculating filter values.
 */
inline float64 zapgremlins(float64 x) {
    float64 absx = std::abs(x);
    // very small numbers fail the first test, eliminating denormalized numbers
    //    (zero also fails the first test, but that is OK since it returns zero.)
    // very large numbers fail the second test, eliminating infinities
    // Not-a-Numbers fail both tests and are eliminated.
    return (absx > 1e-15 && absx < 1e15) ? x : 0.;
}

inline float64 sc_log2(float64 x) { return std::log2(std::abs(x)); }

inline float64 sc_log10(float64 x) { return std::log10(std::abs(x)); }

/// Convert MIDI note to cycles per second
inline float64 sc_midicps(float64 note) { return 440. * std::pow(2., (note - 69.) / 12.); }

/// Convert cycles per second to MIDI note.
inline float64 sc_cpsmidi(float64 freq) { return sc_log2(freq / 440.) * 12. + 69.; }

/// Convert an interval in MIDI notes into a frequency ratio.
inline float64 sc_midiratio(float64 midi) { return std::pow(2., midi / 12.); }

/// Convert a frequency ratio to an interval in MIDI notes.
inline float64 sc_ratiomidi(float64 ratio) { return 12. * sc_log2(ratio); }

/// Convert decimal octaves to cycles per second.
inline float64 sc_octcps(float64 note) { return 440. * std::pow(2., note - 4.75); }

/// Convert cycles per second to decimal octaves.
inline float64 sc_cpsoct(float64 freq) { return sc_log2(freq / 440.) + 4.75; }

/// Convert linear amplitude to decibels.
inline float64 sc_ampdb(float64 amp) { return std::log10(amp) * 20.; }

/// Convert decibels to linear amplitude.
inline float64 sc_dbamp(float64 db) { return std::pow(10., db * .05); }

/// Squared value
inline float64 sc_squared(float64 x) { return x * x; }

/// Cubed value
inline float64 sc_cubed(float64 x) { return x * x * x; }

/*
 * Square root
 * The definition of square root is extended for signals so that sqrt(a) when a<0 returns -sqrt(-a).
 */
inline float64 sc_sqrt(float64 x) { return x < 0. ? -sqrt(-x) : sqrt(x); }

/// A value for a hanning window function between 0 and 1.
inline float64 sc_hanwindow(float64 x) {
    if (x < 0. || x > 1.)
        return 0.;
    return 0.5 - 0.5 * cos(x * twopi);
}

/// A value for a welsh window function between 0 and 1.
inline float64 sc_welwindow(float64 x) {
    if (x < 0. || x > 1.)
        return 0.;
    return sin(x * pi);
}

/// a value for a triangle window function between 0 and 1.
inline float64 sc_triwindow(float64 x) {
    if (x < 0. || x > 1.)
        return 0.;
    if (x < 0.5)
        return 2. * x;
    else
        return -2. * x + 2.;
}

/// a bilateral value for a triangle window function
inline float64 sc_bitriwindow(float64 x) {
    float64 ax = std::abs(x);
    if (ax > 1.)
        return 0.;
    return 1. - ax;
}

/// a value for a rectangular window function between 0 and 1.
inline float64 sc_rectwindow(float64 x) {
    if (x < 0. || x > 1.)
        return 0.;
    return 1.;
}

/// Map x onto an S-curve.
inline float64 sc_scurve(float64 x) {
    if (x <= 0.)
        return 0.;
    if (x >= 1.)
        return 1.;
    return x * x * (3. - 2. * x);
}

/*
 * Map x onto an S-curve.
 * Assumes that x is in range
 */
inline float64 sc_scurve0(float64 x) {
    // assumes that x is in range
    return x * x * (3. - 2. * x);
}

/// Map x onto a ramp starting at 0.
inline float64 sc_ramp(float64 x) {
    if (x <= 0.)
        return 0.;
    if (x >= 1.)
        return 1.;
    return x;
}

/// Answer -1 if negative, +1 if positive or 0 if zero
inline float64 sc_sign(float64 x) { return x < 0. ? -1. : (x > 0. ? 1. : 0.); }

/// A nonlinear distortion function.
inline float64 sc_distort(float64 x) { return x / (1. + std::abs(x)); }

inline float64 sc_distortneg(float64 x) {
    if (x < 0.)
        return x / (1. - x);
    else
        return x;
}

/// Distortion with a perfectly linear region from -0.5 to +0.5
inline float64 sc_softclip(float64 x) {
    float64 absx = std::abs(x);
    if (absx <= 0.5)
        return x;
    else
        return (absx - 0.25) / x;
}

/// Taylor expansion out to x**9/9! factored into multiply-adds from Phil Burk.
inline float64 taylorsin(float64 x) {
    x = pi2 - std::abs(pi2 - x);
    float64 x2 = x * x;
    return x * (x2 * (x2 * (x2 * (x2 * (1.0 / 362880.0) - (1.0 / 5040.0)) + (1.0 / 120.0)) - (1.0 / 6.0)) + 1.0);
}

/// Truncate to multiple of x (e.g. it rounds numbers down to a multiple of x).
inline float64 sc_trunc(float64 x) { return std::trunc(x); }

inline float64 sc_ceil(float64 x) {
#ifdef __SSE4_1__
    __m128d a = _mm_set_sd(x);
    const int cntrl = _MM_FROUND_TO_POS_INF;
    __m128d b = _mm_round_sd(a, a, cntrl);
    return _mm_cvtsd_f64(b);
#else
    return std::ceil(x);
#endif
}

inline float64 sc_floor(float64 x) {
#ifdef __SSE4_1__
    __m128d a = _mm_set_sd(x);
    const int cntrl = _MM_FROUND_TO_NEG_INF;
    __m128d b = _mm_round_sd(a, a, cntrl);
    return _mm_cvtsd_f64(b);
#else
    return std::floor(x);
#endif
}

/// 1 divided by x
inline float64 sc_reciprocal(float64 x) { return 1. / x; }

/// Return fractional part
inline float64 sc_frac(float64 x) { return x - sc_floor(x); }

/// Wrap x around ±1, wrapping only once.
inline float64 sc_wrap1(float64 x) {
    if (x >= 1.)
        return x + -2.;
    if (x < -1.)
        return x + 2.;
    return x;
}

/// Fold x around ±1, folding only once.
inline float64 sc_fold1(float64 x) {
    if (x >= 1.)
        return 2. - x;
    if (x < -1.)
        return -2. - x;
    return x;
}

/// Convert binary to Gray code.
inline int32 sc_grayCode(int32 x) { return x ^ (x >> 1); }
