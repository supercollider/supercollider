//  reader-writer spinlock
//  Copyright (C) 2009 Tim Blechmann
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; see the file COPYING.  If not, write to
//  the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
//  Boston, MA 02111-1307, USA.


#ifndef RW_SPINLOCK_HPP
#define RW_SPINLOCK_HPP

#include <cerrno>
#include <cassert>

#include "boost/thread/locks.hpp"

#include "boost/lockfree/detail/cas.hpp"

#include "branch_hints.hpp"

namespace nova
{

class rw_spinlock
{
    static const uint32_t unlocked_state = 0;
    static const uint32_t locked_state = 0x80000000;
    static const uint32_t reader_mask  = 0x7fffffff;

public:
    typedef boost::lock_guard<rw_spinlock> scoped_lock;
    typedef boost::unique_lock<rw_spinlock> unique_lock;
    typedef boost::shared_lock<rw_spinlock> shared_lock;

    rw_spinlock(void):
        state(unlocked_state)
    {}

    ~rw_spinlock(void)
    {
        assert(state == unlocked_state);
    }

    void lock(void)
    {
        for(;;)
        {
            if (boost::lockfree::cas(&state, uint32_t(unlocked_state), uint32_t(locked_state)))
                return;
        }
    }

    bool try_lock(void)
    {
        if (boost::lockfree::cas(&state, uint32_t(unlocked_state), uint32_t(locked_state)))
            return true;
        else
            return false;
    }

    void unlock(void)
    {
        assert(state == locked_state);
        boost::lockfree::memory_barrier(); /* the previous write operations need to be completed */
        state = unlocked_state;
    }

    void lock_shared(void)
    {
        for(;;)
        {
            if (try_lock_shared())
                return;
        }
    }

    bool try_lock_shared(void)
    {
        const uint32_t current_state = state & reader_mask; /* with the mask, the cas will fail, locked exclusively */
        const uint32_t next_state    = current_state + 1;

        if (likely(boost::lockfree::cas(&state, current_state, next_state)))
            return true;
        else
            return false;
    }

    void unlock_shared(void)
    {
        for(;;)
        {
            const uint32_t current_state = state; /* we don't need the reader_mask */
            const uint32_t next_state    = current_state - 1;

            if (likely(boost::lockfree::cas(&state, current_state, next_state)))
                return;
        }
    }

private:
    uint32_t state;
};

} /* namespace nova */

#endif /* RW_SPINLOCK_HPP */
