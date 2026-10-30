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
#include "SC_CoreAudio.h"
#include "SC_UnitDef.h"
#include "SC_BufGen.h"
#include "SC_World.h"
#include "SC_WorldOptions.h"
#include "SC_StringParser.h"
#include "SC_InterfaceTable.h"
#include "SC_Filesystem.hpp"
#include "ErrorMessage.hpp" // for apiVersionMismatch, apiVersionMissing
#include <iostream>

#ifdef STATIC_PLUGINS
#    include "../plugins/SC_StaticPlugins.h"
#else
#    ifdef _WIN32
#        ifndef NOMINMAX
#            define NOMINMAX
#        endif
#        include <windows.h>
#        include "SC_Codecvt.hpp"
#    else
#        include <dlfcn.h>
#        include <sys/param.h>
#    endif // _WIN32

#    ifdef __APPLE__
extern "C" {
#        include <mach-o/dyld.h>
#        include <mach-o/getsect.h>
}
char gTempVal;
#    endif // __APPLE__

#endif // STATIC_PLUGINS

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

std::vector<void*> open_handles;

#    ifdef __APPLE__
void read_section(const struct mach_header* mhp, unsigned long slide, const char* segname, const char* sectname) {
    u_int32_t size;
    char* sect = getsectdatafromheader(mhp, segname, sectname, &size);
    if (!sect)
        return;

    char* start = sect + slide;
    char* end = start + size;

    while (start != end) {
        gTempVal += *(char*)start;
        start++;
    }
}
#    endif

void unloadUGenPlugins() {
#    ifdef _WIN32
    for (void* ptrhinstance : open_handles) {
        HINSTANCE hinstance = (HINSTANCE)ptrhinstance;
        void* ptr = (void*)GetProcAddress(hinstance, "unload");
        if (ptr) {
            UnLoadPlugInFunc unloadFunc = (UnLoadPlugInFunc)ptr;
            (*unloadFunc)();
        }
        FreeLibrary(hinstance);
    }
#    else
    for (void* handle : open_handles) {
        void* ptr = dlsym(handle, "unload");
        if (ptr) {
            UnLoadPlugInFunc unloadFunc = (UnLoadPlugInFunc)ptr;
            (*unloadFunc)();
        }
        dlclose(handle);
    }
#    endif
    open_handles.clear();
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
    /* on darwin plugins are lazily loaded (dlopen uses mmap internally), which can produce audible
        glitches when UGens have to be paged-in. to work around this we preload all the plugins by
        iterating through their memory space. */

#        ifndef __x86_64__
    /* seems to cause a stack corruption on llvm-gcc-4.2, sdk 10.5 on 10.6 */

    unsigned long images = _dyld_image_count();
    for (unsigned long i = 0; i < images; i++) {
        const mach_header* hdr = _dyld_get_image_header(i);
        unsigned long slide = _dyld_get_image_vmaddr_slide(i);
        const char* name = _dyld_get_image_name(i);
        uint32_t size;
        char* sect;

        if (!strcmp(name + (strlen(name) - 4), ".scx")) {
            read_section(hdr, slide, "__TEXT", "__text");
            read_section(hdr, slide, "__TEXT", "__const");
            read_section(hdr, slide, "__TEXT", "__cstring");
            read_section(hdr, slide, "__TEXT", "__picsymbol_stub");
            read_section(hdr, slide, "__TEXT", "__symbol_stub");
            read_section(hdr, slide, "__TEXT", "__const");
            read_section(hdr, slide, "__TEXT", "__literal4");
            read_section(hdr, slide, "__TEXT", "__literal8");

            read_section(hdr, slide, "__DATA", "__data");
            read_section(hdr, slide, "__DATA", "__la_symbol_ptr");
            read_section(hdr, slide, "__DATA", "__nl_symbol_ptr");
            read_section(hdr, slide, "__DATA", "__dyld");
            read_section(hdr, slide, "__DATA", "__const");
            read_section(hdr, slide, "__DATA", "__mod_init_func");
            read_section(hdr, slide, "__DATA", "__bss");
            read_section(hdr, slide, "__DATA", "__common");

            read_section(hdr, slide, "__IMPORT", "__jump_table");
            read_section(hdr, slide, "__IMPORT", "__pointers");
        }
    }
#        endif // __x86_64__

#    endif // ifdef __APPLE__
}

typedef int (*InfoFunction)();

bool checkAPIVersion(void* f, const char* filename) {
    using namespace scsynth;

    if (f) {
        InfoFunction fn = (InfoFunction)f;
        int pluginVersion = (*fn)();
        if (pluginVersion == sc_api_version)
            return true;
        else
            std::cout << ErrorMessage::apiVersionMismatch(filename, sc_api_version, pluginVersion) << std::endl;
    } else {
        std::cout << ErrorMessage::apiVersionNotFound(filename) << std::endl;
    }

    return false;
}

bool checkServerVersion(void* f, const char* filename) {
    if (f) {
        InfoFunction fn = (InfoFunction)f;
        if ((*fn)() != sc_server_scsynth)
            return false;
    }
    return true;
}

static bool PlugIn_Load(const fs::path& filename) {
#    ifdef _WIN32
    // This allows plugins to place DLL dependencies in the same directory.
    SetDllDirectoryW(filename.parent_path().c_str());
    HINSTANCE hinstance = LoadLibraryW(filename.c_str());
    // Reset DLL directory
    SetDllDirectoryW(nullptr);
    // here, we have to use a utf-8 version of the string for printing
    // because the native encoding on Windows is utf-16.
    const std::string filename_utf8_str = SC_Codecvt::path_to_utf8_str(filename);
    if (!hinstance) {
        wchar_t* s = nullptr;
        DWORD lastErr = GetLastError();
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, lastErr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (wchar_t*)&s, 0, NULL);
        scprintf("*** ERROR: LoadLibrary '%s' err '%s'\n", filename_utf8_str.c_str(),
                 SC_Codecvt::utf16_wcstr_to_utf8_string(s).c_str());
        LocalFree(s);
        return false;
    }

    void* apiVersionPtr = (void*)GetProcAddress(hinstance, "api_version");
    if (!checkAPIVersion(apiVersionPtr, filename_utf8_str.c_str())) {
        FreeLibrary(hinstance);
        return false;
    }

    void* serverCheckPtr = (void*)GetProcAddress(hinstance, "server_type");
    if (!checkServerVersion(serverCheckPtr, filename_utf8_str.c_str())) {
        FreeLibrary(hinstance);
        return false;
    }

    void* ptr = (void*)GetProcAddress(hinstance, "load");
    if (!ptr) {
        wchar_t* s;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (wchar_t*)&s, 0, NULL);
        scprintf("*** ERROR: GetProcAddress err '%s'\n", SC_Codecvt::utf16_wcstr_to_utf8_string(s).c_str());
        LocalFree(s);

        FreeLibrary(hinstance);
        return false;
    }

    LoadPlugInFunc loadFunc = (LoadPlugInFunc)ptr;
    (*loadFunc)(&gInterfaceTable);

    open_handles.push_back(hinstance);
    return true;

#    else // (ifndef _WIN32)
    void* handle = dlopen(filename.c_str(), RTLD_NOW);

    if (!handle) {
        scprintf("*** ERROR: dlopen '%s' err '%s'\n", filename.c_str(), dlerror());
        return false;
    }

    void* apiVersionPtr = (void*)dlsym(handle, "api_version");
    if (!checkAPIVersion(apiVersionPtr, filename.c_str())) {
        dlclose(handle);
        return false;
    }

    void* serverCheckPtr = (void*)dlsym(handle, "server_type");
    if (!checkServerVersion(serverCheckPtr, filename.c_str())) {
        dlclose(handle);
        return false;
    }

    void* ptr = dlsym(handle, "load");
    if (!ptr) {
        scprintf("*** ERROR: dlsym load err '%s'\n", dlerror());
        dlclose(handle);
        return false;
    }

    LoadPlugInFunc loadFunc = (LoadPlugInFunc)ptr;
    (*loadFunc)(&gInterfaceTable);

    open_handles.push_back(handle);
    return true;

#    endif // _WIN32
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
