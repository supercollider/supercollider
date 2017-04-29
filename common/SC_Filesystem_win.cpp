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

// stdlib
#include <string>

#ifdef DEBUG_SCFS
#include <iostream>
using std::cout;
using std::endl;
#endif

// boost
#include <boost/algorithm/string/predicate.hpp> // iequals

// system includes
#include <Shlobj.h> // SHGetKnownFolderPath

using Path = SC_Filesystem::Path;
using DirName = SC_Filesystem::DirName;
using DirMap = SC_Filesystem::DirMap;

//============ DIRECTORY NAMES =============//
const char* gIdeName = "none"; // @TODO: move out
const char* SUPERCOLLIDER_DIR_NAME = "SuperCollider";
const char* EXTENSIONS_DIR_NAME = "Extensions";

//============ PATH UTILITIES =============//

Path SC_Filesystem::getDirectory(const DirName& dn)
{
#ifdef DEBUG_SCFS
	cout << "SCFS::getDirectory: enter" << endl;
#endif
	const DirMap::const_iterator& it = mDirectoryMap.find(dn);
	Path p;
	if (it != mDirectoryMap.end()) {
		p = it->second;
	} else {
		p = initDirectory(dn) ? mDirectoryMap.find(dn)->second : Path();
	}
#ifdef DEBUG_SCFS
	cout << "\tgot " << p << endl;
	cout << "SCFS::getDirectory: exit" << endl;
#endif
	return p;
}

Path SC_Filesystem::expandTilde(const Path& p)
{
	Path::const_iterator piter = p.begin();

	if (piter != p.end() && *piter == "~") {
		Path ret = getDirectory(DirName::UserHome);
		while (++piter != p.end())
			ret /= *piter;
		return ret;
	} else {
		return p;
	}
}

bool SC_Filesystem::shouldNotCompileDirectory(const Path& p) const
{
	const std::string& dirname = p.filename().string(Codecvt());
	const std::string& idePath = std::string("scide_") + gIdeName;
	return (boost::iequals(dirname, "help") ||
			boost::iequals(dirname, "ignore") ||
			dirname == ".svn" ||
			dirname == ".git" ||
			dirname == "_darcs" ||
			dirname == idePath ||
			isNonHostPlatformDirectory(dirname));
}

bool SC_Filesystem::isStandalone() { return false; }

Path SC_Filesystem::resolveIfAlias(const Path& p, bool& isAlias) { isAlias = true; return p; }

// Returns false if initialization failed
bool SC_Filesystem::initDirectory(const DirName& dn)
{
#ifdef DEBUG_SCFS
	cout << "SCFS::initDirectory: enter" << endl;
	cout << "\tdn: " << (int)dn << endl;
#endif
	Path p;

	switch (dn) {
		case DirName::SystemAppSupport:
			p = defaultSystemAppSupportDirectory();
			break;

		case DirName::SystemExtension:
			p = defaultSystemExtensionDirectory();
			break;

		case DirName::UserHome:
			p = defaultUserHomeDirectory();
			break;

		case DirName::UserAppSupport:
			p = defaultUserAppSupportDirectory();
			break;

		case DirName::UserExtension:
			p = defaultUserExtensionDirectory();
			break;

		case DirName::UserConfig:
			p = defaultUserConfigDirectory();
			break;

		case DirName::Resource:
			p = defaultResourceDirectory();
			break;

		default:
#ifdef DEBUG_SCFS
			cout << "\tdefault case" << endl;
#endif
			break;
	}

	if (!p.empty()) {
#ifdef DEBUG_SCFS
		cout << "\tsuccess: " << p << endl;
#endif
		mDirectoryMap[dn] = p;
	} else {
#ifdef DEBUG_SCFS
		cout << "\tinit failed" << endl;
#endif
	}

#ifdef DEBUG_SCFS
	cout << "SCFS::initDirectory: exit" << endl;
#endif
	return !p.empty();
}

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

bool SC_Filesystem::isNonHostPlatformDirectory(const std::string& s)
{
	return s == "linux" || s == "osx" || s == "iphone";
}

Path SC_Filesystem::defaultSystemAppSupportDirectory()
{
	wchar_t* wptr;
	const HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, wptr);
	return FAILED(hr) ? Path() : Path(wptr) / SUPERCOLLIDER_DIR_NAME;
}

Path SC_Filesystem::defaultSystemExtensionDirectory()
{
	const Path& p = defaultSystemAppSupportDirectory();
	return p.empty() ? p : p / EXTENSIONS_DIR_NAME;
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
	return FAILED(hr) ? Path() : Path(wptr) / SUPERCOLLIDER_DIR_NAME;
}

Path SC_Filesystem::defaultUserExtensionDirectory()
{
	const Path& p = defaultUserAppSupportDirectory();
	return p.empty() ? p : p / EXTENSIONS_DIR_NAME;
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
