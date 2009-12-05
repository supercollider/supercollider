//  simd functions for mixing, double fallbacks
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

#ifndef SIMD_MIX_FALLBACKS_DOUBLE_HPP
#define SIMD_MIX_FALLBACKS_DOUBLE_HPP

namespace nova
{

template <unsigned int n>
void mix_vec_simd(double * out, const double * in0, double factor0, const double * in1, double factor1)
{
    mix_vec_simd(out, in0, factor0, in1, factor1, n);
}

template <unsigned int n>
inline void mix_vec_simd_mp(double * out, const double * in0, double factor0, const double * in1, double factor1)
{
    mix_vec_simd(out, in0, factor0, in1, factor1, n);
}

template <unsigned int n>
void mix_vec_simd(double * out, const double * in0, double factor0, double slope0,
                  const double * in1, double factor1, double slope1)
{
    mix_vec_simd(out, in0, factor0, slope0, in1, factor1, slope1, n);
}

template <unsigned int n>
inline void mix_vec_simd_mp(double * out, const double * in0, double factor0, double slope0,
                            const double * in1, double factor1, double slope1)
{
    mix_vec_simd(out, in0, factor0, slope0, in1, factor1, slope1, n);
}

} /* namespace nova */

#undef always_inline

#endif /* SIMD_MIX_FALLBACKS_DOUBLE_HPP */
