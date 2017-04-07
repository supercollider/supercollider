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

/*
 * SC_Filesystem implementation for Apple.
 */
#if defined(__APPLE__) && !defined(SC_IPHONE)

#include "SC_Filesystem.hpp"

// stdlib
#include <string>

// boost
#include <boost/algorithm/string/predicate.hpp> // iequals

// system includes
#import <Foundation/Foundation.h>      // @TODO: ???
#include <CoreFoundation/CFString.h>   // CFString
#include <CoreFoundation/CFBundle.h>   // CFBundle
#include <CoreServices/CoreServices.h> // @TODO: ???

// @TODO: add debugging
using SC_Filesystem::Path;
using std::map;

//============ DIRECTORY NAMES =============//
const char* gIdeName = "none"; // @TODO: move out
const char* DOT_CONFIG = ".config";
const char* CWD = "./";
const char* SUPERCOLLIDER_DIR_NAME = "SuperCollider";
const char* DOCUMENTS_DIR_NAME = "Documents";
const char* EXTENSIONS_DIR_NAME = "Extensions";
const char* LIBRARY_DIR_NAME = "Library";
const char* APPLICATION_SUPPORT_DIR_NAME = "Application Support";
const char* SHARE_DIR_NAME = "share";
const char* DOT_LOCAL = ".local";
const Path* ROOT_PATH = Path("/");

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

Path SC_Filesystem::expandTilde(const Path& p)
{
	Path::const_iterator piter = p.begin();

	if (piter != p.end() && *piter == "~") {
		Path& ret = getUserHomeDirectory();
		while (++piter != p.end())
			ret /= *piter;
		return ret;
	} else {
		return p;
	}
}

bool SC_Filesystem::shouldNotCompileDirectory(const Path& p)
{
	const std::string& dirname = p.filename().string();
	static const std::string& idePath = std::string("scide_") + gIdeName;
	return (boost::iequals(dirname, "help") ||
					boost::iequals(dirname, "ignore") ||
					dirname == ".svn" ||
					dirname == ".git" ||
					dirname == "_darcs" ||
					dirname == idePath ||
					isNonHostPlatformDirectory(dirname));
}

Path SC_Filesystem::resolveIfAlias(const Path& p, bool& ok)
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
			NSError *resolvedURLError;
			BOOL isStale;
			NSURL *resolvedURL = [NSURL URLByResolvingBookmarkData: bookmark options: NSURLBookmarkResolutionWithoutUI relativeToURL: nil bookmarkDataIsStale: &isStale error: &resolvedURLError];
			// does it actually lead to something?
			if (isStale || resolvedURL == nullptr) {
				// Return original path.
				printf("Error: Target missing for alias at %s\n", p.c_str());
				ok = false;
				return p;
			}

			NSString *resolvedString = [resolvedURL path];
			const char *resolvedPath = [resolvedString cStringUsingEncoding: NSUTF8StringEncoding];
			ok = true;
			return Path(resolvedPath);
		}
	}

	[pool release];
	return p;
}

const char* SC_Filesystem::getBundleName()
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
	return SUPERCOLLIDER_DIR_NAME;
}

//============= DETAIL FUNCTIONS =============//

// @TODO: determine if this functionality is still needed.
// better avoid if possible; may cause issues in the future.
// we can always exclude using lang config.

// Returns `true` if the directory corresponds to another platform.
static bool isNonHostPlatformDirectory(const std::string& s)
{
	return s == "linux" || s == "windows" || s == "iphone";
}

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
			// @TODO: print error
			return false;
	}

	if (!ok) {
		// @TODO: print error
		return false;
	}

	gDirectoryMap[dn] = p;
	return true;
}

static bool getSystemAppSupportDirectory(Path& p)
{
	p = ROOT_PATH / LIBRARY_DIR_NAME / APPLICATION_SUPPORT_DIR_NAME / SC_Filesystem::getBundleName();
	return true;
}

static bool getSystemExtensionDirectory(Path& p)
{
	p = SC_Filesystem::getDirectory(SC_DirectoryName::SystemAppSupport) / EXTENSIONS_DIR_NAME;
	return true;
}

static bool getUserHomeDirectory(Path& p)
{
	const char *home = getenv("HOME");
	if (home) {
		p = Path(home);
		return true;
	} {
		// @TODO: error message
		return false;
	}
}

static bool getUserAppSupportDirectory(Path& p)
{
	// XDG support
	// see http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
	const char* xdgDataHome = getenv("XDG_DATA_HOME");
	if (xdgDataHome) {
		p = Path(xdgDataHome) / SUPERCOLLIDER_DIR_NAME;
		return true;
	} else {
		p = SC_Filesystem::getDirectory(SC_DirectoryName::UserHome)
				/ LIBRARY_DIR_NAME
				/ APPLICATION_SUPPORT_DIR_NAME
				/ SC_Filesystem::getBundleName();
		return true;
	}
}

static bool getUserExtensionDirectory(Path& p)
{
	p = SC_Filesystem::getDirectory(SC_DirectoryName::UserAppSupport) / EXTENSIONS_DIR_NAME;
	return true;
}

static bool getUserConfigDirectory(Path& p)
{
	// XDG support
	// see http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
	const char* xdgConfigHome = getenv("XDG_CONFIG_HOME");
	if (xdgConfigHome) {
		p = Path(xdgConfigHome) / SUPERCOLLIDER_DIR_NAME;
		return true;
	}

	p = SC_Filesystem::getDirectory(SC_DirectoryName::UserAppSupport);
	return true;
}

static bool getResourceDirectory(Path& p)
{
	char pathBuf[PATH_MAX];
	SC_StandAloneInfo::GetResourceDir(pathBuf, PATH_MAX);
	p = Path(pathBuf);
	return true;
}

struct SC_Filesystem::Glob
{
	glob_t mHandle;
	size_t mEntry;
};

SC_Filesystem::Glob* makeGlob(const char* pattern)
{
	Glob ret = new SC_GlobHandle::Glob;

	int flags = GLOB_MARK | GLOB_TILDE | GLOB_QUOTE;

	int err = ::glob(pattern, flags, NULL, mGlob->mHandle);
	if (err < 0) {
		delete glob;
		return 0;
	}

	glob->mEntry = 0;

	return glob;
}

void SC_Filesystem::freeGlob(Glob*)
{
	globfree(&glob->mHandle);
}

SC_Filesystem::Path globNext(Glob* glob)
{
	if (glob->mEntry >= glob->mHandle.gl_pathc)
		return Path();
	return Path(glob->mHandle.gl_pathv[glob->mEntry++]);
}

#endif // defined(__APPLE__) && !defined(SC_IPHONE)
