//  synth factory
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
    struct compare_prototype
    {
        bool operator()(synth_prototype const & lhs,
                        const char * rhs) const
        {
            return strcmp(lhs.name(), rhs) < 0;
        }

        bool operator()(const char * lhs,
                        synth_prototype const & rhs) const
        {
            return strcmp(lhs, rhs.name()) < 0;
        }
    };

public:
    void register_prototype(synth_prototype_ptr const & prototype)
    {
        prototype_map_type::iterator it = prototype_map.find(prototype->name(),
                                                             compare_prototype());
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
        prototype_map_type::iterator it = prototype_map.find(name, compare_prototype());
        if (it == prototype_map.end())
            return 0;

        return it->create_instance(node_id);
    }

    void remove_prototype(const char * name)
    {
        prototype_map_type::iterator it = prototype_map.find(name, compare_prototype());
        if (it == prototype_map.end())
            return;

        prototype_map.erase(it);
        it->release();
    }

    ~synth_factory(void)
    {
        while(prototype_map.begin() != prototype_map.end()) {
            prototype_map_type::iterator it = prototype_map.begin();
            prototype_map.erase(it);
            it->release();
        }
    }


private:
    typedef boost::intrusive::set<synth_prototype> prototype_map_type;
    prototype_map_type prototype_map;
};

} /* namespace nova */

#endif /* SERVER_SYNTH_FACTORY_HPP */
