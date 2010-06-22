//  nova server
//  Copyright (C) 2008, 2009, 2010 Tim Blechmann
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

#include "server.hpp"
#include "sync_commands.hpp"

#include "nrt_synthesis.hpp"

#include "sc/sc_synth_prototype.hpp"
#include "sc/sc_ugen_factory.hpp"

namespace nova
{

class nova_server * instance = 0;

nova_server::nova_server(server_arguments const & args):
    scheduler(args.threads), buffer_manager(1024), sc_osc_handler(args),
    io_interpreter(1, true, thread_priority_interval_rt().first)
{
    assert(instance == 0);
    sc_factory = new sc_ugen_factory;
    instance = this;

    /** todo: backend may force sample rate */
    time_per_tick = time_tag::from_samples(args.blocksize, args.samplerate);
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
}

nova_server::~nova_server(void)
{
#if defined (JACK_BACKEND)
    if (audio_is_active())
        deactivate_audio();
#endif
    instance = 0;
}

abstract_synth * nova_server::add_synth(const char * name, int id, node_position_constraint const & constraints)
{
    abstract_synth * ret = synth_factory::create_instance(name, id);

    if (ret == 0)
        return 0;

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
    notification_node_ended(node);
    node_graph::remove_node(node);
    update_dsp_queue();
}

void nova_server::run_nonrt_synthesis(server_arguments const & args)
{
    non_realtime_synthesis_engine engine(args);
    engine.run();
}

namespace
{

struct register_prototype_cb:
    public audio_sync_callback
{
    register_prototype_cb (synth_prototype_ptr const & prototype):
        prototype(prototype)
    {}

    void run(void)
    {
        instance->synth_factory::register_prototype(prototype);
    }

    synth_prototype_ptr prototype;
};

} /* namespace */

void nova_server::register_prototype(synth_prototype_ptr const & prototype)
{
    scheduler::add_sync_callback(new register_prototype_cb(prototype));
}


void nova_server::rebuild_dsp_queue(void)
{
    assert(dsp_queue_dirty);
    std::auto_ptr<dsp_thread_queue> new_queue = node_graph::generate_dsp_queue();
    scheduler::reset_queue_sync(new_queue);
    dsp_queue_dirty = false;
}

void scheduler::operator()(void)
{
    cbs.run_callbacks();
    instance->execute_scheduled_bundles();
    threads.run();
}


void synth_prototype_deleter::dispose(synth_prototype * ptr)
{
    if (instance) /// todo: hack to fix test suite
        instance->add_system_callback(new delete_callback<synth_prototype>(ptr));
    else
        delete ptr;
}


} /* namespace nova */
