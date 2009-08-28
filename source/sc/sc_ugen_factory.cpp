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
#include "supercollider/Headers/plugin_interface/SC_Wire.h"

namespace nova
{


sc_ugen_factory ugen_factory;

sc_ugen_def::sc_ugen_def (const char *inUnitClassName, size_t inAllocSize,
                          UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags):
    name_(inUnitClassName), alloc_size(inAllocSize), ctor(inCtor), dtor(inDtor), flags(inFlags)
{}

Unit * sc_ugen_def::construct(sc_synthdef::unit_spec_t const & unit_spec)
{
    Unit * unit = (Unit*)sc_synth::allocate(alloc_size);

    /* todo: later we can pack all memory chunks to the same region */
    unit->mNumInputs  = unit_spec.input_specs.size();
    unit->mNumOutputs = unit_spec.output_specs.size();

    unit->mInput  =  (Wire**)sc_synth::allocate( unit_spec.input_specs.size() * sizeof(Wire*));
    unit->mOutput =  (Wire**)sc_synth::allocate(unit_spec.output_specs.size() * sizeof(Wire*));
    unit->mInBuf  = (float**)sc_synth::allocate( unit_spec.input_specs.size() * sizeof(float*));
    unit->mOutBuf = (float**)sc_synth::allocate(unit_spec.output_specs.size() * sizeof(float*));

    /* initialize members */
    unit->mCalcRate = unit_spec.rate;
    unit->mSpecialIndex = unit_spec.special_index;
    unit->mDone = false;
    unit->mUnitDef = reinterpret_cast<struct UnitDef*>(this); /* we abuse this field to store our reference */

    return unit;
}

void sc_ugen_def::initialize(Unit * unit, sc_synth * s, sc_synthdef::unit_spec_t const & unit_spec)
{
    unit->mParent = &s->graph;

    if (unit_spec.rate == 2)
        unit->mRate = &s->full_rate;
    else
        unit->mRate = &s->control_rate;

    /* allocate wire structs */
    size_t outputs = unit_spec.output_specs.size();

    for (size_t i = 0; i != outputs; ++i) {
        Wire * w = sc_synth::allocate<Wire>(1);

        w->mFromUnit = unit;
        w->mCalcRate = unit->mCalcRate;

        w->mBuffer = 0;
        w->mScalarValue = 0;

        if (unit->mCalcRate == 2) {
            /* allocate a new buffer */
            unit->mOutBuf[i] = (sample*)sc_synth::allocate(64 * sizeof(sample));
            w->mBuffer = unit->mOutBuf[i];
        }
        else
            unit->mOutBuf[i] = &w->mScalarValue;

        unit->mOutput[i] = w;
    }

    for (size_t i = 0; i != unit_spec.input_specs.size(); ++i)
    {
        int source = unit_spec.input_specs[i].source;
        int index = unit_spec.input_specs[i].index;

        if (source == -1)
            unit->mInput[i] = &unit->mParent->mWire[index];
        else
        {
            Unit * prev = s->units[source].unit;
            unit->mInput[i] = prev->mOutput[index];
        }

        if (unit->mInput[i]->mCalcRate == 2)
            unit->mInBuf[i] = unit->mInput[i]->mBuffer;
        else
            unit->mInBuf[i] = &unit->mInput[i]->mScalarValue;
    }

    (*ctor)(unit);
}

void sc_ugen_def::destruct(Unit * unit)
{
    (*dtor)(unit);

    /* free */
    sc_synth::free(unit->mInput);
    sc_synth::free(unit->mOutput);
    sc_synth::free(unit->mInBuf);
    sc_synth::free(unit->mOutBuf);
    sc_synth::free(unit);
}


#ifdef DLOPEN
void sc_ugen_factory::load_plugin ( boost::filesystem::path const & path )
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
void sc_ugen_factory::load_plugin ( boost::filesystem::path const & path )
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

sc_unit sc_ugen_factory::allocate_ugen(sc_synth * synth,
                                       sc_synthdef::unit_spec_t const & unit_spec)
{
    ugen_map_t::iterator it = ugen_map.find(unit_spec.name,
                                            compare_def<sc_ugen_def>());
    if (it == ugen_map.end())
        return sc_unit();

    Unit * unit = it->construct(unit_spec);
    unit->mWorld = &world;
    it->initialize(unit, synth, unit_spec);

    return sc_unit(unit);
}

void sc_ugen_factory::free_ugen(sc_unit const & unit)
{
    sc_ugen_def * def = reinterpret_cast<sc_ugen_def*>(unit.unit->mUnitDef);
    def->destruct(unit.unit);
}

} /* namespace nova */
