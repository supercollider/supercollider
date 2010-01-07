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
#include "memory_pool.hpp"
#include "node_graph.hpp"
#include "sc_osc_handler.hpp"
#include "server_scheduler.hpp"
#include "synth_factory.hpp"
#include "../sc/sc_ugen_factory.hpp"
#include "../utilities/callback_interpreter.hpp"
#include "../utilities/static_pooled_class.hpp"
#include "../utilities/osc_server.hpp"

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
    public node_graph,
    public scheduler,
#ifdef PORTAUDIO_BACKEND
    public audio_frontend<&run_scheduler_tick>,
#elif defined(JACK_BACKEND)
    public jack_backend<&run_scheduler_tick, float, false>,
#endif
    public synth_factory,
    public buffer_manager,
    public sc_osc_handler
{
public:
    /* main nova_server function */
    nova_server(unsigned int port = 59595, unsigned int threads = boost::thread::hardware_concurrency());

    ~nova_server(void);

    void add_io_callback(system_callback * cb)
    {
        system_interpreter.add_callback(cb);
    }

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
    abstract_synth * add_synth(const char * name, int id, node_position_constraint const & constraints);

    group * add_group(int id, node_position_constraint const & constraints);
    parallel_group * add_parallel_group(int id, node_position_constraint const & constraints);

    void free_node(int id);
    void free_node(server_node * node);

    void set_node_slot(int node_id, slot_index_t slot, float value);
    void set_node_slot(int node_id, const char * slot, float value);

    void register_prototype(synth_prototype_ptr const & prototype);

    uint32_t synth_count(void) const
    {
        return synth_count_;
    }

    uint32_t group_count(void) const
    {
        return group_count_;
    }

    float cpu_load(void) const
    {
#ifdef JACK_BACKEND
        return get_cpuload();
#else
        return 0.f
#endif
    }

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
    callback_interpreter_threadpool<system_callback> io_interpreter;
    uint32_t synth_count_, group_count_;
};

inline void run_scheduler_tick(void)
{
    const int blocksize = instance->get_audio_blocksize();
    const int input_channels = instance->get_input_count();
    const int output_channels = instance->get_output_count();
    const int buf_counter = ++sc_factory.world.mBufCounter;
    for (int channel = 0; channel != input_channels; ++channel) {
        instance->fetch_adc_input(sc_factory.world.mAudioBus + (blocksize * (output_channels + channel)),
                                  channel, blocksize);
        sc_factory.world.mAudioBusTouched[output_channels + channel] = buf_counter;
    }

    (*instance)();

    sc_factory.update_nodegraph();

    for (int channel = 0; channel != output_channels; ++channel) {
        if (sc_factory.world.mAudioBusTouched[channel] == buf_counter)
            instance->copy_dac_output(sc_factory.world.mAudioBus + blocksize * channel, channel, blocksize);
    }
}

} /* namespace nova */

#endif /* SERVER_NOVA_SERVER_HPP */
