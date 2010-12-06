//  cos table
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

#ifndef COS_TABLE_HPP
#define COS_TABLE_HPP

#include <cmath>

#include <boost/math/constants/constants.hpp>

#include "cache_aligned_array.hpp"


namespace nova
{

template<typename sampletype, uint cos_size, uint overlap>
class cos_table:
    public aligned_array<sampletype, cos_size + overlap>
{
    typedef aligned_array<sampletype, cos_size + overlap> array_type;

public:
    cos_table(void)
    {
        for (uint i = 0; i != cos_size + overlap; ++i)
            array_type::at(i) = std::cos (sampletype(i) / sampletype(cos_size) * 2
                                          * boost::math::constants::pi<sampletype>());
    }

    static const uint size = cos_size;
};

template<typename sampletype, uint tab_size, uint overlap>
class half_sin_table:
    public aligned_array<sampletype, tab_size + overlap>
{
    typedef aligned_array<sampletype, tab_size + overlap> array_type;

public:
    half_sin_table(void)
    {
        for (uint i = 0; i != tab_size + overlap; ++i)
            array_type::at(i) = std::sin (sampletype(i) / sampletype(tab_size)
                                          * boost::math::constants::pi<sampletype>());
    }

    static const uint size = tab_size;
};



/* gcc x86_64 workaround ... they shouldn't be needed ... */
/* @{ */
template<typename sampletype, uint cos_size, uint overlap>
inline const sampletype * get_samples(cos_table<sampletype, cos_size, overlap> const & buffer)
{
    return buffer.begin();
}

template<typename sampletype, uint tab_size, uint overlap>
inline const sampletype * get_samples(half_sin_table<sampletype, tab_size, overlap> const & buffer)
{
    return buffer.begin();
}
/* @} */


} /* namespace nova */

#endif /* COS_TABLE_HPP */
