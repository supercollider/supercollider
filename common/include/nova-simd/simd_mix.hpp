//  simd functions for mixing
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


#ifndef SIMD_MIX_HPP
#define SIMD_MIX_HPP

#include "vec.hpp"

namespace nova
{

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif

namespace detail
{

template <typename F, unsigned int n>
struct mix_vec
{
    static const int offset = vec<F>::size;

    static always_inline void mp_iteration(F * out, const F * in0, vec<F> const & factor0,
                                          const F * in1, vec<F> const & factor1)
    {
        vec<F> vin0, vin1, vout;
        vin0.load_aligned(in0);
        vin1.load_aligned(in1);
        vout = vin0 * factor0 + vin1 * factor1;
        vout.store_aligned(out);
        mix_vec<F, n-offset>::mp_iteration(out+offset, in0+offset, factor0, in1+offset, factor1);
    }

    static always_inline void mp_iteration(F * out, const F * in0, vec<F> & factor0, vec<F> const & slope0,
                                          const F * in1, vec<F> & factor1, vec<F> const & slope1)
    {
        vec<F> vin0, vin1, vout;
        vin0.load_aligned(in0);
        vin1.load_aligned(in1);
        vout = vin0 * factor0 + vin1 * factor1;
        vout.store_aligned(out);
        factor0 += slope0;
        factor1 += slope1;
        mix_vec<F, n-offset>::mp_iteration(out+offset, in0+offset, factor0, slope0, in1+offset, factor1, slope1);
    }

};

template <typename F>
struct mix_vec<F, 0>
{
    static always_inline void mp_iteration(F * out, const F * in0, vec<F> const & factor0,
                                          const F * in1, vec<F> const & factor1)
    {}

    static always_inline void mp_iteration(F * out, const F * in0, vec<F> & factor0, vec<F> const & slope0,
                                          const F * in1, vec<F> & factor1, vec<F> const & slope1)
    {}
};

}

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
    vec<F> vf0(factor0), vf1(factor1);
    const int per_loop = vec<F>::objects_per_cacheline;

    n /= per_loop;
    do {
        detail::mix_vec<F, per_loop>::mp_iteration(out, in0, vf0, in1, vf1);
        out += per_loop; in0 += per_loop; in1 += per_loop;
    } while(--n);
}

template <unsigned int n, typename F>
void mix_vec_simd(F * out, const F * in0, F factor0, const F * in1, F factor1)
{
    vec<F> vf0(factor0), vf1(factor1);
    detail::mix_vec<F, n>::mp_iteration(out, in0, vf0, in1, vf1);
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
    vec<F> vf0, vf1;
    const int per_loop = vec<F>::objects_per_cacheline;

    F vslope0_offset = vf0.set_slope(factor0, slope0);
    F vslope1_offset = vf1.set_slope(factor1, slope1);

    vec<F> vslope0, vslope1;
    vslope0.set_vec(vslope0_offset);
    vslope1.set_vec(vslope1_offset);

    n /= per_loop;
    do {
        detail::mix_vec<F, per_loop>::mp_iteration(out, in0, vf0, vslope0, in1, vf1, vslope1);
        out += per_loop; in0 += per_loop; in1 += per_loop;
    } while(--n);
}

template <unsigned int n, typename F>
void mix_vec_simd(F * out, const F * in0, F factor0, F slope0,
                  const F * in1, F factor1, F slope1)
{
    vec<F> vf0, vf1;

    vf0.set_slope(factor0, slope0);
    vf1.set_slope(factor1, slope1);

    vec<F> vslope0, vslope1;
    vslope0.set_vec(slope0*vec<F>::size);
    vslope1.set_vec(slope1*vec<F>::size);

    detail::mix_vec<F, n>::mp_iteration(out, in0, vf0, vslope0, in1, vf1, vslope1);
}

}

#undef always_inline

#endif /* SIMD_MIX_HPP */
