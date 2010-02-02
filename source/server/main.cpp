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

} /* namespace */

int main(int argc, char * argv[])
{
    server_arguments::initialize(argc, argv);
    server_arguments const & args = server_arguments::instance();

    uint32_t threads = std::min(uint(args.threads), boost::thread::hardware_concurrency());

    rt_pool.init(args.rt_pool_size * 1024, args.memory_locking);

    sc_factory.initialize();
#ifdef __linux__
    sc_factory.load_plugin_folder("/usr/local/lib/supernova/plugins");
    sc_factory.load_plugin_folder("/usr/lib/supernova/plugins");
#else
#error "Don't know how to locate plugins on this platform"
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
#endif

    if (args.load_synthdefs) {
        boost::filesystem::path synthdef_path(getenv("HOME"));
        synthdef_path = synthdef_path / "share" / "SuperCollider" / "synthdefs";
        register_synthdefs(server, sc_read_synthdefs_dir(synthdef_path));
    }

    server.run();

#if defined (JACK_BACKEND)
    server.deactivate_audio();
#endif
    return 0;
}
