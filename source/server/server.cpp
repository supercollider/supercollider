//  nova server
//  Copyright (C) 2008, 2009 Tim Blechmann
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

#include "sc/sc_synth_prototype.hpp"
#include "sc/sc_ugen_factory.hpp"

namespace nova
{

class nova_server * instance = 0;

nova_server::nova_server(unsigned int port, unsigned int threads):
    scheduler(threads), buffer_manager(1024), sc_osc_handler(port)
{
    assert(instance == 0);
    instance = this;

    ugen_factory.load_plugin_folder("plugins");
}

nova_server::~nova_server(void)
{
    instance = 0;
}

abstract_synth * nova_server::add_synth(std::string const & name, int id, node_position_constraint const & constraints)
{
    abstract_synth * ret = synth_factory::create_instance(name, id);

    if (ret == 0)
        return 0;

    node_graph::add_node(ret, constraints);
    update_dsp_queue();
    return ret;
}

abstract_synth * nova_server::add_synth(const char * name, int id, node_position_constraint const & constraints)
{
    abstract_synth * ret = synth_factory::create_instance(name, id);

    if (ret == 0)
        return 0;

    node_graph::add_node(ret, constraints);
    update_dsp_queue();
    return ret;
}

group * nova_server::add_group(int id, node_position_constraint const & constraints)
{
    group * g = new group(id);
    if (g)
        instance->add_node(g, constraints);
    /* no need to update the dsp queue */
    return g;
}

parallel_group * nova_server::add_parallel_group(int id, node_position_constraint const & constraints)
{
    parallel_group * g = new parallel_group(id);
    if (g)
        instance->add_node(g, constraints);
    /* no need to update the dsp queue */
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

void nova_server::free_node(int node_id)
{
    server_node * node = find_node(node_id);
    if (node == NULL)
        throw std::runtime_error("cannot find server node");

    remove_node(node);
    update_dsp_queue();
}

void nova_server::free_synth(abstract_synth * s)
{
    node_graph::remove_node(s);
    update_dsp_queue();
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


void nova_server::update_dsp_queue(void)
{
    std::auto_ptr<dsp_thread_queue> new_queue = node_graph::generate_dsp_queue();
    scheduler::reset_queue(new_queue);
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
