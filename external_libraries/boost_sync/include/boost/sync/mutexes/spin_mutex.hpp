/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 * (C) Copyright 2013 Tim Blechmann
 */
/*!
 * \file   sync/mutexes/spin_mutex.hpp
 *
 * \brief  This header defines a spin mutex primitive.
 */

#ifndef BOOST_SYNC_MUTEXES_SPIN_MUTEX_HPP_INCLUDED_
#define BOOST_SYNC_MUTEXES_SPIN_MUTEX_HPP_INCLUDED_

#if defined(BOOST_SYNC_DETAIL_DOXYGEN)

namespace boost {

namespace sync {

class spin_mutex
{
public:
    /*!
     * \brief Default constructor
     *
     * Creates a mutual exclusion primitive in the unlocked state.
     */
    spin_mutex() noexcept;

    /*!
     * \brief Destructor
     *
     * Destroys the mutual exclusion primitive.
     *
     * \pre The primitive is in the unlocked state.
     */
    ~spin_mutex() noexcept;

    spin_mutex(mutex const&) = delete;
    spin_mutex& operator= (mutex const&) = delete;

    /*!
     * \brief Locks the mutex
     *
     * If the mutex is not locked, the method locks it and returns. Otherwise the method blocks until the mutex is unlocked.
     */
    void lock() noexcept;

    /*!
     * \brief Attempts to lock the mutex
     *
     * If the mutex is not locked, the method locks it and returns \c true. Otherwise the method returns \c false.
     */
    bool try_lock() noexcept;

    /*!
     * \brief Unlocks the mutex
     *
     * Releases the mutex that has been locked by the current thread.
     *
     * \pre The mutex is locked by the current thread.
     */
    void unlock() noexcept;
};

} // namespace sync

} // namespace boost

#else // defined(BOOST_SYNC_DETAIL_DOXYGEN)

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#include <boost/sync/detail/mutexes/spin_mutex.hpp>

#endif // defined(BOOST_SYNC_DETAIL_DOXYGEN)

#endif // BOOST_SYNC_MUTEXES_SPIN_MUTEX_HPP_INCLUDED_
