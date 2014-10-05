/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Tim Blechmann
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   futex.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 *
 * This header contains Linux futex API declarations.
 */

#ifndef BOOST_SYNC_DETAIL_FUTEX_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_FUTEX_HPP_INCLUDED_

#include <stddef.h>
#include <limits.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

namespace linux_ {

//! Invokes an operation on the futex
BOOST_FORCEINLINE int futex_invoke(int* addr1, int op, int val1, const struct ::timespec* timeout = NULL, int* addr2 = NULL, int val3 = 0) BOOST_NOEXCEPT
{
    return ::syscall(SYS_futex, addr1, op, val1, timeout, addr2, val3);
}

//! Checks that the value \c pval is \c expected and blocks
BOOST_FORCEINLINE int futex_wait(int* pval, int expected) BOOST_NOEXCEPT
{
    return futex_invoke
    (
        pval,
#ifdef FUTEX_WAIT_PRIVATE
        FUTEX_WAIT_PRIVATE,
#else
        FUTEX_WAIT,
#endif
        expected
    );
}

//! Checks that the value \c pval is \c expected and blocks until \c timeout_nsec expires
BOOST_FORCEINLINE int futex_timedwait(int* pval, int expected, uint64_t timeout_nsec) BOOST_NOEXCEPT
{
    struct ::timespec timeout;
    timeout.tv_sec = timeout_nsec / 1000000000u;
    timeout.tv_nsec = timeout_nsec % 1000000000u;
    return futex_invoke
    (
        pval,
#ifdef FUTEX_WAIT_PRIVATE
        FUTEX_WAIT_PRIVATE,
#else
        FUTEX_WAIT,
#endif
        expected,
        &timeout
    );
}

//! Wakes all threads waiting on the futex
BOOST_FORCEINLINE int futex_broadcast(int* pval) BOOST_NOEXCEPT
{
    return futex_invoke
    (
        pval,
#ifdef FUTEX_WAKE_PRIVATE
        FUTEX_WAKE_PRIVATE,
#else
        FUTEX_WAKE,
#endif
        INT_MAX
    );
}

//! Wakes the specified number of threads waiting on the futex
BOOST_FORCEINLINE int futex_signal(int* pval, int count = 1) BOOST_NOEXCEPT
{
    return futex_invoke
    (
        pval,
#ifdef FUTEX_WAKE_PRIVATE
        FUTEX_WAKE_PRIVATE,
#else
        FUTEX_WAKE,
#endif
        count
    );
}

} // namespace linux_

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_FUTEX_HPP_INCLUDED_
