//  nova server
//  Copyright (C) 2008, 2009 Tim Blechmann
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
#include <cstdlib>
#include <csignal>

#include <boost/program_options.hpp>

#include "server.hpp"

#include "../audio_backend/portaudio.hpp"
#include "../sc/sc_ugen_factory.hpp"
#include "../utilities/utils.hpp"


using namespace nova;

namespace
{

using namespace std;

void dummy(void)
{}

void list_pa_devices(void)
{
    portaudio_backend<dummy> pa_backend;

    device_list devices =  pa_backend.list_devices();

    for (int i = 0; i != devices.size(); ++i)
        cout << i << ": " << devices[i].name << endl;
}

void parse_args(boost::program_options::variables_map & vm, int argc, char * argv[])
{
    using namespace boost::program_options;

    options_description options("general options");
    options.add_options()
        ("help", "show this help")
        ("port,p", value<unsigned int>()->default_value(59595), "osc port")
        ("threads,t", value<unsigned int>()->default_value(boost::thread::hardware_concurrency()), "threads")
        ("rt-memory,m", value<unsigned int>()->default_value(8192),
         "size of real-time memory pool in kb")
        ;

    options_description audio_options("audio options");

    audio_options.add_options()
        ("device,d", value<int>()->default_value(0), "portaudio device")
        ("indevice", value<int>()->default_value(-1), "input device")
        ("outdevice", value<int>()->default_value(-1), "output device")

        ("inchannels,i", value<int>()->default_value(2), "input channels")
        ("outchannels,o", value<int>()->default_value(2), "output channels")
        ("samplerate,s", value<int>()->default_value(44100), "sample rate")

        ("lsdev", "list audio devices")
        ;

    options_description cmdline_options;
    cmdline_options
        .add(options)
        .add(audio_options)
        ;

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

    if (vm.count("help"))
    {
        cout << options << endl;
        cout << audio_options << endl;
        std::exit(EXIT_SUCCESS);
    }
}

/* signal handler */
void terminate(int i)
{
    instance->terminate();
}

void register_handles(void)
{
    void (*prev_fn)(int);

    prev_fn = ::signal (SIGINT, terminate);
}

} /* namespace */

int main(int argc, char * argv[])
{
    int in_device, out_device, inchannels, outchannels, samplerate;

    unsigned int port, threads;

    {
        unsigned int rt_mem_size;
        boost::program_options::variables_map vm;

        parse_args(vm, argc, argv);

        if (vm.count("lsdev"))
        {
            list_pa_devices();
            return 0;
        }

        int device = vm["device"].as<int>();

        in_device = vm["outdevice"].as<int>();
        out_device = vm["indevice"].as<int>();

        if (in_device == -1)
            in_device = device;

        if (out_device == -1)
            out_device = device;

        inchannels = vm["inchannels"].as<int>();
        outchannels = vm["outchannels"].as<int>();

        samplerate = vm["samplerate"].as<int>();

        threads = vm["threads"].as<unsigned int>();
        threads = std::min(threads, boost::thread::hardware_concurrency());
        port = vm["port"].as<unsigned int>();

        rt_mem_size = vm["rt-memory"].as<unsigned int>() * 1024;
        rt_pool.init(rt_mem_size, true);
    }

    nova_server server(port, threads);
    register_handles();

    server.open_portaudio_backend();

    {
        device_list devs = server.list_devices();

        server.open_audio_stream(devs[in_device], inchannels,
                                 devs[out_device], outchannels,
                                 samplerate);
    }
    server.activate_audio();
    ugen_factory.set_audio_channels(inchannels, outchannels);
    server.run();

    server.deactivate_audio();
    server.close_audio_stream();
    server.close_audio_backend();

    return 0;
}
