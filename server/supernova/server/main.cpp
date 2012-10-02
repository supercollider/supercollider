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

#include <csignal>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string.hpp>

#include "server.hpp"
#include "server_args.hpp"

#include "../sc/sc_ugen_factory.hpp"
#include "../sc/sc_synth_definition.hpp"
#include "../utilities/utils.hpp"

#ifdef __linux__
#include <wordexp.h>
#include <sys/resource.h>
#endif

#if (_POSIX_MEMLOCK - 0) >=  200112L
# include <sys/resource.h>
# include <sys/mman.h>
#endif

#include "SC_DirUtils.h"

using namespace nova;
using namespace std;

namespace {

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
void connect_jack_ports(void)
{
    using namespace boost;
    using namespace boost::algorithm;

    const char * input_string = getenv("SC_JACK_DEFAULT_INPUTS");
    if (input_string) {
        string input_port(input_string);

        if (input_port.find(",") == string::npos)
            instance->connect_all_inputs(input_port.c_str());
        else {
            vector<string> portnames;
            boost::split(portnames, input_port, is_any_of(","));
            for (size_t i = 0; i != portnames.size(); ++i)
                instance->connect_input(i, portnames[i].c_str());
        }
    }

    const char * output_string = getenv("SC_JACK_DEFAULT_OUTPUTS");
    if (output_string) {
        string output_port(output_string);

        if (output_port.find(",") == string::npos)
            instance->connect_all_outputs(output_port.c_str());
        else {
            vector<string> portnames;
            boost::split(portnames, output_port, is_any_of(","));
            for (size_t i = 0; i != portnames.size(); ++i)
                instance->connect_output(i, portnames[i].c_str());
        }
    }
}

void get_jack_names(server_arguments const & args, string & server_name, string & client_name)
{
    client_name = "supernova";

    if (!args.hw_name.empty()) {
        vector<string> names;
        boost::split(names, args.hw_name, boost::algorithm::is_any_of(":"));

        if (names.size() == 1) {
            server_name = names[0];
        } else if (names.size() == 2) {
            server_name = names[0];
            client_name = names[1];
        } else {
            cout << "Error: cannot parse hardware device name. Using defaults." << endl;
        }
    }
}

void start_audio_backend(server_arguments const & args)
{
    string server_name, client_name;
    get_jack_names(args, server_name, client_name);

    instance->open_client(server_name, client_name, args.input_channels, args.output_channels, args.blocksize);
    instance->prepare_backend();
    instance->activate_audio();

    unsigned int real_sampling_rate = instance->get_samplerate();

    if (args.samplerate && args.samplerate != real_sampling_rate) {
        cout << "samplerate mismatch between command line argument and jack" << endl;
        cout << "forcing samplerate of " << real_sampling_rate << "Hz" << endl;

        server_arguments::set_samplerate((uint32_t)real_sampling_rate);
        sc_factory->reset_sampling_rate(real_sampling_rate);
    }

    connect_jack_ports();
    instance->start_dsp_threads();
}

#elif defined(PORTAUDIO_BACKEND)

void start_audio_backend(server_arguments const & args)
{
    bool success = instance->open_stream(args.hw_name, args.input_channels, args.hw_name, args.output_channels,
        args.samplerate, args.blocksize, args.blocksize);

    if (!success) {
        cout << "could not open portaudio device name:" << args.hw_name << endl;
        exit(1);
    }
    cout << "opened portaudio device name:" << args.hw_name << endl;
    instance->prepare_backend();
    instance->activate_audio();
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
        throw runtime_error("cannot detect home directory");

    path home (wexp.we_wordv[0]);
    wordfree(&wexp);
    return home;
#elif defined(__APPLE__)
    path home(getenv("HOME"));
    return home;
#elif defined(WIN32)
    path home(getenv("USERPROFILE"));
    return home;
#else
#error platform not supported
#endif
}

void set_plugin_paths(void)
{
    server_arguments const & args = server_arguments::instance();

    if (!args.ugen_paths.empty()) {
        for(string const & path : args.ugen_paths)
            sc_factory->load_plugin_folder(path);
    } else {
#ifdef __linux__
        path home = resolve_home();
        sc_factory->load_plugin_folder("/usr/local/lib/SuperCollider/plugins");
        sc_factory->load_plugin_folder("/usr/lib/SuperCollider/plugins");
        sc_factory->load_plugin_folder(home / "/.local/share/SuperCollider/plugins");
        sc_factory->load_plugin_folder(home / "share/SuperCollider/plugins");
#else
        char plugin_dir[MAXPATHLEN];
        sc_GetResourceDirectory(plugin_dir, MAXPATHLEN);
        sc_AppendToPath(plugin_dir, MAXPATHLEN, "plugins");

        char extension_dir[MAXPATHLEN];

        sc_GetSystemExtensionDirectory(extension_dir, MAXPATHLEN);
        sc_factory->load_plugin_folder(path(extensions_dir) / "plugins");

        sc_GetUserExtensionDirectory(extension_dir, MAXPATHLEN);
        sc_factory->load_plugin_folder(path(extensions_dir) / "plugins");
#endif
    }

#ifndef NDEBUG
    cout << "Unit Generators initialized" << endl;
#endif
}

void load_synthdef_folder(nova_server & server, path const & folder, bool verbose)
{
    if (verbose)
        std::printf("Loading synthdefs from path: %s\n", folder.c_str());

    register_synthdefs(server, std::move(sc_read_synthdefs_dir(folder)));
}

void load_synthdefs(nova_server & server, server_arguments const & args)
{
#ifndef _WIN32
    const char pathSeparator[] = ":";
#else
    const char pathSeparator[] = ";";
#endif

    if (args.load_synthdefs) {
        const char * env_synthdef_path = getenv("SC_SYNTHDEF_PATH");
        vector<path> directories;
        if (env_synthdef_path) {
            boost::split(directories, env_synthdef_path, boost::is_any_of(pathSeparator));
        } else {
            char resourceDir[MAXPATHLEN];
            if(sc_IsStandAlone())
                sc_GetResourceDirectory(resourceDir, MAXPATHLEN);
            else
                sc_GetUserAppSupportDirectory(resourceDir, MAXPATHLEN);

            directories.push_back(path(resourceDir) / "synthdefs");
        }

        for(path const & directory : directories)
            load_synthdef_folder(server, directory, args.verbosity > 0);
    }
#ifndef NDEBUG
    cout << "SynthDefs loaded" << endl;
#endif
}

void drop_rt_scheduling()
{
    bool success = nova::thread_set_priority(0);
    if (!success)
        cerr << "Warning: cannot drop rt priority" << endl;
}

void enable_core_dumps(void)
{
#ifdef __LINUX__
    rlimit core_limit = { RLIM_INFINITY, RLIM_INFINITY };
    assert( setrlimit( RLIMIT_CORE, &core_limit ) == 0 ); // enable core dumps for debug builds
#endif
}

} /* namespace */

int main(int argc, char * argv[])
{
    drop_rt_scheduling(); // when being called from sclang, we inherit a low rt-scheduling priority. but we don't want it!
    enable_core_dumps();

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

    cout << "Supernova booting" << endl;
#ifndef NDEBUG
    cout << "compiled for debugging" << endl;
#endif

    server_shared_memory_creator::cleanup(args.port());
    nova_server server(args);
    register_handles();

    set_plugin_paths();
    load_synthdefs(server, args);

    if (!args.non_rt) {
        try {
            start_audio_backend(args);
            cout << "Supernova ready" << endl;
        } catch (exception const & e) {
            cout << "Error: " << e.what() << endl;
            exit(1);
        }
        server.run();
    } else
        server.run_nonrt_synthesis(args);

    return 0;
}
