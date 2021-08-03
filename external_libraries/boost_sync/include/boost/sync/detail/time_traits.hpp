/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2012-2013 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   time_traits.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_TIME_TRAITS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_TIME_TRAITS_HPP_INCLUDED_

#include <boost/mpl/bool.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

struct time_point_tag {};
struct time_duration_tag {};

template< typename T, typename Void = void >
struct time_traits
{
    static BOOST_CONSTEXPR_OR_CONST bool is_specialized = false;
};

template< typename T, typename Tag, typename R >
struct enable_if_tag
{
};

template< typename T, typename R >
struct enable_if_tag< T, typename time_traits< T >::tag, R >
{
    typedef R type;
};

template< typename T, typename TagT >
struct is_time_tag_of : mpl::false_ {};

template< typename T >
struct is_time_tag_of< T, typename time_traits< T >::tag > : mpl::true_ {};

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_TIME_TRAITS_HPP_INCLUDED_
