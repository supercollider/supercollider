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
#include <boost/checked_delete.hpp>

#include "sc_synthdef.hpp"
#include "sc_synth.hpp"
#include "sc_plugin_interface.hpp"

#include "supercollider/Headers/plugin_interface/SC_InterfaceTable.h"
#include "supercollider/Headers/plugin_interface/SC_Unit.h"

namespace nova
{
namespace bi = boost::intrusive;

class sc_ugen_def:
    public bi::set_base_hook<>
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

    Unit * construct(sc_synthdef::unit_spec_t const & unit_spec);
    /* initialize io regions */
    void initialize(Unit * unit, sc_synth * s, sc_synthdef::unit_spec_t const & unit_spec);
    void destruct(Unit * unit);

public:
    /* sort by name */
    friend bool operator< (sc_ugen_def const & a,
                           sc_ugen_def const & b)
    {
        return a.name_ < b.name_;
    }
};

class sc_bufgen_def:
    public bi::set_base_hook<>
{
    const std::string name_;
    const BufGenFunc func;

public:
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
    typedef bi::set<sc_ugen_def> ugen_map_t;
    typedef bi::set<sc_bufgen_def> bufgen_map_t;

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

    ugen_map_t ugen_map;
    bufgen_map_t bufgen_map;

    std::vector<void*> open_handles;

public:
    ~sc_ugen_factory (void)
    {
        ugen_map.clear_and_dispose(boost::checked_delete<sc_ugen_def>);
        bufgen_map.clear_and_dispose(boost::checked_delete<sc_bufgen_def>);
        close_handles();
    }

    sc_unit allocate_ugen(sc_synth * synth, sc_synthdef::unit_spec_t const & unit_spec);
    void free_ugen(sc_unit const & unit);

    void load_plugin_folder(boost::filesystem::path const & path);
    void load_plugin(boost::filesystem::path const & path);

    void register_ugen(const char *inUnitClassName, size_t inAllocSize,
                       UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags);

    void register_bufgen(const char * name, BufGenFunc func);

    uint32_t ugen_count(void) const
    {
        return ugen_count_;
    }

private:
    void close_handles(void);
    uint32_t ugen_count_;
};

extern sc_ugen_factory ugen_factory;

} /* namespace nova */


#endif /* SC_UGEN_FACTORY_HPP */
