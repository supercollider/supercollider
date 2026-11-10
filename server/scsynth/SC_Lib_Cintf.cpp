/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/


#include "SC_Lib_Cintf.h"
#include "SC_UnitDef.h"
#include "SC_BufGen.h"
#include "SC_World.h"
#include "SC_WorldOptions.h"
#include "SC_StringParser.h"
#include "SC_InterfaceTable.h"
#include "SC_Codecvt.hpp"
#include "SC_Filesystem.hpp"

#ifdef STATIC_PLUGINS
#    include "../plugins/SC_StaticPlugins.h"
#else
#    include "SC_ServerPluginModule.hpp"
#endif

#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

Malloc gMalloc;
HashTable<SC_LibCmd, Malloc>* gCmdLib = nullptr;
HashTable<struct UnitDef, Malloc>* gUnitDefLib = nullptr;
HashTable<struct BufGen, Malloc>* gBufGenLib = nullptr;
HashTable<struct PlugInCmd, Malloc>* gPlugInCmds = nullptr;
extern struct InterfaceTable gInterfaceTable;
// numeric plugin commands. The SC_LibCmd entries are owned by gCmdLib.
SC_LibCmd* gCmdArray[NUMBER_OF_COMMANDS];

void initMiscCommands();
void loadUGenPlugins(const char* ugensPluginPath);
void unloadUGenPlugins();

void deinitializeLibrary(bool unloadPlugins) {
    if (unloadPlugins) {
#ifdef STATIC_PLUGINS
        unloadStaticPlugins();
#else
        unloadUGenPlugins();
#endif
        delete gCmdLib;
        gCmdLib = nullptr;
        delete gUnitDefLib;
        gUnitDefLib = nullptr;
        delete gBufGenLib;
        gBufGenLib = nullptr;
        delete gPlugInCmds;
        gPlugInCmds = nullptr;
    }
}

void initializeLibrary(const char* ugensPluginPath) {
    gCmdLib = new HashTable<SC_LibCmd, Malloc>(&gMalloc, 64, true);
    gUnitDefLib = new HashTable<UnitDef, Malloc>(&gMalloc, 512, true);
    gBufGenLib = new HashTable<BufGen, Malloc>(&gMalloc, 512, true);
    gPlugInCmds = new HashTable<PlugInCmd, Malloc>(&gMalloc, 64, true);

    initMiscCommands();

#ifdef STATIC_PLUGINS
    loadStaticPlugins(&gInterfaceTable);
#else
    loadUGenPlugins(ugensPluginPath);
#endif // STATIC_PLUGINS
}

/////////////////////////////// load/unload plugins /////////////////////////////

#ifndef STATIC_PLUGINS

// The generic .so extension has been deprecated in SC 3.15 so developers can ship shared libraries
// along their plugins. We do the following to provide a smooth upgrade path:
// If there has been a plugin with the SC_PLUGIN_EXT extension in the same folder or one of its parent
// folders we simply ignore the .so file because we can assume it is a dependency.
// Otherwise we load the .so file and post a warning.
// We have to make an exception for 'top-level' search paths because system-wide extensions install their
// plugin binaries in the same directory as the core plugins. Since the latter already use the new extension,
// any plugins that still use the old .so extension would be skipped silently.
// NOTE: we should remove this workaround in the future!
#    ifdef __linux__
#        define LINUX_PLUGIN_WORKAROUND
#    endif

#    ifdef LINUX_PLUGIN_WORKAROUND
static bool PlugIn_LoadDir(const fs::path& dir, bool reportError, bool isTopLevel = true, bool foundScxFile = false);
#    else
static bool PlugIn_LoadDir(const fs::path& dir, bool reportError);
#    endif

std::vector<ServerPluginModule> open_modules;

void unloadUGenPlugins() {
    for (auto& module : open_modules) {
        module.unload();
    }
    open_modules.clear();
}

void loadUGenPlugins(const char* ugensPluginPath) {
    // If ugensPluginPath is supplied, it is exclusive.
    if (ugensPluginPath) {
        SC_StringParser sp(ugensPluginPath, SC_STRPARSE_PATHDELIMITER);
        while (!sp.AtEnd()) {
            PlugIn_LoadDir(const_cast<char*>(sp.NextToken()), true);
        }
    } else {
        using DirName = SC_Filesystem::DirName;

#    ifdef SC_PLUGIN_DIR
        // load globally installed plugins
        if (fs::is_directory(SC_PLUGIN_DIR)) {
            PlugIn_LoadDir(SC_PLUGIN_DIR, true);
        }
#    endif // SC_PLUGIN_DIR

        // load default plugin directory
        const fs::path pluginDir = SC_Filesystem::instance().getDirectory(DirName::Resource) / SC_PLUGIN_DIR_NAME;

        if (fs::is_directory(pluginDir)) {
            PlugIn_LoadDir(pluginDir, true);
        }

        // load system extension plugins
        const fs::path sysExtDir = SC_Filesystem::instance().getDirectory(DirName::SystemExtension);
        PlugIn_LoadDir(sysExtDir, false);

        // load user extension plugins
        const fs::path userExtDir = SC_Filesystem::instance().getDirectory(DirName::UserExtension);
        PlugIn_LoadDir(userExtDir, false);

        // load user plugin directories
        SC_StringParser sp(getenv("SC_PLUGIN_PATH"), SC_STRPARSE_PATHDELIMITER);
        while (!sp.AtEnd()) {
            PlugIn_LoadDir(sp.NextToken(), true);
        }
    }
#    ifdef __APPLE__
    preloadAllServerPlugins();
#    endif // ifdef __APPLE__
}

static bool PlugIn_Load(const fs::path& path) {
    const std::string filename_utf8 = SC_Codecvt::path_to_utf8_str(path);

    try {
        ServerPluginModule module(path);
        module.load(&gInterfaceTable, filename_utf8.c_str());

        open_modules.push_back(std::move(module));

        return true;
    } catch (const ServerPluginException& e) {
        // silently ignore plugins compiled for the wrong Server type
        if (e.errorCode() != ServerPluginError::WrongServerType) {
            scprintf("ERROR: '%s': %s\n", filename_utf8.c_str(), e.what());
        }
        return false;
    }
}

#    ifndef LINUX_PLUGIN_WORKAROUND
static bool PlugIn_LoadDir(const fs::path& dir, bool reportError) {
#    else
// if 'foundScxFile' is true, it means that we have alredy found a .scx file in one of the parent
// directories. In this case, we treat all .so files as shared libraries and ignore them.
static bool PlugIn_LoadDir(const fs::path& dir, bool reportError, bool isTopLevel, bool foundScxFile) {
#    endif
    std::error_code ec;
    fs::directory_iterator iter(dir, fs::directory_options::follow_directory_symlink, ec);
    fs::directory_iterator end;

    if (ec) {
        if (reportError) {
            scprintf("*** ERROR: open directory failed '%s': %s\n", SC_Codecvt::path_to_utf8_str(dir).c_str(),
                     ec.message().c_str());
            fflush(stdout);
        }
        return false;
    }

    if (SC_Filesystem::instance().shouldNotCompileDirectory(dir)) {
        return true;
    }

#    ifndef LINUX_PLUGIN_WORKAROUND
    while (iter != end) {
        const fs::path path = *iter;

        if (fs::is_directory(iter->status())) {
            PlugIn_LoadDir(path, reportError);
        } else if (path.extension() == SC_PLUGIN_EXT) {
            // don't need to check result: PlugIn_Load does its own error handling and printing.
            // A `false` return value here just means that loading didn't occur, which is not
            // an error condition for us.
            PlugIn_Load(path);
        }

        iter.increment(ec);
        if (ec) {
            scprintf("*** ERROR: Could not iterate on directory '%s': %s\n", SC_Codecvt::path_to_utf8_str(path).c_str(),
                     ec.message().c_str());
            return false;
        }
    }
#    else
    // first only iterate over .scx files
    while (iter != end) {
        const fs::path path = *iter; // copy!

        if (fs::is_regular_file(iter->status()) && path.extension() == SC_PLUGIN_EXT) {
            PlugIn_Load(path);
            foundScxFile = true;
        }

        iter.increment(ec);
        if (ec) {
            scprintf("*** ERROR: Could not iterate on directory '%s': %s\n", SC_Codecvt::path_to_utf8_str(path).c_str(),
                     ec.message().c_str());
            return false;
        }
    }

    // then iterate over subdirectories and .so files. Since we have already iterated over all files,
    // we don't necessarily have to check for errors again.
    try {
        fs::directory_iterator iter(dir, fs::directory_options::follow_directory_symlink);

        for (const auto& entry : iter) {
            const fs::path& path = entry.path();

            if (fs::is_directory(path)) {
                PlugIn_LoadDir(path, reportError, false, foundScxFile);
            } else if ((isTopLevel || !foundScxFile) && path.extension() == ".so") {
                // No .scx plugins were found in this directory or any parent directory -> assume the .so file is a
                // plugin.
                if (PlugIn_Load(path)) {
                    scprintf("WARNING: '%s': the .so extension has been deprecated!\n",
                             SC_Codecvt::path_to_utf8_str(path).c_str());

                    static bool didWarn = false;
                    if (!didWarn) {
                        scprintf("*** Please try to upgrade any SC extension where the UGen plugin still has the .so "
                                 "extension.\n");
                        scprintf("*** If you already have the latest version, please ask the developer to update the "
                                 "extension.\n");
                        scprintf("*** To suppress this warning in the meantime, you can manually change the extension "
                                 "to .scx.\n");
                        didWarn = true;
                    }
                }
            }
        }
    } catch (...) {}
#    endif

    return true;
}

#endif // ifndef STATIC_PLUGINS
