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
#include <vector>
#include <string>

#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>

#include "server.hpp"
#include "server_args.hpp"

#include "../sc/sc_ugen_factory.hpp"
#include "../sc/sc_synth_prototype.hpp"
#include "../utilities/utils.hpp"

using namespace nova;

namespace
{

using namespace std;

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

#ifdef JACK_BACKEND
bool check_connection_string(string const & str)
{
    if (str.find(":") != string::npos) {
        std::cerr << "connecting to individual ports not yet implemented" << std::endl;
        return false;
    }
    return true;
}

void connect_jack_ports(void)
{
    using namespace boost;
    using namespace boost::algorithm;

    string input_port(getenv("SC_JACK_DEFAULT_INPUTS"));

    if (check_connection_string(input_port))
        instance->connect_all_inputs(input_port.c_str());
    else
    {
        vector<string> portnames;
        boost::split(portnames, input_port, is_any_of(":"));
        for (int i = 0; i != portnames.size(); ++i)
            instance->connect_input(i, portnames[i].c_str());
    }

    string output_port(getenv("SC_JACK_DEFAULT_OUTPUTS"));

    if (check_connection_string(output_port))
        instance->connect_all_outputs(output_port.c_str());
    else
    {
        vector<string> portnames;
        boost::split(portnames, output_port, is_any_of(":"));
        for (int i = 0; i != portnames.size(); ++i)
            instance->connect_output(i, portnames[i].c_str());
    }

}
#endif

} /* namespace */

int main(int argc, char * argv[])
{
    server_arguments::initialize(argc, argv);
    server_arguments const & args = server_arguments::instance();

    rt_pool.init(args.rt_pool_size * 1024, args.memory_locking);

    std::cout << "Supernova booting" << std::endl;
#ifndef NDEBUG
    std::cout << "compiled for debugging" << std::endl;
#endif

    sc_factory.initialize();
#ifdef __linux__
    sc_factory.load_plugin_folder("/usr/local/lib/supernova/plugins");
    sc_factory.load_plugin_folder("/usr/lib/supernova/plugins");
#else
#error "Don't know how to locate plugins on this platform"
#endif

#ifndef NDEBUG
    std::cout << "Unit Generators initialized" << std::endl;
#endif

    nova_server server(args);
    register_handles();

#if defined (JACK_BACKEND)
    server.open_client("supernova", args.input_channels, args.output_channels, args.blocksize);
    server.prepare_backend();
    server.activate_audio();

    if (args.samplerate == 0)
        server_arguments::set_samplerate((uint32_t)server.get_samplerate());
    else
        if (args.samplerate != server.get_samplerate()) {
            std::cerr << "samplerate mismatch between command line argument and jack" << endl;
            server.deactivate_audio();
            return 1;
        }

    connect_jack_ports();

#endif

    if (args.load_synthdefs) {
        boost::filesystem::path synthdef_path(getenv("HOME"));
        synthdef_path = synthdef_path / "share" / "SuperCollider" / "synthdefs";
        register_synthdefs(server, sc_read_synthdefs_dir(synthdef_path));
    }

#ifndef NDEBUG
    std::cout << "SynthDefs loaded" << std::endl;
#endif

    std::cout << "Supernova ready" << std::endl;
    server.run();

#if defined (JACK_BACKEND)
    server.deactivate_audio();
#endif
    return 0;
}
