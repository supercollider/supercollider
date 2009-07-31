//
//
//  Copyright (C) 2008 Tim Blechmann
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

#ifndef NOISE_UTILS_HPP
#define NOISE_UTILS_HPP

#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>

#include <boost/random.hpp>
#include <boost/cstdint.hpp>

#include "../utilities/branch_hints.hpp"

#include "noise_engines.hpp"

namespace nova
{
namespace detail
{

using boost::uint32_t;
typedef boost::rand48 nova_default_rng_engine;

inline int generate_seed(void)
{
    static nova_default_rng_engine rng;
    static boost::mutex mutex;

    boost::mutex::scoped_lock lock(mutex);
    return rng();
}


inline uint32_t thread_local_noise (void)
{
    static __thread uint32_t val = 0;

    if (unlikely(val==0))
        val = generate_seed();

    val = val * 435898247 + 382842987;
    return val;
}

/** scale random int to interval (-1..1]
 *
 * */
/* @{ */

template <typename float_type>
inline float_type white_noise_scale(uint32_t rand)
{
    return (float_type(2.0) * float_type(rand) /
            (float_type(std::numeric_limits<uint32_t>::max()) + float_type(1.0)))
        - float_type(1.0);
}

template <>
inline float white_noise_scale(uint32_t rand)
{
    union
    {
        boost::uint32_t i;
        float f;
    } u;
    u.i = 0x40000000 | (rand >> 9);  /* set sign & exponent, randomize mantissa  */
    return u.f - 3.f;
}
/* @} */

/** scale random int to -1 or 1
 *
 *  optimized version adapted from supercollider
 *
 * */
/* @{ */
template <typename float_type>
inline float_type clipped_noise_scale(uint32_t rand)
{
    if (0x80000000 & rand)
        return float_type(1);
    else
        return float_type(-1);
}

template <>
inline float clipped_noise_scale(uint32_t rand)
{
    union
    {
        boost::uint32_t i;
        float f;
    } u;
    u.i = 0x3F800000 | (0x80000000 & rand);  /* set exponent & mantissa, randomize sign  */
    return u.f;
}
/* @} */

/** scale random int to [0..1)
 *
 * */
/* @{ */
template <typename float_type>
inline float_type noise_scale_1(uint32_t rand)
{
    return float_type(rand) /
        (float_type(std::numeric_limits<uint32_t>::max()) + float_type(1.0));
}

template <>
inline float noise_scale_1(uint32_t rand)
{
    union
    {
        boost::uint32_t i;
        float f;
    } u;
    u.i = 0x3E800000 | (rand >> 9); /* set sign & exponent, randomize mantissa  */
    return u.f - 1;
}
/* @} */

/** scale random int to [-0.125..0.125)
 *
 * */
/* @{ */
template <typename float_type>
inline float_type noise_scale_8(uint32_t rand)
{
    return (float_type(0.25) * float_type(rand) /
            (float_type(std::numeric_limits<uint32_t>::max()) + float_type(1.0)))
        - float_type(0.125);
}

template <>
inline float noise_scale_8(uint32_t rand)
{
    union
    {
        boost::uint32_t i;
        float f;
    } u;
    u.i = 0x3E800000 | (rand >> 9); /* set sign & exponent, randomize mantissa  */
    return u.f;
}
/* @} */


} /* namespace detail */

template <typename sample_type>
class pos_white_noise
{
public:
    pos_white_noise(void)
    {
        gen.seed(detail::generate_seed());
    }

    inline sample_type step(sample_type max)
    {
        return sample_type(gen()) / sample_type(gen.max()) * max;
    }

private:
    detail::cheap_noise_engine gen;
};


/* @{ */
/* thread-safe templated noise generators */
template <typename sample_type>
sample_type generate_white_noise(void)
{
    uint val = detail::thread_local_noise();
    return ((float)((val & 0x7fffffff) - 0x40000000)) *
        (float)(1.0 / 0x40000000);
}

template <typename sample_type>
sample_type generate_pos_white_noise(sample_type max)
{
    uint val = detail::thread_local_noise();

    return val / sample_type(std::numeric_limits<uint>::max()) * max;
}
/* @} */


} /* namespace nova */

#endif /* NOISE_UTILS_HPP */
