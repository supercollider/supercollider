//  amplitude modulation, generic implementation
//
//  Copyright (C) 2008, 2009 Tim Blechmann
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

#ifndef SIMD_AMPMOD_GENERIC_HPP
#define SIMD_AMPMOD_GENERIC_HPP

namespace nova
{

template <typename float_type>
inline void amp_mod(float_type * out, const float_type * in1, const float_type * in2,
                    const float_type amount, unsigned int n)
{
    for (unsigned int i = 0; i != n; ++i)
        out[i] = in1[i] * (float_type(1) + in2[i] * amount);
}

template <typename float_type>
inline void amp_mod(float_type * out, const float_type * in1, const float_type * in2,
                    float_type amount, const float_type amount_slope,
                    unsigned int n)
{
    for (unsigned int i = 0; i != n; ++i)
    {
        out[i] = in1[i] * (float_type(1) + in2[i] * amount);
        amount += amount_slope;
    }
}

template <typename float_type>
inline void amp_mod(float_type * out, const float_type * in1, const float_type * in2,
                    const float_type * amount, unsigned int n)
{
    for (unsigned int i = 0; i != n; ++i)
        out[i] = in1[i] * (float_type(1) + in2[i] * amount[i]);
}

template <typename float_type>
inline void amp_mod_simd(float_type * out, const float_type * in1, const float_type * in2,
                         const float_type amount, unsigned int n)
{
    for (unsigned int i = 0; i != n/4; ++i) {
        out[0] = in1[0] * (float_type(1) + in2[0] * amount);
        out[1] = in1[1] * (float_type(1) + in2[1] * amount);
        out[2] = in1[2] * (float_type(1) + in2[2] * amount);
        out[3] = in1[3] * (float_type(1) + in2[3] * amount);

        out += 4;
        in1 += 4;
        in2 += 4;
    }
}

template <typename float_type>
inline void amp_mod_simd(float_type * out, const float_type * in1, const float_type * in2,
                          float_type amount, const float_type amount_slope,
                          unsigned int n)
{
    amp_mod(out, in1, in2, amount, amount_slope, n);
}

template <typename float_type>
inline void amp_mod_simd(float_type * out, const float_type * in1, const float_type * in2,
                         const float_type * amount, unsigned int n)
{
    for (unsigned int i = 0; i != n/4; ++i) {
        out[0] = in1[0] * (float_type(1) + in2[0] * amount[0]);
        out[1] = in1[1] * (float_type(1) + in2[1] * amount[1]);
        out[2] = in1[2] * (float_type(1) + in2[2] * amount[2]);
        out[3] = in1[3] * (float_type(1) + in2[3] * amount[3]);

        out += 4;
        in1 += 4;
        in2 += 4;
        amount += 4;
    }
}


} /* namespace nova */

#endif /* SIMD_AMPMOD_GENERIC_HPP */
