//  prototype of a supercollider-synthdef-based synth prototype, implementation
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
#include <future>

#include <boost/filesystem/operations.hpp>

#include "sc_synth.hpp"
#include "sc_ugen_factory.hpp"
#include "sc_synth_definition.hpp"

#include "server/synth_factory.hpp"

namespace nova {

using namespace std;

void register_synthdefs(synth_factory & factory, std::vector<sc_synthdef> && defs)
{
    std::vector<sc_synthdef> synthdefs(std::move(defs));
    for (sc_synthdef & def : synthdefs) {
        sc_synth_definition * sp = new sc_synth_definition(std::move(def));
        factory.register_definition(sp);
    }
}

std::vector<sc_synthdef> sc_read_synthdefs_file(path const & file)
{
    try {
        return read_synthdef_file(file.string());
    } catch(std::exception const & e)
    {
        cout << "Exception when parsing synthdef: " << e.what() << endl;
        return std::vector<sc_synthdef>();
    }
}

std::vector<sc_synthdef> sc_read_synthdefs_dir(path const & dir)
{
    using namespace boost::filesystem;
    using namespace std;

    typedef vector<sc_synthdef> def_vector;
    def_vector ret;

    if (!exists(dir))
        return ret;

    // FIXME: some kind of threadpool would be nice!
    auto launch_policy = thread::hardware_concurrency() > 1 ? launch::async
                                                            : launch::deferred;

    vector<future<def_vector> > futures;

    recursive_directory_iterator end;
    for (recursive_directory_iterator it(dir); it != end; ++it) {
        if (!is_directory(it->status())) {
            auto path_name = it->path();
            futures.emplace_back( std::move(async(launch_policy, [=]() { return sc_read_synthdefs_file(path_name);} )) );
        }
    }

    for (future<def_vector> & synthdef_future : futures) {
        for (sc_synthdef & definition : synthdef_future.get())
            ret.emplace_back(std::move(definition));
    }

    return ret;
}

sc_synth_definition::sc_synth_definition(sc_synthdef const & sd):
    synth_definition(sd.name()), sc_synthdef(sd)
{
    typedef sc_synthdef::parameter_index_map_t::const_iterator iterator;

    for (iterator it = parameter_map.begin(); it != parameter_map.end(); ++it)
        slot_resolver::register_slot(it->first, it->second);
}


abstract_synth * sc_synth_definition::create_instance(int node_id)
{
    sc_synth * synth = new sc_synth(node_id, this);

    sc_factory->schedule_for_preparation(synth);
    return synth;
}


} /* namespace nova */
