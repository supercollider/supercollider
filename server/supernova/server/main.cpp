//  nova server
//  Copyright (C) 2008, 2009, 2010 Tim Blechmann
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

#include <wordexp.h> /**< \todo: this is posix-only */

#if (_POSIX_MEMLOCK - 0) >=  200112L
# include <sys/resource.h>
# include <sys/mman.h>
#endif

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

    const char * input_string = getenv("SC_JACK_DEFAULT_INPUTS");
    if (input_string)
    {
        string input_port(input_string);

        if (check_connection_string(input_port))
            instance->connect_all_inputs(input_port.c_str());
        else
        {
            vector<string> portnames;
            boost::split(portnames, input_port, is_any_of(":"));
            for (int i = 0; i != portnames.size(); ++i)
                instance->connect_input(i, portnames[i].c_str());
        }
    }

    const char * output_string = getenv("SC_JACK_DEFAULT_OUTPUTS");
    if (output_string)
    {
        string output_port(output_string);

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
}

void start_audio_backend(server_arguments const & args)
{
    instance->open_client("supernova", args.input_channels, args.output_channels, args.blocksize);
    instance->prepare_backend();
    instance->activate_audio();

    if (args.samplerate && args.samplerate != instance->get_samplerate()) {
        std::cout << "samplerate mismatch between command line argument and jack" << endl;
        std::cout << "forcing samplerate of " << instance->get_samplerate() << "Hz" << endl;
    }

    server_arguments::set_samplerate((uint32_t)instance->get_samplerate());
    connect_jack_ports();
    instance->start_dsp_threads();
}


#else

void start_audio_backend(server_arguments const & args)
{}

#endif

boost::filesystem::path resolve_home(void)
{
#ifdef __linux__
    wordexp_t wexp;
    int status = wordexp("~", &wexp, 0);
    if (status || wexp.we_wordc != 1)
        throw std::runtime_error("cannot detect home directory");

    path home (wexp.we_wordv[0]);
    wordfree(&wexp);
    return home;
#elif defined(__APPLE__)
    path home(getenv("HOME"));
    return home;
#else
#error platform not supported
#endif
}

void set_plugin_paths(void)
{
    server_arguments const & args = server_arguments::instance();

    if (!args.ugen_paths.empty())
    {
        foreach(std::string const & path, args.ugen_paths)
            sc_factory->load_plugin_folder(path);
    }
    else
    {
        path home = resolve_home();

#ifdef __linux__
        sc_factory->load_plugin_folder("/usr/local/lib/supernova/plugins");
        sc_factory->load_plugin_folder("/usr/lib/supernova/plugins");
        sc_factory->load_plugin_folder(home / "share/SuperCollider/supernova_plugins");
#elif defined(__APPLE__)
        sc_factory->load_plugin_folder(home / "Library/Application Support/SuperCollider/supernova_plugins/");
        sc_factory->load_plugin_folder("/Library/Application Support/SuperCollider/supernova_plugins/");
#else
        std::cerr << "Don't know how to locate plugins on this platform. Please specify search path in command line."
#endif
    }

#ifndef NDEBUG
    std::cout << "Unit Generators initialized" << std::endl;
#endif
}

void load_synthdefs(nova_server & server, server_arguments const & args)
{
    if (args.load_synthdefs) {
        path home = resolve_home();

#ifdef __linux__
        register_synthdefs(server, sc_read_synthdefs_dir(home / "share/SuperCollider/synthdefs/"));
#elif defined(__APPLE__)
        register_synthdefs(server, sc_read_synthdefs_dir(home / "Library/Application Support/SuperCollider/synthdefs/"));
        register_synthdefs(server, sc_read_synthdefs_dir("Library/Application Support/SuperCollider/synthdefs/"));
#else
        std::cerr << "Don't know how to locate synthdefs on this platform. Please load them dynamically."
#endif
    }
#ifndef NDEBUG
    std::cout << "SynthDefs loaded" << std::endl;
#endif
}

} /* namespace */

int main(int argc, char * argv[])
{
    server_arguments::initialize(argc, argv);
    server_arguments const & args = server_arguments::instance();

#if (_POSIX_MEMLOCK - 0) >=  200112L
    if (args.memory_locking)
    {
        bool lock_memory = false;

        rlimit limit;

        int failure = getrlimit(RLIMIT_MEMLOCK, &limit);
        if (failure)
            printf("getrlimit failure\n");
        else
        {
            if (limit.rlim_cur == RLIM_INFINITY and
                limit.rlim_max == RLIM_INFINITY)
                lock_memory = true;
            else
                printf("memory locking disabled due to resource limiting\n");

            if (lock_memory)
            {
                if (mlockall(MCL_FUTURE) != -1)
                    printf("memory locking enabled.\n");
            }
        }
    }
#endif


    rt_pool.init(args.rt_pool_size * 1024, args.memory_locking);

    std::cout << "Supernova booting" << std::endl;
#ifndef NDEBUG
    std::cout << "compiled for debugging" << std::endl;
#endif

    nova_server server(args);
    register_handles();

    sc_factory->initialize();

    set_plugin_paths();
    load_synthdefs(server, args);

    if (!args.non_rt)
    {
        start_audio_backend(args);

        std::cout << "Supernova ready" << std::endl;
        server.run();
    }
    else
        server.run_nonrt_synthesis(args);
    return 0;
}
