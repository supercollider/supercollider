/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   sync/condition_variables/notify_all_at_thread_exit.hpp
 *
 * \brief  This header defines the \c notify_all_at_thread_exit function.
 */

#ifndef BOOST_SYNC_CONDITION_VARIABLES_NOTIFY_ALL_AT_THREAD_EXIT_HPP_INCLUDED_
#define BOOST_SYNC_CONDITION_VARIABLES_NOTIFY_ALL_AT_THREAD_EXIT_HPP_INCLUDED_

#include <boost/assert.hpp>
#include <boost/sync/detail/link_config.hpp>
#include <boost/sync/locks/unique_lock.hpp>
#include <boost/sync/mutexes/mutex.hpp>
#include <boost/sync/condition_variables/condition_variable.hpp>
#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {

namespace sync {

namespace detail {

//! Adds a notification entry at thread termination
BOOST_SYNC_API void add_thread_exit_notify_entry(sync::mutex& mtx, sync::condition_variable& cond);

} // namespace detail

/*!
 * Schedules a notification of the condition variable at the calling thread termination. The notification
 * shall be performed with the <tt>cond.notify_all()</tt> method shortly before the thread terminates.
 *
 * The provided lock must be locked, its ownership is transferred to an external storage until the notification
 * is performed. If there are threads blocked on the \a cond object, these threads should have used the same
 * mutex object as the one referred to by the \a lock.
 *
 * \param cond Condition variable to perform notification on.
 * \param lock The lock that owns the mutex, used to block on the condition variable.
 *
 * \pre <tt>lock.owns_lock()</tt>
 * \post <tt>!lock.owns_lock()</tt>
 */
inline void notify_all_at_thread_exit(sync::condition_variable& cond, sync::unique_lock< sync::mutex > lock)
{
    BOOST_ASSERT(lock.owns_lock());
    sync::detail::add_thread_exit_notify_entry(*lock.mutex(), cond);
    lock.release();
}

} // namespace sync

} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_CONDITION_VARIABLES_NOTIFY_ALL_AT_THREAD_EXIT_HPP_INCLUDED_
