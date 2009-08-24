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

/* \todo for now we use dlopen, later we want to become cross-platform
 */

#define DLOPEN

#ifdef DLOPEN
#include <dlfcn.h>
#endif

#include "sc_ugen_factory.hpp"

#include "supercollider/Headers/plugin_interface/SC_InterfaceTable.h"
#include "supercollider/Headers/plugin_interface/SC_World.h"

namespace nova
{

sc_ugen_factory ugen_factory;

sc_ugen_def::sc_ugen_def (const char *inUnitClassName, size_t inAllocSize,
                          UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags):
    name_(inUnitClassName), alloc_size(inAllocSize), ctor(inCtor), dtor(inDtor), flags(inFlags)
{}

#ifdef DLOPEN
void sc_ugen_factory::load_ugen ( boost::filesystem::path const & path )
{
    void * handle = dlopen(path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle == NULL)
        return;

    void * load_symbol = dlsym(handle, "load");
    if (!load_symbol) {
        dlclose(handle);
        return;
    }

    open_handles.push_back(handle);
    LoadPlugInFunc load_func = reinterpret_cast<LoadPlugInFunc>(load_symbol);
    (*load_func)(&sc_interface);

    /* don't close the handle */
    return;
}

void sc_ugen_factory::close_handles(void)
{
    foreach(void * handle, open_handles)
        dlclose(handle);
}

#else
void sc_ugen_factory::load_ugen ( boost::filesystem::path const & path )
{
}

void sc_ugen_factory::close_handles(void)
{}
#endif

void sc_ugen_factory::register_ugen(const char *inUnitClassName, size_t inAllocSize,
                                    UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags)
{
    sc_ugen_def * def = new sc_ugen_def(inUnitClassName, inAllocSize, inCtor, inDtor, inFlags);
    ugen_map.insert(*def);
}

void sc_ugen_factory::register_bufgen(const char * name, BufGenFunc func)
{
    sc_bufgen_def * def = new sc_bufgen_def(name, func);
    bufgen_map.insert(*def);
}


} /* namespace nova */
