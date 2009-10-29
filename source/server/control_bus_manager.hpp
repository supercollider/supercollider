//  nova server, control bus
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

#ifndef SERVER_CONTROL_BUS_MANAGER_HPP
#define SERVER_CONTROL_BUS_MANAGER_HPP

#include <boost/noncopyable.hpp>

#include "sample_types.hpp"

#include "spin_lock.hpp"

namespace nova
{

/** control bus class
 *
 *  \todo maybe we can replace the spin lock by atomic reads and writes
 * */
class control_bus
{
public:
    control_bus(sample value = sample(0)):
        value_(value)
    {}

    control_bus(control_bus const & rhs):
        value_(rhs.value_)
    {}

    sample get(void) const
    {
        spin_lock::scoped_lock lock(lock_);
        return value_;
    }

    void set(sample value)
    {
        spin_lock::scoped_lock lock(lock_);
        value_ = value;
    }

private:
    sample value_;
    mutable spin_lock lock_;
};


class control_bus_manager:
    public boost::noncopyable
{
public:
    control_bus_manager(unsigned int size = 4096):
        busses_(4092)
    {}

    void set_control_bus(unsigned int index, sample value)
    {
        busses_[index].set(value);
    }

    sample get_control_bus(unsigned int index)
    {
        return busses_[index].get();
    }

private:
    std::vector<control_bus> busses_;
};

} /* namespace nova */

#endif /* SERVER_CONTROL_BUS_MANAGER_HPP */
