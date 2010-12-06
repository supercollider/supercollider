//  $Id$
//
//  interpolation functions
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

#ifndef INTERPOLATION_HPP
#define INTERPOLATION_HPP

#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>

#include <cmath>

namespace nova
{

namespace detail
{
template <typename table_type,
          typename index_type>
inline table_type interpolation_helper(const table_type * old_table, const index_type x,
                                       const unsigned int iteration, const unsigned int points)
{
    if (points == 0)
    {
        return old_table[0];
    }

    table_type table[points];

    for (uint i = 0; i != points; ++i)
    {
        const index_type x_i = i;
        const index_type x_j = i+iteration;

        table_type left = old_table[i];
        table_type right = old_table[i+1];

        table [i] = ((x - x_i) * right - (x - x_j) * left) / (x_j - x_i);
    }

    return interpolation_helper(table, x, iteration + 1, points - 1);
}

}


/** \brief neville interpolation algorithm
 *
 *  constraints:
 *  - index should be between 0 and points
 *  - index should be between floor(points/2) and seil(points/2) for best results
 *  - table is expected to be valid from table to table+points
 *
 * */
/* @{ */
template <typename table_type,
          typename index_type>
inline table_type interpolate(const table_type * table, index_type index, unsigned int points)
{
    assert(index >= 0);
    assert(index < points);

    return detail::interpolation_helper(table, index, 1, points-1);
}



template <uint points,
          typename table_type,
          typename index_type>
inline table_type interpolate(const table_type * table, const index_type index)
{
    assert(index >= 0);
    assert(index < points);

    return detail::interpolation_helper(table, index, 1, points-1);
}
/* @} */

/** \brief linear interpolation */
/* @{ */
template <typename index_type,
          typename value_type>
inline value_type linearinterp(index_type frac_pos, value_type x0, value_type x1)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<value_type>::value);
    BOOST_STATIC_ASSERT(boost::is_floating_point<index_type>::value);
    return x0 + frac_pos * (x1 - x0);
}

template <typename index_type,
          typename value_type>
inline value_type linearinterp(value_type * buffer, index_type index)
{
    std::size_t left_index = std::size_t(std::floor(index));
    std::size_t right_index = left_index + 1;

    index_type frac_pos = index - left_index;
    return linearinterp(frac_pos, buffer[left_index], buffer[right_index]);
}
/* @} */


/** \brief laurent de soras's cubic interpolation from musicdsp */
/* @{ */
template <typename index_type,
          typename value_type>
inline value_type cubicinterp(index_type frac_pos, value_type xm1, value_type x0, value_type x1, value_type x2)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<index_type>::value);
    BOOST_STATIC_ASSERT(boost::is_floating_point<value_type>::value);
    const value_type c = (x1 - xm1) * 0.5;
    const value_type v = x0 - x1;
    const value_type w = c + v;
    const value_type a = w + v + (x2 - x0) * 0.5;
    const value_type b_neg = w + a;

    return ((((a * frac_pos) - b_neg) * frac_pos + c) * frac_pos + x0);
}

template <typename index_type,
          typename value_type>
inline value_type cubicinterp(value_type * buffer, index_type index)
{
    std::size_t x0 = std::size_t(std::floor(index));
    std::size_t xm = x0 - 1;
    std::size_t x1 = x0 + 1;
    std::size_t x2 = x0 + 2;

    index_type frac_pos = index - x0;
    return cubicinterp(frac_pos, buffer[xm], buffer[x0], buffer[x1], buffer[x2]);
}


/* @} */

template <uint interpolation, typename float_type, typename index_type>
inline float_type interpolation_wrapper(float_type * buffer, index_type index)
{
    /* the compiler will optimize this */
    switch (interpolation)
    {
    case 0: // no interpolation
    {
        std::size_t table_index = std::size_t(std::floor(index));
        return buffer[table_index];
    }
    case 1: // linear interpolation
    {
        return linearinterp(buffer, index);
    }

    case 3: // cubic interpolation (use heremite interpolation)
    {
        return cubicinterp(buffer, index);
    }

    default:
    {
        const uint interpolation_points = interpolation + 1;

        std::size_t offset = std::size_t(std::floor(index));

        return interpolate<interpolation_points>(buffer + offset, index - offset);
    }

    }
}

} /* namespace nova */


#endif /* INTERPOLATION_HPP */
