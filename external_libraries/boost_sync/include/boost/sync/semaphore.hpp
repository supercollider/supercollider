// semaphore.hpp
//
// Copyright (C) 2013 Tim Blechmann
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_SYNC_SEMAPHORE_HPP_INCLUDED_
#define BOOST_SYNC_SEMAPHORE_HPP_INCLUDED_

#ifdef BOOST_SYNC_DETAIL_DOXYGEN

namespace boost {

namespace sync  {

class semaphore
{
public:
    /**
     * Constructs a semaphore object.
     *
     * \param initial_count The initial value of the semaphore. There may be an implementation-defined upper limit for the counter value,
     *                      but it is safe to assume that values up to \c INT_MAX are supported.
     *
     * \b Throws: if an error occurs.
     */
    explicit semaphore(unsigned int initial_count = 0);

    /**
     * Destroys the object
     *
     * \pre No thread is waiting on the semaphore
     */
    ~semaphore();

    semaphore(semaphore const&) = delete;
    semaphore& operator= (semaphore const&) = delete;

    /**
     * Increments the semaphore counter. If one or multiple threads are blocked waiting for the semaphore, then one of these threads returns successfully
     * from its wait function. It is unspecified which thread is released from the wait function.
     *
     * \b Memory Ordering: release
     *
     * \note The implementation may have an upper limit for the semaphore counter, upon exceeding which the behavior is unspecified. It is safe to assume
     *       that values up to \c INT_MAX are supported.
     *
     * \b Throws: if an error occurs.
     */
    void post();

    /**
     * If the semaphore counter is greater than zero, decrements the counter and returns. If the semaphore value is not greater than zero,
     * then the calling thread blocks until it can decrement the counter.
     *
     * \b Memory Ordering: acquire
     *
     * \b Throws: if an error occurs.
     */
    void wait();

    /**
     * If the semaphore counter is greater than zero, decrements the counter and returns \c true. If the semaphore value is not greater than zero, returns \c false.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \b Throws: if an error occurs.
     */
    bool try_wait();

    /**
     * If the semaphore counter is greater than zero, decrements the counter and returns \c true. If the semaphore value is not greater than zero,
     * then the calling thread blocks until it can decrement the counter or the specified timeout expires.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \param timeout The timeout for the operation to complete. The timeout can be specified either as an absolute time point or as a duration.
     *                In the latter case the time is measured according to the system clock.
     *
     * \returns If the timeout expires, the function returns \c false. If the semaphore counter is decremented, the function returns \c true.
     *
     * \b Throws: if an error occurs.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template <class Time>
    bool timed_wait(const Time & timeout);

    /**
     * If the semaphore counter is greater than zero, decrements the counter and returns \c true. If the semaphore value is not greater than zero,
     * then the calling thread blocks until it can decrement the counter or the specified timeout expires.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \param duration The timeout for the operation to complete. The timeout should be a duration, which is measured according to the system clock.
     *
     * \returns If the timeout expires, the function returns \c false. If the semaphore counter is decremented, the function returns \c true.
     *
     * \b Throws: if an error occurs.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template <class Duration>
    bool wait_for(const Duration & duration);

    /**
     * If the semaphore counter is greater than zero, decrements the counter and returns \c true. If the semaphore value is not greater than zero,
     * then the calling thread blocks until it can decrement the counter or the specified timeout expires.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \param timeout The timeout for the operation to complete. The timeout should be an absolute time point.
     *
     * \returns If the timeout expires, the function returns \c false. If the semaphore counter is decremented, the function returns \c true.
     *
     * \b Throws: if an error occurs.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template <class TimePoint>
    bool wait_until(const TimePoint & timeout);
};

} // namespace sync

} // namespace boost

#else // BOOST_SYNC_DETAIL_DOXYGEN

#include <boost/sync/detail/semaphore_config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI)
#include <boost/sync/detail/semaphore/semaphore_windows.hpp>
#elif defined(BOOST_SYNC_DETAIL_USE_DISPATCH_SEMAPHORES)
#include <boost/sync/detail/semaphore/semaphore_dispatch.hpp>
#elif defined(BOOST_SYNC_DETAIL_USE_POSIX_SEMAPHORES)
#include <boost/sync/detail/semaphore/semaphore_posix.hpp>
#else
#include <boost/sync/detail/semaphore/semaphore_emulation.hpp>
#endif

#endif // BOOST_SYNC_DETAIL_DOXYGEN

#endif // BOOST_SYNC_SEMAPHORE_HPP_INCLUDED_
