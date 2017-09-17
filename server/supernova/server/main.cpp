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

#include "SC_Version.hpp"

#include "../sc/sc_ugen_factory.hpp"
#include "../sc/sc_synth_definition.hpp"
#include "../utilities/utils.hpp"

#if defined(__linux__) || defined(__FreeBSD__) || defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
#include <wordexp.h>
#include <sys/resource.h>
#endif

#ifdef __APPLE__
# include <ApplicationServices/ApplicationServices.h>
# include <SC_Apple.hpp>
#endif


#if (_POSIX_MEMLOCK - 0) >=  200112L
# include <sys/resource.h>
# include <sys/mman.h>
#endif

#include "SC_Filesystem.hpp"
#ifndef _WIN32
    const char pathSeparator[] = ":";
#else
    const char pathSeparator[] = ";";
#endif

using namespace nova;
using namespace std;
using DirName = SC_Filesystem::DirName;

namespace {

/* signal handler */
void terminate(int i)
{
    instance->terminate();
}

void register_signal_handler(void)
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

    if (!args.hw_name.empty() && !args.hw_name[0].empty()) {
        vector<string> names;
        boost::split(names, args.hw_name[0], boost::algorithm::is_any_of(":"));

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
}

#elif defined(PORTAUDIO_BACKEND)

void start_audio_backend(server_arguments const & args)
{
    int input_channels = args.input_channels;
    int output_channels = args.output_channels;

    std::string input_device, output_device;
    if (args.hw_name.empty()) {
        boost::tie(input_device, output_device) = instance->default_device_names();
    } else if (args.hw_name.size() == 1) {
        input_device = output_device = args.hw_name[0];
    } else {
        input_device = args.hw_name[0];
        output_device = args.hw_name[1];
    }

    cout << "opening portaudio device name: ";
    cout << input_device << " / " << output_device << endl;

    if (input_device == "nil") {
        input_device.clear();
        input_channels = 0;
    }

    if (output_device == "nil") {
        output_device.clear();
        output_channels = 0;
    }

    cout << "opening portaudio device name: ";
    cout << input_device << " / " << output_device << endl;


    bool success = instance->open_stream(input_device, input_channels, output_device, output_channels,
        args.samplerate, args.blocksize, args.hardware_buffer_size);

    if (!success) {
        cout << "could not open portaudio device name: " << input_device << " / " << output_device << endl;
        exit(1);
    }
    cout << "opened portaudio device name: ";
    cout << input_device << " / " << output_device << endl;

    instance->report_latency();

    instance->prepare_backend();
    instance->activate_audio();
}

#else

void start_audio_backend(server_arguments const & args)
{}

#endif

boost::filesystem::path resolve_home(void)
{
#if defined(__linux__) || defined(__FreeBSD__) || defined(__DragonFly__) || defined(__OpenBSD__) || defined(__NetBSD__)
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
#elif defined(_WIN32)
    path home(getenv("USERPROFILE"));
    return home;
#else
#error platform not supported
#endif
}

void set_plugin_paths(server_arguments const & args, nova::sc_ugen_factory * factory)
{
    if (!args.ugen_paths.empty()) {
        for(path const & path1 : args.ugen_paths){
            vector<std::string> directories;
            boost::split(directories, path1.string(), boost::is_any_of(pathSeparator));
            for(string const & path : directories){
                factory->load_plugin_folder(path);
            }
        }
    } else {
#ifdef __linux__
        const path home = resolve_home();
        std::vector<path> folders = { "/usr/local/lib/SuperCollider/plugins",
                                      "/usr/lib" LIB_SUFFIX "/SuperCollider/plugins",
                                      home / "/.local/share/SuperCollider/Extensions",
                                      home / "share/SuperCollider/plugins" };

        for (path const & folder : folders)
            factory->load_plugin_folder(folder);
#else
        factory->load_plugin_folder(SC_Filesystem::instance().getDirectory(DirName::Resource) / "plugins");
        factory->load_plugin_folder(SC_Filesystem::instance().getDirectory(DirName::SystemExtension) / "plugins");
        factory->load_plugin_folder(SC_Filesystem::instance().getDirectory(DirName::UserExtension) / "plugins");
#endif
    }

#ifndef NDEBUG
    cout << "Unit Generators initialized" << endl;
#endif
}

void load_synthdef_folder(nova_server & server, path const & folder, bool verbose)
{
    if (verbose)
        std::cout << "Loading synthdefs from path: " << folder.string() << std::endl;

    register_synthdefs(server, std::move(sc_read_synthdefs_dir(folder)));
}

void load_synthdefs(nova_server & server, server_arguments const & args)
{
    using namespace std;

#ifndef NDEBUG
    auto start_time = std::chrono::high_resolution_clock::now();
#endif

    if (args.load_synthdefs) {
        const char * env_synthdef_path = getenv("SC_SYNTHDEF_PATH");
        vector<path> directories;
        if (env_synthdef_path) {
            boost::split(directories, env_synthdef_path, boost::is_any_of(pathSeparator));
        } else {
            path synthdef_path = SC_Filesystem::instance().getDirectory(DirName::UserAppSupport);
            directories.push_back(synthdef_path / "synthdefs");
        }

        for(path const & directory : directories)
            load_synthdef_folder(server, directory, args.verbosity > 0);
    }
#ifndef NDEBUG
    auto end_time = std::chrono::high_resolution_clock::now();
    cout << "SynthDefs loaded in "
         << std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count()
         << " ms"
         << endl;
#endif
}

void drop_rt_scheduling()
{
    bool success = nova::thread_set_priority(0);
    if (!success)
        cout << "Warning: cannot drop rt priority" << endl;
}

void enable_core_dumps(void)
{
#ifdef __LINUX__
    rlimit core_limit = { RLIM_INFINITY, RLIM_INFINITY };
    assert( setrlimit( RLIMIT_CORE, &core_limit ) == 0 ); // enable core dumps for debug builds
#endif
}

void lock_memory(server_arguments const & args)
{
#if (_POSIX_MEMLOCK - 0) >=  200112L
    if (args.memory_locking) {
        bool lock_memory = false;

        rlimit limit;

        int failure = getrlimit(RLIMIT_MEMLOCK, &limit);
        if (failure)
            printf("getrlimit failure\n");
        else {
            if (limit.rlim_cur == RLIM_INFINITY and
                limit.rlim_max == RLIM_INFINITY)
                lock_memory = true;
            else
                printf("memory locking disabled due to resource limiting\n");

            if (lock_memory) {
                if (mlockall(MCL_FUTURE) != -1)
                    printf("memory locking enabled.\n");
            }
        }
    }
#endif
}

} /* namespace */

int main(int argc, char * argv[])
{
    drop_rt_scheduling(); // when being called from sclang, we inherit a low rt-scheduling priority. but we don't want it!
    enable_core_dumps();

    server_arguments::initialize(argc, argv);
    server_arguments const & args = server_arguments::instance();

    if(args.dump_version){
        cout << "supernova " << SC_VersionString() << " (" << SC_BuildString() << ")" << endl;
        return 0;
    }

#ifdef __APPLE__
    ProcessSerialNumber psn;
    if (GetCurrentProcess(&psn) == noErr) {
        TransformProcessType(&psn, kProcessTransformToUIElementApplication);
    }

    SC::Apple::disableAppNap();
#endif

    rt_pool.init(args.rt_pool_size * 1024, args.memory_locking);
    lock_memory(args);

    cout << "Supernova booting" << endl;
#ifndef NDEBUG
    cout << "compiled for debugging" << endl;
#endif

    server_shared_memory_creator::cleanup(args.port());
    nova_server server(args);
    register_signal_handler();

    set_plugin_paths(args, sc_factory.get());
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
