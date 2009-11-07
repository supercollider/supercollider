//  templated memory simd functions
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


#ifndef SIMD_MEMORY_GENERIC_HPP
#define SIMD_MEMORY_GENERIC_HPP

#include <cassert>
#include <cstring>

namespace nova {

template <typename F>
inline void zerovec(F * dest, unsigned int n)
{
    std::memset(dest, 0, n*sizeof(F));
}

template <typename F>
inline void zerovec_simd(F * dest, unsigned int n)
{
    std::memset(dest, 0, n*sizeof(F));
}

template <typename F>
inline void zerovec_na_simd(F * dest, unsigned int n)
{
    std::memset(dest, 0, n*sizeof(F));
}

template <typename F>
inline void setvec(F * dest, F f, uint n)
{
    assert(n);
    do
        *dest++ = f;
    while (--n);
}

template <typename F>
inline void setvec_simd(F * dest, F f, uint n)
{
    for (uint i = 0; i != n; i+=8)
    {
        dest[i+0] = dest[i+1] = dest[i+2] = dest[i+3] = dest[i+4] =
            dest[i+5] = dest[i+6] = dest[i+7] = f;
    }
}

template <typename F>
inline void set_slope_vec(F * dest, F f, F slope, uint n)
{
    assert(n);
    do
    {
        *dest++ = f; f += slope;
    }
    while (--n);
}

template <typename F>
inline void set_slope_vec_simd(F * dest, F f, F slope, uint n)
{
    for (uint i = 0; i != n; i+=8)
    {
        *dest++ = f; f += slope;
        *dest++ = f; f += slope;
        *dest++ = f; f += slope;
        *dest++ = f; f += slope;
        *dest++ = f; f += slope;
        *dest++ = f; f += slope;
        *dest++ = f; f += slope;
        *dest++ = f; f += slope;
    }
}

template <typename F>
inline void set_exp_vec(F * dest, F f, F curve, uint n)
{
    assert(n);
    do
    {
        *dest++ = f; f *= curve;
    }
    while (--n);
}

template <typename F>
inline void set_exp_vec_simd(F * dest, F f, F curve, uint n)
{
    for (uint i = 0; i != n; i+=8)
    {
        *dest++ = f; f *= curve;
        *dest++ = f; f *= curve;
        *dest++ = f; f *= curve;
        *dest++ = f; f *= curve;
        *dest++ = f; f *= curve;
        *dest++ = f; f *= curve;
        *dest++ = f; f *= curve;
        *dest++ = f; f *= curve;
    }
}


template <typename F>
inline void copyvec(F * dest, const F * src, uint n)
{
    std::memcpy(dest, src, n*sizeof(F));
}

template <typename F>
inline void copyvec_aa_simd(F * dest, const F * src, uint n)
{
    std::memcpy(dest, src, n*sizeof(F));
}

template <typename F>
inline void copyvec_simd(F * dest, const F * src, uint n)
{
    copyvec_aa_simd(dest, src, n);
}

template <typename F>
inline void copyvec_nn_simd(F * dest, const F * src, uint n)
{
    std::memcpy(dest, src, n*sizeof(F));
}

template <typename F>
inline void copyvec_an_simd(F * dest, const F * src, uint n)
{
    std::memcpy(dest, src, n*sizeof(F));
}

template <typename F>
inline void copyvec_na_simd(F * dest, const F * src, uint n)
{
    std::memcpy(dest, src, n*sizeof(F));
}

template <typename F>
inline void addvec(F * out, const F * in, unsigned int n)
{
    do
    {
        *out++ += *in++;
    }
    while (--n);
}

template <typename F>
inline void addvec(F * out, const F in, unsigned int n)
{
    do
    {
        *out++ += in;
    }
    while (--n);
}

template <typename F>
inline void addvec(F * out, const F in, const F slope, unsigned int n)
{
    do
    {
        *out++ += in; in += slope;
    }
    while (--n);
}

template <typename F>
inline void addvec_simd(F * out, const F * in, unsigned int n)
{
    n /= 8;
    do
    {
        F out0 = out[0] + in[0];
        F out1 = out[1] + in[1];
        F out2 = out[2] + in[2];
        F out3 = out[3] + in[3];
        F out4 = out[4] + in[4];
        F out5 = out[5] + in[5];
        F out6 = out[6] + in[6];
        F out7 = out[7] + in[7];

        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
        in += 8;
    }
    while (--n);
}

template <typename F>
inline void addvec_simd(F * out, const F in, unsigned int n)
{
    n /= 8;
    do
    {
        F out0 = out[0] + in;
        F out1 = out[1] + in;
        F out2 = out[2] + in;
        F out3 = out[3] + in;
        F out4 = out[4] + in;
        F out5 = out[5] + in;
        F out6 = out[6] + in;
        F out7 = out[7] + in;

        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
    }
    while (--n);
}

template <typename F>
inline void addvec_simd(F * out, const F in, const F slope, unsigned int n)
{
    n /= 8;
    do
    {
        F out0 = out[0] + in; in += slope;
        F out1 = out[1] + in; in += slope;
        F out2 = out[2] + in; in += slope;
        F out3 = out[3] + in; in += slope;
        F out4 = out[4] + in; in += slope;
        F out5 = out[5] + in; in += slope;
        F out6 = out[6] + in; in += slope;
        F out7 = out[7] + in; in += slope;

        out[0] = out0;
        out[1] = out1;
        out[2] = out2;
        out[3] = out3;
        out[4] = out4;
        out[5] = out5;
        out[6] = out6;
        out[7] = out7;

        out += 8;
    }
    while (--n);
}


} /* namespace nova */

#endif /* SIMD_MEMORY_GENERIC_HPP */
