
//  timer system
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

#ifndef TIMER_SYSTEM_HPP
#define TIMER_SYSTEM_HPP

#include "utils.hpp"
#include "boost/d_heap.hpp"

namespace nova
{

template <typename time_representation>
class timer_system
{
public:
    class timer_callback;

private:
    typedef compare_by_instance<timer_callback, std::greater_equal<timer_callback> > timer_callback_cmp;
    typedef boost::d_heap<class timer_callback*, timer_callback_cmp> timer_callback_heap;

public:
    class timer_callback
    {
    public:
        timer_callback(time_representation const & time):
            time(time)
        {}

        virtual ~timer_callback(void)
        {}

        virtual void run(void) = 0;

        bool operator>=(timer_callback const & rhs) const
        {
            return !(time < rhs.time);
        }

    protected:
        time_representation time;
        typename timer_callback_heap::pointer this_ptr;
        friend class timer_system;
    };

    /* takes ownership
     *
     * \todo: rethink: shall we leave the deallocation to the callback?
     * */
    void add_callback(timer_callback * cb)
    {
        cb->this_ptr = heap.push(cb);
    }

    void set_current_time(time_representation const & tc)
    {
        now = tc;
    }

    void set_time_diff(time_representation const & td)
    {
        diff = td;
    }

    void tick(void)
    {
        tick(now + diff);
    }

    time_representation const & current_time(void) const
    {
        return now;
    }

    void tick(time_representation const & end)
    {
        while (likely(not heap.empty()) && heap.top()->time <= end)
        {
            timer_callback * runme = heap.top();
            heap.pop();

            now = runme->time;

            runme->run();
            delete runme;
        }
        now = end;
    }

    /* returns true if timer_callback needs to be executed in the next interval */
    bool check_next_interval(time_representation const & interval) const
    {
        return check_time(now + interval);
    }

    /* returns true if timer_callback needs to be executed before end */
    bool check_time(time_representation const & end) const
    {
        if (heap.empty())
            return false;

        return heap.top()->time < end;
    }


    time_representation next_cb_time(void) const
    {
        if (heap.empty())
            return time_representation();
        return heap.top()->time;
    }

private:
    timer_callback_heap heap;
    time_representation now;
    time_representation diff;
};

} /* namespace nova */

#endif /* TIMER_SYSTEM_HPP */
