//  simdfied round functions
//  Copyright (C) 2009 Tim Blechmann
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

#ifndef SIMD_ROUND_HPP
#define SIMD_ROUND_HPP

#include "simd_unroll_constraints.hpp"
#include "simd_round_generic.hpp"

#ifdef __SSE4_1__
#include "simd_round_sse41.hpp"
#elif !defined(NO_GPL3_CODE) && defined(__GNUC__)                       \
    && !( (__GNUC__ < 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ == 1))) && defined (__SSE__)
#include "simd_round_vec.hpp"
#endif

#endif /* SIMD_ROUND_HPP */
