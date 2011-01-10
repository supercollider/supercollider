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
#include "synth_prototype.hpp"

namespace nova
{
class synth_factory
{
    struct hash_prototype
    {
        std::size_t operator()(const char * lhs) const
        {
            return synth_prototype::hash(lhs);
        }
    };

    struct equal_prototype
    {
        bool operator()(const char * lhs, synth_prototype const & rhs) const
        {
            return strcmp(lhs, rhs.name()) == 0;
        }
    };

public:
    void register_prototype(synth_prototype_ptr const & prototype)
    {
        prototype_map_type::iterator it = prototype_map.find(*prototype);
        if (it != prototype_map.end())
        {
            prototype_map.erase(it);
            it->release();
        }

        prototype_map.insert(*prototype.get());
        prototype->add_ref();
    }

    abstract_synth * create_instance(const char * name, int node_id)
    {
        prototype_map_type::iterator it = prototype_map.find(name,
                                                             hash_prototype(),
                                                             equal_prototype());
        if (it == prototype_map.end())
            return 0;

        return it->create_instance(node_id);
    }

    void remove_prototype(const char * name)
    {
        prototype_map_type::iterator it = prototype_map.find(name,
                                                             hash_prototype(),
                                                             equal_prototype());
        if (it == prototype_map.end())
            return;

        prototype_map.erase(it);
        it->release();
    }

    std::size_t prototype_count(void) const
    {
        return prototype_map.size();
    }

    synth_factory(void):
        prototype_map(prototype_map_type::bucket_traits(buckets, 1024))
    {}

    ~synth_factory(void)
    {
        while(prototype_map.begin() != prototype_map.end()) {
            prototype_map_type::iterator it = prototype_map.begin();
            prototype_map.erase(it);
            it->release();
        }
    }

private:
    typedef boost::intrusive::unordered_set<synth_prototype,
                                            boost::intrusive::power_2_buckets<true>
                                           > prototype_map_type;
    prototype_map_type::bucket_type buckets[1024];
    prototype_map_type prototype_map;

};

} /* namespace nova */

#endif /* SERVER_SYNTH_FACTORY_HPP */
