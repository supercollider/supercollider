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

#pragma once

#include <cassert>
#include <iostream>

#include <cstdint>

#include "node_types.hpp"
#include "synth_definition.hpp"
#include "utilities/time_tag.hpp"

namespace nova {
namespace detail {

/** wrapper class implementing the functionality of synth, that accesses its prototype */
class synth_definition_instance {
public:
    synth_definition_instance(synth_definition_ptr const& ptr): class_ptr(ptr) {}

    slot_index_t resolve_slot(const char* str) { return class_ptr->resolve_slot(str); }

    slot_index_t resolve_slot_with_size(const char* str, int& size_of_slot) {
        return class_ptr->resolve_slot_with_size(str, size_of_slot);
    }

    slot_index_t resolve_slot(const char* str, std::size_t hashed_str) {
        return class_ptr->resolve_slot(str, hashed_str);
    }

    slot_index_t resolve_slot_with_size(const char* str, std::size_t hashed_str, int& size_of_slot) {
        return class_ptr->resolve_slot_with_size(str, hashed_str, size_of_slot);
    }

    const char* definition_name(void) const { return class_ptr->name(); }

    size_t number_of_slots() const { return class_ptr->number_of_slots(); }

    const char* name_of_slot(size_t index) const { return class_ptr->name_of_slot(index); }

protected:
    synth_definition_ptr class_ptr;
};

} /* namespace detail */

class abstract_synth : public server_node, public detail::synth_definition_instance {
public:
    typedef float sample;
    typedef std::uint_fast16_t samplecount_t;

    typedef detail::synth_definition_instance definition_instance;

protected:
    abstract_synth(int node_id, synth_definition_ptr const& definition):
        server_node(node_id, true),
        definition_instance(definition) {}

public:
    virtual void run(void) = 0;

    virtual float get(slot_index_t slot_id) const = 0;

    /** returns true (and writes characters to *str) if the control at slot_id is indeed mapped, otherwise returns false
     */
    virtual bool getMappedSymbol(slot_index_t slot_id, char* str) const = 0;

    /** set a slot */
    /* @{ */
    virtual void set(slot_index_t slot_id, float val) override = 0;

    void set(const char* slot_str, sample val) override { set(slot_str, string_hash(slot_str), val); }

    void set(const char* slot_str, size_t hashed_str, sample val) override {
        slot_index_t slot_id = definition_instance::resolve_slot(slot_str, hashed_str);
        if (likely(slot_id >= 0))
            this->set(slot_id, val);
    }

    void set_control_array(const char* slot_str, size_t count, sample* val) override {
        set_control_array(slot_str, string_hash(slot_str), count, val);
    }

    void set_control_array(const char* slot_str, size_t hashed_str, size_t count, sample* val) override {
        int size_of_parameter;
        slot_index_t slot_id = definition_instance::resolve_slot_with_size(slot_str, hashed_str, size_of_parameter);
        count = std::min(size_t(size_of_parameter), count);
        if (likely(slot_id >= 0))
            for (size_t i = 0; i != count; ++i)
                this->set(slot_id + i, val[i]);
    }

    virtual void set_control_array(slot_index_t slot_id, size_t n, float* values) override = 0;

    virtual void set_control_array_element(slot_index_t slot_id, size_t index, float val) override {
        this->set(slot_id + index, val);
    }

    void set_control_array_element(const char* slot_str, size_t index, sample val) override {
        set_control_array_element(slot_str, string_hash(slot_str), index, val);
    }

    void set_control_array_element(const char* slot_str, size_t hashed_str, size_t index, sample val) override {
        int size_of_parameter;
        slot_index_t slot_id = definition_instance::resolve_slot_with_size(slot_str, hashed_str, size_of_parameter);
        if (likely((slot_id >= 0) && (index < size_of_parameter)))
            this->set(slot_id + index, val);
    }
    /* @} */
};

} /* namespace nova */
