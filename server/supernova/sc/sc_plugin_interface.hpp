//  interface for supercollider plugins
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

#pragma once

#include <mutex>
#include <vector>

#include "../server/audio_bus_manager.hpp"
#include "../server/node_types.hpp"
#include "../server/synth.hpp"
#include "../server/memory_pool.hpp"

#include "SC_InterfaceTable.h"
#include "SC_World.h"

#include <boost/scoped_array.hpp>

namespace nova {

class sc_done_action_handler {
public:
    void add_pause_node(server_node* node) {
        spin_lock::scoped_lock lock(cmd_lock);
        pause_nodes.push_back(node);
    }

    void add_resume_node(server_node* node) {
        spin_lock::scoped_lock lock(cmd_lock);
        resume_nodes.push_back(node);
    }

    void add_done_node(server_node* node) {
        spin_lock::scoped_lock lock(cmd_lock);
        if (std::find(done_nodes.begin(), done_nodes.end(), node) != done_nodes.end())
            return;

        done_nodes.push_back(node);
    }

    void add_freeDeep_node(abstract_group* node) {
        spin_lock::scoped_lock lock(cmd_lock);
        if (std::find(freeDeep_nodes.begin(), freeDeep_nodes.end(), node) != freeDeep_nodes.end())
            return;
        freeDeep_nodes.push_back(node);
    }

    void add_freeAll_node(abstract_group* node) {
        spin_lock::scoped_lock lock(cmd_lock);
        if (std::find(freeAll_nodes.begin(), freeAll_nodes.end(), node) != freeAll_nodes.end())
            return;
        freeAll_nodes.push_back(node);
    }

    void apply_done_actions(void);

protected:
    typedef rt_pool_allocator<server_node*> server_node_alloc;
    typedef rt_pool_allocator<abstract_group*> abstract_group_alloc;
    std::vector<server_node*, server_node_alloc> done_nodes, pause_nodes, resume_nodes;
    std::vector<abstract_group*, abstract_group_alloc> freeAll_nodes, freeDeep_nodes;

private:
    spin_lock cmd_lock; /* multiple synths can be scheduled for removal, so we need to guard this
                           later we can try different approaches like a lockfree stack or bitmask */
};

class sc_plugin_interface : public sc_done_action_handler {
public:
    void initialize(class server_arguments const& args, float* control_busses);
    void reset_sampling_rate(int sr);

    sc_plugin_interface(void) = default;

    ~sc_plugin_interface(void);

    InterfaceTable sc_interface;
    World world;

    audio_bus_manager audio_busses;

    int buf_counter(void) const { return world.mBufCounter; }

    /* @{ */
    /* audio buffer handling */
    SndBuf* allocate_buffer(uint32_t index, uint32_t frames, uint32_t channels);
    void allocate_buffer(SndBuf* buf, uint32_t frames, uint32_t channels, double samplerate);
    void buffer_read_alloc(uint32_t index, const char* filename, uint32_t start, uint32_t frames);
    void buffer_alloc_read_channels(uint32_t index, const char* filename, uint32_t start, uint32_t frames,
                                    uint32_t channel_count, const uint32_t* channel_data);
    void buffer_read(uint32_t index, const char* filename, uint32_t start_file, uint32_t frames, uint32_t start_buffer,
                     bool leave_open);
    void buffer_read_channel(uint32_t index, const char* filename, uint32_t start_file, uint32_t frames,
                             uint32_t start_buffer, bool leave_open, uint32_t channel_count,
                             const uint32_t* channel_data);

    sample* get_nrt_mirror_buffer(uint32_t index) { return world.mSndBufsNonRealTimeMirror[index].data; }

    sample* get_buffer(uint32_t index) { return world.mSndBufs[index].data; }

    SndBuf* get_buffer_struct(uint32_t index) { return world.mSndBufs + index; }

    int buffer_write(uint32_t index, const char* filename, const char* header_format, const char* sample_format,
                     uint32_t start, uint32_t frames, bool leave_open);

    void buffer_zero(uint32_t index);
    void buffer_close(uint32_t index);

    sample* buffer_generate(uint32_t index, const char* cmd_name, struct sc_msg_iter& msg);

    void increment_write_updates(uint32_t index) { world.mSndBufUpdates[index].writes++; }

    void free_buffer(uint32_t index);

    typedef std::lock_guard<std::mutex> buffer_lock_t;

    std::mutex& buffer_guard(size_t index) { return async_buffer_guards[index]; }

    bool is_realtime_synthesis() const { return world.mRealTime; }

private:
    boost::scoped_array<std::mutex> async_buffer_guards;
    /* @} */

public:
    /* copies nrt mirror to rt buffers */
    void buffer_sync(uint32_t index) noexcept;

    /* @{ */
    /* control bus handling */

private:
    void controlbus_set_unchecked(uint32_t bus, float value) {
        world.mControlBus[bus] = value;
        world.mControlBusTouched[bus] = world.mBufCounter;
    }

public:
    void controlbus_set(uint32_t bus, float value) {
        if (bus < world.mNumControlBusChannels)
            controlbus_set_unchecked(bus, value);
    }

    void controlbus_fill(uint32_t bus, size_t count, float value) {
        if (bus + count >= world.mNumControlBusChannels)
            count = world.mNumAudioBusChannels - bus;

        for (size_t i = 0; i != count; ++i)
            controlbus_set_unchecked(bus + i, value);
    }

    sample controlbus_get(uint32_t bus) {
        if (bus < world.mNumControlBusChannels)
            return world.mControlBus[bus];
        else
            return 0.f;
    }

    void controlbus_getn(uint32_t bus, size_t count, size_t* r_count, float* r_values) {
        size_t remain = count;
        if (bus + count >= world.mNumControlBusChannels)
            remain = world.mNumAudioBusChannels - bus;
        *r_count = remain;

        for (size_t i = 0; i != remain; ++i)
            r_values[i] = world.mControlBus[i];
    }
    /* @}*/
};

} /* namespace nova */
