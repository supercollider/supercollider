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

#include <iostream>

#include <boost/filesystem/operations.hpp>

#include "sc_synth.hpp"
#include "sc_synth_prototype.hpp"

#include "server/synth_factory.hpp"

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


abstract_synth * sc_synth_prototype::create_instance(int node_id)
{
    sc_synth * synth = new sc_synth(node_id, this);

    return synth;
}


} /* namespace nova */
