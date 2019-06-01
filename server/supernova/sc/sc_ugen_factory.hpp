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

#pragma once

#include <boost/intrusive/set.hpp>
#include <boost/intrusive/unordered_set.hpp>
#include <boost/checked_delete.hpp>

#include "sc_synthdef.hpp"
#include "sc_synth.hpp"
#include "sc_plugin_interface.hpp"

#include "SC_InterfaceTable.h"
#include "SC_Unit.h"

#include "utilities/named_hash_entry.hpp"

namespace nova {
namespace bi = boost::intrusive;

struct sc_unitcmd_def : public named_hash_entry {
    const UnitCmdFunc func;

    sc_unitcmd_def(const char* cmd_name, UnitCmdFunc func): named_hash_entry(cmd_name), func(func) {}

    void run(Unit* unit, struct sc_msg_iter* args) { (func)(unit, args); }
};

class sc_ugen_def : public aligned_class, public named_hash_entry {
private:
    const size_t alloc_size;
    const UnitCtorFunc ctor;
    const UnitDtorFunc dtor;
    const uint32_t flags;

    static const std::size_t unitcmd_set_bucket_count = 4;
    typedef bi::unordered_set<sc_unitcmd_def, bi::constant_time_size<false>, bi::power_2_buckets<true>,
                              bi::store_hash<true>>
        unitcmd_set_type;

    unitcmd_set_type::bucket_type unitcmd_set_buckets[unitcmd_set_bucket_count];
    unitcmd_set_type unitcmd_set;

public:
    sc_ugen_def(const char* inUnitClassName, size_t inAllocSize, UnitCtorFunc inCtor, UnitDtorFunc inDtor,
                uint32 inFlags):
        named_hash_entry(inUnitClassName),
        alloc_size(inAllocSize),
        ctor(inCtor),
        dtor(inDtor),
        flags(inFlags),
        unitcmd_set(unitcmd_set_type::bucket_traits(unitcmd_set_buckets, unitcmd_set_bucket_count)) {}

    Unit* construct(sc_synthdef::unit_spec_t const& unit_spec, sc_synth* s, World* world, linear_allocator& allocator);

    void initialize(Unit* unit) { (*ctor)(unit); }

    void destruct(Unit* unit) {
        if (dtor)
            (*dtor)(unit);
    }

    bool cant_alias(void) const { return flags & kUnitDef_CantAliasInputsToOutputs; }

    std::size_t memory_requirement(void) const {
        return alloc_size + 64; // overallocate to allow alignment
    }

    bool add_command(const char* cmd_name, UnitCmdFunc func);
    void run_unit_command(const char* cmd_name, Unit* unit, struct sc_msg_iter* args);
};

struct sc_bufgen_def : public named_hash_entry {
    const BufGenFunc func;

    sc_bufgen_def(const char* name, BufGenFunc func): named_hash_entry(name), func(func) {}

    sample* run(World* world, uint32_t buffer_index, struct sc_msg_iter* args);
};

struct sc_cmdplugin_def : public named_hash_entry {
    const PlugInCmdFunc func;
    void* user_data;

    sc_cmdplugin_def(const char* name, PlugInCmdFunc func, void* user_data):
        named_hash_entry(name),
        func(func),
        user_data(user_data) {}

    void run(World* world, struct sc_msg_iter* args, void* replyAddr) { (func)(world, user_data, args, replyAddr); }
};

class sc_plugin_container {
    static const std::size_t ugen_set_bucket_count = 512;
    typedef bi::unordered_set<sc_ugen_def, bi::constant_time_size<false>, bi::power_2_buckets<true>,
                              bi::store_hash<true>>
        ugen_set_type;

    static const std::size_t bufgen_set_bucket_count = 64;
    typedef bi::unordered_set<sc_bufgen_def, bi::constant_time_size<false>, bi::power_2_buckets<true>,
                              bi::store_hash<true>>
        bufgen_set_type;

    static const std::size_t cmdplugin_set_bucket_count = 8;
    typedef bi::unordered_set<sc_cmdplugin_def, bi::constant_time_size<false>, bi::power_2_buckets<true>,
                              bi::store_hash<true>>
        cmdplugin_set_type;

    ugen_set_type::bucket_type ugen_set_buckets[ugen_set_bucket_count];
    ugen_set_type ugen_set;

    bufgen_set_type::bucket_type bufgen_set_buckets[bufgen_set_bucket_count];
    bufgen_set_type bufgen_set;

    cmdplugin_set_type::bucket_type cmdplugin_set_buckets[cmdplugin_set_bucket_count];
    cmdplugin_set_type cmdplugin_set;

protected:
    sc_plugin_container(void):
        ugen_set(ugen_set_type::bucket_traits(ugen_set_buckets, ugen_set_bucket_count)),
        bufgen_set(bufgen_set_type::bucket_traits(bufgen_set_buckets, bufgen_set_bucket_count)),
        cmdplugin_set(cmdplugin_set_type::bucket_traits(cmdplugin_set_buckets, cmdplugin_set_bucket_count)) {}

    ~sc_plugin_container(void) {
        ugen_set.clear_and_dispose(boost::checked_delete<sc_ugen_def>);
        bufgen_set.clear_and_dispose(boost::checked_delete<sc_bufgen_def>);
        cmdplugin_set.clear_and_dispose(boost::checked_delete<sc_cmdplugin_def>);
    }

public:
    void register_ugen(const char* inUnitClassName, size_t inAllocSize, UnitCtorFunc inCtor, UnitDtorFunc inDtor,
                       uint32 inFlags);

    void register_bufgen(const char* name, BufGenFunc func);

    sc_ugen_def* find_ugen(symbol const& name);

    bool register_ugen_command_function(const char* ugen_name, const char* cmd_name, UnitCmdFunc);
    bool register_cmd_plugin(const char* cmd_name, PlugInCmdFunc func, void* user_data);

    sample* run_bufgen(World* world, const char* name, uint32_t buffer_index, struct sc_msg_iter* args);
    bool run_cmd_plugin(World* world, const char* name, struct sc_msg_iter* args, void* replyAddr);
};

/** factory class for supercollider ugens
 *
 *  \todo do we need to take care of thread safety? */
class sc_ugen_factory : public sc_plugin_interface, public sc_plugin_container {
public:
    sc_ugen_factory() = default;

    ~sc_ugen_factory(void) { close_handles(); }

    /* @{ */
    /** ugen count handling */
    void allocate_ugens(uint32_t count) { ugen_count_ += count; }

    void free_ugens(uint32_t count) { ugen_count_ -= count; }

    uint32_t ugen_count(void) const { return ugen_count_; }
    /* @} */

    void load_plugin_folder(boost::filesystem::path const& path);
    void load_plugin(boost::filesystem::path const& path);

private:
    void close_handles(void);

    uint32_t ugen_count_ = 0;
    std::vector<void*> open_handles;
};

extern std::unique_ptr<sc_ugen_factory> sc_factory;

} /* namespace nova */
