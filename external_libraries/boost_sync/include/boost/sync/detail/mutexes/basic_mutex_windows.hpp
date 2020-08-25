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
#include <boost/cstdint.hpp>
#include <boost/assert.hpp>
#include <boost/memory_order.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/winapi/handles.hpp>
#include <boost/winapi/event.hpp>
#include <boost/winapi/wait.hpp>
#include <boost/winapi/get_last_error.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/exceptions/lock_error.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/detail/throw_exception.hpp>

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
    boost::atomic< boost::uint32_t > m_active_count;
    // Note: Using uintptr_t instead of boost::winapi::HANDLE_ so that boost::atomic constructor is constexpr
    boost::atomic< boost::uintptr_t > m_event;

public:
    BOOST_CONSTEXPR basic_mutex() BOOST_NOEXCEPT : m_active_count(0u), m_event(0u)
    {
    }

    ~basic_mutex()
    {
        boost::winapi::HANDLE_ event = reinterpret_cast< boost::winapi::HANDLE_ >(m_event.load(boost::memory_order_relaxed));
        if (event)
            BOOST_VERIFY(boost::winapi::CloseHandle(event) != 0);
    }

    void lock()
    {
        if (!try_lock())
            priv_lock();
    }

    void unlock() BOOST_NOEXCEPT
    {
        BOOST_CONSTEXPR_OR_CONST boost::uint32_t offset = lock_flag_value;
        const boost::uint32_t old_count = m_active_count.fetch_add(offset, boost::memory_order_release);
        if ((old_count & event_set_flag_value) == 0u && (old_count > offset))
        {
            if (!m_active_count.bit_test_and_set(event_set_flag_bit, boost::memory_order_relaxed))
            {
                boost::winapi::SetEvent(get_event());
            }
        }
    }

    bool try_lock()
    {
        return !m_active_count.bit_test_and_set(lock_flag_bit, boost::memory_order_acquire);
    }

    boost::winapi::HANDLE_ get_event()
    {
        boost::uintptr_t event = m_event.load(boost::memory_order_acquire);

        if (BOOST_UNLIKELY(!event))
        {
            boost::winapi::HANDLE_ new_event = boost::winapi::create_anonymous_event(NULL, false, false);
            if (BOOST_LIKELY(!!new_event))
            {
                event = reinterpret_cast< boost::uintptr_t >(new_event);
                boost::uintptr_t old_event = 0u;
                const bool succeeded = m_event.compare_exchange_strong(old_event, event, boost::memory_order_acquire, boost::memory_order_relaxed);
                if (BOOST_UNLIKELY(!succeeded))
                {
                    boost::winapi::CloseHandle(reinterpret_cast< boost::winapi::HANDLE_ >(event));
                    return reinterpret_cast< boost::winapi::HANDLE_ >(old_event);
                }
            }
            else
            {
                const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
                BOOST_SYNC_DETAIL_THROW(resource_error, (err)("Failed to create an event object"));
            }
        }

        return reinterpret_cast< boost::winapi::HANDLE_ >(event);
    }

    void mark_waiting_and_try_lock(boost::uint32_t& old_count)
    {
        while (true)
        {
            const boost::uint32_t was_locked = (old_count & lock_flag_value);
            const boost::uint32_t new_count = was_locked ? (old_count + 1u) : (old_count | lock_flag_value);
            if (m_active_count.compare_exchange_weak(old_count, new_count, boost::memory_order_relaxed, boost::memory_order_relaxed))
            {
                if (was_locked)
                    old_count = new_count;
                break;
            }
        }
    }

    void clear_waiting_and_try_lock(boost::uint32_t& old_count)
    {
        old_count &= ~static_cast< boost::uint32_t >(lock_flag_value);
        old_count |= event_set_flag_value;
        while (true)
        {
            const boost::uint32_t new_count = ((old_count & lock_flag_value) ? old_count : ((old_count - 1u) | lock_flag_value)) & ~static_cast< boost::uint32_t >(event_set_flag_value);
            if (m_active_count.compare_exchange_weak(old_count, new_count, boost::memory_order_relaxed, boost::memory_order_relaxed))
                break;
        }
    }

    BOOST_DELETED_FUNCTION(basic_mutex(basic_mutex const&))
    BOOST_DELETED_FUNCTION(basic_mutex& operator= (basic_mutex const&))

private:
    void priv_lock()
    {
        boost::uint32_t old_count = m_active_count.load(boost::memory_order_acquire);
        mark_waiting_and_try_lock(old_count);

        if (old_count & lock_flag_value) try
        {
            boost::winapi::HANDLE_ const evt = get_event();
            do
            {
                const boost::winapi::DWORD_ retval = boost::winapi::WaitForSingleObject(evt, boost::winapi::infinite);
                if (BOOST_UNLIKELY(retval != boost::winapi::wait_object_0))
                {
                    const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
                    BOOST_SYNC_DETAIL_THROW(lock_error, (err)("Failed to wait on the event object"));
                }
                clear_waiting_and_try_lock(old_count);
            }
            while (old_count & lock_flag_value);
        }
        catch (...)
        {
            m_active_count.opaque_sub(1u, boost::memory_order_relaxed);
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
