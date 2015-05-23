/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 */
/*!
 * \file   sync/mutexes/mutex.hpp
 *
 * \brief  This header defines a basic mutex primitive.
 */

#ifndef BOOST_SYNC_MUTEXES_MUTEX_HPP_INCLUDED_
#define BOOST_SYNC_MUTEXES_MUTEX_HPP_INCLUDED_

#if defined(BOOST_SYNC_DETAIL_DOXYGEN)

namespace boost {

namespace sync {

class mutex
{
public:
    /*!
     * \brief A platform-specific type of the low level mutex implementation.
     * \note This type is only available if \c BOOST_SYNC_DEFINES_MUTEX_NATIVE_HANDLE macro is defined by the library.
     */
    typedef unspecified native_handle_type;

public:
    /*!
     * \brief Default constructor
     *
     * Creates a mutual exclusion primitive in the unlocked state.
     *
     * \b Throws: An exception in case if the operating system is unable to create the primitive (e.g. due to insufficient resources).
     */
    mutex();

    /*!
     * \brief Destructor
     *
     * Destroys the mutual exclusion primitive.
     *
     * \pre The primitive is in the unlocked state.
     */
    ~mutex();

    mutex(mutex const&) = delete;
    mutex& operator= (mutex const&) = delete;

    /*!
     * \brief Locks the mutex
     *
     * If the mutex is not locked, the method locks it and returns. Otherwise the method blocks until the mutex is unlocked.
     *
     * \b Throws: An exception in case if the operating system is unable to fulfill the request.
     */
    void lock();

    /*!
     * \brief Attempts to lock the mutex
     *
     * If the mutex is not locked, the method locks it and returns \c true. Otherwise the method returns \c false.
     *
     * \b Throws: An exception in case if the operating system is unable to fulfill the request.
     */
    bool try_lock();

    /*!
     * \brief Unlocks the mutex
     *
     * Releases the mutex that has been locked by the current thread.
     *
     * \pre The mutex is locked by the current thread.
     */
    void unlock() noexcept;

    /*!
     * \brief Returns a handle that represents a native operating system primitive that implements the mutex
     *
     * \note This method is only available if \c BOOST_SYNC_DEFINES_MUTEX_NATIVE_HANDLE macro is defined by the library.
     */
    native_handle_type native_handle() noexcept;
};

} // namespace sync

} // namespace boost

#else // defined(BOOST_SYNC_DETAIL_DOXYGEN)

#include <boost/sync/detail/config.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(BOOST_SYNC_DETAIL_PLATFORM_WINAPI)
#include <boost/sync/detail/mutexes/mutex_windows.hpp>
#else
#include <boost/sync/detail/mutexes/mutex_posix.hpp>
#endif

#endif // defined(BOOST_SYNC_DETAIL_DOXYGEN)

#endif // BOOST_SYNC_MUTEXES_MUTEX_HPP_INCLUDED_
