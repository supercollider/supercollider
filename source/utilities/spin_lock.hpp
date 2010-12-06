//  spin_lock class, wrapper for pthread's spinlock
//  Copyright (C) 2008 Tim Blechmann
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

#ifndef UTILITIES_SPIN_LOCK_HPP
#define UTILITIES_SPIN_LOCK_HPP

#include <pthread.h>
#include <cerrno>
#include <cassert>

#include <boost/noncopyable.hpp>
#include <boost/thread/locks.hpp>

namespace nova
{

class spin_lock:
    public boost::noncopyable
{
public:
    spin_lock(void)
    {
        int status = pthread_spin_init(&lock_, PTHREAD_PROCESS_PRIVATE);
        assert(status == 0);
    };

    ~spin_lock(void)
    {
        int status = pthread_spin_destroy(&lock_);
        assert(status == 0);
    }

    void lock(void)
    {
        int status = pthread_spin_lock(&lock_);
        assert(status == 0);
    }

    bool try_lock(void)
    {
        int status = pthread_spin_trylock(&lock_);

        if (status == EBUSY)
            return false;
        assert(status == 0);
        return true;
    }

    void unlock(void)
    {
        int status = pthread_spin_unlock(&lock_);
        assert(status == 0);
    }

    typedef boost::lock_guard<spin_lock> scoped_lock;

private:
    pthread_spinlock_t lock_;
};


} /* namespace nova */

#endif /* UTILITIES_SPIN_LOCK_HPP */
