//  simd functions for panning, generic implementation
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


#ifndef SIMD_PAN_GENERIC_HPP
#define SIMD_PAN_GENERIC_HPP

namespace nova
{

template <typename F>
void pan2_vec(F * out0, F * out1, const F * in, F factor0, F factor1, unsigned int n)
{
    do
    {
        F sig = *in++;
        *out0++ = sig * factor0;
        *out1++ = sig * factor1;
    } while(--n);
}

template <typename F>
void pan2_vec_simd(F * out0, F * out1, const F * in, F factor0, F factor1, unsigned int n)
{
    pan2_vec(out0, out1, in, factor0, factor1, n);
}

template <typename F>
void pan2_vec(F * out0, F * out1, const F * in, F factor0, F slope0, F factor1, F slope1, unsigned int n)
{
    do
    {
        F sig = *in++;
        *out0++ = sig * factor0;
        *out1++ = sig * factor1;
        factor0 += slope0;
        factor1 += slope1;
    } while(--n);
}

template <typename F>
void pan2_vec_simd(F * out0, F * out1, const F * in, F factor0, F slope0, F factor1, F slope1, unsigned int n)
{
    pan2_vec(out0, out1, in, factor0, slope0, factor1, slope1, n);
}

} /* namespace nova */

#endif /* SIMD_PAN_GENERIC_HPP */
