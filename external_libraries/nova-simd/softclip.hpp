//  softclip
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

#ifndef SIMD_SOFTCLIP_HPP
#define SIMD_SOFTCLIP_HPP

#include <cassert>
#include <cmath>

#include "vec.hpp"

#include "detail/unroll_helpers.hpp"

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif


namespace nova
{
namespace detail
{

template <typename FloatType>
struct softclip
{
    always_inline FloatType operator()(FloatType arg) const
    {
        FloatType abs = fabs(arg);
        if (abs < FloatType(0.5))
            return arg;
        else
            return (abs - FloatType(0.25)) / arg;
    }
};

#ifdef SSE

/* this computes both parts of the branch
 *
 * benchmarks (core2) showed:
 * 6.5 seconds for simdfied code
 * 5.3 seconds for non-simd code for samples with abs(sample) < 0.5
 * 17 seconds                    when 50% of the samples have abs(sample) < 0.5
 * 26 seconds                    for samples with abs(sample) > 0.5
 *
 * on intel nethalem cpus, the simdfied code is faster than all non-simd code
 * */

template <>
struct softclip<vec<float> >
{
    typedef vec<float> vec_type;

    always_inline vec_type operator()(vec_type arg) const
    {
        const vec_type const05  = vec_type::gen_05();
        const vec_type const025 = vec_type::gen_025();

        vec_type abs = abs(arg);
        vec_type selecter = mask_lt(abs, const05);
        vec_type alt_ret = (abs - const025) / arg;

        return select(alt_ret, arg, selecter);
    }
};

#endif

}


template <typename float_type>
inline void softclip_vec(float_type * out, const float_type * in, unsigned int n)
{
    detail::apply_on_vector(out, in, n, detail::softclip<float>());
}

template <typename float_type>
inline void softclip_vec_simd(float_type * out, const float_type * in, unsigned int n)
{
    softclip_vec(out, in, n);
}

template <unsigned int n, typename float_type>
inline void softclip_vec_simd(float_type * out, const float_type * in)
{
    softclip_vec_simd(out, in, n);
}

#ifdef SSE

template <>
inline void softclip_vec_simd<float>(float_type * out, const float_type * in, unsigned int n)
{
    detail::generate_simd_loop(out, in, n, detail::softclip<vec<float> >());
}

#endif

} /* namespace nova */

#undef always_inline

#endif /* SIMD_SOFTCLIP_HPP */
