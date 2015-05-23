// semaphore.hpp, posix implementation
//
// Copyright (C) 2013 Tim Blechmann
// Copyright (C) 2013 Andrey Semashev
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)


#ifndef BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_POSIX_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_POSIX_HPP_INCLUDED_

#include <errno.h>
#include <semaphore.h>

#include <boost/assert.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/sync/detail/config.hpp>
#include <boost/sync/exceptions/resource_error.hpp>
#include <boost/sync/exceptions/overflow_error.hpp>
#include <boost/sync/detail/throw_exception.hpp>
#include <boost/sync/detail/time_traits.hpp>
#include <boost/sync/detail/time_units.hpp>
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
        const int status = sem_init(&m_sem, 0, i);
        if (status)
        {
            const int err = errno;
            BOOST_SYNC_DETAIL_THROW(resource_error, (err)("boost::sync::semaphore constructor failed in sem_init"));
        }
    }

    ~semaphore() BOOST_NOEXCEPT
    {
        BOOST_VERIFY(sem_destroy(&m_sem) == 0);
    }

    void post()
    {
        const int status = sem_post(&m_sem);
        if (status == 0)
            return;

        const int err = errno;
        switch (err)
        {
        case EOVERFLOW:
            BOOST_SYNC_DETAIL_THROW(overflow_error, (err)("semaphore post failed: semaphore counter overflow"));
            break;

        case EINVAL:
            BOOST_ASSERT(false);

        default:
            break;
        }
    }

    void wait()
    {
        for (;;)
        {
            const int status = sem_wait(&m_sem);
            if (status == 0)
                return;

            switch (errno)
            {
            case EINTR: // interrupted by a signal handler
                continue;

            case EINVAL:
            default:
                // we should not reach here
                BOOST_ASSERT(false);
                return;
            }
        }
    }

    bool try_wait()
    {
        const int status = sem_trywait(&m_sem);
        if (status == 0)
            return true;

        switch (errno)
        {
        case EINTR:
        case EAGAIN:
            return false;

        case EINVAL:
        default:
            BOOST_ASSERT(false);
            return false;
        }
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
        return priv_timed_wait(sync::detail::system_time_point::now() + dur);
    }

    bool priv_timed_wait(sync::detail::system_time_point const& t)
    {
        for (;;)
        {
            const int status = sem_timedwait(&m_sem, &t.get());
            if (status == 0)
                return true;

            switch (errno)
            {
            case ETIMEDOUT:
                return false;

            case EINTR: // interrupted by a signal handler
                continue;

            case EINVAL:
            case EAGAIN:
            default:
                BOOST_ASSERT(false);
                return false;
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
        while (now < t.get())
        {
            if (priv_timed_wait(sync::detail::time_traits< duration >::to_sync_unit(t.get() - now)))
                return true;
            now = clock::now();
        }
        return false;
    }

private:
    sem_t m_sem;
};

} // namespace posix

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_SEMAPHORE_SEMAPHORE_POSIX_HPP_INCLUDED_
