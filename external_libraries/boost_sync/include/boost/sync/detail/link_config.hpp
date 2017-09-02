/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   link_config.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html. This header
 *         defines macros for linking with the library.
 */

#ifndef BOOST_SYNC_DETAIL_LINK_CONFIG_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_LINK_CONFIG_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if !defined(BOOST_SYNC_DYN_LINK) && defined(BOOST_ALL_DYN_LINK)
#define BOOST_SYNC_DYN_LINK
#endif

#if !defined(BOOST_SYNC_NO_LIB) && defined(BOOST_ALL_NO_LIB)
#define BOOST_SYNC_NO_LIB
#endif

#if !defined(BOOST_SYNC_BUILDING)
#   if defined(BOOST_SYNC_DYN_LINK)
#       define BOOST_SYNC_API BOOST_SYMBOL_IMPORT
#   endif // defined(BOOST_SYNC_DYN_LINK)
#   if !defined(BOOST_SYNC_NO_LIB)
#       define BOOST_LIB_NAME boost_sync
#       if defined(BOOST_SYNC_DYN_LINK)
#           define BOOST_DYN_LINK
#       endif
#       include <boost/config/auto_link.hpp>
#   endif // !defined(BOOST_SYNC_NO_LIB)
#else // !defined(BOOST_SYNC_BUILDING)
#   if defined(BOOST_SYNC_DYN_LINK)
#       define BOOST_SYNC_API BOOST_SYMBOL_EXPORT
#   endif
#endif // !defined(BOOST_SYNC_BUILDING)

#ifndef BOOST_SYNC_API
#define BOOST_SYNC_API
#endif

#endif // BOOST_SYNC_DETAIL_LINK_CONFIG_HPP_INCLUDED_
