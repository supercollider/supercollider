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

#include "sc_synth_prototype.hpp"

namespace nova
{

class nova_server * instance = 0;

nova_server::nova_server(unsigned int port, unsigned int threads):
    osc_server(port), scheduler(threads)
{
    assert(instance == 0);
    instance = this;

    init_osc_handles();
}

nova_server::~nova_server(void)
{
    instance = 0;
}


namespace
{

struct handle_quit:
    public osc_responder
{
    virtual void run(osc::ReceivedMessageArgumentIterator begin,
                     osc::ReceivedMessageArgumentIterator const & end)
    {
        instance->terminate();
    }
};

handle_quit quit_handler;


class handle_set:
    public osc_responder
{
public:
    virtual void run(osc::ReceivedMessageArgumentIterator it,
                     osc::ReceivedMessageArgumentIterator const & end)
    {
        try
        {
            using namespace osc;

            int nodeid = it->AsInt32();
            ++it;

            if (it == end)
                throw std::exception();

            osc::ReceivedMessageArgumentIterator slot_it = it;
            ++it;
            osc::ReceivedMessageArgumentIterator value_it = it;
            if (it == end)
                throw std::exception();

            if (it->IsInt32())
                instance->add_sync_callback(
                    new set_cmd_index<slot_index_t>(nodeid,
                                                    slot_it->AsInt32Unchecked(),
                                                    value_it->AsFloat())
                    );
            else if (it->IsString())
                instance->add_sync_callback(
                    new set_cmd_index<slot_identifier_type>(nodeid,
                                                            std::string(slot_it->AsString()),
                                                            value_it->AsFloat())
                    );
            else
                throw std::exception();
        }
        catch (std::exception const & e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
};

handle_set set_handler;


/* load synthdef */
class handle_d_load:
    public osc_responder
{
public:
    virtual void run(osc::ReceivedMessageArgumentIterator it,
                     osc::ReceivedMessageArgumentIterator const & end)
    {
        try
        {
            sc_read_synthdef(*instance, it->AsString());

            /* todo: send \done reply */
            /* todo: handle completion message */
        }
        catch (std::exception const & e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
};

handle_d_load d_load_handler;

class handle_d_loadDir:
    public osc_responder
{
public:
    virtual void run(osc::ReceivedMessageArgumentIterator it,
                     osc::ReceivedMessageArgumentIterator const & end)
    {
        try
        {
            sc_read_synthdefs_dir(*instance, it->AsString());

            /* todo: send \done reply */
            /* todo: handle completion message */
        }
        catch (std::exception const & e)
        {
            std::cerr << e.what() << std::endl;
        }
    }
};

handle_d_load d_loadDir_handler;

}

void nova_server::init_osc_handles(void)
{
    add_responder("/quit", &quit_handler);
    add_responder("/set", &set_handler);
    add_responder("/d_load", &d_load_handler);
    add_responder("/d_loadDir", &d_loadDir_handler);
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


} /* namespace nova */
