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

#ifndef SERVER_NOVA_SERVER_HPP
#define SERVER_NOVA_SERVER_HPP

#include <boost/thread.hpp>

#include "buffer_manager.hpp"
#include "control_bus_manager.hpp"
#include "memory_pool.hpp"
#include "node_graph.hpp"
#include "sc_osc_handler.hpp"
#include "server_scheduler.hpp"
#include "synth_factory.hpp"
#include "audio_backend/audio_frontend.hpp"
#include "utilities/callback_interpreter.hpp"
#include "utilities/static_pooled_class.hpp"
#include "utilities/osc_server.hpp"


namespace nova
{

inline void run_scheduler_tick(void);

extern class nova_server * instance;

/** callback class for audio-thread to system-thread communcation
 *
 *  the system_callback uses an internal memory pool for real-time safe
 *  memory management. objects are only allowed to be allocated from
 *  the real-time thread.
 * */

class system_callback:
    public static_pooled_class<system_callback,
                               1<<20,    /* 1mb pool of realtime memory */
                               false, 5>
{
public:
    virtual ~system_callback(void)
    {}

    virtual void run(void) = 0;
};

class nova_server:
    public node_graph,
    public scheduler,
    public audio_frontend<&run_scheduler_tick>,
    public synth_factory,
    public buffer_manager,
    public control_bus_manager,
    public sc_osc_handler
{
public:
    /* main nova_server function */
    nova_server(unsigned int port = 59595, unsigned int threads = boost::thread::hardware_concurrency());

    ~nova_server(void);

    /* system interpreter */
    /* @{ */
    void add_system_callback(system_callback * cb)
    {
        system_interpreter.add_callback(cb);
    }

    void run(void)
    {
        system_interpreter.run();
    }

    void terminate(void)
    {
        system_interpreter.terminate();
    }
    /* @} */

    abstract_synth * add_synth(std::string const & name, int id, node_position_constraint const & constraints);
    group * add_group(int id, node_position_constraint const & constraints);
    parallel_group * add_parallel_group(int id, node_position_constraint const & constraints);
    void free_synth(abstract_synth *);

    void free_node(int id);

    void set_node_slot(int node_id, slot_index_t slot, float value);
    void set_node_slot(int node_id, slot_identifier_type const & slot, float value);

#if 0
    /* node control */
    void free_node(node_id);

    /* synth control */
    node_id add_synth(synthdef def, list<pair<slot_id, float> > args, node_position_constraint const &);
    void set_synth_slot(node_id, slot_id, float);

    /* group control */
    node_id add_group(node_position_constraint const &);
    void set_group_slot(node_id, slot_id, float);
#endif

private:
    void update_dsp_queue(void);

    callback_interpreter<system_callback> system_interpreter;
};

inline void run_scheduler_tick(void)
{
    (*instance)();
}

} /* namespace nova */

#endif /* SERVER_NOVA_SERVER_HPP */
