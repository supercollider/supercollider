//  native jack backend
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

#ifndef AUDIO_BACKEND_JACK_BACKEND_HPP
#define AUDIO_BACKEND_JACK_BACKEND_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <jack/jack.h>

#include "simd/simd_memory.hpp"

#include "spin_lock.hpp"
#include "thread_affinity.hpp"
#include "utilities/branch_hints.hpp"

namespace nova
{

/** jack backend
 *
 *  the jack callback thread is pinned to the first cpu of the system
 *
 *  \todo later it may be interesting to directly map the io busses to the jack port regions
 *  \todo rethink the use of output port lock
 */
template <void(*dsp_cb)(void)>
class jack_backend
{
public:
    jack_backend(void):
        client(NULL)
    {}

    ~jack_backend(void)
    {
        close_client();
    }

    /* to be called from the audio callback */
    /* @{  */
    void deliver_dac_output(const_restricted_sample_ptr source, uint channel, uint frames)
    {
        if (likely(audio_is_active()))
        {
            if (channel < output_channels) {
                spin_lock::scoped_lock lock(output_lock);
                addvec_simd(output_samples[channel], source, frames);
            }
        }
    }

    void deliver_dac_output_64(const_restricted_sample_ptr source, uint channel)
    {
        if (likely(audio_is_active()))
        {
            if (channel < output_channels) {
                spin_lock::scoped_lock lock(output_lock);
                addvec_simd<64>(output_samples[channel], source);
            }
        }
    }

    void fetch_adc_input(restricted_sample_ptr destination, uint channel, uint frames)
    {
        if (likely(audio_is_active()))
            copyvec_simd(destination, input_samples[channel], frames);
        else
            zerovec_simd(destination, frames);
    }

    void fetch_adc_input_64(restricted_sample_ptr destination, uint channel)
    {
        if (likely(audio_is_active()))
            copyvec_simd<64>(destination, input_samples[channel]);
        else
            zerovec_simd<64>(destination);
    }
    /* @} */

    uint32_t get_audio_blocksize(void) const
    {
        return blocksize_;
    }

public:
    void open_client(std::string const & name, uint32_t input_port_count, uint32_t output_port_count, uint32_t blocksize)
    {
        blocksize_ = blocksize;

        /* open client */
        client = jack_client_open(name.c_str(), JackNoStartServer, &status);
        if (status & JackServerFailed)
            throw std::runtime_error("Unable to connect to JACK server");

        if (status & JackNameNotUnique) {
            const char * client_name = jack_get_client_name(client);
            std::cout << "unique client name: " << client_name << std::endl;
        }

        /* initialize callbacks */
        jack_set_thread_init_callback (client, jack_thread_init_callback, NULL);
        jack_set_process_callback (client, jack_process_callback, this);

        /* register ports */
        input_ports.clear();
        for (uint32_t i = 0; i != input_port_count; ++i) {
            std::string portname ("input_");
            portname += boost::lexical_cast<std::string>(i+1);
            jack_port_t * port =
                jack_port_register(client, portname.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
            input_ports.push_back(port);
        }
        input_channels = input_port_count;
        input_samples.resize(input_port_count);

        output_ports.clear();
        for (uint32_t i = 0; i != output_port_count; ++i) {
            std::string portname ("output_");
            portname += boost::lexical_cast<std::string>(i+1);
            jack_port_t * port =
                jack_port_register(client, portname.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
            output_ports.push_back(port);
        }
        output_channels = output_port_count;
        output_samples.resize(output_port_count);

        samplerate_ = jack_get_sample_rate(client);
        jack_frames = jack_get_buffer_size(client);

        if (jack_frames % blocksize_)
            throw std::runtime_error("jack buffer size is not a multiple of blocksize");
    }

    void close_client(void)
    {
        if (client)
            jack_client_close(client);
    }

    bool audio_is_opened(void)
    {
        return client != NULL;
    }

    bool audio_is_active(void)
    {
        return is_active;
    }

    void activate_audio(void)
    {
        is_active = true;
        jack_activate(client);
    }

    void deactivate_audio(void)
    {
        jack_deactivate(client);
        is_active = false;
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

    float get_cpuload(void) const
    {
        if (likely(client))
            return jack_cpu_load(client);
        else
            return 0.f;
    }

private:
    static void jack_thread_init_callback(void * arg)
    {
        if (!thread_set_affinity(0))
            std::cerr << "Warning: cannot set thread affinity of jack thread" << std::endl;
    }

    static int jack_process_callback(jack_nframes_t frames, void * arg)
    {
        return static_cast<jack_backend*>(arg)->perform(frames);
    }

    int perform(jack_nframes_t frames)
    {
        /* get port regions */
        for (uint16_t i = 0; i != input_channels; ++i)
            input_samples[i] = (jack_default_audio_sample_t*) jack_port_get_buffer(input_ports[i], frames);

        for (uint16_t i = 0; i != output_channels; ++i) {
            output_samples[i] = (jack_default_audio_sample_t*) jack_port_get_buffer(output_ports[i], frames);
            zerovec_simd(output_samples[i], frames); /* we clear the outputs, that we can simply add the delivered data */
        }

        jack_nframes_t i = 0;
        while (true)
        {
            (*dsp_cb)();

            i += blocksize_;
            if (i == frames)
                break;

            /* increment cached port regions */
            for (uint16_t i = 0; i != input_channels; ++i)
                input_samples[i] += blocksize_;
            for (uint16_t i = 0; i != output_channels; ++i)
                output_samples[i] += blocksize_;
        }

        return 0;
    }

    static int jack_buffersize_callback(jack_nframes_t frames, void * arg)
    {
        return static_cast<jack_backend*>(arg)->buffer_size_callback(frames);
    }

    int buffer_size_callback(jack_nframes_t frames)
    {
        jack_frames = frames;
        if (jack_frames % blocksize_)
            /* we need a multiple of the blocksize */
            return 1;
        return 0;
    }

    jack_client_t * client;
    jack_status_t status;

    bool is_active;
    float samplerate_;
    uint16_t input_channels, output_channels;
    uint32_t blocksize_;

    std::vector<jack_default_audio_sample_t*> input_samples, output_samples;
    std::vector<jack_port_t*> input_ports, output_ports;
    jack_nframes_t jack_frames;

    spin_lock output_lock;
};

} /* namespace nova */


#endif /* AUDIO_BACKEND_JACK_BACKEND_HPP */
