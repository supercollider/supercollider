//  unroll helpers
//  Copyright (C) 2010 Tim Blechmann
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

#ifndef NOVA_SIMD_DETAIL_UNROLL_HELPERS_HPP
#define NOVA_SIMD_DETAIL_UNROLL_HELPERS_HPP

#include "../vec.hpp"

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif


/* this needs to be a macro, since friend functions are not pulled into scope */
#define NOVA_SIMD_DEFINE_UNROLLER(NAME, FUNCTION)                       \
namespace detail {                                                      \
template <typename FloatType, unsigned int n>                           \
struct NAME##_unroller                                                  \
{                                                                       \
    typedef vec<FloatType> vec_type;                                    \
                                                                        \
    static const int offset = vec_type::size;                           \
                                                                        \
    template <typename arg1_type, typename arg2_type>                   \
    static always_inline void mp_iteration(FloatType * out, arg1_type & in1, arg2_type & in2) \
    {                                                                   \
        vec_type result = FUNCTION(in1.get(), in2.get());               \
        result.store_aligned(out);                                      \
        in1.increment(); in2.increment();                               \
        NAME##_unroller<FloatType, n-offset>::mp_iteration(out+offset, in1, in2); \
    }                                                                   \
                                                                        \
    template <typename arg1_type, typename arg2_type, typename arg3_type> \
    static always_inline void mp_iteration(FloatType * out, arg1_type & in1, arg2_type & in2, arg2_type & in3) \
    {                                                                   \
        vec_type result = FUNCTION(in1.get(), in2.get(), in2.get());    \
        result.store_aligned(out);                                      \
        in1.increment(); in2.increment(); in3.increment();              \
        NAME##_unroller<FloatType, n-offset>::mp_iteration(out+offset, in1, in2, in3); \
    }                                                                   \
                                                                        \
};                                                                      \
                                                                        \
template <typename FloatType>                                           \
struct NAME##_unroller<FloatType, 0>                                    \
{                                                                       \
    template <typename Arg1, typename Arg2>                             \
    static always_inline void mp_iteration(FloatType * out, Arg1 const &, Arg2 const &) \
    {}                                                                  \
                                                                        \
    template <typename Arg1, typename Arg2, typename Arg3>              \
    static always_inline void mp_iteration(FloatType * out, Arg1 const &, Arg2 const &, Arg3 const &) \
    {}                                                                  \
};                                                                      \
}

namespace nova {
namespace detail {

template <typename FloatType,
          int N
         >
struct compile_time_unroller
{
    typedef vec<FloatType> vec_type;

    static const int offset = vec_type::size;

    template <typename arg1_type,
              typename Functor
             >
    static always_inline void mp_iteration(FloatType * out, arg1_type & in1, Functor const & f)
    {
        vec_type result = f(in1.get());
        result.store_aligned(out);
        in1.increment();
        compile_time_unroller<FloatType, N-offset>::mp_iteration(out+offset, in1, f);
    }

    template <typename arg1_type,
              typename arg2_type,
              typename Functor
             >
    static always_inline void mp_iteration(FloatType * out, arg1_type & in1, arg2_type & in2, Functor const & f)
    {
        vec_type result = f(in1.get(), in2.get());
        result.store_aligned(out);
        in1.increment(); in2.increment();
        compile_time_unroller<FloatType, N-offset>::mp_iteration(out+offset, in1, in2, f);
    }

    template <typename arg1_type,
              typename arg2_type,
              typename arg3_type,
              typename Functor
             >
    static always_inline void mp_iteration(FloatType * out, arg1_type & in1, arg2_type & in2,
                                           arg3_type & in3, Functor const & f)
    {
        vec_type result = f(in1.get(), in2.get(), in3.get());
        result.store_aligned(out);
        in1.increment(); in2.increment(); in3.increment();
        compile_time_unroller<FloatType, N-offset>::mp_iteration(out+offset, in1, in2, in3, f);
    }
};

template <typename FloatType>
struct compile_time_unroller<FloatType, 0>
{
    template <typename Arg1,
              typename Functor
             >
    static always_inline void mp_iteration(FloatType * out, Arg1 const &, Functor const & f)
    {}

    template <typename Arg1,
              typename Arg2,
              typename Functor
             >
    static always_inline void mp_iteration(FloatType * out, Arg1 const &, Arg2 const &, Functor const & f)
    {}

    template <typename Arg1,
              typename Arg2,
              typename Arg3,
              typename Functor
             >
    static always_inline void mp_iteration(FloatType * out, Arg1 const &, Arg2 const &, Arg3 const &, Functor const & f)
    {}
};


template <typename float_type,
          typename Arg1,
          typename Functor
         >
always_inline void generate_simd_loop(float_type * out, Arg1 arg1, unsigned int n, Functor const & f)
{
    const unsigned int per_loop = vec<float_type>::objects_per_cacheline;
    n /= per_loop;
    do {
        detail::compile_time_unroller<float_type, per_loop>::mp_iteration(out, arg1, f);
        out += per_loop;
    } while (--n);
}

template <typename float_type,
          typename Arg1,
          typename Arg2,
          typename Functor
         >
always_inline void generate_simd_loop(float_type * out, Arg1 arg1, Arg2 arg2, unsigned int n, Functor const & f)
{
    const unsigned int per_loop = vec<float_type>::objects_per_cacheline;
    n /= per_loop;
    do {
        detail::compile_time_unroller<float_type, per_loop>::mp_iteration(out, arg1, arg2, f);
        out += per_loop;
    } while (--n);
}

template <typename float_type,
          typename Arg1,
          typename Arg2,
          typename Arg3,
          typename Functor
         >
always_inline void generate_simd_loop(float_type * out, Arg1 arg1, Arg2 arg2, Arg3 arg3, unsigned int n, Functor const & f)
{
    const unsigned int per_loop = vec<float_type>::objects_per_cacheline;
    n /= per_loop;
    do {
        detail::compile_time_unroller<float_type, per_loop>::mp_iteration(out, arg1, arg2, arg3, f);
        out += per_loop;
    } while (--n);
}


}
}

#undef always_inline

#endif /* NOVA_SIMD_DETAIL_UNROLL_HELPERS_HPP */
