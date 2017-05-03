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
 * SC_Filesystem implementation for macOS.
 */
#if defined(__APPLE__) && !defined(SC_IPHONE)

#include "SC_Filesystem.hpp"
#include "SC_StandAloneInfo_Darwin.h"

#ifdef DEBUG_SCFS
#include <iostream>
using std::cout;
using std::endl;
#endif

// system includes
#include <Foundation/NSAutoreleasePool.h>
#include <Foundation/NSFileManager.h>
#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFString.h>

#include <glob.h> // ::glob, glob_t

using Path = SC_Filesystem::Path;
using DirName = SC_Filesystem::DirName;
using DirMap = SC_Filesystem::DirMap;

//============ DIRECTORY NAMES =============//
const char* LIBRARY_DIR_NAME = "Library";
const char* APPLICATION_SUPPORT_DIR_NAME = "Application Support";
const Path ROOT_PATH = Path("/");

//====== STATIC FUNC FORWARD DECLS =======//

// Get the bundle name
static const char* getBundleName();

//============ PATH UTILITIES =============//

bool SC_Filesystem::isStandalone()
{
	return SC_StandAloneInfo::IsStandAlone();
}

std::string SC_Filesystem::pathAsUTF8String(const Path& p) { return p.string(); }

Path SC_Filesystem::UTF8StringAsPath(const std::string& s) { return Path(s); }

Path SC_Filesystem::resolveIfAlias(const Path& p, bool& isAlias)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString *nsstringPath = [NSString stringWithCString: p.c_str() encoding: NSUTF8StringEncoding];
	BOOL isDirectory;

	// does the file exist? If not just copy and bail
	if ([[NSFileManager defaultManager] fileExistsAtPath: nsstringPath isDirectory: &isDirectory]) {
		NSError *error;
		NSData *bookmark = [NSURL bookmarkDataWithContentsOfURL: [NSURL fileURLWithPath:nsstringPath isDirectory: isDirectory] error: &error];

		// is it an alias?
		if (bookmark) {
			isAlias = true;
			NSError *resolvedURLError;
			BOOL isStale;
			NSURL *resolvedURL = [NSURL URLByResolvingBookmarkData: bookmark options: NSURLBookmarkResolutionWithoutUI relativeToURL: nil bookmarkDataIsStale: &isStale error: &resolvedURLError];
			// does it actually lead to something?
			if (isStale || resolvedURL == nullptr) {
				// Return original path.
				printf("Error: Target missing for alias at %s\n", p.c_str());
				return Path();
			}

			NSString *resolvedString = [resolvedURL path];
			const char *resolvedPath = [resolvedString cStringUsingEncoding: NSUTF8StringEncoding];
			return Path(resolvedPath);
		}
	}

	[pool release];
	isAlias = false;
	return p;
}

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
	return s == "linux" || s == "windows" || s == "iphone";
}

Path SC_Filesystem::defaultSystemAppSupportDirectory()
{
	// "/Library/Application Support/[SuperCollider]"
	return ROOT_PATH / LIBRARY_DIR_NAME / APPLICATION_SUPPORT_DIR_NAME / getBundleName();
}

Path SC_Filesystem::defaultUserHomeDirectory()
{
	// "/Users/[username]"
	const char *home = getenv("HOME");
	return Path(home ? home : "");
}

Path SC_Filesystem::defaultUserAppSupportDirectory()
{
	// XDG support
	// see http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
	const char* home = getenv("XDG_DATA_HOME");
	if (home)
		return Path(home) / SC_FOLDERNAME_APPLICATION_NAME;

	const Path& p = defaultUserHomeDirectory();
	// "/Users/[username]/Library/Application Support/[SuperCollider]"
	return p.empty() ? p : p / LIBRARY_DIR_NAME / APPLICATION_SUPPORT_DIR_NAME / getBundleName();
}

Path SC_Filesystem::defaultUserConfigDirectory()
{
	// XDG support
	// see http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
	const char* xdgConfigHome = getenv("XDG_CONFIG_HOME");

	if (xdgConfigHome)
		return Path(xdgConfigHome) / SC_FOLDERNAME_APPLICATION_NAME;
	else
		return defaultUserAppSupportDirectory();
}

Path SC_Filesystem::defaultResourceDirectory()
{
	char pathBuf[PATH_MAX];
	SC_StandAloneInfo::GetResourceDir(pathBuf, PATH_MAX);
	return Path(pathBuf);
}

//============= STATIC FUNCTIONS =============//

const char* getBundleName()
{
	CFBundleRef mainBundle;
	mainBundle = CFBundleGetMainBundle();
	if (mainBundle) {
		CFDictionaryRef dictRef = CFBundleGetInfoDictionary(mainBundle);
		CFStringRef strRef;
		strRef = (CFStringRef)CFDictionaryGetValue(dictRef, CFSTR("CFBundleName"));
		if (strRef) {
			const char *bundleName = CFStringGetCStringPtr(strRef, CFStringGetSystemEncoding());
			if (bundleName) {
				return bundleName;
			}
		}
	}
	return SC_FOLDERNAME_APPLICATION_NAME;
}

#endif // defined(__APPLE__) && !defined(SC_IPHONE)
