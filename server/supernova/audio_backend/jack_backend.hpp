//  native jack backend
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

#ifndef AUDIO_BACKEND_JACK_BACKEND_HPP
#define AUDIO_BACKEND_JACK_BACKEND_HPP

#include <iostream>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

#include <jack/jack.h>
#include <jack/thread.h>

#include "nova-tt/thread_affinity.hpp"
#include "utilities/branch_hints.hpp"

#include "audio_backend_common.hpp"

namespace nova
{

/** jack backend
 *
 *  the jack callback thread is pinned to the first cpu of the system
 *
 *  \todo later it may be interesting to directly map the io busses to the jack port regions
 *  \todo rethink the use of output port lock
 */
template <typename engine_functor,
          typename sample_type = float,
          bool blocking = false
         >
class jack_backend:
    public detail::audio_delivery_helper<sample_type, jack_default_audio_sample_t, blocking, false>,
    public detail::audio_settings_basic,
    protected engine_functor
{
    typedef detail::audio_delivery_helper<sample_type, jack_default_audio_sample_t, blocking, false> super;

public:
    jack_backend(void):
        client(NULL)
    {}

    ~jack_backend(void)
    {
        if (audio_is_active())
            deactivate_audio();

        close_client();
    }

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
        super::input_samples.resize(input_port_count);

        output_ports.clear();
        for (uint32_t i = 0; i != output_port_count; ++i) {
            std::string portname ("output_");
            portname += boost::lexical_cast<std::string>(i+1);
            jack_port_t * port =
                jack_port_register(client, portname.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
            output_ports.push_back(port);
        }
        output_channels = output_port_count;
        super::output_samples.resize(output_port_count);

        samplerate_ = jack_get_sample_rate(client);
        jack_frames = jack_get_buffer_size(client);

        if (jack_frames % blocksize_)
            throw std::runtime_error("jack buffer size is not a multiple of blocksize");
    }

    void close_client(void)
    {
        if (client) {
            jack_client_close(client);
            client = NULL;
        }
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

    float get_cpuload(void) const
    {
        if (likely(client))
            return jack_cpu_load(client);
        else
            return 0.f;
    }

    int connect_input(int channel, const char * portname)
    {
        if (channel >= input_ports.size())
            return -1;
        return jack_connect(client, portname, jack_port_name(input_ports[channel]));
    }

    int connect_output(int channel, const char * portname)
    {
        if (channel >= output_ports.size())
            return -1;
        return jack_connect(client, jack_port_name(output_ports[channel]), portname);
    }

    int connect_all_inputs(const char * client_name)
    {
        const char **ports = jack_get_ports (client, client_name, NULL, JackPortIsOutput);

        if (!ports)
            return -1;

        std::size_t i = 0;
        while (ports[i]) {
            if (i == input_ports.size())
                break;

            int err = jack_connect(client, ports[i], jack_port_name(input_ports[i]));
            if (err)
                return err;
            ++i;
        }

        free(ports);
        return 0;
    }

    int connect_all_outputs(const char * client_name)
    {
        const char **ports = jack_get_ports (client, client_name, NULL, JackPortIsInput);

        if (!ports)
            return -1;

        std::size_t i = 0;
        while (ports[i]) {
            if (i == output_ports.size())
                break;

            int err = jack_connect(client, jack_port_name(output_ports[i]), ports[i]);
            if (err)
                return err;
            ++i;
        }

        free(ports);
        return 0;
    }

    int max_realtime_priority(void) const
    {
        return jack_client_max_real_time_priority(client);
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
        engine_functor::init_tick();

        /* get port regions */
        jack_default_audio_sample_t * inputs[input_channels];
        jack_default_audio_sample_t * outputs[output_channels];
        for (uint16_t i = 0; i != input_channels; ++i)
            inputs[i] = (jack_default_audio_sample_t*) jack_port_get_buffer(input_ports[i], frames);

        for (uint16_t i = 0; i != output_channels; ++i)
            outputs[i] = (jack_default_audio_sample_t*) jack_port_get_buffer(output_ports[i], frames);

        jack_nframes_t processed = 0;
        while (processed != frames)
        {
            for (uint16_t i = 0; i != input_channels; ++i) {
                copyvec(super::input_samples[i].get(), inputs[i], frames);
                inputs[i] += blocksize_;
            }

            engine_functor::run_tick();

            for (uint16_t i = 0; i != output_channels; ++i)
            {
                copyvec(outputs[i], super::output_samples[i].get(), frames);
                outputs[i] += blocksize_;
            }

            processed += blocksize_;
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
    uint32_t blocksize_;

    std::vector<jack_port_t*> input_ports, output_ports;
    jack_nframes_t jack_frames;
};

} /* namespace nova */


#endif /* AUDIO_BACKEND_JACK_BACKEND_HPP */
