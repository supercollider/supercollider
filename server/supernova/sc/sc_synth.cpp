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

#include <boost/align/align_up.hpp>

#include "sc_synth.hpp"
#include "sc_ugen_factory.hpp"
#include "../server/server.hpp"

namespace nova {

sc_synth::sc_synth(int node_id, sc_synth_definition_ptr const & prototype):
    abstract_synth(node_id, prototype)
{
    World const & world = sc_factory->world;
    const bool rt_synthesis = world.mRealTime;

    mNode.mWorld = &sc_factory->world;
    rgen.init((uint32_t)(uint64_t)this);

    /* initialize sc wrapper class */
    mRGen = &rgen;
    mSubsampleOffset = world.mSubsampleOffset;
    mSampleOffset = world.mSampleOffset;
    mLocalAudioBusUnit = nullptr;
    mLocalControlBusUnit = nullptr;

    localBufNum = 0;
    localMaxBufNum = 0;

    mNode.mID = node_id;

    sc_synthdef const & synthdef = *prototype;

    const size_t parameter_count = synthdef.parameter_count();
    const size_t constants_count = synthdef.constants.size();

    /* we allocate one memory chunk */
    const size_t wire_buffer_alignment = 64 * 8; // align to cache line boundaries
    const size_t alloc_size = prototype->memory_requirement();

    const size_t sample_alloc_size = world.mBufLength * synthdef.buffer_count
        + wire_buffer_alignment /* for alignment */;

    const size_t total_alloc_size = alloc_size + sample_alloc_size*sizeof(sample);

    char * raw_chunk = rt_synthesis ? (char*)rt_pool.malloc(total_alloc_size)
                                    : (char*)malloc(total_alloc_size);

    if (raw_chunk == nullptr)
        throw std::bad_alloc();

    linear_allocator allocator(raw_chunk);

    /* prepare controls */
    mNumControls = parameter_count;
    mControls     = allocator.alloc<float>(parameter_count);
    mControlRates = allocator.alloc<int>(parameter_count);
    mAudioBusOffsets = allocator.alloc<int>(parameter_count);
    mMapControls  = allocator.alloc<float*>(parameter_count);

    /* initialize controls */
    for (size_t i = 0; i != parameter_count; ++i) {
        mControls[i] = synthdef.parameters[i]; /* initial parameters */
        mMapControls[i] = &mControls[i];       /* map to control values */
        mControlRates[i] = 0;                  /* init to 0*/
        mAudioBusOffsets[i] = -1;              /* init to -1: not mapped to an audio bus yet */
    }

    /* allocate constant wires */
    mWire = allocator.alloc<Wire>(constants_count);
    for (size_t i = 0; i != synthdef.constants.size(); ++i) {
        Wire * wire = mWire + i;
        wire->mFromUnit = nullptr;
        wire->mCalcRate = 0;
        wire->mBuffer = nullptr;
        wire->mScalarValue = get_constant(i);
    }

    unit_count = prototype->unit_count();
    calc_unit_count = prototype->calc_unit_count();
    units        = allocator.alloc<Unit*>(unit_count);
    calc_units   = allocator.alloc<Unit*>(calc_unit_count + 1); // over-allocate to allow prefetching
    unit_buffers = allocator.alloc<sample>(sample_alloc_size);

    unit_buffers = static_cast<sample*>( boost::alignment::align_up( unit_buffers, wire_buffer_alignment ) );

    /* allocate unit generators */
    sc_factory->allocate_ugens(synthdef.graph.size());
    for (size_t i = 0; i != synthdef.graph.size(); ++i) {
        sc_synthdef::unit_spec_t const & spec = synthdef.graph[i];
        units[i] = spec.prototype->construct(spec, this, &sc_factory->world, allocator);
    }

    for (size_t i = 0; i != synthdef.calc_unit_indices.size(); ++i) {
        int32_t index = synthdef.calc_unit_indices[i];
        calc_units[i] = units[index];
    }

    assert((char*)mControls + alloc_size <= allocator.alloc<char>()); // ensure the memory boundaries
}

sc_synth::~sc_synth(void)
{
    assert(!initialized);
}

// FIXME: for some reason, we cannot initialize multiple synths in parallel
static spin_lock scsynth_preparation_lock;

void sc_synth::prepare(void)
{
    assert(!initialized);

    scsynth_preparation_lock.lock();
    std::for_each(units, units + unit_count, [](Unit * unit) {
        sc_ugen_def * def = reinterpret_cast<sc_ugen_def*>(unit->mUnitDef);
        def->initialize(unit);
    });
    scsynth_preparation_lock.unlock();

    initialized = true;
}


void sc_synth::finalize()
{
    if (initialized) {
        std::for_each(units, units + unit_count, [](Unit * unit) {
            sc_ugen_def * def = reinterpret_cast<sc_ugen_def*>(unit->mUnitDef);
            def->destruct(unit);
        });
    }

    sc_factory->free_ugens(unit_count);
    if (sc_factory->is_realtime_synthesis())
        rt_pool.free(mControls);
    else
        free(mControls);
    initialized = false;
}

void sc_synth::set(slot_index_t slot_index, sample val)
{
    if (slot_index >= mNumControls) {
        log("argument number out of range\n");
        return;
    }

    mControlRates[slot_index] = 0;
    mMapControls[slot_index] = &mControls[slot_index];
    mControls[slot_index] = val;
}

void sc_synth::set_control_array(slot_index_t slot_index, size_t count, sample * val)
{
    if (slot_index+count >= mNumControls)
        return;
    for (size_t i = 0; i != count; ++i)
        set(slot_index+i, val[i]);
}

float sc_synth::get(slot_index_t slot_index) const
{
    return mControls[slot_index];
}

void sc_synth::map_control_bus_control (unsigned int slot_index, int control_bus_index)
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

void sc_synth::map_control_buses_control (unsigned int slot_index, int control_bus_index, int count)
{
    if (slot_index >= mNumControls)
        return;

    int slots_to_set = std::min(count, int(mNumControls - slot_index));

    for (int i = 0; i != slots_to_set; ++i)
        map_control_bus_control(slot_index+i, control_bus_index+i);
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
        // lets AudioControl know that we've been mapped into an audio bus, so we can
        // zero the output when it's freed later.
        mAudioBusOffsets[slot_index] = audio_bus_index;
        mMapControls[slot_index] = world->mAudioBus + (audio_bus_index * world->mBufLength);
    }
}

void sc_synth::map_control_buses_audio (unsigned int slot_index, int audio_bus_index, int count)
{
    if (slot_index >= mNumControls)
        return;

    int slots_to_set = std::min(count, int(mNumControls - slot_index));

    for (int i = 0; i != slots_to_set; ++i)
        map_control_bus_audio(slot_index+i, audio_bus_index+i);
}

void sc_synth::apply_unit_cmd(const char * unit_cmd, unsigned int unit_index, struct sc_msg_iter *args)
{
    Unit * unit = units[unit_index];
    sc_ugen_def * def = reinterpret_cast<sc_ugen_def*>(unit->mUnitDef);

    def->run_unit_command(unit_cmd, unit, args);
}

void sc_synth::run(void)
{
    perform();
}

extern spin_lock log_guard;

#if defined(__GNUC__) && !defined(__APPLE__)
#define thread_local __thread
#endif

#ifdef thread_local
static thread_local std::array<char, 262144> trace_scratchpad;
#else
static std::array<char, 262144> trace_scratchpad;
#endif

struct scratchpad_printer
{
    scratchpad_printer(char * str):
        string(str), position(0)
    {
        clear();
    }

    void printf(const char *fmt, ...)
    {
        va_list vargs;
        va_start(vargs, fmt);
        printf(fmt, vargs);
    }

    const char * data(void) const
    {
        return string;
    }

    bool shouldFlush(void) const
    {
        return position + 1024 > 32768;
    }

    void clear(void)
    {
        position = 0;
        string[0] = '\0'; // zero-terminated
    }

private:
    void printf(const char *fmt, va_list vargs)
    {
        position += vsprintf(string + position, fmt, vargs);
    }

    char * string;
    int position;
};

void sc_synth::run_traced(void)
{
    trace = 0;

#ifndef thread_local
    spin_lock::scoped_lock lock (log_guard);
#endif

    scratchpad_printer printer(trace_scratchpad.data());

    printer.printf("\nTRACE %d  %s    #units: %d\n", id(), this->definition_name(), calc_unit_count);

    for (size_t calc_unit_index = 0; calc_unit_index != calc_unit_count; ++calc_unit_index) {
        Unit * unit = calc_units[calc_unit_index];

        sc_ugen_def * def = reinterpret_cast<sc_ugen_def*>(unit->mUnitDef);
        printer.printf("  unit %zd %s\n    in ", calc_unit_index, def->name());
        for (uint16_t j=0; j!=unit->mNumInputs; ++j) {
            printer.printf(" %g", unit->mInBuf[j][0]);
            if (printer.shouldFlush()) {
#ifdef thread_local
                spin_lock::scoped_lock lock (log_guard);
#endif
                log(printer.data());
                printer.clear();
            }
        }

        printer.printf("\n");

        (unit->mCalcFunc)(unit, unit->mBufLength);

        printer.printf("    out");
        for (int j=0; j<unit->mNumOutputs; ++j) {
            printer.printf(" %g", unit->mOutBuf[j][0]);
            if (printer.shouldFlush()) {
#ifdef thread_local
                spin_lock::scoped_lock lock (log_guard);
#endif
                log(printer.data());
                printer.clear();
            }
        }
        printer.printf("\n");
    }
    printer.printf("\n");

#ifdef thread_local
    spin_lock::scoped_lock lock (log_guard);
#endif
    log(printer.data());
}

} /* namespace nova */
