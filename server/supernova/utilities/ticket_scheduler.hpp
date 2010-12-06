//  ticket-based scheduler
//  Copyright (C) 2007, 2008 Tim Blechmann
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

#ifndef TICKET_SCHEDULER_HPP
#define TICKET_SCHEDULER_HPP

#include <boost/noncopyable.hpp>
#include <boost/atomic.hpp>
#include <boost/thread/condition.hpp>
#include <boost/date_time/posix_time/ptime.hpp>

#include <cstddef>

namespace nova
{

class ticket_scheduler:
    boost::noncopyable
{
    typedef std::size_t size_t;
    typedef boost::atomic<size_t> atomic_int;

public:
    size_t get_ticket(void)
    {
        return ticket_source++;
    }

    template <class functor>
    bool run_nonblocking(size_t ticket, functor const & f)
    {
        if (ticket == current_ticket)
        {
            run_functor(f);
            return true;
        }
        return false;
    }

    template <class functor>
    void run(size_t ticket, functor const & f)
    {
        boost::mutex::scoped_lock lock(mutex);
        while (ticket != current_ticket)
        {
             /* now this is ugly ... however, just waiting for the event results in a deadlock
              * if the notification occurs before the wait operation
              * semaphores are resistant against this problem, but harder to use for a ticket
              * scheduling algorithm (would require thread-specific semaphores)
              * */
            cond.timed_wait(lock, boost::posix_time::milliseconds(100));
        }

        run_functor(f);
    }

private:
    template <class functor>
    void run_functor(functor const & f)
    {
        f();
        ++current_ticket;
        cond.notify_all();
    }

    atomic_int ticket_source;
    atomic_int current_ticket;

    boost::mutex mutex;
    boost::condition cond;
};

} /* namespace nova */


#endif /* TICKET_SCHEDULER_HPP */
