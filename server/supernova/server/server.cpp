//  nova server
//  Copyright (C) 2008 - 2012 Tim Blechmann
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

#include <stdexcept>

#include "SC_Win32Utils.h"

#include "nova-tt/thread_affinity.hpp"
#include "nova-tt/thread_priority.hpp"
#include "nova-tt/name_thread.hpp"

#include "server.hpp"
#include "sync_commands.hpp"

#ifndef NO_LIBSNDFILE
#    include "nrt_synthesis.hpp"
#endif

#include "sc/sc_synth_definition.hpp"
#include "sc/sc_ugen_factory.hpp"

#ifdef __APPLE__
#    include <AvailabilityMacros.h>
#    include <CoreAudio/HostTime.h>
#    include <CoreAudio/CoreAudioTypes.h>
#endif

#define DEBUG_THREAD_PINNING 0

namespace nova {

class nova_server* instance = nullptr;

nova_server::nova_server(server_arguments const& args):
    // FIXME: In case of multiple supernova instances on the same port (e.g. when running on
    // different interfaces), they can end up using the same shmem location.
    server_shared_memory_creator(args.port(), args.control_busses),

    scheduler<thread_init_functor>(args.threads, !args.non_rt, args.thread_pinning),
    buffer_manager(args.buffers),
    sc_osc_handler(args) {
    assert(instance == 0);
    instance = this;

    use_system_clock = (args.use_system_clock == 1);
    non_rt = args.non_rt;
    pin_threads = args.thread_pinning;
    smooth_samplerate = args.samplerate;

    if (!args.non_rt) {
        audio_backend::initialize();
        io_interpreter.start_thread();
    }

    sc_factory.reset(new sc_ugen_factory);
    sc_factory->initialize(args, server_shared_memory_creator::shm->get_control_busses());

    /** first guess: needs to be updated, once the backend is started */
    time_per_tick = time_tag::from_samples(args.blocksize, args.samplerate ? args.samplerate : 44100);

    if (!args.non_rt)
        start_receive_thread();
}

void nova_server::prepare_backend(void) {
    /* register audio backend ports */
    const int blocksize = get_audio_blocksize();
    const int input_channels = get_input_count();
    const int output_channels = get_output_count();

    std::vector<sample*> inputs, outputs;
    for (int channel = 0; channel != input_channels; ++channel)
        inputs.push_back(sc_factory->world.mAudioBus + (blocksize * (output_channels + channel)));

    audio_backend::input_mapping(inputs.begin(), inputs.end());

    for (int channel = 0; channel != output_channels; ++channel)
        outputs.push_back(sc_factory->world.mAudioBus + blocksize * channel);

    audio_backend::output_mapping(outputs.begin(), outputs.end());

    time_per_tick = time_tag::from_samples(blocksize, get_samplerate());
}

nova_server::~nova_server(void) {
    // we should delete but get chrashes at the moment on linux and macosx
    // delete sc_factory;
#if defined(JACK_BACKEND) || defined(PORTAUDIO_BACKEND)
    deactivate_audio();
#endif

    group_free_all(static_cast<abstract_group*>(find_node(0))); // free the root group

    scheduler<thread_init_functor>::terminate();
    io_interpreter.join_thread();

    sc_factory.reset();
    instance = nullptr;
}

void nova_server::perform_node_add(server_node* node, node_position_constraint const& constraints,
                                   bool update_dsp_queue) {
    node_graph::add_node(node, constraints, [&](server_node& node) { finalize_node(node); });

    if (constraints.second == replace || update_dsp_queue)
        request_dsp_queue_update();

    notification_node_started(node);
}

abstract_synth* nova_server::add_synth(const char* name, int id, node_position_constraint const& constraints) {
    abstract_synth* ret = synth_factory::create_instance(name, id);
    if (ret == nullptr)
        return nullptr;

    perform_node_add(ret, constraints, true);
    return ret;
}

group* nova_server::add_group(int id, node_position_constraint const& constraints) {
    group* g = new group(id);
    if (g == nullptr)
        return nullptr;

    perform_node_add(g, constraints, false);
    return g;
}

parallel_group* nova_server::add_parallel_group(int id, node_position_constraint const& constraints) {
    parallel_group* g = new parallel_group(id);
    if (g == nullptr)
        return nullptr;

    perform_node_add(g, constraints, false);
    return g;
}


void nova_server::set_node_slot(int node_id, slot_index_t slot, float value) {
    server_node* node = find_node(node_id);
    if (node)
        node->set(slot, value);
}

void nova_server::set_node_slot(int node_id, const char* slot, float value) {
    server_node* node = find_node(node_id);
    if (node)
        node->set(slot, value);
}

void nova_server::finalize_node(server_node& node) {
    if (node.is_synth()) {
        sc_synth& synth = static_cast<sc_synth&>(node);
        synth.finalize();
    }
    notification_node_ended(&node);
}


void nova_server::free_node(server_node* node) {
    if (node->get_parent() == nullptr)
        return; // has already been freed by a different event

    node_graph::remove_node(node, [&](server_node& node) { finalize_node(node); });
    request_dsp_queue_update();
}

void nova_server::group_free_all(abstract_group* group) {
    std::vector<server_node*, rt_pool_allocator<server_node*>> nodes_to_free;

    group->apply_on_children([&](server_node& node) { nodes_to_free.push_back(&node); });

    for (server_node* node : nodes_to_free)
        free_node(node);
}

void nova_server::group_free_deep(abstract_group* group) {
    std::vector<server_node*, rt_pool_allocator<server_node*>> nodes_to_free;
    group->apply_deep_on_children([&](server_node& node) {
        if (node.is_synth())
            nodes_to_free.push_back(&node);
    });
    for (server_node* node : nodes_to_free)
        free_node(node);
}


void nova_server::run_nonrt_synthesis(server_arguments const& args) {
#ifndef NO_LIBSNDFILE
    start_dsp_threads();
    non_realtime_synthesis_engine engine(args);
    engine.run();
#else
    std::cout << "Warning: Non-RT synthesis not supported as supernova was compiled without libsndfile" << std::endl;
#endif
}

void nova_server::rebuild_dsp_queue(void) {
    assert(dsp_queue_dirty);
    node_graph::dsp_thread_queue_ptr new_queue = node_graph::generate_dsp_queue();
    scheduler<thread_init_functor>::reset_queue_sync(std::move(new_queue));
    dsp_queue_dirty = false;
}

static void name_current_thread(int thread_index) {
    char buf[1024];
    sprintf(buf, "DSP Thread %d", thread_index);
#if defined(__linux__)
    name_thread(buf);
#elif defined(__APPLE__)
    // TODO
#elif defined(_WIN32)
    win32_name_thread(buf);
#endif
}

static void set_daz_ftz(void) {
#if BOOST_HW_SIMD_X86 >= BOOST_HW_SIMD_X86_SSE_VERSION
    /* denormal handling */
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _mm_setcsr(_mm_getcsr() | 0x40);
#endif
}

static bool set_realtime_priority(int thread_index) {
    bool success = false;

#ifdef NOVA_TT_PRIORITY_PERIOD_COMPUTATION_CONSTRAINT
    double blocksize = server_arguments::instance().blocksize;
    double samplerate = server_arguments::instance().samplerate ? server_arguments::instance().samplerate : 44100;

    double ns_per_block = 1e9 / samplerate * blocksize;

#    ifdef __APPLE__

    success = thread_set_priority_rt(0, AudioConvertNanosToHostTime(ns_per_block - 5000),
                                     AudioConvertNanosToHostTime(ns_per_block), true);

#    else

    success = thread_set_priority_rt(ns_per_block, ns_per_block - 2, ns_per_block - 1, false);

#    endif

    if (!success)
        std::cout << "Warning: initialize deadline scheduling" << std::endl;

#endif

    if (!success) {
#ifdef NOVA_TT_PRIORITY_RT

#    if defined(JACK_BACKEND)
        int priority = instance->realtime_priority();
        /* This line has effectively been bypassed for years because
         * of a logic error further below. With the logic error fixed,
         * it would cause lower realtime priorities for DSP helper threads.
         * Let's keep the old behavior until we figure out what this code
         * is supposed to do in the first place... */
#        if 0
        if (priority >= 0)
            success = true;
#        endif

#    elif defined(_WIN32)
        int priority = thread_priority_interval_rt().second;
#    else
        int min, max;
        boost::tie(min, max) = thread_priority_interval_rt();
        int priority = max - 3;
        priority = std::max(min, priority);
#    endif

        if (!success)
            success = thread_set_priority_rt(priority);
#endif
    }

    if (!success)
        std::cout << "Warning: cannot raise thread priority" << std::endl;

    return success;
}

/* utilities/hardware_topology.cpp */
int get_cpu_for_thread_index(int thread_index);

void thread_init_functor::operator()(int thread_index) {
    set_daz_ftz();
    name_current_thread(thread_index);

    if (rt)
        set_realtime_priority(thread_index);

    if (pin) {
        auto cpu = get_cpu_for_thread_index(thread_index);
#if DEBUG_THREAD_PINNING
        std::cout << "pin thread " << thread_index << " to CPU " << cpu << std::endl;
#endif
#ifdef _WIN32
        // nova::thread_set_affinity() is not implemented for Windows
        bool result = win32_thread_set_affinity(cpu);
#else
        bool result = thread_set_affinity(cpu);
#endif
        if (!result)
            std::cout << "Warning: cannot set thread affinity of audio helper thread" << std::endl;
    }
}

void io_thread_init_functor::operator()() const {
    /* NB: on macOS we just keep the default thread priority */
#ifdef NOVA_TT_PRIORITY_RT
    int priority = thread_priority_interval_rt().first;
    thread_set_priority_rt(priority);
#endif

    name_thread("Network Send");
}

void synth_definition_deleter::dispose(synth_definition* ptr) {
    if (instance) /// todo: hack to fix test suite
        instance->add_system_callback(new delete_callback<synth_definition>(ptr));
    else
        delete ptr;
}

void realtime_engine_functor::init_thread(void) {
    set_daz_ftz();

    if (instance->pin_threads) {
        auto cpu = get_cpu_for_thread_index(0);
#if DEBUG_THREAD_PINNING
        std::cout << "pin thread 0 to CPU " << cpu << std::endl;
#endif
#ifdef _WIN32
        // nova::thread_set_affinity() is not implemented for Windows
        bool result = win32_thread_set_affinity(cpu);
#else
        bool result = thread_set_affinity(cpu);
#endif
        if (!result)
            std::cout << "Warning: cannot set thread affinity of main audio thread" << std::endl;
    }
#ifdef JACK_BACKEND
    set_realtime_priority(0);
#endif
    if (instance->use_system_clock) {
        double nows = (uint64)(OSCTime(std::chrono::system_clock::now())) * kOSCtoSecs;
        instance->mDLL.Reset(sc_factory->world.mSampleRate, sc_factory->world.mBufLength, SC_TIME_DLL_BW, nows);
    }

    name_current_thread(0);
}

void realtime_engine_functor::log_(const char* str) {
    // Silently drop messages; if instance is null it means something failed while constructing nova_server
    if (instance)
        instance->log_printf(str);
}

void realtime_engine_functor::log_printf_(const char* fmt, ...) {
    // Silently drop messages; if instance is null it means something failed while constructing nova_server
    if (instance == nullptr)
        return;

    va_list vargs;
    va_start(vargs, fmt);
    instance->log_printf(fmt, vargs);
    va_end(vargs);
}


} /* namespace nova */
