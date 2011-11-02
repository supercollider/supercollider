//  prototype of a supercollider-synthdef-based synth prototype
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

#ifndef SC_SYNTH_PROTOTYPE_HPP
#define SC_SYNTH_PROTOTYPE_HPP

#include <boost/filesystem/path.hpp>

#include "sc_synthdef.hpp"

#include "server/synth_prototype.hpp"
#include "utilities/sized_array.hpp"

namespace nova {

using boost::filesystem::path;

/* read synthdefs from path pattern */
std::vector<sc_synthdef> sc_read_synthdefs_file(path const & filename);
std::vector<sc_synthdef> sc_read_synthdefs_dir(path const & dir);

class sc_synth_prototype:
    public synth_prototype,
    public sc_synthdef
{
public:
    sc_synth_prototype(sc_synthdef const & sd);

private:
    friend class sc_synth;

    virtual abstract_synth * create_instance(int);
};

typedef boost::intrusive_ptr<sc_synth_prototype> sc_synth_prototype_ptr;

void register_synthdefs(class synth_factory & factory, std::vector<sc_synthdef> const &);

#ifdef BOOST_HAS_RVALUE_REFS
void register_synthdefs(class synth_factory & factory, std::vector<sc_synthdef> &&);
#endif

} /* namespace nova */

#endif /* SC_SYNTH_PROTOTYPE_HPP */
