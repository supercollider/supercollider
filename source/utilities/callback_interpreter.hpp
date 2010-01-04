//  templated callback system
//  Copyright (C) 2008, 2009 Tim Blechmann
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

#ifndef UTILITIES_CALLBACK_INTERPRETER_HPP
#define UTILITIES_CALLBACK_INTERPRETER_HPP

#include "branch_hints.hpp"
#include "callback_system.hpp"

#include "nova-tt/semaphore.hpp"

#include <boost/atomic.hpp>
#include <boost/bind.hpp>
#include <boost/checked_delete.hpp>
#include <boost/ref.hpp>
#include <boost/thread.hpp>


namespace nova
{

template <class callback_type,
          class callback_deleter = boost::checked_deleter<callback_type> >
class callback_interpreter:
    callback_system<callback_type>
{
public:
    callback_interpreter(void):
        running(true)
    {}

    void add_callback(callback_type * cb)
    {
        callback_system<callback_type>::add_callback(cb);
        sem.post();
    }

    void run(void)
    {
        for(;;)
        {
            sem.wait();
            if (unlikely(not running.load(boost::memory_order_relaxed)))
            {
                callback_system<callback_type>::run_callbacks();
                return;
            }
            callback_system<callback_type>::run_callback();
        }
    }

    void run(semaphore & sync_sem)
    {
        sync_sem.post();
        run();
    }

    void terminate(void)
    {
        running.store(false, boost::memory_order_relaxed);
        sem.post();
    }

    boost::thread start_thread(void)
    {
        semaphore sync_sem;
        semaphore_sync sync(sync_sem);
        running.store(true, boost::memory_order_release);
        return boost::thread (boost::bind(&callback_interpreter::run, this, boost::ref(sync_sem)));
    }

private:
    semaphore sem;
    boost::atomic<bool> running;
};

} /* namespace nova */

#endif /* UTILITIES_CALLBACK_INTERPRETER_HPP */
