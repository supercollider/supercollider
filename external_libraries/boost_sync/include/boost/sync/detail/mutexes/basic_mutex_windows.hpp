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
 * \file   detail/mutexes/basic_mutex_windows.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_MUTEXES_BASIC_MUTEX_WINDOWS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_MUTEXES_BASIC_MUTEX_WINDOWS_HPP_INCLUDED_

#include <cstddef>
#include <boost/assert.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/detail/winapi/synchronization.hpp>
#include <boost/detail/winapi/GetLastError.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/exceptions/lock_error.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/detail/interlocked.hpp>

#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

namespace windows {

class basic_mutex
{
public:
    enum
    {
        lock_flag_bit = 31u,
        event_set_flag_bit = 30u,
        lock_flag_value = 1u << lock_flag_bit,
        event_set_flag_value = 1u << event_set_flag_bit
    };

public:
    long m_active_count;
    boost::detail::winapi::HANDLE_ m_event;

public:
    BOOST_CONSTEXPR basic_mutex() BOOST_NOEXCEPT : m_active_count(0), m_event(NULL)
    {
    }

    ~basic_mutex()
    {
        if (m_event)
            BOOST_VERIFY(boost::detail::winapi::CloseHandle(m_event) != 0);
    }

    void lock()
    {
        if (!try_lock())
            priv_lock();
    }

    void unlock() BOOST_NOEXCEPT
    {
        long const offset = lock_flag_value;
        long const old_count = BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(&m_active_count, lock_flag_value);
        if ((old_count & event_set_flag_value) == 0 && (old_count > offset))
        {
            if (!sync::detail::windows::interlocked_bit_test_and_set(&m_active_count, event_set_flag_bit))
            {
                boost::detail::winapi::SetEvent(get_event());
            }
        }
    }

    bool try_lock()
    {
        return !sync::detail::windows::interlocked_bit_test_and_set(&m_active_count, lock_flag_bit);
    }

    boost::detail::winapi::HANDLE_ get_event()
    {
        boost::detail::winapi::HANDLE_ event = sync::detail::windows::interlocked_read_acquire(&m_event);

        if (!event)
        {
            event = boost::detail::winapi::create_anonymous_event(NULL, false, false);
            if (event)
            {
#ifdef BOOST_MSVC
#pragma warning(push)
#pragma warning(disable:4311)
#pragma warning(disable:4312)
#endif
                boost::detail::winapi::HANDLE_ const old_event = BOOST_ATOMIC_INTERLOCKED_COMPARE_EXCHANGE_POINTER(&m_event, event, NULL);
#ifdef BOOST_MSVC
#pragma warning(pop)
#endif
                if (old_event != NULL)
                {
                    boost::detail::winapi::CloseHandle(event);
                    return old_event;
                }
            }
            else
            {
                const boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
                BOOST_SYNC_DETAIL_THROW(resource_error, (err)("failed to create an event object"));
            }
        }

        return event;
    }

    void mark_waiting_and_try_lock(long& old_count)
    {
        while (true)
        {
            long const was_locked = (old_count & lock_flag_value);
            long const new_count = was_locked ? (old_count + 1) : (old_count | lock_flag_value);
            long const current = BOOST_ATOMIC_INTERLOCKED_COMPARE_EXCHANGE(&m_active_count, new_count, old_count);
            if (current == old_count)
            {
                if (was_locked)
                    old_count = new_count;
                break;
            }
            old_count = current;
        }
    }

    void clear_waiting_and_try_lock(long& old_count)
    {
        old_count &= ~lock_flag_value;
        old_count |= event_set_flag_value;
        while (true)
        {
            long const new_count = ((old_count & lock_flag_value) ? old_count : ((old_count - 1) | lock_flag_value)) & ~static_cast< long >(event_set_flag_value);
            long const current = BOOST_ATOMIC_INTERLOCKED_COMPARE_EXCHANGE(&m_active_count, new_count, old_count);
            if (current == old_count)
                break;

            old_count = current;
        }
    }

    BOOST_DELETED_FUNCTION(basic_mutex(basic_mutex const&))
    BOOST_DELETED_FUNCTION(basic_mutex& operator= (basic_mutex const&))

private:
    void priv_lock()
    {
        long old_count = m_active_count;
        mark_waiting_and_try_lock(old_count);

        if (old_count & lock_flag_value) try
        {
            boost::detail::winapi::HANDLE_ const evt = get_event();
            do
            {
                const boost::detail::winapi::DWORD_ retval = boost::detail::winapi::WaitForSingleObject(evt, boost::detail::winapi::infinite);
                if (retval != boost::detail::winapi::wait_object_0)
                {
                    const boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
                    BOOST_SYNC_DETAIL_THROW(lock_error, (err)("failed to wait on the event object"));
                }
                clear_waiting_and_try_lock(old_count);
            }
            while (old_count & lock_flag_value);
        }
        catch (...)
        {
            BOOST_ATOMIC_INTERLOCKED_EXCHANGE_ADD(&m_active_count, -1);
            throw;
        }
    }
};

} // namespace windows

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_MUTEXES_MUTEX_WINDOWS_HPP_INCLUDED_
