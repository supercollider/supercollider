/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Tim Blechmann
 */
/*!
 * \file   auto_reset_event.hpp
 *
 * \brief  This header defines an automatically reset event.
 */

#ifndef BOOST_SYNC_EVENTS_AUTO_RESET_EVENT_HPP_INCLUDED_
#define BOOST_SYNC_EVENTS_AUTO_RESET_EVENT_HPP_INCLUDED_

#ifdef BOOST_SYNC_DETAIL_DOXYGEN

namespace boost {

namespace sync {

class auto_reset_event
{
public:
    /**
     * Constructs an event object in a non-signalled state.
     *
     * \b Throws: if an error occurs.
     */
    auto_reset_event();

    auto_reset_event(auto_reset_event const&) = delete;
    auto_reset_event& operator= (auto_reset_event const&) = delete;

    /**
     * Destroys the event object.
     *
     * \pre No threads are blocked on this object.
     */
    ~auto_reset_event();

    /**
     * If one or more threads are blocked on the event, releases one thread from its wait function. It is unspecified which thread is released.
     * After releasing the thread the object is left in a non-signalled state. If no threads are blocked on the event, the object is left in a signalled state.
     * The object remains signalled until exactly one thread completes waiting on this object. Posting an already signalled event has no effect.
     *
     * \b Memory Ordering: release
     *
     * \b Throws: if an error occurs.
     */
    void post();

    /**
     * If the event is signalled, the call makes the object non-signalled and returns. Otherwise the call blocks until the event is posted.
     * When a waiting thread is released, the state of the event is automatically reset to the non-signaled state.
     *
     * \b Memory Ordering: acquire
     *
     * \b Throws: if an error occurs.
     */
    void wait();

    /**
     * Tries to wait for the event. If successful, the state of the event is automatically reset to the non-signaled state.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \returns \c true if the event had been signalled and was reset to the non-signalled state by this call, otherwise \c false.
     *
     * \b Throws: if an error occurs.
     */
    bool try_wait();

    /**
     * If the event is signalled, the call makes the object non-signalled and returns. Otherwise the call blocks until the event is posted or the timeout expires.
     * When a waiting thread is released due to a post, the state of the event is automatically reset to the non-signaled state.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \param timeout The timeout for the operation to complete. The timeout can be specified either as an absolute time point or as a duration.
     *                In the latter case the time is measured according to the system clock.
     *
     * \returns \c true if the event had been signalled and was reset to the non-signalled state by this call, otherwise \c false.
     *
     * \b Throws: if an error occurs.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template <typename Time>
    bool timed_wait(const Time & timeout);

    /**
     * If the event is signalled, the call makes the object non-signalled and returns. Otherwise the call blocks until the event is posted or the timeout expires.
     * When a waiting thread is released due to a post, the state of the event is automatically reset to the non-signaled state.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \param duration The timeout for the operation to complete. The timeout should be a duration, which is measured according to the system clock.
     *
     * \returns \c true if the event had been signalled and was reset to the non-signalled state by this call, otherwise \c false.
     *
     * \b Throws: if an error occurs.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template <typename Duration>
    bool wait_for(const Duration & duration);

    /**
     * If the event is signalled, the call makes the object non-signalled and returns. Otherwise the call blocks until the event is posted or the timeout expires.
     * When a waiting thread is released due to a post, the state of the event is automatically reset to the non-signaled state.
     *
     * \b Memory Ordering: acquire, if successful, relaxed otherwise
     *
     * \param timeout The timeout for the operation to complete. The timeout should be an absolute time point.
     *
     * \returns \c true if the event had been signalled and was reset to the non-signalled state by this call, otherwise \c false.
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
#include <boost/sync/detail/events/auto_reset_event_windows.hpp>
#elif defined(BOOST_SYNC_DETAIL_PLATFORM_LINUX)
#include <boost/sync/detail/events/auto_reset_event_futex.hpp>
#else
// Check to see that there's support for native semaphores
#include <boost/sync/detail/semaphore_config.hpp>
#if defined(BOOST_SYNC_DETAIL_USE_POSIX_SEMAPHORES)
#include <boost/sync/detail/events/auto_reset_event_semaphore.hpp>
#else
#include <boost/sync/detail/events/auto_reset_event_emulation.hpp>
#endif
#endif

#endif // BOOST_SYNC_DETAIL_DOXYGEN

#endif // BOOST_SYNC_EVENTS_AUTO_RESET_EVENT_HPP_INCLUDED_
