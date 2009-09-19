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

#include <cstdarg>
#include <cstdio>

#include "sc_plugin_interface.hpp"
#include "sc_ugen_factory.hpp"

#include "../server/server_args.hpp"
#include "../server/memory_pool.hpp"
#include "../server/server.hpp"
#include "../simd/simd_memory.hpp"

#include "supercollider/Headers/server/SC_Samp.h"
#include "supercollider/Headers/server/SC_Prototypes.h"

namespace
{

void pause_node(int32_t node_id)
{
    nova::ugen_factory.add_pause_node(node_id);
}

} /* namespace */

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

bool define_plugincmd(const char * name, PlugInCmdFunc func, void * user_data)
{
    std::cerr << "plugin commands not implemented: " << name << std::endl;
    return false;
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
    nova::ugen_factory.add_done_node(node->mID);
}

int print(const char *fmt, ...)
{
    va_list vargs;
    va_start(vargs, fmt);

    char data[1024];

    vsprintf(data, fmt, vargs);

    std::cout << data << std::endl;

    va_end(vargs);
    return 0;
}

/* todo: we need to implement most of the done actions */
void done_action(int done_action, struct Unit *unit)
{
    switch(done_action)
    {
    case 0:
        // do nothing when the UGen is finished
        return;

    case 1:
        // pause the enclosing synth, but do not free it
        return;

    case 2:
        // free the enclosing synth
        node_end(&unit->mParent->mNode);
        return;

    case 3:
        // free both this synth and the preceding node
        return;

    case 4:
        // free both this synth and the following node
        return;

    case 5:
        // free this synth; if the preceding node is a group then do g_freeAll on it, else free it
        return;

    case 6:
        // free this synth; if the following node is a group then do g_freeAll on it, else free it
        return;

    case 7:
        //free this synth and all preceding nodes in this group
        return;

    case 8:
        //free this synth and all following nodes in this group
        return;

    case 9:
        // free this synth and pause the preceding node
        return;

    case 10:
        // free this synth and pause the following node
        return;

    case 11:
        // free this synth and if the preceding node is a group then do g_deepFree on it, else free it
        return;

    case 12:
        // free this synth and if the following node is a group then do g_deepFree on it, else free it
        return;

    case 13:
        // free this synth and all other nodes in this group (before and after)
        return;

    case 14:
        // free the enclosing group and all nodes within it (including this synth)
        return;

    default:
        return;
    }
}


} /* extern "C" */

namespace nova
{

void sc_plugin_interface::initialize(void)
{
    done_nodes.reserve(1024); // reserve enough space

    server_arguments const & args = server_arguments::instance();

    /* define functions */
    sc_interface.fDefineUnit = &define_unit;
    sc_interface.fDefineBufGen = &define_bufgen;
    sc_interface.fDefinePlugInCmd = &define_plugincmd;

    /* interface functions */
    sc_interface.fNodeEnd = &node_end;
    sc_interface.fPrint = &print;
    sc_interface.fDoneAction = &done_action;

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
    /* control busses */
    world.mControlBus = new float[args.control_busses];
    world.mControlBusTouched = new int32[args.control_busses];
    for (size_t i = 0; i != args.control_busses; ++i)
        world.mControlBusTouched[i] = -1;

    /* audio busses */
    audio_busses.initialize(args.audio_busses, args.blocksize);

    world.mAudioBus = audio_busses.buffers;
    world.mAudioBusTouched = new int32[args.audio_busses];
    world.mAudioBusLocks = audio_busses.locks;
    for (size_t i = 0; i != args.audio_busses; ++i)
        world.mAudioBusTouched[i]   = -1;

    /* audio buffers */
    world.mNumSndBufs = args.buffers;
    world.mSndBufs = new SndBuf[world.mNumSndBufs];
    world.mBufCounter = 0;

    /* audio settings */
    world.mBufLength = args.blocksize;

    Rate_Init(&world.mFullRate, args.samplerate, args.blocksize);
    Rate_Init(&world.mBufRate, float(args.samplerate)/args.blocksize, 1);

    world.mNumInputs = args.input_channels;
    world.mNumOutputs = args.output_channels;
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
