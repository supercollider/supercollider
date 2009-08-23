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

#ifndef SC_UGEN_FACTORY_HPP
#define SC_UGEN_FACTORY_HPP

#include "sc_synthdef.hpp"

namespace nova
{

class sc_ugen_factory
{
public:
    sc_unit allocate_ugen(sc_synth * synth,
                          sc_synthdef::unit_spec_t const & unit_spec)
    {
        return sc_unit();
    }
};

sc_ugen_factory ugen_factory;

} /* namespace nova */


#endif /* SC_UGEN_FACTORY_HPP */
