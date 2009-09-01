//  interface for supercollider plugins
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

#include "sc_plugin_interface.hpp"
#include "sc_ugen_factory.hpp"

#include "../server/memory_pool.hpp"
#include "../server/server.hpp"
#include "../simd/simd_memory.hpp"

#include "supercollider/Headers/server/SC_Samp.h"

extern "C"
{

bool define_unit(const char *inUnitClassName, size_t inAllocSize,
                 UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags)
{
    try {
        nova::ugen_factory.register_ugen(inUnitClassName, inAllocSize, inCtor, inDtor, inFlags);
        return true;
    }
    catch(...)
    {
        return false;
    }
}

bool define_bufgen(const char * name, BufGenFunc func)
{
    try {
        nova::ugen_factory.register_bufgen(name, func);
        return true;
    }
    catch(...)
    {
        return false;
    }
}

void * rt_alloc(World * dummy, size_t size)
{
    if (size)
        return nova::rt_pool.malloc(size);
    else
        return NULL;
}

void * rt_realloc(World * dummy, void * ptr, size_t size)
{
    return nova::rt_pool.realloc(ptr, size);
}

void rt_free(World * dummy, void * ptr)
{
    if (ptr)
        nova::rt_pool.free(ptr);
}

void * nrt_alloc(size_t size)
{
    return malloc(size);
}

void * nrt_realloc(void * ptr, size_t size)
{
    return realloc(ptr, size);
}

void nrt_free(void * ptr)
{
    free(ptr);
}

void clear_outputs(Unit *unit, int samples)
{
    size_t outputs = unit->mNumOutputs;

    if ((samples & 15) == 0)
        for (size_t i=0; i!=outputs; ++i)
            nova::zerovec_simd(OUT(i), samples);
    else
        for (size_t i=0; i!=outputs; ++i)
            nova::zerovec(OUT(i), samples);
}

void node_end(struct Node * node)
{
    nova::spin_lock::scoped_lock lock(nova::ugen_factory.cmd_lock);
    nova::ugen_factory.done_nodes.push_back(node->mID);
}


} /* extern "C" */

namespace nova
{

sc_plugin_interface::sc_plugin_interface(void):
    audio_busses(1024, 64)
{
    done_nodes.reserve(1024); // reserve enough space

    /* define functions */
    sc_interface.fDefineUnit = &define_unit;
    sc_interface.fDefineBufGen = &define_bufgen;

    /* node functions */
    sc_interface.fNodeEnd = &node_end;

    /* wave tables */
    sc_interface.mSine = gSine;
    sc_interface.mCosecant = gInvSine;
    sc_interface.mSineSize = kSineSize;
    sc_interface.mSineWavetable = gSineWavetable;

    /* memory allocation */
    sc_interface.fRTAlloc = &rt_alloc;
    sc_interface.fRTRealloc = &rt_realloc;
    sc_interface.fRTFree = &rt_free;

    sc_interface.fNRTAlloc = &nrt_alloc;
    sc_interface.fNRTRealloc = &nrt_realloc;
    sc_interface.fNRTFree = &nrt_free;

    /* ugen functions */
    sc_interface.fClearUnitOutputs = clear_outputs;

    /* initialize world */
    world.mControlBus = new float[1024];
    world.mControlBusTouched = new int32[1024];

    /* audio busses */
    world.mAudioBus = audio_busses.buffers;
    world.mAudioBusTouched = new int32[1024];
    world.mAudioBusLocks = audio_busses.locks;

    /* audio buffers */
    world.mNumSndBufs = 1024;
    world.mSndBufs = new SndBuf[world.mNumSndBufs];
}

void sc_plugin_interface::set_audio_channels(int audio_inputs, int audio_outputs)
{
    world.mNumInputs = audio_inputs;
    world.mNumOutputs = audio_outputs;
}

void sc_plugin_interface::update_nodegraph(void)
{
    for (size_t i = 0; i != done_nodes.size(); ++i) {
        instance->free_node(done_nodes[i]);
    }
    done_nodes.clear();
}

sc_plugin_interface::~sc_plugin_interface(void)
{
    delete[] world.mAudioBusTouched;
    delete[] world.mControlBus;
    delete[] world.mControlBusTouched;
    delete[] world.mSndBufs;
}

} /* namespace nova */
