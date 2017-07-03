// semaphore_config.hpp
//
// Copyright (C) 2013 Tim Blechmann
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_SEMAPHORE_CONFIG_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_SEMAPHORE_CONFIG_HPP_INCLUDED_

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(BOOST_SYNC_DETAIL_PLATFORM_MACH)

// OSX
#if defined(__MAC_OS_X_VERSION_MIN_REQUIRED)

#if __MAC_OS_X_VERSION_MIN_REQUIRED >= __MAC_10_6
#define BOOST_SYNC_DETAIL_USE_DISPATCH_SEMAPHORES
#endif

#elif defined(__IPHONE_OS_VERSION_MIN_REQUIRED)

// untested!
#if __IPHONE_OS_VERSION_MIN_REQUIRED >= __IPHONE_4_0
#define BOOST_SYNC_DETAIL_USE_DISPATCH_SEMAPHORES
#endif

#endif

#endif // defined(BOOST_SYNC_DETAIL_PLATFORM_MACH)

#if !defined(BOOST_SYNC_DETAIL_USE_DISPATCH_SEMAPHORES) && defined(BOOST_HAS_UNISTD_H)
#include <unistd.h>

#if (_POSIX_SEMAPHORES - 0) >= 200112L
#define BOOST_SYNC_DETAIL_USE_POSIX_SEMAPHORES
#endif
#endif // !defined(BOOST_SYNC_DETAIL_USE_DISPATCH_SEMAPHORES) && defined(BOOST_HAS_UNISTD_H)

#endif // BOOST_SYNC_DETAIL_SEMAPHORE_CONFIG_HPP_INCLUDED_
