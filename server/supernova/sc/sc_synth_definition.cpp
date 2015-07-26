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
#include <boost/range/iterator_range.hpp>

#include "sc_synth.hpp"
#include "sc_ugen_factory.hpp"
#include "sc_synth_definition.hpp"

#include "server/synth_factory.hpp"

namespace nova {

using namespace std;

void register_synthdefs(synth_factory & factory, std::vector<sc_synthdef> && defs)
{
    for (sc_synthdef & def : defs) {
        sc_synth_definition_ptr sp = new sc_synth_definition( std::move(def) );
        factory.register_definition( std::move(sp) );
    }
}

std::vector<sc_synthdef> sc_read_synthdefs_file(path const & file)
{
    try {
        return read_synthdef_file(file.string());
    } catch(std::exception const & e)
    {
        cout << "Exception when parsing synthdef file " << file.string() << ": " << e.what() << endl;
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

    for( auto const & entry : boost::make_iterator_range( recursive_directory_iterator(dir),
                                                          recursive_directory_iterator()     )) {
        if (!is_directory( entry.status() )) {
            auto path_name = entry.path();
            futures.emplace_back( std::move(async(launch_policy, [=]() { return sc_read_synthdefs_file(path_name);} )) );
        }
    }

    for (future<def_vector> & synthdef_future : futures) {
        for (sc_synthdef & definition : synthdef_future.get())
            ret.emplace_back( std::move(definition) );
    }

    return ret;
}

sc_synth_definition::sc_synth_definition(sc_synthdef && sd):
    synth_definition(sd.name()), sc_synthdef( std::forward<sc_synthdef>(sd) )
{
    std::map<int, symbol> reversed_parameter_map;
    for( auto const & elem : parameter_map)
        reversed_parameter_map.insert( std::make_pair(elem.second, elem.first));

    for (auto const & entry : parameter_map)
    {
        const symbol name           = entry.first;
        const int32_t current_index = entry.second;

        int controls_per_parameter = 1;

        for(int32_t next_index = current_index + 1; next_index != parameter_count(); ++next_index) {
            if (reversed_parameter_map.find(next_index) != reversed_parameter_map.end())
                break;
            controls_per_parameter += 1;
        }

        slot_resolver::register_slot(name, current_index, controls_per_parameter);
    }
}


abstract_synth * sc_synth_definition::create_instance(int node_id)
{
    sc_synth * synth = new sc_synth(node_id, this);
    return synth;
}


} /* namespace nova */
