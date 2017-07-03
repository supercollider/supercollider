// semaphore.hpp, osx/ios dispatch semaphores
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_DISPATCH_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_DISPATCH_HPP_INCLUDED_

#include <time.h>
#include <cstddef>
#include <dispatch/dispatch.h>

#include <boost/cstdint.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/detail/system_error.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync  {

BOOST_SYNC_DETAIL_OPEN_ABI_NAMESPACE {

class semaphore
{
    BOOST_DELETED_FUNCTION(semaphore(semaphore const&))
    BOOST_DELETED_FUNCTION(semaphore& operator=(semaphore const&))

public:
    explicit semaphore(unsigned int i = 0)
    {
        // Note that dispatch_release requires the semaphore counter to be equal to the one specified for dispatch_semaphore_create.
        // Otherwise dispatch_release just crashes. This is probably a bug in libdispatch, but in order to work around it, we have to
        // set the counter manually in a loop, after creating the semaphore.
        m_sem = dispatch_semaphore_create(0);
        if (m_sem == NULL)
            BOOST_SYNC_DETAIL_THROW(resource_error, (sync::detail::system_ns::errc::not_enough_memory)("semaphore constructor failed in dispatch_semaphore_create"));

        for (; i > 0; --i)
            dispatch_semaphore_signal(m_sem);
    }

    ~semaphore()
    {
        dispatch_release(m_sem);
    }

    void post()
    {
        dispatch_semaphore_signal(m_sem);
    }

    void wait()
    {
        dispatch_semaphore_wait(m_sem, DISPATCH_TIME_FOREVER);
    }

    bool try_wait()
    {
        return dispatch_semaphore_wait(m_sem, DISPATCH_TIME_NOW) == 0;
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
    bool priv_timed_wait(sync::detail::system_duration dur)
    {
        dispatch_time_t timeout = DISPATCH_TIME_NOW;

        sync::detail::system_duration::native_type time_left = dur.get();
        if (time_left > 0)
        {
            enum
            {
                nanoseconds_fraction = 1000000000u,
                conversion_ratio = static_cast< uint64_t >(nanoseconds_fraction) >= sync::detail::system_duration::subsecond_fraction ?
                    nanoseconds_fraction / sync::detail::system_duration::subsecond_fraction :
                    sync::detail::system_duration::subsecond_fraction / nanoseconds_fraction
            };

            const int64_t nanoseconds_left = static_cast< uint64_t >(nanoseconds_fraction) >= sync::detail::system_duration::subsecond_fraction ?
                time_left / conversion_ratio : time_left * conversion_ratio;

            timeout = dispatch_time(DISPATCH_TIME_NOW, nanoseconds_left);
        }

        return dispatch_semaphore_wait(m_sem, timeout) == 0;
    }

    bool priv_timed_wait(sync::detail::system_time_point const& t)
    {
        struct timespec timespec_timeout = t.get();
        dispatch_time_t timeout = dispatch_walltime( &timespec_timeout, 0 );
        return dispatch_semaphore_wait(m_sem, timeout) == 0;
    }

    template< typename TimePoint >
    bool priv_timed_wait(sync::detail::chrono_time_point< TimePoint > const& t)
    {
        typedef TimePoint time_point;
        typedef typename time_point::clock clock;
        typedef typename time_point::duration duration;
        time_point now = clock::now();
        while (now < t.get())
        {
            if (priv_timed_wait(sync::detail::time_traits< duration >::to_sync_unit(t.get() - now)))
                return true;
            now = clock::now();
        }
        return false;
    }

private:
    dispatch_semaphore_t m_sem;
};

} // namespace posix

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_DISPATCH_HPP_INCLUDED_
