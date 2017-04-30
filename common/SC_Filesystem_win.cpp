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

// system
#include <Shlobj.h> // SHGetKnownFolderPath

using Path = SC_Filesystem::Path;
using DirName = SC_Filesystem::DirName;
using DirMap = SC_Filesystem::DirMap;

//============ PATH UTILITIES =============//

bool SC_Filesystem::isStandalone() { return false; }

std::string SC_Filesystem::pathAsUTF8String(const Path& p)
{
	return p.string(std::codecvt_utf8<wchar_t>());
}

Path SC_Filesystem::resolveIfAlias(const Path& p, bool& isAlias) { isAlias = true; return p; }

//============ GLOB UTILITIES =============//

// @TODO: replace chars w paths
struct SC_Filesystem::Glob
{
	HANDLE mHandle;
	char mFolder[MAX_PATH];
	WIN32_FIND_DATA mEntry;
	char mEntryPath[MAX_PATH];
	bool mAtEnd;
};

// @TODO: fix up
SC_Filesystem::Glob* SC_Filesystem::makeGlob(const char* pattern)
{
	Glob* glob = new Glob;
	char patternWin[1024];

	strncpy(patternWin, pattern, 1024);
	patternWin[1023] = 0;
	win32_ReplaceCharInString(patternWin, 1024, '/', '\\'); // TODO: replace

	win32_ExtractContainingFolder(glob->mFolder, patternWin, PATH_MAX); // TODO: replace

	glob->mHandle = ::FindFirstFile(patternWin, &glob->mEntry);
	if (glob->mHandle == INVALID_HANDLE_VALUE) {
		delete glob;
		return 0;
	}

	glob->mAtEnd = false;
	return glob;
}

void SC_Filesystem::freeGlob(Glob* glob)
{
	::FindClose(glob->mHandle);
	delete glob;
}

// @TODO: fix up
Path SC_Filesystem::globNext(Glob* glob)
{
	if (glob->mAtEnd)
		return Path();
	strncpy(glob->mEntryPath, glob->mFolder, PATH_MAX);
	glob->mEntryPath = (Path(glob->mEntryPath) / glob->mEntry.cFileName).c_str();
	if (!::FindNextFile(glob->mHandle, &glob->mEntry))
		glob->mAtEnd = true;
	return Path(glob->mEntryPath);
}

//============= PRIVATE METHODS ==============//

bool SC_Filesystem::isNonHostPlatformDirectoryName(const std::string& s)
{
	return s == "linux" || s == "osx" || s == "iphone";
}

Path SC_Filesystem::defaultSystemAppSupportDirectory()
{
	wchar_t* wptr;
	const HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, wptr);
	return FAILED(hr) ? Path() : Path(wptr) / SC_FOLDERNAME_APPLICATION_NAME;
}

Path SC_Filesystem::defaultUserHomeDirectory()
{
	wchar_t* wptr;
	const HRESULT hr = SHGetKnownFolderPath(FOLDERID_Profile, 0, nullptr, wptr);
	return FAILED(hr) ? Path() : Path(wptr);
}

Path SC_Filesystem::defaultUserAppSupportDirectory()
{
	wchar_t* wptr;
	const HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, 0, nullptr, wptr);
	return FAILED(hr) ? Path() : Path(wptr) / SC_FOLDERNAME_APPLICATION_NAME;
}

Path SC_Filesystem::defaultUserConfigDirectory()
{
	return defaultUserAppSupportDirectory();
}

Path SC_Filesystem::defaultResourceDirectory()
{
	wchar_t buf[MAX_PATH];
	// @TODO: error check? (none in original)
	GetModuleFileName(nullptr, buf, MAX_PATH);
	return Path(buf).parent_path();
}

#endif // _WIN32
