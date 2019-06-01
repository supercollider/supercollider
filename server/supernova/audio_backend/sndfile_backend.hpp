//  file-based backend (via libsndfile)
//  Copyright (C) 2010-2013 Tim Blechmann
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

#include <atomic>
#include <chrono>
#include <cmath>
#include <deque>
#include <string>
#include <thread>
#include <vector>

#include <boost/align/aligned_allocator.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/sync/semaphore.hpp>

#include "SC_SndFileHelpers.hpp"

#include "nova-tt/name_thread.hpp"
#include "utilities/branch_hints.hpp"

#include "audio_backend_common.hpp"

namespace nova {

/** sndfile backend
 *
 *  audio backend, reading/writing sound files via libsndfile
 *
 */
template <typename engine_functor, typename sample_type = float, bool blocking = false>
class sndfile_backend : public detail::audio_backend_base<sample_type, float, blocking, false>,
                        public detail::audio_settings_basic,
                        private engine_functor {
    typedef detail::audio_backend_base<sample_type, float, blocking, false> super;
    typedef std::size_t size_t;

    static const size_t queue_size = 10 * 1024 * 1024; // 30 MB

public:
    sndfile_backend(void): read_frames(queue_size), write_frames(queue_size) {}

    size_t get_audio_blocksize(void) const { return block_size_; }

    std::vector<sample_type> const& get_peaks() const { return max_peaks; }

public:
    void open_client(std::string const& input_file_name, std::string const& output_file_name, float samplerate,
                     int format, uint32_t output_channel_count, size_t block_size) {
        output_channels = output_channel_count;
        max_peaks.assign(output_channels, 0);
        samplerate_ = samplerate = std::floor(samplerate);
        block_size_ = block_size;

        if (!input_file_name.empty()) {
            input_file = makeSndfileHandle(input_file_name.c_str(), SFM_READ);
            if (!input_file)
                throw std::runtime_error("cannot open input file");

            if (input_file.samplerate() != samplerate)
                throw std::runtime_error("input file: samplerate mismatch");

            input_channels = input_file.channels();
            super::input_samples.resize(input_channels);
        } else
            input_channels = 0;
        read_position = 0;

        output_file = makeSndfileHandle(output_file_name.c_str(), SFM_WRITE, format, output_channel_count, samplerate);
        if (!output_file)
            throw std::runtime_error("cannot open output file");

        output_file.command(SFC_SET_CLIPPING, nullptr, SF_TRUE);

        super::output_samples.resize(output_channel_count);

        temp_buffer.reset(calloc_aligned<float>(std::max(input_channels, output_channels) * block_size));
    }

    void close_client(void) {
        output_file.writeSync();
        input_file = output_file = SndfileHandle();
    }

    bool audio_is_opened(void) { return output_file; }

    bool audio_is_active(void) { return running.load(std::memory_order_acquire); }

    void activate_audio(void) {
        running.store(true);

        if (input_file) {
            reader_running.store(true);
            reader_thread = std::thread(std::bind(&sndfile_backend::sndfile_read_thread, this));
        }


        writer_running.store(true);
        writer_thread = std::thread(std::bind(&sndfile_backend::sndfile_write_thread, this));
    }

    void deactivate_audio(void) {
        running.store(false);

        if (input_file) {
            reader_running.store(false);
            reader_thread.join();
        }

        writer_running.store(false);
        write_semaphore.post();
        writer_thread.join();
    }

private:
    /* read input fifo from the rt context */
    void read_input_buffers(size_t frames_per_tick) {
        if (reader_running.load(std::memory_order_acquire)) {
            const size_t total_samples = input_channels * frames_per_tick;
            size_t remaining = total_samples;

            read_semaphore.wait();
            do {
                remaining -= read_frames.pop(temp_buffer.get(), remaining);

                if (unlikely(read_frames.empty() && !reader_running.load(std::memory_order_acquire))) {
                    /* at the end, we are not able to read a full sample block, clear the final parts */
                    const size_t last_frame = (total_samples - remaining) / input_channels;
                    const size_t remaining_per_channel = remaining / input_channels;
                    assert(remaining % input_channels == 0);
                    assert(remaining_per_channel % input_channels == 0);

                    for (uint16_t channel = 0; channel != input_channels; ++channel)
                        zerovec(super::input_samples[channel].get() + last_frame, remaining_per_channel);

                    break;
                }
            } while (remaining);

            const size_t frames = (total_samples - remaining) / input_channels;
            for (size_t frame = 0; frame != frames; ++frame) {
                for (uint16_t channel = 0; channel != input_channels; ++channel)
                    super::input_samples[channel].get()[frame] = temp_buffer.get()[frame * input_channels + channel];
            }
        } else
            super::clear_inputs(frames_per_tick);
    }

    void sndfile_read_thread(void) {
        nova::name_thread("sndfile reader");
        assert(input_file);

        const size_t frames_per_tick = get_audio_blocksize();

        // something like autobuffer might be good
        std::vector<sample_type, boost::alignment::aligned_allocator<sample_type, 64>> data_to_read(
            input_channels * frames_per_tick, 0.f);

        for (;;) {
            if (unlikely(reader_running.load(std::memory_order_acquire) == false))
                return;

            if (read_position < (size_t)input_file.frames()) {
                size_t frames = input_file.frames() - read_position;
                if (frames > frames_per_tick)
                    frames = frames_per_tick;

                input_file.readf(data_to_read.data(), frames);
                read_position += frames;

                const size_t item_to_enqueue = input_channels * frames;
                size_t remaining = item_to_enqueue;

                do {
                    remaining -= read_frames.push(data_to_read.data(), remaining);
                } while (remaining);
                read_semaphore.post();
            } else
                reader_running.store(false, std::memory_order_release);
        }
    }

    /* write output fifo from rt context */
    void write_output_buffers(size_t frames_per_tick) {
        for (size_t frame = 0; frame != frames_per_tick; ++frame) {
            for (uint16_t channel = 0; channel != output_channels; ++channel)
                temp_buffer.get()[frame * output_channels + channel] = super::output_samples[channel].get()[frame];
        }

        const size_t total_samples = output_channels * frames_per_tick;
        sample_type* buffer = temp_buffer.get();

        size_t count = total_samples;

        do {
            size_t consumed = write_frames.push(buffer, count);
            count -= consumed;
            buffer += consumed;
            write_semaphore.post();
            if (!consumed)
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
        } while (count);
    }

    void sndfile_write_thread(void) {
        nova::name_thread("sndfile writer");

        const size_t frames_per_tick = get_audio_blocksize();
        const size_t deque_per_tick = output_channels * frames_per_tick * 64;
        aligned_storage_ptr<sample_type> data_to_write(deque_per_tick);

        size_t pending_samples = 0;

        for (;;) {
            write_semaphore.wait();
            poll_writer_queue(data_to_write.get(), deque_per_tick, pending_samples);
            if (unlikely(writer_running.load(std::memory_order_acquire) == false))
                break;
        }

        while (poll_writer_queue(data_to_write.get(), deque_per_tick, pending_samples)) {
        }
    }

    bool poll_writer_queue(sample_type* data_to_write, const size_t buffer_samples, size_t& pending_samples) {
        bool consumed_item = false;
        for (;;) {
            const size_t available_samples = write_frames.read_available();
            const size_t available_frames = available_samples / output_channels;

            if (available_frames == 0)
                return consumed_item;

            const size_t buffer_frames = buffer_samples / output_channels;
            const size_t frames_to_read = std::min(available_frames, buffer_frames);

            const size_t samples_to_read = frames_to_read * output_channels;
            const size_t dequeued = write_frames.pop(data_to_write, samples_to_read);

            assert(dequeued == samples_to_read);

            consumed_item = true;

            const sf_count_t written_frames = output_file.writef(data_to_write, frames_to_read);
            assert(frames_to_read == written_frames);
            if (written_frames == -1)
                throw std::runtime_error(std::string("sndfile write failed: ") + output_file.strError());

            for (size_t frame = 0; frame != frames_to_read; ++frame) {
                for (size_t channel = 0; channel != output_channels; ++channel) {
                    const sample_type current_sample = data_to_write[frame * output_channels + channel];

                    sample_type current_peak = max_peaks[channel];

                    max_peaks[channel] = std::max(current_peak, std::abs(current_sample));
                }
            }
        }

        return consumed_item;
    }

public:
    void audio_fn_noinput(size_t frames_per_tick) {
        engine_functor::run_tick();
        write_output_buffers(frames_per_tick);
    }

    void audio_fn(size_t frames_per_tick) {
        super::clear_outputs(frames_per_tick);
        read_input_buffers(frames_per_tick);
        engine_functor::run_tick();
        write_output_buffers(frames_per_tick);
    }

private:
    SndfileHandle input_file, output_file;
    std::size_t read_position;
    int block_size_;

    aligned_storage_ptr<sample_type> temp_buffer;

    std::thread reader_thread, writer_thread;
    boost::lockfree::spsc_queue<sample_type> read_frames, write_frames;
    boost::sync::semaphore read_semaphore, write_semaphore;
    std::atomic<bool> running = { false }, reader_running = { false }, writer_running = { false };
    std::vector<sample_type> max_peaks;
};

} /* namespace nova */
