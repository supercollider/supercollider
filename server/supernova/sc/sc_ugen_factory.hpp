//  prototype of a supercollider-synthdef-based synth prototype
//  Copyright (C) 2009 Tim Blechmann
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

#ifndef SC_UGEN_FACTORY_HPP
#define SC_UGEN_FACTORY_HPP

#include <boost/intrusive/set.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <boost/checked_delete.hpp>

#include "sc_synthdef.hpp"
#include "sc_synth.hpp"
#include "sc_plugin_interface.hpp"

#include "SC_InterfaceTable.h"
#include "SC_Unit.h"

namespace nova
{
namespace bi = boost::intrusive;

class sc_ugen_def:
    public bi::unordered_set_base_hook<>
{
private:
    const std::string name_;
    const size_t alloc_size;
    const UnitCtorFunc ctor;
    const UnitDtorFunc dtor;
    const uint32_t flags;

public:
    sc_ugen_def(const char *inUnitClassName, size_t inAllocSize,
                UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags);

    std::string const & name(void) const
    {
        return name_;
    }

    static std::size_t hash(std::string const & value)
    {
        return string_hash(value.c_str());
    }

    friend std::size_t hash_value(sc_ugen_def const & that)
    {
        return hash(that.name());
    }

    Unit * construct(sc_synthdef::unit_spec_t const & unit_spec, sc_synth * s, World * world, char *& chunk);

    void initialize(Unit * unit)
    {
        (*ctor)(unit);
    }

    void destruct(Unit * unit)
    {
        if (dtor)
            (*dtor)(unit);
    }

    bool cant_alias(void) const
    {
        return flags & kUnitDef_CantAliasInputsToOutputs;
    }

    std::size_t memory_requirement(void) const
    {
        return alloc_size;
    }

public:
    /* sort by name */
    friend bool operator< (sc_ugen_def const & a,
                           sc_ugen_def const & b)
    {
        return a.name_ < b.name_;
    }

    friend bool operator== (sc_ugen_def const & a,
                           sc_ugen_def const & b)
    {
        return a.name_ == b.name_;
    }
};

class sc_bufgen_def:
    public bi::set_base_hook<>
{
    const std::string name_;

public:
    const BufGenFunc func;

    sc_bufgen_def(const char * name, BufGenFunc func):
        name_(name), func(func)
    {}

    std::string const & name(void) const
    {
        return name_;
    }

public:
    /* sort by name */
    friend bool operator< (sc_bufgen_def const & a,
                           sc_bufgen_def const & b)
    {
        return a.name_ < b.name_;
    }
};

/** factory class for supercollider ugens
 *
 *  \todo do we need to take care of thread safety? */
class sc_ugen_factory:
    public sc_plugin_interface
{
    static const std::size_t ugen_set_bucket_count = 512;
    typedef bi::unordered_set< sc_ugen_def,
                               bi::constant_time_size<false>,
                               bi::power_2_buckets<true>,
                               bi::store_hash<true>
                             > ugen_set_type;

    typedef bi::set<sc_bufgen_def> bufgen_set_t;

    template<typename def>
    struct compare_def
    {
        bool operator()(def const & lhs,
                        std::string const & rhs) const
        {
            return lhs.name() < rhs;
        }

        bool operator()(std::string const & lhs,
                        def const & rhs) const
        {
            return lhs < rhs.name();
        }

        bool operator()(def const & lhs,
                        const char * rhs) const
        {
            return (strcmp(lhs.name().c_str(), rhs) < 0);
        }

        bool operator()(const char * lhs,
                        def const & rhs) const
        {
            return (strcmp(lhs, rhs.name().c_str()) < 0);
        }
    };

    template<typename def>
    struct equal_def
    {
        bool operator()(def const & lhs,
                        std::string const & rhs) const
        {
            return lhs.name() == rhs;
        }

        bool operator()(std::string const & lhs,
                        def const & rhs) const
        {
            return lhs == rhs.name();
        }
    };

    template<typename def>
    struct hash_def
    {
        std::size_t operator()(std::string const & value)
        {
            return def::hash(value);
        }
    };

    ugen_set_type::bucket_type node_buckets[ugen_set_bucket_count];
    ugen_set_type ugen_set;

    bufgen_set_t bufgen_map;

    std::vector<void*> open_handles;

public:
    sc_ugen_factory (void):
        ugen_set(ugen_set_type::bucket_traits(node_buckets, ugen_set_bucket_count))
    {}

    ~sc_ugen_factory (void)
    {
        ugen_set.clear_and_dispose(boost::checked_delete<sc_ugen_def>);
        bufgen_map.clear_and_dispose(boost::checked_delete<sc_bufgen_def>);
        close_handles();
    }

    /* @{ */
    /** ugen count handling */
    void allocate_ugens(uint32_t count)
    {
        ugen_count_ += count;
    }

    void free_ugens(uint32_t count)
    {
        ugen_count_ -= count;
    }

    uint32_t ugen_count(void) const
    {
        return ugen_count_;
    }
    /* @} */


    void load_plugin_folder(boost::filesystem::path const & path);
    void load_plugin(boost::filesystem::path const & path);

    void register_ugen(const char *inUnitClassName, size_t inAllocSize,
                       UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags);

    void register_bufgen(const char * name, BufGenFunc func);
    BufGenFunc find_bufgen(const char * name);

    sc_ugen_def * find_ugen(std::string const & name);

private:
    void close_handles(void);
    uint32_t ugen_count_;
};

extern sc_ugen_factory * sc_factory;

} /* namespace nova */


#endif /* SC_UGEN_FACTORY_HPP */
