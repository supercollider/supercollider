/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2007-2008 Anthony Williams
 * (C) Copyright 2011-2012 Vicente J. Botet Escriba
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   detail/condition_variables/condition_variable_any_generic.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_CONDITION_VARIABLES_CONDITION_VARIABLE_ANY_GENERIC_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_CONDITION_VARIABLES_CONDITION_VARIABLE_ANY_GENERIC_HPP_INCLUDED_

#include <cstddef>
#include <boost/utility/enable_if.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/locks/lock_guard.hpp>
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/condition_variables/condition_variable.hpp>
#include <boost/sync/condition_variables/cv_status.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class condition_variable_any
{
private:
    template< typename Lock >
    class relocker
    {
    private:
        Lock* m_lock;

    public:
        relocker() : m_lock(NULL) {}

        ~relocker() BOOST_NOEXCEPT_IF(false)
        {
            if (m_lock)
                m_lock->lock();
        }

        void unlock(Lock& lock)
        {
            lock.unlock();
            m_lock = &lock;
        }
    };

private:
    sync::mutex m_mutex;
    sync::condition_variable m_cond;

public:
#if defined(BOOST_SYNC_DEFINES_MUTEX_CONSTEXPR_CONSTRUCTOR) && defined(BOOST_SYNC_DEFINES_CONDITION_VARIABLE_CONSTEXPR_CONSTRUCTOR)
#define BOOST_SYNC_DEFINES_CONDITION_VARIABLE_ANY_CONSTEXPR_CONSTRUCTOR
    BOOST_DEFAULTED_FUNCTION(BOOST_CONSTEXPR condition_variable_any() BOOST_NOEXCEPT, {})
#else
    BOOST_DEFAULTED_FUNCTION(condition_variable_any(), {})
#endif

    void notify_one()
    {
        sync::lock_guard< sync::mutex > internal_lock(m_mutex);
        m_cond.notify_one();
    }

    void notify_all()
    {
        sync::lock_guard< sync::mutex > internal_lock(m_mutex);
        m_cond.notify_all();
    }

    template< typename Lock >
    void wait(Lock& lock)
    {
        relocker< Lock > relock_guard;
        sync::unique_lock< sync::mutex > internal_lock(m_mutex);
        relock_guard.unlock(lock);
        m_cond.wait(internal_lock);
    }

    template< typename Lock, typename Predicate >
    void wait(Lock& lock, Predicate pred)
    {
        while (!pred())
            this->wait(lock);
    }

    template< typename Lock, typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type
    timed_wait(Lock& lock, Time const& t)
    {
        relocker< Lock > relock_guard;
        sync::unique_lock< sync::mutex > internal_lock(m_mutex);
        relock_guard.unlock(lock);
        return m_cond.timed_wait(internal_lock, t);
    }

    template< typename Lock, typename Time, typename Predicate >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type
    timed_wait(Lock& lock, Time const& t, Predicate pred)
    {
        while (!pred())
        {
            if (!this->timed_wait(lock, t))
                return pred();
        }
        return true;
    }

    template< typename Lock, typename TimePoint >
    typename detail::enable_if_tag< TimePoint, detail::time_point_tag, sync::cv_status >::type
    wait_until(Lock& lock, TimePoint const& abs_time)
    {
        relocker< Lock > relock_guard;
        sync::unique_lock< sync::mutex > internal_lock(m_mutex);
        relock_guard.unlock(lock);
        return m_cond.wait_until(internal_lock, abs_time);
    }

    template< typename Lock, typename TimePoint, typename Predicate >
    typename detail::enable_if_tag< TimePoint, detail::time_point_tag, bool >::type
    wait_until(Lock& lock, TimePoint const& abs_time, Predicate pred)
    {
        while (!pred())
        {
            if (this->wait_until(lock, abs_time) != sync::cv_status::no_timeout)
                return pred();
        }
        return true;
    }

    template< typename Lock, typename Duration >
    typename detail::enable_if_tag< Duration, detail::time_duration_tag, sync::cv_status >::type
    wait_for(Lock& lock, Duration const& rel_time)
    {
        relocker< Lock > relock_guard;
        sync::unique_lock< sync::mutex > internal_lock(m_mutex);
        relock_guard.unlock(lock);
        return m_cond.wait_for(internal_lock, rel_time);
    }

    template< typename Lock, typename Duration, typename Predicate >
    typename detail::enable_if_tag< Duration, detail::time_duration_tag, bool >::type
    wait_for(Lock& lock, Duration const& rel_time, Predicate pred)
    {
        while (!pred())
        {
            if (this->wait_for(lock, rel_time) != sync::cv_status::no_timeout)
                return pred();
        }
        return true;
    }

    BOOST_DELETED_FUNCTION(condition_variable_any(condition_variable_any const&))
    BOOST_DELETED_FUNCTION(condition_variable_any& operator= (condition_variable_any const&))
};

} // namespace abi

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_CONDITION_VARIABLES_CONDITION_VARIABLE_ANY_GENERIC_HPP_INCLUDED_
