//  implementation of several noise engines
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


#ifndef DSP_NOISE_ENGINES_HPP
#define DSP_NOISE_ENGINES_HPP

#include <boost/cstdint.hpp>
#include <boost/array.hpp>

#include <limits>               /* for numeric_limits */

namespace nova
{
namespace detail
{

/** \brief cheap noise engine
 *
 *  hardcoded linear congruental rng
 *
 *  usable as white noise source, otherwise it is quite a bad quality rng
 *
 * */
struct cheap_noise_engine
{
    typedef uint32_t result_type;

    cheap_noise_engine(result_type i = 0):
        val(i)
    {}

    cheap_noise_engine(cheap_noise_engine const & rhs):
        val(rhs.val)
    {}

    void seed(result_type i)
    {
        val = i;
    }

    result_type operator()(void)
    {
        val = val * 435898247 + 382842987;
        return val;
    }

    void operator=(cheap_noise_engine const & rhs)
    {
        val = rhs.val;
    }

    result_type min(void) {return std::numeric_limits<result_type>::min();}
    result_type max(void) {return std::numeric_limits<result_type>::max();}

private:
    result_type val;
};

/** \brief taus88 noise generator engine
 *
 *  see:
 *  P. L'Ecuyer, "Maximally Equidistributed Combined Tausworthe
 *  Generators", Mathematics of Computation, 65, 213 (1996), 203--213.
 *
 *
 * */
struct taus88_engine
{
    typedef uint32_t result_type;

    taus88_engine(result_type s = 0)
    {
        seed(s);
    }

    taus88_engine(taus88_engine const & rhs):
        state(rhs.state)
    {}

    void seed(result_type s)
    {
        /* seeding algorithm used in different implementations */
        state[0] = 1243598713U ^ s;
        if (state[0] < 2)
            state[0] = 1243598713U;

        state[1] = 3093459404U ^ s;
        if (state[1] < 8)
            state[1] = 3093459404U;

        state[2] = 1821928721U ^ s;
        if (state[2] < 16)
            state[2] = 1821928721U;
    }

    result_type operator()(void)
    {
        state[0] = (((state[0] & 4294967294u) << 12) ^ (((state[0] << 13) ^ state[0]) >> 19));
        state[1] = (((state[1] & 4294967288u) <<  4) ^ (((state[1] <<  2) ^ state[1]) >> 25));
        state[2] = (((state[2] & 4294967280u) << 17) ^ (((state[2] <<  3) ^ state[2]) >> 11));

        return state[0] ^ state[1] ^ state[2];
    }

    void operator=(taus88_engine const & rhs)
    {
        state = rhs.state;
    }

    result_type min(void) {return std::numeric_limits<result_type>::min();}
    result_type max(void) {return std::numeric_limits<result_type>::max();}

private:
    boost::array<result_type, 3> state;
};



} /* namespace detail */
} /* namespace nova */








#endif /* DSP_NOISE_ENGINES_HPP */
