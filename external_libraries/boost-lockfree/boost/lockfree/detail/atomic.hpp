//  Copyright (C) 2011 Tim Blechmann
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_LOCKFREE_DETAIL_ATOMIC_HPP
#define BOOST_LOCKFREE_DETAIL_ATOMIC_HPP

#include <boost/config.hpp>

// at this time, neither gcc (4.7) not clang (3.2) completely implement atomic<>
#define BOOST_LOCKFREE_NO_HDR_ATOMIC

#if (__cplusplus < 201103L) || defined(BOOST_LOCKFREE_NO_HDR_ATOMIC)
#include <boost/atomic.hpp>
#else
#include <atomic>
#endif

namespace boost {
namespace lockfree {
namespace detail {

#if (__cplusplus < 201103L) || defined(BOOST_LOCKFREE_NO_HDR_ATOMIC)
using boost::atomic;
using boost::memory_order_acquire;
using boost::memory_order_consume;
using boost::memory_order_relaxed;
using boost::memory_order_release;
#else
using std::atomic;
using std::memory_order_acquire;
using std::memory_order_consume;
using std::memory_order_relaxed;
using std::memory_order_release;
#endif

}
using detail::atomic;
using detail::memory_order_acquire;
using detail::memory_order_consume;
using detail::memory_order_relaxed;
using detail::memory_order_release;

}}

#endif /* BOOST_LOCKFREE_DETAIL_ATOMIC_HPP */
