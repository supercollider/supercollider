/*
 *  Copyright (C) Christof Ressi 2026. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA
 */

#include "SC_ServerPluginModule.hpp"
#include "SC_Codecvt.hpp"

#ifdef _WIN32
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    include <windows.h>
#else
#    include <dlfcn.h>
#    include <sys/param.h>
#endif // _WIN32

#include "SC_InterfaceTable.h"

#include <cassert>
#include <sstream>
#include <unordered_map>
#include <utility>

#ifdef __APPLE__
extern "C" {
#    include <mach-o/dyld.h>
#    include <mach-o/getsect.h>
}

namespace {

volatile char gTempVal;

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

} // namespace

void preloadAllServerPlugins() {
#    ifndef __x86_64__
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
#    endif // __x86_64__
}

#endif // __APPLE__

namespace {

// NOTE: the API version can be overwritten for testing
#if defined(TEST_API_VERSION_MAJOR) || defined(TEST_API_VERSION_MINOR)
constexpr int sc_api_version_major = TEST_API_VERSION_MAJOR;
constexpr int sc_api_version_minor = TEST_API_VERSION_MINOR;
#else
constexpr int sc_api_version_major = SC_API_VERSION_MAJOR;
constexpr int sc_api_version_minor = SC_API_VERSION_MINOR;
#endif

constexpr int sc_api_version = SC_MAKE_API_VERSION(sc_api_version_major, sc_api_version_minor);

std::tuple<int, int> apiVersionTuple(int versionNumber) {
    int major = versionNumber >> 8;
    int minor = versionNumber & 0xFF;
    return { major, minor };
}

// These error messages have been originally implemented by Moss Heim in the now replaced SC_ErrorMessage.hpp header.
namespace ErrorMessage {

//! String used to indent all messages.
constexpr const char* indent = "    ";

//! Returns the error message for a major API version mismatch
std::string majorApiVersionMismatch(const std::filesystem::path& path, int version) {
    // both 1 and 2 were introduced in 3.6.
    static const std::unordered_map<int, std::string> scVersionForAPIVersion = {
        { 1, "3.6.0" }, { 2, "3.6.0" }, { 3, "3.9.0" }, { 4, "3.15" }
    };

    auto [majorVersion, minorVersion] = apiVersionTuple(version);

    std::stringstream ss;
    ss << "API version mismatch.\n";
    try {
        // majorVersion + 1: the version that broke compatibility
        const auto& scVersion = scVersionForAPIVersion.at(majorVersion + 1);
        ss << indent << "This plugin is not compatible with SuperCollider >= " << scVersion << ".\n";
        ss << indent << "The plugin has not been loaded; please find or compile a newer version.\n";
    } catch (const std::out_of_range& exception) {
        ss << indent << "This plugin has been built against a future API version.\n";
        ss << indent << "You may need to update SuperCollider in order to use it.\n";
    }
    ss << indent << "(Plugin's API version: " << majorVersion << "." << minorVersion
       << ", expected: " << sc_api_version_major << ".x)\n";

    // if it looks like sc3-plugins, give specific info to help
#ifdef _WIN32
    if (path.native().find(L"SC3plugins") != std::string::npos) {
#else
    if (path.native().find("SC3plugins") != std::string::npos) {
#endif
        ss << indent << "Releases of sc3-plugins can be downloaded from "
           << "https://github.com/supercollider/sc3-plugins/releases\n";
    }

    return std::move(ss).str();
}

//! Returns the error message for an unmet minimum API version requirement.
std::string minApiVersionNotMet(int minVersion) {
    auto [majorVersion, minorVersion] = apiVersionTuple(minVersion);

    std::stringstream ss;
    ss << "Minimum API version requirement not met.\n";
    ss << indent << "This plugin requires at least API version " << majorVersion << "." << minorVersion << ",\n";
    ss << indent << "but the Server only supports " << sc_api_version_major << "." << sc_api_version_minor
       << " at most.\n";
    ss << indent << "You need to update SuperCollider in order to use this plugin.\n";

    return std::move(ss).str();
}

//! Returns an error message for a missing API version.
std::string symbolNotFound(const char* symbol) {
    std::stringstream ss;
    ss << "Symbol '" << symbol << "' not found.\n";
    ss << indent << "This file may not be a SuperCollider plugin.\n";
    return std::move(ss).str();
}

} // namespace ErrorMessage

//! @brief Function pointer type for reporting the plugin API version or Server type.
using PluginInfoFunction = int (*)();

//! @brief Checks the plugin API version and returns the appropriate error code resp. error message.
//! @param path The plugin file path.
//! @param versionFunc The function that returns the plugin's API version. Can be nullptr!
//! @param minVersionFunc The function that returns the min. required API version. Can be nullptr!
//! @returns an error code and optional error message
[[nodiscard]] std::pair<ServerPluginError, std::string>
checkAPIVersion(const std::filesystem::path& path, PluginInfoFunction versionFunc, PluginInfoFunction minVersionFunc) {
    if (!versionFunc) {
        return { ServerPluginError::MissingSymbol, ErrorMessage::symbolNotFound("api_version") };
    }

    if (!minVersionFunc) {
        // before SC 3.15, "api_version" only returned the major version
        assert(versionFunc);
        int majorVersion = reinterpret_cast<PluginInfoFunction>(versionFunc)();
        if (majorVersion > 3) {
            // plugins compiled against API version 4 or higher are supposed to export "api_min_version".
            return { ServerPluginError::MissingSymbol, ErrorMessage::symbolNotFound("api_min_version") };
        } else {
            int version = SC_MAKE_API_VERSION(majorVersion, 0);
            return { ServerPluginError::MajorAPIVersionMismatch, ErrorMessage::majorApiVersionMismatch(path, version) };
        }
    }

    int version = reinterpret_cast<PluginInfoFunction>(versionFunc)();
    if (auto [majorVersion, minorVersion] = apiVersionTuple(version); majorVersion != sc_api_version_major) {
        // API version mismatch
        return { ServerPluginError::MajorAPIVersionMismatch, ErrorMessage::majorApiVersionMismatch(path, version) };
    }

    int minVersion = reinterpret_cast<PluginInfoFunction>(minVersionFunc)();
    if (minVersion > sc_api_version) {
        // min. required version not met
        return { ServerPluginError::MinRequiredAPIVersionNotMet, ErrorMessage::minApiVersionNotMet(minVersion) };
    }

    return { ServerPluginError::NoError, "" }; // success
}

#ifdef _WIN32

using ModuleHandle = HINSTANCE;

inline void closeModule(ModuleHandle handle) { FreeLibrary(handle); }

inline void* getSymbol(ModuleHandle handle, const char* sym) { return (void*)GetProcAddress(handle, sym); }

#else

using ModuleHandle = void*;

inline void closeModule(ModuleHandle handle) { dlclose(handle); }

inline void* getSymbol(ModuleHandle handle, const char* sym) { return dlsym(handle, sym); }

#endif // _WIN32

} // namespace

ServerPluginModule::ServerPluginModule(const std::filesystem::path& path) {
#ifdef _WIN32
    // This allows plugins to place DLL dependencies in the same directory.
    SetDllDirectoryW(path.parent_path().c_str());
    auto handle = LoadLibraryW(path.c_str());
    SetDllDirectoryW(nullptr); // Reset DLL directory

    if (!handle) {
        wchar_t* message = nullptr;
        DWORD lastErr = GetLastError();
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                       NULL, lastErr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (wchar_t*)&message, 0, NULL);

        std::stringstream ss;
        ss << "LoadLibraryW() failed:\n";
        ss << SC_Codecvt::utf16_to_utf8_str(message); // message already ends with a newline

        LocalFree(message);

        throw ServerPluginException(ServerPluginError::SystemError, std::move(ss).str());
    }
#else
    auto handle = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
    if (!handle) {
        std::stringstream ss;
        ss << "dlopen() failed:\n";
        ss << dlerror() << "\n";
        throw ServerPluginException(ServerPluginError::SystemError, std::move(ss).str());
    }
#endif // _WIN32

    auto serverTypeFunc = reinterpret_cast<PluginInfoFunction>(getSymbol(handle, "server_type"));
#ifdef SUPERNOVA
    if (!serverTypeFunc || serverTypeFunc() != sc_server_supernova) {
#else
    if (serverTypeFunc && serverTypeFunc() != sc_server_scsynth) {
#endif
        throw ServerPluginException(ServerPluginError::WrongServerType, "Wrong Server type.\n");
    }

    auto apiVersionFunc = reinterpret_cast<PluginInfoFunction>(getSymbol(handle, "api_version"));
    auto apiMinVersionFunc = reinterpret_cast<PluginInfoFunction>(getSymbol(handle, "api_min_version"));

    if (auto [errorCode, errorMessage] = checkAPIVersion(path, apiVersionFunc, apiMinVersionFunc);
        errorCode != ServerPluginError::NoError) {
        closeModule(handle);
        throw ServerPluginException(errorCode, std::move(errorMessage));
    }

    m_module = handle;
}

ServerPluginModule::~ServerPluginModule() { close(); }

ServerPluginModule::ServerPluginModule(ServerPluginModule&& other) noexcept:
    m_module(std::exchange(other.m_module, nullptr)) {}

ServerPluginModule& ServerPluginModule::operator=(ServerPluginModule&& other) noexcept {
    close();
    m_module = std::exchange(other.m_module, nullptr);
    return *this;
}

void ServerPluginModule::load(InterfaceTable* interfaceTable, const char* pluginPath) {
    assert(m_module != nullptr);
    auto sym = getSymbol(static_cast<ModuleHandle>(m_module), "load");
    if (sym) {
        auto loadFunc = reinterpret_cast<LoadPlugInFunc>(sym);
        loadFunc(interfaceTable, pluginPath);
    } else {
        throw ServerPluginException(ServerPluginError::MissingSymbol, ErrorMessage::symbolNotFound("load"));
    }
}

void ServerPluginModule::unload() noexcept {
    if (auto sym = getSymbol(static_cast<ModuleHandle>(m_module), "unload")) {
        auto unloadFunc = reinterpret_cast<UnloadPlugInFunc>(sym);
        unloadFunc();
    }
}

void ServerPluginModule::close() noexcept {
    if (m_module) {
        closeModule(static_cast<ModuleHandle>(m_module));
        m_module = nullptr;
    }
}
