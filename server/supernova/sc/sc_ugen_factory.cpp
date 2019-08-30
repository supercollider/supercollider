//  prototype of a supercollider-synthdef-based synth prototype
//  Copyright (C) 2009-2013 Tim Blechmann
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

/* \todo for now we use dlopen, later we want to become cross-platform
 */

#ifdef DLOPEN
#    include <dlfcn.h>
#elif defined(_WIN32)
#    include "Windows.h"
#endif

#include <boost/filesystem.hpp>

#include "sc_ugen_factory.hpp"

#include "SC_World.h"
#include "SC_Wire.h"
#include "ErrorMessage.hpp"
#include "SC_Filesystem.hpp" // SC_PLUGIN_EXT

namespace nova {

std::unique_ptr<sc_ugen_factory> sc_factory;

Unit* sc_ugen_def::construct(sc_synthdef::unit_spec_t const& unit_spec, sc_synth* s, World* world,
                             linear_allocator& allocator) {
    const int buffer_length = world->mBufLength;

    const size_t output_count = unit_spec.output_specs.size();

    /* size for wires and buffers */
    uint8_t* chunk = allocator.alloc<uint8_t>(memory_requirement());
    memset(chunk, 0, memory_requirement());

    Unit* unit = (Unit*)(std::uintptr_t(chunk + 63) & (~63)); // align on 64 byte boundary

    unit->mInBuf = allocator.alloc<float*>(unit_spec.input_specs.size());
    unit->mOutBuf = allocator.alloc<float*>(unit_spec.output_specs.size());
    unit->mInput = allocator.alloc<Wire*>(unit_spec.input_specs.size());
    unit->mOutput = allocator.alloc<Wire*>(unit_spec.output_specs.size());

    unit->mNumInputs = unit_spec.input_specs.size();
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

    float* buffer_base = s->unit_buffers;

    /* allocate buffers */
    for (size_t i = 0; i != output_count; ++i) {
        Wire* w = allocator.alloc<Wire>();

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
        } else
            unit->mOutBuf[i] = &w->mScalarValue;

        unit->mOutput[i] = w;
    }

    /* prepare inputs */
    for (size_t i = 0; i != unit_spec.input_specs.size(); ++i) {
        int source = unit_spec.input_specs[i].source;
        int index = unit_spec.input_specs[i].index;

        if (source == -1)
            unit->mInput[i] = &unit->mParent->mWire[index];
        else {
            Unit* prev = s->units[source];
            unit->mInput[i] = prev->mOutput[index];
        }

        if (unit->mInput[i]->mCalcRate == 2)
            unit->mInBuf[i] = unit->mInput[i]->mBuffer;
        else
            unit->mInBuf[i] = &unit->mInput[i]->mScalarValue;
    }

    return unit;
}

bool sc_ugen_def::add_command(const char* cmd_name, UnitCmdFunc func) {
    sc_unitcmd_def* def = new sc_unitcmd_def(cmd_name, func);
    unitcmd_set.insert(*def);
    return true;
}

void sc_ugen_def::run_unit_command(const char* cmd_name, Unit* unit, struct sc_msg_iter* args) {
    unitcmd_set_type::iterator it = unitcmd_set.find(cmd_name, named_hash_hash(), named_hash_equal());

    if (it != unitcmd_set.end())
        it->run(unit, args);
}

sample* sc_bufgen_def::run(World* world, uint32_t buffer_index, struct sc_msg_iter* args) {
    SndBuf* buf = World_GetNRTBuf(world, buffer_index);
    sample* data = buf->data;

    (func)(world, buf, args);

    if (data == buf->data)
        return nullptr;
    else
        return data;
}

void sc_plugin_container::register_ugen(const char* inUnitClassName, size_t inAllocSize, UnitCtorFunc inCtor,
                                        UnitDtorFunc inDtor, uint32 inFlags) {
    sc_ugen_def* def = new sc_ugen_def(inUnitClassName, inAllocSize, inCtor, inDtor, inFlags);
    ugen_set.insert(*def);
}

void sc_plugin_container::register_bufgen(const char* name, BufGenFunc func) {
    sc_bufgen_def* def = new sc_bufgen_def(name, func);
    bufgen_set.insert(*def);
}

sc_ugen_def* sc_plugin_container::find_ugen(symbol const& name) {
    ugen_set_type::iterator it = ugen_set.find(name, named_hash_hash(), named_hash_equal());
    if (it == ugen_set.end())
        return nullptr;

    return &*it;
}

bool sc_plugin_container::register_ugen_command_function(const char* ugen_name, const char* cmd_name,
                                                         UnitCmdFunc func) {
    sc_ugen_def* def = find_ugen(symbol(ugen_name));
    if (def)
        return false;
    return def->add_command(cmd_name, func);
}

bool sc_plugin_container::register_cmd_plugin(const char* cmd_name, PlugInCmdFunc func, void* user_data) {
    cmdplugin_set_type::iterator it = cmdplugin_set.find(cmd_name, named_hash_hash(), named_hash_equal());
    if (it != cmdplugin_set.end()) {
        std::cout << "cmd plugin already registered: " << cmd_name << std::endl;
        return false;
    }

    sc_cmdplugin_def* def = new sc_cmdplugin_def(cmd_name, func, user_data);
    cmdplugin_set.insert(*def);

    return true;
}

sample* sc_plugin_container::run_bufgen(World* world, const char* name, uint32_t buffer_index,
                                        struct sc_msg_iter* args) {
    bufgen_set_type::iterator it = bufgen_set.find(name, named_hash_hash(), named_hash_equal());
    if (it == bufgen_set.end()) {
        std::cout << "unable to find buffer generator: " << name << std::endl;
        return nullptr;
    }

    return it->run(world, buffer_index, args);
}


bool sc_plugin_container::run_cmd_plugin(World* world, const char* name, struct sc_msg_iter* args, void* replyAddr) {
    cmdplugin_set_type::iterator it = cmdplugin_set.find(name, named_hash_hash(), named_hash_equal());
    if (it == cmdplugin_set.end()) {
        std::cout << "unable to find cmd plugin: " << name << std::endl;
        return false;
    }

    it->run(world, args, replyAddr);

    return true;
}


void sc_ugen_factory::load_plugin_folder(boost::filesystem::path const& path) {
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

static bool check_api_version(int (*api_version)(), std::string const& filename) {
    using namespace std;
    using namespace scsynth;

    if (api_version) {
        int plugin_version = (*api_version)();
        if (plugin_version == sc_api_version)
            return true;
        else
            cout << ErrorMessage::apiVersionMismatch(filename, sc_api_version, plugin_version) << endl;
    } else {
        cout << ErrorMessage::apiVersionNotFound(filename) << endl;
    }

    return false;
}

#ifdef DLOPEN
void sc_ugen_factory::load_plugin(boost::filesystem::path const& path) {
    using namespace std;

    // Ignore files that don't have the extension of an SC plugin
    if (path.extension() != SC_PLUGIN_EXT) {
        return;
    }

    void* handle = dlopen(path.string().c_str(), RTLD_NOW | RTLD_LOCAL);
    if (handle == nullptr)
        return;

    typedef int (*info_function)();

    info_function api_version = reinterpret_cast<info_function>(dlsym(handle, "api_version"));

    if (!check_api_version(api_version, path.string())) {
        dlclose(handle);
        return;
    }

    info_function supernova_check = reinterpret_cast<info_function>(dlsym(handle, "server_type"));
    if (!supernova_check || (*supernova_check)() == sc_server_scsynth) {
        // silently ignore
        dlclose(handle);
        return;
    }

    void* load_symbol = dlsym(handle, "load");
    if (!load_symbol) {
        cout << "Problem when loading plugin: \"load\" function undefined" << path << endl;
        dlclose(handle);
        return;
    }

    open_handles.push_back(handle);
    LoadPlugInFunc load_func = reinterpret_cast<LoadPlugInFunc>(load_symbol);
    (*load_func)(&sc_interface);

    /* don't close the handle */
    return;
}

void sc_ugen_factory::close_handles(void) {
    for (void* handle : open_handles) {
        void* ptr = dlsym(handle, "unload");
        if (ptr) {
            UnLoadPlugInFunc unloadFunc = (UnLoadPlugInFunc)ptr;
            (*unloadFunc)();
        }
        dlclose(handle);
    }
}

#elif defined(_WIN32)

void sc_ugen_factory::load_plugin(boost::filesystem::path const& path) {
    // Ignore files that don't have the extension of an SC plugin
    if (path.extension() != SC_PLUGIN_EXT) {
        return;
    }

    // std::cout << "try open plugin: " << path << std::endl;
    const char* filename = path.string().c_str();
    HINSTANCE hinstance = LoadLibrary(path.string().c_str());
    if (!hinstance) {
        char* s;
        DWORD lastErr = GetLastError();
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      nullptr, lastErr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&s, 0, NULL);

        std::cout << "Cannot open plugin: " << path << s << std::endl;
        LocalFree(s);
        return;
    }

    typedef int (*info_function)();
    info_function api_version = reinterpret_cast<info_function>(GetProcAddress(hinstance, "api_version"));

    if (!check_api_version(api_version, filename)) {
        FreeLibrary(hinstance);
        return;
    }

    typedef int (*info_function)();
    info_function server_type = reinterpret_cast<info_function>(GetProcAddress(hinstance, "server_type"));
    if (!server_type) {
        FreeLibrary(hinstance);
        return;
    }

    if ((*server_type)() != sc_server_supernova) {
        FreeLibrary(hinstance);
        return;
    }

    void* ptr = (void*)GetProcAddress(hinstance, "load");
    if (!ptr) {
        char* s;
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                      nullptr, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (char*)&s, 0, NULL);

        std::cout << "*** ERROR: GetProcAddress err " << s << std::endl;
        LocalFree(s);

        FreeLibrary(hinstance);
        return;
    }
    open_handles.push_back(hinstance);
    LoadPlugInFunc loadFunc = (LoadPlugInFunc)ptr;
    (*loadFunc)(&sc_interface);

    return;
}

void sc_ugen_factory::close_handles(void) {
    for (void* ptrhinstance : open_handles) {
        HINSTANCE hinstance = (HINSTANCE)ptrhinstance;
        void* ptr = (void*)GetProcAddress(hinstance, "unload");
        if (ptr) {
            UnLoadPlugInFunc unloadFunc = (UnLoadPlugInFunc)ptr;
            (*unloadFunc)();
        }
        FreeLibrary(hinstance);
    }
}
#else

#endif

} /* namespace nova */
