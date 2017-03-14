/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2007-2008 Anthony Williams
 * (C) Copyright 2012-2013 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   detail/mutexes/timed_mutex_posix.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_MUTEXES_TIMED_MUTEX_POSIX_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_MUTEXES_TIMED_MUTEX_POSIX_HPP_INCLUDED_

#include <errno.h>
#include <cstddef>
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/sync/exceptions/lock_error.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/pthread.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#if !defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
#include <boost/sync/detail/pthread_mutex_locks.hpp>
#endif
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
#define BOOST_SYNC_DEFINES_TIMED_MUTEX_NATIVE_HANDLE
#endif

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class timed_mutex
{
public:
#if defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
    typedef void _is_condition_variable_compatible;
#endif
#if defined(BOOST_SYNC_DEFINES_TIMED_MUTEX_NATIVE_HANDLE)
    typedef pthread_mutex_t* native_handle_type;
#endif

private:
    pthread_mutex_t m_mutex;
#if !defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
    pthread_cond_t m_cond;
    bool m_is_locked;
#endif

public:
#if defined(PTHREAD_MUTEX_INITIALIZER) && (defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK) || defined(PTHREAD_COND_INITIALIZER))
#if !defined(BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX)
#if !defined(BOOST_NO_CXX11_CONSTEXPR)
#define BOOST_SYNC_DEFINES_TIMED_MUTEX_CONSTEXPR_CONSTRUCTOR
#endif

    BOOST_CONSTEXPR timed_mutex() BOOST_NOEXCEPT :
        m_mutex(PTHREAD_MUTEX_INITIALIZER)
#if !defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
        , m_cond(PTHREAD_COND_INITIALIZER),
        m_is_locked(false)
#endif
    {
    }
#else
    timed_mutex() BOOST_NOEXCEPT
    {
        BOOST_CONSTEXPR_OR_CONST pthread_mutex_t temp = PTHREAD_MUTEX_INITIALIZER;
        m_mutex = temp;

#if !defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
        BOOST_CONSTEXPR_OR_CONST pthread_cond_t temp2 = PTHREAD_COND_INITIALIZER;
        m_cond = temp2;
        m_is_locked = false;
#endif
    }
#endif
#else // defined(PTHREAD_MUTEX_INITIALIZER)
    timed_mutex()
    {
        int const res = pthread_mutex_init(&m_mutex, NULL);
        if (res)
            BOOST_SYNC_DETAIL_THROW(resource_error, (res)("timed_mutex constructor failed in pthread_mutex_init"));

#if !defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
        int const res2 = pthread_cond_init(&m_cond, NULL);
        if (res2)
            BOOST_SYNC_DETAIL_THROW(resource_error, (res2)("timed_mutex constructor failed in pthread_cond_init"));
        m_is_locked = false;
#endif
    }
#endif // defined(PTHREAD_MUTEX_INITIALIZER)

    ~timed_mutex()
    {
        BOOST_VERIFY(sync::detail::posix::pthread_mutex_destroy(&m_mutex) == 0);
#if !defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
        BOOST_VERIFY(sync::detail::posix::pthread_cond_destroy(&m_cond) == 0);
#endif
    }

#if defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)

    void lock()
    {
        int const res = sync::detail::posix::pthread_mutex_lock(&m_mutex);
        if (res)
            BOOST_SYNC_DETAIL_THROW(lock_error, (res)("timed_mutex lock failed in pthread_mutex_lock"));
    }

    void unlock() BOOST_NOEXCEPT
    {
        BOOST_VERIFY(sync::detail::posix::pthread_mutex_unlock(&m_mutex) == 0);
    }

    bool try_lock()
    {
        int const res = sync::detail::posix::pthread_mutex_trylock(&m_mutex);

        if (res == 0)
            return true;
        else if (res != EBUSY)
            BOOST_SYNC_DETAIL_THROW(lock_error, (res)("timed_mutex try_lock failed in pthread_mutex_trylock"));
        return false;
    }

#else // defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)

    void lock()
    {
        sync::detail::posix::pthread_mutex_lock_guard const local_lock(m_mutex);
        while (m_is_locked)
        {
            BOOST_VERIFY(sync::detail::posix::pthread_cond_wait(&m_cond, &m_mutex) == 0);
        }
        m_is_locked = true;
    }

    void unlock() BOOST_NOEXCEPT
    {
        sync::detail::posix::pthread_mutex_lock_guard const local_lock(m_mutex);
        m_is_locked = false;
        BOOST_VERIFY(pthread_cond_signal(&m_cond) == 0);
    }

    bool try_lock()
    {
        sync::detail::posix::pthread_mutex_lock_guard const local_lock(m_mutex);
        if (m_is_locked)
            return false;
        m_is_locked = true;
        return true;
    }

#endif // defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)

    template< typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type timed_lock(Time const& t)
    {
        return priv_timed_lock(sync::detail::time_traits< Time >::to_sync_unit(t));
    }

    template< typename Duration >
    typename detail::enable_if_tag< Duration, detail::time_duration_tag, bool >::type try_lock_for(Duration const& rel_time)
    {
        return priv_timed_lock(sync::detail::time_traits< Duration >::to_sync_unit(rel_time));
    }

    template< typename TimePoint >
    typename detail::enable_if_tag< TimePoint, detail::time_point_tag, bool >::type try_lock_until(TimePoint const& abs_time)
    {
        return priv_timed_lock(sync::detail::time_traits< TimePoint >::to_sync_unit(abs_time));
    }

#if defined(BOOST_SYNC_DEFINES_TIMED_MUTEX_NATIVE_HANDLE)
    native_handle_type native_handle() BOOST_NOEXCEPT
    {
        return &m_mutex;
    }
#endif

    BOOST_DELETED_FUNCTION(timed_mutex(timed_mutex const&))
    BOOST_DELETED_FUNCTION(timed_mutex& operator= (timed_mutex const&))

private:
    bool priv_timed_lock(sync::detail::system_duration dur)
    {
        return priv_timed_lock(sync::detail::system_time_point::now() + dur);
    }

    bool priv_timed_lock(sync::detail::system_time_point const& t)
    {
#if defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)

        int const res = sync::detail::posix::pthread_mutex_timedlock(&m_mutex, &t.get());

        if (res == 0)
            return true;
        else if (res != ETIMEDOUT)
            BOOST_SYNC_DETAIL_THROW(lock_error, (res)("timed_mutex timed_lock failed in pthread_mutex_timedlock"));
        return false;

#else // defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)

        sync::detail::posix::pthread_mutex_lock_guard const local_lock(m_mutex);
        while (m_is_locked)
        {
            int const cond_res = sync::detail::posix::pthread_cond_timedwait(&m_cond, &m_mutex, &t.get());
            if (cond_res == ETIMEDOUT)
            {
                if (!m_is_locked)
                    break;
                else
                    return false;
            }
            else if (cond_res != 0)
            {
                BOOST_SYNC_DETAIL_THROW(lock_error, (cond_res)("timed_mutex timed_lock failed in pthread_cond_timedwait"));
            }
        }
        m_is_locked = true;
        return true;

#endif // defined(BOOST_SYNC_DETAIL_PTHREAD_HAS_TIMEDLOCK)
    }

    template< typename TimePoint >
    bool priv_timed_lock(sync::detail::chrono_time_point< TimePoint > const& t)
    {
        if (try_lock())
            return true;

        typedef TimePoint time_point;
        typedef typename time_point::clock clock;
        typedef typename time_point::duration duration;
        time_point now = clock::now();
        while (now < t.get())
        {
            if (priv_timed_lock(sync::detail::time_traits< duration >::to_sync_unit(t.get() - now)))
                return true;
            now = clock::now();
        }
        return false;
    }
};

} // namespace posix

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_MUTEXES_TIMED_MUTEX_POSIX_HPP_INCLUDED_
