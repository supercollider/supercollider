/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2007 Anthony Williams
 * (C) Copyright 2011-2012 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   locks/shared_lock.hpp
 *
 * \brief  This header defines a shared lock guard.
 */

#ifndef BOOST_SYNC_LOCKS_SHARED_LOCK_HPP_INCLUDED_
#define BOOST_SYNC_LOCKS_SHARED_LOCK_HPP_INCLUDED_

#include <cstddef>
#include <boost/move/core.hpp>
#include <boost/move/utility.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/utility/explicit_operator_bool.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/detail/system_error.hpp>
#include <boost/sync/exceptions/lock_error.hpp>
#include <boost/sync/locks/lock_options.hpp>
#include <boost/sync/locks/shared_lock_fwd.hpp>
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)
#include <boost/sync/locks/unique_lock_fwd.hpp>
#include <boost/sync/locks/upgrade_lock_fwd.hpp>
#else
// These locks need to be defined for Boost.Move emulation
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/locks/upgrade_lock.hpp>
#endif

#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

/*!
 * \brief A shared lock scope guard
 */
template< typename Mutex >
class shared_lock
{
public:
    typedef Mutex mutex_type;

private:
    mutex_type* m_mutex;
    bool m_is_locked;

public:
    BOOST_MOVABLE_BUT_NOT_COPYABLE(shared_lock)

public:
    shared_lock() BOOST_NOEXCEPT :
        m_mutex(NULL), m_is_locked(false)
    {
    }

    explicit shared_lock(mutex_type& m) :
        m_mutex(&m), m_is_locked(false)
    {
        lock();
    }
    shared_lock(mutex_type& m, adopt_lock_t) BOOST_NOEXCEPT :
        m_mutex(&m), m_is_locked(true)
    {
    }
    shared_lock(mutex_type& m, defer_lock_t) BOOST_NOEXCEPT :
        m_mutex(&m), m_is_locked(false)
    {
    }
    shared_lock(mutex_type& m, try_to_lock_t) :
        m_mutex(&m), m_is_locked(false)
    {
        try_lock();
    }

    template< typename Time >
    shared_lock(typename enable_if_c< detail::time_traits< Time >::is_specialized, mutex_type& >::type m, Time const& t) :
        m_mutex(&m), m_is_locked(false)
    {
        timed_lock(t);
    }

    shared_lock(BOOST_RV_REF(shared_lock) that) BOOST_NOEXCEPT :
        m_mutex(that.m_mutex), m_is_locked(that.m_is_locked)
    {
        that.m_mutex = NULL;
        that.m_is_locked = false;
    }

    // Conversion from unique locking
    explicit shared_lock(BOOST_RV_REF(unique_lock< mutex_type >) that) :
        m_mutex(that.m_mutex), m_is_locked(that.m_is_locked)
    {
        if (m_is_locked)
            m_mutex->unlock_and_lock_shared();
        that.release();
    }

    explicit shared_lock(BOOST_RV_REF(upgrade_lock< mutex_type >) that) :
        m_mutex(that.m_mutex), m_is_locked(that.m_is_locked)
    {
        if (m_is_locked)
            m_mutex->unlock_upgrade_and_lock_shared();
        that.release();
    }

    ~shared_lock()
    {
        if (m_is_locked)
            m_mutex->unlock_shared();
    }

    shared_lock& operator= (BOOST_RV_REF(shared_lock) that) BOOST_NOEXCEPT
    {
        swap((shared_lock&)that);
        return *this;
    }

    shared_lock& operator= (BOOST_RV_REF(unique_lock< mutex_type >) that)
    {
        shared_lock temp(boost::move(that));
        swap(temp);
        return *this;
    }

    shared_lock& operator= (BOOST_RV_REF(upgrade_lock< mutex_type >) that)
    {
        shared_lock temp(boost::move(that));
        swap(temp);
        return *this;
    }

    void lock()
    {
        if (!m_mutex)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::operation_not_permitted)("boost shared_lock has no mutex"));

        if (m_is_locked)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::resource_deadlock_would_occur)("boost shared_lock already owns the mutex"));

        m_mutex->lock_shared();
        m_is_locked = true;
    }

    bool try_lock()
    {
        if (!m_mutex)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::operation_not_permitted)("boost shared_lock has no mutex"));

        if (m_is_locked)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::resource_deadlock_would_occur)("boost shared_lock already owns the mutex"));

        m_is_locked = m_mutex->try_lock_shared();

        return m_is_locked;
    }

    template< typename Time >
    typename enable_if_c< detail::time_traits< Time >::is_specialized, bool >::type timed_lock(Time const& time)
    {
        if (!m_mutex)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::operation_not_permitted)("boost shared_lock has no mutex"));

        if (m_is_locked)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::resource_deadlock_would_occur)("boost shared_lock already owns the mutex"));

        m_is_locked = m_mutex->timed_lock_shared(time);

        return m_is_locked;
    }

    template< typename Duration >
    typename detail::enable_if_tag< Duration, detail::time_duration_tag, bool >::type try_lock_for(Duration const& rel_time)
    {
        if (!m_mutex)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::operation_not_permitted)("boost shared_lock has no mutex"));

        if (m_is_locked)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::resource_deadlock_would_occur)("boost shared_lock already owns the mutex"));

        m_is_locked = m_mutex->try_lock_shared_for(rel_time);

        return m_is_locked;
    }

    template< typename TimePoint >
    typename detail::enable_if_tag< TimePoint, detail::time_point_tag, bool >::type try_lock_until(TimePoint const& abs_time)
    {
        if (!m_mutex)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::operation_not_permitted)("boost shared_lock has no mutex"));

        if (m_is_locked)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::resource_deadlock_would_occur)("boost shared_lock already owns the mutex"));

        m_is_locked = m_mutex->try_lock_shared_until(abs_time);

        return m_is_locked;
    }

    void unlock()
    {
        if (!m_mutex)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::operation_not_permitted)("boost shared_lock has no mutex"));

        if (!m_is_locked)
            BOOST_SYNC_DETAIL_THROW(lock_error, (detail::system_ns::errc::operation_not_permitted)("boost shared_lock doesn't own the mutex"));

        m_mutex->unlock_shared();
        m_is_locked = false;
    }

    BOOST_EXPLICIT_OPERATOR_BOOL()

    bool operator! () const BOOST_NOEXCEPT
    {
        return !m_is_locked;
    }

    bool owns_lock() const BOOST_NOEXCEPT
    {
        return m_is_locked;
    }

    mutex_type* mutex() const BOOST_NOEXCEPT
    {
        return m_mutex;
    }

    mutex_type* release() BOOST_NOEXCEPT
    {
        mutex_type* const res = m_mutex;
        m_mutex = NULL;
        m_is_locked = false;
        return res;
    }

    void swap(shared_lock& that) BOOST_NOEXCEPT
    {
        mutex_type* const p = m_mutex;
        m_mutex = that.m_mutex;
        that.m_mutex = p;
        const bool f = m_is_locked;
        m_is_locked = that.m_is_locked;
        that.m_is_locked = f;
    }
};

template< typename Mutex >
inline void swap(shared_lock< Mutex >& lhs, shared_lock< Mutex >& rhs) BOOST_NOEXCEPT
{
    lhs.swap(rhs);
}

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_LOCKS_SHARED_LOCK_HPP_INCLUDED_
