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

#include "nova-tt/thread_affinity.hpp"
#include "nova-tt/thread_priority.hpp"
#include "nova-tt/name_thread.hpp"

#include "server.hpp"
#include "sync_commands.hpp"

#include "nrt_synthesis.hpp"

#include "sc/sc_synth_definition.hpp"
#include "sc/sc_ugen_factory.hpp"

#ifdef __APPLE__
#include <AvailabilityMacros.h>
#include <CoreAudio/HostTime.h>
#include <CoreAudio/CoreAudioTypes.h>
#endif

namespace nova
{

class nova_server * instance = 0;

nova_server::nova_server(server_arguments const & args):
    server_shared_memory_creator(args.port(), args.control_busses),
    scheduler<nova::scheduler_hook, thread_init_functor>(args.threads, !args.non_rt),
    buffer_manager(args.buffers), sc_osc_handler(args), dsp_queue_dirty(false)
{
    assert(instance == 0);
    instance = this;

    if (!args.non_rt)
        io_interpreter.start_thread();

    sc_factory = new sc_ugen_factory;
    sc_factory->initialize(args, server_shared_memory_creator::shm->get_control_busses());


    /** first guess: needs to be updated, once the backend is started */
    time_per_tick = time_tag::from_samples(args.blocksize, args.samplerate);

    if (!args.non_rt)
        start_receive_thread();
}

void nova_server::prepare_backend(void)
{
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

#ifdef __SSE__
    /* denormal handling */
    _MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
    _mm_setcsr(_mm_getcsr() | 0x40);
#endif

    time_per_tick = time_tag::from_samples(blocksize, get_samplerate());
}

nova_server::~nova_server(void)
{
#if defined(JACK_BACKEND) || defined(PORTAUDIO_BACKEND)
    deactivate_audio();
#endif

    scheduler<scheduler_hook, thread_init_functor>::terminate();
    io_interpreter.join_thread();
    instance = 0;
}

abstract_synth * nova_server::add_synth(const char * name, int id, node_position_constraint const & constraints)
{
    abstract_synth * ret = synth_factory::create_instance(name, id);

    if (ret == 0)
        return 0;

    if (constraints.second == replace)
        notification_node_ended(constraints.first);

    node_graph::add_node(ret, constraints);
    update_dsp_queue();
    notification_node_started(ret);
    return ret;
}

group * nova_server::add_group(int id, node_position_constraint const & constraints)
{
    group * g = new group(id);
    if (g == 0)
        return 0;

    instance->add_node(g, constraints);
    /* no need to update the dsp queue */
    notification_node_started(g);
    return g;
}

parallel_group * nova_server::add_parallel_group(int id, node_position_constraint const & constraints)
{
    parallel_group * g = new parallel_group(id);
    if (g == 0)
        return 0;
    instance->add_node(g, constraints);
    /* no need to update the dsp queue */
    notification_node_started(g);
    return g;
}


void nova_server::set_node_slot(int node_id, slot_index_t slot, float value)
{
    server_node * node = find_node(node_id);
    if (node)
        node->set(slot, value);
}

void nova_server::set_node_slot(int node_id, const char * slot, float value)
{
    server_node * node = find_node(node_id);
    if (node)
        node->set(slot, value);
}

void nova_server::free_node(server_node * node)
{
    if (node->get_parent() == NULL)
        return; // has already been freed by a different event
    notification_node_ended(node);
    node_graph::remove_node(node);
    update_dsp_queue();
}

void nova_server::run_nonrt_synthesis(server_arguments const & args)
{
    start_dsp_threads();
    non_realtime_synthesis_engine engine(args);
    engine.run();
}

void nova_server::rebuild_dsp_queue(void)
{
    assert(dsp_queue_dirty);
    node_graph::dsp_thread_queue_ptr new_queue = node_graph::generate_dsp_queue();
    scheduler<scheduler_hook, thread_init_functor>::reset_queue_sync(std::move(new_queue));
    dsp_queue_dirty = false;
}

static void name_current_thread(int thread_index)
{
    char buf[1024];
    sprintf(buf, "DSP Thread %d", thread_index);
    name_thread(buf);
}

void thread_init_functor::operator()(int thread_index)
{
    name_current_thread(thread_index);

    if (rt) {
        bool success = true;
#ifdef NOVA_TT_PRIORITY_RT

#ifdef JACK_BACKEND
        int priority = instance->realtime_priority();
        if (priority < 0)
            success = false;
#else
        int min, max;
        boost::tie(min, max) = thread_priority_interval_rt();
        int priority = max - 3;
        priority = std::max(min, priority);
#endif

        if (success)
            success = thread_set_priority_rt(priority);
#endif

#if defined(NOVA_TT_PRIORITY_PERIOD_COMPUTATION_CONSTRAINT) && defined (__APPLE__)
        double blocksize = server_arguments::instance().blocksize;
        double samplerate = server_arguments::instance().samplerate;

        double ns_per_block = 1e9 / samplerate * blocksize;

        success = thread_set_priority_rt(AudioConvertNanosToHostTime(ns_per_block),
                                         AudioConvertNanosToHostTime(ns_per_block - 2),
                                         AudioConvertNanosToHostTime(ns_per_block - 1),
                                         false);
#endif

        if (!success)
            std::cerr << "Warning: cannot raise thread priority" << std::endl;
    }

    if (!thread_set_affinity(thread_index))
        std::cerr << "Warning: cannot set thread affinity of audio helper thread" << std::endl;
}

void io_thread_init_functor::operator()() const
{
#ifdef NOVA_TT_PRIORITY_RT
    int priority = thread_priority_interval_rt().first;
    thread_set_priority_rt(priority);
#else
    int priority = thread_priority_interval().second;
    thread_set_priority(priority);
#endif

    name_thread("Network Send");
}

void synth_definition_deleter::dispose(synth_definition * ptr)
{
    if (instance) /// todo: hack to fix test suite
        instance->add_system_callback(new delete_callback<synth_definition>(ptr));
    else
        delete ptr;
}

void realtime_engine_functor::init_thread(void)
{
    if (!thread_set_affinity(0))
        std::cerr << "Warning: cannot set thread affinity of main audio thread" << std::endl;

    name_current_thread(0);
}

void realtime_engine_functor::log_(const char * str)
{
    instance->log_printf(str);
}

void realtime_engine_functor::log_printf_(const char * fmt, ...)
{
    va_list vargs;
    va_start(vargs, fmt);
    instance->log_printf(fmt, vargs);
}


} /* namespace nova */
