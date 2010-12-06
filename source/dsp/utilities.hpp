//  $Id$
//
//  some utility functions
//  Copyright (C) 2006, 2007 Tim Blechmann
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

//  $Revision$
//  $LastChangedRevision$
//  $LastChangedDate$
//  $LastChangedBy$

#ifndef _UTILITIES_HPP
#define _UTILITIES_HPP

/* #include "../primitives/float.hpp" */
#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include <cassert>
#include <cmath>

namespace nova
{

/** \brief amplitude to db conversion */
template <typename T>
inline T amp2db(T amp)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<T>::value);
    if (unlikely(amp <= 0.f))
        return -120;
    else
        return log10(amp) * T(10);
}

/** \brief db to amplitude conversion */
template <typename T>
inline T db2amp(T db)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<T>::value);
    return pow(T(10), db * T(0.1));
}

template <typename T>
inline T rms2db(T amp)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<T>::value);
    if (unlikely(amp <= 0.f))
        return -120;
    else
        return log10(amp) * T(20);
}

template <typename T>
inline T db2rms(T db)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<T>::value);
    return pow(T(10), db * T(0.05));
}

/** \brief midi to frequency conversion */
template <typename T>
inline T midi2frequency(T midi)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<T>::value);
    const T expfactor = 0.0577622650467; // log(2**(1/12.))
    const T basefactor = 8.17579891564;  // 2 ** (-57/12.) * 440 / 2

    return basefactor * exp(expfactor * midi);
}

/** \brief frequency to midi conversion */
template <typename T>
inline T frequency2midi(T freq)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<T>::value);
    const T OneDiv440 = 1./440;
    if (unlikely(freq == 0.f))
        return T(-1500);
    else
        return T(12) * log2(freq * OneDiv440) + T(57);
}


/** \brief compute a decay factor from for a feedback delay line */
template <typename T>
inline T decay_factor(T delay, T decay)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<T>::value);
    const T log_threshold = log(0.001); // db2amp(-60)
    return exp(log_threshold * delay / decay);
}

/* decay factor for a delay of 1 */
template <typename T>
inline T decay_factor(T decay)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<T>::value);
    const T log_threshold = log(0.001); // db2amp(-60)
    return exp(log_threshold / decay);
}

template <typename T>
inline T GetDecayFactor(T delaytime, T decaytime)
{
    return decay_factor(delaytime, decaytime);
}



template <typename T>
inline T phasor_increment(T frequency, T samplerate)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<T>::value);
    return frequency / samplerate;
}

} /* namespace nova */




#endif /* _UTILITIES_HPP */
