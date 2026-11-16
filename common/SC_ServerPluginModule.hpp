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

#pragma once

#include <filesystem>

struct InterfaceTable;

enum class ServerPluginError {
    //! No error has occured
    NoError,
    //! The OS could not load the plugin
    SystemError,
    //! The plugin has been built for another Server type
    WrongServerType,
    //! The plugin has been built against another major API version
    MajorAPIVersionMismatch,
    //! The Server does not meet the plugin's min. required API version
    MinRequiredAPIVersionNotMet,
    //! The plugin misses one or more required symbols
    MissingSymbol
};

class ServerPluginException : public std::exception {
public:
    ServerPluginException(ServerPluginError errorCode, std::string errorMessage):
        m_errorCode(errorCode),
        m_errorMessage(std::move(errorMessage)) {}

    //! @see errorMessage()
    const char* what() const noexcept override { return m_errorMessage.c_str(); }

    //! @brief Returns an informative multi-line error message that is suitable
    //! for posting to the console.
    const std::string& errorMessage() const noexcept { return m_errorMessage; }

    //! @brief Returns the error code.
    //! This is useful if certain errors need to be handled specially.
    ServerPluginError errorCode() const noexcept { return m_errorCode; }

private:
    ServerPluginError m_errorCode;
    std::string m_errorMessage;
};

class ServerPluginModule {
public:
    //! @brief Tries to open the plugin at the given path.
    //! @param path The absolute plugin file path.
    //! @throws ServerPluginException if the plugin module could not be loaded.
    ServerPluginModule(const std::filesystem::path& path);

    //! @brief Destructor. Automatically calls close().
    ~ServerPluginModule();

    ServerPluginModule(ServerPluginModule&& other) noexcept;
    ServerPluginModule& operator=(ServerPluginModule&& other) noexcept;

    ServerPluginModule(const ServerPluginModule& other) = delete;
    ServerPluginModule& operator=(const ServerPluginModule& other) = delete;

    //! @brief calls the plugin's entry point function ("load")
    //! @param intefaceTable Pointer to the global interface table
    //! @param pluginPath The absolute plugin path as a UTF-8 encoded C-string
    //! @throws ServerPluginException in case of an error
    //! @attention You must call this exactly once!
    void load(InterfaceTable* interfaceTable, const char* pluginPath);

    //! @brief calls the plugin's (optional) "unload" function.
    //! @attention You must not call this more than once!
    //!
    //! Some plugins have global state that is initialized in the "load" function
    //! and is suppposed to be freed in the "unload" function.
    //! In some plugins the "unload" function also joins one or more helper threads.
    //!
    //! You can decide to not call this function and rather let the plugin leak its
    //! global resources. However, some plugins might crash or even hang the Server
    //! when they are finally unloaded by the OS.
    void unload() noexcept;

    //! @brief Close the plugin module.
    //! If the module has already been closed, this is a no-op.
    //! @attention close() does *not* automatically call unload()!
    void close() noexcept;

private:
    void* m_module = nullptr; // type-erased module handle
};


#ifdef __APPLE__
//! On Darwin platforms, plugins are lazily loaded (dlopen uses mmap internally), which can produce
//! audible glitches when UGens have to be paged-in. To work around this we preload all the plugins
//! by iterating through their memory space.
//! This function should be called after all Server plugins have been loaded.
void preloadAllServerPlugins();
#endif
