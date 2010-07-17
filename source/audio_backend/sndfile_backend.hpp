//  file-based backend (via libsndfile)
//  Copyright (C) 2010 Tim Blechmann
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

#ifndef AUDIO_BACKEND_SNDFILE_BACKEND_HPP
#define AUDIO_BACKEND_SNDFILE_BACKEND_HPP

#include <cmath>
#include <string>

#include <boost/atomic.hpp>
#include <boost/lockfree/ringbuffer.hpp>
#include <boost/thread.hpp>

#include <sndfile.hh>

#include "nova-tt/semaphore.hpp"
#include "utilities/branch_hints.hpp"

#include "audio_backend_common.hpp"

namespace nova
{

/** sndfile backend
 *
 *  audio backend, reading/writing sound files via libsndfile
 *
 */
template <typename engine_functor,
          typename sample_type = float,
          bool blocking = false
         >
class sndfile_backend:
    public detail::audio_delivery_helper<sample_type, float, blocking, false>,
    public detail::audio_settings_basic,
    private engine_functor
{
    typedef detail::audio_delivery_helper<sample_type, float, blocking, false> super;
    typedef std::size_t size_t;

public:
    sndfile_backend(void):
        read_frames(65536), write_frames(65536), running(false), reader_running(false), writer_running(false)
    {}

    ~sndfile_backend(void)
    {}

    size_t get_audio_blocksize(void)
    {
        return 64;
    }

public:
    void open_client(std::string const & input_file_name, std::string const & output_file_name,
                     float samplerate, int format, uint32_t output_channel_count)
    {
        output_channels = output_channel_count;
        samplerate_ = samplerate = std::floor(samplerate);

        if (!input_file_name.empty())
        {
            input_file = SndfileHandle(input_file_name.c_str(), SFM_READ);
            if (!input_file)
                throw std::runtime_error("cannot open input file");

            if (input_file.samplerate() != samplerate)
                throw std::runtime_error("input file: samplerate mismatch");

            input_channels = input_file.channels();
            super::input_samples.resize(input_channels);
        }
        else
            input_channels = 0;
        read_position = 0;

        output_file = SndfileHandle(output_file_name.c_str(), SFM_WRITE, format, output_channel_count, samplerate);
        if (!output_file)
            throw std::runtime_error("cannot open output file");
        super::output_samples.resize(output_channel_count);

        temp_buffer.reset(calloc_aligned<float>(std::max(input_channels, output_channels) * 64));

    }

    void close_client(void)
    {
        output_file.writeSync();
        input_file = output_file = SndfileHandle();
    }

    bool audio_is_opened(void)
    {
        return output_file;
    }

    bool audio_is_active(void)
    {
        return running.load(boost::memory_order_acquire);
    }

    void activate_audio(void)
    {
        running.store(true);

        if (input_file)
        {
            reader_running.store(true);
            reader_thread = boost::thread(boost::bind(&sndfile_backend::sndfile_read_thread, this));
        }

        writer_running.store(true);
        writer_thread = boost::thread(boost::bind(&sndfile_backend::sndfile_write_thread, this));
    }

    void deactivate_audio(void)
    {
        running.store(false);

        if (input_file)
        {
            reader_running.store(false);
            reader_thread.join();
        }

        writer_running.store(false);
        write_semaphore.post();
        writer_thread.join();
    }

private:
    /* read input fifo from the rt context */
    void read_input_buffers(size_t frames_per_tick)
    {
        if (reader_running.load(boost::memory_order_acquire))
        {
            const size_t total_samples = input_channels * frames_per_tick;
            size_t remaining = total_samples;

            read_semaphore.wait();
            do {
                remaining -= read_frames.dequeue(temp_buffer.get(), remaining);

                if (unlikely(read_frames.empty() &&
                             !reader_running.load(boost::memory_order_acquire)))
                {
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
            for (size_t frame = 0; frame != frames; ++frame)
            {
                for (uint16_t channel = 0; channel != input_channels; ++channel)
                    super::input_samples[channel].get()[frame] = temp_buffer.get()[frame * input_channels + channel];
            }
        }
        else
            super::clear_inputs(frames_per_tick);
    }

    void sndfile_read_thread(void)
    {
        assert(input_file);

        const size_t frames_per_tick = get_audio_blocksize();
        sized_array<sample_type, aligned_allocator<sample_type> > data_to_read(input_channels * frames_per_tick, 0.f);

        for (;;)
        {
            if (unlikely(reader_running.load(boost::memory_order_acquire) == false))
                return;

            if (read_position < (size_t)input_file.frames())
            {
                size_t frames = input_file.frames() - read_position;
                if (frames > frames_per_tick)
                    frames = frames_per_tick;

                input_file.readf(data_to_read.c_array(), frames);
                read_position += frames;

                const size_t item_to_enqueue = input_channels * frames;
                size_t remaining = item_to_enqueue;

                do {
                    remaining -= read_frames.enqueue(data_to_read.c_array(), remaining);
                } while(remaining);
                read_semaphore.post();
            }
            else
                reader_running.store(false, boost::memory_order_release);
        }
    }

    /* write output fifo from rt context */
    void write_output_buffers(size_t frames_per_tick)
    {
        for (size_t frame = 0; frame != frames_per_tick; ++frame)
        {
            for (uint16_t channel = 0; channel != output_channels; ++channel)
                temp_buffer.get()[frame * output_channels + channel] = super::output_samples[channel].get()[frame];
        }

        const size_t total_samples = output_channels * frames_per_tick;
        sample_type * buffer = temp_buffer.get();

        size_t count = total_samples;
        do {
            size_t consumed = write_frames.enqueue(buffer, count);
            count -= consumed;
            buffer += consumed;
            write_semaphore.post();
        } while (count);
    }

    void sndfile_write_thread(void)
    {
        const size_t frames_per_tick = get_audio_blocksize();
        sized_array<sample_type, aligned_allocator<sample_type> > data_to_write(output_channels * frames_per_tick, 0.f);

        for (;;)
        {
            write_semaphore.wait();
            for (;;)
            {
                size_t dequeued = write_frames.dequeue(data_to_write.c_array(), data_to_write.size());

                if (dequeued == 0)
                    break;
                output_file.write(data_to_write.c_array(), dequeued);
            }
            if (unlikely(writer_running.load(boost::memory_order_acquire) == false))
                return;
        }

    }

public:
    void audio_fn_noinput(size_t frames_per_tick)
    {
        engine_functor::run_tick();
        write_output_buffers(frames_per_tick);
    }

    void audio_fn(size_t frames_per_tick)
    {
        super::clear_outputs(frames_per_tick);
        read_input_buffers(frames_per_tick);
        engine_functor::run_tick();
        write_output_buffers(frames_per_tick);
    }

private:
    SndfileHandle input_file, output_file;
    std::size_t read_position;

    aligned_storage_ptr<sample_type> temp_buffer;

    boost::thread reader_thread, writer_thread;
    boost::lockfree::ringbuffer< sample_type, 0 > read_frames, write_frames;
    nova::semaphore read_semaphore, write_semaphore;
    boost::atomic<bool> running, reader_running, writer_running;
};

} /* namespace nova */

#endif /* AUDIO_BACKEND_SNDFILE_BACKEND_HPP */
