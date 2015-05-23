// manual_reset_event_mach.hpp, mach events
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_EVENTS_MANUAL_RESET_EVENT_MACH_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_EVENTS_MANUAL_RESET_EVENT_MACH_HPP_INCLUDED_

#include <boost/cstdint.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/atomic.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/semaphore/basic_semaphore_mach.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class manual_reset_event
{
    BOOST_DELETED_FUNCTION(manual_reset_event(manual_reset_event const&))
    BOOST_DELETED_FUNCTION(manual_reset_event& operator=(manual_reset_event const&));

public:
    manual_reset_event() : m_state(0)
    {
    }

    void set()
    {
        m_state.store(1, detail::atomic_ns::memory_order_release);
        m_sem.post_all(); // wake all threads & reset semaphore count
    }

    void reset()
    {
        m_state.store(0, detail::atomic_ns::memory_order_release);
    }

    void wait() BOOST_NOEXCEPT
    {
        if (m_state.load(detail::atomic_ns::memory_order_acquire) == 1)
            return;

        m_sem.wait();
    }

    bool try_wait() BOOST_NOEXCEPT
    {
        return m_state.load(detail::atomic_ns::memory_order_acquire) == 1;
    }

    template< typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type timed_wait(Time const& t)
    {
        if (try_wait())
            return true;
        return m_sem.timed_wait(sync::detail::time_traits< Time >::to_sync_unit(t));
    }

    template< typename Duration >
    typename enable_if< detail::is_time_tag_of< Duration, detail::time_duration_tag >, bool >::type wait_for(Duration const& duration)
    {
        if (try_wait())
            return true;
        return m_sem.timed_wait(sync::detail::time_traits< Duration >::to_sync_unit(duration));
    }

    template< typename TimePoint >
    typename enable_if< detail::is_time_tag_of< TimePoint, detail::time_point_tag >, bool >::type wait_until(TimePoint const& abs_time)
    {
        if (try_wait())
            return true;
        return m_sem.timed_wait(sync::detail::time_traits< TimePoint >::to_sync_unit(abs_time));
    }

private:
    sync::detail::mach::basic_semaphore m_sem;
    sync::detail::atomic_ns::atomic< int32_t > m_state;
};

} // namespace abi

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_EVENTS_MANUAL_RESET_EVENT_MACH_HPP_INCLUDED_
