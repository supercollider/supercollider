/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   config.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html. In this file
 *         internal configuration macros are defined.
 */

#ifndef BOOST_SYNC_DETAIL_CONFIG_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_CONFIG_HPP_INCLUDED_

#include <boost/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

// BOOST_SYNC_USE_PTHREAD is a user-configurable macro
#if defined(BOOST_SYNC_USE_PTHREAD)

#define BOOST_SYNC_DETAIL_PLATFORM_POSIX

#elif defined(BOOST_WINDOWS)

#include <boost/detail/winapi/config.hpp>
#define BOOST_SYNC_DETAIL_PLATFORM_WINAPI

#elif defined(__linux__) || defined(__linux) || defined(linux)

#define BOOST_SYNC_DETAIL_PLATFORM_LINUX

#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)

#include <Availability.h>
#define BOOST_SYNC_DETAIL_PLATFORM_MACH

#else

#define BOOST_SYNC_DETAIL_PLATFORM_POSIX

#endif

#if !defined(BOOST_SYNC_USE_COMPILER_TLS)
#if defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI) && BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6
// Compiler-based TLS are safe to use in DLLs only since Windows Vista
#define BOOST_SYNC_USE_COMPILER_TLS
#endif
#endif // !defined(BOOST_SYNC_USE_COMPILER_TLS)

#if defined(BOOST_SYNC_USE_COMPILER_TLS)
#if defined(__GNUC__) || defined(__SUNPRO_CC)
#define BOOST_SYNC_DETAIL_TLS __thread
#elif defined(BOOST_MSVC)
#define BOOST_SYNC_DETAIL_TLS __declspec(thread)
#else
#undef BOOST_SYNC_USE_COMPILER_TLS
#endif
#endif // defined(BOOST_SYNC_USE_COMPILER_TLS)

#if defined(BOOST_SYNC_DETAIL_PLATFORM_POSIX)
#define BOOST_SYNC_DETAIL_ABI_NAMESPACE posix
#elif defined(BOOST_SYNC_DETAIL_PLATFORM_LINUX)
#define BOOST_SYNC_DETAIL_ABI_NAMESPACE linux_
#elif defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI)
#if BOOST_USE_WINAPI_VERSION >= BOOST_WINAPI_VERSION_WIN6
#define BOOST_SYNC_DETAIL_ABI_NAMESPACE winnt6
#else
#define BOOST_SYNC_DETAIL_ABI_NAMESPACE winnt5
#endif
#elif defined(BOOST_SYNC_DETAIL_PLATFORM_MACH)
#define BOOST_SYNC_DETAIL_ABI_NAMESPACE mach
#else
#error Boost.Sync: Internal configuration error: unknown base threading API
#endif

#if !defined(BOOST_NO_CXX11_INLINE_NAMESPACES)
#define BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE inline namespace BOOST_SYNC_DETAIL_ABI_NAMESPACE
#else
#define BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE namespace BOOST_SYNC_DETAIL_ABI_NAMESPACE
namespace boost {
namespace sync {

// Emulate inline namespace with a using directive
BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {}

using namespace BOOST_SYNC_DETAIL_ABI_NAMESPACE
#if defined(__GNUC__) && (__GNUC__ >= 4 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 4)) && !defined(__clang__)
__attribute__((__strong__))
#endif
;

}
}
#endif // !defined(BOOST_NO_CXX11_INLINE_NAMESPACES)

#endif // BOOST_SYNC_DETAIL_CONFIG_HPP_INCLUDED_
