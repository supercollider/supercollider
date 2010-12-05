//  cpu utilization timer
//  Copyright (C) 2006, 2008 Tim Blechmann
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



#ifndef CPU_TIMER_HPP
#define CPU_TIMER_HPP

#if defined(__linux__) || defined(__APPLE__) || defined(HAVE_TIMES_H)
#define USE_TIMES_H
#include <sys/times.h>
#elif defined(WIN32)
#include <Windows.h>
#else
#error not implemented
#endif


namespace nova
{

class cpu_timer
{
public:
    void set(void)
    {
#ifdef USE_TIMES_H
        static const double inv_clock_per_ms = 1. / (CLOCKS_PER_SEC * 1000);
        tms time;
        times(&time);
        ms = (time.tms_utime + time.tms_stime) * inv_clock_per_ms;
#elif defined(WIN32)
        FILETIME ignorethis, ignorethat;

        bool status = GetProcessTimes(GetCurrentProcess(), &ignorethis, &ignorethat,
                                      &m_kerneltime, &m_usertime);
        assert(status);
#else
#error not implemented
#endif
    }

    double get(void)
    {
#ifdef USE_TIMES_H
        static const double inv_clock_per_ms = 1. / (CLOCKS_PER_SEC * 1000);
        tms time;
        times(&time);
        double now = (time.tms_utime + time.tms_stime) * inv_clock_per_ms;
        return now - ms;
#elif defined(WIN32)
        FILETIME ignorethis, ignorethat;
        FILETIME usertime, kerneltime;

        bool status = retval = GetProcessTimes(GetCurrentProcess(), &ignorethis, &ignorethat,
                                               &kerneltime, &usertime);

        assert(status);

        /* ignore high word */
        double low = ( (kerneltime.dwLowDateTime - m_kerneltime.dwLowDateTime) +
                       (usertime.dwLowDateTime - m_usertime.dwLowDateTime) ) * 0.0001;
        return low;
#else
#error not implemented
#endif
    }

#ifdef USE_TIMES_H
    double ms;
#elif defined(WIN32)
    FILETIME m_kerneltime;
    FILETIME m_usertime;
#endif
};

} /* namespace nova */

#endif /* CPU_TIMER_HPP */
