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

#ifndef SERVER_NRT_SYNTHESIS_HPP
#define SERVER_NRT_SYNTHESIS_HPP

#include <iostream>
#include <string>
#include <fstream>

#include <boost/integer/endian.hpp>

#include "server_args.hpp"
#include "server.hpp"
#include "audio_backend/sndfile_backend.hpp"
#include "sc/sc_plugin_interface.hpp"

namespace nova
{

struct non_rt_functor
{
    static inline void init_thread(void)
    {
        realtime_engine_functor::init_thread();
    }

    static inline void run_tick(void)
    {
        run_scheduler_tick();
        instance->increment_logical_time();
    }
};

struct non_realtime_synthesis_engine
{
    typedef std::string string;

    non_realtime_synthesis_engine(server_arguments const & args)
    {
        int format = headerFormatFromString(args.header_format.c_str())
                   | sampleFormatFromString(args.sample_format.c_str());

        string input_file = args.input_file;
        if (input_file == string("_"))
            input_file.clear();

        backend.open_client(input_file, args.output_file, args.samplerate, format, args.output_channels);

        command_stream.open(args.command_file.c_str(), std::fstream::in | std::fstream::binary);

        has_inputs = !input_file.empty();
        samples_per_block = args.blocksize;
        prepare_backend(args.blocksize, args.input_channels, args.output_channels);
    }

    void prepare_backend(int blocksize, int input_channels, int output_channels)
    {
        std::vector<sample*> inputs, outputs;
        for (int channel = 0; channel != input_channels; ++channel)
            inputs.push_back(sc_factory->world.mAudioBus + (blocksize * (output_channels + channel)));

        backend.input_mapping(inputs.begin(), inputs.end());

        for (int channel = 0; channel != output_channels; ++channel)
            outputs.push_back(sc_factory->world.mAudioBus + blocksize * channel);

        backend.output_mapping(outputs.begin(), outputs.end());
    }

    void run(void)
    {
        backend.activate_audio();

        boost::array<char, 16384> packet_buffer;

        using namespace std;

        cout << "Starting non-rt synthesis" << endl;

        while (!command_stream.eof())
        {
            boost::integer::big32_t packet_size;
            command_stream.read((char*)&packet_size, sizeof(packet_size));

            assert(packet_size > 0);
            command_stream.read(packet_buffer.data(), packet_size);

            time_tag bundle_time = instance->handle_bundle_nrt(packet_buffer.data(), packet_size);

            cout << "Next OSC bundle " << bundle_time.get_secs() << "." << bundle_time.get_nanoseconds() << endl;

            while (instance->current_time() < bundle_time)
            {
                if (has_inputs)
                    backend.audio_fn(64);
                else
                    backend.audio_fn_noinput(64);
            }
        }
        backend.deactivate_audio();
        cout << "Non-rt synthesis finished" << endl;
    }

    sndfile_backend<non_rt_functor, float, true> backend;
    std::fstream command_stream;
    int samples_per_block;
    bool has_inputs;
};

} // namespace nova

#endif /* SERVER_NRT_SYNTHESIS_HPP */
