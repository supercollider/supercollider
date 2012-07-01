//  server arguments, implementation
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

#include <iostream>
#include <thread>

#include <boost/lexical_cast.hpp>
#include <boost/program_options.hpp>

#include "server_args.hpp"

namespace nova {

server_arguments::server_arguments(int argc, char * argv[])
{
    using namespace boost::program_options;
    using namespace std;

    /* prepare options */
    options_description options("general options");
    options.add_options()
        ("help,h", "show this help")
        ("udp-port,u", value<uint32_t>(&udp_port)->default_value(0), "udp port")
        ("tcp-port,t", value<uint32_t>(&tcp_port)->default_value(0), "tdp port")
        ("control-busses,c", value<uint32_t>(&control_busses)->default_value(4096), "number of control busses")
        ("audio-busses,a", value<uint32_t>(&audio_busses)->default_value(128), "number of audio busses")
        ("block-size,z", value<uint32_t>(&blocksize)->default_value(64), "audio block size")
        ("hardware-buffer-size,Z", value<uint32_t>()->default_value(0), "hardware buffer size")
        ("samplerate,S", value<uint32_t>(&samplerate)->default_value(44100), "hardware sample rate")
        ("buffers,b", value<uint32_t>(&buffers)->default_value(1024), "number of sample buffers")
        ("max-nodes,n", value<uint32_t>(&max_nodes)->default_value(1024), "maximum number of server nodes")
        ("max-synthdefs,d", value<uint32_t>(&max_synthdefs)->default_value(1024), "maximum number of synthdefs")
        ("rt-memory,m", value<uint32_t>(&rt_pool_size)->default_value(8192),
         "size of real-time memory pool in kb")
        ("wires,w", value<uint32_t>(&wires)->default_value(64), "number of wire buffers")
        ("randomseeds,r", value<uint32_t>(&randomseeds)->default_value(64), "number of random seeds")
        ("load-synthdefs,D", value<uint16_t>(&load_synthdefs)->default_value(1), "load synthdefs? (1 or 0)")
        ("rendezvous,R", value<uint16_t>()->default_value(1), "publish to Rendezvous? (1 or 0)")
        ("max-logins,l", value<uint32_t>()->default_value(64), "maximum number of named return addresses")
        ("password,p", value<std::string>(&server_password)->default_value(""),
                                                            "When using TCP, the session password must be the first command sent.\n"
                                                            "The default is no password.\n"
                                                            "UDP ports never require passwords, so for security use TCP.")
        ("nrt,N", value<std::vector<std::string> >()->multitoken(), "nrt synthesis <cmd-filename> <input-filename> <output-filename> <sample-rate> <header-format> <sample-format>")
        ("memory-locking,L", "enable memory locking")
        ("hardware-device-name,H", value<std::string>(&hw_name)->default_value(""), "hardware device name")
        ("verbose,v", value<int16_t>(&verbosity)->default_value(0), "verbosity: 0 is normal behaviour\n-1 suppresses informational messages\n"
                                                         "-2 suppresses informational and many error messages")
        ("ugen-search-path,U", value<std::vector<std::string> >(&ugen_paths), "a colon-separated list of paths\n"
                                                                 "if -U is specified, the standard paths are NOT searched for plugins.")
        ("restricted-path,P", value<std::vector<std::string> >(&restrict_paths), "if specified, prevents file-accessing OSC commands from accessing files outside <restricted-path>")
        ("threads,T", value<uint16_t>(&threads)->default_value(std::thread::hardware_concurrency()), "number of audio threads")
        ;

    options_description audio_options("audio options");

    audio_options.add_options()
        ("inchannels,i", value<uint16_t>(&input_channels)->default_value(8), "number of input channels")
        ("outchannels,o", value<uint16_t>(&output_channels)->default_value(8), "number of output channels")
        ;

    options_description cmdline_options;
    cmdline_options
        .add(options)
        .add(audio_options)
        ;

    /* parse options */
    boost::program_options::variables_map vm;

    try
    {
        store(command_line_parser(argc, argv).options(cmdline_options).run(), vm);
    }
    catch(error const & e)
    {
        cout << "Error when parsing command line arguments:" << endl
             << e.what() << endl << endl;
        std::exit(EXIT_FAILURE);
    };

    notify(vm);

    memory_locking = vm.count("memory-locking");

    if (vm.count("help"))
    {
        cout << cmdline_options<< endl;
        std::exit(EXIT_SUCCESS);
    }

    non_rt = vm.count("nrt");

    if (non_rt)
    {
        std::vector<std::string> const & nrt_options = vm["nrt"].as<std::vector<std::string> >();
        if (nrt_options.size() != 6)
        {
            cout << "Error when parsing command line:" << endl;
            std::exit(EXIT_FAILURE);
        }

        command_file = nrt_options[0];
        input_file = nrt_options[1];
        output_file = nrt_options[2];
        samplerate = boost::lexical_cast<uint32_t>(nrt_options[3]);
        header_format = nrt_options[4];
        sample_format = nrt_options[5];
    }
}

std::unique_ptr<server_arguments> server_arguments::instance_;

} /* namespace nova */
