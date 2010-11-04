//  generic vector class
//
//  Copyright (C) 2010 Tim Blechmann
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

#ifndef VEC_HPP
#define VEC_HPP

#include "vec_generic.hpp"
#ifdef __SSE__
#include "vec_sse.hpp"
#endif

#ifdef __SSE2__
#include "vec_sse2.hpp"
#endif

namespace nova
{

template <typename T>
T max_(T const & left, T const & right)
{
    return std::max(left, right);
}

template <typename T>
T min_(T const & left, T const & right)
{
    return std::min(left, right);
}

}

#endif /* VEC_HPP */
