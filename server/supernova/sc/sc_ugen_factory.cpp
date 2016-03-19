//  prototype of a supercollider-synthdef-based synth prototype
//  Copyright (C) 2009-2016 Tim Blechmann
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


#include <boost/dll.hpp>
#include <boost/filesystem.hpp>
#include <boost/predef.h>


#include "sc_ugen_factory.hpp"

#include "SC_World.h"
#include "SC_Wire.h"

namespace nova {

std::unique_ptr<sc_ugen_factory> sc_factory;

Unit * sc_ugen_def::construct(sc_synthdef::unit_spec_t const & unit_spec, sc_synth * s, World * world, linear_allocator & allocator)
{
    const int buffer_length = world->mBufLength;

    const size_t output_count = unit_spec.output_specs.size();

    /* size for wires and buffers */
    uint8_t * chunk  = allocator.alloc<uint8_t>(memory_requirement());
    memset(chunk, 0, memory_requirement());

    Unit * unit   = (Unit*) (std::uintptr_t(chunk + 63) & (~63)); // align on 64 byte boundary

    unit->mInBuf  = allocator.alloc<float*>(unit_spec.input_specs.size());
    unit->mOutBuf = allocator.alloc<float*>(unit_spec.output_specs.size());
    unit->mInput  = allocator.alloc<Wire*>(unit_spec.input_specs.size());
    unit->mOutput = allocator.alloc<Wire*>(unit_spec.output_specs.size());

    unit->mNumInputs  = unit_spec.input_specs.size();
    unit->mNumOutputs = unit_spec.output_specs.size();

    /* initialize members */
    unit->mCalcRate = unit_spec.rate;
    unit->mSpecialIndex = unit_spec.special_index;
    unit->mDone = false;
    unit->mUnitDef = reinterpret_cast<struct UnitDef*>(this); /* we abuse this field to store our reference */
    unit->mWorld = world;

    /* initialize members from synth */
    unit->mParent = static_cast<Graph*>(s);
    if (unit_spec.rate == 2)
        unit->mRate = &world->mFullRate;
    else
        unit->mRate = &world->mBufRate;

    unit->mBufLength = unit->mRate->mBufLength;

    float * buffer_base = s->unit_buffers;

    /* allocate buffers */
    for (size_t i = 0; i != output_count; ++i) {
        Wire * w = allocator.alloc<Wire>();

        w->mFromUnit = unit;
        w->mCalcRate = unit->mCalcRate;

        w->mBuffer = nullptr;
        w->mScalarValue = 0;

        if (unit->mCalcRate == 2) {
            /* allocate a new buffer */
            assert(unit_spec.buffer_mapping[i] >= 0);
            std::size_t buffer_id = unit_spec.buffer_mapping[i];
            unit->mOutBuf[i] = buffer_base + buffer_length * buffer_id;
            w->mBuffer = unit->mOutBuf[i];
        }
        else
            unit->mOutBuf[i] = &w->mScalarValue;

        unit->mOutput[i] = w;
    }

    /* prepare inputs */
    for (size_t i = 0; i != unit_spec.input_specs.size(); ++i)
    {
        int source = unit_spec.input_specs[i].source;
        int index = unit_spec.input_specs[i].index;

        if (source == -1)
            unit->mInput[i] = &unit->mParent->mWire[index];
        else
        {
            Unit * prev = s->units[source];
            unit->mInput[i] = prev->mOutput[index];
        }

        if (unit->mInput[i]->mCalcRate == 2)
            unit->mInBuf[i] = unit->mInput[i]->mBuffer;
        else
            unit->mInBuf[i] = &unit->mInput[i]->mScalarValue;
    }

    return unit;
}

bool sc_ugen_def::add_command(const char* cmd_name, UnitCmdFunc func)
{
    sc_unitcmd_def * def = new sc_unitcmd_def(cmd_name, func);
    unitcmd_set.insert(*def);
    return true;
}

void sc_ugen_def::run_unit_command(const char * cmd_name, Unit * unit, struct sc_msg_iter *args)
{
    unitcmd_set_type::iterator it = unitcmd_set.find(cmd_name, named_hash_hash(), named_hash_equal());

    if (it != unitcmd_set.end())
        it->run(unit, args);
}

sample * sc_bufgen_def::run(World * world, uint32_t buffer_index, struct sc_msg_iter *args)
{
    SndBuf * buf = World_GetNRTBuf(world, buffer_index);
    sample * data = buf->data;

    (func)(world, buf, args);

    if (data == buf->data)
        return nullptr;
    else
        return data;
}

void sc_plugin_container::register_ugen(const char *inUnitClassName, size_t inAllocSize,
                                    UnitCtorFunc inCtor, UnitDtorFunc inDtor, uint32 inFlags)
{
    sc_ugen_def * def = new sc_ugen_def(inUnitClassName, inAllocSize, inCtor, inDtor, inFlags);
    ugen_set.insert(*def);
}

void sc_plugin_container::register_bufgen(const char * name, BufGenFunc func)
{
    sc_bufgen_def * def = new sc_bufgen_def(name, func);
    bufgen_set.insert(*def);
}

sc_ugen_def * sc_plugin_container::find_ugen(symbol const & name)
{
    ugen_set_type::iterator it = ugen_set.find(name, named_hash_hash(), named_hash_equal());
    if (it == ugen_set.end())
        return nullptr;

    return &*it;
}

bool sc_plugin_container::register_ugen_command_function(const char * ugen_name, const char * cmd_name,
                                                     UnitCmdFunc func)
{
    sc_ugen_def * def = find_ugen(symbol(ugen_name));
    if (def)
        return false;
    return def->add_command(cmd_name, func);
}

bool sc_plugin_container::register_cmd_plugin(const char * cmd_name, PlugInCmdFunc func, void * user_data)
{
    cmdplugin_set_type::iterator it = cmdplugin_set.find(cmd_name, named_hash_hash(), named_hash_equal());
    if (it != cmdplugin_set.end()) {
        std::cout << "cmd plugin already registered: " << cmd_name << std::endl;
        return false;
    }

    sc_cmdplugin_def * def = new sc_cmdplugin_def(cmd_name, func, user_data);
    cmdplugin_set.insert(*def);

    return true;
}

sample * sc_plugin_container::run_bufgen(World * world, const char * name, uint32_t buffer_index, struct sc_msg_iter *args)
{
    bufgen_set_type::iterator it = bufgen_set.find(name, named_hash_hash(), named_hash_equal());
    if (it == bufgen_set.end()) {
        std::cout << "unable to find buffer generator: " << name << std::endl;
        return nullptr;
    }

    return it->run(world, buffer_index, args);
}


bool sc_plugin_container::run_cmd_plugin(World * world, const char * name, struct sc_msg_iter *args, void *replyAddr)
{
    cmdplugin_set_type::iterator it = cmdplugin_set.find(name, named_hash_hash(), named_hash_equal());
    if (it == cmdplugin_set.end()) {
        std::cout << "unable to find cmd plugin: " << name << std::endl;
        return false;
    }

    it->run(world, args, replyAddr);

    return true;
}

struct wrong_server_type: public std::exception
{};

sc_server_plugin::sc_server_plugin(const boost::filesystem::path &path, struct InterfaceTable * table):
    plugin( path, boost::dll::load_mode::rtld_now | boost::dll::load_mode::rtld_local )
{
    namespace dll = boost::dll;

    if( !plugin.is_loaded() )
        throw std::runtime_error( std::string("cannot load ") + path.string() );

    try {
        auto api_version = plugin.get<int()>( "api_version" );
        const int plugin_api_version = sc_api_version; // default
        if( api_version() != plugin_api_version )
            throw std::runtime_error( std::string("ABI VersionMismatch: ") + path.string() );
    } catch( boost::system::system_error const & ) {
        throw std::runtime_error( std::string("ABI VersionMismatch: ") + path.string() );
    }

    try {
        auto supernova_check = plugin.get<int()>( "server_type");
        if( supernova_check() != sc_server_supernova )
            throw wrong_server_type();
    } catch( boost::system::system_error const & ) {
        throw wrong_server_type();
    }

    try {
        auto loadFunction = plugin.get<void(struct InterfaceTable *)>( "load" );
        loadFunction( table );
    } catch( boost::system::system_error const & ) {
        throw std::runtime_error( std::string("Problem when loading plugin: \"load\" function undefined: ") + path.string() );
    }

    initialised = true;
}

sc_server_plugin::~sc_server_plugin()
{
    if( initialised ) {
        try {
            auto unloadFunction = plugin.get<void(void)>( "unload" );
            unloadFunction();
        } catch( boost::system::system_error const & ) {
        }
    }
}


void sc_ugen_factory::load_plugin_folder (boost::filesystem::path const & path)
{
    using namespace boost::filesystem;

    directory_iterator end;

    if (!is_directory(path))
        return;

    for (directory_iterator it(path); it != end; ++it) {
        if (is_regular_file(it->status()))
            load_plugin(it->path());
        if (is_directory(it->status()))
            load_plugin_folder(it->path());
    }
}

void sc_ugen_factory::load_plugin ( boost::filesystem::path const & path )
{
#ifdef BOOST_OS_LINUX
    if( path.extension() != ".so" )
        return;
#elif BOOST_OS_WINDOWS
    if( path.extension() != ".dll" )
        return;
#elif BOOST_OS_MACOS
    if( path.extension() != ".scx" )
        return;
#endif

    try {
        sc_server_plugin plugin( path, &sc_interface );

        plugins.emplace_back( std::move( plugin ) );

    } catch( wrong_server_type const & ) {
    } catch( std::exception const & error) {
        std::cout << error.what() << std::endl;
    }
}

} /* namespace nova */
