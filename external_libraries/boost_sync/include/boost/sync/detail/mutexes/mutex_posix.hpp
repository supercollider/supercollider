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
 * \file   detail/mutexes/mutex_posix.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_MUTEXES_MUTEX_POSIX_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_MUTEXES_MUTEX_POSIX_HPP_INCLUDED_

#include <cstddef>
#include <boost/assert.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/exceptions/lock_error.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/detail/pthread.hpp>

#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#define BOOST_SYNC_DEFINES_MUTEX_NATIVE_HANDLE

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class mutex
{
public:
    typedef void _is_condition_variable_compatible;

    typedef pthread_mutex_t* native_handle_type;

private:
    pthread_mutex_t m_mutex;

public:
#if defined(PTHREAD_MUTEX_INITIALIZER)
#if !defined(BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX)
#if !defined(BOOST_NO_CXX11_CONSTEXPR)
#define BOOST_SYNC_DEFINES_MUTEX_CONSTEXPR_CONSTRUCTOR
#endif

    BOOST_CONSTEXPR mutex() BOOST_NOEXCEPT : m_mutex(PTHREAD_MUTEX_INITIALIZER)
    {
    }
#else
    mutex() BOOST_NOEXCEPT
    {
        BOOST_CONSTEXPR_OR_CONST pthread_mutex_t temp = PTHREAD_MUTEX_INITIALIZER;
        m_mutex = temp;
    }
#endif
#else // defined(PTHREAD_MUTEX_INITIALIZER)
    mutex()
    {
        int const res = pthread_mutex_init(&m_mutex, NULL);
        if (res)
        {
            BOOST_SYNC_DETAIL_THROW(resource_error, (res)("boost:: mutex constructor failed in pthread_mutex_init"));
        }
    }
#endif // defined(PTHREAD_MUTEX_INITIALIZER)

    ~mutex()
    {
        BOOST_VERIFY(sync::detail::posix::pthread_mutex_destroy(&m_mutex) == 0);
    }

    void lock()
    {
        int const res = sync::detail::posix::pthread_mutex_lock(&m_mutex);
        if (res)
        {
            BOOST_SYNC_DETAIL_THROW(lock_error, (res)("boost: mutex lock failed in pthread_mutex_lock"));
        }
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
            BOOST_SYNC_DETAIL_THROW(lock_error, (res)("boost: mutex trylock failed in pthread_mutex_trylock"));
        return false;
    }

    native_handle_type native_handle() BOOST_NOEXCEPT
    {
        return &m_mutex;
    }

    BOOST_DELETED_FUNCTION(mutex(mutex const&))
    BOOST_DELETED_FUNCTION(mutex& operator= (mutex const&))
};

} // namespace posix

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_MUTEXES_MUTEX_POSIX_HPP_INCLUDED_
