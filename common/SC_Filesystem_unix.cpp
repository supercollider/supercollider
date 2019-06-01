/*
 *  Copyright (c) 2005 Tim Walters. All rights reserved.
 *  Copyright (c) 2017 Brian Heim. All rights reserved.
 *  Created by Tim Walters on 2005-10-19.
 *
 *	Revision history:
 *	  Changed from SC_DirUtils to SC_Filesystem (Brian Heim, 2017-04-03)
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

/*
 * SC_Filesystem implementation for Linux/FreeBSD/OpenBSD.
 */
#if defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)

#    include "SC_Filesystem.hpp"

// system
#    include <glob.h> // ::glob, glob_t

using Path = SC_Filesystem::Path;
using DirName = SC_Filesystem::DirName;
using DirMap = SC_Filesystem::DirMap;

//============ DIRECTORY NAMES =============//
const char* SHARE_DIR_NAME = "share";
const char* USER_DIR_NAME = "usr";
const char* LOCAL_DIR_NAME = "local";
const char* DOT_LOCAL = ".local";
const char* DOT_CONFIG = ".config";
const Path ROOT_PATH = Path("/");

//============ PATH UTILITIES =============//

Path SC_Filesystem::resolveIfAlias(const Path& p, bool& isAlias) {
    isAlias = false;
    return p;
}

//============ GLOB UTILITIES =============//

struct SC_Filesystem::Glob {
    glob_t mHandle;
    size_t mEntry;
};

SC_Filesystem::Glob* SC_Filesystem::makeGlob(const char* pattern) {
    Glob* glob = new Glob;
    const int flags = GLOB_MARK | GLOB_TILDE;
    const int err = ::glob(pattern, flags, nullptr, &glob->mHandle);
    if (err < 0) {
        delete glob;
        return nullptr;
    }

    glob->mEntry = 0;
    return glob;
}

void SC_Filesystem::freeGlob(Glob* glob) {
    globfree(&glob->mHandle);
    delete glob;
}

Path SC_Filesystem::globNext(Glob* glob) {
    if (glob->mEntry >= glob->mHandle.gl_pathc)
        return Path();
    return Path(glob->mHandle.gl_pathv[glob->mEntry++]);
}

//============= PRIVATE METHODS ==============//

bool SC_Filesystem::isNonHostPlatformDirectoryName(const std::string& s) {
    return s == "osx" || s == "windows" || s == "iphone";
}

Path SC_Filesystem::defaultSystemAppSupportDirectory() {
#    ifdef SC_DATA_DIR
    return Path(SC_DATA_DIR);
#    else
    return ROOT_PATH / LOCAL_DIR_NAME / SHARE_DIR_NAME / SC_FOLDERNAME_APPLICATION_NAME;
#    endif
}

Path SC_Filesystem::defaultUserHomeDirectory() {
    const char* home = getenv("HOME");
    return Path(home ? home : "");
}

Path SC_Filesystem::defaultUserAppSupportDirectory() {
    const char* xdg_data_home = getenv("XDG_DATA_HOME");
    if (xdg_data_home)
        return Path(xdg_data_home) / SC_FOLDERNAME_APPLICATION_NAME;

    const Path& p = defaultUserHomeDirectory();
    return p.empty() ? p : p / DOT_LOCAL / SHARE_DIR_NAME / SC_FOLDERNAME_APPLICATION_NAME;
}

Path SC_Filesystem::defaultUserConfigDirectory() {
    const char* xdg_config_home = getenv("XDG_CONFIG_HOME");
    if (xdg_config_home)
        return Path(xdg_config_home) / SC_FOLDERNAME_APPLICATION_NAME;

    const Path& p = defaultUserHomeDirectory();
    return p.empty() ? p : p / DOT_CONFIG / SC_FOLDERNAME_APPLICATION_NAME;
}

Path SC_Filesystem::defaultResourceDirectory() {
#    ifdef SC_DATA_DIR
    return Path(SC_DATA_DIR);
#    else
    return ROOT_PATH / USER_DIR_NAME / SHARE_DIR_NAME / SC_FOLDERNAME_APPLICATION_NAME;
#    endif
}

#endif // defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)
