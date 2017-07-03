// auto_reset_event_emulation.hpp, event emulation
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_EVENTS_AUTO_RESET_EVENT_EMULATION_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_EVENTS_AUTO_RESET_EVENT_EMULATION_HPP_INCLUDED_

#include <boost/utility/enable_if.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/locks/lock_guard.hpp>
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/condition_variables/condition_variable.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#define BOOST_SYNC_AUTO_RESET_EVENT_EMULATED

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class auto_reset_event
{
    BOOST_DELETED_FUNCTION(auto_reset_event(auto_reset_event const&));
    BOOST_DELETED_FUNCTION(auto_reset_event& operator= (auto_reset_event const&));

public:
    auto_reset_event() : m_is_set(false)
    {
    }

    void post()
    {
        sync::lock_guard< sync::mutex > lock(m_mutex);
        const bool already_signaled = m_is_set;
        if (!already_signaled)
        {
            m_is_set = true;
            m_cond.notify_one();
        }
    }

    void wait()
    {
        sync::unique_lock< sync::mutex > lock(m_mutex);

        while (!m_is_set)
            m_cond.wait(lock);

        m_is_set = false;
    }

    bool try_wait()
    {
        sync::lock_guard< sync::mutex > lock(m_mutex);
        const bool res = m_is_set;
        if (res)
            m_is_set = false;
        return res;
    }

    template< typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type timed_wait(Time const& timeout)
    {
        sync::unique_lock< sync::mutex > lock(m_mutex);
        while (!m_is_set)
        {
            if (!m_cond.timed_wait(lock, timeout))
            {
                if (!m_is_set)
                    return false;
                else
                    break;
            }
        }

        m_is_set = false;
        return true;
    }

    template< typename Duration >
    typename enable_if< detail::is_time_tag_of< Duration, detail::time_duration_tag >, bool >::type wait_for(Duration const& duration)
    {
        return timed_wait(duration);
    }

    template< typename TimePoint >
    typename enable_if< detail::is_time_tag_of< TimePoint, detail::time_point_tag >, bool >::type wait_until(TimePoint const& abs_time)
    {
        return timed_wait(abs_time);
    }

private:
    sync::mutex m_mutex;
    sync::condition_variable m_cond;
    bool m_is_set;
};

} // namespace abi

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_EVENTS_AUTO_RESET_EVENT_EMULATION_HPP_INCLUDED_
