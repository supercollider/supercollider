/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2007-2008 Anthony Williams
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   pthread_mutex_locks.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_PTHREAD_MUTEX_LOCKS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_PTHREAD_MUTEX_LOCKS_HPP_INCLUDED_

#include <boost/assert.hpp>
#include <boost/sync/detail/pthread.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

namespace posix {

class pthread_mutex_lock_guard
{
    pthread_mutex_t* const m_mutex;

public:
    explicit pthread_mutex_lock_guard(pthread_mutex_t& m) BOOST_NOEXCEPT : m_mutex(&m)
    {
        BOOST_VERIFY(sync::detail::posix::pthread_mutex_lock(m_mutex) == 0);
    }

    ~pthread_mutex_lock_guard()
    {
        BOOST_VERIFY(sync::detail::posix::pthread_mutex_unlock(m_mutex) == 0);
    }

    BOOST_DELETED_FUNCTION(pthread_mutex_lock_guard(pthread_mutex_lock_guard const&))
    BOOST_DELETED_FUNCTION(pthread_mutex_lock_guard& operator= (pthread_mutex_lock_guard const&))
};

class pthread_mutex_unlock_guard
{
    pthread_mutex_t* const m_mutex;

public:
    explicit pthread_mutex_unlock_guard(pthread_mutex_t& m) BOOST_NOEXCEPT : m_mutex(&m)
    {
        BOOST_VERIFY(sync::detail::posix::pthread_mutex_unlock(m_mutex) == 0);
    }

    ~pthread_mutex_unlock_guard()
    {
        BOOST_VERIFY(sync::detail::posix::pthread_mutex_lock(m_mutex) == 0);
    }

    BOOST_DELETED_FUNCTION(pthread_mutex_unlock_guard(pthread_mutex_unlock_guard const&))
    BOOST_DELETED_FUNCTION(pthread_mutex_unlock_guard& operator= (pthread_mutex_unlock_guard const&))
};

} // namespace posix

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_PTHREAD_MUTEX_LOCKS_HPP_INCLUDED_
