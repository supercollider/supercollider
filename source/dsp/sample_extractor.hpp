//  sample extractor function
//  Copyright (C) 2007, 2008 Tim Blechmann
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

#ifndef SAMPLE_EXTRACTOR_HPP
#define SAMPLE_EXTRACTOR_HPP

#include <boost/array.hpp>

namespace nova
{
/* get_samples implementation for float and double pointers */
/* @{ */
inline float * get_samples (float * arg)
{
    return arg;
}

inline const float * get_samples (const float * arg)
{
    return arg;
}

inline double * get_samples (double * arg)
{
    return arg;
}

inline const double * get_samples (const double * arg)
{
    return arg;
}

inline float * get_samples (float ** arg)
{
    return arg[0];
}

inline const float * get_samples (const float ** arg)
{
    return arg[0];
}

inline double * get_samples (double ** arg)
{
    return arg[0];
}

inline const double * get_samples (const double ** arg)
{
    return arg[0];
}

template <typename sample_type, unsigned int n>
inline sample_type * get_samples(boost::array<sample_type, n> & arg)
{
    return arg.begin();
}

template <typename sample_type, unsigned int n>
inline const sample_type * get_samples(boost::array<sample_type, n> const & arg)
{
    return arg.begin();
}

/* @} */
}

#endif /* SAMPLE_EXTRACTOR_HPP */
