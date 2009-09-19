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

#include "server.hpp"
#include "server_args.hpp"

#include "../sc/sc_ugen_factory.hpp"
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

    rt_pool.init(args.rt_pool_size, args.memory_locking);

    nova_server server(args.udp_port, threads);
    register_handles();

#if defined (JACK_BACKEND)
    server.open_client("supernova", args.input_channels, args.output_channels);
    server.activate_audio();
#endif

    ugen_factory.set_audio_channels(args.input_channels, args.output_channels);
    server.run();

#if defined (JACK_BACKEND)
    server.deactivate_audio();
#endif
    return 0;
}
