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

#include "sndfile.hh"

#include "sc_plugin_interface.hpp"
#include "sc_ugen_factory.hpp"
#include "sc_synth.hpp"

#include "../server/server_args.hpp"
#include "../server/memory_pool.hpp"
#include "../server/server.hpp"
#include "../simd/simd_memory.hpp"
#include "../utilities/malloc_aligned.hpp"
#include "../utilities/sized_array.hpp"

#include "supercollider/Headers/server/SC_Samp.h"
#include "supercollider/Headers/server/SC_Prototypes.h"
#include "supercollider/Headers/server/SC_Errors.h"
#include "supercollider/Headers/plugin_interface/clz.h"

namespace nova
{
namespace
{

void pause_node(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_pause_node(node);
}

void free_node(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);
}

void free_node_and_preceding(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        std::cerr << "parallel groups have no notion of preceding nodes" << std::endl;
        return;
    }

    server_node * preceding = node->previous_node();
    if (preceding)
        ugen_factory.add_done_node(preceding);
}

void free_node_and_pause_preceding(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        std::cerr << "parallel groups have no notion of preceding nodes" << std::endl;
        return;
    }

    server_node * preceding = node->previous_node();
    if (preceding)
        ugen_factory.add_pause_node(preceding);
}

void free_node_and_preceding_children(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        std::cerr << "parallel groups have no notion of preceding nodes" << std::endl;
        return;
    }

    server_node * preceding = node->previous_node();
    if (!preceding)
        return;
    if (preceding->is_synth())
        ugen_factory.add_done_node(preceding);
    else {
        abstract_group * preceding_group = static_cast<abstract_group*>(preceding);
        ugen_factory.add_freeAll_node(preceding_group);
    }
}


void free_node_and_preceding_deep(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        std::cerr << "parallel groups have no notion of preceding nodes" << std::endl;
        return;
    }

    server_node * preceding = node->previous_node();
    if (!preceding)
        return;
    if (preceding->is_synth())
        ugen_factory.add_done_node(preceding);
    else {
        abstract_group * preceding_group = static_cast<abstract_group*>(preceding);
        ugen_factory.add_freeDeep_node(preceding_group);
    }
}

void free_node_and_all_preceding(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        std::cerr << "parallel groups have no notion of preceding nodes" << std::endl;
        return;
    }

    for(;;)
    {
        node = node->previous_node();
        if (node)
            ugen_factory.add_done_node(node);
        else
            return;
    }
}

void free_node_and_following(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        std::cerr << "parallel groups have no notion of following nodes" << std::endl;
        return;
    }

    server_node * next = node->next_node();
    if (next)
        ugen_factory.add_done_node(next);
}

void free_node_and_pause_following(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        std::cerr << "parallel groups have no notion of following nodes" << std::endl;
        return;
    }

    server_node * next = node->next_node();
    if (next)
        ugen_factory.add_pause_node(next);
}

void free_node_and_following_children(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        std::cerr << "parallel groups have no notion of following nodes" << std::endl;
        return;
    }

    server_node * following = node->previous_node();
    if (!following)
        return;
    if (following->is_synth())
        ugen_factory.add_done_node(following);
    else {
        abstract_group * following_group = static_cast<abstract_group*>(following);
        ugen_factory.add_freeAll_node(following_group);
    }
}

void free_node_and_following_deep(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        std::cerr << "parallel groups have no notion of following nodes" << std::endl;
        return;
    }

    server_node * following = node->previous_node();
    if (!following)
        return;
    if (following->is_synth())
        ugen_factory.add_done_node(following);
    else {
        abstract_group * following_group = static_cast<abstract_group*>(following);
        ugen_factory.add_freeDeep_node(following_group);
    }
}

void free_node_and_all_following(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    ugen_factory.add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        std::cerr << "parallel groups have no notion of following nodes" << std::endl;
        return;
    }

    for(;;)
    {
        node = node->previous_node();
        if (node)
            ugen_factory.add_done_node(node);
        else
            return;
    }
}

void free_group_members(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    abstract_group * group = const_cast<abstract_group*>(node->get_parent());

    ugen_factory.add_freeAll_node(group);
}

void free_parent_group(Unit * unit)
{
    server_node * node = static_cast<sc_synth*>(unit->mParent);
    abstract_group * group = const_cast<abstract_group*>(node->get_parent());
    ugen_factory.add_done_node(group);
}

} /* namespace */
} /* namespace nova */

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
    nova::server_node * s = nova::instance->find_node(node->mID);
    nova::ugen_factory.add_done_node(s);
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
    using namespace nova;
    switch(done_action)
    {
    case 0:
        // do nothing when the UGen is finished
        return;

    case 1:
        // pause the enclosing synth, but do not free it
        nova::pause_node(unit);
        return;
    case 2:
        // free the enclosing synth
        nova::free_node(unit);
        return;

    case 3:
        // free both this synth and the preceding node
        nova::free_node_and_preceding(unit);
        return;

    case 4:
        // free both this synth and the following node
        nova::free_node_and_following(unit);
        return;

    case 5:
        // free this synth; if the preceding node is a group then do g_freeAll on it, else free it
        nova::free_node_and_preceding_children(unit);
        return;

    case 6:
        nova::free_node_and_following_children(unit);
        // free this synth; if the following node is a group then do g_freeAll on it, else free it
        return;

    case 7:
        //free this synth and all preceding nodes in this group
        nova::free_node_and_all_preceding(unit);
        return;

    case 8:
        //free this synth and all following nodes in this group
        nova::free_node_and_all_following(unit);
        return;

    case 9:
        // free this synth and pause the preceding node
        nova::free_node_and_pause_preceding(unit);
        return;

    case 10:
        // free this synth and pause the following node
        nova::free_node_and_pause_following(unit);
        return;

    case 11:
        // free this synth and if the preceding node is a group then do g_deepFree on it, else free it
        nova::free_node_and_preceding_deep(unit);
        return;

    case 12:
        // free this synth and if the following node is a group then do g_deepFree on it, else free it
        nova::free_node_and_following_deep(unit);
        return;

    case 13:
        // free this synth and all other nodes in this group (before and after)
        nova::free_group_members(unit);
        return;

    case 14:
        // free the enclosing group and all nodes within it (including this synth)
        nova::free_parent_group(unit);
        return;

    default:
        return;
    }
}

int buf_alloc(SndBuf * buf, int channels, int frames, double samplerate)
{
    return nova::ugen_factory.allocate_buffer(buf, channels, frames, samplerate);
}

} /* extern "C" */

namespace nova
{

void sc_plugin_interface::initialize(void)
{
    done_nodes.reserve(64);
    pause_nodes.reserve(16);
    freeAll_nodes.reserve(16);
    freeDeep_nodes.reserve(16);

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

    /* buffer functions */
    sc_interface.fBufAlloc = &buf_alloc;

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
    world.mSndBufsNonRealTimeMirror = new SndBuf[world.mNumSndBufs];
    world.mSndBufUpdates = new SndBufUpdates[world.mNumSndBufs];
    memset(world.mSndBufs, 0, world.mNumSndBufs*sizeof(SndBuf));
    memset(world.mSndBufsNonRealTimeMirror, 0, world.mNumSndBufs*sizeof(SndBuf));
    memset(world.mSndBufUpdates, 0, world.mNumSndBufs*sizeof(SndBufUpdates));
    world.mBufCounter = 0;

    /* audio settings */
    world.mBufLength = args.blocksize;

    Rate_Init(&world.mFullRate, args.samplerate, args.blocksize);
    Rate_Init(&world.mBufRate, float(args.samplerate)/args.blocksize, 1);

    world.mNumInputs = args.input_channels;
    world.mNumOutputs = args.output_channels;
}

void sc_done_action_handler::update_nodegraph(void)
{
    for (size_t i = 0; i != done_nodes.size(); ++i)
        instance->free_node(done_nodes[i]);
    done_nodes.clear();

    std::for_each(pause_nodes.begin(), pause_nodes.end(), boost::mem_fn(&server_node::pause));
    pause_nodes.clear();

    std::for_each(freeDeep_nodes.begin(), freeDeep_nodes.end(), boost::mem_fn(&abstract_group::free_synths_deep));
    freeDeep_nodes.clear();

    std::for_each(freeAll_nodes.begin(), freeAll_nodes.end(), boost::mem_fn(&abstract_group::free_children));
    freeAll_nodes.clear();
}

sc_plugin_interface::~sc_plugin_interface(void)
{
    delete[] world.mAudioBusTouched;
    delete[] world.mControlBus;
    delete[] world.mControlBusTouched;
    delete[] world.mSndBufs;
    delete[] world.mSndBufUpdates;
}

namespace
{

sample * allocate_buffer(size_t samples)
{
    const size_t alloc_size = samples * sizeof(sample);
    sample * ret = (sample*)calloc_aligned(alloc_size);
    if (ret)
        mlock(ret, alloc_size);
    return ret;
}

void free_buffer(sample * chunk)
{
    free_aligned(chunk);
}

inline int32 bufmask(int32 x)
{
    return (1 << (31 - CLZ(x))) - 1;
}

inline void sndbuf_init(SndBuf * buf)
{
    buf->samplerate = 0;
    buf->sampledur = 0;
    buf->data = 0;
    buf->channels = 0;
    buf->samples = 0;
    buf->frames = 0;
    buf->mask = 0;
    buf->mask1 = 0;
    buf->coord = 0;
    buf->sndfile = 0;
}

inline void sndbuf_copy(SndBuf * dest, const SndBuf * src)
{
    dest->samplerate = src->samplerate;
    dest->sampledur = src->sampledur;
    dest->data = src->data;
    dest->channels = src->channels;
    dest->samples = src->samples;
    dest->frames = src->frames;
    dest->mask = src->mask;
    dest->mask1 = src->mask1;
    dest->coord = src->coord;
    dest->sndfile = src->sndfile;
}



} /* namespace */

int sc_plugin_interface::allocate_buffer(SndBuf * buf, uint32_t frames, uint32_t channels, double samplerate)
{
    const uint32_t samples = frames * channels;
    if (samples == 0)
        return kSCErr_Failed; /* invalid buffer size */

    sample * data = nova::allocate_buffer(samples);
    if (data == NULL)
        return kSCErr_Failed; /* could not allocate memory */

    buf->data = data;

    buf->channels = channels;
    buf->frames = frames;
    buf->samples = samples;
    buf->mask = bufmask(samples); /* for delay lines */
    buf->mask1 = buf->mask - 1;    /* for oscillators */
    buf->samplerate = samplerate;
    buf->sampledur = 1.0 / samplerate;
    return kSCErr_None;
}

SndBuf * sc_plugin_interface::allocate_buffer(uint32_t index, uint32_t frames, uint32_t channels)
{
    SndBuf * buf = World_GetNRTBuf(&world, index);
    allocate_buffer(buf, frames, channels, world.mFullRate.mSampleRate);
    return buf;
}

int sc_plugin_interface::buffer_read_alloc(uint32_t index, const char * filename, uint32_t start, uint32_t frames)
{
    SndfileHandle f(filename);

    if (!f)
        return -1; /* file cannot be opened */

    const size_t sf_frames = f.frames();

    if (sf_frames > start)
        start = sf_frames;

    if (frames == 0 || frames + start > sf_frames)
        frames = sf_frames - start;

    SndBuf * buf = World_GetNRTBuf(&world, index);
    allocate_buffer(buf, frames, f.channels(), f.samplerate());

    f.seek(start, SEEK_SET);
    f.readf(buf->data, frames);
    return 0;
}

int sc_plugin_interface::buffer_alloc_read_channels(uint32_t index, const char * filename, uint32_t start,
                                                    uint32_t frames, uint32_t channel_count,
                                                    const uint32_t * channel_data)
{
    SndfileHandle f(filename);

    if (!f)
        return -1; /* file cannot be opened */

    const size_t sf_frames = f.frames();

    if (sf_frames > start)
        start = sf_frames;

    if (frames == 0 || frames + start > sf_frames)
        frames = sf_frames - start;

    SndBuf * buf = World_GetNRTBuf(&world, index);
    allocate_buffer(buf, frames, channel_count, f.samplerate());

    sample * data = buf->data;
    f.seek(start, SEEK_SET);

    sized_array<sample> read_frame(f.channels());

    for (size_t i = 0; i != frames; ++i)
    {
        f.readf(read_frame.c_array(), 1);

        for (size_t c = 0; c != channel_count; ++c)
        {
            size_t channel_mapping = channel_data[c];
            data[channel_mapping] = read_frame[c];
        }

        data += channel_count;
    }

    return 0;
}


void sc_plugin_interface::buffer_sync(uint32_t index)
{
    sndbuf_copy(world.mSndBufs + index, world.mSndBufsNonRealTimeMirror + index);
    world.mSndBufUpdates[index].writes++;
}

void sc_plugin_interface::free_buffer(uint32_t index)
{
    sndbuf_init(world.mSndBufsNonRealTimeMirror + index);
}




} /* namespace nova */
