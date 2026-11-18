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
// NOTE: replace with std::format once we switch to C++20
#include <boost/format.hpp>

#include "server.hpp"
#include "audio_backend/sndfile_backend.hpp"

#include "../../common/SC_SndFileHelpers.hpp"
#include "../../common/SC_ServerProgramOptions.hpp"
#include "../../include/plugin_interface/SC_InlineUnaryOp.h"

namespace nova {

struct non_rt_functor {
    // implemented in server.cpp
    static void init_thread();

    static inline void run_tick() {
        run_scheduler_tick();
        instance->update_logical_time();
    }
};

struct non_realtime_synthesis_engine {
    non_realtime_synthesis_engine(ServerProgramOptions const& args) {
        int format = headerFormatFromString(args.mNonRealTimeOutputHeaderFormat)
            | sampleFormatFromString(args.mNonRealTimeOutputSampleFormat);

        backend.open_client(args.mNonRealTimeInputFilename, args.mNonRealTimeOutputFilename, args.mPreferredSampleRate,
                            format, args.mNumOutputBusChannels, args.mBufLength);

        assert(args.mNonRealTimeCmdFilename != nullptr);
        command_stream.open(args.mNonRealTimeCmdFilename, std::fstream::in | std::fstream::binary);
        if (!command_stream) {
            throw std::runtime_error("cannot open OSC command file");
        }

        has_inputs = args.mNonRealTimeInputFilename != nullptr;
        samples_per_block = args.mBufLength;

        prepare_backend(args.mBufLength, args.mNumInputBusChannels, args.mNumOutputBusChannels);
    }

    /** setup the mapping between input/output buses and the input/output file buffers */
    void prepare_backend(int blocksize, int input_bus_channels, int output_bus_channels) {
        assert(blocksize > 0 && input_bus_channels > 0 && output_bus_channels > 0);

        auto& world = sc_factory->world;

        std::vector<sample*> inputs, outputs;

        if (input_bus_channels != backend.get_input_count()) {
            log_printf("WARNING: input file channels didn't match number of inputs specified in options.\n");
            // keep the number of input buses!
            // otherwise clients would operate with a wrong private audio bus offset!
        }
        for (int channel = 0; channel < input_bus_channels; ++channel)
            inputs.push_back(sc_factory->world.mAudioBus + (blocksize * (output_bus_channels + channel)));

        backend.input_mapping(inputs.begin(), inputs.end());

        for (int channel = 0; channel < output_bus_channels; ++channel)
            outputs.push_back(sc_factory->world.mAudioBus + blocksize * channel);

        backend.output_mapping(outputs.begin(), outputs.end());
    }

    void run() {
        using namespace std::chrono;

        const int reserved_packet_size = 16384;
        std::vector<char> packet_vector(reserved_packet_size, 0);

        log_printf("\nPerforming non-rt synthesis:\n");
        backend.activate_audio();

        auto start_time = steady_clock::now();
        time_tag bundle_time;

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

            bundle_time = instance->handle_bundle_nrt(packet_vector.data(), packet_size);

            size_t seconds = bundle_time.get_secs();
            size_t nano_seconds = bundle_time.get_nanoseconds();
            log_printf("  Next OSC bundle: %zu.%09zu\n", seconds, nano_seconds);

            // compute audio up to and excluding the control block that the current bundle timestamp
            // falls into. This is because we first have to process all bundles for a given control
            // block before we can compute audio for it.
            while (instance->next_time() < bundle_time) {
                if (instance->quit_requested())
                    goto done;

                if (has_inputs)
                    backend.audio_fn(samples_per_block);
                else
                    backend.audio_fn_noinput(samples_per_block);
            }
        }

        // finish the very last block, but only if the last bundle does not fall directly on a
        // block boundary. For example, if the last bundle is scheduled for 1.0 seconds @ 48 kHz,
        // we do not want to compute samples 48000-48064, instead we want the sound file to be
        // exactly 1 second long.
        if (instance->now < bundle_time) {
            if (has_inputs)
                backend.audio_fn(samples_per_block);
            else
                backend.audio_fn_noinput(samples_per_block);
        }

    done:
        backend.deactivate_audio();

        auto end_time = steady_clock::now();
        std::string elapsed_string = format_duration(end_time - start_time);
        log_printf("\nNon-rt synthesis finished in %s\n", elapsed_string.c_str());

        auto total_time = instance->current_time().to_seconds();
        auto total_duration = std::chrono::duration<double>(total_time);
        log_printf("Output file duration: %s\n", format_duration(total_duration).c_str());

        log_printf("Peak summary:\n");
        auto peaks = backend.get_peaks();
        for (size_t channel = 0; channel != peaks.size(); ++channel) {
            auto amplitude = peaks[channel];
            log_printf("  Channel %zu: %.3fdB\n", channel, sc_ampdb(amplitude));
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
