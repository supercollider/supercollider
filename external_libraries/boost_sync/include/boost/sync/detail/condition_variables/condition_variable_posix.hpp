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
 * \file   detail/condition_variables/condition_variable_posix.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_CONDITION_VARIABLES_CONDITION_VARIABLE_POSIX_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_CONDITION_VARIABLES_CONDITION_VARIABLE_POSIX_HPP_INCLUDED_

#include <cstddef>
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/mpl/and.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/locks/unique_lock_fwd.hpp>
#include <boost/sync/exceptions/wait_error.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/traits/is_condition_variable_compatible.hpp>
#include <boost/sync/detail/pthread.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/condition_variables/cv_status.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#define BOOST_SYNC_DEFINES_CONDITION_VARIABLE_NATIVE_HANDLE

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class condition_variable
{
public:
    typedef pthread_cond_t* native_handle_type;

private:
    pthread_cond_t m_cond;

public:
#if defined(PTHREAD_COND_INITIALIZER)
#if !defined(BOOST_NO_CXX11_UNIFIED_INITIALIZATION_SYNTAX)
#if !defined(BOOST_NO_CXX11_CONSTEXPR)
#define BOOST_SYNC_DEFINES_CONDITION_VARIABLE_CONSTEXPR_CONSTRUCTOR
#endif

    BOOST_CONSTEXPR condition_variable() BOOST_NOEXCEPT : m_cond(PTHREAD_COND_INITIALIZER)
    {
    }
#else
    condition_variable() BOOST_NOEXCEPT
    {
        BOOST_CONSTEXPR_OR_CONST pthread_cond_t temp = PTHREAD_COND_INITIALIZER;
        m_cond = temp;
    }
#endif
#else // defined(PTHREAD_COND_INITIALIZER)
    condition_variable()
    {
        int const res = pthread_cond_init(&m_cond, NULL);
        if (res)
            BOOST_SYNC_DETAIL_THROW(resource_error, (res)("condition_variable constructor failed in pthread_cond_init"));
    }
#endif // defined(PTHREAD_COND_INITIALIZER)

    ~condition_variable()
    {
        BOOST_VERIFY(sync::detail::posix::pthread_cond_destroy(&m_cond) == 0);
    }

    void notify_one() BOOST_NOEXCEPT
    {
        BOOST_VERIFY(::pthread_cond_signal(&m_cond) == 0);
    }

    void notify_all() BOOST_NOEXCEPT
    {
        BOOST_VERIFY(::pthread_cond_broadcast(&m_cond) == 0);
    }

    template< typename Mutex >
    typename enable_if< is_condition_variable_compatible< Mutex >, void >::type wait(unique_lock< Mutex >& lock)
    {
        BOOST_ASSERT(lock.owns_lock());
        this->priv_wait(lock.mutex()->native_handle());
    }

    template< typename Mutex, typename Predicate >
    typename enable_if< is_condition_variable_compatible< Mutex >, void >::type wait(unique_lock< Mutex >& lock, Predicate pred)
    {
        BOOST_ASSERT(lock.owns_lock());
        pthread_mutex_t* const mtx = lock.mutex()->native_handle();
        while (!pred())
            this->priv_wait(mtx);
    }

    template< typename Mutex, typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized && is_condition_variable_compatible< Mutex >::value, bool >::type
    timed_wait(unique_lock< Mutex >& lock, Time const& t)
    {
        BOOST_ASSERT(lock.owns_lock());
        return priv_timed_wait(lock, sync::detail::time_traits< Time >::to_sync_unit(t)) == sync::cv_status::no_timeout;
    }

    template< typename Mutex, typename TimePoint, typename Predicate >
    typename enable_if< mpl::and_< detail::is_time_tag_of< TimePoint, detail::time_point_tag >, is_condition_variable_compatible< Mutex > >, bool >::type
    timed_wait(unique_lock< Mutex >& lock, TimePoint const& t, Predicate pred)
    {
        BOOST_ASSERT(lock.owns_lock());
        typedef typename sync::detail::time_traits< TimePoint >::unit_type unit_type;
        unit_type abs_timeout = sync::detail::time_traits< TimePoint >::to_sync_unit(t);
        while (!pred())
        {
            if (this->priv_timed_wait(lock, abs_timeout) != sync::cv_status::no_timeout)
                return pred();
        }
        return true;
    }

    template< typename Mutex, typename Duration, typename Predicate >
    typename enable_if< mpl::and_< detail::is_time_tag_of< Duration, detail::time_duration_tag >, is_condition_variable_compatible< Mutex > >, bool >::type
    timed_wait(unique_lock< Mutex >& lock, Duration const& t, Predicate pred)
    {
        BOOST_ASSERT(lock.owns_lock());
        sync::detail::system_time_point abs_timeout = sync::detail::system_time_point::now() + sync::detail::time_traits< Duration >::to_sync_unit(t);
        while (!pred())
        {
            if (this->priv_timed_wait(lock, abs_timeout) != sync::cv_status::no_timeout)
                return pred();
        }
        return true;
    }

    template< typename Mutex, typename TimePoint >
    typename enable_if< mpl::and_< detail::is_time_tag_of< TimePoint, detail::time_point_tag >, is_condition_variable_compatible< Mutex > >, sync::cv_status >::type
    wait_until(unique_lock< Mutex >& lock, TimePoint const& abs_time)
    {
        BOOST_ASSERT(lock.owns_lock());
        return priv_timed_wait(lock, sync::detail::time_traits< TimePoint >::to_sync_unit(abs_time));
    }

    template< typename Mutex, typename TimePoint, typename Predicate >
    typename enable_if< mpl::and_< detail::is_time_tag_of< TimePoint, detail::time_point_tag >, is_condition_variable_compatible< Mutex > >, bool >::type
    wait_until(unique_lock< Mutex >& lock, TimePoint const& abs_time, Predicate pred)
    {
        BOOST_ASSERT(lock.owns_lock());
        typedef typename sync::detail::time_traits< TimePoint >::unit_type unit_type;
        unit_type abs_timeout = sync::detail::time_traits< TimePoint >::to_sync_unit(abs_time);
        while (!pred())
        {
            if (this->priv_timed_wait(lock, abs_timeout) != sync::cv_status::no_timeout)
                return pred();
        }
        return true;
    }

    template< typename Mutex, typename Duration >
    typename enable_if< mpl::and_< detail::is_time_tag_of< Duration, detail::time_duration_tag >, is_condition_variable_compatible< Mutex > >, sync::cv_status >::type
    wait_for(unique_lock< Mutex >& lock, Duration const& rel_time)
    {
        BOOST_ASSERT(lock.owns_lock());
        return priv_timed_wait(lock, sync::detail::time_traits< Duration >::to_sync_unit(rel_time));
    }

    template< typename Mutex, typename Duration, typename Predicate >
    typename enable_if< mpl::and_< detail::is_time_tag_of< Duration, detail::time_duration_tag >, is_condition_variable_compatible< Mutex > >, bool >::type
    wait_for(unique_lock< Mutex >& lock, Duration const& rel_time, Predicate pred)
    {
        BOOST_ASSERT(lock.owns_lock());
        sync::detail::system_time_point abs_timeout = sync::detail::system_time_point::now() + sync::detail::time_traits< Duration >::to_sync_unit(rel_time);
        while (!pred())
        {
            if (this->priv_timed_wait(lock, abs_timeout) != sync::cv_status::no_timeout)
                return pred();
        }
        return true;
    }

    native_handle_type native_handle() BOOST_NOEXCEPT
    {
        return &m_cond;
    }

    BOOST_DELETED_FUNCTION(condition_variable(condition_variable const&))
    BOOST_DELETED_FUNCTION(condition_variable& operator= (condition_variable const&))

private:
    void priv_wait(pthread_mutex_t* mtx)
    {
        int const res = sync::detail::posix::pthread_cond_wait(&m_cond, mtx);
        if (res != 0)
            BOOST_SYNC_DETAIL_THROW(wait_error, (res)("condition_variable::wait failed in pthread_cond_wait"));
    }

    template< typename Mutex >
    sync::cv_status priv_timed_wait(unique_lock< Mutex >& lock, sync::detail::system_duration dur)
    {
        return priv_timed_wait(lock, sync::detail::system_time_point::now() + dur);
    }

    template< typename Mutex >
    sync::cv_status priv_timed_wait(unique_lock< Mutex >& lock, sync::detail::system_time_point const& t)
    {
        int const res = sync::detail::posix::pthread_cond_timedwait(&m_cond, lock.mutex()->native_handle(), &t.get());
        if (res == ETIMEDOUT)
            return sync::cv_status::timeout;
        else if (res != 0)
            BOOST_SYNC_DETAIL_THROW(wait_error, (res)("condition_variable::timed_wait failed in pthread_cond_timedwait"));
        return sync::cv_status::no_timeout;
    }

    template< typename Mutex, typename TimePoint >
    sync::cv_status priv_timed_wait(unique_lock< Mutex >& lock, sync::detail::chrono_time_point< TimePoint > const& t)
    {
        typedef TimePoint time_point;
        typedef typename time_point::clock clock;
        typedef typename time_point::duration duration;
        time_point now = clock::now();
        while (now < t.get())
        {
            if (priv_timed_wait(lock, sync::detail::time_traits< duration >::to_sync_unit(t.get() - now)) == sync::cv_status::no_timeout)
                return sync::cv_status::no_timeout;
            now = clock::now();
        }
        return sync::cv_status::timeout;
    }
};

} // namespace posix

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_CONDITION_VARIABLES_CONDITION_VARIABLE_POSIX_HPP_INCLUDED_
