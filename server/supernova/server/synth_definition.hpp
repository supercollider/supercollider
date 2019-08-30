//  synth prototype
//  Copyright (C) 2008 Tim Blechmann
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
#include <cstdint>

#include <boost/checked_delete.hpp>
#include <boost/intrusive/unordered_set.hpp>

#include "utilities/aligned_class.hpp"
#include "utilities/named_hash_entry.hpp"
#include "utilities/utils.hpp"

namespace nova {

typedef std::int32_t slot_index_t;
typedef symbol slot_identifier_type;

namespace detail {

/** class to resolve alphanumeric string to slot id */
class slot_resolver {
protected:
    struct map_type : public named_hash_entry {
        map_type(slot_identifier_type const& name, slot_index_t index, int number_of_values):
            named_hash_entry(name),
            index(index),
            number_of_values(number_of_values) {}

        const slot_index_t index;
        const int number_of_values;
    };

private:
    struct hash_value {
        hash_value(std::size_t v): value(v) {}

        std::size_t operator()(const char*) const { return value; }

        const std::size_t value;
    };

    bool exists(const char* str) const {
        return slot_resolver_map.find(str, named_hash_hash(), named_hash_equal()) != slot_resolver_map.end();
    }

protected:
    slot_resolver(void): slot_resolver_map(slot_resolver_map_t::bucket_traits(buckets, resolver_map_bucket_count)) {}

    ~slot_resolver(void) { slot_resolver_map.clear_and_dispose(boost::checked_deleter<map_type>()); }

    void register_slot(symbol const& str, slot_index_t i, int number_of_values) {
        assert(not exists(str.c_str()));
        map_type* elem = new map_type(str, i, number_of_values);
        bool success = slot_resolver_map.insert(*elem).second;
        assert(success);

        if (size_t(i) >= slot_names.size())
            slot_names.resize(i + 1, nullptr);
        slot_names[i] = elem->name();
    }

public:
    /*@{*/
    /** resolve slot by symbolic name
     *
     *  \return nonnegative index of slot
     *          -1, if symbolic name cannot be resolved
     */
    slot_index_t resolve_slot(const char* str) const { return resolve_slot(str, string_hash(str)); }

    slot_index_t resolve_slot_with_size(const char* str, int& number_of_values) const {
        return resolve_slot_with_size(str, string_hash(str), number_of_values);
    }

    slot_index_t resolve_slot(const char* str, std::size_t hashed_value) const {
        auto it = slot_resolver_map.find(str, hash_value(hashed_value), named_hash_equal());
        if (it == slot_resolver_map.end())
            return -1;
        else
            return it->index;
    }

    slot_index_t resolve_slot_with_size(const char* str, std::size_t hashed_value, int& number_of_values) const {
        auto it = slot_resolver_map.find(str, hash_value(hashed_value), named_hash_equal());
        if (it == slot_resolver_map.end())
            return -1;

        number_of_values = it->number_of_values;
        return it->index;
    }
    /*@}*/

    size_t number_of_slots() const { return slot_names.size(); }

    const char* name_of_slot(size_t slot_index) const {
        assert(slot_index < slot_names.size());
        return slot_names[slot_index];
    }

private:
    static const int resolver_map_bucket_count = 512;

    typedef boost::intrusive::unordered_set<map_type, boost::intrusive::power_2_buckets<true>> slot_resolver_map_t;
    slot_resolver_map_t::bucket_type buckets[resolver_map_bucket_count];
    slot_resolver_map_t slot_resolver_map;

    std::vector<const char*> slot_names;
};

} /* namespace detail */

class abstract_synth;

struct synth_definition_deleter {
    template <typename T> void operator()(T* ptr) { dispose(static_cast<class synth_definition*>(ptr)); }

    void dispose(class synth_definition*);
};

/** prototype of a synth
 *
 * - works as a synth factory
 *
 * */
class synth_definition : public aligned_class,
                         public named_hash_entry,
                         public intrusive_refcountable<>,
                         public detail::slot_resolver {
public:
    explicit synth_definition(symbol const& name): named_hash_entry(name) {}

    virtual ~synth_definition(void) = default;

    virtual abstract_synth* create_instance(int node_id) = 0;

    template <typename synth_t> static inline synth_t* allocate(void);
};

typedef boost::intrusive_ptr<synth_definition> synth_definition_ptr;

} /* namespace nova */
