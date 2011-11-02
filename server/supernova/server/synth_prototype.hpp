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

#ifndef SERVER_SYNTH_PROTOTYPE_HPP
#define SERVER_SYNTH_PROTOTYPE_HPP

#include <cassert>

#include <boost/cstdint.hpp>
#include <boost/checked_delete.hpp>
#include <boost/intrusive/unordered_set.hpp>

#include "utilities/aligned_class.hpp"
#include "utilities/named_hash_entry.hpp"
#include "utilities/utils.hpp"

namespace nova {

typedef boost::int16_t slot_index_t;
typedef c_string slot_identifier_type;

namespace detail {

/** class to resolve alphanumeric string to slot id */
class slot_resolver
{
protected:
    struct map_type:
        public named_hash_entry
    {
        map_type(slot_identifier_type const & name, slot_index_t index):
            named_hash_entry(name), index(index)
        {}

        slot_index_t index;
    };

private:
    struct hash_value
    {
        hash_value(std::size_t v):
            value(v)
        {}

        std::size_t operator()(const char *) const
        {
            return value;
        }

        std::size_t value;
    };

    bool exists(const char * str) const
    {
        return slot_resolver_map.find(str, named_hash_hash(), named_hash_equal()) != slot_resolver_map.end();
    }

protected:
    slot_resolver(void):
        slot_resolver_map(slot_resolver_map_t::bucket_traits(buckets, resolver_map_bucket_count))
    {}

    ~slot_resolver(void)
    {
        slot_resolver_map.clear_and_dispose(boost::checked_deleter<map_type>());
    }

    void register_slot(c_string const & str, slot_index_t i)
    {
        assert(not exists(str.c_str()));
        map_type * elem = new map_type(str, i);
        bool success = slot_resolver_map.insert(*elem).second;
        assert(success);
    }

public:
    /*@{*/
    /** resolve slot by symbolic name
     *
     *  \return nonnegative index of slot
     *          -1, if symbolic name cannot be resolved
     */
    slot_index_t resolve_slot(const char * str) const
    {
        return resolve_slot(str, string_hash(str));
    }

    slot_index_t resolve_slot(const char * str, std::size_t hashed_value) const
    {
        slot_resolver_map_t::const_iterator it = slot_resolver_map.find(str, hash_value(hashed_value), named_hash_equal());
        if (it == slot_resolver_map.end())
            return -1;
        else
            return it->index;
    }
    /*@}*/

private:
    static const int resolver_map_bucket_count = 512;

    typedef boost::intrusive::unordered_set<map_type,
                                            boost::intrusive::power_2_buckets<true>
                                           > slot_resolver_map_t;
    slot_resolver_map_t::bucket_type buckets[resolver_map_bucket_count];
    slot_resolver_map_t slot_resolver_map;
};

} /* namespace detail */

class abstract_synth;

struct synth_prototype_deleter
{
    template <typename T>
    void operator()(T * ptr)
    {
        dispose (static_cast<class synth_prototype *>(ptr));
    }

    void dispose(class synth_prototype *);
};

/** prototype of a synth
 *
 * - works as a synth factory
 *
 * */
class synth_prototype:
    public aligned_class,
    public named_hash_entry,
    public intrusive_refcountable<>,
    public detail::slot_resolver
{
public:
    synth_prototype(c_string const & name):
        named_hash_entry(name)
    {}

    virtual ~synth_prototype(void)
    {}

    virtual abstract_synth * create_instance(int node_id) = 0;

    template <typename synth_t>
    static inline synth_t * allocate(void);
};

typedef boost::intrusive_ptr<synth_prototype> synth_prototype_ptr;

} /* namespace nova */

#endif /* SERVER_SYNTH_PROTOTYPE_HPP */
