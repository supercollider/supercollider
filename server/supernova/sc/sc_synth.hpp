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


#ifndef SC_SYNTH_HPP
#define SC_SYNTH_HPP

#include "SC_Unit.h"
#include "SC_Graph.h"
#include "SC_Rate.h"
#include "SC_RGen.h"
#include "SC_Wire.h"

#include "sc_synth_prototype.hpp"

#include "../server/synth.hpp"
#include "../server/memory_pool.hpp"

namespace nova
{

class sc_synth:
    public abstract_synth,
    public Graph
{
    typedef std::vector<struct Unit*, rt_pool_allocator<void*> > unit_vector;
    typedef sc_synthdef::graph_t graph_t;

    friend class sc_synth_prototype;

public:
    sc_synth(int node_id, sc_synth_prototype_ptr const & prototype);

    ~sc_synth(void);

    /** run ugen constructors and initialize first sample
     *
     *  to be executed after preparing the synth and setting the controls
     */
    void prepare(void);

    inline void perform(void)
    {
        if (likely(trace == 0))
        {
            size_t count = calc_unit_count;
            Unit ** units = calc_units;

            size_t preroll = count & 7;

            for (size_t i = 0; i != preroll; ++i)
            {
                Unit * unit = units[i];
                (unit->mCalcFunc)(unit, unit->mBufLength);
            }

            size_t unroll = count >> 3;
            if (unroll == 0)
                return;

            units += preroll;

            for (size_t i = 0; i != unroll; ++i)
            {
                Unit * unit = units[0];
                (unit->mCalcFunc)(unit, unit->mBufLength);
                unit = units[1];
                (unit->mCalcFunc)(unit, unit->mBufLength);
                unit = units[2];
                (unit->mCalcFunc)(unit, unit->mBufLength);
                unit = units[3];
                (unit->mCalcFunc)(unit, unit->mBufLength);
                unit = units[4];
                (unit->mCalcFunc)(unit, unit->mBufLength);
                unit = units[5];
                (unit->mCalcFunc)(unit, unit->mBufLength);
                unit = units[6];
                (unit->mCalcFunc)(unit, unit->mBufLength);
                unit = units[7];
                (unit->mCalcFunc)(unit, unit->mBufLength);
                units += 8;
            }
        }
        else
            run_traced();
    }

    void run(void);

    void set(slot_index_t slot_index, sample val);
    void set_control_array(slot_index_t slot_index, size_t count, sample * val);

    sample get(slot_index_t slot_index)
    {
        return mControls[slot_index];
    }

    /* @{ */
    /** control mapping */
private:
    void map_control_bus_control(unsigned int slot_index, int control_bus_index);
    void map_control_buses_control(unsigned int slot_index, int control_bus_index, int count);
    void map_control_bus_audio(unsigned int slot_index, int audio_bus_index);
    void map_control_buses_audio(unsigned int slot_index, int audio_bus_index, int count);

public:
    template <bool ControlBusIsAudio>
    void map_control_bus(unsigned int slot_index, int bus_index)
    {
        if (ControlBusIsAudio)
            map_control_bus_audio(slot_index, bus_index);
        else
            map_control_bus_control(slot_index, bus_index);
    }

    template <bool ControlBusIsAudio>
    void map_control_buses(unsigned int slot_index, int bus_index, int count)
    {
        if (ControlBusIsAudio)
            map_control_buses_audio(slot_index, bus_index, count);
        else
            map_control_buses_control(slot_index, bus_index, count);
    }

    template <bool ControlBusIsAudio>
    void map_control_bus(const char * slot_name, int bus_index)
    {
        int slot_index = resolve_slot(slot_name);
        map_control_bus<ControlBusIsAudio>(slot_index, bus_index);
    }

    template <bool ControlBusIsAudio>
    void map_control_buses(const char * slot_name, int bus_index, int count)
    {
        int slot_index = resolve_slot(slot_name);
        map_control_buses<ControlBusIsAudio>(slot_index, bus_index, count);
    }

    template <bool ControlBusIsAudio>
    void map_control_bus(unsigned int slot_index, size_t arrayed_slot_index, int bus_index)
    {
        map_control_bus<ControlBusIsAudio>(slot_index + arrayed_slot_index, bus_index);
    }

    template <bool ControlBusIsAudio>
    void map_control_bus(const char * slot_name, size_t arrayed_slot_index, int bus_index)
    {
        size_t slot_index = resolve_slot(slot_name) + arrayed_slot_index;
        map_control_bus<ControlBusIsAudio>(slot_index, bus_index);
    }
    /* @} */

    void enable_tracing(void)
    {
        trace = 1;
    }

    void apply_unit_cmd(const char * unit_cmd, unsigned int unit_index, struct sc_msg_iter *args);

private:
    void run_traced(void);

    sample get_constant(size_t index)
    {
        return static_cast<sc_synth_prototype*>(class_ptr.get())->constants[index];
    }

    friend class sc_ugen_def;

    int_fast8_t trace;
    Unit ** calc_units;
    sample * unit_buffers;
    int32_t calc_unit_count, unit_count;

    RGen rgen;

    Unit ** units;
};

} /* namespace nova */

#endif /* SC_SYNTH_HPP */
