/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   weak_linkage.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_WEAK_LINKAGE_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_WEAK_LINKAGE_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

//! The template creates a module-wide global value
template< typename T, typename Tag = void >
struct weak_linkage
{
    static T value;
};

template< typename T, typename Tag >
T weak_linkage< T, Tag >::value = {};

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_WEAK_LINKAGE_HPP_INCLUDED_
