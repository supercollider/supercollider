// semaphore.hpp, win32 semaphores
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_WINDOWS_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_WINDOWS_HPP_INCLUDED_

#include <limits.h>
#include <cstddef>
#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/detail/winapi/GetLastError.hpp>
#include <boost/detail/winapi/synchronization.hpp>
#include <boost/detail/winapi/handles.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/exceptions/overflow_error.hpp>
#include <boost/sync/exceptions/wait_error.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/waitable_timer.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class semaphore
{
    BOOST_DELETED_FUNCTION(semaphore(semaphore const&))
    BOOST_DELETED_FUNCTION(semaphore& operator=(semaphore const&))

public:
    explicit semaphore(unsigned int i = 0)
    {
        m_sem = boost::detail::winapi::create_anonymous_semaphore(NULL, i, LONG_MAX);
        if (!m_sem)
        {
            const boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(resource_error, (err)("semaphore constructor failed in CreateSemaphore"));
        }
    }

    ~semaphore() BOOST_NOEXCEPT
    {
        BOOST_VERIFY(boost::detail::winapi::CloseHandle(m_sem) != 0);
    }

    void post()
    {
        const boost::detail::winapi::BOOL_ status = boost::detail::winapi::ReleaseSemaphore(m_sem, 1, NULL);
        if (status == 0)
        {
            const boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(overflow_error, (err)("semaphore::post failed in ReleaseSemaphore"));
        }
    }

    void wait()
    {
        if (boost::detail::winapi::WaitForSingleObject(m_sem, boost::detail::winapi::infinite) != boost::detail::winapi::wait_object_0)
        {
            const boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(wait_error, (err)("semaphore::wait failed in WaitForSingleObject"));
        }
    }


    bool try_wait()
    {
        const boost::detail::winapi::DWORD_ res = boost::detail::winapi::WaitForSingleObject(m_sem, 0);
        if (res == boost::detail::winapi::wait_failed)
        {
            const boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(wait_error, (err)("semaphore::try_wait failed in WaitForSingleObject"));
        }

        return res == boost::detail::winapi::wait_object_0;
    }

    template< typename Time >
    typename enable_if_c< sync::detail::time_traits< Time >::is_specialized, bool >::type timed_wait(Time const& t)
    {
        return priv_timed_wait(sync::detail::time_traits< Time >::to_sync_unit(t));
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
    bool priv_timed_wait(sync::detail::system_duration t)
    {
        sync::detail::system_duration::native_type time_left = t.get();
        if (time_left < 0)
            time_left = 0;

        do
        {
            const boost::detail::winapi::DWORD_ dur = time_left > boost::detail::winapi::max_non_infinite_wait ?
                boost::detail::winapi::max_non_infinite_wait : static_cast< boost::detail::winapi::DWORD_ >(time_left);
            const boost::detail::winapi::DWORD_ res = boost::detail::winapi::WaitForSingleObject(m_sem, dur);
            switch (res)
            {
            case boost::detail::winapi::wait_object_0:
                return true;

            case boost::detail::winapi::wait_timeout:
                time_left -= dur;
                break;

            default:
                {
                    const boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
                    BOOST_SYNC_DETAIL_THROW(wait_error, (err)("semaphore::timed_wait failed in WaitForSingleObject"));
                }
            }
        }
        while (time_left > 0);

        return false;
    }

    bool priv_timed_wait(sync::detail::system_time_point const& t)
    {
        boost::detail::winapi::HANDLE_ handles[2];
        handles[0] = m_sem;
        handles[1] = sync::detail::windows::get_waitable_timer();

        if (!boost::detail::winapi::SetWaitableTimer(handles[1], reinterpret_cast< const boost::detail::winapi::LARGE_INTEGER_* >(&t.get()), 0, NULL, NULL, false))
        {
            const boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
            BOOST_SYNC_DETAIL_THROW(wait_error, (err)("semaphore::timed_wait failed to set a timeout"));
        }

        const boost::detail::winapi::DWORD_ res = boost::detail::winapi::WaitForMultipleObjects(sizeof(handles) / sizeof(*handles), handles, false, boost::detail::winapi::infinite);
        switch (res)
        {
        case boost::detail::winapi::wait_object_0: // semaphore was signalled
            return true;

        default:
            BOOST_ASSERT(false);

        case boost::detail::winapi::wait_object_0 + 1: // timeout has expired
            return false;

        case boost::detail::winapi::wait_failed:
            {
                const boost::detail::winapi::DWORD_ err = boost::detail::winapi::GetLastError();
                BOOST_SYNC_DETAIL_THROW(lock_error, (err)("semaphore::timed_wait failed in WaitForMultipleObjects"));
            }
        }
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
            if (priv_timed_wait(sync::detail::time_traits< duration >::to_sync_unit(t.get() - now)))
                return true;
            now = clock::now();
        }
        while (now < t.get());

        return false;
    }

private:
    boost::detail::winapi::HANDLE_ m_sem;
};

} // namespace winnt

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_WINDOWS_HPP_INCLUDED_
