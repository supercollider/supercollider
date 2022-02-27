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

#include <cstdio>
#include <chrono>
#include <fstream>
#include <string>

#include <boost/endian/arithmetic.hpp>
#include <boost/format.hpp>

#include "server_args.hpp"
#include "server.hpp"
#include "audio_backend/sndfile_backend.hpp"
#include "sc/sc_plugin_interface.hpp"

#include "../../common/SC_SndFileHelpers.hpp"
#include "../../include/plugin_interface/SC_InlineUnaryOp.h"

namespace nova {

struct non_rt_functor {
    static inline void init_thread(void) { realtime_engine_functor::init_thread(); }

    static inline void run_tick(void) {
        run_scheduler_tick();
        instance->increment_logical_time();
    }
};

struct non_realtime_synthesis_engine {
    typedef std::string string;

    non_realtime_synthesis_engine(server_arguments const& args) {
        int format =
            headerFormatFromString(args.header_format.c_str()) | sampleFormatFromString(args.sample_format.c_str());

        string input_file = args.input_file;
        if (input_file == string("_"))
            input_file.clear();

        backend.open_client(input_file, args.output_file, args.samplerate, format, args.output_channels,
                            args.blocksize);

        command_stream.open(args.command_file.c_str(), std::fstream::in | std::fstream::binary);
        if (!command_stream) {
            throw std::runtime_error("cannot open OSC command file");
        }

        has_inputs = !input_file.empty();
        samples_per_block = args.blocksize;
        prepare_backend(args.blocksize, args.input_channels, args.output_channels);
    }

    void prepare_backend(int blocksize, int input_channels, int output_channels) {
        std::vector<sample*> inputs, outputs;
        for (int channel = 0; channel != input_channels; ++channel)
            inputs.push_back(sc_factory->world.mAudioBus + (blocksize * (output_channels + channel)));

        backend.input_mapping(inputs.begin(), inputs.end());

        for (int channel = 0; channel != output_channels; ++channel)
            outputs.push_back(sc_factory->world.mAudioBus + blocksize * channel);

        backend.output_mapping(outputs.begin(), outputs.end());
    }

    void run(void) {
        using namespace std;
        using namespace std::chrono;

        const int reserved_packet_size = 16384;
        std::vector<char> packet_vector(reserved_packet_size, 0);

        log_printf("\nPerforming non-rt synthesis:\n");
        backend.activate_audio();

        auto start_time = steady_clock::now();

        for (;;) {
            boost::endian::big_int32_t packet_size;
            if (!command_stream.read((char*)&packet_size, sizeof(packet_size)))
                break; // done

            assert(packet_size > 0);

            if (packet_size > (int32_t)packet_vector.size()) {
                packet_vector.resize(packet_size);
            }

            if (!command_stream.read(packet_vector.data(), packet_size)) {
                log_printf("ERROR: missing bundle data\n");
                break;
            }

            time_tag bundle_time = instance->handle_bundle_nrt(packet_vector.data(), packet_size);

            size_t seconds = bundle_time.get_secs();
            size_t nano_seconds = bundle_time.get_nanoseconds();
            log_printf("  Next OSC bundle: %zu.%09zu\n", seconds, nano_seconds);

            while (instance->next_time() < bundle_time) {
                if (instance->quit_requested())
                    goto done;

                if (has_inputs)
                    backend.audio_fn(samples_per_block);
                else
                    backend.audio_fn_noinput(samples_per_block);
            }
        }

    done:
        backend.deactivate_audio();
        auto end_time = steady_clock::now();
        std::string elapsed_string = format_duration(end_time - start_time);

        log_printf("\nNon-rt synthesis finished in %s\n", elapsed_string.c_str());

        auto peaks = backend.get_peaks();
        log_printf("Peak summary:\n");
        for (size_t channel = 0; channel != peaks.size(); ++channel) {
            auto amplitude = peaks[channel];
            log_printf("  Channel %zu: %gdB\n", channel, sc_ampdb(amplitude));
        }
    }

    template <typename Duration> static std::string format_duration(Duration const& duration) {
        using namespace boost;
        using namespace std::chrono;

        auto elapsed_hours = duration_cast<hours>(duration);
        auto remain = duration - elapsed_hours;
        auto elapsed_minutes = duration_cast<minutes>(remain);
        remain -= elapsed_minutes;
        auto elapsed_nanoseconds = duration_cast<nanoseconds>(remain);
        double seconds = elapsed_nanoseconds.count() * 1e-9;

        if (elapsed_hours.count())
            return str(format("%|d|h %|d|m %|0.3f|s") % elapsed_hours.count() % elapsed_minutes.count() % seconds);
        if (elapsed_minutes.count())
            return str(format("%|d|m %|.3f|s") % elapsed_minutes.count() % seconds);
        else
            return str(format("%|.3f|s") % seconds);
    }

private:
    sndfile_backend<non_rt_functor, float, true> backend;
    std::fstream command_stream;
    int samples_per_block;
    bool has_inputs;
};

} // namespace nova
