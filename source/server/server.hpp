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

#include "memory_pool.hpp"
#include "node_graph.hpp"
#include "server_scheduler.hpp"
#include "synth_factory.hpp"
#include "utilities/callback_interpreter.hpp"
#include "utilities/static_pooled_class.hpp"
#include "utilities/osc_server.hpp"

#ifdef PORTAUDIO_BACKEND
#include "audio_backend/audio_frontend.hpp"
#elif defined(JACK_BACKEND)
#include "audio_backend/jack_backend.hpp"
#endif


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

/** system_callback to delete object in the system thread. useful to avoid hitting the memory allocator
 *  from within the real-time threads
 */
template <typename T>
class delete_callback:
    public system_callback
{
public:
    delete_callback (T * ptr):
        ptr_(ptr)
    {}

private:
    virtual void run(void)
    {
        delete ptr_;
    }

    const T * const ptr_;
};

class nova_server:
    public osc_server,
    public node_graph,
    public scheduler,
#ifdef PORTAUDIO_BACKEND
    public audio_frontend<&run_scheduler_tick>,
#elif defined(JACK_BACKEND)
    public jack_backend<&run_scheduler_tick>,
#endif
    public synth_factory
{
public:
    /* main nova_server function */
    nova_server(unsigned int port = 59595, unsigned int threads = boost::thread::hardware_concurrency());

    ~nova_server(void);

    /* @{ */
    /** system interpreter */
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


private:
    template <bool (node_graph::*fn)(abstract_group*)>
    bool group_free_implementation(abstract_group * group)
    {
        bool success = (this->*fn)(group);
        if (success)
            update_dsp_queue();
        return success;
    }

public:
    /* @{ */
    /** graph handling */
    abstract_synth * add_synth(const char * name, int id, node_position_constraint const & constraints);

    group * add_group(int id, node_position_constraint const & constraints);
    parallel_group * add_parallel_group(int id, node_position_constraint const & constraints);

    void free_node(server_node * node);

    bool group_free_all(abstract_group * node_id)
    {
        return group_free_implementation<&node_graph::group_free_all>(node_id);
    }
    bool group_free_deep(abstract_group * node_id)
    {
        return group_free_implementation<&node_graph::group_free_deep>(node_id);
    }
    /* @} */

    void set_node_slot(int node_id, slot_index_t slot, float value);
    void set_node_slot(int node_id, const char * slot, float value);

    void register_prototype(synth_prototype_ptr const & prototype);

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
    /* initialize osc handles */
    void init_osc_handles(void);

    void update_dsp_queue(void);

    callback_interpreter<system_callback> system_interpreter;
};

inline void run_scheduler_tick(void)
{
    (*instance)();
}

} /* namespace nova */

#endif /* SERVER_NOVA_SERVER_HPP */
