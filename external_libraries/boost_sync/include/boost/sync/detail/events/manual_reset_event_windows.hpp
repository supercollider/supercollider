// manual_reset_event_windows.hpp, win32 events
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_EVENTS_MANUAL_RESET_EVENT_WINDOWS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_EVENTS_MANUAL_RESET_EVENT_WINDOWS_HPP_INCLUDED_

#include <boost/utility/enable_if.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/events/basic_event_windows.hpp>
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
    BOOST_DELETED_FUNCTION(manual_reset_event& operator=(manual_reset_event const&))

public:
    manual_reset_event() : m_event(true)
    {
    }

    void set() { m_event.set(); }
    void reset() { m_event.reset(); }
    void wait() { m_event.wait(); }
    bool try_wait() { return m_event.try_wait(); }

    template< typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type timed_wait(Time const& timeout)
    {
        return m_event.timed_wait(sync::detail::time_traits< Time >::to_sync_unit(timeout));
    }

    template< typename Duration >
    typename enable_if< detail::is_time_tag_of< Duration, detail::time_duration_tag >, bool >::type wait_for(Duration const& duration)
    {
        return m_event.timed_wait(sync::detail::time_traits< Duration >::to_sync_unit(duration));
    }

    template< typename TimePoint >
    typename enable_if< detail::is_time_tag_of< TimePoint, detail::time_point_tag >, bool >::type wait_until(TimePoint const& abs_time)
    {
        return m_event.timed_wait(sync::detail::time_traits< TimePoint >::to_sync_unit(abs_time));
    }

private:
    sync::detail::windows::basic_event m_event;
};

} // namespace abi

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_EVENTS_MANUAL_RESET_EVENT_WINDOWS_HPP_INCLUDED_
