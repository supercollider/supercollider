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
 * SC_Filesystem implementation for Windows.
 */
#ifdef _WIN32

#    include "SC_Filesystem.hpp"
#    include "SC_Codecvt.hpp"

// boost
#    include <boost/filesystem/operations.hpp> // is_directory

// system
#    include <Shlobj.h> // SHGetKnownFolderPath

using Path = SC_Filesystem::Path;
using DirName = SC_Filesystem::DirName;
using DirMap = SC_Filesystem::DirMap;

//============ PATH UTILITIES =============//

Path SC_Filesystem::resolveIfAlias(const Path& p, bool& isAlias) {
    isAlias = false;
    return p;
}

//============ GLOB UTILITIES =============//

// Keep a buffer of one filename to return on the next call to nextGlob.
// This is because FindFirstFile already loads one filename.
struct SC_Filesystem::Glob {
    HANDLE mHandle; // find handle
    Path mFolder; // parent folder of the search path
    WIN32_FIND_DATAW mEntry;
    bool mAtEnd; // true if the NEXT call to nextGlob should fail
    Path mFilename; // filename to return on the next call to nextGlob
};

SC_Filesystem::Glob* SC_Filesystem::makeGlob(const char* pattern) {
    Glob* glob = new Glob;

    // use make_preferred() to change / -> \ on Windows
    boost::filesystem::path path = SC_Codecvt::utf8_str_to_path(pattern).make_preferred();

    // remove a trailing backslash. Even if searching with 'foo/.', this will
    // change to 'foo' harmlessly. Use has_parent_path() because otherwise '.'
    // (referring to the CWD) won't be recognized as a valid query.
    if (path.filename_is_dot() && path.has_parent_path())
        path = path.parent_path();

    glob->mHandle = ::FindFirstFileW(path.wstring().c_str(), &glob->mEntry);
    if (glob->mHandle == INVALID_HANDLE_VALUE) {
        delete glob;
        return nullptr;
    }

    glob->mFolder = path.parent_path();
    glob->mAtEnd = false;
    return glob;
}

void SC_Filesystem::freeGlob(Glob* glob) {
    ::FindClose(glob->mHandle);
    delete glob;
}

Path SC_Filesystem::globNext(Glob* glob) {
    bool isDirectory = false;

    // loop to ignore . and .. results
    do {
        if (glob->mAtEnd)
            return Path();
        glob->mFilename = glob->mFolder / glob->mEntry.cFileName;

        // record whether it's a directory here, since we overwrite mEntry on the next step
        isDirectory = (glob->mEntry.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) > 0;

        if (!::FindNextFileW(glob->mHandle, &glob->mEntry))
            glob->mAtEnd = true;
    } while (glob->mFilename.filename_is_dot() || glob->mFilename.filename_is_dot_dot());

    // add preferred separator (L'\\') for directories on Windows, to match
    // POSIX globbing. boost::filesystem::is_directory won't work for this because
    // in the case of input '.' and '..', the filename here is just a single folder,
    // not a relative path, and so can't be correctly identified. Plus, it's faster
    // to check the attributes than to make another system call.
    return isDirectory ? glob->mFilename += boost::filesystem::path::preferred_separator : glob->mFilename;
}

//============= PRIVATE METHODS ==============//

bool SC_Filesystem::isNonHostPlatformDirectoryName(const std::string& s) {
    return s == "linux" || s == "osx" || s == "iphone";
}

Path SC_Filesystem::defaultSystemAppSupportDirectory() {
    PWSTR wptr = nullptr;
    const HRESULT hr = SHGetKnownFolderPath(FOLDERID_ProgramData, 0, nullptr, &wptr);
    return FAILED(hr) ? Path() : Path(wptr) / SC_FOLDERNAME_APPLICATION_NAME;
}

Path SC_Filesystem::defaultUserHomeDirectory() {
    PWSTR wptr = nullptr;
    const HRESULT hr = SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &wptr);
    return FAILED(hr) ? Path() : Path(wptr);
}

Path SC_Filesystem::defaultUserAppSupportDirectory() {
    PWSTR wptr = nullptr;
    const HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &wptr);
    return FAILED(hr) ? Path() : Path(wptr) / SC_FOLDERNAME_APPLICATION_NAME;
}

Path SC_Filesystem::defaultUserConfigDirectory() { return defaultUserAppSupportDirectory(); }

Path SC_Filesystem::defaultMyDocumentsDirectory() {
    PWSTR wptr = nullptr;
    const HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, nullptr, &wptr);
    return FAILED(hr) ? Path() : Path(wptr) / SC_FOLDERNAME_APPLICATION_NAME;
}

Path SC_Filesystem::defaultResourceDirectory() {
    WCHAR buf[MAX_PATH];
    GetModuleFileNameW(nullptr, buf, MAX_PATH);
    return Path(buf).parent_path();
}

#endif // _WIN32
