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
 * \file   detail/condition_variables/basic_condition_variable_windows.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_CONDITION_VARIABLES_BASIC_CONDITION_VARIABLE_WINDOWS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_CONDITION_VARIABLES_BASIC_CONDITION_VARIABLE_WINDOWS_HPP_INCLUDED_

#include <limits.h>
#include <cstddef>
#include <boost/assert.hpp>
#include <boost/memory_order.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/winapi/get_last_error.hpp>
#include <boost/winapi/semaphore.hpp>
#include <boost/winapi/wait.hpp>
#include <boost/winapi/waitable_timer.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/exceptions/wait_error.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/waitable_timer.hpp>
#include <boost/sync/locks/unique_lock_fwd.hpp>
#include <boost/sync/locks/lock_guard.hpp>
#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/condition_variables/cv_status.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

namespace windows {

class basic_condition_variable
{
private:
    typedef boost::sync::mutex mutex_type;

    /*!
     * \brief An entry of the list of semaphores to block and release threads.
     *
     * The semaphores are organized in a doubly linked list so that it is guaranteed that waits are never
     * performed on the semaphores that are participating in ongoing notifications. This is needed because
     * unblocking from a semaphore and updating internal thread counters is not atomic, and the following
     * scenario is possible:
     *
     *  1. Threads A and B are blocked on the CV.
     *  2. Thread C calls notify_all(), the semaphore is released (set to value 2).
     *  3. Thread D calls wait(). It may enter WaitForSingleObject() before thread A or B is released by OS,
     *     reducing the counter of the semaphore and leaving one of the threads blocked. This makes thread D
     *     to be notified by thread C and either thread A or B being left blocked.
     *
     * In the current implementation, thread D would have to create a new semaphore to block on. The semaphores
     * are reusable, so that when the notification is complete, it can be used for waiting again.
     */
    struct waiter_state
    {
        // Pointers to the adjacent nodes in the list
        waiter_state* m_prev, *m_next;
        // Semaphore to block on
        boost::winapi::HANDLE_ m_semaphore;
        // The number of blocked threads on this semaphore and the number of threads that are in the process of being woken
        unsigned long m_waiter_count, m_notify_count;

        waiter_state()
        {
            m_prev = m_next = this;
            m_semaphore = boost::winapi::create_anonymous_semaphore(NULL, 0, LONG_MAX);
            if (!m_semaphore)
            {
                const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
                BOOST_SYNC_DETAIL_THROW(resource_error, (err)("condition_variable failed to create a semaphore"));
            }
            m_waiter_count = m_notify_count = 0u;
        }

        ~waiter_state()
        {
            BOOST_VERIFY(boost::winapi::CloseHandle(m_semaphore) != 0);
        }

        BOOST_DELETED_FUNCTION(waiter_state(waiter_state const&))
        BOOST_DELETED_FUNCTION(waiter_state& operator= (waiter_state const&))
    };

    //! A scope guard that automatically locks the lock object on destructor
    template< typename Lockable >
    class relocker
    {
        Lockable& m_lock;
        bool m_unlocked;

    public:
        explicit relocker(Lockable& lock) BOOST_NOEXCEPT : m_lock(lock), m_unlocked(false)
        {
        }

        ~relocker() BOOST_NOEXCEPT_IF(false)
        {
            if (m_unlocked)
                m_lock.lock();
        }

        void unlock() BOOST_NOEXCEPT
        {
            m_lock.unlock();
            m_unlocked = true;
        }

        BOOST_DELETED_FUNCTION(relocker(relocker const&))
        BOOST_DELETED_FUNCTION(relocker& operator= (relocker const&))
    };

private:
    // Internal mutex to protect internal data
    mutex_type m_internal_mutex;
    // Points to the waiter state that will be used to wake up blocked threads
    waiter_state* m_notify_state;
    // Points to the waiter state that will be used to block threads
    waiter_state* m_wait_state;
    // Total number of blocked threads, in all waiter states
    boost::atomic< std::size_t > m_total_waiter_count;

public:
    BOOST_SYNC_DETAIL_CONSTEXPR_WITH_NON_LITERAL_BASE basic_condition_variable() BOOST_NOEXCEPT :
        m_notify_state(NULL), m_wait_state(NULL), m_total_waiter_count(0u)
    {
    }

    ~basic_condition_variable()
    {
        BOOST_ASSERT(m_total_waiter_count.load(boost::memory_order_relaxed) == 0u);

        if (m_notify_state)
        {
            waiter_state* p = m_notify_state, *const end = p;
            do
            {
                waiter_state* next = p->m_next;
                delete p;
                p = next;
            }
            while (p != end);

            m_notify_state = NULL;
            m_wait_state = NULL;
        }
    }

    void notify_one() BOOST_NOEXCEPT
    {
        if (m_total_waiter_count.load(boost::memory_order_acquire) > 0u)
        {
            boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);
            if (m_total_waiter_count.load(boost::memory_order_relaxed) == 0u)
                return;

            wake_waiters(1u);
        }
    }

    void notify_all() BOOST_NOEXCEPT
    {
        if (m_total_waiter_count.load(boost::memory_order_acquire) > 0u)
        {
            boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);
            const std::size_t count = m_total_waiter_count.load(boost::memory_order_relaxed);
            if (count == 0u)
                return;

            wake_waiters(count);
        }
    }

    template< typename Lockable >
    void wait(Lockable& lock)
    {
        relocker< Lockable > unlocker(lock);

        waiter_state* const state = initiate_wait();

        unlocker.unlock();

        priv_wait(state);
    }

    template< typename Lockable >
    sync::cv_status timed_wait(Lockable& lock, sync::detail::system_duration t)
    {
        relocker< Lockable > unlocker(lock);

        waiter_state* const state = initiate_wait();

        unlocker.unlock();

        return priv_timed_wait(state, t);
    }

    template< typename Lockable >
    sync::cv_status timed_wait(Lockable& lock, sync::detail::system_time_point const& t)
    {
        relocker< Lockable > unlocker(lock);

        const boost::winapi::HANDLE_ waitable_timer = sync::detail::windows::get_waitable_timer();

        if (BOOST_UNLIKELY(!boost::winapi::SetWaitableTimer(waitable_timer, reinterpret_cast< const boost::winapi::LARGE_INTEGER_* >(&t.get()), 0, NULL, NULL, false)))
        {
            const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(wait_error, (err)("condition_variable timed_wait failed to set a timeout"));
        }

        waiter_state* const state = initiate_wait();

        unlocker.unlock();

        return priv_timed_wait(state, waitable_timer);
    }

    template< typename Lockable, typename TimePoint >
    sync::cv_status timed_wait(Lockable& lock, sync::detail::chrono_time_point< TimePoint > const& t)
    {
        typedef TimePoint time_point;
        typedef typename time_point::clock clock;
        typedef typename time_point::duration duration;
        time_point now = clock::now();
        while (now < t.get())
        {
            if (timed_wait(lock, sync::detail::time_traits< duration >::to_sync_unit(t.get() - now)) == sync::cv_status::no_timeout)
                return sync::cv_status::no_timeout;
            now = clock::now();
        }
        return sync::cv_status::timeout;
    }

    BOOST_DELETED_FUNCTION(basic_condition_variable(basic_condition_variable const&))
    BOOST_DELETED_FUNCTION(basic_condition_variable& operator= (basic_condition_variable const&))

private:
    void wake_waiters(std::size_t count_to_wake) BOOST_NOEXCEPT
    {
        m_total_waiter_count.opaque_sub(count_to_wake, boost::memory_order_relaxed);
        if (count_to_wake > 0u)
        {
            while (true)
            {
                unsigned long n = m_notify_state->m_waiter_count - m_notify_state->m_notify_count;
                if (n > 0u)
                {
                    if (n > count_to_wake)
                        n = static_cast< unsigned long >(count_to_wake);
                    m_notify_state->m_notify_count += n;
                    count_to_wake -= n;
                    boost::winapi::ReleaseSemaphore(m_notify_state->m_semaphore, static_cast< long >(n), NULL);
                }

                if (count_to_wake > 0u)
                    m_notify_state = m_notify_state->m_next;
                else
                    break;
            }
        }
    }

    waiter_state* initiate_wait()
    {
        boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);

        if (!m_wait_state)
        {
            // It's the first waiter
            waiter_state* p = new waiter_state();
            m_notify_state = m_wait_state = p;
            p->m_waiter_count = 1u;
            m_total_waiter_count.store(1u, boost::memory_order_relaxed);
            return p;
        }

        // Find a waiter state without any ongoing notifications
        waiter_state* const end = m_wait_state;
        do
        {
            if (m_wait_state->m_notify_count == 0u && m_wait_state->m_waiter_count < static_cast< unsigned long >(LONG_MAX))
            {
                ++m_wait_state->m_waiter_count;
                m_total_waiter_count.opaque_add(1u, boost::memory_order_relaxed);
                return m_wait_state;
            }

            m_wait_state = m_wait_state->m_next;
        }
        while (m_wait_state != end);

        // None found, create a new waiter state
        m_wait_state = new waiter_state();

        m_wait_state->m_prev = end;
        waiter_state* const next = end->m_next;
        m_wait_state->m_next = next;
        next->m_prev = end->m_next = m_wait_state;

        m_wait_state->m_waiter_count = 1u;
        m_total_waiter_count.opaque_add(1u, boost::memory_order_relaxed);

        return m_wait_state;
    }

    void priv_wait(waiter_state* state)
    {
        while (true)
        {
            const boost::winapi::DWORD_ res = boost::winapi::WaitForSingleObject(state->m_semaphore, boost::winapi::infinite);
            if (BOOST_UNLIKELY(res != boost::winapi::wait_object_0))
            {
                const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
                {
                    boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);
                    --state->m_waiter_count;
                    if (state->m_notify_count > 0u)
                        --state->m_notify_count;
                    else
                        m_total_waiter_count.opaque_sub(1u, boost::memory_order_relaxed);
                }
                BOOST_SYNC_DETAIL_THROW(wait_error, (err)("condition_variable wait failed in WaitForSingleObject"));
            }

            boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);
            if (state->m_notify_count > 0u)
            {
                // Total waiter count is already actual here (see wake_waiters)
                --state->m_notify_count;
                --state->m_waiter_count;
                return;
            }
        }
    }

    sync::cv_status priv_timed_wait(waiter_state* state, sync::detail::system_duration t)
    {
        sync::detail::system_duration::native_type time_left = t.get();
        while (time_left > 0)
        {
            const boost::winapi::DWORD_ dur = time_left > boost::winapi::max_non_infinite_wait ?
                boost::winapi::max_non_infinite_wait : static_cast< boost::winapi::DWORD_ >(time_left);
            const boost::winapi::DWORD_ res = boost::winapi::WaitForSingleObject(state->m_semaphore, dur);
            switch (res)
            {
            case boost::winapi::wait_object_0: // condition variable was signalled
                {
                    boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);
                    if (state->m_notify_count > 0u)
                    {
                        // Total waiter count is already actual here (see wake_waiters)
                        --state->m_notify_count;
                        --state->m_waiter_count;
                        return sync::cv_status::no_timeout;
                    }
                }
                break;

            case boost::winapi::wait_timeout: // timeout expired
                time_left -= dur;
                break;

            default: // error occurred
                {
                    const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
                    {
                        boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);
                        --state->m_waiter_count;
                        if (state->m_notify_count > 0u)
                            --state->m_notify_count;
                        else
                            m_total_waiter_count.opaque_sub(1u, boost::memory_order_relaxed);
                    }
                    BOOST_SYNC_DETAIL_THROW(wait_error, (err)("condition_variable timed_wait failed in WaitForSingleObject"));
                }
            }
        }

        boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);
        --state->m_waiter_count;
        if (state->m_notify_count > 0u)
        {
            // Total waiter count is already actual here (see wake_waiters)
            --state->m_notify_count;
            return sync::cv_status::no_timeout;
        }
        m_total_waiter_count.opaque_sub(1u, boost::memory_order_relaxed);
        return sync::cv_status::timeout;
    }

    sync::cv_status priv_timed_wait(waiter_state* state, boost::winapi::HANDLE_ waitable_timer)
    {
        boost::winapi::HANDLE_ handles[2];
        handles[0] = state->m_semaphore;
        handles[1] = waitable_timer;

        while (true)
        {
            const boost::winapi::DWORD_ res = boost::winapi::WaitForMultipleObjects(sizeof(handles) / sizeof(*handles), handles, false, boost::winapi::infinite);
            if (BOOST_UNLIKELY(res == boost::winapi::wait_failed))
            {
                const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
                {
                    boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);
                    --state->m_waiter_count;
                    if (state->m_notify_count > 0u)
                        --state->m_notify_count;
                    else
                        m_total_waiter_count.opaque_sub(1u, boost::memory_order_relaxed);
                }
                BOOST_SYNC_DETAIL_THROW(wait_error, (err)("condition_variable timed_wait failed in WaitForMultipleObjects"));
            }

            switch (res)
            {
            case boost::winapi::wait_object_0: // condition variable was signalled
                {
                    boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);
                    if (state->m_notify_count > 0u)
                    {
                        // Total waiter count is already actual here (see wake_waiters)
                        --state->m_notify_count;
                        --state->m_waiter_count;
                        return sync::cv_status::no_timeout;
                    }
                }
                break;

            case boost::winapi::wait_object_0 + 1: // timeout has expired
                {
                    boost::sync::lock_guard< mutex_type > internal_lock(m_internal_mutex);
                    --state->m_waiter_count;
                    if (state->m_notify_count > 0u)
                    {
                        // Total waiter count is already actual here (see wake_waiters)
                        --state->m_notify_count;
                        return sync::cv_status::no_timeout;
                    }
                    m_total_waiter_count.opaque_sub(1u, boost::memory_order_relaxed);
                    return sync::cv_status::timeout;
                }
                break;

            default:
                BOOST_UNREACHABLE_RETURN(sync::cv_status::no_timeout);
            }
        }
    }
};

} // namespace windows

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_CONDITION_VARIABLES_BASIC_CONDITION_VARIABLE_WINDOWS_HPP_INCLUDED_
