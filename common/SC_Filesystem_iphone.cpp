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
 * SC_Filesystem implementation for iPhone.
 */
#ifdef SC_IPHONE

#include "SC_Filesystem.hpp"

// system
#include <glob.h> // ::glob, glob_t

using Path = SC_Filesystem::Path;
using DirName = SC_Filesystem::DirName;
using DirMap = SC_Filesystem::DirMap;

//============ DIRECTORY NAMES =============//
const char* LIBRARY_DIR_NAME = "Library";
const char* DOCUMENTS_DIR_NAME = "Documents";
const char* APPLICATION_SUPPORT_DIR_NAME = "Application Support";
const Path ROOT_PATH = Path("/");

//============ PATH UTILITIES =============//

Path SC_Filesystem::resolveIfAlias(const Path& p, bool& isAlias) { isAlias = false; return p; }

//============ GLOB UTILITIES =============//

struct SC_Filesystem::Glob
{
	glob_t mHandle;
	size_t mEntry;
};

SC_Filesystem::Glob* SC_Filesystem::makeGlob(const char* pattern)
{
	Glob* glob = new Glob;

	const int flags = GLOB_MARK | GLOB_TILDE | GLOB_QUOTE;
	const int err = ::glob(pattern, flags, nullptr, &glob->mHandle);
	if (err < 0) {
		delete glob;
		return nullptr;
	}

	glob->mEntry = 0;
	return glob;
}

void SC_Filesystem::freeGlob(Glob* glob)
{
	globfree(&glob->mHandle);
	delete glob;
}

Path SC_Filesystem::globNext(Glob* glob)
{
	if (glob->mEntry >= glob->mHandle.gl_pathc)
		return Path();
	return Path(glob->mHandle.gl_pathv[glob->mEntry++]);
}

//============= PRIVATE METHODS ==============//

bool SC_Filesystem::isNonHostPlatformDirectoryName(const std::string& s)
{
	return s == "linux" || s == "windows" || s == "osx";
}

Path SC_Filesystem::defaultSystemAppSupportDirectory()
{
	// Note: original implementation called sc_AppendBundleName if defined(__APPLE__).
	// However, that function is only defined when !defined(SC_IPHONE). I have taken
	// the more conservative approach and avoided appending the bundle name here. -BH
	return ROOT_PATH;
}

Path SC_Filesystem::defaultUserHomeDirectory()
{
	const char *home = getenv("HOME");
	return Path(home ? home : "");
}

Path SC_Filesystem::defaultUserAppSupportDirectory()
{
	// Note: I have not added XDG support here because that seems highly unlikely on iPhone. -BH
	const Path& p = defaultUserHomeDirectory();
	return p.empty() ? p : p / DOCUMENTS_DIR_NAME;
}

Path SC_Filesystem::defaultUserConfigDirectory()
{
	// Note: I have not added XDG support here because that seems highly unlikely on iPhone. -BH
	return defaultUserAppSupportDirectory();
}

Path SC_Filesystem::defaultResourceDirectory()
{
	return defaultUserAppSupportDirectory();
}

#endif // SC_IPHONE
