// basic_event_windows.hpp, base class for win32 events
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_EVENTS_BASIC_EVENT_WINDOWS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_EVENTS_BASIC_EVENT_WINDOWS_HPP_INCLUDED_

#include <cstddef>
#include <boost/assert.hpp>
#include <boost/winapi/get_last_error.hpp>
#include <boost/winapi/event.hpp>
#include <boost/winapi/wait.hpp>
#include <boost/winapi/waitable_timer.hpp>
#include <boost/winapi/handles.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/waitable_timer.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/exceptions/runtime_exception.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/exceptions/wait_error.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

namespace windows {

class basic_event
{
    BOOST_DELETED_FUNCTION(basic_event(basic_event const&))
    BOOST_DELETED_FUNCTION(basic_event& operator=(basic_event const&))

public:
    explicit basic_event(bool manual_reset)
    {
        m_handle = boost::winapi::create_anonymous_event(NULL, manual_reset, 0);
        if (!m_handle)
        {
            const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(resource_error, (err)("event constructor failed in CreateEvent"));
        }
    }

    ~basic_event()
    {
        BOOST_VERIFY(boost::winapi::CloseHandle(m_handle));
    }

    void set()
    {
        const boost::winapi::BOOL_ status = boost::winapi::SetEvent(m_handle);
        if (status == 0)
        {
            const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(runtime_exception, (err)("event::set failed in SetEvent"));
        }
    }

    void reset()
    {
        const boost::winapi::BOOL_ status = boost::winapi::ResetEvent(m_handle);
        if (status == 0)
        {
            const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(runtime_exception, (err)("event::reset failed in ResetEvent"));
        }
    }

    void wait()
    {
        const boost::winapi::DWORD_ res = boost::winapi::WaitForSingleObject(m_handle, boost::winapi::infinite);
        if (BOOST_UNLIKELY(res != boost::winapi::wait_object_0))
        {
            const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(wait_error, (err)("event::wait failed in WaitForSingleObject"));
        }
    }

    bool try_wait()
    {
        const boost::winapi::DWORD_ res = boost::winapi::WaitForSingleObject(m_handle, 0);
        if (res == boost::winapi::wait_object_0)
            return true;

        if (BOOST_UNLIKELY(res != boost::winapi::wait_timeout))
        {
            const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(wait_error, (err)("event::try_wait failed in WaitForSingleObject"));
        }

        return false;
    }

    bool timed_wait(sync::detail::system_duration t)
    {
        sync::detail::system_duration::native_type time_left = t.get();
        if (time_left < 0)
            time_left = 0;

        do
        {
            const boost::winapi::DWORD_ dur = time_left > boost::winapi::max_non_infinite_wait ?
                boost::winapi::max_non_infinite_wait : static_cast< boost::winapi::DWORD_ >(time_left);
            const boost::winapi::DWORD_ res = boost::winapi::WaitForSingleObject(m_handle, dur);
            switch (res)
            {
            case boost::winapi::wait_object_0: // event was signalled
                return true;

            case boost::winapi::wait_timeout: // timeout expired
                time_left -= dur;
                break;

            default: // error occurred
                {
                    const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
                    BOOST_SYNC_DETAIL_THROW(wait_error, (err)("event timed_wait failed in WaitForSingleObject"));
                }
            }
        }
        while (time_left > 0);

        return false;
    }

    bool timed_wait(sync::detail::system_time_point const& t)
    {
        boost::winapi::HANDLE_ handles[2];
        handles[0] = m_handle;
        handles[1] = sync::detail::windows::get_waitable_timer();

        if (!boost::winapi::SetWaitableTimer(handles[1], reinterpret_cast< const boost::winapi::LARGE_INTEGER_* >(&t.get()), 0, NULL, NULL, false))
        {
            const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(wait_error, (err)("event timed_wait failed to set a timeout"));
        }

        const boost::winapi::DWORD_ res = boost::winapi::WaitForMultipleObjects(sizeof(handles) / sizeof(*handles), handles, false, boost::winapi::infinite);
        if (BOOST_UNLIKELY(res == boost::winapi::wait_failed))
        {
            const boost::winapi::DWORD_ err = boost::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(wait_error, (err)("event timed_wait failed in WaitForMultipleObjects"));
        }

        switch (res)
        {
        case boost::winapi::wait_object_0: // event was signalled
            return true;

        default:
            BOOST_ASSERT(false);
        case boost::winapi::wait_object_0 + 1: // timeout has expired
            return false;
        }
    }

    template< typename TimePoint >
    bool timed_wait(sync::detail::chrono_time_point< TimePoint > const& t)
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
    boost::winapi::HANDLE_ m_handle;
};

} // namespace windows

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_EVENTS_BASIC_EVENT_WINDOWS_HPP_INCLUDED_
