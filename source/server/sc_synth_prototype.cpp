//  prototype of a supercollider-synthdef-based synth prototype, implementation
//  Copyright (C) 2009 Tim Blechmann
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

#include <boost/filesystem/operations.hpp>

#include "sc_synth_prototype.hpp"
#include "synth_factory.hpp"

#include "sc_synth.hpp"

namespace nova
{

using namespace std;

void sc_read_synthdef(synth_factory & factory, path const & file)
{
    try {
        auto_ptr<sc_synth_prototype> sp(new sc_synth_prototype(sc_synthdef(file)));
        factory.register_prototype(sp.get());
        sp.release();
    }
    catch(std::exception & e)
    {
        cout << "Exception when parsing synthdef: " << e.what() << endl;
    }
}


void sc_read_synthdefs_dir(synth_factory & factory, path const & dir)
{
    using namespace boost::filesystem;

    if (!exists(dir))
        return;

    directory_iterator end;
    for (directory_iterator it(dir);
         it != end; ++it)
    {
        if (is_directory(it->status()))
            sc_read_synthdefs_dir(factory, it->path());
        else
        {
            try {
                std::vector<sc_synthdef> defs = read_synthdef_file(it->path().string());
                foreach(sc_synthdef const & def, defs) {
                    auto_ptr<sc_synth_prototype> sp(new sc_synth_prototype(def));
                    factory.register_prototype(sp.get());
                    sp.release();
                }
            }
            catch(std::exception & e)
            {
                cout << "Exception when parsing synthdef: " << e.what() << endl;
            }
        }
    }
}

sc_unit allocate_unit_generator(sc_synth * synth/* , */
/*                                 sc_synthdef::unit_spec_t const & unit_spec */)
{
    return sc_unit();
}


abstract_synth * sc_synth_prototype::create_instance(int node_id)
{
    typedef sc_synthdef::graph_t graph_t;

    sc_synth * synth = new sc_synth(node_id, this);

    /* allocate wire buffers
     *
     * \todo we allocate one buffer per ugen, output, which is very inefficient,
     *       but we can optimize it later
     * */
    unsigned int control_buffers, audio_buffers;
    for (graph_t::iterator it = synthdef.graph.begin();
         it != synthdef.graph.end(); ++it) {
        for (int i = 0; i != it->output_specs.size(); ++i)
            if (it->output_specs[i] == 2)
                ++audio_buffers;
            else
                ++control_buffers;
    }

    synth->allocate_unit_buffers(64, audio_buffers, control_buffers);

    /* allocate unit generators */
    for (graph_t::iterator it = synthdef.graph.begin();
         it != synthdef.graph.end(); ++it) {
        sc_unit unit = allocate_unit_generator(synth/* , *it */);
        synth->units.push_back(unit);
    }

    return synth;
}


} /* namespace nova */
