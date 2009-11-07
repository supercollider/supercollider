//  simd functions for mixing, generic implementation
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


#ifndef SIMD_MIX_GENERIC_HPP
#define SIMD_MIX_GENERIC_HPP

namespace nova
{

template <typename F>
void mix_vec(F * out, const F * in0, F factor0, const F * in1, F factor1, unsigned int n)
{
    do {
        *out++ = *in0++ * factor0 + *in1++ * factor1;
    } while(--n);
}

template <typename F>
void mix_vec_simd(F * out, const F * in0, F factor0, const F * in1, F factor1, unsigned int n)
{
    n /= 8;
    do {
        *out++ = *in0++ * factor0 + *in1++ * factor1;
        *out++ = *in0++ * factor0 + *in1++ * factor1;
        *out++ = *in0++ * factor0 + *in1++ * factor1;
        *out++ = *in0++ * factor0 + *in1++ * factor1;
        *out++ = *in0++ * factor0 + *in1++ * factor1;
        *out++ = *in0++ * factor0 + *in1++ * factor1;
        *out++ = *in0++ * factor0 + *in1++ * factor1;
        *out++ = *in0++ * factor0 + *in1++ * factor1;
    } while(--n);
}

template <typename F>
void mix_vec(F * out, const F * in0, F factor0, F slope0,
             const F * in1, F factor1, F slope1, unsigned int n)
{
    do {
        *out++ = *in0++ * factor0 + *in1++ * factor1; factor0 += slope0; factor1 += slope1;
    } while(--n);
}

template <typename F>
void mix_vec_simd(F * out, const F * in0, F factor0, F slope0,
                  const F * in1, F factor1, F slope1, unsigned int n)
{
    n /= 8;
    do {
        *out++ = *in0++ * factor0 + *in1++ * factor1; factor0 += slope0; factor1 += slope1;
        *out++ = *in0++ * factor0 + *in1++ * factor1; factor0 += slope0; factor1 += slope1;
        *out++ = *in0++ * factor0 + *in1++ * factor1; factor0 += slope0; factor1 += slope1;
        *out++ = *in0++ * factor0 + *in1++ * factor1; factor0 += slope0; factor1 += slope1;
        *out++ = *in0++ * factor0 + *in1++ * factor1; factor0 += slope0; factor1 += slope1;
        *out++ = *in0++ * factor0 + *in1++ * factor1; factor0 += slope0; factor1 += slope1;
        *out++ = *in0++ * factor0 + *in1++ * factor1; factor0 += slope0; factor1 += slope1;
        *out++ = *in0++ * factor0 + *in1++ * factor1; factor0 += slope0; factor1 += slope1;
    } while(--n);
}

} /* namespace nova */


#endif /* SIMD_MIX_GENERIC_HPP */
