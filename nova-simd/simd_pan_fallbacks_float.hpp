//  simd functions for panning, fallback implementation for float
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


#ifndef SIMD_PAN_FALLBACKS_FLOAT_HPP
#define SIMD_PAN_FALLBACKS_FLOAT_HPP

namespace nova
{

template <unsigned int n>
void pan2_vec_simd(float * out0, float * out1, const float * in,
                   float factor0, float factor1)
{
    pan2_vec_simd(out0, out1, in, factor0, factor1, n);
}

template <unsigned int n>
inline void pan2_vec_simd_mp(float * out0, float * out1, const float * in,
                             float factor0, float factor1)
{
    pan2_vec_simd(out0, out1, in, factor0, factor1, n);
}

template <unsigned int n>
void pan2_vec_simd(float * out0, float * out1, const float * in, float factor0, float slope0,
                   float factor1, float slope1)
{
    pan2_vec_simd(out0, out1, in, factor0, slope0, factor1, slope1, n);
}

template <unsigned int n>
inline void pan2_vec_simd_mp(float * out0, float * out1, const float * in, float factor0, float slope0,
                   float factor1, float slope1)
{
    pan2_vec_simd(out0, out1, in, factor0, slope0, factor1, slope1, n);
}

} /* namespace nova */

#endif /* SIMD_PAN_FALLBACKS_FLOAT_HPP */
