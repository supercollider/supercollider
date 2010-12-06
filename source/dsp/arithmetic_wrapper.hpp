//  $Id$
//
//  wrapper classes for arithmetic functions
//  Copyright (C) 2005, 2006, 2007 Tim Blechmann
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

//  $Revision$
//  $LastChangedRevision$
//  $LastChangedDate$
//  $LastChangedBy$

#ifndef ARITHMETIC_WRAPPER_HPP
#define ARITHMETIC_WRAPPER_HPP

#include "arithmetic.hpp"
#ifdef __SSE__
#include "simd_sse.hpp"
#include "simd_sse_mp.hpp"
#else /* __SSE__ */
#include "simd_fallbacks.hpp"
#endif /* __SSE__ */

#include "simd_static.hpp"

#include "../utilities/force_inline.hpp"

namespace nova
{
#define BINARY_SIMD_WRAPPER(NAME, PREFIX)                                                                       \
    template <int n>                                                                                            \
    struct NAME                                                                                                 \
    {                                                                                                           \
        static always_inline void vec(float * dest, const float * src1, const float * src2, uint blocksize)     \
        {                                                                                                       \
            PREFIX##_vec(dest, src1, src2, blocksize);                                                          \
        }                                                                                                       \
                                                                                                                \
        static always_inline void vec_8(float * dest, const float * src1, const float * src2, uint blocksize)   \
        {                                                                                                       \
            PREFIX##_vec_simd(dest, src1, src2, blocksize);                                                     \
        }                                                                                                       \
                                                                                                                \
        static always_inline void vec(float * dest, const float * src1, const float * src2)                     \
        {                                                                                                       \
            PREFIX##_vec_simd_mp<n>(dest, src1, src2);                                                          \
        }                                                                                                       \
                                                                                                                \
        static always_inline void scalar(float * dest, const float * src1, float src2, uint blocksize)          \
        {                                                                                                       \
            PREFIX##_vec(dest, src1, src2, blocksize);                                                          \
        }                                                                                                       \
                                                                                                                \
        static always_inline void scalar_8(float * dest, const float * src1, float src2, uint blocksize)        \
        {                                                                                                       \
            PREFIX##_vec_simd(dest, src1, src2, blocksize);                                                     \
        }                                                                                                       \
                                                                                                                \
        static always_inline void scalar(float * dest, const float * src1, float src2)                          \
        {                                                                                                       \
            PREFIX##_vec_simd_mp<n>(dest, src1, src2);                                                          \
        }                                                                                                       \
    }

    BINARY_SIMD_WRAPPER(plus_wrapper, plus);
    BINARY_SIMD_WRAPPER(minus_wrapper, minus);
    BINARY_SIMD_WRAPPER(times_wrapper, times);
    BINARY_SIMD_WRAPPER(over_wrapper, over);

    BINARY_SIMD_WRAPPER(max_wrapper, max);
    BINARY_SIMD_WRAPPER(min_wrapper, min);

    BINARY_SIMD_WRAPPER(less_wrapper, less);
    BINARY_SIMD_WRAPPER(lesseq_wrapper, less_equal);
    BINARY_SIMD_WRAPPER(equal_wrapper, equal);
    BINARY_SIMD_WRAPPER(notequal_wrapper, notequal);
    BINARY_SIMD_WRAPPER(greater_wrapper, greater);
    BINARY_SIMD_WRAPPER(greatereq_wrapper, greater_equal);

    template <int n>
    struct minus_inverse_wrapper
    {
        static always_inline void vec(float * dest, const float * src1, const float * src2, uint blocksize)
        {
            minus_vec(dest, src2, src1, blocksize);
        }

        static always_inline void vec_8(float * dest, const float * src1, const float * src2, uint blocksize)
        {
            minus_vec_simd(dest, src2, src1, blocksize);
        }

        static always_inline void vec(float * dest, const float * src1, const float * src2)
        {
            minus_vec_simd_mp<n>(dest, src2, src1);
        }

        static always_inline void scalar(float * dest, const float * src1, float src2, uint blocksize)
        {
            minus_vec(dest, src2, src1, blocksize);
        }

        static always_inline void scalar_8(float * dest, const float * src1, float src2, uint blocksize)
        {
            minus_vec_simd(dest, src2, src1, blocksize);
        }

        static always_inline void scalar(float * dest, const float * src1, float src2)
        {
            minus_vec_simd_mp<n>(dest, src2, src1);
        }
    };



    template <int n>
    struct over_inverse_wrapper
    {
        static always_inline void vec(float * dest, const float * src1, const float * src2, uint blocksize)
        {
            over_vec(dest, src2, src1, blocksize);
        }

        static always_inline void vec_8(float * dest, const float * src1, const float * src2, uint blocksize)
        {
            over_vec_simd(dest, src2, src1, blocksize);
        }

        static always_inline void vec(float * dest, const float * src1, const float * src2)
        {
            over_vec_simd_mp<n>(dest, src2, src1);
        }

        static always_inline void scalar(float * dest, const float * src1, float src2, uint blocksize)
        {
            over_vec(dest, src2, src1, blocksize);
        }

        static always_inline void scalar_8(float * dest, const float * src1, float src2, uint blocksize)
        {
            over_vec_simd(dest, src2, src1, blocksize);
        }

        static always_inline void scalar(float * dest, const float * src1, float src2)
        {
            over_vec_simd_mp<n>(dest, src2, src1);
        }
    };

    struct zero_vec_wrapper
    {
        static always_inline void vec(float * dest, uint blocksize)
        {
            zerovec(dest, blocksize);
        }

        static always_inline void vec_8(float * dest, uint blocksize)
        {
            zerovec_simd(dest, blocksize);
        }

        template <int n>
        static always_inline void vec(float * dest)
        {
            zerovec_simd_mp<n>(dest);
        }
    };

    struct copy_vec_wrapper
    {
        static always_inline void vec(float * dest, const float * src, uint blocksize)
        {
            copyvec(dest, src, blocksize);
        }

        static always_inline void vec_8(float * dest, const float * src, uint blocksize)
        {
            copyvec_simd(dest, src, blocksize);
        }

        template <int n>
        static always_inline void vec(float * dest, const float * src)
        {
            copyvec_simd<n>(dest, src);
        }

        template <int n>
        static always_inline void vec_unaligned(float * dest, const float * src)
        {
            copyvec_nn_simd<n>(dest, src);
        }

        template <int n>
        static always_inline void vec_aligned_unaligned(float * dest, const float * src)
        {
            copyvec_an_simd<n>(dest, src);
        }

        template <int n>
        static always_inline void vec_unaligned_aligned(float * dest, const float * src)
        {
            copyvec_na_simd<n>(dest, src);
        }
    };

    struct add_vec_wrapper
    {
        static always_inline void vec(float * dest, const float * src, uint blocksize)
        {
            addvec(dest, src, blocksize);
        }

        static always_inline void vec_8(float * dest, const float * src, uint blocksize)
        {
            addvec_simd(dest, src, blocksize);
        }

        template <int n>
        static always_inline void vec(float * dest, const float * src)
        {
            addvec_simd_mp<n>(dest, src);
        }
    };


#define UNARY_SIMD_WRAPPER(NAME, PREFIX)                                                    \
    template <int n>                                                                        \
    struct NAME                                                                             \
    {                                                                                       \
        static always_inline void vec(float * dest, const float * src, uint blocksize)      \
        {                                                                                   \
            PREFIX##_vec(dest, src, blocksize);                                             \
        }                                                                                   \
                                                                                            \
        static always_inline void vec_8(float * dest, const float * src, uint blocksize)    \
        {                                                                                   \
            PREFIX##_vec_simd(dest, src, blocksize);                                        \
        }                                                                                   \
                                                                                            \
        static always_inline void vec(float * dest, const float * src)                      \
        {                                                                                   \
            PREFIX##_vec_simd_mp<n>(dest, src);                                             \
        }                                                                                   \
    }

    UNARY_SIMD_WRAPPER(sgn_wrapper, sgn);
    UNARY_SIMD_WRAPPER(abs_wrapper, abs);
}

#undef always_inline
#endif /* ARITHMETIC_WRAPPER_HPP */
