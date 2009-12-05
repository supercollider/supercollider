//  templated arithmetic simd functions
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

#ifndef SIMD_TERNARY_ARITHMETIC_FALLBACKS_FLOAT_HPP
#define SIMD_TERNARY_ARITHMETIC_FALLBACKS_FLOAT_HPP


namespace nova {

template <unsigned int n>
inline void clip_vec_simd(float * out, const float * arg1, const float * arg2, const float * arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_mp(float * out, const float * arg1, const float * arg2, const float * arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd(float * out, const float * arg1, const float * arg2, const float arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_mp(float * out, const float * arg1, const float * arg2, const float arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r3(float * out, const float * arg1, const float * arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void clip_vec_simd_r3_mp(float * out, const float * arg1, const float * arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void clip_vec_simd(float * out, const float * arg1, const float arg2, const float * arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_mp(float * out, const float * arg1, const float arg2, const float * arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd(float * out, const float * arg1, const float arg2, const float arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_mp(float * out, const float * arg1, const float arg2, const float arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r3(float * out, const float * arg1, const float arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void clip_vec_simd_r3_mp(float * out, const float * arg1, const float arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void clip_vec_simd_r2(float * out, const float * arg1, float arg2, const float arg2_slope, const float * arg3)
{
    clip_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_r2_mp(float * out, const float * arg1, float arg2, const float arg2_slope, const float * arg3)
{
    clip_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r2(float * out, const float * arg1, float arg2, const float arg2_slope, const float arg3)
{
    clip_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_r2_mp(float * out, const float * arg1, float arg2, const float arg2_slope, const float arg3)
{
    clip_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r2r3(float * out, const float * arg1, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    clip_vec_simd_r2r3(out, arg1, arg2, arg2_slope, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void clip_vec_simd_r2r3_mp(float * out, const float * arg1, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    clip_vec_simd_r2r3(out, arg1, arg2, arg2_slope, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void clip_vec_simd(float * out, const float arg1, const float * arg2, const float * arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_mp(float * out, const float arg1, const float * arg2, const float * arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd(float * out, const float arg1, const float * arg2, const float arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_mp(float * out, const float arg1, const float * arg2, const float arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r3(float * out, const float arg1, const float * arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void clip_vec_simd_r3_mp(float * out, const float arg1, const float * arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void clip_vec_simd(float * out, const float arg1, const float arg2, const float * arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_mp(float * out, const float arg1, const float arg2, const float * arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd(float * out, const float arg1, const float arg2, const float arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_mp(float * out, const float arg1, const float arg2, const float arg3)
{
    clip_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r3(float * out, const float arg1, const float arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void clip_vec_simd_r3_mp(float * out, const float arg1, const float arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void clip_vec_simd_r2(float * out, const float arg1, float arg2, const float arg2_slope, const float * arg3)
{
    clip_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_r2_mp(float * out, const float arg1, float arg2, const float arg2_slope, const float * arg3)
{
    clip_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r2(float * out, const float arg1, float arg2, const float arg2_slope, const float arg3)
{
    clip_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_r2_mp(float * out, const float arg1, float arg2, const float arg2_slope, const float arg3)
{
    clip_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r2r3(float * out, const float arg1, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    clip_vec_simd_r2r3(out, arg1, arg2, arg2_slope, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void clip_vec_simd_r2r3_mp(float * out, const float arg1, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    clip_vec_simd_r2r3(out, arg1, arg2, arg2_slope, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void clip_vec_simd_r1(float * out, float arg1, const float arg1_slope, const float * arg2, const float * arg3)
{
    clip_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_r1_mp(float * out, float arg1, const float arg1_slope, const float * arg2, const float * arg3)
{
    clip_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r1(float * out, float arg1, const float arg1_slope, const float * arg2, const float arg3)
{
    clip_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_r1_mp(float * out, float arg1, const float arg1_slope, const float * arg2, const float arg3)
{
    clip_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r1r3(float * out, float arg1, const float arg1_slope, const float * arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r1r3(out, arg1, arg1_slope, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void clip_vec_simd_r1r3_mp(float * out, float arg1, const float arg1_slope, const float * arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r1r3(out, arg1, arg1_slope, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void clip_vec_simd_r1(float * out, float arg1, const float arg1_slope, const float arg2, const float * arg3)
{
    clip_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_r1_mp(float * out, float arg1, const float arg1_slope, const float arg2, const float * arg3)
{
    clip_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r1(float * out, float arg1, const float arg1_slope, const float arg2, const float arg3)
{
    clip_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_r1_mp(float * out, float arg1, const float arg1_slope, const float arg2, const float arg3)
{
    clip_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r1r3(float * out, float arg1, const float arg1_slope, const float arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r1r3(out, arg1, arg1_slope, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void clip_vec_simd_r1r3_mp(float * out, float arg1, const float arg1_slope, const float arg2, float arg3, const float arg3_slope)
{
    clip_vec_simd_r1r3(out, arg1, arg1_slope, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void clip_vec_simd_r1r2(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, const float * arg3)
{
    clip_vec_simd_r1r2(out, arg1, arg1_slope, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_r1r2_mp(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, const float * arg3)
{
    clip_vec_simd_r1r2(out, arg1, arg1_slope, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r1r2(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, const float arg3)
{
    clip_vec_simd_r1r2(out, arg1, arg1_slope, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void clip_vec_simd_r1r2_mp(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, const float arg3)
{
    clip_vec_simd_r1r2(out, arg1, arg1_slope, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void clip_vec_simd_r1r2r3(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    clip_vec_simd_r1r2r3(out, arg1, arg1_slope, arg2, arg2_slope, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void clip_vec_simd_r1r2r3_mp(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    clip_vec_simd_r1r2r3(out, arg1, arg1_slope, arg2, arg2_slope, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void muladd_vec_simd(float * out, const float * arg1, const float * arg2, const float * arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_mp(float * out, const float * arg1, const float * arg2, const float * arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd(float * out, const float * arg1, const float * arg2, const float arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_mp(float * out, const float * arg1, const float * arg2, const float arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r3(float * out, const float * arg1, const float * arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r3_mp(float * out, const float * arg1, const float * arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void muladd_vec_simd(float * out, const float * arg1, const float arg2, const float * arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_mp(float * out, const float * arg1, const float arg2, const float * arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd(float * out, const float * arg1, const float arg2, const float arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_mp(float * out, const float * arg1, const float arg2, const float arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r3(float * out, const float * arg1, const float arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r3_mp(float * out, const float * arg1, const float arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r2(float * out, const float * arg1, float arg2, const float arg2_slope, const float * arg3)
{
    muladd_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r2_mp(float * out, const float * arg1, float arg2, const float arg2_slope, const float * arg3)
{
    muladd_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r2(float * out, const float * arg1, float arg2, const float arg2_slope, const float arg3)
{
    muladd_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r2_mp(float * out, const float * arg1, float arg2, const float arg2_slope, const float arg3)
{
    muladd_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r2r3(float * out, const float * arg1, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r2r3(out, arg1, arg2, arg2_slope, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r2r3_mp(float * out, const float * arg1, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r2r3(out, arg1, arg2, arg2_slope, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void muladd_vec_simd(float * out, const float arg1, const float * arg2, const float * arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_mp(float * out, const float arg1, const float * arg2, const float * arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd(float * out, const float arg1, const float * arg2, const float arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_mp(float * out, const float arg1, const float * arg2, const float arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r3(float * out, const float arg1, const float * arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r3_mp(float * out, const float arg1, const float * arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void muladd_vec_simd(float * out, const float arg1, const float arg2, const float * arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_mp(float * out, const float arg1, const float arg2, const float * arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd(float * out, const float arg1, const float arg2, const float arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_mp(float * out, const float arg1, const float arg2, const float arg3)
{
    muladd_vec_simd(out, arg1, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r3(float * out, const float arg1, const float arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r3_mp(float * out, const float arg1, const float arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r3(out, arg1, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r2(float * out, const float arg1, float arg2, const float arg2_slope, const float * arg3)
{
    muladd_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r2_mp(float * out, const float arg1, float arg2, const float arg2_slope, const float * arg3)
{
    muladd_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r2(float * out, const float arg1, float arg2, const float arg2_slope, const float arg3)
{
    muladd_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r2_mp(float * out, const float arg1, float arg2, const float arg2_slope, const float arg3)
{
    muladd_vec_simd_r2(out, arg1, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r2r3(float * out, const float arg1, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r2r3(out, arg1, arg2, arg2_slope, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r2r3_mp(float * out, const float arg1, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r2r3(out, arg1, arg2, arg2_slope, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r1(float * out, float arg1, const float arg1_slope, const float * arg2, const float * arg3)
{
    muladd_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r1_mp(float * out, float arg1, const float arg1_slope, const float * arg2, const float * arg3)
{
    muladd_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r1(float * out, float arg1, const float arg1_slope, const float * arg2, const float arg3)
{
    muladd_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r1_mp(float * out, float arg1, const float arg1_slope, const float * arg2, const float arg3)
{
    muladd_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r1r3(float * out, float arg1, const float arg1_slope, const float * arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r1r3(out, arg1, arg1_slope, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r1r3_mp(float * out, float arg1, const float arg1_slope, const float * arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r1r3(out, arg1, arg1_slope, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r1(float * out, float arg1, const float arg1_slope, const float arg2, const float * arg3)
{
    muladd_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r1_mp(float * out, float arg1, const float arg1_slope, const float arg2, const float * arg3)
{
    muladd_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r1(float * out, float arg1, const float arg1_slope, const float arg2, const float arg3)
{
    muladd_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r1_mp(float * out, float arg1, const float arg1_slope, const float arg2, const float arg3)
{
    muladd_vec_simd_r1(out, arg1, arg1_slope, arg2, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r1r3(float * out, float arg1, const float arg1_slope, const float arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r1r3(out, arg1, arg1_slope, arg2, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r1r3_mp(float * out, float arg1, const float arg1_slope, const float arg2, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r1r3(out, arg1, arg1_slope, arg2, arg3, arg3_slope, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r1r2(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, const float * arg3)
{
    muladd_vec_simd_r1r2(out, arg1, arg1_slope, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r1r2_mp(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, const float * arg3)
{
    muladd_vec_simd_r1r2(out, arg1, arg1_slope, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r1r2(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, const float arg3)
{
    muladd_vec_simd_r1r2(out, arg1, arg1_slope, arg2, arg2_slope, arg3, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r1r2_mp(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, const float arg3)
{
    muladd_vec_simd_r1r2(out, arg1, arg1_slope, arg2, arg2_slope, arg3, n);
}


template <unsigned int n>
inline void muladd_vec_simd_r1r2r3(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r1r2r3(out, arg1, arg1_slope, arg2, arg2_slope, arg3, arg3_slope, n);
}

template <unsigned int n>
inline void muladd_vec_simd_r1r2r3_mp(float * out, float arg1, const float arg1_slope, float arg2, const float arg2_slope, float arg3, const float arg3_slope)
{
    muladd_vec_simd_r1r2r3(out, arg1, arg1_slope, arg2, arg2_slope, arg3, arg3_slope, n);
}


} /* namespace nova */

#endif /* SIMD_TERNARY_ARITHMETIC_FALLBACKS_FLOAT_HPP */
