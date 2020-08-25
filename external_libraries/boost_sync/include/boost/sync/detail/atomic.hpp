// atomic.hpp: pull in std::atomic or boost::atomic
//
// Copyright (C) 2013 Tim Blechmann
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_ATOMIC_HPP
#define BOOST_SYNC_DETAIL_ATOMIC_HPP

#include <boost/sync/detail/config.hpp>

#if !defined(BOOST_SYNC_USE_STD_ATOMIC)

#if defined(BOOST_CLANG)

// Clang can be used with libstdc++, which only enables C++11 headers when explicitly enabled in the command line.
#if (__cplusplus >= 201103L) && __has_include( <atomic> )
#define BOOST_SYNC_USE_STD_ATOMIC
#endif

#elif defined(BOOST_MSVC)

#if (BOOST_MSVC >= 1700)
#define BOOST_SYNC_USE_STD_ATOMIC
#endif

#elif defined(BOOST_GCC)

#if (BOOST_GCC >= 40800) && (__cplusplus >= 201103L)
#define BOOST_SYNC_USE_STD_ATOMIC
#endif

#endif

#endif // !defined(BOOST_SYNC_USE_STD_ATOMIC)

#ifdef BOOST_SYNC_USE_STD_ATOMIC
#include <atomic>
#else
#include <boost/atomic.hpp>
#endif

#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost  {
namespace sync   {
namespace detail {

#ifdef BOOST_SYNC_USE_STD_ATOMIC

namespace atomic_ns = std;

#else

namespace atomic_ns = boost;

#endif

}
}
}

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_ATOMIC_HPP
