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

namespace nova
{

class nova_server * instance = 0;

nova_server::nova_server(unsigned int port, unsigned int threads):
    scheduler(threads), buffer_manager(1024), sc_osc_handler(port)
{
    assert(instance == 0);
    instance = this;
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

group * nova_server::add_group(int id, node_position_constraint const & constraints)
{
    group * g = group::allocate_group(id);
    instance->add_node(g, constraints);
    /* no need to update the dsp queue */
    return g;
}

parallel_group * nova_server::add_parallel_group(int id, node_position_constraint const & constraints)
{
    parallel_group * g = parallel_group::allocate_parallel_group(id);
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

void nova_server::set_node_slot(int node_id, slot_identifier_type const & slot, float value)
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


} /* namespace nova */
