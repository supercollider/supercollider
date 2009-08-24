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

#include "sc_synth.hpp"
#include "sc_ugen_factory.hpp"

namespace nova
{

sc_synth::sc_synth(int node_id, sc_synth_prototype_ptr const & prototype):
    abstract_synth(node_id, prototype), unit_buffers(0),
    controls(0)
{
    sc_synthdef const & synthdef = prototype->synthdef;

    /* allocate controls
     *
     * \todo control bus mappings
     *
     * */
    size_t parameter_count = synthdef.parameter_count();
    controls = allocate<float>(parameter_count);
    for (size_t i = 0; i != parameter_count; ++i)
        controls[i] = synthdef.parameters[i];

    /* allocate wire buffers
     *
     * \todo we allocate one buffer per ugen, output, which is very inefficient,
     *       but we can optimize it later
     * */
    unsigned int control_buffers, audio_buffers;
    for (graph_t::const_iterator it = synthdef.graph.begin();
         it != synthdef.graph.end(); ++it)
    {
        for (size_t i = 0; i != it->output_specs.size(); ++i)
            if (it->output_specs[i] == 2)
                ++audio_buffers;
            else
                ++control_buffers;
    }

    allocate_unit_buffers(64, audio_buffers, control_buffers);

    /* allocate unit generators */
    for (graph_t::const_iterator it = synthdef.graph.begin();
         it != synthdef.graph.end(); ++it)
    {
        sc_unit unit = ugen_factory.allocate_ugen(*it);
        units.push_back(unit);
    }
}


} /* namespace nova */
