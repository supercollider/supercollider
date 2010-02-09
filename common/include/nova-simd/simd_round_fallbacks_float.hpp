//  simdfied round functions, fallback functions
//  Copyright (C) 2009 Tim Blechmann <tim@klingt.org>
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

#ifndef SIMD_ROUND_FALLBACKS_FLOAT_HPP
#define SIMD_ROUND_FALLBACKS_FLOAT_HPP

namespace nova {

template <unsigned int n>
inline void round_vec_simd(float * out, const float * arg)
{
    round_vec_simd(out, arg, n);
}

template <unsigned int n>
inline void ceil_vec_simd(float * out, const float * arg)
{
    ceil_vec_simd(out, arg, n);
}

template <unsigned int n>
inline void floor_vec_simd(float * out, const float * arg)
{
    floor_vec_simd(out, arg, n);
}

template <unsigned int n>
inline void frac_vec_simd(float * out, const float * arg)
{
    frac_vec_simd(out, arg, n);
}

template <unsigned int n>
inline void round_vec_simd_mp(float * out, const float * arg)
{
    round_vec_simd(out, arg, n);
}

template <unsigned int n>
inline void ceil_vec_simd_mp(float * out, const float * arg)
{
    ceil_vec_simd(out, arg, n);
}

template <unsigned int n>
inline void floor_vec_simd_mp(float * out, const float * arg)
{
    floor_vec_simd(out, arg, n);
}

template <unsigned int n>
inline void frac_vec_simd_mp(float * out, const float * arg)
{
    frac_vec_simd(out, arg, n);
}

} /* namespace nova */

#endif /* SIMD_ROUND_FALLBACKS_FLOAT_HPP */
