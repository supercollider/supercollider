// auto_reset_event_futex.hpp, futex-based event
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_EVENTS_AUTO_RESET_EVENT_FUTEX_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_EVENTS_AUTO_RESET_EVENT_FUTEX_HPP_INCLUDED_

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

class auto_reset_event
{
    BOOST_DELETED_FUNCTION(auto_reset_event(auto_reset_event const&));
    BOOST_DELETED_FUNCTION(auto_reset_event& operator= (auto_reset_event const&));

private:
    // State bits are divided into post count and waiter count. Post counter is needed to wake
    // the correct number of threads blocked on the event in case if multiple concurrent posts are made.
    enum
    {
        post_count_lowest_bit = 22u,
        post_count_one = 1u << post_count_lowest_bit,
        post_count_mask = 0u - post_count_one,
        wait_count_mask = (~0u) ^ post_count_mask
    };

public:
    auto_reset_event() BOOST_NOEXCEPT : m_state(0)
    {
    }

    void post() BOOST_NOEXCEPT
    {
        unsigned int old_state = m_state.load(detail::atomic_ns::memory_order_acquire);
        unsigned int waiters, posts;
        while (true)
        {
            waiters = old_state & wait_count_mask;
            posts = old_state >> post_count_lowest_bit;
            if (waiters >= posts)
            {
                if (m_state.compare_exchange_weak(old_state, old_state + post_count_one, detail::atomic_ns::memory_order_acquire, detail::atomic_ns::memory_order_release))
                    break;

                detail::pause();
            }
            else
                return; // the event is already set (enough times so that all waiters are released and the event is still left signalled)
        }

        if (waiters > 0)
            sync::detail::linux_::futex_signal(reinterpret_cast< int* >(&m_state));
    }

    void wait() BOOST_NOEXCEPT
    {
        // Try the fast path first
        if (this->try_wait())
            return;

        // Add one waiter
        unsigned int old_state = m_state.fetch_add(1, detail::atomic_ns::memory_order_acq_rel);
        while (true)
        {
            unsigned int posts = old_state >> post_count_lowest_bit;
            if (posts == 0)
            {
            again:
                const int status = sync::detail::linux_::futex_wait(reinterpret_cast< int* >(&m_state), old_state);
                if (status != 0)
                {
                    const int err = errno;
                    switch (err)
                    {
                    case EINTR:       // signal received
                        goto again;

                    case EWOULDBLOCK: // another thread changed the state
                        break;

                    default:
                        BOOST_ASSERT(false);
                    }
                }

                old_state = m_state.load(detail::atomic_ns::memory_order_acquire);
                posts = old_state >> post_count_lowest_bit;
                if (posts == 0)
                    goto again;
            }

            // Remove one post and one waiter from the counters
            if (m_state.compare_exchange_strong(old_state, old_state - (post_count_one + 1u), detail::atomic_ns::memory_order_acquire, detail::atomic_ns::memory_order_release))
                break;
        }
    }

    bool try_wait() BOOST_NOEXCEPT
    {
        unsigned int old_state = m_state.load(detail::atomic_ns::memory_order_acquire);

        for (unsigned int posts = old_state >> post_count_lowest_bit; posts > 0; posts = old_state >> post_count_lowest_bit)
        {
            if (m_state.compare_exchange_weak(old_state, old_state - post_count_one, detail::atomic_ns::memory_order_acquire, detail::atomic_ns::memory_order_release))
                return true;

            detail::pause();
        }

        return false;
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

        // Add one waiter
        unsigned int old_state = m_state.fetch_add(1, detail::atomic_ns::memory_order_acq_rel);
        while (true)
        {
            unsigned int posts = old_state >> post_count_lowest_bit;
            if (posts == 0)
            {
            again:
                sync::detail::system_duration::native_type time_left = (abs_timeout - sync::detail::system_time_point::now()).get();
                if (time_left <= 0)
                    return on_wait_timed_out();
                const int status = sync::detail::linux_::futex_timedwait(reinterpret_cast< int* >(&m_state), old_state, time_left);
                if (status != 0)
                {
                    const int err = errno;
                    switch (err)
                    {
                    case ETIMEDOUT:
                        return on_wait_timed_out();

                    case EINTR:       // signal received
                        goto again;

                    case EWOULDBLOCK: // another thread changed the state
                        break;

                    default:
                        BOOST_ASSERT(false);
                    }
                }

                old_state = m_state.load(detail::atomic_ns::memory_order_acquire);
                posts = old_state >> post_count_lowest_bit;
                if (posts == 0)
                    goto again;
            }

            // Remove one post and one waiter from the counters
            if (m_state.compare_exchange_strong(old_state, old_state - (post_count_one + 1u), detail::atomic_ns::memory_order_acquire, detail::atomic_ns::memory_order_release))
                break;
        }

        return true;
    }

    bool priv_timed_wait(sync::detail::system_duration dur)
    {
        // Try the fast path first
        if (this->try_wait())
            return true;

        sync::detail::system_duration::native_type time_left = dur.get();
        if (time_left <= 0)
            return false;

        // Add one waiter
        unsigned int old_state = m_state.fetch_add(1, detail::atomic_ns::memory_order_acq_rel);
        while (true)
        {
            unsigned int posts = old_state >> post_count_lowest_bit;
            if (posts == 0)
            {
            again:
                const int status = sync::detail::linux_::futex_timedwait(reinterpret_cast< int* >(&m_state), old_state, time_left);
                if (status != 0)
                {
                    const int err = errno;
                    switch (err)
                    {
                    case ETIMEDOUT:
                        return on_wait_timed_out();

                    case EINTR:       // signal received
                        goto again;

                    case EWOULDBLOCK: // another thread changed the state
                        break;

                    default:
                        BOOST_ASSERT(false);
                    }
                }

                old_state = m_state.load(detail::atomic_ns::memory_order_acquire);
                posts = old_state >> post_count_lowest_bit;
                if (posts == 0)
                    goto again;
            }

            // Remove one post and one waiter from the counters
            if (m_state.compare_exchange_strong(old_state, old_state - (post_count_one + 1u), detail::atomic_ns::memory_order_acquire, detail::atomic_ns::memory_order_release))
                break;
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

    bool on_wait_timed_out()
    {
        unsigned int old_state = m_state.load(detail::atomic_ns::memory_order_acquire);
        while (true)
        {
            unsigned int posts = old_state >> post_count_lowest_bit;
            if (posts == 0)
            {
                // Remove one waiter
                if (m_state.compare_exchange_weak(old_state, old_state - 1u, detail::atomic_ns::memory_order_acquire, detail::atomic_ns::memory_order_release))
                    return false;
            }
            else
            {
                // Remove one post and one waiter from the counters
                if (m_state.compare_exchange_weak(old_state, old_state - (post_count_one + 1u), detail::atomic_ns::memory_order_acquire, detail::atomic_ns::memory_order_release))
                    return true;
            }

            detail::pause();
        }
    }

private:
    BOOST_STATIC_ASSERT_MSG(sizeof(detail::atomic_ns::atomic< unsigned int >) == sizeof(int), "Boost.Sync: unexpected size of atomic< unsigned int >");
    detail::atomic_ns::atomic< unsigned int > m_state;
};

} // namespace abi

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_EVENTS_AUTO_RESET_EVENT_FUTEX_HPP_INCLUDED_
