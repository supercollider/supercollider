//  synth based on supercollider-style synthdef
//  Copyright (C) 2009, 2010 Tim Blechmann
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

#include <cstdio>

#include <boost/bind.hpp>

#include "sc_synth.hpp"
#include "sc_ugen_factory.hpp"

namespace nova
{

sc_synth::sc_synth(int node_id, sc_synth_prototype_ptr const & prototype):
    abstract_synth(node_id, prototype), trace(0), unit_buffers(0)
{
    World const & world = sc_factory->world;
    mNode.mWorld = &sc_factory->world;
    rgen.init((uint32_t)(uint64_t)this);

    /* initialize sc wrapper class */
    mRGen = &rgen;
    mSampleOffset = 0;
    mLocalAudioBusUnit = 0;
    mLocalControlBusUnit = 0;

    localBufNum = 0;
    localMaxBufNum = 0;

    mNode.mID = node_id;

    sc_synthdef const & synthdef = *prototype;

    const size_t parameter_count = synthdef.parameter_count();
    const size_t constants_count = synthdef.constants.size();

    /* we allocate one memory chunk */
    const size_t alloc_size = prototype->memory_requirement();

    const size_t sample_alloc_size = world.mBufLength * synthdef.buffer_count + 64;  /* allocate 64 samples more than required */

    char * chunk = (char*)rt_pool.malloc(alloc_size + sample_alloc_size*sizeof(sample));
    if (chunk == NULL)
        throw std::bad_alloc();

    /* prepare controls */
    mNumControls = parameter_count;
    mControls = (float*)chunk;     chunk += sizeof(float) * parameter_count;
    mControlRates = (int*)chunk;   chunk += sizeof(int) * parameter_count;
    mMapControls = (float**)chunk; chunk += sizeof(float*) * parameter_count;

    /* initialize controls */
    for (size_t i = 0; i != parameter_count; ++i) {
        mControls[i] = synthdef.parameters[i]; /* initial parameters */
        mMapControls[i] = &mControls[i]; /* map to control values */
        mControlRates[i] = 0;                  /* init to 0*/
    }

    /* allocate constant wires */
    mWire = (Wire*)chunk;          chunk += sizeof(Wire) * constants_count;
    for (size_t i = 0; i != synthdef.constants.size(); ++i) {
        Wire * wire = mWire + i;
        wire->mFromUnit = 0;
        wire->mCalcRate = 0;
        wire->mBuffer = 0;
        wire->mScalarValue = get_constant(i);
    }

    unit_count = prototype->unit_count();
    calc_unit_count = prototype->calc_unit_count();
    units = (Unit**)chunk; chunk += unit_count * sizeof(Unit*);
    calc_units = (Unit**)chunk; chunk += calc_unit_count * sizeof(Unit*);
    unit_buffers = (sample*)chunk; chunk += sample_alloc_size*sizeof(sample);

    unit_buffers = (sample*) ((size_t(unit_buffers) + 63) & ~63); /* next multiple of 64 */

    /* allocate unit generators */
    sc_factory->allocate_ugens(synthdef.graph.size());
    for (size_t i = 0; i != synthdef.graph.size(); ++i)
    {
        sc_synthdef::unit_spec_t const & spec = synthdef.graph[i];
        units[i] = spec.prototype->construct(spec, this, &sc_factory->world, chunk);
    }

    for (size_t i = 0; i != synthdef.calc_unit_indices.size(); ++i)
    {
        int32_t index = synthdef.calc_unit_indices[i];
        calc_units[i] = units[index];
    }

    assert((char*)mControls + alloc_size <= chunk); // ensure the memory boundaries
}

namespace
{

void free_ugen(struct Unit * unit)
{
    sc_ugen_def * def = reinterpret_cast<sc_ugen_def*>(unit->mUnitDef);
    def->destruct(unit);
}

} /* namespace */

sc_synth::~sc_synth(void)
{
    rt_pool.free(mControls);
    std::for_each(units, units + unit_count, free_ugen);

    sc_factory->free_ugens(unit_count);
}

void sc_synth::prepare(void)
{
    for (size_t i = 0; i != unit_count; ++i) {
        struct Unit * unit = units[i];
        sc_ugen_def * def = reinterpret_cast<sc_ugen_def*>(unit->mUnitDef);
        def->initialize(unit);
    }
}

void sc_synth::set(slot_index_t slot_index, sample val)
{
    if (slot_index >= mNumControls)
    {
        std::cerr << "argument number out of range" << std::endl;
        return;
    }

    mControlRates[slot_index] = 0;
    mMapControls[slot_index] = &mControls[slot_index];
    mControls[slot_index] = val;
}

void sc_synth::set(slot_index_t slot_index, size_t count, sample * val)
{
    if (slot_index+count >= mNumControls)
        return;
    for (size_t i = 0; i != count; ++i)
        set(slot_index+i, val[i]);
}


void sc_synth::map_control_bus (unsigned int slot_index, int control_bus_index)
{
    if (slot_index >= mNumControls)
        return;

    World *world = mNode.mWorld;
    if (control_bus_index < 0) {
        mControlRates[slot_index] = 0;
        mMapControls[slot_index] = mControls + slot_index;
    }
    else if (uint32(control_bus_index) < world->mNumControlBusChannels) {
        mControlRates[slot_index] = 1;
        mMapControls[slot_index] = world->mControlBus + control_bus_index;
    }
}

void sc_synth::map_control_buses (unsigned int slot_index, int control_bus_index, int count)
{
    if (slot_index >= mNumControls)
        return;

    int slots_to_set = std::min(count, int(mNumControls - slot_index));

    for (int i = 0; i != slots_to_set; ++i)
        map_control_bus(slot_index+i, control_bus_index+i);
}

void sc_synth::map_control_bus_audio (unsigned int slot_index, int audio_bus_index)
{
    if (slot_index >= mNumControls)
        return;

    World *world = mNode.mWorld;

    if (audio_bus_index < 0) {
        mControlRates[slot_index] = 0;
        mMapControls[slot_index] = mControls + slot_index;
    } else if (uint(audio_bus_index) < world->mNumAudioBusChannels) {
        mControlRates[slot_index] = 2;
        mMapControls[slot_index] = world->mAudioBus + (audio_bus_index * world->mBufLength);
    }
}

void sc_synth::map_control_buses_audio (unsigned int slot_index, int audio_bus_index, int count)
{
    if (slot_index >= mNumControls)
        return;

    int slots_to_set = std::min(count, int(mNumControls - slot_index));

    for (int i = 0; i != slots_to_set; ++i)
        map_control_bus(slot_index+i, audio_bus_index+i);
}

void sc_synth::map_control_bus (const char * slot_name, int control_bus_index)
{
    int index = resolve_slot(slot_name);
    map_control_bus(index, control_bus_index);
}

void sc_synth::map_control_buses (const char * slot_name, int control_bus_index, int count)
{
    int index = resolve_slot(slot_name);
    map_control_buses(index, control_bus_index, count);
}

void sc_synth::map_control_bus_audio (const char * slot_name, int audio_bus_index)
{
    int index = resolve_slot(slot_name);
    map_control_bus_audio(index, audio_bus_index);
}

void sc_synth::map_control_buses_audio (const char * slot_name, int audio_bus_index, int count)
{
    int index = resolve_slot(slot_name);
    map_control_buses_audio(index, audio_bus_index, count);
}

void sc_synth::apply_unit_cmd(const char * unit_cmd, unsigned int unit_index, struct sc_msg_iter *args)
{
    Unit * unit = units[unit_index];
    sc_ugen_def * def = reinterpret_cast<sc_ugen_def*>(unit->mUnitDef);

    UnitCmdFunc func = def->find_command(unit_cmd);
    (func)(unit, args);
}


void sc_synth::run(void)
{
    perform();
}

void sc_synth::run_traced(void)
{
    using namespace std;

    printf("\nTRACE %d  %s    #units: %d\n", id(), this->prototype_name(), calc_unit_count);

    for (size_t i = 0; i != calc_unit_count; ++i)
    {
        Unit * unit = calc_units[i];

        sc_ugen_def * def = reinterpret_cast<sc_ugen_def*>(unit->mUnitDef);
        printf("  unit %zd %s\n    in ", i, def->name().c_str());
        for (uint16_t j=0; j!=unit->mNumInputs; ++j)
            printf(" %g", unit->mInBuf[j][0]);
        putchar('\n');

        (unit->mCalcFunc)(unit, unit->mBufLength);

        fputs("    out", stdout);
        for (int j=0; j<unit->mNumOutputs; ++j)
            printf(" %g", unit->mOutBuf[j][0]);
        putchar('\n');
    }
    std::cout << std::endl;

    trace = 0;
}

} /* namespace nova */
