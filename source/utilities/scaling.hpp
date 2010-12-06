//  templated scaling functions
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

#ifndef SCALING_HPP
#define SCALING_HPP

#include "force_inline.hpp"

namespace nova
{

template <typename I>
always_inline I scale_linear(I in,
                             I const & low_in, I const & high_in,
                             I const & low_out, I const & high_out)
{
    return (in - low_in) / (high_in - low_in) * (high_out - low_out) + low_out;
}

template <typename I>
always_inline I scale_linear_clipped(I in,
                                     I const & low_in, I const & high_in,
                                     I const & low_out, I const & high_out)
{
    if (in <= low_in)
        return low_in;
    if (in >= high_in)
        return high_in;

    return (in - low_in) / (high_in - low_in) * (high_out - low_out) + low_out;
}

} /* namespace nova */

#endif /* SCALING_HPP */
