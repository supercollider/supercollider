/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   sync/condition_variables/condition_variable_any.hpp
 *
 * \brief  This header defines a basic condition variable primitive.
 */

#ifndef BOOST_SYNC_CONDITION_VARIABLES_CONDITION_VARIABLE_ANY_HPP_INCLUDED_
#define BOOST_SYNC_CONDITION_VARIABLES_CONDITION_VARIABLE_ANY_HPP_INCLUDED_

#if defined(BOOST_SYNC_DETAIL_DOXYGEN)

namespace boost {

namespace sync {

/*!
 * \brief The condition variable class
 */
class condition_variable_any
{
public:
    /*!
     * \brief Default constructor
     *
     * \b Throws: An exception in case if the operating system is unable to create the primitive (e.g. due to insufficient resources).
     */
    condition_variable_any();

    /*!
     * \brief Destructor
     *
     * Destroys the condition variable.
     *
     * \pre There are no threads blocked on the object.
     */
    ~condition_variable_any();

    condition_variable_any(condition_variable_any const&) = delete;
    condition_variable_any& operator= (condition_variable_any const&) = delete;

    /*!
     * \brief Wakes up one thread blocked on the object
     */
    void notify_one();

    /*!
     * \brief Wakes up all threads blocked on the object
     */
    void notify_all();

    /*!
     * \brief Blocks the current thread on the object
     *
     * Atomically unlocks the lock and blocks on the object. When unblocked, it locks the lock and returns.
     * The function can unblock upon another thread calling \c notify_one(), \c notify_all() or spuriously.
     *
     * \param lock Lock object. Must be a model of \c BasicLockable.
     *
     * \b Throws: An exception in case if the operating system is unable to fulfill the request. The lock is left in the locked state in case of exception.
     */
    template< typename Lock >
    void wait(Lock& lock);

    /*!
     * \brief Blocks the current thread on the object until the predicate is satisfied
     *
     * Works equivalent to:
     *
     * <code>
     * while (!pred())
     *   wait(lock);
     * </code>
     *
     * \param lock Lock object. Must be a model of \c BasicLockable.
     * \param pred Condition predicate.
     *
     * \b Throws: An exception in case if the operating system is unable to fulfill the request. The mutex is left in the locked state in case of exception.
     */
    template< typename Lock, typename Predicate >
    void wait(Lock& lock, Predicate pred);

    /*!
     * \brief Blocks the current thread on the object until the timeout expires
     *
     * Atomically unlocks the lock and blocks on the object. When unblocked, it locks the lock and returns.
     * The function can unblock after the specified timeout expires, upon another thread calling \c notify_one(), \c notify_all() or spuriously, whichever happens first.
     * The timeout can be specified as an absolute time point or duration.
     *
     * \param lock Lock object. Must be a model of \c BasicLockable.
     * \param timeout Relative or absolute timeout. If timeout is relative, the time is measured according to the system clock.
     *
     * \return \c true if the function returned before the timeout expired, otherwise \c false.
     *
     * \b Throws: An exception in case if the operating system is unable to fulfill the request. The lock is left in the locked state in case of exception.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template< typename Lock, typename Time >
    bool timed_wait(Lock& lock, Time const& timeout);

    /*!
     * \brief Blocks the current thread on the object until the predicate is satisfied or the timeout expires
     *
     * If the timeout is an absolute time point, works equivalent to:
     *
     * <code>
     * while (!pred())
     *   if (!timed_wait(lock, timeout))
     *     return pred();
     * </code>
     *
     * otherwise works as if equivalent to:
     *
     * <code>
     * auto abs_timeout = chrono::system_clock::now() + timeout;
     * while (!pred())
     *   if (!timed_wait(lock, abs_timeout))
     *     return pred();
     * </code>
     *
     * \param lock Lock object. Must be a model of \c BasicLockable.
     * \param timeout Relative or absolute timeout. If timeout is relative, the time is measured according to the system clock.
     * \param pred Condition predicate.
     *
     * \b Throws: An exception in case if the operating system is unable to fulfill the request. The lock is left in the locked state in case of exception.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template< typename Lock, typename Time, typename Predicate >
    bool timed_wait(Lock& lock, Time const& timeout, Predicate pred);

    /*!
     * \brief Blocks the current thread on the object until the timeout expires
     *
     * Atomically unlocks the lock and blocks on the object. When unblocked, it locks the lock and returns.
     * The function can unblock after the specified timeout expires, upon another thread calling \c notify_one(), \c notify_all() or spuriously, whichever happens first.
     * The timeout must be specified as a time duration.
     *
     * \param lock Lock object. Must be a model of \c BasicLockable.
     * \param timeout Relative timeout. The time is measured according to the system clock.
     *
     * \return \c cv_status::no_timeout if the function returned before the timeout expired, otherwise \c cv_status::timeout.
     *
     * \b Throws: An exception in case if the operating system is unable to fulfill the request. The lock is left in the locked state in case of exception.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template< typename Lock, typename Duration >
    cv_status wait_for(Lock& lock, Duration const& rel_timeout);

    /*!
     * \brief Blocks the current thread on the object until the timeout expires
     *
     * Works equivalent to:
     *
     * <code>
     * return wait_until(lock, chrono::system_clock::now() + rel_timeout, pred);
     * </code>
     *
     * \param lock Lock object. Must be a model of \c BasicLockable.
     * \param timeout Relative timeout. The time is measured according to the system clock.
     * \param pred Condition predicate.
     *
     * \b Throws: An exception in case if the operating system is unable to fulfill the request. The lock is left in the locked state in case of exception.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template< typename Lock, typename Duration, typename Predicate >
    bool wait_for(Lock& lock, Duration const& rel_timeout, Predicate pred);

    /*!
     * \brief Blocks the current thread on the object until the timeout expires
     *
     * Atomically unlocks the lock and blocks on the object. When unblocked, it locks the lock and returns.
     * The function can unblock after the specified timeout expires, upon another thread calling \c notify_one(), \c notify_all() or spuriously, whichever happens first.
     * The timeout must be specified as an absolute time point.
     *
     * \param lock Lock object. Must be a model of \c BasicLockable.
     * \param timeout Absolute timeout.
     *
     * \return \c cv_status::no_timeout if the function returned before the timeout expired, otherwise \c cv_status::timeout.
     *
     * \b Throws: An exception in case if the operating system is unable to fulfill the request. The lock is left in the locked state in case of exception.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template< typename Lock, typename TimePoint >
    cv_status wait_until(Lock& lock, TimePoint const& abs_timeout);

    /*!
     * \brief Blocks the current thread on the object until the timeout expires
     *
     * Works equivalent to:
     *
     * <code>
     * while (!pred())
     *   if (wait_until(lock, timeout) == cv_status::timeout)
     *     return pred();
     * </code>
     *
     * \param lock Lock object. Must be a model of \c BasicLockable.
     * \param timeout Absolute timeout.
     * \param pred Condition predicate.
     *
     * \b Throws: An exception in case if the operating system is unable to fulfill the request. The lock is left in the locked state in case of exception.
     *
     * \note In order to use this method, a supplementary header must be included from boost/sync/support to enable support for particular time units.
     */
    template< typename Lock, typename TimePoint, typename Predicate >
    bool wait_until(Lock& lock, TimePoint const& abs_timeout, Predicate pred);
};

} // namespace sync

} // namespace boost

#else // defined(BOOST_SYNC_DETAIL_DOXYGEN)

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI)
#include <boost/sync/detail/condition_variables/condition_variable_any_windows.hpp>
#else
#include <boost/sync/detail/condition_variables/condition_variable_any_generic.hpp>
#endif

#endif // defined(BOOST_SYNC_DETAIL_DOXYGEN)

#endif // BOOST_SYNC_CONDITION_VARIABLES_CONDITION_VARIABLE_ANY_HPP_INCLUDED_
