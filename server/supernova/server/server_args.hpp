//  server arguments
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

#ifndef SERVER_SERVER_ARGS_HPP
#define SERVER_SERVER_ARGS_HPP

#include <stdint.h>
#include <string>
#include <vector>
#include <memory>

namespace nova {

class server_arguments
{
    server_arguments(int argc, char * argv[]);

public:
    static server_arguments const &initialize(int argc, char * argv[])
    {
        instance_.reset(new server_arguments(argc, argv));
        return instance();
    }

    static server_arguments const & instance(void)
    {
        return *instance_;
    }

    /** set the sample rate (from the audio backend) */
    static void set_samplerate(uint32_t samplerate)
    {
        instance_->samplerate = samplerate;
    }

    uint32_t port(void) const
    {
        if (udp_port)
            return udp_port;
        else
            return tcp_port;
    }

    uint32_t udp_port, tcp_port;
    uint32_t control_busses, audio_busses;
    uint32_t blocksize, samplerate;
    int32_t hardware_buffer_size;
    uint32_t buffers, max_nodes, max_synthdefs;
    uint16_t use_system_clock;

    uint32_t rt_pool_size;
    uint32_t wires, rng_count;

    uint16_t load_synthdefs;
    int16_t verbosity;
    bool dump_version;
    std::vector<std::string> hw_name;
    bool memory_locking;
    uint16_t threads;

    std::vector<std::string> ugen_paths, restrict_paths;

    uint16_t input_channels, output_channels;
    std::string server_password;

    /* for non-rt synthesis */
    bool non_rt;
    std::string command_file, input_file, output_file, header_format, sample_format;

private:
    static std::unique_ptr<server_arguments> instance_;
};

} /* namespace nova */

#endif /* SERVER_SERVER_ARGS_HPP */
