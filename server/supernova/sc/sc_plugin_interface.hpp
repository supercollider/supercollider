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

#include <cassert>
#include <vector>

#include "../server/audio_bus_manager.hpp"
#include "../server/node_types.hpp"
#include "../server/memory_pool.hpp"

#include "SC_InterfaceTable.h"
#include "SC_World.h"

namespace nova {

void initialize_rate(Rate& rate, double sample_rate, int blocksize);

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

    void apply_done_actions();

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

    sc_plugin_interface() = default;

    ~sc_plugin_interface();

    InterfaceTable sc_interface;
    World world;

    audio_bus_manager audio_busses;

    int buf_counter() const { return world.mBufCounter; }

    /* @{ */
    /* audio buffer handling */

    /** @brief Allocate Buffer space.
     *  @param index        the Buffer index
     *  @param frames       the number of frames
     *  @param channels     the number of channels
     *  @param samplerate   the sample rate
     *  @returns the SndBuf structor at the given index
     *  @throws std::runtime_error if the Buffer could not be allocated.
     */
    SndBuf* allocate_buffer(uint32_t index, uint32_t frames, uint32_t channels, double samplerate);

    /** @brief Allocate Buffer space.
     *
     *  variation of allocate_buffer() that takes a SndBuf* instead of a bufnum
     */
    void allocate_buffer(SndBuf& buf, uint32_t frames, uint32_t channels, double samplerate);

    /** @brief Allocate Buffer space and read a sound file.
     *  @param index    the Buffer index
     *  @param filename the file path
     *  @param start    starting frame in the soundfile
     *  @param frames   the number of frames to read. If `frames` <= 0, read until the end of the file.
     *                  We do not allocate more frames than there are in the sound file.
     *  @throws std::runtime_error if
     *          - the Buffer could not be allocated
     *          - the sound file could not be read.
     *
     *  The resulting Buffer will have the same sample rate and channel count as the sound file.
     */
    void buffer_alloc_read(uint32_t index, const char* filename, uint32_t start, int32_t frames);

    /** @brief Allocate Buffer space and read channels from a sound file.
     *  @param index          the Buffer index
     *  @param filename       the file path
     *  @param start          starting frame in the soundfile
     *  @param frames         the number of frames to read. If `frames` <= 0, read until the end of the file.
     *                        We do not allocate more frames than there are in the sound file.
     *  @param channel_count  the number of channels.
     *                        If zero, all sound files channels are read in the original order.
     *  @param channel_data   an array of channels to read from (in that order)
     *  @throws std::runtime_error if
     *          - the Buffer could not be allocated
     *          - the sound file could not be read
     *          - one of the provided channel numbers is out of range
     *
     *  The resulting Buffer will have the same sample rate as the sound file.
     */
    void buffer_alloc_read_channels(uint32_t index, const char* filename, uint32_t start, int32_t frames,
                                    uint32_t channel_count, const uint32_t* channel_data);

    /** @brief Read sound file data into an existing Buffer.
     *  @param index        the Buffer index
     *  @param filename     the file path
     *  @param start_file   starting frame in the soundfile
     *  @param frames       the number of frames to read. If `frames` < 0, read until the end of the file.
     *  @param start_buffer the starting frame in the Buffer
     *  @param leave_open   leave file open
     *                      @note if \c false, the sample rate of the sound file must match that of the Buffer.
     *
     *  @note The number of channels in the sound file must match that of the Buffer.
     *  @throws std::runtime_error if
     *          - the Buffer is not allocated
     *          - the sound file could not be read
     *          - `start_file` is out of range
     *          - `start_buffer` is out of range
     *          - the channel count of the Buffer does not match that of the sound file
     */
    void buffer_read(uint32_t index, const char* filename, uint32_t start_file, int32_t frames, uint32_t start_buffer,
                     bool leave_open);

    /** @brief Read sound file channel data into an existing buffer.
     *  @param index          the Buffer index
     *  @param filename       the file path
     *  @param start          starting frame in the soundfile
     *  @param frames         the number of frames to read. If `frames` < 0, read until the end of the file.
     *  @param start_buffer   the starting frame in the Buffer
     *  @param leave_open     leave file open.
     *                        @note if \c false, the sample rate of the sound file must match that of the Buffer.
     *  @param channel_count  the number of channels.
     *                        @note The number of channels requested must match the number of channels in the Buffer.
     *  @param channel_data   an array of channels to read from (in that order)
     *  @throws std::runtime_exception if
     *          - the Buffer is not allocated
     *          - the sound file could not be read
     *          - `start_file` is out of range
     *          - `start_buffer` is out of range
     *          - the number of requested channels does not match that of the Buffer
     *          - one of the provided channel numbers is out of range
     */
    void buffer_read_channel(uint32_t index, const char* filename, uint32_t start_file, int32_t frames,
                             uint32_t start_buffer, bool leave_open, uint32_t channel_count,
                             const uint32_t* channel_data);

    /** @brief returns the Buffer at the given index.
     *
     *  @note if index is out-of-range, returns the Buffer at index 0. See World_GetBuf().
     */
    SndBuf* get_buffer(uint32_t index) { return World_GetBuf(&world, index); }

    /** @brief returns the NRT mirror Buffer at the given index.
     *
     *  @note if index is out-of-range, returns the Buffer at index 0. See World_GetNRTBuf().
     */
    SndBuf* get_nrt_buffer(uint32_t index) { return World_GetNRTBuf(&world, index); }

    /** @brief Write sound file data.
     *  @param index          the Buffer index
     *  @param filename       the file path
     *  @param header_format  the header format string
     *  @param sample_format  the sample format string
     *  @param start          starting frame in Buffer
     *  @param frames         the number of frames to write.
     *                        If `frames` < 0, write all samples until the end of the Buffer.
     *  @param leave_open     leave file open
     *  @throws std::runtime_error if
     *          - the Buffer is not allocated
     *          - the sound file could not be written
     *          - `start` is out of range
     *
     *  The resulting sound file will have the same sample rate and channel count as the Buffer.
     */
    void buffer_write(uint32_t index, const char* filename, const char* header_format, const char* sample_format,
                      uint32_t start, int32_t frames, bool leave_open);

    /** @brief Zero the Buffer sample data at the given index. */
    void buffer_zero(uint32_t index);

    /** @brief Close the sound file of the Buffer at the given index. */
    void buffer_close(uint32_t index);

    /** @brief Free the Buffer at the given index.
     *  @note Does *not* free the Buffer data!
     */
    void buffer_free(uint32_t index);

    sample* buffer_generate(uint32_t index, const char* cmd_name, sc_msg_iter& msg);

    void increment_write_updates(uint32_t index) { world.mSndBufUpdates[index].writes++; }


    bool is_realtime_synthesis() const { return world.mRealTime; }

private:
    /* @} */

public:
    /** copies nrt mirror to rt buffer. Called on the rt thread.
     *
     *  @todo This is not really thread-safe! OSC command handlers should rather cache
     *  the NRT mirror, just like they do in scsynth. The problem is that SndBuf is not
     *  copyable because of the spinlock member... We have to wait until we finally move
     *  the spinlocks out of the SndBuf struct and into their own array.
     */
    void buffer_sync(uint32_t index) noexcept;

    /* @{ */
    /* control bus handling */
private:
    void controlbus_set_unchecked(uint32_t bus, float value) {
        assert(bus < world.mNumControlBusChannels);
        world.mControlBus[bus] = value;
        world.mControlBusTouched[bus] = world.mBufCounter;
    }

public:
    /** @brief set the value of a control bus.
     *  @param bus      the control bus index
     *  @param value    the new value
     *
     *  Fails silently if `bus` is out-of-range
     */
    void controlbus_set(uint32_t bus, float value) {
        if (bus < world.mNumControlBusChannels)
            controlbus_set_unchecked(bus, value);
    }

    /** @brief fills a range of control buses with the given value.
     *  @param bus      the control bus start index
     *  @param count    the number of consecutive control buses
     *  @param value    the new value
     *
     *  Out-of-range control buses are silently ignored.
     */
    void controlbus_fill(uint32_t bus, size_t count, float value) {
        if (bus < world.mNumControlBusChannels) {
            if (bus + count >= world.mNumControlBusChannels)
                count = world.mNumControlBusChannels - bus;

            for (size_t i = 0; i < count; ++i)
                controlbus_set_unchecked(bus + i, value);
        }
    }

    /** @brief get the value of a control bus.
     *  @param bus  the control bus index
     *  @returns the control bus value or 0.0 if `bus` is out-of-range.
     */
    sample controlbus_get(uint32_t bus) {
        if (bus < world.mNumControlBusChannels)
            return world.mControlBus[bus];
        else
            return 0.f;
    }

    /** @brief gets a range of control buses.
     *  @param bus      the control bus start index
     *  @param count    the number of consecutive control buses
     *  @param r_count  the number of return values. Always <= `count`.
     *  @param r_values the array that should receive the values.
     *
     *  Out-of-range control buses are silently ignored.
     */
    void controlbus_getn(uint32_t bus, size_t count, size_t& r_count, float* r_values) {
        if (bus < world.mNumControlBusChannels) {
            size_t remain = count;
            if (bus + count >= world.mNumControlBusChannels)
                remain = world.mNumControlBusChannels - bus;
            r_count = remain;

            assert(r_values != nullptr);
            for (size_t i = 0; i < remain; ++i)
                r_values[i] = world.mControlBus[i];
        } else {
            r_count = 0;
        }
    }
    /* @}*/
};

} /* namespace nova */
