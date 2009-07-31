//  dummy mutex class
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

#ifndef UTILITIES_DUMMY_MUTEX_HPP
#define UTILITIES_DUMMY_MUTEX_HPP

#include <boost/noncopyable.hpp>
#include <boost/thread/locks.hpp>

namespace nova
{

/** dummy mutex class, implementing the TryMutex concept
 *
 *  all operations are noops
 * */
class dummy_mutex:
    public boost::noncopyable
{
public:
    dummy_mutex(void)
    {}

    ~dummy_mutex(void)
    {}

    void lock(void)
    {}

    bool try_lock(void)
    {
        return true;
    }

    void unlock(void)
    {}

    typedef boost::lock_guard<dummy_mutex> scoped_lock;
};

} /* namespace nova */

#endif /* UTILITIES_DUMMY_MUTEX_HPP */
