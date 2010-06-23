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

#include <boost/filesystem/operations.hpp>

#include "sc_synth.hpp"
#include "sc_ugen_factory.hpp"
#include "sc_synth_prototype.hpp"

#include "server/synth_factory.hpp"

namespace nova
{

using namespace std;

void register_synthdefs(synth_factory & factory, std::vector<sc_synthdef> const & defs)
{
    foreach(sc_synthdef const & def, defs) {
        auto_ptr<sc_synth_prototype> sp(new sc_synth_prototype(def));
        factory.register_prototype(sp.get());
        sp.release();
    }
}

std::vector<sc_synthdef> sc_read_synthdefs_file(path const & file)
{
    try {
        return read_synthdef_file(file.string());
    }
    catch(std::exception & e)
    {
        cout << "Exception when parsing synthdef: " << e.what() << endl;
        return std::vector<sc_synthdef>();
    }
}

std::vector<sc_synthdef> sc_read_synthdefs_dir(path const & dir)
{
    using namespace boost::filesystem;
    std::vector<sc_synthdef> ret;

    if (!exists(dir))
        return ret;

    directory_iterator end;
    for (directory_iterator it(dir);
         it != end; ++it)
    {
        std::vector<sc_synthdef> to_append;
        if (is_directory(it->status()))
            to_append = sc_read_synthdefs_dir(it->path());
        else
            to_append = sc_read_synthdefs_file(it->path());
        ret.insert(ret.end(), to_append.begin(), to_append.end());
    }
    return ret;
}

sc_synth_prototype::sc_synth_prototype(sc_synthdef const & sd):
    synth_prototype(sd.name()), sc_synthdef(sd)
{
    typedef sc_synthdef::parameter_map_t::const_iterator iterator;

    for (iterator it = parameter_map.begin(); it != parameter_map.end(); ++it)
        slot_resolver::register_slot(it->first, it->second);
}


abstract_synth * sc_synth_prototype::create_instance(int node_id)
{
    sc_synth * synth = new sc_synth(node_id, this);

    sc_factory->schedule_for_preparation(synth);
    return synth;
}


} /* namespace nova */
