//  branch hints
//  Copyright (C) 2007-2015 Tim Blechmann
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

#ifndef BRANCH_HINTS_HPP
#define BRANCH_HINTS_HPP

#include <boost/config.hpp>

#define likely(x)   BOOST_LIKELY(x)
#define unlikely(x) BOOST_UNLIKELY(x)

#endif /* BRANCH_HINTS_HPP */
