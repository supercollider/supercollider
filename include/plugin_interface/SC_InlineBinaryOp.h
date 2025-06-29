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

#include "SC_BoundsMacros.h"
#include "SC_InlineUnaryOp.h"
#include <cmath>
#include <cstdint>

/// Modulo
inline float sc_mod(float in, float hi) {
    // avoid the divide if possible
    const float lo = (float)0.;
    if (in >= hi) {
        in -= hi;
        if (in < hi)
            return in;
    } else if (in < lo) {
        in += hi;
        if (in >= lo)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    return in - hi * sc_floor(in / hi);
}

/// Modulo
inline double sc_mod(double in, double hi) {
    // avoid the divide if possible
    const double lo = (double)0.;
    if (in >= hi) {
        in -= hi;
        if (in < hi)
            return in;
    } else if (in < lo) {
        in += hi;
        if (in >= lo)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    return in - hi * sc_floor(in / hi);
}


/// Wrap in between lo and hi
inline float sc_wrap(float in, float lo, float hi) {
    float range;
    // avoid the divide if possible
    if (in >= hi) {
        range = hi - lo;
        in -= range;
        if (in < hi)
            return in;
    } else if (in < lo) {
        range = hi - lo;
        in += range;
        if (in >= lo)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    return in - range * sc_floor((in - lo) / range);
}

/// Wrap in between lo and hi
inline double sc_wrap(double in, double lo, double hi) {
    double range;
    // avoid the divide if possible
    if (in >= hi) {
        range = hi - lo;
        in -= range;
        if (in < hi)
            return in;
    } else if (in < lo) {
        range = hi - lo;
        in += range;
        if (in >= lo)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    return in - range * sc_floor((in - lo) / range);
}

/// Wrap in between lo and hi to range
inline double sc_wrap(double in, double lo, double hi, double range) {
    // avoid the divide if possible
    if (in >= hi) {
        in -= range;
        if (in < hi)
            return in;
    } else if (in < lo) {
        in += range;
        if (in >= lo)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    return in - range * sc_floor((in - lo) / range);
}

/// Wrap in between lo and hi to range
inline double sc_wrap(float in, float lo, float hi, float range) {
    // avoid the divide if possible
    if (in >= hi) {
        in -= range;
        if (in < hi)
            return in;
    } else if (in < lo) {
        in += range;
        if (in >= lo)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    return in - range * sc_floor((in - lo) / range);
}

/// Folds in to value between lo and hi
inline float sc_fold(float in, float lo, float hi) {
    float x, c, range, range2;
    x = in - lo;

    // avoid the divide if possible
    if (in >= hi) {
        in = hi + hi - in;
        if (in >= lo)
            return in;
    } else if (in < lo) {
        in = lo + lo - in;
        if (in < hi)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    // ok do the divide
    range = hi - lo;
    range2 = range + range;
    c = x - range2 * sc_floor(x / range2);
    if (c >= range)
        c = range2 - c;
    return c + lo;
}

/// Folds in to value between lo and hi
inline double sc_fold(double in, double lo, double hi) {
    double x, c, range, range2;
    x = in - lo;

    // avoid the divide if possible
    if (in >= hi) {
        in = hi + hi - in;
        if (in >= lo)
            return in;
    } else if (in < lo) {
        in = lo + lo - in;
        if (in < hi)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    // ok do the divide
    range = hi - lo;
    range2 = range + range;
    c = x - range2 * sc_floor(x / range2);
    if (c >= range)
        c = range2 - c;
    return c + lo;
}

/// Folds in to value between lo and hi
inline double sc_fold(float in, float lo, float hi, float range, float range2) {
    float x, c;
    x = in - lo;

    // avoid the divide if possible
    if (in >= hi) {
        in = hi + hi - in;
        if (in >= lo)
            return in;
    } else if (in < lo) {
        in = lo + lo - in;
        if (in < hi)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    // ok do the divide
    c = x - range2 * sc_floor(x / range2);
    if (c >= range)
        c = range2 - c;
    return c + lo;
}

inline double sc_fold(double in, double lo, double hi, double range, double range2) {
    double x, c;
    x = in - lo;

    // avoid the divide if possible
    if (in >= hi) {
        in = hi + hi - in;
        if (in >= lo)
            return in;
    } else if (in < lo) {
        in = lo + lo - in;
        if (in < hi)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    // ok do the divide
    c = x - range2 * sc_floor(x / range2);
    if (c >= range)
        c = range2 - c;
    return c + lo;
}

/// Exponentiation
inline float sc_pow(float a, float b) { return a >= 0.f ? std::pow(a, b) : -std::pow(-a, b); }

/// Exponentiation
inline double sc_pow(double a, double b) { return a >= 0.f ? std::pow(a, b) : -std::pow(-a, b); }

/// Quantization by rounding. Rounds x to the nearest multiple of quant
inline float sc_round(float x, float quant) { return quant == 0. ? x : sc_floor(x / quant + .5f) * quant; }

/// Quantization by rounding. Rounds x to the nearest multiple of quant
inline double sc_round(double x, double quant) { return quant == 0. ? x : sc_floor(x / quant + .5) * quant; }

/// Round x up to multiple of quant
inline float sc_roundUp(float x, float quant) { return quant == 0. ? x : sc_ceil(x / quant) * quant; }

/// Round x up to multiple of quant
inline double sc_roundUp(double x, double quant) { return quant == 0. ? x : sc_ceil(x / quant) * quant; }

/// Truncate to multiple of quant (e.g. it rounds numbers down to a multiple of quant)
inline float sc_trunc(float x, float quant) { return quant == 0. ? x : sc_floor(x / quant) * quant; }

/// Truncate to multiple of quant (e.g. it rounds numbers down to a multiple of quant)
inline double sc_trunc(double x, double quant) { return quant == 0. ? x : sc_floor(x / quant) * quant; }

/// Computes the arc tangent of a/b using the signs of arguments to determine the correct quadrant.
inline float sc_atan2(float a, float b) { return std::atan2(a, b); }

const float kFSQRT2M1 = static_cast<float32>(sqrt(2.) - 1.);
const double kDSQRT2M1 = sqrt(2.) - 1.;

///  Returns the hypotenuse of a right-angled triangle whose legs are x and y.
inline float sc_hypotx(float x, float y) {
    float minxy;

    x = std::abs(x);
    y = std::abs(y);

    minxy = sc_min(x, y);

    return x + y - kFSQRT2M1 * minxy;
}

/**
 * Compute abs(x) + abs(y) - (min(abs(x), abs(y)) * (sqrt(2) - 1)),
 * the minimum distance one will have to travel from the origin to (x,y)
 * using only orthogonal and diagonal movements.
 */
inline double sc_hypotx(double x, double y) {
    double minxy;

    x = std::abs(x);
    y = std::abs(y);

    minxy = sc_min(x, y);

    return x + y - kDSQRT2M1 * minxy;
}

inline int sc_div(int a, int b) {
    int c;
    if (b) {
        if (a < 0)
            c = (a + 1) / b - 1;
        else
            c = a / b;
    } else
        c = a;
    return c;
}

/// Modulo
inline int sc_mod(int in, int hi) {
    // avoid the divide if possible
    const int lo = 0;
    if (in >= hi) {
        in -= hi;
        if (in < hi)
            return in;
    } else if (in < lo) {
        in += hi;
        if (in >= lo)
            return in;
    } else
        return in;

    if (hi == lo)
        return lo;
    return in - hi * sc_floor((float64)in / hi);
}

/**
 * Modulo with old behavior;
 * gives unexpected results for all values with negative integer modulus,
 *  i.e., affected cases: for a, b both integers: a mod -b.
 */
inline int sc_mod_seaside(int in, int hi) {
    // avoid the divide if possible
    const int lo = 0;
    if (in >= hi) {
        in -= hi;
        if (in < hi)
            return in;
    } else if (in < lo) {
        in += hi;
        if (in >= lo)
            return in;
    } else
        return in;


    int c;
    c = in % hi;
    if (c < 0)
        c += hi;
    return c;
}

/// Wrap in between lo and hi
inline int sc_wrap(int in, int lo, int hi) { return sc_mod(in - lo, hi - lo + 1) + lo; }

/// Folds in to value between lo and hi
inline int sc_fold(int in, int lo, int hi) {
    int b = hi - lo;
    int b2 = b + b;
    int c = sc_mod(in - lo, b2);
    if (c > b)
        c = b2 - c;
    return c + lo;
}

/// Greatest common divisor
inline int sc_gcd(int a, int b) {
    if (a == 0)
        return b;

    if (b == 0)
        return a;

    const bool negative = (a <= 0 && b <= 0);

    a = sc_abs(a);
    b = sc_abs(b);

    if (a == 1 || b == 1) {
        if (negative) {
            return -1;
        } else {
            return 1;
        }
    }

    if (a < b) {
        int t = a;
        a = b;
        b = t;
    }

    while (b > 0) {
        int t = a % b;
        a = b;
        b = t;
    }

    if (negative) {
        a = 0 - a;
    }

    return a;
}

/// Least common multiple
inline int sc_lcm(int a, int b) {
    if (a == 0 || b == 0)
        return 0;
    else
        return (a * b) / sc_gcd(a, b);
}


/// Greatest common divisor
inline std::int64_t sc_gcd(std::int64_t a, std::int64_t b) {
    if (a == 0)
        return b;

    if (b == 0)
        return a;

    const bool negative = (a <= 0 && b <= 0);

    a = sc_abs(a);
    b = sc_abs(b);

    if (a == 1 || b == 1) {
        if (negative) {
            return -1LL;
        } else {
            return 1LL;
        }
    }

    if (a < b) {
        std::int64_t t = a;
        a = b;
        b = t;
    }

    while (b > 0) {
        std::int64_t t = a % b;
        a = b;
        b = t;
    }

    if (negative) {
        a = 0 - a;
    }

    return a;
}

/// Least common multiple
inline std::int64_t sc_lcm(std::int64_t a, std::int64_t b) {
    if (a == 0 || b == 0)
        return 0LL;
    else
        return (a * b) / sc_gcd(a, b);
}

/// Greatest common divisor
inline float sc_gcd(float u, float v) {
    return (float)sc_gcd((std::int64_t)std::trunc(u), (std::int64_t)std::trunc(v));
}

/// Least common multiple
inline float sc_lcm(float u, float v) {
    return (float)sc_lcm((std::int64_t)std::trunc(u), (std::int64_t)std::trunc(v));
}

/// Performs a bitwise and with the number b
inline int sc_bitAnd(int a, int b) { return a & b; }

/// Performs a bitwise or with the number b
inline int sc_bitOr(int a, int b) { return a | b; }

/// Performs a binary leftshift with the number b
inline int sc_leftShift(int a, int b) { return a << b; }

/// Performs a binary rightshift with the number b
inline int sc_rightShift(int a, int b) { return a >> b; }

/// Recast a as an unsigned integer and then perform a binary rightshift with the number b
inline int sc_unsignedRightShift(int a, int b) { return (int)((uint32)a >> b); }

/// Quantization by rounding. Rounds x to the nearest multiple of quant
inline int sc_round(int x, int quant) { return quant == 0 ? x : sc_div(x + quant / 2, quant) * quant; }

/// Round x up to multiple of quant
inline int sc_roundUp(int x, int quant) { return quant == 0 ? x : sc_div(x + quant - 1, quant) * quant; }

/// Truncate to multiple of quant (e.g. it rounds numbers down to a multiple of quant)
inline int sc_trunc(int x, int quant) { return quant == 0 ? x : sc_div(x, quant) * quant; }

/// Exponentiation: x to the power of an integer
template <typename F> inline F sc_powi(F x, unsigned int n) {
    F z = 1;
    while (n != 0) {
        if ((n & 1) != 0) {
            z *= x;
        }
        n >>= 1;
        x *= x;
    }

    return z;
}

/**
 * Thresholding:
 * 0 when a < b, otherwise a
 */
template <typename T, typename U> inline T sc_thresh(T a, U b) { return a < b ? (T)0 : a; }

/**
 * Bilateral clipping.
 * clips input wave a to +/- b
 */
template <typename T> inline T sc_clip2(T a, T b) { return sc_clip(a, -b, b); }

/**
 * Bilateral wrapping.
 * Wraps input a to +/- b
 */
template <typename T> inline T sc_wrap2(T a, T b) { return sc_wrap(a, -b, b); }

/**
 * Bilateral folding.
 * Folds input a to +/- b
 */
template <typename T> inline T sc_fold2(T a, T b) { return sc_fold(a, -b, b); }

/*
 * Excess.
 * Residual of clipping a between -/+b
 */
template <typename T> inline T sc_excess(T a, T b) { return a - sc_clip(a, -b, b); }

/*
 * Scale negative part of input:
 * a*b when a < 0, otherwise a.
 */
template <typename T> inline T sc_scaleneg(T a, T b) {
    if (a < 0)
        return a * b;
    else
        return a;
}

/*
 * Scale negative part of input:
 * a*b when a < 0, otherwise a.
 */
template <> inline float sc_scaleneg<float>(float a, float b) {
    b = 0.5f * b + 0.5f;
    return (std::abs(a) - a) * b + a;
}

/*
 * Scale negative part of input:
 * a*b when a < 0, otherwise a.
 */
template <> inline double sc_scaleneg<double>(double a, double b) {
    b = 0.5 * b + 0.5;
    return (std::abs(a) - a) * b + a;
}

/*
 * Two quadrant multiply.
 * 0 when b <= 0, a*b when b > 0
 */
template <typename T> inline T sc_amclip(T a, T b) {
    if (b < 0)
        return 0;
    else
        return a * b;
}

/*
 * Two quadrant multiply.
 * 0 when b <= 0, a*b when b > 0
 */
template <> inline float sc_amclip<float>(float a, float b) { return a * 0.5f * (b + std::abs(b)); }

/*
 * Two quadrant multiply.
 * 0 when b <= 0, a*b when b > 0
 */
template <> inline double sc_amclip<double>(double a, double b) { return a * 0.5 * (b + std::abs(b)); }

/*
 * Ring modulation plus first source.
 * Return the value of ((a*b) + a).
 */
template <typename T> inline T sc_ring1(T a, T b) { return a * b + a; }

/*
 * Ring modulation plus both sources.
 * Return the value of (a*b) + a + b
 */
template <typename T> inline T sc_ring2(T a, T b) { return a * b + a + b; }

/*
 * Ring modulation variant.
 * Returns the value of a*a*b
 */
template <typename T> inline T sc_ring3(T a, T b) { return a * a * b; }

/*
 * Ring modulation variant.
 * Returns the value of (a * a * b) - (a * b * b)
 * */
template <typename T> inline T sc_ring4(T a, T b) { return a * a * b - a * b * b; }

/*
 * Difference of squares.
 * Return the value of (a*a) - (b*b)
 */
template <typename T> inline T sc_difsqr(T a, T b) { return a * a - b * b; }

/*
 * Sum of squares.
 * Return the value of a*a + b*b
 */
template <typename T> inline T sc_sumsqr(T a, T b) { return a * a + b * b; }

/*
 * Square of the sum.
 * Return the value of (a + b)**2
 */
template <typename T> inline T sc_sqrsum(T a, T b) {
    T z = a + b;
    return z * z;
}

/*
 * Square of the difference.
 * Return the value of (a - b)**2
 */
template <typename T> inline T sc_sqrdif(T a, T b) {
    T z = a - b;
    return z * z;
}

#if 0

inline std::int64_t sc_div(std::int64_t a, std::int64_t b)
{
	int c;
	if (b) {
		if (a<0) c = (a+1)/b - 1;
		else c = a/b;
	} else c = a;
	return c;
}


inline std::int64_t sc_wrap(std::int64_t in, std::int64_t lo, std::int64_t hi)
{
	return sc_mod(in - lo, hi - lo + 1) + lo;
}

/// Folds in to value between lo and hi
inline std::int64_t sc_fold(std::int64_t in, std::int64_t lo, std::int64_t hi)
{
	std::int64_t b = hi - lo;
	int b2 = b+b;
	std::int64_t c = sc_mod(in - lo, b2);
	if (c>b) c = b2-c;
	return c + lo;
}

/// Performs a bitwise and with the number b
inline std::int64_t sc_bitAnd(std::int64_t a, std::int64_t b)
{
	return a & b;
}

/// Performs a bitwise or with the number b
inline std::int64_t sc_bitOr(std::int64_t a, std::int64_t b)
{
	return a | b;
}

/// Performs a binary leftshift with the number b
inline std::int64_t sc_leftShift(std::int64_t a, std::int64_t b)
{
	return a << b;
}

/// Performs a binary rightshift with the number b
inline std::int64_t sc_rightShift(std::int64_t a, std::int64_t b)
{
	return a >> b;
}

/// Recasts a as unsigned and performs a binary rightshift with the number b
inline std::int64_t sc_unsignedRightShift(std::int64_t a, std::int64_t b)
{
	return (std::uint64_t)a >> b;
}

/// Quantization by rounding. Rounds x to the nearest multiple of quant
inline std::int64_t sc_round(std::int64_t x, std::int64_t quant)
{
	return quant==0 ? x : sc_div(x + quant/2, quant) * quant;
}

#endif
