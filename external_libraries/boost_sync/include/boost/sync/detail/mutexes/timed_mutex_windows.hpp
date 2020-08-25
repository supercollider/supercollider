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
 * \file   detail/mutexes/timed_mutex_windows.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_MUTEXES_TIMED_MUTEX_WINDOWS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_MUTEXES_TIMED_MUTEX_WINDOWS_HPP_INCLUDED_

#include <cstddef>
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#include <boost/memory_order.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/core/enable_if.hpp>
#include <boost/winapi/wait.hpp>
#include <boost/winapi/waitable_timer.hpp>
#include <boost/sync/exceptions/lock_error.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/waitable_timer.hpp>
#include <boost/sync/detail/mutexes/basic_mutex_windows.hpp>

#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class timed_mutex
{
public:
    typedef void _is_condition_variable_compatible;

private:
    sync::detail::windows::basic_mutex m_mutex;

public:
#if !defined(BOOST_NO_CXX11_CONSTEXPR) && !defined(BOOST_SYNC_DETAIL_NO_CONSTEXPR_WITH_NON_LITERAL_BASE)
#define BOOST_SYNC_DEFINES_TIMED_MUTEX_CONSTEXPR_CONSTRUCTOR
#endif

    BOOST_SYNC_DETAIL_CONSTEXPR_WITH_NON_LITERAL_BASE timed_mutex() BOOST_NOEXCEPT : m_mutex()
    {
    }

    void lock()
    {
        m_mutex.lock();
    }

    void unlock() BOOST_NOEXCEPT
    {
        m_mutex.unlock();
    }

    bool try_lock()
    {
        return m_mutex.try_lock();
    }

    template< typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type timed_lock(Time const& t)
    {
        if (m_mutex.try_lock())
            return true;

        return priv_timed_lock(sync::detail::time_traits< Time >::to_sync_unit(t));
    }

    template< typename Duration >
    typename detail::enable_if_tag< Duration, detail::time_duration_tag, bool >::type try_lock_for(Duration const& rel_time)
    {
        if (m_mutex.try_lock())
            return true;

        return priv_timed_lock(sync::detail::time_traits< Duration >::to_sync_unit(rel_time));
    }

    template< typename TimePoint >
    typename detail::enable_if_tag< TimePoint, detail::time_point_tag, bool >::type try_lock_until(TimePoint const& abs_time)
    {
        if (m_mutex.try_lock())
            return true;

        return priv_timed_lock(sync::detail::time_traits< TimePoint >::to_sync_unit(abs_time));
    }

    BOOST_DELETED_FUNCTION(timed_mutex(timed_mutex const&))
    BOOST_DELETED_FUNCTION(timed_mutex& operator= (timed_mutex const&))

private:
    bool priv_timed_lock(sync::detail::system_time_point const& t)
    {
        boost::uint32_t old_count = m_mutex.m_active_count.load(boost::memory_order_acquire);
        m_mutex.mark_waiting_and_try_lock(old_count);
        if ((old_count & sync::detail::windows::basic_mutex::lock_flag_value) == 0u)
            return true;

        try
        {
            boost::winapi::HANDLE_ handles[2];
            handles[0] = m_mutex.get_event();
            handles[1] = sync::detail::windows::get_waitable_timer();

            if (BOOST_UNLIKELY(!boost::winapi::SetWaitableTimer(handles[1], reinterpret_cast< const boost::winapi::LARGE_INTEGER_* >(&t.get()), 0, NULL, NULL, false)))
            {
                const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
                BOOST_SYNC_DETAIL_THROW(lock_error, (err)("timed_mutex::timed_lock failed to set a timeout"));
            }

            while (true)
            {
                const boost::winapi::DWORD_ res = boost::winapi::WaitForMultipleObjects(sizeof(handles) / sizeof(*handles), handles, false, boost::winapi::infinite);
                if (BOOST_UNLIKELY(res == boost::winapi::wait_failed))
                {
                    const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
                    BOOST_SYNC_DETAIL_THROW(lock_error, (err)("timed_mutex::timed_lock failed in WaitForMultipleObjects"));
                }

                switch (res)
                {
                case boost::winapi::wait_object_0: // event was notified
                    m_mutex.clear_waiting_and_try_lock(old_count);
                    if ((old_count & sync::detail::windows::basic_mutex::lock_flag_value) == 0)
                        return true;
                    break;

                case boost::winapi::wait_object_0 + 1: // timeout has expired
                    m_mutex.m_active_count.opaque_sub(1u, boost::memory_order_relaxed);
                    return false;

                default:
                    BOOST_UNREACHABLE_RETURN(false);
                }
            }
        }
        catch (...)
        {
            m_mutex.m_active_count.opaque_sub(1u, boost::memory_order_relaxed);
            throw;
        }
    }

    bool priv_timed_lock(sync::detail::system_duration t)
    {
        boost::uint32_t old_count = m_mutex.m_active_count.load(boost::memory_order_acquire);
        m_mutex.mark_waiting_and_try_lock(old_count);
        if ((old_count & sync::detail::windows::basic_mutex::lock_flag_value) == 0u)
            return true;

        try
        {
            const boost::winapi::HANDLE_ evt = m_mutex.get_event();
            sync::detail::system_duration::native_type time_left = t.get();
            while (time_left > 0)
            {
                const boost::winapi::DWORD_ dur = time_left > boost::winapi::max_non_infinite_wait ?
                    boost::winapi::max_non_infinite_wait : static_cast< boost::winapi::DWORD_ >(time_left);
                const boost::winapi::DWORD_ res = boost::winapi::WaitForSingleObject(evt, dur);
                switch (res)
                {
                case boost::winapi::wait_object_0:
                    m_mutex.clear_waiting_and_try_lock(old_count);
                    if ((old_count & sync::detail::windows::basic_mutex::lock_flag_value) == 0)
                        return true;
                    break;

                case boost::winapi::wait_timeout:
                    time_left -= dur;
                    break;

                default:
                    {
                        const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
                        BOOST_SYNC_DETAIL_THROW(lock_error, (err)("timed_mutex::timed_lock failed in WaitForSingleObject"));
                    }
                }
            }

            m_mutex.m_active_count.opaque_sub(1u, boost::memory_order_relaxed);

            return false;
        }
        catch (...)
        {
            m_mutex.m_active_count.opaque_sub(1u, boost::memory_order_relaxed);
            throw;
        }
    }

    template< typename TimePoint >
    bool priv_timed_lock(sync::detail::chrono_time_point< TimePoint > const& t)
    {
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

} // namespace winnt

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_MUTEXES_TIMED_MUTEX_WINDOWS_HPP_INCLUDED_
