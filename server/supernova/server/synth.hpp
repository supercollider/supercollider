//  synth
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

#ifndef SERVER_SYNTH_HPP
#define SERVER_SYNTH_HPP

#include <cassert>
#include <iostream>

#include <boost/cstdint.hpp>

#include "node_types.hpp"
#include "synth_prototype.hpp"
#include "utilities/time_tag.hpp"

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

    slot_index_t resolve_slot(const char * str, std::size_t hashed_str)
    {
        return class_ptr->resolve_slot(str, hashed_str);
    }

    const char * prototype_name(void) const
    {
        return class_ptr->name();
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

public:
    virtual void run(void) = 0;

    /** set a slot */
    /* @{ */
    void set(const char * slot_str, sample val)
    {
        set(slot_str, string_hash(slot_str), val);
    }

    void set(const char * slot_str, size_t count, sample * val)
    {
        set(slot_str, string_hash(slot_str), count, val);
    }

    void set(const char * slot_str, size_t hashed_str, sample val)
    {
        slot_index_t slot_id = prototype_instance::resolve_slot(slot_str, hashed_str);
        if (likely(slot_id >= 0))
            this->set(slot_id, val);
    }

    void set(const char * slot_str, size_t hashed_str, size_t count, sample * val)
    {
        slot_index_t slot_id = prototype_instance::resolve_slot(slot_str, hashed_str);
        if (likely(slot_id >= 0))
            for (size_t i = 0; i != count; ++i)
                this->set(slot_id+i, val[i]);
    }

    virtual void set(slot_index_t slot_id, float val) = 0;
    virtual void set(slot_index_t slot_str, size_t n, float * values) = 0;
    /* @} */
};

} /* namespace nova */

#endif /* SERVER_SYNTH_HPP */
