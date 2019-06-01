//  interface for supercollider plugins
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

#include <cstdarg>
#include <random>

#include "SC_SndFileHelpers.hpp"

#include "sc_plugin_interface.hpp"
#include "sc_ugen_factory.hpp"
#include "sc_synth.hpp"

#include "nova-simd/simd_memory.hpp"

#include "../server/server_args.hpp"
#include "../server/memory_pool.hpp"
#include "../server/server.hpp"
#include "malloc_aligned.hpp"
#include "../utilities/sized_array.hpp"

#include "SC_Prototypes.h"
#include "SC_Unit.h"
#include "SC_Lock.h"
#include "clz.h"
#include "SC_fftlib.h"
#include "SC_Lock.h"
#include "../../common/Samp.hpp"
#include "../../common/SC_SndFileHelpers.hpp"

#include <boost/math/constants/constants.hpp>

// undefine the shadowed scfft functions
#undef scfft_create
#undef scfft_dofft
#undef scfft_doifft
#undef scfft_destroy

namespace nova {

spin_lock log_guard; // needs to be acquired for logging from the helper threads!

namespace {

spin_lock rt_pool_guard;

inline Node* as_Node(server_node* node) {
    if (node == nullptr)
        return nullptr;

    // hack!!! we only assume that the 32bit integer mID member can be accessed via Node
    if (node->is_synth()) {
        sc_synth* s = static_cast<sc_synth*>(node);
        return &s->mNode;
    } else {
        void* nodePointer = &node->node_id;
        return (Node*)nodePointer;
    }
}

void pause_node(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_pause_node(node);
}

void free_node(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);
}

void free_node_and_preceding(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        spin_lock::scoped_lock lock(log_guard);
        log("parallel groups have no notion of preceding nodes\n");
        return;
    }

    server_node* preceding = node->previous_node();
    if (preceding)
        sc_factory->add_done_node(preceding);
}

void free_node_and_pause_preceding(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        spin_lock::scoped_lock lock(log_guard);
        log("parallel groups have no notion of preceding nodes\n");
        return;
    }

    server_node* preceding = node->previous_node();
    if (preceding)
        sc_factory->add_pause_node(preceding);
}

void free_node_and_preceding_children(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        spin_lock::scoped_lock lock(log_guard);
        log("parallel groups have no notion of preceding nodes");
        return;
    }

    server_node* preceding = node->previous_node();
    if (!preceding)
        return;
    if (preceding->is_synth())
        sc_factory->add_done_node(preceding);
    else {
        abstract_group* preceding_group = static_cast<abstract_group*>(preceding);
        sc_factory->add_freeAll_node(preceding_group);
    }
}


void free_node_and_preceding_deep(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        log("parallel groups have no notion of preceding nodes\n");
        return;
    }

    server_node* preceding = node->previous_node();
    if (!preceding)
        return;
    if (preceding->is_synth())
        sc_factory->add_done_node(preceding);
    else {
        abstract_group* preceding_group = static_cast<abstract_group*>(preceding);
        sc_factory->add_freeDeep_node(preceding_group);
    }
}

void free_node_and_all_preceding(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        spin_lock::scoped_lock lock(log_guard);
        log("parallel groups have no notion of preceding nodes\n");
        return;
    }

    for (;;) {
        node = node->previous_node();
        if (node)
            sc_factory->add_done_node(node);
        else
            return;
    }
}

void free_node_and_following(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        spin_lock::scoped_lock lock(log_guard);
        log("parallel groups have no notion of following nodes\n");
        return;
    }

    server_node* next = node->next_node();
    if (next)
        sc_factory->add_done_node(next);
}

void free_node_and_pause_following(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        spin_lock::scoped_lock lock(log_guard);
        log("parallel groups have no notion of following nodes\n");
        return;
    }

    server_node* next = node->next_node();
    if (next)
        sc_factory->add_pause_node(next);
}

void free_node_and_resume_following(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        spin_lock::scoped_lock lock(log_guard);
        log("parallel groups have no notion of following nodes\n");
        return;
    }

    server_node* next = node->next_node();
    if (next)
        sc_factory->add_resume_node(next);
}

void free_node_and_following_children(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        spin_lock::scoped_lock lock(log_guard);
        log("parallel groups have no notion of following nodes\n");
        return;
    }

    server_node* following = node->previous_node();
    if (!following)
        return;
    if (following->is_synth())
        sc_factory->add_done_node(following);
    else {
        abstract_group* following_group = static_cast<abstract_group*>(following);
        sc_factory->add_freeAll_node(following_group);
    }
}

void free_node_and_following_deep(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        spin_lock::scoped_lock lock(log_guard);
        log("parallel groups have no notion of following nodes\n");
        return;
    }

    server_node* following = node->previous_node();
    if (!following)
        return;
    if (following->is_synth())
        sc_factory->add_done_node(following);
    else {
        abstract_group* following_group = static_cast<abstract_group*>(following);
        sc_factory->add_freeDeep_node(following_group);
    }
}

void free_node_and_all_following(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    sc_factory->add_done_node(node);

    if (node->get_parent()->is_parallel()) {
        spin_lock::scoped_lock lock(log_guard);
        log("parallel groups have no notion of following nodes\n");
        return;
    }

    for (;;) {
        node = node->previous_node();
        if (node)
            sc_factory->add_done_node(node);
        else
            return;
    }
}

void free_group_members(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    abstract_group* group = const_cast<abstract_group*>(node->get_parent());

    sc_factory->add_freeAll_node(group);
}

void free_parent_group(Unit* unit) {
    server_node* node = static_cast<sc_synth*>(unit->mParent);
    abstract_group* group = const_cast<abstract_group*>(node->get_parent());
    sc_factory->add_done_node(group);
}

bool get_scope_buffer(World* inWorld, int index, int channels, int maxFrames, ScopeBufferHnd& hnd) {
    scope_buffer_writer writer = instance->get_scope_buffer_writer(index, channels, maxFrames);

    if (writer.valid()) {
        hnd.internalData = writer.buffer;
        hnd.data = writer.data();
        hnd.channels = channels;
        hnd.maxFrames = maxFrames;
        return true;
    } else {
        hnd.internalData = nullptr;
        return false;
    }
}

void push_scope_buffer(World* inWorld, ScopeBufferHnd& hnd, int frames) {
    scope_buffer_writer writer(reinterpret_cast<scope_buffer*>(hnd.internalData));
    writer.push(frames);
    hnd.data = writer.data();
}

void release_scope_buffer(World* inWorld, ScopeBufferHnd& hnd) {
    scope_buffer_writer writer(reinterpret_cast<scope_buffer*>(hnd.internalData));
    instance->release_scope_buffer_writer(writer);
}

} /* namespace */
} /* namespace nova */

extern "C" {

bool define_unit(const char* inUnitClassName, size_t inAllocSize, UnitCtorFunc inCtor, UnitDtorFunc inDtor,
                 uint32 inFlags) {
    try {
        nova::sc_factory->register_ugen(inUnitClassName, inAllocSize, inCtor, inDtor, inFlags);
        return true;
    } catch (...) {
        return false;
    }
}

bool define_bufgen(const char* name, BufGenFunc func) {
    try {
        nova::sc_factory->register_bufgen(name, func);
        return true;
    } catch (...) {
        return false;
    }
}

bool define_unitcmd(const char* unitClassName, const char* cmdName, UnitCmdFunc inFunc) {
    return nova::sc_factory->register_ugen_command_function(unitClassName, cmdName, inFunc);
}

bool define_plugincmd(const char* name, PlugInCmdFunc func, void* user_data) {
    return nova::sc_factory->register_cmd_plugin(name, func, user_data);
}

void* rt_alloc(World* dummy, size_t size) {
    nova::spin_lock::scoped_lock lock(nova::rt_pool_guard);
    if (size)
        return nova::rt_pool.malloc(size);
    else
        return nullptr;
}

void* rt_realloc(World* dummy, void* ptr, size_t size) {
    nova::spin_lock::scoped_lock lock(nova::rt_pool_guard);
    return nova::rt_pool.realloc(ptr, size);
}

void rt_free(World* dummy, void* ptr) {
    nova::spin_lock::scoped_lock lock(nova::rt_pool_guard);
    if (ptr)
        nova::rt_pool.free(ptr);
}

void* nrt_alloc(size_t size) { return malloc(size); }

void* nrt_realloc(void* ptr, size_t size) { return realloc(ptr, size); }

void nrt_free(void* ptr) { free(ptr); }

void* nrt_alloc_2(World* dummy, size_t size) { return malloc(size); }

void* nrt_realloc_2(World* dummy, void* ptr, size_t size) { return realloc(ptr, size); }

void nrt_free_2(World* dummy, void* ptr) { free(ptr); }


void clear_outputs(Unit* unit, int samples) {
    const uint32_t outputs = unit->mNumOutputs;

    if ((samples & 63) == 0) {
        const uint32_t loops = samples / 64;

        for (int i = 0; i != outputs; ++i) {
            float* buffer = unit->mOutBuf[i];
            for (int loop = 0; loop != loops; ++loop) {
                nova::zerovec_simd<64>(buffer + loop * 64);
            }
        }
        return;
    }

    if ((samples & 15) == 0) {
        for (int i = 0; i != outputs; ++i)
            nova::zerovec_simd(unit->mOutBuf[i], samples);
        return;
    }

    for (int i = 0; i != outputs; ++i)
        nova::zerovec(unit->mOutBuf[i], samples);
}

void node_end(struct Node* node) {
    nova::server_node* s = nova::instance->find_node(node->mID);
    nova::sc_factory->add_done_node(s);
}

void node_set_run(struct Node* node, int run) {
    using namespace nova;
    server_node* s = instance->find_node(node->mID);

    if (run == 0)
        sc_factory->add_pause_node(s);
    else
        sc_factory->add_resume_node(s);
}


int print(const char* fmt, ...) {
    va_list vargs;
    va_start(vargs, fmt);

    nova::log_guard.lock();
    bool status = nova::instance->log_printf(fmt, vargs);
    nova::log_guard.unlock();

    va_end(vargs);
    return (status == true) ? 0 : -1;
}

/* todo: we need to implement most of the done actions */
void done_action(int done_action, struct Unit* unit) {
    using namespace nova;
    switch (done_action) {
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
        // free this synth and all preceding nodes in this group
        nova::free_node_and_all_preceding(unit);
        return;

    case 8:
        // free this synth and all following nodes in this group
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

    case 15:
        // free this synth and resume the following node
        nova::free_node_and_resume_following(unit);
        return;

    default:
        return;
    }
}

int buf_alloc(SndBuf* buf, int channels, int frames, double samplerate) {
    try {
        nova::sc_factory->allocate_buffer(buf, channels, frames, samplerate);
        return kSCErr_None;
    } catch (std::exception const& e) {
        std::cout << e.what() << std::endl;
        return kSCErr_Failed;
    }
}

void send_trigger(Node* unit, int trigger_id, float value) {
    nova::instance->send_trigger(unit->mID, trigger_id, value);
}

void world_lock(World* world) { reinterpret_cast<SC_Lock*>(world->mNRTLock)->lock(); }

void world_unlock(World* world) { reinterpret_cast<SC_Lock*>(world->mNRTLock)->unlock(); }

Node* get_node(World* world, int id) {
    nova::server_node* node = nova::instance->find_node(id);
    return nova::as_Node(node);
}

void send_node_reply(Node* node, int reply_id, const char* command_name, int argument_count, const float* values) {
    if (!nova::sc_factory->world.mRealTime)
        return;

    nova::instance->send_node_reply(node->mID, reply_id, command_name, argument_count, values);
}

int do_asynchronous_command(
    World* inWorld, void* replyAddr, const char* cmdName, void* cmdData,
    AsyncStageFn stage2, // stage2 is non real time
    AsyncStageFn stage3, // stage3 is real time - completion msg performed if stage3 returns true
    AsyncStageFn stage4, // stage4 is non real time - sends done if stage4 returns true
    AsyncFreeFn cleanup, int completionMsgSize, void* completionMsgData) {
    nova::instance->do_asynchronous_command(inWorld, replyAddr, cmdName, cmdData, stage2, stage3, stage4, cleanup,
                                            completionMsgSize, completionMsgData);
    return 0;
}

} /* extern "C" */

namespace nova {


inline void initialize_rate(Rate& rate, double sample_rate, int blocksize) {
    rate.mSampleRate = sample_rate;
    rate.mSampleDur = 1. / sample_rate;
    rate.mRadiansPerSample = 2 * boost::math::constants::pi<double>() / sample_rate;

    rate.mBufLength = blocksize;
    rate.mBufDuration = blocksize / sample_rate;
    rate.mBufRate = sample_rate / blocksize;
    rate.mSlopeFactor = 1. / blocksize;

    div_t div_result = std::div(blocksize, 3);
    rate.mFilterLoops = div_result.quot;
    rate.mFilterRemain = div_result.rem;
    if (rate.mFilterLoops == 0.)
        rate.mFilterSlope = 0.;
    else
        rate.mFilterSlope = 1. / rate.mFilterLoops;
}


void sc_plugin_interface::initialize(server_arguments const& args, float* control_busses) {
    const bool nrt_mode = args.non_rt;

    done_nodes.reserve(64);
    pause_nodes.reserve(16);
    resume_nodes.reserve(16);
    freeAll_nodes.reserve(16);
    freeDeep_nodes.reserve(16);

    /* define functions */
    sc_interface.fDefineUnit = &define_unit;
    sc_interface.fDefineBufGen = &define_bufgen;
    sc_interface.fDefinePlugInCmd = &define_plugincmd;
    sc_interface.fDefineUnitCmd = &define_unitcmd;

    /* interface functions */
    sc_interface.fNodeEnd = &node_end;
    sc_interface.fGetNode = &get_node;
    sc_interface.fNodeRun = &node_set_run;
    sc_interface.fPrint = &print;
    sc_interface.fDoneAction = &done_action;

    /* sndfile functions */
#ifdef NO_LIBSNDFILE
    sc_interface.fSndFileFormatInfoFromStrings = nullptr;
#else
    sc_interface.fSndFileFormatInfoFromStrings = &sndfileFormatInfoFromStrings;
#endif

    /* wave tables */
    sc_interface.mSine = gSine;
    sc_interface.mCosecant = gInvSine;
    sc_interface.mSineSize = kSineSize;
    sc_interface.mSineWavetable = gSineWavetable;

    /* memory allocation */
    if (!nrt_mode) {
        sc_interface.fRTAlloc = &rt_alloc;
        sc_interface.fRTRealloc = &rt_realloc;
        sc_interface.fRTFree = &rt_free;
    } else {
        sc_interface.fRTAlloc = &nrt_alloc_2;
        sc_interface.fRTRealloc = &nrt_realloc_2;
        sc_interface.fRTFree = &nrt_free_2;
    }

    sc_interface.fNRTAlloc = &nrt_alloc;
    sc_interface.fNRTRealloc = &nrt_realloc;
    sc_interface.fNRTFree = &nrt_free;

    /* ugen functions */
    sc_interface.fClearUnitOutputs = clear_outputs;

    /* buffer functions */
    sc_interface.fBufAlloc = &buf_alloc;

    /* trigger functions */
    sc_interface.fSendTrigger = &send_trigger;
    sc_interface.fSendNodeReply = &send_node_reply;

    /* world locks */
    sc_interface.fNRTLock = &world_lock;
    sc_interface.fNRTUnlock = &world_unlock;
    world.mNRTLock = new SC_Lock();

    /* fft library */
    sc_interface.fSCfftCreate = &scfft_create;
    sc_interface.fSCfftDestroy = &scfft_destroy;
    sc_interface.fSCfftDoFFT = &scfft_dofft;
    sc_interface.fSCfftDoIFFT = &scfft_doifft;

    /* scope API */
    sc_interface.fGetScopeBuffer = &get_scope_buffer;
    sc_interface.fPushScopeBuffer = &push_scope_buffer;
    sc_interface.fReleaseScopeBuffer = &release_scope_buffer;

    /* osc plugins */
    sc_interface.fDoAsynchronousCommand = &do_asynchronous_command;

    /* initialize world */
    /* control busses */
    world.mControlBus = control_busses;
    world.mNumControlBusChannels = args.control_busses;
    world.mControlBusTouched = new int32[args.control_busses];
    std::fill(world.mControlBusTouched, world.mControlBusTouched + args.control_busses, -1);

    /* audio busses */
    audio_busses.initialize(args.audio_busses, args.blocksize);

    world.mAudioBus = audio_busses.buffers;
    world.mNumAudioBusChannels = args.audio_busses;
    world.mAudioBusTouched = new int32[args.audio_busses];
    world.mAudioBusLocks = audio_busses.locks;
    world.mControlBusLock = new spin_lock();
    std::fill(world.mAudioBusTouched, world.mAudioBusTouched + args.audio_busses, -1);

    /* audio buffers */
    world.mNumSndBufs = args.buffers;
    world.mSndBufs = new SndBuf[world.mNumSndBufs];
    world.mSndBufsNonRealTimeMirror = new SndBuf[world.mNumSndBufs];
    world.mSndBufUpdates = new SndBufUpdates[world.mNumSndBufs];
    memset(world.mSndBufs, 0, world.mNumSndBufs * sizeof(SndBuf));
    memset(world.mSndBufsNonRealTimeMirror, 0, world.mNumSndBufs * sizeof(SndBuf));
    memset(world.mSndBufUpdates, 0, world.mNumSndBufs * sizeof(SndBufUpdates));
    world.mBufCounter = 0;

    async_buffer_guards.reset(new std::mutex[world.mNumSndBufs]);

    /* audio settings */
    world.mBufLength = args.blocksize;
    world.mSampleRate = args.samplerate;

    initialize_rate(world.mFullRate, args.samplerate, args.blocksize);
    initialize_rate(world.mBufRate, double(args.samplerate) / args.blocksize, 1);

    world.mNumInputs = args.input_channels;
    world.mNumOutputs = args.output_channels;

    world.mRealTime = !args.non_rt;
    world.mVerbosity = args.verbosity;

    /* rngs */
    world.mNumRGens = args.rng_count;
    world.mRGen = new RGen[world.mNumRGens];
    std::vector<std::uint32_t> seeds(world.mNumRGens);

    try {
        std::random_device rd;
        std::seed_seq seq({ rd(), rd(), rd() });
        seq.generate(seeds.begin(), seeds.end());
    } catch (...) {
        auto now = std::chrono::high_resolution_clock::now();

        auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());

        std::seed_seq seq({ seconds.count() });
        seq.generate(seeds.begin(), seeds.end());
    }

    for (int i = 0; i < world.mNumRGens; ++i)
        world.mRGen[i].init(seeds[i]);
}

void sc_plugin_interface::reset_sampling_rate(int sr) {
    world.mSampleRate = sr;

    initialize_rate(world.mFullRate, sr, world.mBufLength);
    initialize_rate(world.mBufRate, double(sr) / world.mBufLength, 1);
}


void sc_done_action_handler::apply_done_actions(void) {
    for (server_node* node : done_nodes)
        instance->free_node(node);
    done_nodes.clear();

    for (server_node* node : resume_nodes)
        instance->node_resume(node);
    resume_nodes.clear();

    for (server_node* node : pause_nodes)
        instance->node_pause(node);
    pause_nodes.clear();

    for (abstract_group* group : freeDeep_nodes)
        instance->group_free_deep(group);
    freeDeep_nodes.clear();

    for (abstract_group* group : freeAll_nodes)
        instance->group_free_all(group);
    freeAll_nodes.clear();
}

sc_plugin_interface::~sc_plugin_interface(void) {
    delete[] world.mAudioBusTouched;
    delete[] world.mControlBusTouched;
    delete[] world.mSndBufs;
    delete[] world.mSndBufsNonRealTimeMirror;
    delete[] world.mSndBufUpdates;
    delete[] world.mRGen;
    delete reinterpret_cast<SC_Lock*>(world.mNRTLock);
}

namespace {

sample* allocate_buffer(size_t samples) {
    const size_t alloc_size = samples * sizeof(sample);
    sample* ret = (sample*)calloc_aligned(alloc_size);
    if (ret)
        mlock(ret, alloc_size);
    return ret;
}

inline int32 bufmask(int32 x) { return (1 << (31 - CLZ(x))) - 1; }

inline void sndbuf_init(SndBuf* buf) {
    buf->samplerate = 0;
    buf->sampledur = 0;
    buf->data = nullptr;
    buf->channels = 0;
    buf->samples = 0;
    buf->frames = 0;
    buf->mask = 0;
    buf->mask1 = 0;
    buf->coord = 0;
    buf->sndfile = nullptr;
    buf->isLocal = false;
}

inline void sndbuf_copy(SndBuf* dest, const SndBuf* src) {
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
    dest->isLocal = src->isLocal;
}

static inline size_t compute_remaining_samples(size_t frames_per_read, size_t already_read, size_t total_frames) {
    int remain = frames_per_read;
    if (already_read + frames_per_read > total_frames)
        remain = total_frames - already_read;
    return remain;
}

void read_channel(SndfileHandle& sf, uint32_t channel_count, const uint32_t* channel_data, uint32 total_frames,
                  sample* data) {
    const unsigned int frames_per_read = 1024;
    sized_array<sample> read_frame(sf.channels() * frames_per_read);

    if (channel_count == 1) {
        // fast-path for single-channel read
        for (size_t i = 0; i < total_frames; i += frames_per_read) {
            int remaining_samples = compute_remaining_samples(frames_per_read, i, total_frames);
            size_t read = sf.readf(read_frame.c_array(), remaining_samples);

            size_t channel_mapping = channel_data[0];
            for (size_t frame = 0; frame != read; ++frame) {
                data[0] = read_frame[frame * sf.channels() + channel_mapping];
                data += channel_count;
            }
        }
    } else {
        for (size_t i = 0; i < total_frames; i += frames_per_read) {
            int remaining_samples = compute_remaining_samples(frames_per_read, i, total_frames);
            size_t read = sf.readf(read_frame.c_array(), remaining_samples);

            for (size_t frame = 0; frame != read; ++frame) {
                for (size_t c = 0; c != channel_count; ++c) {
                    size_t channel_mapping = channel_data[c];
                    data[c] = read_frame[frame * sf.channels() + channel_mapping];
                }
                data += channel_count;
            }
        }
    }
}

} /* namespace */

void sc_plugin_interface::allocate_buffer(SndBuf* buf, uint32_t frames, uint32_t channels, double samplerate) {
    const uint32_t samples = frames * channels;
    if (samples == 0)
        throw std::runtime_error("invalid buffer size");

    sample* data = nova::allocate_buffer(samples);
    if (data == nullptr)
        throw std::runtime_error("could not allocate memory");

    buf->data = data;
    buf->channels = channels;
    buf->frames = frames;
    buf->samples = samples;
    buf->mask = bufmask(samples); /* for delay lines */
    buf->mask1 = buf->mask - 1; /* for oscillators */
    buf->samplerate = samplerate;
    buf->sampledur = 1.0 / samplerate;
    buf->isLocal = false;
}

SndBuf* sc_plugin_interface::allocate_buffer(uint32_t index, uint32_t frames, uint32_t channels) {
    SndBuf* buf = World_GetNRTBuf(&world, index);
    allocate_buffer(buf, frames, channels, world.mFullRate.mSampleRate);
    return buf;
}

void sc_plugin_interface::buffer_read_alloc(uint32_t index, const char* filename, uint32_t start, uint32_t frames) {
    auto f = makeSndfileHandle(filename);
    if (f.rawHandle() == nullptr)
        throw std::runtime_error(f.strError());

    const size_t sf_frames = f.frames();

    if (start > sf_frames)
        start = sf_frames;

    if (frames == 0 || frames > sf_frames - start)
        frames = sf_frames - start;

    SndBuf* buf = World_GetNRTBuf(&world, index);
    allocate_buffer(buf, frames, f.channels(), f.samplerate());

    f.seek(start, SEEK_SET);
    f.readf(buf->data, frames);
}


void sc_plugin_interface::buffer_alloc_read_channels(uint32_t index, const char* filename, uint32_t start,
                                                     uint32_t frames, uint32_t channel_count,
                                                     const uint32_t* channel_data) {
    // If no channel argument provided we read all channels.
    if (channel_count == 0) {
        buffer_read_alloc(index, filename, start, frames);
        return;
    }

    auto f = makeSndfileHandle(filename);
    if (f.rawHandle() == nullptr)
        throw std::runtime_error(f.strError());

    uint32_t sf_channels = uint32_t(f.channels());
    const uint32_t* max_chan = std::max_element(channel_data, channel_data + channel_count);
    if (*max_chan >= sf_channels)
        throw std::runtime_error("Channel out of range");

    const size_t sf_frames = f.frames();

    if (start > sf_frames)
        start = sf_frames;

    if (frames == 0 || frames > sf_frames - start)
        frames = sf_frames - start;

    SndBuf* buf = World_GetNRTBuf(&world, index);
    allocate_buffer(buf, frames, channel_count, f.samplerate());

    f.seek(start, SEEK_SET);
    read_channel(f, channel_count, channel_data, frames, buf->data);
}


int sc_plugin_interface::buffer_write(uint32_t index, const char* filename, const char* header_format,
                                      const char* sample_format, uint32_t start, uint32_t frames, bool leave_open) {
    SndBuf* buf = World_GetNRTBuf(&world, index);
    int format = headerFormatFromString(header_format) | sampleFormatFromString(sample_format);

    auto sf = makeSndfileHandle(filename, SFM_WRITE, format, buf->channels, buf->samplerate);

    if (!sf)
        return -1;

    if (frames == 0xffffffff)
        frames = buf->frames;

    const uint32_t remain = uint32_t(buf->frames) > start ? buf->frames - start : 0;
    const uint32_t frames_to_write = std::min(remain, frames);

    if (frames_to_write)
        sf.writef(buf->data + start * buf->channels, frames_to_write);

    if (leave_open && !buf->sndfile)
        buf->sndfile = sf.takeOwnership();

    return 0;
}

static void buffer_read_verify(SndfileHandle& sf, size_t min_length, size_t samplerate, bool check_samplerate) {
    if (sf.rawHandle() == nullptr)
        throw std::runtime_error(sf.strError());
    if (sf.frames() < min_length)
        throw std::runtime_error("no more frames to read");

    if (check_samplerate && (sf.samplerate() != samplerate))
        throw std::runtime_error("sample rate mismatch");
}

void sc_plugin_interface::buffer_read(uint32_t index, const char* filename, uint32_t start_file, uint32_t frames,
                                      uint32_t start_buffer, bool leave_open) {
    SndBuf* buf = World_GetNRTBuf(&world, index);

    if (uint32_t(buf->frames) < start_buffer)
        throw std::runtime_error("buffer already full");

    auto sf = makeSndfileHandle(filename, SFM_READ);
    buffer_read_verify(sf, start_file, buf->samplerate, !leave_open);

    if (sf.channels() != buf->channels)
        throw std::runtime_error("channel count mismatch");

    const uint32_t buffer_remain = buf->frames - start_buffer;
    const uint32_t file_remain = sf.frames() - start_file;
    const uint32_t frames_to_read = std::min(frames, std::min(buffer_remain, file_remain));

    sf.seek(start_file, SEEK_SET);
    sf.readf(buf->data + start_buffer * buf->channels, frames_to_read);

    if (leave_open)
        buf->sndfile = sf.takeOwnership();
}

void sc_plugin_interface::buffer_read_channel(uint32_t index, const char* filename, uint32_t start_file,
                                              uint32_t frames, uint32_t start_buffer, bool leave_open,
                                              uint32_t channel_count, const uint32_t* channel_data) {
    SndBuf* buf = World_GetNRTBuf(&world, index);

    if (channel_count != uint32_t(buf->channels))
        throw std::runtime_error("channel count mismatch");

    if (uint32_t(buf->frames) >= start_buffer)
        throw std::runtime_error("buffer already full");

    auto sf = makeSndfileHandle(filename, SFM_READ);
    buffer_read_verify(sf, start_file, buf->samplerate, !leave_open);

    uint32_t sf_channels = uint32_t(sf.channels());
    const uint32_t* max_chan = std::max_element(channel_data, channel_data + channel_count);
    if (*max_chan >= sf_channels)
        throw std::runtime_error("channel count mismatch");

    const uint32_t buffer_remain = buf->frames - start_buffer;
    const uint32_t file_remain = sf.frames() - start_file;

    const uint32_t frames_to_read = std::min(frames, std::min(buffer_remain, file_remain));

    sf.seek(start_file, SEEK_SET);
    read_channel(sf, channel_count, channel_data, frames_to_read, buf->data);

    if (leave_open)
        buf->sndfile = sf.takeOwnership();
}

void sc_plugin_interface::buffer_close(uint32_t index) {
    SndBuf* buf = World_GetNRTBuf(&world, index);

    if (buf->sndfile == nullptr)
        return;
    sf_close(buf->sndfile);
    buf->sndfile = nullptr;
}


void sc_plugin_interface::buffer_zero(uint32_t index) {
    SndBuf* buf = World_GetNRTBuf(&world, index);

    uint32_t length = buf->frames * buf->channels;

    uint32_t unrolled = length & ~63;
    uint32_t remain = length & 63;

    zerovec_simd(buf->data, unrolled);
    zerovec(buf->data + unrolled, remain);
}

sample* sc_plugin_interface::buffer_generate(uint32_t buffer_index, const char* cmd_name, struct sc_msg_iter& msg) {
    return sc_factory->run_bufgen(&world, cmd_name, buffer_index, &msg);
}

void sc_plugin_interface::buffer_sync(uint32_t index) noexcept {
    sndbuf_copy(world.mSndBufs + index, world.mSndBufsNonRealTimeMirror + index);
    increment_write_updates(index);
}

void sc_plugin_interface::free_buffer(uint32_t index) {
    SndBuf* buf = world.mSndBufsNonRealTimeMirror + index;
    if (buf->sndfile)
        sf_close(buf->sndfile);

    sndbuf_init(buf);
}

} /* namespace nova */
