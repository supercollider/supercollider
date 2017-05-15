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

#include "SC_Filesystem.hpp"

#ifdef DEBUG_SCFS
#include <iostream>
using std::cout;
using std::endl;
#endif

// stdlib
#include <codecvt>

// system
#include <Shlobj.h> // SHGetKnownFolderPath

using Path = SC_Filesystem::Path;
using DirName = SC_Filesystem::DirName;
using DirMap = SC_Filesystem::DirMap;

//============ PATH UTILITIES =============//

bool SC_Filesystem::isStandalone() { return false; }

Path SC_Filesystem::resolveIfAlias(const Path& p, bool& isAlias) { isAlias = true; return p; }

//============ GLOB UTILITIES =============//

// Keep a buffer of one filename to return on the next call to nextGlob.
// This is because FindFirstFile already loads one filename.
struct SC_Filesystem::Glob
{
	HANDLE mHandle; // find handle
	Path mFolder; // parent folder of the search path
	WIN32_FIND_DATAW mEntry;
	bool mAtEnd; // true if the NEXT call to nextGlob should fail
	Path mFilename; // filename to return on the next call to nextGlob
};

SC_Filesystem::Glob* SC_Filesystem::makeGlob(const char* pattern)
{
	Glob* glob = new Glob;
	boost::filesystem::path path = SC_Codecvt::utf8_str_to_path(pattern);

	glob->mHandle = ::FindFirstFileW(path.wstring().c_str(), &glob->mEntry);
	if (glob->mHandle == INVALID_HANDLE_VALUE) {
		delete glob;
		return nullptr;
	}

	glob->mFolder = path.parent_path();
	glob->mAtEnd = false;
	return glob;
}

void SC_Filesystem::freeGlob(Glob* glob)
{
	::FindClose(glob->mHandle);
	delete glob;
}

Path SC_Filesystem::globNext(Glob* glob)
{
	// loop to ignore . and .. results
	do {
		if (glob->mAtEnd)
			return Path();
		glob->mFilename = glob->mFolder / glob->mEntry.cFileName;
		if (!::FindNextFileW(glob->mHandle, &glob->mEntry))
			glob->mAtEnd = true;
	} while (glob->mFilename.filename_is_dot() || glob->mFilename.filename_is_dot_dot());

	return glob->mFilename;
}

//============= PRIVATE METHODS ==============//

bool SC_Filesystem::isNonHostPlatformDirectoryName(const std::string& s)
{
	return s == "linux" || s == "osx" || s == "iphone";
}

Path SC_Filesystem::defaultSystemAppSupportDirectory()
{
	PWSTR wptr = nullptr;
	const HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &wptr);
	return FAILED(hr) ? Path() : Path(wptr) / SC_FOLDERNAME_APPLICATION_NAME;
}

Path SC_Filesystem::defaultUserHomeDirectory()
{
	PWSTR wptr = nullptr;
	const HRESULT hr = SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, &wptr);
	return FAILED(hr) ? Path() : Path(wptr);
}

Path SC_Filesystem::defaultUserAppSupportDirectory()
{
	PWSTR wptr = nullptr;
	const HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, &wptr);
	return FAILED(hr) ? Path() : Path(wptr) / SC_FOLDERNAME_APPLICATION_NAME;
}

Path SC_Filesystem::defaultUserConfigDirectory()
{
	return defaultUserAppSupportDirectory();
}

Path SC_Filesystem::defaultResourceDirectory()
{
	WCHAR buf[MAX_PATH];
	// @TODO: error check? (none in original)
	GetModuleFileNameW(nullptr, buf, MAX_PATH);
	return Path(buf).parent_path();
}

#endif // _WIN32
