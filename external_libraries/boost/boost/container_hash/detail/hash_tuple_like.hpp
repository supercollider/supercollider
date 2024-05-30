// Copyright 2005-2009 Daniel James.
// Copyright 2021 Peter Dimov.
// Distributed under the Boost Software License, Version 1.0.
// https://www.boost.org/LICENSE_1_0.txt

#ifndef BOOST_HASH_DETAIL_HASH_TUPLE_LIKE_HPP
#define BOOST_HASH_DETAIL_HASH_TUPLE_LIKE_HPP

#include <boost/container_hash/hash_fwd.hpp>
#include <boost/container_hash/is_tuple_like.hpp>
#include <boost/container_hash/is_range.hpp>
#include <boost/type_traits/enable_if.hpp>
#include <boost/config.hpp>
#include <boost/config/workaround.hpp>

#if defined(BOOST_NO_CXX11_HDR_TUPLE)

// no support for tuple-likes

#else

#include <tuple>

namespace boost
{
namespace hash_detail
{

template <std::size_t I, typename T>
inline
typename boost::enable_if_<(I == std::tuple_size<T>::value), void>::type
    hash_combine_tuple_like( std::size_t&, T const& )
{
}

template <std::size_t I, typename T>
inline
typename boost::enable_if_<(I < std::tuple_size<T>::value), void>::type
    hash_combine_tuple_like( std::size_t& seed, T const& v )
{
    using std::get;
    boost::hash_combine( seed, get<I>( v ) );

    boost::hash_detail::hash_combine_tuple_like<I + 1>( seed, v );
}

template <typename T>
inline std::size_t hash_tuple_like( T const& v )
{
    std::size_t seed = 0;

    boost::hash_detail::hash_combine_tuple_like<0>( seed, v );

    return seed;
}

} // namespace hash_detail

#if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

#if !BOOST_WORKAROUND(BOOST_MSVC, <= 1800)

template <class T>
inline
typename boost::enable_if_<
    container_hash::is_tuple_like<T>::value && !container_hash::is_range<T>::value,
std::size_t>::type
    hash_value( T const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

#else

template <typename... T>
inline std::size_t hash_value( std::tuple<T...> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

#endif

#else

inline std::size_t hash_value( std::tuple<> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

template<typename A0>
inline std::size_t hash_value( std::tuple<A0> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

template<typename A0, typename A1>
inline std::size_t hash_value( std::tuple<A0, A1> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

template<typename A0, typename A1, typename A2>
inline std::size_t hash_value( std::tuple<A0, A1, A2> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

template<typename A0, typename A1, typename A2, typename A3>
inline std::size_t hash_value( std::tuple<A0, A1, A2, A3> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

template<typename A0, typename A1, typename A2, typename A3, typename A4>
inline std::size_t hash_value( std::tuple<A0, A1, A2, A3, A4> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
inline std::size_t hash_value( std::tuple<A0, A1, A2, A3, A4, A5> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6>
inline std::size_t hash_value( std::tuple<A0, A1, A2, A3, A4, A5, A6> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7>
inline std::size_t hash_value( std::tuple<A0, A1, A2, A3, A4, A5, A6, A7> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8>
inline std::size_t hash_value( std::tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, typename A6, typename A7, typename A8, typename A9>
inline std::size_t hash_value( std::tuple<A0, A1, A2, A3, A4, A5, A6, A7, A8, A9> const& v )
{
    return boost::hash_detail::hash_tuple_like( v );
}

#endif // #if !defined(BOOST_NO_CXX11_VARIADIC_TEMPLATES)

} // namespace boost

#endif // #if defined(BOOST_NO_CXX11_HDR_TUPLE)

#endif // #ifndef BOOST_HASH_DETAIL_HASH_TUPLE_LIKE_HPP
