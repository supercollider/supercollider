/*
 *  Copyright (c) 2017 Tim Walters. All rights reserved.
 *  Created by Brian Heim 2017-04-03.
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

#include "SC_Filesystem.hpp"

using SC_Filesystem::Path;
using std::map;

//============= FORWARD DECLS ==============//

// All functions return true for success, false for failure
// @TODO: Decide whose job it is to print error messages
static bool initDirectory(const SC_DirectoryName&);
static bool getSystemAppSupportDirectory(Path&);
static bool getSystemExtensionDirectory(Path&);
static bool getUserHomeDirectory(Path&);
static bool getUserAppSupportDirectory(Path&);
static bool getUserExtensionDirectory(Path&);
static bool getUserConfigDirectory(Path&);
static bool getResourceDirectory(Path&);

// Returns `true` if the directory corresponds to another platform.
static bool isNonHostPlatformDirectory(const Path&);

//============ CLASS FUNCTIONS =============//

Path SC_Filesystem::getDirectory(const SC_DirectoryName& dn)
{
	map<SC_DirectoryName, Path>::const_iterator it = gDirectoryMap.find(dn);
	if (it != gDirectoryMap.end())
		return *it;
	else
		if ( ! initDirectory(dn) )
			// failed, return empty directory
			return Path p;
		else
			return gDirectoryMap.find(dn);
}

void SC_Filesystem::setDirectory(const SC_DirectoryName& dn, const Path& p)
{
	gDirectoryMap[dn] = p;
}

// @TODO
Path expandTilde(const Path& p)
{
	return p;
}

// @TODO
bool shouldNotCompileDirectory(const Path& p)
{
	return false;
}

// @TODO
Path resolveIfAlias(const Path& p, bool& isAlias)
{
	return p;
}

//============= DETAIL FUNCTIONS =============//

// Returns false if initialization failed
static bool initDirectory(const SC_DirectoryName& dn)
{
	Path& p;
	bool ok;

	switch (dn) {
		case SystemAppSupport:
			ok = getSystemAppSupportDirectory(p);
			break;

		case SystemExtension:
			ok = getSystemExtensionDirectory(p);
			break;

		case UserHome:
			ok = getUserHomeDirectory(p);
			break;

		case UserAppSupport:
			ok = getUserAppSupportDirectory(p);
			break;

		case UserExtension:
			ok = getUserExtensionSupportDirectory(p);
			break;

		case UserConfig:
			ok = getUserConfigDirectory(p);
			break;

		case Resource:
			ok = getResourceDirectory(p);
			break;

		default:
			// TODO: print error
			return false;
	}

	if (!ok) {
		// TODO: print error
		return false;
	}

	gDirectoryMap[dn] = p;
	return true;
}

// @TODO: implement getXDirectory functions

// @TODO: implement remainder of functions
