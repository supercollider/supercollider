//  nova server, audio bus
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

#ifndef SERVER_AUDIO_BUS_MANAGER_HPP
#define SERVER_AUDIO_BUS_MANAGER_HPP

#include <boost/noncopyable.hpp>

#include "sample_types.hpp"

#include "../utilities/malloc_aligned.hpp"
#include "nova-tt/rw_spinlock.hpp"

namespace nova
{

class audio_bus_manager:
    public boost::noncopyable
{
    typedef boost::uint16_t uint16_t;

public:
/*    audio_bus_manager(uint16_t count = 4096, uint16_t blocksize = 64)
    {
        initialize(count, blocksize);
    }
*/
    audio_bus_manager(void)
    {}

    void initialize(uint16_t c, uint16_t b)
    {
        count = c;
        blocksize = b;
        buffers = calloc_aligned<sample>(count * blocksize);
        locks = new padded_rw_spinlock[count];
    }

    ~audio_bus_manager(void)
    {
        free_aligned(buffers);
        delete[] locks;
    }

    sample * acquire_bus(uint16_t index)
    {
        locks[index].lock();
        return get_bus(index);
    }

    sample * get_bus(uint16_t index)
    {
        assert(index < count);
        return buffers + index * blocksize;
    }

    void release_bus(uint16_t index)
    {
        locks[index].unlock();
    }

private:
    friend class sc_plugin_interface;

    uint16_t count;
    uint16_t blocksize;
    sample * buffers;
    padded_rw_spinlock * locks;
};

} /* namespace nova */

#endif /* SERVER_AUDIO_BUS_MANAGER_HPP */
