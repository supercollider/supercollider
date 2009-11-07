//  binary arithmetic simd function fallbacks, float version
//  Copyright (C) 2008, 2009 Tim Blechmann <tim@klingt.org>
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
//
//  implemented as part of nova

#ifndef SIMD_BINARY_ARITHMETIC_FALLBACKS_FLOAT_HPP
#define SIMD_BINARY_ARITHMETIC_FALLBACKS_FLOAT_HPP

#include "simd_binary_arithmetic.hpp"

namespace nova {

/* vector/vector */
template <int n>
inline void plus_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    plus_vec_simd(out, src1, src2, n);
}

template <int n>
inline void plus_vec_simd(float * out, const float * src1, const float * src2)
{
    plus_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void plus_vec_simd_mp(float * out, const float * src1, const float src2)
{
    plus_vec_simd(out, src1, src2, n);
}

template <int n>
inline void plus_vec_simd(float * out, const float * src1, const float src2)
{
    plus_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void plus_vec_simd_mp(float * out, const float src1, const float * src2)
{
    plus_vec_simd(out, src1, src2, n);
}

template <int n>
inline void plus_vec_simd(float * out, const float src1, const float * src2)
{
    plus_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void minus_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    minus_vec_simd(out, src1, src2, n);
}

template <int n>
inline void minus_vec_simd(float * out, const float * src1, const float * src2)
{
    minus_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void minus_vec_simd_mp(float * out, const float * src1, const float src2)
{
    minus_vec_simd(out, src1, src2, n);
}

template <int n>
inline void minus_vec_simd(float * out, const float * src1, const float src2)
{
    minus_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void minus_vec_simd_mp(float * out, const float src1, const float * src2)
{
    minus_vec_simd(out, src1, src2, n);
}

template <int n>
inline void minus_vec_simd(float * out, const float src1, const float * src2)
{
    minus_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void times_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    times_vec_simd(out, src1, src2, n);
}

template <int n>
inline void times_vec_simd(float * out, const float * src1, const float * src2)
{
    times_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void times_vec_simd_mp(float * out, const float * src1, const float src2)
{
    times_vec_simd(out, src1, src2, n);
}

template <int n>
inline void times_vec_simd(float * out, const float * src1, const float src2)
{
    times_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void times_vec_simd_mp(float * out, const float src1, const float * src2)
{
    times_vec_simd(out, src1, src2, n);
}

template <int n>
inline void times_vec_simd(float * out, const float src1, const float * src2)
{
    times_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void over_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    over_vec_simd(out, src1, src2, n);
}

template <int n>
inline void over_vec_simd(float * out, const float * src1, const float * src2)
{
    over_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void over_vec_simd_mp(float * out, const float * src1, const float src2)
{
    over_vec_simd(out, src1, src2, n);
}

template <int n>
inline void over_vec_simd(float * out, const float * src1, const float src2)
{
    over_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void over_vec_simd_mp(float * out, const float src1, const float * src2)
{
    over_vec_simd(out, src1, src2, n);
}

template <int n>
inline void over_vec_simd(float * out, const float src1, const float * src2)
{
    over_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void min_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    min_vec_simd(out, src1, src2, n);
}

template <int n>
inline void min_vec_simd(float * out, const float * src1, const float * src2)
{
    min_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void min_vec_simd_mp(float * out, const float * src1, const float src2)
{
    min_vec_simd(out, src1, src2, n);
}

template <int n>
inline void min_vec_simd(float * out, const float * src1, const float src2)
{
    min_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void min_vec_simd_mp(float * out, const float src1, const float * src2)
{
    min_vec_simd(out, src1, src2, n);
}

template <int n>
inline void min_vec_simd(float * out, const float src1, const float * src2)
{
    min_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void max_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    max_vec_simd(out, src1, src2, n);
}

template <int n>
inline void max_vec_simd(float * out, const float * src1, const float * src2)
{
    max_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void max_vec_simd_mp(float * out, const float * src1, const float src2)
{
    max_vec_simd(out, src1, src2, n);
}

template <int n>
inline void max_vec_simd(float * out, const float * src1, const float src2)
{
    max_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void max_vec_simd_mp(float * out, const float src1, const float * src2)
{
    max_vec_simd(out, src1, src2, n);
}

template <int n>
inline void max_vec_simd(float * out, const float src1, const float * src2)
{
    max_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void less_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    less_vec_simd(out, src1, src2, n);
}

template <int n>
inline void less_vec_simd(float * out, const float * src1, const float * src2)
{
    less_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void less_vec_simd_mp(float * out, const float * src1, const float src2)
{
    less_vec_simd(out, src1, src2, n);
}

template <int n>
inline void less_vec_simd(float * out, const float * src1, const float src2)
{
    less_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void less_vec_simd_mp(float * out, const float src1, const float * src2)
{
    less_vec_simd(out, src1, src2, n);
}

template <int n>
inline void less_vec_simd(float * out, const float src1, const float * src2)
{
    less_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void less_equal_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    less_equal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void less_equal_vec_simd(float * out, const float * src1, const float * src2)
{
    less_equal_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void less_equal_vec_simd_mp(float * out, const float * src1, const float src2)
{
    less_equal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void less_equal_vec_simd(float * out, const float * src1, const float src2)
{
    less_equal_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void less_equal_vec_simd_mp(float * out, const float src1, const float * src2)
{
    less_equal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void less_equal_vec_simd(float * out, const float src1, const float * src2)
{
    less_equal_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void greater_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    greater_vec_simd(out, src1, src2, n);
}

template <int n>
inline void greater_vec_simd(float * out, const float * src1, const float * src2)
{
    greater_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void greater_vec_simd_mp(float * out, const float * src1, const float src2)
{
    greater_vec_simd(out, src1, src2, n);
}

template <int n>
inline void greater_vec_simd(float * out, const float * src1, const float src2)
{
    greater_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void greater_vec_simd_mp(float * out, const float src1, const float * src2)
{
    greater_vec_simd(out, src1, src2, n);
}

template <int n>
inline void greater_vec_simd(float * out, const float src1, const float * src2)
{
    greater_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void greater_equal_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    greater_equal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void greater_equal_vec_simd(float * out, const float * src1, const float * src2)
{
    greater_equal_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void greater_equal_vec_simd_mp(float * out, const float * src1, const float src2)
{
    greater_equal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void greater_equal_vec_simd(float * out, const float * src1, const float src2)
{
    greater_equal_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void greater_equal_vec_simd_mp(float * out, const float src1, const float * src2)
{
    greater_equal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void greater_equal_vec_simd(float * out, const float src1, const float * src2)
{
    greater_equal_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void equal_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    equal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void equal_vec_simd(float * out, const float * src1, const float * src2)
{
    equal_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void equal_vec_simd_mp(float * out, const float * src1, const float src2)
{
    equal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void equal_vec_simd(float * out, const float * src1, const float src2)
{
    equal_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void equal_vec_simd_mp(float * out, const float src1, const float * src2)
{
    equal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void equal_vec_simd(float * out, const float src1, const float * src2)
{
    equal_vec_simd_mp<n>(out, src1, src2);
}


/* vector/vector */
template <int n>
inline void notequal_vec_simd_mp(float * out, const float * src1, const float * src2)
{
    notequal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void notequal_vec_simd(float * out, const float * src1, const float * src2)
{
    notequal_vec_simd_mp<n>(out, src1, src2);
}

/* vector/scalar */
template <int n>
inline void notequal_vec_simd_mp(float * out, const float * src1, const float src2)
{
    notequal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void notequal_vec_simd(float * out, const float * src1, const float src2)
{
    notequal_vec_simd_mp<n>(out, src1, src2);
}

/* scalar/vector */
template <int n>
inline void notequal_vec_simd_mp(float * out, const float src1, const float * src2)
{
    notequal_vec_simd(out, src1, src2, n);
}

template <int n>
inline void notequal_vec_simd(float * out, const float src1, const float * src2)
{
    notequal_vec_simd_mp<n>(out, src1, src2);
}


} /* namespace nova */

#endif /* SIMD_BINARY_ARITHMETIC_FALLBACKS_FLOAT_HPP */
