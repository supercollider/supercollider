// manual_reset_event_futex.hpp, futex-based event
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013, 2019 Andrey Semashev
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
#include <boost/memory_order.hpp>
#include <boost/atomic/atomic.hpp>
#include <boost/core/enable_if.hpp>
#include <boost/sync/detail/config.hpp>
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

private:
    // State bits are divided into signalled bit and waiter count.
    enum
    {
        post_bit = 1u,
        wait_count_one = 2u,
        wait_count_mask = ~static_cast< unsigned int >(post_bit)
    };

public:
    manual_reset_event() BOOST_NOEXCEPT : m_state(0u)
    {
    }

    void set() BOOST_NOEXCEPT
    {
        unsigned int old_state = m_state.fetch_or(post_bit, boost::memory_order_release); // set state
        if ((old_state & post_bit) == 0u && (old_state & wait_count_mask) != 0u)
            sync::detail::linux_::futex_broadcast(reinterpret_cast< int* >(&m_state.value())); // wake all threads
    }

    void reset() BOOST_NOEXCEPT
    {
        m_state.opaque_and(wait_count_mask, boost::memory_order_relaxed);
    }

    void wait() BOOST_NOEXCEPT
    {
        // Try the fast path first
        if (this->try_wait())
            return;

        unsigned int old_state = m_state.add(wait_count_one, boost::memory_order_relaxed);
        while ((old_state & post_bit) == 0u)
        {
            sync::detail::linux_::futex_wait(reinterpret_cast< int* >(&m_state.value()), old_state);
            old_state = m_state.load(boost::memory_order_relaxed);
        }

        m_state.opaque_sub(wait_count_one, boost::memory_order_acquire);
    }

    bool try_wait()
    {
        return (m_state.load(boost::memory_order_acquire) & post_bit) != 0u;
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
        // Try the fast path first
        if (this->try_wait())
            return true;

        unsigned int old_state = m_state.add(wait_count_one, boost::memory_order_relaxed);
        while ((old_state & post_bit) == 0u)
        {
            sync::detail::system_duration::native_type time_left = (abs_timeout - sync::detail::system_time_point::now()).get();
            if (time_left <= 0)
            {
                m_state.opaque_sub(wait_count_one, boost::memory_order_relaxed);
                return false;
            }

            // Check that system time resolution is nanoseconds
            BOOST_STATIC_ASSERT(sync::detail::system_duration::subsecond_fraction == 1000000000u);

            const int status = sync::detail::linux_::futex_timedwait(reinterpret_cast< int* >(&m_state.value()), old_state, time_left);
            if (status != 0)
            {
                const int err = errno;
                if (BOOST_LIKELY(err == ETIMEDOUT))
                {
                    m_state.opaque_sub(wait_count_one, boost::memory_order_relaxed);
                    return false;
                }
            }

            old_state = m_state.load(boost::memory_order_relaxed);
        }

        m_state.opaque_sub(wait_count_one, boost::memory_order_acquire);

        return true;
    }

    bool priv_timed_wait(sync::detail::system_duration dur)
    {
        // Try the fast path first
        if (this->try_wait())
            return true;

        unsigned int old_state = m_state.add(wait_count_one, boost::memory_order_relaxed);
        while ((old_state & post_bit) == 0u)
        {
            sync::detail::system_duration::native_type time_left = dur.get();
            if (time_left <= 0)
            {
                m_state.opaque_sub(wait_count_one, boost::memory_order_relaxed);
                return false;
            }

            // Check that system time resolution is nanoseconds
            BOOST_STATIC_ASSERT(sync::detail::system_duration::subsecond_fraction == 1000000000u);
            do
            {
                const int status = sync::detail::linux_::futex_timedwait(reinterpret_cast< int* >(&m_state.value()), old_state, time_left);
                if (status != 0)
                {
                    const int err = errno;
                    if (BOOST_LIKELY(err == ETIMEDOUT))
                    {
                        m_state.opaque_sub(wait_count_one, boost::memory_order_relaxed);
                        return false;
                    }
                }

                old_state = m_state.load(boost::memory_order_relaxed);
            }
            while ((old_state & post_bit) == 0u);
        }

        m_state.opaque_sub(wait_count_one, boost::memory_order_acquire);

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
    boost::atomic< unsigned int > m_state;
};

} // namespace abi

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_EVENTS_MANUAL_RESET_EVENT_FUTEX_HPP_INCLUDED_
