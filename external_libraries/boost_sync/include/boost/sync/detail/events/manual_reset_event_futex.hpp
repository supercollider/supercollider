// manual_reset_event_futex.hpp, futex-based event
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_EVENTS_MANUAL_RESET_EVENT_FUTEX_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_EVENTS_MANUAL_RESET_EVENT_FUTEX_HPP_INCLUDED_

#include <errno.h>
#include <sys/time.h>

#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/atomic.hpp>
#include <boost/sync/detail/pause.hpp>
#include <boost/sync/detail/futex.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class manual_reset_event
{
    BOOST_DELETED_FUNCTION(manual_reset_event(manual_reset_event const&));
    BOOST_DELETED_FUNCTION(manual_reset_event& operator= (manual_reset_event const&));

public:
    manual_reset_event() BOOST_NOEXCEPT : m_state(0)
    {
    }

    void set() BOOST_NOEXCEPT
    {
        int old_state = m_state.exchange(1, detail::atomic_ns::memory_order_release); // set state
        if (old_state == 0)
            sync::detail::linux_::futex_broadcast(reinterpret_cast< int* >(&m_state)); // wake all threads
    }

    void reset() BOOST_NOEXCEPT
    {
        m_state.store(0, detail::atomic_ns::memory_order_release);
    }

    void wait() BOOST_NOEXCEPT
    {
        while (m_state.load(detail::atomic_ns::memory_order_acquire) == 0)
        {
            const int status = sync::detail::linux_::futex_wait(reinterpret_cast< int* >(&m_state), 0);
            if (status == 0)
                break;

            switch (errno)
            {
            case EINTR:       // signal received
            case EWOULDBLOCK: // another thread has reset the event
                continue;

            default:
                BOOST_ASSERT(false);
            }
        }
    }

    bool try_wait()
    {
        return m_state.load(detail::atomic_ns::memory_order_acquire) == 1;
    }

    template< typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type timed_wait(Time const& timeout)
    {
        return priv_timed_wait(sync::detail::time_traits< Time >::to_sync_unit(timeout));
    }

    template< typename Duration >
    typename enable_if< detail::is_time_tag_of< Duration, detail::time_duration_tag >, bool >::type wait_for(Duration const& duration)
    {
        return priv_timed_wait(sync::detail::time_traits< Duration >::to_sync_unit(duration));
    }

    template< typename TimePoint >
    typename enable_if< detail::is_time_tag_of< TimePoint, detail::time_point_tag >, bool >::type wait_until(TimePoint const& abs_time)
    {
        return priv_timed_wait(sync::detail::time_traits< TimePoint >::to_sync_unit(abs_time));
    }

private:
    bool priv_timed_wait(sync::detail::system_time_point const& abs_timeout)
    {
        while (m_state.load(detail::atomic_ns::memory_order_acquire) == 0)
        {
            sync::detail::system_duration::native_type time_left = (abs_timeout - sync::detail::system_time_point::now()).get();
            if (time_left <= 0)
                return false;

            // Check that system time resolution is nanoseconds
            BOOST_STATIC_ASSERT(sync::detail::system_duration::subsecond_fraction == 1000000000u);

            const int status = sync::detail::linux_::futex_timedwait(reinterpret_cast< int* >(&m_state), 0, time_left);
            if (status == 0)
                break;

            switch (errno)
            {
            case ETIMEDOUT:
                return false;

            case EINTR:       // signal received
            case EWOULDBLOCK: // another thread has reset the event
                continue;

            default:
                BOOST_ASSERT(false);
            }
        }

        return true;
    }

    bool priv_timed_wait(sync::detail::system_duration dur)
    {
        if (m_state.load(detail::atomic_ns::memory_order_acquire) == 0)
        {
            sync::detail::system_duration::native_type time_left = dur.get();
            if (time_left <= 0)
                return false;

            // Check that system time resolution is nanoseconds
            BOOST_STATIC_ASSERT(sync::detail::system_duration::subsecond_fraction == 1000000000u);
            do
            {
                const int status = sync::detail::linux_::futex_timedwait(reinterpret_cast< int* >(&m_state), 0, time_left);
                if (status == 0)
                    break;

                switch (errno)
                {
                case ETIMEDOUT:
                    return false;

                case EINTR:       // signal received
                case EWOULDBLOCK: // another thread has reset the event
                    continue;

                default:
                    BOOST_ASSERT(false);
                }
            }
            while (m_state.load(detail::atomic_ns::memory_order_acquire) == 0);
        }

        return true;
    }

    template< typename TimePoint >
    bool priv_timed_wait(sync::detail::chrono_time_point< TimePoint > const& t)
    {
        typedef TimePoint time_point;
        typedef typename time_point::clock clock;
        typedef typename time_point::duration duration;
        time_point now = clock::now();
        do
        {
            if (timed_wait(sync::detail::time_traits< duration >::to_sync_unit(t.get() - now)))
                return true;
            now = clock::now();
        }
        while (now < t.get());

        return false;
    }

private:
    BOOST_STATIC_ASSERT_MSG(sizeof(detail::atomic_ns::atomic< int >) == sizeof(int), "Boost.Sync: unexpected size of atomic< int >");
    detail::atomic_ns::atomic< int > m_state;
};

} // namespace abi

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_EVENTS_MANUAL_RESET_EVENT_FUTEX_HPP_INCLUDED_
