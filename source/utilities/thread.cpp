//  thread class, system specific implementation
//  Copyright (C) 2005, 2006, 2007, 2008  Tim Blechmann
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


#include "thread.hpp"

namespace nova
{
    bool SetRTPriority(int p)
    {
#if (_POSIX_PRIORITY_SCHEDULING - 0) >=  200112L
        int policy = SCHED_FIFO;
        sched_param parm;

        int minimum = sched_get_priority_min(policy);
        int maximum = sched_get_priority_max(policy);

        assert (minimum <= maximum);

        if (p > 0)
            parm.sched_priority = std::min(minimum + p, maximum);
        else
            parm.sched_priority = std::max(maximum + p, minimum);

        thread_t thread_id = GetCurrentThread();

        if (pthread_setschedparam(thread_id, policy, &parm) < 0)
            return false;
#ifdef DEBUG
        std::cerr << "raising thread priority of " << thread_id << " to " << parm.sched_priority << std::endl;
#endif  /* DEBUG */
        return true;
#else
        return false;
#endif
    }

    bool SetPriority(int p)
    {
#if (_POSIX_PRIORITY_SCHEDULING - 0) >=  200112L
        int policy = SCHED_OTHER;
        sched_param parm;

        int minimum = sched_get_priority_min(policy);
        int maximum = sched_get_priority_min(policy);

        assert (minimum <= maximum);

        if (p > 0)
            parm.sched_priority = std::min(minimum + p, maximum);
        else
            parm.sched_priority = std::max(maximum + p, minimum);

        thread_t thread_id = GetCurrentThread();

        if (int status = pthread_setschedparam(thread_id, policy, &parm))
        {
            std::cerr << "failed to raise thread priority of " << thread_id << " to " << p << " ... " << status << std::endl;
            return false;
        }
#ifdef DEBUG
        std::cerr << "raising thread priority of " << thread_id << " to " << p << std::endl;
#endif  /* DEBUG */
        return true;
#else
        return false;
#endif
    }

}
