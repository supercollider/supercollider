//  helper macros for defining the external interface functions
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

#ifndef NOVA_SIMD_DETAIL_DEFINE_MACROS_HPP
#define NOVA_SIMD_DETAIL_DEFINE_MACROS_HPP

#include "wrap_argument_vector.hpp"
#include "unroll_helpers.hpp"

#if defined(__GNUC__) && defined(NDEBUG)
#define always_inline inline  __attribute__((always_inline))
#else
#define always_inline inline
#endif


#define NOVA_SIMD_DEFINE_UNARY_FUNCTIONS(NAME, FUNCTOR)                 \
                                                                        \
template <typename FloatType>                                           \
inline void NAME##_vec(FloatType * out, const FloatType * arg, unsigned int n) \
{                                                                       \
    nova::detail::apply_on_vector(out, arg, n, FUNCTOR<FloatType>());   \
}                                                                       \
                                                                        \
template <typename FloatType>                                           \
inline void NAME##_vec_simd(FloatType * out, const FloatType * arg, unsigned int n) \
{                                                                       \
    nova::detail::generate_simd_loop(out, nova::detail::wrap_vector_arg(wrap_argument(arg)), \
                                     n, FUNCTOR<vec<FloatType> >());    \
}                                                                       \
                                                                        \
template <unsigned int n, typename FloatType>                           \
inline void NAME##_vec_simd(FloatType * out, const FloatType * arg)     \
{                                                                       \
    nova::detail::vector_pointer_argument<FloatType> varg(arg);         \
    nova::detail::compile_time_unroller<FloatType, n>::mp_iteration(out, varg, FUNCTOR<vec<FloatType> >()); \
}

namespace nova {
namespace detail {

template <unsigned int n,
          typename FloatType,
          typename arg1_type,
          typename arg2_type,
          typename Functor>
always_inline void binary_vec_simd(FloatType * out, arg1_type arg1, arg2_type arg2, Functor const & f)
{
    detail::compile_time_unroller<FloatType, n>::mp_iteration(out, arg1, arg2, f);
}

}
}


#define NOVA_SIMD_DEFINE_BINARY_FUNCTIONS(NAME, FUNCTOR)                \
template <typename FloatType, typename arg1_type, typename arg2_type>   \
inline void NAME##_vec(FloatType * out, arg1_type arg1, arg2_type arg2, unsigned int n) \
{                                                                       \
    detail::apply_on_vector(out, wrap_argument(arg1), wrap_argument(arg2), \
                            n, FUNCTOR<FloatType>());                   \
}                                                                       \
                                                                        \
template <typename FloatType>                                           \
inline void NAME##_vec(FloatType * out, const FloatType * arg1, FloatType arg2, \
                      const FloatType arg2_slope, unsigned int n)       \
{                                                                       \
    detail::apply_on_vector(out, wrap_argument(arg1), wrap_argument(arg2, arg2_slope), \
                            n, FUNCTOR<FloatType>());                   \
}                                                                       \
                                                                        \
template <typename FloatType>                                           \
inline void NAME##_vec(FloatType * out, FloatType arg1, const FloatType arg1_slope, \
                      const FloatType * arg2, unsigned int n)           \
{                                                                       \
    detail::apply_on_vector(out, wrap_argument(arg1), wrap_argument(arg2), n, FUNCTOR<FloatType>()); \
}                                                                       \
                                                                        \
template <typename FloatType, typename Arg1Type, typename Arg2Type>     \
inline void NAME##_vec_simd(FloatType * out, Arg1Type arg1, Arg2Type arg2, unsigned int n) \
{                                                                       \
    detail::generate_simd_loop(out, detail::wrap_vector_arg(wrap_argument(arg1)), \
                               detail::wrap_vector_arg(wrap_argument(arg2)), \
                               n, FUNCTOR<vec<FloatType> >());          \
}                                                                       \
                                                                        \
template <typename FloatType>                                           \
inline void NAME##_vec_simd(FloatType * out, const FloatType * arg1, const FloatType arg2, \
                            const FloatType arg2_slope, unsigned int n) \
{                                                                       \
    detail::generate_simd_loop(out, detail::wrap_vector_arg(wrap_argument(arg1)), \
                               detail::wrap_vector_arg(wrap_argument(arg2, arg2_slope)), \
                               n, FUNCTOR<vec<FloatType> >());          \
}                                                                       \
                                                                        \
template <typename FloatType>                                           \
inline void NAME##_vec_simd(FloatType * out, const FloatType arg1, const FloatType arg1_slope, \
                            const FloatType * arg2, unsigned int n)     \
{                                                                       \
    detail::generate_simd_loop(out, detail::wrap_vector_arg(wrap_argument(arg1, arg1_slope)), \
                               detail::wrap_vector_arg(wrap_argument(arg2)), \
                               n, FUNCTOR<vec<FloatType> >());          \
}                                                                       \
                                                                        \
template <unsigned int n, typename FloatType, typename Arg1Type, typename Arg2Type> \
inline void NAME##_vec_simd(FloatType * out, Arg1Type arg1, Arg2Type arg2) \
{                                                                       \
    detail::binary_vec_simd<n>(out, detail::wrap_vector_arg(wrap_argument(arg1)), \
                               detail::wrap_vector_arg(wrap_argument(arg2)), \
                               FUNCTOR<vec<FloatType> >());             \
}                                                                       \
                                                                        \
template <unsigned int n, typename FloatType>                           \
inline void NAME##_vec_simd(FloatType * out, const FloatType * arg1, const FloatType arg2, \
                            const FloatType arg2_slope)                 \
{                                                                       \
    detail::binary_vec_simd<n>(out, detail::wrap_vector_arg(wrap_argument(arg1)), \
                               detail::wrap_vector_arg(wrap_argument(arg2, arg2_slope)), FUNCTOR<vec<FloatType> >()); \
}                                                                       \
                                                                        \
template <unsigned int n, typename FloatType>                           \
inline void NAME##_vec_simd(FloatType * out, const FloatType arg1, const FloatType arg1_slope, \
                            const FloatType * arg2)                     \
{                                                                       \
    detail::binary_vec_simd<n>(out, detail::wrap_vector_arg(wrap_argument(arg1, arg1_slope)), \
                               detail::wrap_vector_arg(wrap_argument(arg2)), FUNCTOR<vec<FloatType> >()); \
}



#define NOVA_SIMD_DEFINE_TERNARY_OPERATION(NAME, FUNCTOR)               \
template <typename FloatType,                                           \
          typename Arg1,                                                \
          typename Arg2,                                                \
          typename Arg3                                                 \
         >                                                              \
inline void NAME##_vec(FloatType * out, Arg1 arg1, Arg2 arg2, Arg3 arg3, unsigned int n) \
{                                                                       \
    nova::detail::apply_on_vector<FloatType>(out, arg1, arg2, arg3,     \
                                             n, FUNCTOR<FloatType>());  \
}                                                                       \
                                                                        \
template <typename FloatType,                                           \
          typename Arg1,                                                \
          typename Arg2,                                                \
          typename Arg3                                                 \
         >                                                              \
inline void NAME##_vec_simd(FloatType * out, Arg1 arg1, Arg2 arg2, Arg3 arg3, unsigned int n) \
{                                                                       \
    nova::detail::generate_simd_loop(out, detail::wrap_vector_arg(arg1), \
                                     detail::wrap_vector_arg(arg2),     \
                                     detail::wrap_vector_arg(arg3),     \
                                     n, FUNCTOR<vec<FloatType> >());    \
}                                                                       \
                                                                        \
namespace detail {                                                      \
template <int N,                                                        \
          typename FloatType,                                           \
          typename Arg1,                                                \
          typename Arg2,                                                \
          typename Arg3                                                 \
         >                                                              \
inline void NAME##_vec_simd_perform(FloatType * out, Arg1 arg1, Arg2 arg2, Arg3 arg3) \
{                                                                       \
    nova::detail::compile_time_unroller<FloatType, N>::mp_iteration(out, arg1, arg2, arg3, \
                                                                    FUNCTOR<vec<FloatType> >()); \
}                                                                       \
                                                                        \
}                                                                       \
                                                                        \
template <int N,                                                        \
          typename FloatType,                                           \
          typename Arg1,                                                \
          typename Arg2,                                                \
          typename Arg3                                                 \
         >                                                              \
inline void NAME##_vec_simd(FloatType * out, Arg1 arg1, Arg2 arg2, Arg3 arg3) \
{                                                                       \
    nova::detail::NAME##_vec_simd_perform<N>(out, detail::wrap_vector_arg(arg1), \
                                             detail::wrap_vector_arg(arg2), \
                                             detail::wrap_vector_arg(arg3)); \
}                                                                       \

#undef always_inline

#endif /* NOVA_SIMD_DETAIL_DEFINE_MACROS_HPP */
