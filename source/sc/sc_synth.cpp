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

void Rate_Init(Rate *inRate, double inSampleRate, int inBufLength);

namespace nova
{

sc_synth::sc_synth(int node_id, sc_synth_prototype_ptr const & prototype):
    abstract_synth(node_id, prototype), unit_buffers(0),
    controls(0)
{
    Rate_Init(&full_rate, 44100.f, 64);
    Rate_Init(&control_rate, 44100.f/64, 1);
    rgen.init((uint32_t)(uint64_t)this);

    /* initialize sc wrapper class */
    graph.mRGen = &rgen;
    graph.mSampleOffset = 0;

    sc_synthdef const & synthdef = prototype->synthdef;

    /* allocate controls
     * */
    size_t parameter_count = synthdef.parameter_count();
    controls = allocate<float>(parameter_count);

    for (size_t i = 0; i != parameter_count; ++i)
        controls[i] = synthdef.parameters[i];

    /* prepare control mappings
     * */
    graph.mMapControls = allocate<float*>(parameter_count);

    for (size_t i = 0; i != parameter_count; ++i)
        graph.mMapControls[i] = &controls[i];

    /* allocate constant wires */
    graph.mWire = allocate<Wire>(synthdef.constants.size());
    for (size_t i = 0; i != synthdef.constants.size(); ++i) {
        Wire * wire = graph.mWire + i;
        wire->mFromUnit = 0;
        wire->mCalcRate = 0;
        wire->mBuffer = 0;
        wire->mScalarValue = get_constant(i);
    }

#if 0
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
#endif

    /* allocate unit generators */
    for (graph_t::const_iterator it = synthdef.graph.begin();
         it != synthdef.graph.end(); ++it)
    {
        sc_unit unit = ugen_factory.allocate_ugen(this, *it);
        units.push_back(unit);
    }
}

void sc_synth::run(dsp_context const & context)
{
    for (size_t i = 0; i != units.size(); ++i) {
        Unit * unit = units[i].unit;
        if (unit->mCalcRate == calc_FullRate or
            unit->mCalcRate == calc_BufRate)
            (unit->mCalcFunc)(unit, unit->mBufLength);
    }
}

} /* namespace nova */
