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

#ifdef STATIC_PLUGINS
#    include "../../plugins/SC_StaticPlugins.h"
#endif // STATIC_PLUGINS

#include "SC_Filesystem.hpp"
#ifdef _WIN32
const char pathSeparator[] = ";";
#else
const char pathSeparator[] = ":";
#endif

using DirName = SC_Filesystem::DirName;

#include <filesystem>
#include <boost/algorithm/string.hpp>

#include "sc_ugen_factory.hpp"

#include "SC_World.h"
#include "SC_Wire.h"
#include "SC_Filesystem.hpp" // SC_PLUGIN_EXT
#include "SC_Codecvt.hpp"
#include "SC_ServerProgramOptions.hpp"

// The generic .so extension has been deprecated in SC 3.15 so developers can ship shared libraries
// along their plugins. We do the following to provide a smooth upgrade path:
// If there has been a plugin with the SC_PLUGIN_EXT extension in the same folder or one of its parent
// folders we simply ignore the .so file because we can assume it is a dependency.
// Otherwise we load the .so file and post a warning.
// We have to make an exception for 'top-level' search paths because system-wide extensions install their
// plugin binaries in the same directory as the core plugins. Since the latter already use the new extension,
// any plugins that still use the old .so extension would be skipped silently.
// NOTE: we should remove this workaround in the future!
#ifdef __linux__
#    define LINUX_PLUGIN_WORKAROUND
#endif

namespace nova {

std::unique_ptr<sc_ugen_factory> sc_factory;

Unit* sc_ugen_def::construct(sc_synthdef::unit_spec_t const& unit_spec, sc_synth* parent, int parentIndex, World* world,
                             sc::linear_allocator& allocator) {
    const int buffer_length = parent->mFullRate->mBufLength;

    const size_t output_count = unit_spec.output_specs.size();

    /* allocate Unit with the given alignment */
    char* chunk = allocator.alloc_aligned<char>(alloc_size, alignment);
    assert(chunk != nullptr);
    memset(chunk, 0, alloc_size);
    Unit* unit = reinterpret_cast<Unit*>(chunk);

    unit->mStructSize = SC_IMPLEMENTED_STRUCT_SIZE(Unit, mBufLength);

    unit->mInBuf = allocator.alloc<float*>(unit_spec.input_specs.size());
    unit->mOutBuf = allocator.alloc<float*>(unit_spec.output_specs.size());
    unit->mInput = allocator.alloc<Wire*>(unit_spec.input_specs.size());
    unit->mOutput = allocator.alloc<Wire*>(unit_spec.output_specs.size());
    Wire* wires = allocator.alloc<Wire>(unit_spec.output_specs.size());

    unit->mNumInputs = unit_spec.input_specs.size();
    unit->mNumOutputs = unit_spec.output_specs.size();

    /* initialize members */
    unit->mCalcRate = unit_spec.rate;
    unit->mSpecialIndex = unit_spec.special_index;
    unit->mDone = false;
    unit->mUnitDef = reinterpret_cast<struct UnitDef*>(this); /* we abuse this field to store our reference */
    unit->mWorld = world;

    /* initialize members from synth */
    unit->mParent = static_cast<Graph*>(parent);
    unit->mParentIndex = parentIndex;
    if (unit_spec.rate == calc_FullRate)
        unit->mRate = parent->mFullRate;
    else
        unit->mRate = parent->mBufRate;

    unit->mBufLength = unit->mRate->mBufLength;

    float* buffer_base = parent->unit_buffers;

    /* allocate buffers */
    for (size_t i = 0; i != output_count; ++i) {
        Wire* w = wires + i;

        w->mFromUnit = unit;
        w->mCalcRate = unit->mCalcRate;

        w->mBuffer = nullptr;
        w->mScalarValue = 0;

        if (unit->mCalcRate == calc_FullRate) {
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
            Unit* prev = parent->units[source];
            unit->mInput[i] = prev->mOutput[index];
        }

        if (unit->mInput[i]->mCalcRate == 2)
            unit->mInBuf[i] = unit->mInput[i]->mBuffer;
        else
            unit->mInBuf[i] = &unit->mInput[i]->mScalarValue;
    }

    return unit;
}

void sc_ugen_def::run_unit_command(const char* cmd_name, Unit* unit, sc_msg_iter* args,
                                   detail::endpoint_ptr const& endpoint) {
    unitcmd_set_type::iterator it = unitcmd_set.find(cmd_name, named_hash_hash(), named_hash_equal());

    if (it != unitcmd_set.end()) {
        it->run(unit, args, endpoint);
    } else {
        std::cout << "can't find unit command: " << cmd_name << std::endl;
    }
}

sample* sc_bufgen_def::run(World* world, uint32_t buffer_index, sc_msg_iter* args) {
    SndBuf* buf = World_GetNRTBuf(world, buffer_index);
    assert(buf != nullptr);

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

sample* sc_plugin_container::run_bufgen(World* world, const char* name, uint32_t buffer_index, sc_msg_iter* args) {
    bufgen_set_type::iterator it = bufgen_set.find(name, named_hash_hash(), named_hash_equal());
    if (it == bufgen_set.end()) {
        std::cout << "unable to find buffer generator: " << name << std::endl;
        return nullptr;
    }

    return it->run(world, buffer_index, args);
}


bool sc_plugin_container::run_cmd_plugin(World* world, const char* name, sc_msg_iter* args,
                                         detail::endpoint_ptr const& endpoint) {
    cmdplugin_set_type::iterator it = cmdplugin_set.find(name, named_hash_hash(), named_hash_equal());
    if (it == cmdplugin_set.end()) {
        std::cout << "unable to find cmd plugin: " << name << std::endl;
        return false;
    }

    it->run(world, args, endpoint);

    return true;
}

#ifdef STATIC_PLUGINS

sc_ugen_factory::~sc_ugen_factory() { unloadStaticPlugins(); }

void sc_ugen_factory::load_plugins(ServerProgramOptions const& args) {
    loadStaticPlugins(&sc_interface);

#    ifndef NDEBUG
    std::cout << "Unit Generators initialized" << std::endl;
#    endif
}

#else

sc_ugen_factory::~sc_ugen_factory() { close_modules(); }

void sc_ugen_factory::load_plugins(ServerProgramOptions const& args) {
    if (args.mUGensPluginPath != nullptr) {
        std::vector<std::string> directories;
        boost::split(directories, args.mUGensPluginPath, boost::is_any_of(pathSeparator));
        for (auto const& path : directories) {
            load_plugin_folder(path);
        }
    } else {
#    ifdef SC_PLUGIN_DIR
        load_plugin_folder(SC_PLUGIN_DIR);
#    endif
        load_plugin_folder(SC_Filesystem::instance().getDirectory(DirName::Resource) / SC_PLUGIN_DIR_NAME);
        load_plugin_folder(SC_Filesystem::instance().getDirectory(DirName::SystemExtension));
        load_plugin_folder(SC_Filesystem::instance().getDirectory(DirName::UserExtension));
        const char* env_plugin_path = getenv("SC_PLUGIN_PATH");
        if (env_plugin_path) {
            std::vector<std::string> directories;
            boost::split(directories, env_plugin_path, boost::is_any_of(pathSeparator));
            for (auto const& path : directories) {
                load_plugin_folder(path);
            }
        }
    }

#    ifndef NDEBUG
    std::cout << "Unit Generators initialized" << std::endl;
#    endif
}

void sc_ugen_factory::load_plugin_folder(std::filesystem::path const& dir) {
#    ifdef LINUX_PLUGIN_WORKAROUND
    load_plugin_folder(dir, true, false);
#    else
    namespace fs = std::filesystem;

    fs::directory_iterator end;

    if (!fs::is_directory(dir))
        return;

    if (SC_Filesystem::instance().shouldNotCompileDirectory(dir)) {
        return;
    }

    fs::directory_iterator iter(dir, fs::directory_options::follow_directory_symlink);
    for (const auto& entry : iter) {
        if (fs::is_regular_file(entry.status())) {
            // Ignore files that don't have the extension of an SC plugin
            if (entry.path().extension() == SC_PLUGIN_EXT) {
                load_plugin(entry.path());
            }
        } else if (fs::is_directory(entry.status())) {
            load_plugin_folder(entry.path());
        }
    }
#    endif
}

#    ifdef LINUX_PLUGIN_WORKAROUND
// if 'found_scx_file' is true, it means that we have alredy found a .scx file in one of the parent
// directories. In this case, we treat all .so files as shared libraries and ignore them.
void sc_ugen_factory::load_plugin_folder(std::filesystem::path const& dir, bool is_top_level, bool found_scx_file) {
    namespace fs = std::filesystem;

    fs::directory_iterator end;

    if (!is_directory(dir))
        return;

    if (SC_Filesystem::instance().shouldNotCompileDirectory(dir)) {
        return;
    }

    auto options = fs::directory_options::follow_directory_symlink;

    // first only iterate over .scx files
    for (const auto& entry : fs::directory_iterator(dir, options)) {
        if (fs::is_regular_file(entry.status()) && entry.path().extension() == SC_PLUGIN_EXT) {
            load_plugin(entry.path());
            found_scx_file = true;
        }
    }

    // then iterate over subdirectories and .so files
    for (const auto& entry : fs::directory_iterator(dir, options)) {
        if (fs::is_directory(entry.path())) {
            load_plugin_folder(entry.path(), false, found_scx_file);
        } else if ((is_top_level || !found_scx_file) && entry.path().extension() == ".so") {
            // No .scx plugins were found in this directory or any parent directory -> assume the .so file is a plugin.
            load_plugin(entry.path());
            std::cout << "WARNING: '" << SC_Codecvt::path_to_utf8_str(entry.path()).c_str()
                      << "': the .so extension has been deprecated!" << std::endl;

            static bool didWarn = false;
            if (!didWarn) {
                std::cout << "*** Please try to upgrade any SC extension where the UGen plugin still has the .so "
                          << "extension.\n"
                          << "*** If you already have the latest version, please ask the developer to update the "
                          << "extension.\n"
                          << "*** To suppress this warning in the meantime, you can manually change the extensions to "
                          << ".scx." << std::endl;
                didWarn = true;
            }
        }
    }
}
#    endif // LINUX_PLUGIN_WORKAROUND

void sc_ugen_factory::load_plugin(std::filesystem::path const& path) {
    const std::string filename_utf8 = SC_Codecvt::path_to_utf8_str(path);

    try {
        ServerPluginModule module(path);
        module.load(&sc_interface, filename_utf8.c_str());

        open_modules.push_back(std::move(module));
    } catch (const ServerPluginException& e) {
        // silently ignore plugins compiled for the wrong Server type
        if (e.errorCode() != ServerPluginError::WrongServerType) {
            std::cout << "ERROR: '" << filename_utf8 << "': " << e.errorMessage() << std::endl;
        }
    }
}

void sc_ugen_factory::close_modules() {
    for (auto& module : open_modules) {
        module.unload();
    }
    open_modules.clear();
}

#endif // STATIC_PLUGINS

} /* namespace nova */
