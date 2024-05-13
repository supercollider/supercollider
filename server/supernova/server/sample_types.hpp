//  basic types for the ugen system
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

#pragma once

#ifdef __GNUC__
#    define RESTRICTED_POINTERS
#    define ALIGNED_POINTERS
#endif

namespace nova {
typedef float sample;

#ifndef RESTRICTED_POINTERS
#    define __restrict__ /*__restrict__*/
#endif /* RESTRICTED_POINTERS */

#ifdef ALIGNED_POINTERS
typedef sample aligned_sample __attribute__((aligned(4 * sizeof(sample))));
#else
typedef sample aligned_sample;
#endif /* ALIGNED_POINTERS */

typedef aligned_sample* __restrict__ restricted_sample_ptr;
typedef const aligned_sample* __restrict__ const_restricted_sample_ptr;

typedef aligned_sample* aligned_sample_ptr;
typedef const aligned_sample* const_aligned_sample_ptr;

} /* namespace nova */
