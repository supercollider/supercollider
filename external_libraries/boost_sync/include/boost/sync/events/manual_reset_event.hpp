/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Tim Blechmann
 */
/*!
 * \file   manual_reset_event.hpp
 *
 * \brief  This header defines a manually reset event.
 */

#ifndef BOOST_SYNC_EVENTS_MANUAL_RESET_EVENT_HPP_INCLUDED_
#define BOOST_SYNC_EVENTS_MANUAL_RESET_EVENT_HPP_INCLUDED_

#ifdef BOOST_SYNC_DETAIL_DOXYGEN

namespace boost {

namespace sync {

class manual_reset_event
{
public:
    /**
     * Constructs an event object in a non-signalled state.
     *
     * \b Throws: if an error occurs.
     */
    manual_reset_event();

    manual_reset_event(manual_reset_event const&) = delete;
    manual_reset_event& operator= (manual_reset_event const&) = delete;

    /**
     * Destroys the event object.
     *
     * \pre No threads are blocked on this object.
     */
    ~manual_reset_event();

    /**
     * Sets the event object to the signalled state. While the event is set, any number of threads that are or would be blocked in one
     * of the wait functions can be released while the event is in signalled state. Setting an already signalled event has no effect.
     *
     * \b Memory Ordering: release
     *
     * \b Throws: if an error occurs.
     *
     * \note If multiple threads are waiting on the event, this call does not guarantee that all these threads will be unblocked. It is
     *       possible that some of the woken threads resets the event to the non-signalled state before the other threads are unblocked.
     */
    void set();

    /**
     * Resets the event to the non-signaled state. Resetting a non-signalled event has no effect.
     *
     * \b Throws: if an error occurs.
     */
    void reset();

    /**
     * If the event is signalled, returns. Otherwise waits for the event to become signalled.
     *
     * \b Memory Ordering: acquire
     *
     * \b Throws: if an error occurs.
     */
    void wait();

    /**
     * Attempts to wait for the event to become signalled.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \returns \c true if the event is signalled, otherwise \c false.
     *
     * \b Throws: if an error occurs.
     */
    bool try_wait();

    /**
     * If the event is signalled, the call returns. Otherwise the call blocks until the event is set to the signalled state or the timeout expires.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \param timeout The timeout for the operation to complete. The timeout can be specified either as an absolute time point or as a duration.
     *                In the latter case the time is measured according to the system clock.
     *
     * \returns \c true if the event is signalled, otherwise \c false.
     *
     * \b Throws: if an error occurs.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template <typename Time>
    bool timed_wait(const Time & timeout);

    /**
     * If the event is signalled, the call returns. Otherwise the call blocks until the event is set to the signalled state or the timeout expires.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \param duration The timeout for the operation to complete. The timeout should be a duration, which is measured according to the system clock.
     *
     * \returns \c true if the event is signalled, otherwise \c false.
     *
     * \b Throws: if an error occurs.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template <typename Duration>
    bool wait_for(const Duration & duration);

    /**
     * If the event is signalled, the call returns. Otherwise the call blocks until the event is set to the signalled state or the timeout expires.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \param timeout The timeout for the operation to complete. The timeout should be an absolute time point.
     *
     * \returns \c true if the event is signalled, otherwise \c false.
     *
     * \b Throws: if an error occurs.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template <typename TimePoint>
    bool wait_until(const TimePoint & timeout);
};

} // namespace sync

} // namespace boost

#else // BOOST_SYNC_DETAIL_DOXYGEN

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI)
#include <boost/sync/detail/events/manual_reset_event_windows.hpp>
#elif defined(BOOST_SYNC_DETAIL_PLATFORM_LINUX)
#include <boost/sync/detail/events/manual_reset_event_futex.hpp>
#elif defined(BOOST_SYNC_DETAIL_PLATFORM_MACH)
// Check to see that there's support for native semaphores
#include <boost/sync/detail/semaphore_config.hpp>
#if defined(BOOST_SYNC_DETAIL_USE_POSIX_SEMAPHORES) || defined(BOOST_SYNC_DETAIL_USE_DISPATCH_SEMAPHORES)
#include <boost/sync/detail/events/manual_reset_event_mach.hpp>
#else
#include <boost/sync/detail/events/manual_reset_event_emulation.hpp>
#endif
#else
#include <boost/sync/detail/events/manual_reset_event_emulation.hpp>
#endif

#endif // BOOST_SYNC_DETAIL_DOXYGEN

#endif // BOOST_SYNC_EVENTS_MANUAL_RESET_EVENT_HPP_INCLUDED_
