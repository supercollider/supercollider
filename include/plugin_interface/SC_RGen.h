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

//----------------------------------------------------------------------------//
// Ran088: L'Ecuyer's 1996 three-component Tausworthe generator "taus88"
//----------------------------------------------------------------------------//
//
// Returns an integer random number uniformly distributed within [0,4294967295]
//
// The period length is approximately 2^88 (which is 3*10^26).
// This generator is very fast and passes all standard statistical tests.
//
// Reference:
//   (1) P. L'Ecuyer, Maximally equidistributed combined Tausworthe generators,
//       Mathematics of Computation, 65, 203-213 (1996), see Figure 4.
//   (2) recommended in:
//       P. L'Ecuyer, Random number generation, chapter 4 of the
//       Handbook on Simulation, Ed. Jerry Banks, Wiley, 1997.
//
//----------------------------------------------------------------------------//

//----------------------------------------------------------------------------//
// I chose this random number generator for the following reasons:
//		fast.
//		easier and faster to seed than other high quality rng's such as Mersenne Twister.
//		the internal state is only 12 bytes.
//		the period is long enough for music/audio.
//		possible to code in altivec in future if needed.
// - James McCartney
//----------------------------------------------------------------------------//

#pragma once

#include "SC_Endian.h"
#include "SC_Types.h"
#include "SC_BoundsMacros.h"
#include "Hash.h"
#include <math.h>

struct RGen {
    void init(uint32 seed);

    uint32 trand();

    int32 irand(int32 scale);
    int32 irand2(int32 scale);
    int32 ilinrand(int32 scale);
    int32 ibilinrand(int32 scale);

    float fcoin();
    float frand();
    float frand2();
    float frand0();
    float frand8();
    float flinrand();
    float fbilinrand();
    float fsum3rand();
    double drand();
    double drand2(double scale);
    double linrand(double scale);
    double bilinrand(double scale);
    double exprandrng(double lo, double hi);
    double exprand(double scale);
    double biexprand(double scale);
    double sum3rand(double scale);

    uint32 s1, s2, s3; // random generator state
};

inline void RGen::init(uint32 seed) {
    // humans tend to use small seeds - mess up the bits
    seed = (uint32)Hash((int)seed);

    // initialize seeds using the given seed value taking care of
    // the requirements. The constants below are arbitrary otherwise
    s1 = 1243598713U ^ seed;
    if (s1 < 2)
        s1 = 1243598713U;
    s2 = 3093459404U ^ seed;
    if (s2 < 8)
        s2 = 3093459404U;
    s3 = 1821928721U ^ seed;
    if (s3 < 16)
        s3 = 1821928721U;
}
/**
 * This function is provided for speed in inner loops where the
 * state variables are loaded into registers.
 * Thus updating the instance variables can
 * be postponed until the end of the loop.
 */
inline uint32 trand(uint32& s1, uint32& s2, uint32& s3) {
    s1 = ((s1 & (uint32)-2) << 12) ^ (((s1 << 13) ^ s1) >> 19);
    s2 = ((s2 & (uint32)-8) << 4) ^ (((s2 << 2) ^ s2) >> 25);
    s3 = ((s3 & (uint32)-16) << 17) ^ (((s3 << 3) ^ s3) >> 11);
    return s1 ^ s2 ^ s3;
}

/// Generate a random 32 bit number
inline uint32 RGen::trand() { return ::trand(s1, s2, s3); }

/// Generate a double from 0.0 to 0.999...
inline double RGen::drand() {
#if BYTE_ORDER == BIG_ENDIAN
    union {
        struct {
            uint32 hi, lo;
        } i;
        double f;
    } du;
#else
    union {
        struct {
            uint32 lo, hi;
        } i;
        double f;
    } du;
#endif
    du.i.hi = 0x41300000;
    du.i.lo = trand();
    return du.f - 1048576.;
}

/// Generate a float from 0.0 to 0.999...
inline float RGen::frand() {
    union {
        uint32 i;
        float f;
    } u; // union for floating point conversion of result
    u.i = 0x3F800000 | (trand() >> 9);
    return u.f - 1.f;
}

/// Generate a float from +1.0 to +1.999...
inline float RGen::frand0() {
    union {
        uint32 i;
        float f;
    } u; // union for floating point conversion of result
    u.i = 0x3F800000 | (trand() >> 9);
    return u.f;
}

/// Generate a float from -1.0 to +0.999...
inline float RGen::frand2() {
    union {
        uint32 i;
        float f;
    } u; // union for floating point conversion of result
    u.i = 0x40000000 | (trand() >> 9);
    return u.f - 3.f;
}

/// Generate a float from -0.125 to +0.124999...
inline float RGen::frand8() {
    union {
        uint32 i;
        float f;
    } u; // union for floating point conversion of result
    u.i = 0x3E800000 | (trand() >> 9);
    return u.f - 0.375f;
}

/// Generate one of the two float values -1.0 or +1.0
inline float RGen::fcoin() {
    union {
        uint32 i;
        float f;
    } u; // union for floating point conversion of result
    u.i = 0x3F800000 | (0x80000000 & trand());
    return u.f;
}

/// Generates a single random float value in linear distribution from 0.0 to 0.999
inline float RGen::flinrand() {
    float a = frand();
    float b = frand();
    return sc_min(a, b);
}

/// Bilateral linearly distributed random float from -0.999 to +0.999.
inline float RGen::fbilinrand() {
    float a = frand();
    float b = frand();
    return a - b;
}

/**
 * Generates a random float that is the
 * result of summing three uniform random generators to yield a
 * bell-like distribution.
 *
 * Larry Polansky's loose approximation of a gaussian generator.
 */
inline float RGen::fsum3rand() { return (float)((frand() + frand() + frand() - 1.5) * 0.666666667); }

/// Generate 32 bit integer from 0 to scale - 1
inline int32 RGen::irand(int32 scale) { return (int32)floor(scale * drand()); }

/// Generate a 32 bit integer from -scale to +scale
inline int32 RGen::irand2(int32 scale) { return (int32)floor((2. * scale + 1.) * drand() - scale); }

/// Generates a single random 32 bit integer value in linear distribution from 0 to +scale
inline int32 RGen::ilinrand(int32 scale) {
    int32 a = irand(scale);
    int32 b = irand(scale);
    return sc_min(a, b);
}

/// Generates a single random double value in linear distribution from 0.0 to +scale
inline double RGen::linrand(double scale) {
    double a = drand();
    double b = drand();
    return sc_min(a, b) * scale;
}

/// Bilateral linearly distributed random 32 bit integer from -scale to +scale.
inline int32 RGen::ibilinrand(int32 scale) {
    int32 a = irand(scale);
    int32 b = irand(scale);
    return a - b;
}

/// Bilateral linearly distributed random double from -scale to +scale.
inline double RGen::bilinrand(double scale) {
    double a = drand();
    double b = drand();
    return (a - b) * scale;
}

/// Generates a single random double value in an exponential distributions from lo to hi
inline double RGen::exprandrng(double lo, double hi) { return lo * exp(log(hi / lo) * drand()); }

inline double RGen::exprand(double scale) {
    double z;
    while ((z = drand()) == 0.0) {}
    return -log(z) * scale;
}

/// Bilateral exponentially distributed random double from -scale to +scale.
inline double RGen::biexprand(double scale) {
    double z;
    while ((z = drand2(1.)) == 0.0 || z == -1.0) {}
    if (z > 0.0)
        z = log(z);
    else
        z = -log(-z);
    return z * scale;
}

/**
 * Generates a random double from -scale to +scale that is the
 * result of summing three uniform random generators to yield a
 * bell-like distribution.
 *
 * Larry Polansky's loose approximation of a gaussian generator.
 */
inline double RGen::sum3rand(double scale) { return (drand() + drand() + drand() - 1.5) * 0.666666667 * scale; }

/// Generate a double from 0.0 to 0.999...
inline double drand(uint32& s1, uint32& s2, uint32& s3) {
    union {
        struct {
            uint32 hi, lo;
        } i;
        double f;
    } u;
    u.i.hi = 0x41300000;
    u.i.lo = trand(s1, s2, s3);
    return u.f - 1048576.;
}

/// Generate a float from 0.0 to 0.999...
inline float frand(uint32& s1, uint32& s2, uint32& s3) {
    union {
        uint32 i;
        float f;
    } u;
    u.i = 0x3F800000 | (trand(s1, s2, s3) >> 9);
    return u.f - 1.f;
}

/// Generate a float from +1.0 to +1.999...
inline float frand0(uint32& s1, uint32& s2, uint32& s3) {
    union {
        uint32 i;
        float f;
    } u;
    u.i = 0x3F800000 | (trand(s1, s2, s3) >> 9);
    return u.f;
}

/// Generate a float from -1.0 to +0.999...
inline float frand2(uint32& s1, uint32& s2, uint32& s3) {
    union {
        uint32 i;
        float f;
    } u;
    u.i = 0x40000000 | (trand(s1, s2, s3) >> 9);
    return u.f - 3.f;
}

/// Generate a float from -0.125 to +0.124999...
inline float frand8(uint32& s1, uint32& s2, uint32& s3) {
    union {
        uint32 i;
        float f;
    } u;
    u.i = 0x3E800000 | (trand(s1, s2, s3) >> 9);
    return u.f - 0.375f;
}

/// Returns one of the two float values -1.0 or +1.0
inline float fcoin(uint32& s1, uint32& s2, uint32& s3) {
    union {
        uint32 i;
        float f;
    } u;
    u.i = 0x3F800000 | (0x80000000 & trand(s1, s2, s3));
    return u.f;
}
