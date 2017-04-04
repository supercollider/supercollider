/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   system_error.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_SYSTEM_ERROR_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_SYSTEM_ERROR_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>
#if defined(BOOST_SYNC_USE_STD_SYSTEM_ERROR)
#include <system_error>
#else
#include <boost/system/error_code.hpp>
#include <boost/system/system_error.hpp>
#endif
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

#if defined(BOOST_SYNC_USE_STD_SYSTEM_ERROR)
namespace system_ns = std;
#else
namespace system_ns = boost::system;
#endif

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_SYSTEM_ERROR_HPP_INCLUDED_
