/*
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 *
 * (C) Copyright 2013 Tim Blechmann
 */

/*!
 * \file   detail/mutexes/shared_spin_mutex.hpp
 *
 * \brief  This header is the Boost.Sync library implementation, see the library documentation
 *         at http://www.boost.org/doc/libs/release/libs/sync/doc/html/index.html.
 */


/** \file shared_spin_mutex.hpp */

#ifndef BOOST_SYNC_DETAIL_MUTEXES_SHARED_SPIN_MUTEX_HPP_INCLUDED_
#define BOOST_SYNC_DETAIL_MUTEXES_SHARED_SPIN_MUTEX_HPP_INCLUDED_

#include <boost/cstdint.hpp>

#include <boost/sync/detail/atomic.hpp>
#include <boost/sync/detail/pause.hpp>

#include <boost/sync/detail/header.hpp>

#ifdef BOOST_HAS_PRAGMA_ONCE
#pragma once
#endif

namespace boost {
namespace sync  {

class shared_spin_mutex
{
    enum {
        unlocked_state = 0,
        reader_mask    = 0x7fffffff,
        locked_state   = 0x80000000
    };

    BOOST_DELETED_FUNCTION(shared_spin_mutex(shared_spin_mutex const&))
    BOOST_DELETED_FUNCTION(shared_spin_mutex& operator= (shared_spin_mutex const&))

public:
    shared_spin_mutex() BOOST_NOEXCEPT:
        state(uint32_t(unlocked_state))
    {}

    ~shared_spin_mutex() BOOST_NOEXCEPT
    {
        BOOST_ASSERT(state == unlocked_state);
    }

    void lock() BOOST_NOEXCEPT
    {
        for (;;) {
            while( state.load(detail::atomic_ns::memory_order_relaxed) != unlocked_state )
                detail::pause();

            uint32_t expected = unlocked_state;
            if( state.compare_exchange_weak(expected, locked_state,
                                            detail::atomic_ns::memory_order_acquire,
                                            detail::atomic_ns::memory_order_relaxed) )
                break;
        }
    }

    bool try_lock() BOOST_NOEXCEPT
    {
        uint32_t expected = unlocked_state;
        if ( state.compare_exchange_strong(expected, locked_state, detail::atomic_ns::memory_order_acquire,
                                           detail::atomic_ns::memory_order_relaxed) )
            return true;
        else
            return false;
    }

    void unlock() BOOST_NOEXCEPT
    {
        BOOST_ASSERT( state.load(detail::atomic_ns::memory_order_relaxed) == locked_state );
        state.store( unlocked_state, detail::atomic_ns::memory_order_release );
    }

    void lock_shared() BOOST_NOEXCEPT
    {
        for(;;) {
            while( state.load(detail::atomic_ns::memory_order_relaxed) == locked_state )
                detail::pause();


            /* with the mask, the cas will fail, locked exclusively */
            uint32_t current_state    = state.load(detail::atomic_ns::memory_order_acquire) & reader_mask;
            const uint32_t next_state = current_state + 1;

            if ( state.compare_exchange_weak(current_state, next_state, detail::atomic_ns::memory_order_acquire,
                                             detail::atomic_ns::memory_order_relaxed) )
                break;
            detail::pause();
        }
    }

    bool try_lock_shared() BOOST_NOEXCEPT
    {
        /* with the mask, the cas will fail, locked exclusively */
        uint32_t current_state    = state.load(detail::atomic_ns::memory_order_acquire) & reader_mask;
        const uint32_t next_state = current_state + 1;

        if ( state.compare_exchange_strong(current_state, next_state, detail::atomic_ns::memory_order_acquire,
                                           detail::atomic_ns::memory_order_relaxed) )
            return true;
        else
            return false;
    }

    void unlock_shared() BOOST_NOEXCEPT
    {
        state.fetch_sub( 1, detail::atomic_ns::memory_order_release );
    }

private:
    detail::atomic_ns::atomic<uint32_t> state;
};

} // namespace sync
} // namespace boost

#include <boost/sync/detail/footer.hpp>

#endif /* BOOST_SYNC_DETAIL_MUTEXES_SHARED_SPIN_MUTEX_HPP_INCLUDED_ */
