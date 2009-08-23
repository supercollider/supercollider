//  synth based on supercollider-style synthdef
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


#ifndef SERVER_SC_SYNTH_HPP
#define SERVER_SC_SYNTH_HPP

#include "sc_synth_prototype.hpp"
#include "synth.hpp"

namespace nova
{

class sc_unit
{
};

class sc_synth:
    public abstract_synth
{
    typedef std::vector<sc_unit> unit_vector;

    friend class sc_synth_prototype;

public:
    sc_synth(int node_id, sc_synth_prototype_ptr const & prototype):
        abstract_synth(node_id, prototype), unit_buffers(0)
    {}

    ~sc_synth(void)
    {
        free(unit_buffers);
    }

    void run(dsp_context const & context)
    {}

    void set(slot_index_t slot_index, sample val)
    {
    }

private:
    void allocate_unit_buffers(uint blocksize, uint audio_bufs, uint control_bufs)
    {
        audio_buffers = audio_bufs;
        size_t buffer_size = sizeof(float) * (control_bufs + blocksize * audio_bufs);
        unit_buffers = allocate(buffer_size);
        memset(unit_buffers, 0, buffer_size);
    }

    unit_vector units;
    void * unit_buffers;
    uint audio_buffers;
};

} /* namespace nova */

#endif /* SERVER_SC_SYNTH_HPP */
