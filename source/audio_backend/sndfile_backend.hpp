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

#include <string>

#include <boost/atomic.hpp>
#include <boost/thread.hpp>

#include <sndfile.hh>

#include "utilities/branch_hints.hpp"

#include "audio_backend_common.hpp"

namespace nova
{

/** sndfile backend
 *
 *  audio backend, reading/writing sound files via libsndfile
 *
 */
template <void(*dsp_cb)(void), typename sample_type = float, bool blocking = false>
class sndfile_backend:
    public detail::audio_delivery_helper<sample_type, blocking, true>
{
    typedef detail::audio_delivery_helper<sample_type, blocking> super;
    typedef std::size_t size_t;

public:
    sndfile_backend(void):
        running(false)
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
        samplerate_ = std::floor(samplerate);

        if (input_file_name != "_")
        {
            input_file = SndfileHandle(input_file_name.c_str(), SFM_READ);
            if (!input_file)
                throw std::runtime_error("cannot open input file");

            if (input_file.samplerate() != samplerate_)
                throw std::runtime_error("input file: samplerate mismatch");

            input_channels = input_file.channels();
        }
        else
            input_channels = 0;
        read_position = 0;

        output_file = SndfileHandle(output_file_name.c_str(), SFM_WRITE, format, output_channel_count, samplerate_);
        if (!output_file)
            throw std::runtime_error("cannot open output file");

        super::prepare_helper_buffers(input_channels, output_channels, 64);

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
            audio_thread = boost::thread(boost::bind(&sndfile_backend::audio_thread_fn, this));
        else
            audio_thread = boost::thread(boost::bind(&sndfile_backend::audio_thread_fn_noinput, this));
    }

    void deactivate_audio(void)
    {
        running.store(false);
        audio_thread.join();
    }

    float get_samplerate(void) const
    {
        return samplerate_;
    }

    uint16_t get_input_count(void) const
    {
        return input_channels;
    }

    uint16_t get_output_count(void) const
    {
        return output_channels;
    }

private:
    void read_input_buffers(size_t frames_per_tick)
    {
        if (input_file && (read_position < (size_t)input_file.frames()))
        {
            size_t frames = input_file.frames() - read_position;
            if (frames > frames_per_tick)
                frames = frames_per_tick;

            input_file.readf(temp_buffer.get(), frames);

            size_t remaining = frames_per_tick - frames;
            if (unlikely(remaining))
                std::memset(temp_buffer.get() + frames*input_channels, 0,
                            sizeof(float) * input_channels * remaining);

            for (size_t frame = 0; frame != frames_per_tick; ++frame)
            {
                for (uint16_t channel = 0; channel != input_channels; ++channel)
                    super::input_samples[channel].get()[frame] = temp_buffer.get()[frame * input_channels + channel];
            }
            read_position += frames;
        }
        else
            super::clear_inputs(frames_per_tick);
    }

    void write_output_buffers(size_t frames_per_tick)
    {
        for (size_t frame = 0; frame != frames_per_tick; ++frame)
        {
            for (uint16_t channel = 0; channel != output_channels; ++channel)
                temp_buffer.get()[frame * output_channels + channel] = super::output_samples[channel].get()[frame];

            output_file.writef(temp_buffer.get(), frames_per_tick);
        }
    }

    void audio_thread_fn_noinput(void)
    {
        while (running.load(boost::memory_order_acquire))
        {
            size_t frames_per_tick = 64;
            /* no need to clear the inputs, since they are not used */
            (*dsp_cb)();
            write_output_buffers(frames_per_tick);
        }
    }

    void audio_thread_fn(void)
    {
        while (running.load(boost::memory_order_acquire))
        {
            size_t frames_per_tick = 64;
            super::clear_outputs(frames_per_tick);
            read_input_buffers(frames_per_tick);
            (*dsp_cb)();
            write_output_buffers(frames_per_tick);
        }
    }

    float samplerate_;
    uint16_t input_channels, output_channels;

    SndfileHandle input_file, output_file;
    std::size_t read_position;

    aligned_storage_ptr<sample_type> temp_buffer;

    boost::thread audio_thread;
    boost::atomic<bool> running;
};

} /* namespace nova */


#endif /* AUDIO_BACKEND_SNDFILE_BACKEND_HPP */
