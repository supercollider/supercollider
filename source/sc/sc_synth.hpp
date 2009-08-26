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


#ifndef SC_SYNTH_HPP
#define SC_SYNTH_HPP

#include "supercollider/Headers/plugin_interface/SC_Unit.h"
#include "supercollider/Headers/plugin_interface/SC_Graph.h"
#include "supercollider/Headers/plugin_interface/SC_Rate.h"
#include "supercollider/Headers/plugin_interface/SC_RGen.h"

#include "sc_synth_prototype.hpp"

#include "server/synth.hpp"


namespace nova
{

class sc_unit
{
public:
    sc_unit(struct Unit * unit = NULL):
        unit(unit)
    {}

    struct Unit * unit;
};

class sc_synth:
    public abstract_synth
{
    typedef std::vector<sc_unit> unit_vector;
    typedef sc_synthdef::graph_t graph_t;

    friend class sc_synth_prototype;

public:
    sc_synth(int node_id, sc_synth_prototype_ptr const & prototype);

    ~sc_synth(void)
    {
        free(graph.mMapControls);
        free(controls);
        free(unit_buffers);
    }

    void run(dsp_context const & context)
    {}

    void set(slot_index_t slot_index, sample val)
    {}

private:
    void allocate_unit_buffers(uint blocksize, uint audio_bufs, uint control_bufs)
    {
        size_t buffer_size = sizeof(sample) * (control_bufs + blocksize * audio_bufs);
        unit_buffers = (float*)allocate(buffer_size);
        memset(unit_buffers, 0, buffer_size);

        control_buffers = unit_buffers + (blocksize * audio_bufs);
    }

    sample get_constant(size_t index)
    {
        return static_cast<sc_synth_prototype*>(class_ptr.get())->synthdef.constants[index];
    }

    friend class sc_ugen_def;

    unit_vector units;
    sample * unit_buffers;
    sample * control_buffers;   /* in the same memory chunk as unit_buffers */

    sample * controls;

    Graph graph;

    Rate full_rate;
    Rate control_rate;
    RGen rgen;
};

} /* namespace nova */

#endif /* SC_SYNTH_HPP */
