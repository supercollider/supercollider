// basic_semaphore_mach.hpp, mach semaphores
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_DETAIL_SEMAPHORE_BASIC_SEMAPHORE_MACH_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_SEMAPHORE_BASIC_SEMAPHORE_MACH_HPP_INCLUDED_

#include <mach/task.h>
#include <mach/semaphore.h>
#include <mach/mach_traps.h>
#include <mach/mach_init.h>

#include <boost/assert.hpp>
#include <boost/cstdint.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/detail/system_error.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/exceptions/wait_error.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync  {

namespace detail {

namespace mach {

class basic_semaphore
{
    BOOST_DELETED_FUNCTION(basic_semaphore(basic_semaphore const&))
    BOOST_DELETED_FUNCTION(basic_semaphore& operator=(basic_semaphore const&))

public:
    explicit basic_semaphore(unsigned int i = 0)
    {
        kern_return_t result = semaphore_create(mach_task_self(), &m_sem, SYNC_POLICY_FIFO, i);
        if (result != KERN_SUCCESS)
            BOOST_SYNC_DETAIL_THROW(resource_error, (sync::detail::system_ns::errc::not_enough_memory)("semaphore constructor failed in semaphore_create"));
    }

    ~basic_semaphore()
    {
        BOOST_VERIFY(semaphore_destroy(mach_task_self(), m_sem) == KERN_SUCCESS);
    }

    void post()
    {
        BOOST_VERIFY(semaphore_signal(m_sem) == KERN_SUCCESS);
    }

    void post_all()
    {
        BOOST_VERIFY(semaphore_signal_all(m_sem) == KERN_SUCCESS);
    }

    void wait()
    {
        kern_return_t result = semaphore_wait(m_sem);
        if (BOOST_UNLIKELY(result != KERN_SUCCESS))
        {
            switch (result)
            {
            case KERN_INVALID_ARGUMENT:
                BOOST_SYNC_DETAIL_THROW(wait_error, (sync::detail::system_ns::errc::invalid_argument)("semaphore::wait failed: the specified semaphore is invalid."));

            case KERN_TERMINATED:
                BOOST_SYNC_DETAIL_THROW(wait_error, (sync::detail::system_ns::errc::invalid_argument)("semaphore::wait failed: the specified semaphore has been destroyed"));

            case KERN_ABORTED:
            default:
                BOOST_SYNC_DETAIL_THROW(wait_error, (sync::detail::system_ns::errc::operation_canceled)("semaphore::wait failed: the wait operation has been aborted"));
            }
        }
    }

    bool try_wait()
    {
        const mach_timespec_t wait_time = { 0, 0 };
        return priv_timed_wait(wait_time);
    }

    bool timed_wait(sync::detail::system_duration dur)
    {
        sync::detail::system_duration::native_type time_left = dur.get();
        if (time_left < 0)
            time_left = 0;
        sync::detail::system_duration::native_type time_left_sec = time_left / sync::detail::system_duration::subsecond_fraction;
        sync::detail::system_duration::native_type time_left_subsec = time_left % sync::detail::system_duration::subsecond_fraction;

        enum
        {
            nanoseconds_fraction = 1000000000u,
            conversion_ratio = static_cast< uint64_t >(nanoseconds_fraction) >= sync::detail::system_duration::subsecond_fraction ?
                nanoseconds_fraction / sync::detail::system_duration::subsecond_fraction :
                sync::detail::system_duration::subsecond_fraction / nanoseconds_fraction
        };

        const mach_timespec_t wait_time =
        {
            static_cast< unsigned int >( time_left_sec ),
            static_cast< uint64_t >( nanoseconds_fraction ) >= sync::detail::system_duration::subsecond_fraction ?
                static_cast< clock_res_t >( time_left_subsec / conversion_ratio)
              : static_cast< clock_res_t >( time_left_subsec * conversion_ratio)
        };

        return priv_timed_wait(wait_time);
    }

    bool timed_wait(sync::detail::system_time_point const& t)
    {
        return timed_wait(t - sync::detail::system_time_point::now());
    }

    template< typename TimePoint >
    bool timed_wait(sync::detail::chrono_time_point< TimePoint > const& t)
    {
        typedef TimePoint time_point;
        typedef typename time_point::clock clock;
        typedef typename time_point::duration duration;
        time_point now = clock::now();
        while (now < t.get())
        {
            if (timed_wait(sync::detail::time_traits< duration >::to_sync_unit(t.get() - now)))
                return true;
            now = clock::now();
        }
        return false;
    }

private:
    bool priv_timed_wait(const mach_timespec_t& wait_time)
    {
        kern_return_t result = semaphore_timedwait(m_sem, wait_time);

        if (result == KERN_SUCCESS)
            return true;

        if (BOOST_UNLIKELY(result != KERN_OPERATION_TIMED_OUT))
        {
            switch (result)
            {
            case KERN_INVALID_ARGUMENT:
                BOOST_SYNC_DETAIL_THROW(wait_error, (sync::detail::system_ns::errc::invalid_argument)("semaphore::timed_wait failed: the specified semaphore is invalid."));

            case KERN_TERMINATED:
                BOOST_SYNC_DETAIL_THROW(wait_error, (sync::detail::system_ns::errc::invalid_argument)("semaphore::timed_wait failed: the specified semaphore has been destroyed"));

            case KERN_ABORTED:
            default:
                BOOST_SYNC_DETAIL_THROW(wait_error, (sync::detail::system_ns::errc::operation_canceled)("semaphore::timed_wait failed: the wait operation has been aborted"));
            }
        }

        return false;
    }

private:
    semaphore_t m_sem;
};

} // namespace mach

} // namespace detail

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_SEMAPHORE_BASIC_SEMAPHORE_MACH_HPP_INCLUDED_
