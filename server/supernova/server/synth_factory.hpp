//  synth factory
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

#ifndef SERVER_SYNTH_FACTORY_HPP
#define SERVER_SYNTH_FACTORY_HPP

#include <map>
#include <string>

#include "synth.hpp"
#include "synth_definition.hpp"

namespace nova {

class synth_factory
{
public:
    void register_definition(synth_definition_ptr && prototype)
    {
        prototype_map_type::iterator it = definition_map.find(prototype->name(), named_hash_hash(), named_hash_equal());
        if (it != definition_map.end()) {
            definition_map.erase(it);
            it->release();
        }

        synth_definition_ptr ptr( std::forward<synth_definition_ptr>(prototype) );

        ptr->add_ref();
        definition_map.insert(*ptr.get());
    }

    abstract_synth * create_instance(const char * name, int node_id)
    {
        prototype_map_type::iterator it = definition_map.find(name, named_hash_hash(), named_hash_equal());
        if (it == definition_map.end())
            return nullptr;

        return it->create_instance(node_id);
    }

    void remove_definition(const char * name)
    {
        prototype_map_type::iterator it = definition_map.find(name, named_hash_hash(), named_hash_equal());
        if (it == definition_map.end())
            return;

        definition_map.erase(it);
        it->release();
    }

    std::size_t definition_count(void) const
    {
        return definition_map.size();
    }

    synth_factory(void):
        definition_map(prototype_map_type::bucket_traits(buckets, bucket_count))
    {}

    ~synth_factory(void)
    {
        while(definition_map.begin() != definition_map.end()) {
            prototype_map_type::iterator it = definition_map.begin();
            definition_map.erase(it);
            it->release();
        }
    }

private:
    static const int bucket_count = 2048;

    typedef boost::intrusive::unordered_set<synth_definition,
                                            boost::intrusive::power_2_buckets<true>
                                           > prototype_map_type;
    prototype_map_type::bucket_type buckets[bucket_count];
    prototype_map_type definition_map;
};

} /* namespace nova */

#endif /* SERVER_SYNTH_FACTORY_HPP */
