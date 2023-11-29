/**
 *  \file SC_Filesystem.hpp
 *
 *  \brief Filesystem utilities for SuperCollider.
 *
 *  This header provides a singleton class, SC_Filesystem, which handles many
 *  important filesystem functions:
 *  - Getting and setting application directories (such as config, resource, and extension)
 *  - Expanding ~ to the home directory
 *  - Alias resolution
 *  - Globbing
 *  - Conversion between Path and UTF-8 encoded string
 *
 *  \note This header replaced an older implementation by Tim Walters, SC_DirUtils.hpp. Most of the globbing and
 *        directory calculation code is his.
 *
 *  $Authors: Brian Heim, Tim Walters $
 *
 *  \version Latest revision by Brian Heim, 2017
 *
 *  $Date: 2017-04-30 $
 *
 *  $Contact: brianlheim@gmail.com $
 *
 *  Created on: 2017-04-03
 *
 *  Original revision by Tim Walters, 2005-10-19.
 */
/*
 *  Copyright (C) Tim Walters, 2005. All rights reserved.
 *  Copyright (C) Brian Heim, 2017. All rights reserved.
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

#ifndef PATH_MAX
#    define PATH_MAX MAX_PATH
#endif

#ifndef MAXPATHLEN
#    define MAXPATHLEN PATH_MAX
#endif

/// Name of the folder used for system and user extensions.
#define SC_FOLDERNAME_EXTENSIONS "Extensions"

/// Name of "SuperCollider" folder. Could probably pop this up another level of importance.
#define SC_FOLDERNAME_APPLICATION_NAME "SuperCollider"

/// Default IDE name.
#define SC_DEFAULT_IDE_NAME "none"

/// Name of the plugins subdirectory of the Resources dir
#define SC_PLUGIN_DIR_NAME "plugins"

/// Extension for server plugins.
#ifndef SC_PLUGIN_EXT
#    define SC_PLUGIN_EXT ".scx"
#endif

#include <map> // map
#include <algorithm> // std::transform
#include <string> // std::string
#include <boost/filesystem/path.hpp> // path

#include "SC_Codecvt.hpp" // path_to_utf8_str

/** \class SC_Filesystem
 *
 *  \brief A singleton class providing SuperCollider filesystem utilities.
 *
 *  - Getting and setting application directories (such as config, resource, and extension)
 *  - Expanding ~ to the home directory
 *  - Alias resolution
 *  - Globbing
 *  - Conversion between Path and UTF-8 encoded string
 */
class SC_Filesystem {
public:
    //--------------------------------------------------------------------------------------//
    //------------------------------ PUBLIC CLASS MEMBERS ----------------------------------//
    //--------------------------------------------------------------------------------------//

    //---------------------------------- PUBLIC TYPES --------------------------------------//

    enum class DirName;
    struct Glob;

    typedef boost::filesystem::path Path; ///< Path type.
    typedef std::map<DirName, Path> DirMap; ///< Type of directory name-to-path map.

    /// SuperCollider common directory names.
    enum class DirName {
        SystemAppSupport,
        SystemExtension,
        UserHome,
        UserAppSupport,
        UserExtension,
        UserConfig,
        Resource,
        MyDocuments // only on Windows, used for Platform.myDocumentsDir in sclang
    };

    //------------------------------- SINGLETON INSTANCE -----------------------------------//

    /// Singleton instance.
    static SC_Filesystem& instance() {
        static SC_Filesystem instance;
        return instance;
    }

    //--------------------------- COMMON DIRECTORY MANAGEMENT ------------------------------//

    /// Get path associated with a common directory; the path is initialized if necessary.
    Path getDirectory(const DirName& dn) {
        const DirMap::const_iterator& it = mDirectoryMap.find(dn);
        if (it != mDirectoryMap.end()) {
            return it->second;
        }
        mDirectoryMap[dn] = defaultDirectory(dn);
        return mDirectoryMap[dn];
    }

    /// Set path associated with a common directory.
    inline void setDirectory(const DirName& dn, const Path& p) { mDirectoryMap[dn] = p; }

    //-------------------------------- GENERAL UTILITIES -----------------------------------//

    /** \brief Checks whether a directory should be compiled.
     * \param p a directory path
     * \return True if the directory should not be traversed during compilation.
     *
     * Specifically, returns true if the directory name, ignoreing case, is equal to:
     * - one of ".svn", ".git", or "_darcs"
     * - one of "help" or "ignore"
     * - a string starting with "scide_" but not ending with the current IDE name
     * - one of "windows", "osx", "iphone", or "linux", but that is not the name of the current platform.
     */
    bool shouldNotCompileDirectory(const Path& p) const {
        std::string dirname = SC_Codecvt::path_to_utf8_str(p.filename());
        std::transform(dirname.begin(), dirname.end(), dirname.begin(), ::tolower);
        return dirname == "help" || dirname == "ignore" || dirname == ".svn" || dirname == ".git" || dirname == "_darcs"
            || isUnusedIdeDirectoryName(dirname) || isNonHostPlatformDirectoryName(dirname);
    }

    /// Expands a path starting with `~` to use the user's home directory.
    Path expandTilde(const Path& p) {
        static const Path tilde("~");
        if (!p.empty() && *p.begin() == tilde) {
            // If the first element in the path is tilde, just concatenate the rest of the path onto it
            const Path homeDir = getDirectory(DirName::UserHome);
            const std::string homeDir_utf8 = SC_Codecvt::path_to_utf8_str(homeDir);
            const std::string inPath_utf8 = SC_Codecvt::path_to_utf8_str(p);
            const std::string result = homeDir_utf8 + inPath_utf8.substr(1);
            return SC_Codecvt::utf8_str_to_path(result);
        } else {
            return p;
        }
    }

    //---------------------------------- IDE UTILITIES -------------------------------------//

    /// Get the IDE name. "none" is the default.
    const std::string& getIdeName() const { return mIdeName; }

    /// Set the IDE name.
    const void setIdeName(const std::string& s) { mIdeName = s; }

    /// Returns true if the IDE name is "none" (the default)
    const bool usingIde() const { return mIdeName != SC_DEFAULT_IDE_NAME; }

    //--------------------------------------------------------------------------------------//
    //------------------------------ PUBLIC STATIC MEMBERS ---------------------------------//
    //--------------------------------------------------------------------------------------//

    //-------------------------------- GENERAL UTILITIES -----------------------------------//

    /** \brief Resolves an alias on macOS.
     * \param p a path to resolve
     * \param isAlias set to true if p is an alias
     * \return An empty path if resolution failed; otherwise, the resolved path.
     *
     * If the path was not an alias, a copy is returned. */
    // Could possibly be split into `isAlias` and `resolveAlias` to avoid
    // unnecessary copying - bh
    static Path resolveIfAlias(const Path& p, bool& isAlias);

    //--------------------------------- GLOB UTILITIES -------------------------------------//

    /** \brief Makes a glob.
     * \param pattern a UTF-8 encoded string
     *
     * Constructs a Glob from a given pattern. Call globNext to get the next
     * Path in the glob. Client is responsible for calling freeGlob. */
    static Glob* makeGlob(const char* pattern);

    /// Next available Path in the glob. Returns empty path to indicate end.
    static Path globNext(Glob* g);

    /// Frees a Glob.
    static void freeGlob(Glob* g);

private:
    //--------------------------------------------------------------------------------------//
    //--------------------------------- PRIVATE MEMBERS ------------------------------------//
    //--------------------------------------------------------------------------------------//

    //---------------------------- CONSTRUCTORS & ASSIGNMENT -------------------------------//

    SC_Filesystem(SC_Filesystem const&) = delete;
    void operator=(SC_Filesystem const&) = delete;

    SC_Filesystem(): mDirectoryMap(), mIdeName(SC_DEFAULT_IDE_NAME) {}

    //----------------------------------- IDE UTILITIES ------------------------------------//

    bool isUnusedIdeDirectoryName(const std::string& s) const {
        return s.size() > 6 && s.substr(0, 6) == "scide_" && s.substr(6) != getIdeName();
    }

    //------------------------------ PRIVATE STATIC MEMBERS --------------------------------//

    /** Of the four strings "windows", "osx", "linux", "iphone", returns true if `p` is one of the
     * three that doesn't correspond to this platform. */
    static bool isNonHostPlatformDirectoryName(const std::string& p);

    static Path defaultDirectory(const DirName& dn) {
        switch (dn) {
        case DirName::SystemAppSupport:
            return defaultSystemAppSupportDirectory();
        case DirName::SystemExtension:
            return defaultSystemExtensionDirectory();
        case DirName::UserHome:
            return defaultUserHomeDirectory();
        case DirName::UserAppSupport:
            return defaultUserAppSupportDirectory();
        case DirName::UserExtension:
            return defaultUserExtensionDirectory();
        case DirName::UserConfig:
            return defaultUserConfigDirectory();
        case DirName::Resource:
            return defaultResourceDirectory();
#ifdef _WIN32
        case DirName::MyDocuments:
            return defaultMyDocumentsDirectory();
#endif
        default:
            return Path();
        }
    }

    static Path defaultSystemAppSupportDirectory();
    static Path defaultUserHomeDirectory();
    static Path defaultUserAppSupportDirectory();
    static Path defaultUserConfigDirectory();
    static Path defaultResourceDirectory();

#ifdef _WIN32
    // this function has no analogue on other OSs, yet.
    static Path defaultMyDocumentsDirectory();
#endif

    static Path defaultSystemExtensionDirectory() {
        const Path& p = defaultSystemAppSupportDirectory();
        return p.empty() ? p : p / SC_FOLDERNAME_EXTENSIONS;
    }

    static Path defaultUserExtensionDirectory() {
        const Path& p = defaultUserAppSupportDirectory();
        return p.empty() ? p : p / SC_FOLDERNAME_EXTENSIONS;
    }

    //------------------------------- PRIVATE DATA MEMBERS ---------------------------------//

    DirMap mDirectoryMap;
    std::string mIdeName;
};
