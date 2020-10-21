/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2012-2013 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   pthread.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 *
 * This header contains POSIX threads support configuration.
 */

#ifndef BOOST_SYNC_DETAIL_PTHREAD_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_PTHREAD_HPP_INCLUDED_

#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#ifdef _POSIX_TIMEOUTS
#if _POSIX_TIMEOUTS >= 0 && _POSIX_TIMEOUTS >= 200112L
#ifndef BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK
#define BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK
#endif
#endif
#endif

namespace boost {

namespace sync {

namespace detail {

namespace posix {

#if !defined(BOOST_SYNC_HAS_PTHREAD_EINTR_BUG)

using ::pthread_mutex_destroy;
using ::pthread_mutex_lock;
using ::pthread_mutex_trylock;
#if defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
using ::pthread_mutex_timedlock;
#endif
using ::pthread_mutex_unlock;
using ::pthread_cond_destroy;
using ::pthread_cond_wait;
using ::pthread_cond_timedwait;

#else // !defined(BOOST_SYNC_HAS_PTHREAD_EINTR_BUG)

// Workaround for https://svn.boost.org/trac/boost/ticket/6200

BOOST_FORCEINLINE int pthread_mutex_destroy(pthread_mutex_t* m) BOOST_NOEXCEPT
{
    int ret;
    do
    {
        ret = ::pthread_mutex_destroy(m);
    }
    while (ret == EINTR);
    return ret;
}

BOOST_FORCEINLINE int pthread_mutex_lock(pthread_mutex_t* m) BOOST_NOEXCEPT
{
    int ret;
    do
    {
        ret = ::pthread_mutex_lock(m);
    }
    while (ret == EINTR);
    return ret;
}

BOOST_FORCEINLINE int pthread_mutex_trylock(pthread_mutex_t* m) BOOST_NOEXCEPT
{
    int ret;
    do
    {
        ret = ::pthread_mutex_trylock(m);
    }
    while (ret == EINTR);
    return ret;
}

#if defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
BOOST_FORCEINLINE int pthread_mutex_timedlock(pthread_mutex_t* m, const struct ::timespec* t) BOOST_NOEXCEPT
{
    int ret;
    do
    {
        ret = ::pthread_mutex_timedlock(m, t);
    }
    while (ret == EINTR);
    return ret;
}
#endif

BOOST_FORCEINLINE int pthread_mutex_unlock(pthread_mutex_t* m) BOOST_NOEXCEPT
{
    int ret;
    do
    {
        ret = ::pthread_mutex_unlock(m);
    }
    while (ret == EINTR);
    return ret;
}

BOOST_FORCEINLINE int pthread_cond_destroy(pthread_cond_t* c) BOOST_NOEXCEPT
{
    int ret;
    do
    {
        ret = ::pthread_cond_destroy(c);
    }
    while (ret == EINTR);
    return ret;
}

BOOST_FORCEINLINE int pthread_cond_wait(pthread_cond_t* c, pthread_mutex_t* m) BOOST_NOEXCEPT
{
    int ret;
    do
    {
        ret = ::pthread_cond_wait(c, m);
    }
    while (ret == EINTR);
    return ret;
}

BOOST_FORCEINLINE int pthread_cond_timedwait(pthread_cond_t* c, pthread_mutex_t* m, const struct ::timespec* t) BOOST_NOEXCEPT
{
    int ret;
    do
    {
        ret = ::pthread_cond_timedwait(c, m, t);
    }
    while (ret == EINTR);
    return ret;
}

#endif // !defined(BOOST_SYNC_HAS_PTHREAD_EINTR_BUG)

} // namespace posix

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_PTHREAD_HPP_INCLUDED_
