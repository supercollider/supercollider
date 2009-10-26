//  synth
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

#ifndef SERVER_SYNTH_HPP
#define SERVER_SYNTH_HPP

#include <cassert>
#include <iostream>

#include <boost/cstdint.hpp>

#include "node_types.hpp"
#include "synth_prototype.hpp"
#include "ugen.hpp"
#include "utilities/time_tag.hpp"
#include "utilities/timer_system.hpp"

namespace nova
{
namespace detail
{

/** wrapper class implementing the functionality of synth, that accesses its prototype */
class synth_prototype_instance
{
public:
    synth_prototype_instance(synth_prototype_ptr const & ptr):
        class_ptr(ptr)
    {}

    slot_index_t resolve_slot(const char * str)
    {
        return class_ptr->resolve_slot(str);
    }

protected:
    synth_prototype_ptr class_ptr;
};

} /* namespace detail */

class abstract_synth:
    public server_node,
    public detail::synth_prototype_instance
{
public:
    typedef float sample;
    typedef boost::uint_fast16_t samplecount_t;

    typedef detail::synth_prototype_instance prototype_instance;

protected:
    abstract_synth(int node_id, synth_prototype_ptr const & prototype):
        server_node(node_id, true), prototype_instance(prototype)
    {}

    virtual void run(dsp_context const & context) = 0;

public:
    /** set a slot */
    /* @{ */
    void set(const char * slot_str, sample val)
    {
        slot_index_t slot_id = prototype_instance::resolve_slot(slot_str);
        if (unlikely(slot_id < 0))
            std::cerr << "cannot resolve slot " << slot_str << std::endl;
        else
            this->set(slot_id, val);
    }

    void set(const char * slot_str, size_t count, sample * val)
    {
        slot_index_t slot_id = prototype_instance::resolve_slot(slot_str);
        if (unlikely(slot_id < 0))
            std::cerr << "cannot resolve slot " << slot_str << std::endl;
        else
            for (int i = 0; i != count; ++i)
                this->set(slot_id+i, val[i]);
    }

    virtual void set(slot_index_t slot_id, sample val) = 0;
    /* @} */
};

/** the synth class
 * */
class synth:
    public abstract_synth,
    protected timer_system<time_tag>
{
public:
    typedef timer_system<time_tag>::timer_callback timer_callback;

public:
    synth(int node_id, synth_prototype_ptr const & prototype):
        abstract_synth(node_id, prototype)
    {}

    void run(dsp_context const & context)
    {
        samplecount_t samples = 64;
        /** \todo run control rate */
        /** \todo currently we are using hardcoded values, since dsp_context is currently not in use */

        const time_tag end_time = current_time() + time_tag::from_samples_small(samples, 44100);
        if (check_time(end_time))
            run_sample_accurate(end_time, samples, context);
        else
        {
            set_current_time(end_time);
            this->run_optimized(samples);
        }
    }

    void set_timer(timer_callback * cb)
    {
        add_callback(cb);
    }

    void set(const char * slot_str, sample val)
    {
        abstract_synth::set(slot_str, val);
    }

    virtual void set(slot_index_t slot_id, sample val)
    {}

    virtual void set(slot_index_t slot_id, size_t count, sample * val)
    {}

private:
    samplecount_t interval_to_samples(time_tag const & tt, dsp_context const & context) const
    {
        return std::ceil((tt.get_secs() + tt.get_fractional_seconds()) * context.samplerate());
    }

    /** sample-accurate dsp computation, based on timer_system */
    void run_sample_accurate(time_tag const & end_time, samplecount_t sample_count, dsp_context const & context)
    {
        assert(current_time() != next_cb_time());
        tick(current_time());

        for(;;)
        {
            const time_tag next = next_cb_time();
            const time_tag diff = next - current_time();
            const samplecount_t samples = std::min(interval_to_samples(diff, context), sample_count);
            const time_tag tick_until = current_time() + time_tag::from_samples_small(samples, context.samplerate());

            assert(samples);
            this->run_loop(samples);

            sample_count -= samples;

            tick(tick_until);

            if (sample_count == 0)
            {
                tick(end_time);
                break;
            }
        }
    }

    virtual void run_loop(samplecount_t samples)
    {
        chain.run(samples, 0);
    }

    /** run optimized version (multiple of 64 samples) */
    virtual void run_optimized(samplecount_t samples)
    {
        chain.run_unrolled(samples, 0);
    }

    friend class node_graph;

protected:
    ugen_chain chain;
};

} /* namespace nova */

#endif /* SERVER_SYNTH_HPP */
