//  fast approximations different functions
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

#include <boost/type_traits.hpp>
#include <boost/static_assert.hpp>
#include <boost/math/constants/constants.hpp>

namespace nova
{

/* fast atan2 approximation
 *
 * adapted from pseudocode posted in
 * http://www.dspguru.com/comp.dsp/tricks/alg/fxdatan2.htm
 *
 * max error of .072 rads, if abs(x) > 1e-8
 * */
template <typename F>
F faster_atan2(F y, F x)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<F>::value);

    F coeff_1 = boost::math::constants::pi<F>()/F(4);
    F coeff_2 = F(3)*coeff_1;
    F abs_y = std::abs(y) + F(1e-10);      // kludge to prevent 0/0 condition

    F angle;
    if (x>=0)
    {
        F r = (x - abs_y) / (x + abs_y);
        angle = coeff_1 - coeff_1 * r;
    }
    else
    {
        F r = (x + abs_y) / (abs_y - x);
        angle = coeff_2 - coeff_1 * r;
    }
    if (y < 0)
        return(-angle);     // negate if in quad III or IV
    else
        return(angle);
}

/* fast atan2 approximation
 *
 * adapted from pseudocode posted in
 * http://www.dspguru.com/comp.dsp/tricks/alg/fxdatan2.htm
 *
 * max error of .011 rads, if abs(x) > 1e-8
 * */
template <typename F>
F fast_atan2(F y, F x)
{
    BOOST_STATIC_ASSERT(boost::is_floating_point<F>::value);

    F coeff_1 = boost::math::constants::pi<F>()/F(4);
    F coeff_2 = F(3)*coeff_1;
    F abs_y = std::abs(y) + F(1e-10);      // kludge to prevent 0/0 condition

    F angle;
    if (x>=0)
    {
        F r = (x - abs_y) / (x + abs_y);
        angle = F(0.1963) * r*r*r - F(0.9817) * r + coeff_1;
    }
    else
    {
        F r = (x + abs_y) / (abs_y - x);
        angle = F(0.1963) * r*r*r - F(0.9817) * r + coeff_2;
    }
    if (y < 0)
        return(-angle);     // negate if in quad III or IV
    else
        return(angle);
}

} /* namespace nova */
