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
#include <boost/intrusive/set.hpp>

#include "utilities/exists.hpp"
#include "utilities/utils.hpp"

namespace nova
{

typedef boost::int_fast16_t slot_index_t;
typedef std::string slot_identifier_type;

namespace detail
{

/** class to resolve alphanumeric string to slot id */
class slot_resolver
{
protected:
    struct map_type:
        public boost::intrusive::set_base_hook<>
    {
        map_type(slot_identifier_type const & name, slot_index_t index):
            name(name), index(index)
        {}

        bool operator<(map_type const & rhs) const
        {
            return name < rhs.name;
        }

        slot_identifier_type name;
        slot_index_t index;
    };

private:
    struct comparer
    {
        bool operator()(map_type const & lhs,
                        std::string const & rhs) const
        {
            return lhs.name < rhs;
        }

        bool operator()(std::string const & lhs,
                        map_type const & rhs) const
        {
            return lhs < rhs.name;
        }

        bool operator()(map_type const & lhs,
                        const char * rhs) const
        {
            return strcmp(lhs.name.c_str(), rhs) < 0;
        }

        bool operator()(const char * lhs,
                        map_type const & rhs) const
        {
            return strcmp(lhs, rhs.name.c_str()) < 0;
        }
    };

    template<typename T>
    bool exists(T const & t) const
    {
        return slot_resolver_map.find(t, comparer()) != slot_resolver_map.end();
    }

protected:
    slot_resolver(void)
    {}

    ~slot_resolver(void)
    {
        slot_resolver_map.clear_and_dispose(boost::checked_deleter<map_type>());
    }

    void register_slot(slot_identifier_type const & str, slot_index_t i)
    {
        assert(not exists(str));
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
    slot_index_t resolve_slot(slot_identifier_type const & str) const
    {
        slot_resolver_map_t::const_iterator it = slot_resolver_map.find(str, comparer());
        if (it == slot_resolver_map.end())
            return -1;
        else
            return it->index;
    }

    slot_index_t resolve_slot(const char * str) const
    {
        slot_resolver_map_t::const_iterator it = slot_resolver_map.find(str, comparer());
        if (it == slot_resolver_map.end())
            return -1;
        else
            return it->index;
    }
    /*@}*/

private:
    typedef boost::intrusive::set<map_type> slot_resolver_map_t;
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
    public detail::slot_resolver,
    public boost::intrusive::set_base_hook<>,
    public intrusive_refcountable<synth_prototype_deleter>
{
public:
    synth_prototype(std::string const & name)
    {
        name_ = strdup(name.c_str());
    }

    virtual ~synth_prototype(void)
    {
        free((char*)name_);
    }

    virtual abstract_synth * create_instance(int node_id) = 0;

    /* sort prototypes by name */
    friend bool operator< (synth_prototype const & a,
                           synth_prototype const & b)
    {
        return strcmp(a.name_, b.name_) < 0;
    }

    const char * name(void) const
    {
        return name_;
    }

    template <typename synth_t>
    static inline synth_t * allocate(void);

private:
    const char * name_;
};

typedef boost::intrusive_ptr<synth_prototype> synth_prototype_ptr;

} /* namespace nova */

#endif /* SERVER_SYNTH_PROTOTYPE_HPP */
