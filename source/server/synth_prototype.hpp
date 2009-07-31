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
#include <map>

#include <boost/cstdint.hpp>
#include <boost/intrusive/set.hpp>

#include "utilities/exists.hpp"
#include "utilities/utils.hpp"

namespace nova
{

typedef boost::uint_fast16_t slot_index_t;
typedef std::string slot_identifier_type;

namespace detail
{

/** class to resolve alphanumeric string to slot id */
class slot_resolver
{
protected:
    slot_resolver(void)
    {}

    void register_slot(slot_identifier_type const & str, slot_index_t i)
    {
        assert(not exists(slot_resolver_map, str));
        slot_resolver_map[str] = i;
    }

public:
    slot_index_t resolve_slot(slot_identifier_type const & str) const
    {
        assert(exists(slot_resolver_map, str));
        return slot_resolver_map.find(str)->second;;
    }

private:
    std::map<slot_identifier_type, slot_index_t> slot_resolver_map;
};

} /* namespace detail */

class synth;

/** prototype of a synth
 *
 * - works as a synth factory
 *
 * */
class synth_prototype:
    public detail::slot_resolver,
    public boost::intrusive::set_base_hook<>,
    public intrusive_refcountable
{
public:
    synth_prototype(std::string const & name):
        name_(name)
    {}

    virtual ~synth_prototype(void)
    {}

    virtual synth * create_instance(int node_id) = 0;

    /* sort prototypes by name */
    friend bool operator< (synth_prototype const & a,
                           synth_prototype const & b)
    {
        return a.name_ < b.name_;
    }

    std::string const & name(void) const
    {
        return name_;
    }

    template <typename synth_t>
    static inline synth_t * allocate(void);

private:
    const std::string name_;
};

typedef boost::intrusive_ptr<synth_prototype> synth_prototype_ptr;

} /* namespace nova */

#endif /* SERVER_SYNTH_PROTOTYPE_HPP */
