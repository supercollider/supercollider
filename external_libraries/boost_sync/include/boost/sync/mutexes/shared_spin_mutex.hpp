/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 * (C) Copyright 2013 Tim Blechmann
 */
/*!
 * \file   sync/mutexes/shared_shared_spin_mutex.hpp
 *
 * \brief  This header defines a shared spin mutex primitive.
 */

#ifndef BOOST_SYNC_MUTEXES_SHARED_SPIN_MUTEX_HPP_INCLUDED_
#define BOOST_SYNC_MUTEXES_SHARED_SPIN_MUTEX_HPP_INCLUDED_

#if defined(BOOST_SYNC_DETAIL_DOXYGEN)

namespace boost {

namespace sync {

class shared_spin_mutex
{
public:
    /*!
     * \brief Default constructor
     *
     * Creates a mutual exclusion primitive in the unlocked state.
     */
    shared_spin_mutex() noexcept;

    /*!
     * \brief Destructor
     *
     * Destroys the mutual exclusion primitive.
     *
     * \pre The primitive is in the unlocked state.
     */
    ~shared_spin_mutex() noexcept;

    shared_spin_mutex(mutex const&) = delete;
    shared_spin_mutex& operator= (mutex const&) = delete;

    /*!
     * \brief Exclusively locks the mutex
     *
     * If the mutex is not locked, the method acquires an exclusive lock and returns. Otherwise the method blocks until the mutex is unlocked.
     */
    void lock() noexcept;

    /*!
     * \brief Attempts to exclusively lock the mutex
     *
     * If the mutex is not locked, the method acquires an exclusive lock and returns \c true. Otherwise the method returns \c false.
     */
    bool try_lock() noexcept;

    /*!
     * \brief Unlocks the mutex
     *
     * Releases the mutex that has been exclusively locked by the current thread.
     *
     * \pre The mutex is exclusively locked by the current thread.
     */
    void unlock() noexcept;

    /*!
     * \brief Shared locks the mutex
     *
     * If the mutex is not exclusively locked or shared locked, the method acquires a shared lock and returns. Otherwise the method blocks until the mutex is unlocked.
     */
    void lock_shared() noexcept;

    /*!
     * \brief Attempts to exclusively lock the mutex
     *
     * If the mutex is not exclusively locked or shared locked, the method acquires a shared lock and returns \c true. Otherwise the method returns \c false.
     */
    bool try_lock() noexcept;

    /*!
     * \brief Unlocks the mutex
     *
     * Releases the mutex that has been shared locked by the current thread.
     *
     * \pre The mutex is shared locked by the current thread.
     */
    void unlock_shared() noexcept;

};

} // namespace sync

} // namespace boost

#else // defined(BOOST_SYNC_DETAIL_DOXYGEN)

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <boost/sync/detail/mutexes/shared_spin_mutex.hpp>

#endif // defined(BOOST_SYNC_DETAIL_DOXYGEN)

#endif // BOOST_SYNC_MUTEXES_SHARED_SPIN_MUTEX_HPP_INCLUDED_
