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


#pragma once

#include "SC_Unit.h"
#include "SC_Graph.h"
#include "SC_Rate.h"
#include "SC_RGen.h"
#include "SC_Wire.h"

#include "sc_synth_definition.hpp"

#include "../server/synth.hpp"
#include "../server/memory_pool.hpp"

namespace nova {

class sc_synth : public abstract_synth, public Graph {
    typedef std::vector<struct Unit*, rt_pool_allocator<void*>> unit_vector;
    typedef sc_synthdef::graph_t graph_t;

    friend class sc_synth_definition;

public:
    sc_synth(int node_id, sc_synth_definition_ptr const& prototype);

    ~sc_synth(void);

    /** run ugen constructors and initialize first sample
     *
     *  to be executed after preparing the synth and setting the controls
     */
    void prepare(void);

    void finalize(void);

    HOT inline void perform(void) {
        if (unlikely(!initialized))
            prepare();

        if (likely(trace == 0)) {
            const size_t count = calc_unit_count;
            Unit** units = calc_units;

            const size_t preroll = count & 7;

            for (size_t i = 0; i != preroll; ++i) {
                Unit* unit = units[i];
                prefetch(units[i + 1]);
                (unit->mCalcFunc)(unit, unit->mBufLength);
            }

            units += preroll;

            const size_t unroll = count / 8;
            if (unroll == 0)
                return;

            for (size_t i = 0; i != unroll; ++i) {
                Unit* unit = units[0];
                prefetch(units[1]);
                (unit->mCalcFunc)(unit, unit->mBufLength);

                unit = units[1];
                prefetch(units[2]);
                (unit->mCalcFunc)(unit, unit->mBufLength);

                unit = units[2];
                prefetch(units[3]);
                (unit->mCalcFunc)(unit, unit->mBufLength);

                unit = units[3];
                prefetch(units[4]);
                (unit->mCalcFunc)(unit, unit->mBufLength);

                unit = units[4];
                prefetch(units[5]);
                (unit->mCalcFunc)(unit, unit->mBufLength);

                unit = units[5];
                prefetch(units[6]);
                (unit->mCalcFunc)(unit, unit->mBufLength);

                unit = units[6];
                prefetch(units[7]);
                (unit->mCalcFunc)(unit, unit->mBufLength);

                unit = units[7];
                prefetch(units[8]);
                (unit->mCalcFunc)(unit, unit->mBufLength);
                units += 8;
            }
        } else
            run_traced();
    }

    void prefetch(Unit* unit) {
        char* ptr = (char*)unit;
        char* end = (char*)unit + sizeof(Unit) + 2 * sizeof(Wire) /* + 4 * sizeof(void*)*/;

        static const size_t cacheline_size = 64;

        for (; ptr < end; ptr += cacheline_size)
#ifdef __GNUC__
            __builtin_prefetch(ptr, 0, 0);
#endif
    }

    void run(void) override;

    void set(slot_index_t slot_index, sample val) override;
    float get(slot_index_t slot_index) const override;
    bool getMappedSymbol(slot_index_t slot_index, char* str) const override;
    void set_control_array(slot_index_t slot_index, size_t count, sample* val) override;

    sample get(slot_index_t slot_index) { return mControls[slot_index]; }

    /* @{ */
    /** control mapping */
private:
    void map_control_bus_control(unsigned int slot_index, int control_bus_index);
    void map_control_buses_control(unsigned int slot_index, int control_bus_index, int count);
    void map_control_bus_audio(unsigned int slot_index, int audio_bus_index);
    void map_control_buses_audio(unsigned int slot_index, int audio_bus_index, int count);

public:
    template <bool ControlBusIsAudio> void map_control_bus(unsigned int slot_index, int bus_index) {
        if (ControlBusIsAudio)
            map_control_bus_audio(slot_index, bus_index);
        else
            map_control_bus_control(slot_index, bus_index);
    }

    template <bool ControlBusIsAudio> void map_control_buses(unsigned int slot_index, int bus_index, int count) {
        if (ControlBusIsAudio)
            map_control_buses_audio(slot_index, bus_index, count);
        else
            map_control_buses_control(slot_index, bus_index, count);
    }

    template <bool ControlBusIsAudio> void map_control_bus(const char* slot_name, int bus_index) {
        int slot_index = resolve_slot(slot_name);
        if (slot_index == -1)
            return;
        map_control_bus<ControlBusIsAudio>(slot_index, bus_index);
    }

    template <bool ControlBusIsAudio> void map_control_buses(const char* slot_name, int bus_index, int count) {
        int controls_per_slot;
        int slot_index = resolve_slot_with_size(slot_name, controls_per_slot);
        if (slot_index == -1)
            return;
        count = std::min(count, controls_per_slot);
        map_control_buses<ControlBusIsAudio>(slot_index, bus_index, count);
    }

    template <bool ControlBusIsAudio>
    void map_control_bus(unsigned int slot_index, size_t arrayed_slot_index, int bus_index) {
        map_control_bus<ControlBusIsAudio>(slot_index + arrayed_slot_index, bus_index);
    }

    template <bool ControlBusIsAudio>
    void map_control_bus(const char* slot_name, size_t arrayed_slot_index, int bus_index) {
        int slot_base_index = resolve_slot(slot_name);
        if (slot_base_index == -1)
            return;
        size_t slot_index = slot_base_index + arrayed_slot_index;
        map_control_bus<ControlBusIsAudio>(slot_index, bus_index);
    }
    /* @} */

    void enable_tracing(void) { trace = 1; }

    void apply_unit_cmd(const char* unit_cmd, unsigned int unit_index, struct sc_msg_iter* args);

private:
    void run_traced(void);

    sample get_constant(size_t index) { return static_cast<sc_synth_definition*>(class_ptr.get())->constants[index]; }

    friend class sc_ugen_def;

    bool initialized = false;
    int_fast8_t trace = 0;
    Unit** calc_units;
    sample* unit_buffers = nullptr;
    int32_t calc_unit_count, unit_count;

    RGen rgen;

    Unit** units;
};

} /* namespace nova */
