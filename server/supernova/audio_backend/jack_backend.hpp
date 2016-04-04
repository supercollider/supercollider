//  native jack backend
//  Copyright (C) 2009-2015 Tim Blechmann
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

#include <jack/jack.h>
#include <jack/thread.h>
#ifdef SC_JACK_USE_METADATA_API
#    include <jack/metadata.h>
#    include <jack/uuid.h>
#    include <jackey.h>
#endif

#include "utilities/branch_hints.hpp"

#include "audio_backend_common.hpp"
#include "cpu_time_info.hpp"

namespace nova {

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
    public detail::audio_backend_base<sample_type, jack_default_audio_sample_t, blocking, false>,
    public detail::audio_settings_basic,
    protected engine_functor
{
    typedef detail::audio_backend_base<sample_type, jack_default_audio_sample_t, blocking, false> super;

public:
    jack_backend(void) = default;

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
    void open_client(std::string const & server_name, std::string const & name, uint32_t input_port_count,
                     uint32_t output_port_count, uint32_t blocksize)
    {
        blocksize_ = blocksize;

        /* open client */
        client = server_name.empty() ? jack_client_open(name.c_str(), JackNoStartServer, &status)
                                     : jack_client_open(name.c_str(), jack_options_t(JackNoStartServer | JackServerName),
                                                        &status, server_name.c_str());
        std::atomic_thread_fence(std::memory_order_release); // ensure visibility on other threads

        if (status & JackServerFailed)
            throw std::runtime_error("Unable to connect to JACK server");

        if (status & JackNameNotUnique) {
            const char * client_name = jack_get_client_name(client);
            std::cout << "unique client name: " << client_name << std::endl;
        }

        /* initialize callbacks */
        jack_set_thread_init_callback (client, jack_thread_init_callback, this);
        jack_set_process_callback (client, jack_process_callback, this);
        jack_set_xrun_callback(client, jack_xrun_callback, this);
        jack_on_info_shutdown(client, (JackInfoShutdownCallback)jack_on_info_shutdown_callback, nullptr);

        /* register ports */
        input_ports.clear();
        for (uint32_t i = 0; i != input_port_count; ++i) {
            std::string portname ("input_");
            portname += std::to_string(i+1);
            jack_port_t * port =
                jack_port_register(client, portname.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0);
            input_ports.push_back(port);

#ifdef SC_JACK_USE_METADATA_API
            jack_uuid_t uuid = jack_port_uuid(port);
            if(!jack_uuid_empty(uuid)) {
                std::string prettyname ("Input ");
                prettyname += std::to_string(i+1);
                jack_set_property(client, uuid,
                    JACK_METADATA_PRETTY_NAME, prettyname.c_str(), "text/plain");
                
                std::string order ("");
                order += std::to_string(i);
                jack_set_property(client, uuid,
                    JACKEY_ORDER, order.c_str(), "http://www.w3.org/2001/XMLSchema#integer");
            }
#endif
        }
        input_channels = input_port_count;
        super::input_samples.resize(input_port_count);

        output_ports.clear();
        for (uint32_t i = 0; i != output_port_count; ++i) {
            std::string portname ("output_");
            portname += std::to_string(i+1);
            jack_port_t * port =
                jack_port_register(client, portname.c_str(), JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0);
            output_ports.push_back(port);

#ifdef SC_JACK_USE_METADATA_API
            jack_uuid_t uuid = jack_port_uuid(port);
            if(!jack_uuid_empty(uuid)) {
                std::string prettyname ("Output ");
                prettyname += std::to_string(i+1);
                jack_set_property(client, uuid, JACK_METADATA_PRETTY_NAME,
                    prettyname.c_str(), "text/plain");
                
                std::string order ("");
                order += std::to_string(input_port_count + i);
                jack_set_property(client, uuid, JACKEY_ORDER,
                    order.c_str(), "http://www.w3.org/2001/XMLSchema#integer");
            }
#endif
        }
        output_channels = output_port_count;
        super::output_samples.resize(output_port_count);

        samplerate_ = jack_get_sample_rate(client);
        jack_frames = jack_get_buffer_size(client);

        if (jack_frames % blocksize_)
            throw std::runtime_error("Jack buffer size is not a multiple of blocksize");

        cpu_time_accumulator.resize( samplerate_, jack_frames, 1 );
    }

    void close_client(void)
    {
        if (client) {
            jack_client_close(client);
            client = nullptr;
        }
    }

    bool audio_is_opened(void)
    {
        return client != nullptr;
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
        if (is_active) {
            jack_deactivate(client);
            is_active = false;
        }
    }

    void get_cpuload(float & peak, float & average) const
    {
        cpu_time_accumulator.get(peak, average);
    }

    int connect_input(size_t channel, const char * portname)
    {
        if (channel >= input_ports.size())
            return -1;
        return jack_connect(client, portname, jack_port_name(input_ports[channel]));
    }

    int connect_output(size_t channel, const char * portname)
    {
        if (channel >= output_ports.size())
            return -1;
        return jack_connect(client, jack_port_name(output_ports[channel]), portname);
    }

    int connect_all_inputs(const char * client_name)
    {
        const char **ports = jack_get_ports (client, client_name, nullptr, JackPortIsOutput);

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
        const char **ports = jack_get_ports (client, client_name, nullptr, JackPortIsInput);

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

    int realtime_priority(void) const
    {
        return jack_client_real_time_priority(client);
    }

private:
    static void jack_thread_init_callback(void * arg)
    {
        std::atomic_thread_fence(std::memory_order_acquire);
        jack_backend * self = static_cast<jack_backend*>(arg);
        if (jack_client_thread_id(self->client) == pthread_self())
            engine_functor::init_thread();
        else
            name_thread("Jack Helper");
    }

    static int jack_process_callback(jack_nframes_t frames, void * arg)
    {
        return static_cast<jack_backend*>(arg)->perform(frames);
    }

    static int jack_on_info_shutdown_callback(jack_status_t code, const char *reason, void *arg)
    {
        std::cerr << "Jack server was shut down: " << reason << std::endl;
        std::cerr << "Exiting ..." << std::endl;
        exit(0); // TODO: later we may want to call a function
    }

    static int jack_xrun_callback(void * arg)
    {
        return static_cast<jack_backend*>(arg)->handle_xrun();
    }

    int handle_xrun(void)
    {
        time_is_synced = false;
        engine_functor::log_("Jack: xrun detected - resyncing clock\n");
        return 0;
    }

    int perform(jack_nframes_t frames)
    {
        if (unlikely(!time_is_synced)) {
            engine_functor::sync_clock();
            time_is_synced = true;
        }

        /* get port regions */
        jack_default_audio_sample_t ** inputs   = (jack_default_audio_sample_t **)alloca(input_channels * sizeof(jack_default_audio_sample_t*));
        jack_default_audio_sample_t ** outputs  = (jack_default_audio_sample_t **)alloca(output_channels * sizeof(jack_default_audio_sample_t*));
        for (uint16_t i = 0; i != input_channels; ++i)
            inputs[i] = (jack_default_audio_sample_t*) jack_port_get_buffer(input_ports[i], frames);

        for (uint16_t i = 0; i != output_channels; ++i)
            outputs[i] = (jack_default_audio_sample_t*) jack_port_get_buffer(output_ports[i], frames);

        jack_nframes_t processed = 0;
        while (processed != frames) {
            super::fetch_inputs((const float**)inputs, blocksize_, input_channels);
            engine_functor::run_tick();
            super::deliver_outputs(outputs, blocksize_, output_channels);
            processed += blocksize_;
        }

        cpu_time_accumulator.update(jack_cpu_load(client));

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

    jack_client_t * client = nullptr;
    jack_status_t status;

    bool is_active      = false;
    bool time_is_synced = false;
    uint32_t blocksize_;

    std::vector<jack_port_t*> input_ports, output_ports;
    jack_nframes_t jack_frames;
    cpu_time_info cpu_time_accumulator;
};

} /* namespace nova */


#endif /* AUDIO_BACKEND_JACK_BACKEND_HPP */
