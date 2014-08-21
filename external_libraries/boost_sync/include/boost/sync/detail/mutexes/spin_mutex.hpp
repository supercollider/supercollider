/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Andrey Semashev
 * (C) Copyright 2013 Tim Blechmann
 */
/*!
 * \file   detail/mutexes/spin_mutex.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */

#ifndef BOOST_SYNC_DETAIL_MUTEXES_SPIN_MUTEX_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_MUTEXES_SPIN_MUTEX_HPP_INCLUDED_

#include <boost/sync/detail/atomic.hpp>
#include <boost/sync/detail/pause.hpp>

#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

#if defined(BOOST_SYNC_USE_STD_ATOMIC) && ( ATOMIC_BOOL_LOCKFREE == 2 )
#define BOOST_SYNC_DETAIL_SPINLOCK_BOOL
#endif
#if !defined(BOOST_SYNC_USE_STD_ATOMIC) && ( BOOST_ATOMIC_BOOL_LOCKFREE == 2 )
#define BOOST_SYNC_DETAIL_SPINLOCK_BOOL
#endif

namespace boost {
namespace sync  {

#ifdef BOOST_SYNC_DETAIL_SPINLOCK_BOOL

// if we have lockfree booleans, we can make use of test-test-and-set locks
class spin_mutex
{
    static const int locked_state   = 0;
    static const int unlocked_state = 1;
    detail::atomic_ns::atomic<bool> state;

public:
    spin_mutex() BOOST_NOEXCEPT:
        state((bool)unlocked_state)
    {}

    ~spin_mutex() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( state == unlocked_state );
    }

    void lock() BOOST_NOEXCEPT
    {
        for(;;) {
            while( state.load(detail::atomic_ns::memory_order_relaxed) != (bool)unlocked_state )
                detail::pause();

            if (try_lock())
                break;
        }
    }

    bool try_lock() BOOST_NOEXCEPT
    {
        return state.exchange( locked_state, detail::atomic_ns::memory_order_acquire ) == (bool)unlocked_state;
    }

    void unlock() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( state.load(detail::atomic_ns::memory_order_relaxed) == (bool)locked_state );
        state.store( (bool)unlocked_state, detail::atomic_ns::memory_order_release );
    }

    BOOST_DELETED_FUNCTION(spin_mutex(spin_mutex const&))
    BOOST_DELETED_FUNCTION(spin_mutex& operator= (spin_mutex const&))
};

#else

// classic test-and-set lock
class spin_mutex
{
    detail::atomic_ns::atomic_flag state;

public:
    spin_mutex() BOOST_NOEXCEPT
    {
        state.clear();
    }

    ~spin_mutex() BOOST_NOEXCEPT
    {}

    void lock() BOOST_NOEXCEPT
    {
        for(;;) {
            if (try_lock())
                break;
            detail::pause();
        }
    }

    bool try_lock() BOOST_NOEXCEPT
    {
        return state.test_and_set( detail::atomic_ns::memory_order_acquire ) == false;
    }

    void unlock() BOOST_NOEXCEPT
    {
        state.clear( detail::atomic_ns::memory_order_release );
    }

    BOOST_DELETED_FUNCTION(spin_mutex(spin_mutex const&))
    BOOST_DELETED_FUNCTION(spin_mutex& operator= (spin_mutex const&))
};

#endif

} // namespace sync
} // namespace boost

#ifdef BOOST_SYNC_DETAIL_SPINLOCK_BOOL
#undef BOOST_SYNC_DETAIL_SPINLOCK_BOOL
#endif

#include <boost/sync/detail/footer.hpp>

#endif // BOOST_SYNC_DETAIL_MUTEXES_SPIN_MUTEX_HPP_INCLUDED_
