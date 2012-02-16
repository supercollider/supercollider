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

#ifndef SERVER_NOVA_SERVER_HPP
#define SERVER_NOVA_SERVER_HPP

#include <boost/thread.hpp>

#include "buffer_manager.hpp"
#include "memory_pool.hpp"
#include "node_graph.hpp"
#include "../sc/sc_osc_handler.hpp"
#include "server_args.hpp"
#include "server_scheduler.hpp"
#include "synth_factory.hpp"
#include "../sc/sc_ugen_factory.hpp"
#include "../utilities/callback_interpreter.hpp"
#include "../utilities/static_pooled_class.hpp"
#include "../utilities/asynchronous_log.hpp"
#include "../../common/server_shm.hpp"

#ifdef PORTAUDIO_BACKEND
#include "audio_backend/portaudio_backend.hpp"
#elif defined(JACK_BACKEND)
#include "audio_backend/jack_backend.hpp"
#endif

namespace nova {

struct realtime_engine_functor
{
    static inline void sync_clock(void);
    static void init_thread(void);
    static inline void run_tick(void);
    static void log_(const char *);
    static void log_printf_(const char *, ...);
};

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


/** dsp thread init functor
 *
 *  for the real-time use, it should acquire real-time scheduling and pin the thread to a certain CPU
 *
 * */
struct thread_init_functor
{
    thread_init_functor(bool real_time):
        rt(real_time)
    {}

    void operator()(int thread_index);

private:
    bool rt;
};

struct io_thread_init_functor
{
    void operator()() const;
};

/** scheduler hook
 *
 *  evaluate scheduled bundles
 *
 * */
struct scheduler_hook
{
    inline void operator()(void);
};

namespace detail {
#if defined(PORTAUDIO_BACKEND)
typedef portaudio_backend<realtime_engine_functor, float, false> audio_backend;
#elif defined(JACK_BACKEND)
typedef jack_backend<realtime_engine_functor, float, false> audio_backend;
#else
#error "no audio backend selected"
#endif

} // detail

class nova_server:
    public asynchronous_log_thread,
    public node_graph,
    public server_shared_memory_creator,
    public scheduler<scheduler_hook, thread_init_functor>,
    public detail::audio_backend,
    public synth_factory,
    public buffer_manager,
    public sc_osc_handler
{
public:
    typedef detail::audio_backend audio_backend;

    /* main nova_server function */
    nova_server(server_arguments const & args);

    ~nova_server(void);

    void prepare_backend(void);

    /* @{ */
    /** io interpreter */
    void add_io_callback(system_callback * cb)
    {
        io_interpreter.add_callback(cb);
    }
    /* @} */

    /* @{ */
    /** system interpreter
      * \note: should only be called from the main audio thread
      */
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

    /** non-rt synthesis */
    void run_nonrt_synthesis(server_arguments const & arguments);

private:
    template <bool (node_graph::*fn)(abstract_group*)>
    bool group_free_implementation(abstract_group * group)
    {
        bool success = (this->*fn)(group);
        if (success)
            update_dsp_queue();
        return success;
    }

    static void free_all_notify(nova_server * server, server_node const & node)
    {
        server->notification_node_ended(&node);
    }

    static void free_deep_notify(nova_server * server, server_node & node)
    {
        if (node.is_synth())
            server->notification_node_ended(&node);
        else {
            abstract_group * group = static_cast<abstract_group*>(&node);
            group->apply_on_children(boost::bind(nova_server::free_deep_notify, server, _1));
        }
    }

public:
    /* @{ */
    /** node control */
    abstract_synth * add_synth(const char * name, int id, node_position_constraint const & constraints);

    group * add_group(int id, node_position_constraint const & constraints);
    parallel_group * add_parallel_group(int id, node_position_constraint const & constraints);

    void free_node(server_node * node);

    bool group_free_all(abstract_group * group)
    {
        /// todo: later we want to traverse the node graph only once
        group->apply_on_children(boost::bind(free_all_notify, this, _1));
        return group_free_implementation<&node_graph::group_free_all>(group);
    }

    bool group_free_deep(abstract_group * group)
    {
        /// todo: later we want to traverse the node graph only once
        group->apply_on_children(boost::bind(nova_server::free_deep_notify, this, _1));
        return group_free_implementation<&node_graph::group_free_deep>(group);
    }

    void node_pause(server_node * node)
    {
        node->pause();
        notification_node_turned_off(node);
    }

    void node_resume(server_node * node)
    {
        node->resume();
        notification_node_turned_on(node);
    }

    void set_node_slot(int node_id, slot_index_t slot, float value);
    void set_node_slot(int node_id, const char * slot, float value);
    /* @} */

    void register_prototype(synth_prototype_ptr const & prototype);

    void cpu_load(float & peak, float & average) const
    {
#ifdef JACK_BACKEND
        return get_cpuload(peak, average);
#else
        peak = average = 0.f;
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

    void increment_logical_time(void)
    {
        sc_osc_handler::increment_logical_time(time_per_tick);
    }

public:
    HOT void operator()(void)
    {
        if (unlikely(dsp_queue_dirty))
            rebuild_dsp_queue();

        sc_factory->initialize_synths();
        scheduler<scheduler_hook, thread_init_functor>::operator()();
    }

    void rebuild_dsp_queue(void);

    void update_dsp_queue(void)
    {
        dsp_queue_dirty = true;
    }

private:
    bool dsp_queue_dirty;

    callback_interpreter<system_callback, false> system_interpreter; // rt to system thread
    threaded_callback_interpreter<system_callback, io_thread_init_functor> io_interpreter; // for network IO
};

inline void run_scheduler_tick(void)
{
    const int blocksize = sc_factory->world.mBufLength;
    const int input_channels = sc_factory->world.mNumInputs;
    const int output_channels = sc_factory->world.mNumOutputs;
    const int buf_counter = ++sc_factory->world.mBufCounter;

    /* touch all input buffers */
    for (int channel = 0; channel != input_channels; ++channel)
        sc_factory->world.mAudioBusTouched[output_channels + channel] = buf_counter;

    (*instance)();

    sc_factory->update_nodegraph();

    /* wipe all untouched output buffers */
    for (int channel = 0; channel != output_channels; ++channel) {
        if (sc_factory->world.mAudioBusTouched[channel] != buf_counter)
            zerovec(sc_factory->world.mAudioBus + blocksize * channel, blocksize);
    }
}

inline void realtime_engine_functor::sync_clock(void)
{
    instance->update_time_from_system();
}

inline void realtime_engine_functor::run_tick(void)
{
    run_scheduler_tick();
    instance->increment_logical_time();
}

inline void scheduler_hook::operator()(void)
{
    instance->execute_scheduled_bundles();
}

inline bool log_printf(const char *fmt, ...)
{
    va_list vargs;
    va_start(vargs, fmt);
    return instance->log_printf(fmt, vargs);
}

inline bool log(const char * string)
{
    return instance->log(string);
}

inline bool log(const char * string, size_t length)
{
    return instance->log(string, length);
}

} /* namespace nova */

#endif /* SERVER_NOVA_SERVER_HPP */
