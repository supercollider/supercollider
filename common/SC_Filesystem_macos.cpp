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

#    include "SC_Filesystem.hpp"

// boost
#    include <boost/filesystem/path.hpp> // path, parent_path()
#    include <boost/filesystem/operations.hpp> // canonical, current_path()

// system includes
#    include <Foundation/NSAutoreleasePool.h>
#    include <Foundation/NSFileManager.h>
#    include <Foundation/NSURL.h>
#    include <CoreFoundation/CFBundle.h>
#    include <CoreFoundation/CFString.h>

#    include <glob.h> // ::glob, glob_t
#    include <mach-o/dyld.h> // _NSGetExecutablePath

#    include <algorithm> // std::mismatch

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

Path SC_Filesystem::resolveIfAlias(const Path& p, bool& isAlias) {
    NSString* nsPath = [NSString stringWithCString:p.c_str() encoding:NSUTF8StringEncoding];
    BOOL isDirectory;

    // does the file exist? If not just copy and bail
    if ([[NSFileManager defaultManager] fileExistsAtPath:nsPath isDirectory:&isDirectory]) {
        NSURL* aliasURL = [NSURL fileURLWithPath:nsPath isDirectory:isDirectory];
        NSError* error = nil;

        // Previously, used bookmarkDataWithContentsOfURL. But for some reason, the call would hang
        // endlessly on certain files (e.g. /dev/auditsessions with mode crw-r--r--). So, we do the
        // less reliable thing and try to resolve it forcefully. If it is an alias, the resulting
        // path will be different (not always true, see below). See issue #4131.
        NSURL* resolvedURL = [NSURL
            URLByResolvingAliasFileAtURL:aliasURL
                                 options:NSURLBookmarkResolutionWithoutMounting | NSURLBookmarkResolutionWithoutUI
                                   error:&error];

        // If there was an error, it was a broken alias. Otherwise no error even if it wasn't an alias
        // to begin with; even if it doesn't exist (i.e. was destroyed between `fileExistsAtPath` and
        // `URLByResolvingAliasFileAtURL`)!
        if (error != nil) {
            isAlias = true;
            return Path();
        }

        const char* resolvedNsPath = [[resolvedURL path] cStringUsingEncoding:NSUTF8StringEncoding];

        // Unlikely, but possible
        if (resolvedNsPath == nullptr) {
            assert(false && "Resolved path could not be converted to C string");
            isAlias = true;
            return Path();
        }

        // #4252 - URLByResolvingAliasFileAtURL will remove last trailing slash ('/Users/' ->
        // '/Users', '/Users///' -> '/Users//'). Protect against this case.
        auto* pEnd = p.c_str() + p.size();
        const auto* mismatchPos = std::mismatch(p.c_str(), pEnd, resolvedNsPath).first;
        // note that p.size() >= 1 because empty string would fail 'fileExistsAtPath'
        if (mismatchPos == pEnd || (mismatchPos == pEnd - 1 && *mismatchPos == '/')) {
            isAlias = false;
            return p;
        } else {
            isAlias = true;
            return Path(resolvedNsPath);
        }
    }

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

    const int flags = GLOB_MARK | GLOB_TILDE | GLOB_QUOTE;
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
    return s == "linux" || s == "windows" || s == "iphone";
}

Path SC_Filesystem::defaultSystemAppSupportDirectory() {
    // "/Library/Application Support/[SuperCollider]"
    return ROOT_PATH / LIBRARY_DIR_NAME / APPLICATION_SUPPORT_DIR_NAME / getBundleName();
}

Path SC_Filesystem::defaultUserHomeDirectory() {
    // "/Users/[username]"
    const char* home = getenv("HOME");
    return Path(home ? home : "");
}

Path SC_Filesystem::defaultUserAppSupportDirectory() {
    // XDG support
    // see http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
    const char* home = getenv("XDG_DATA_HOME");
    if (home)
        return Path(home) / SC_FOLDERNAME_APPLICATION_NAME;

    const Path& p = defaultUserHomeDirectory();
    // "/Users/[username]/Library/Application Support/[SuperCollider]"
    return p.empty() ? p : p / LIBRARY_DIR_NAME / APPLICATION_SUPPORT_DIR_NAME / getBundleName();
}

Path SC_Filesystem::defaultUserConfigDirectory() {
    // XDG support
    // see http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
    const char* xdgConfigHome = getenv("XDG_CONFIG_HOME");

    if (xdgConfigHome)
        return Path(xdgConfigHome) / SC_FOLDERNAME_APPLICATION_NAME;
    else
        return defaultUserAppSupportDirectory();
}

Path SC_Filesystem::defaultResourceDirectory() {
    Path ret;
    CFStringEncoding encoding = kCFStringEncodingUTF8;

    CFURLRef enablerURL = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("SCClassLibrary"), NULL, NULL);
    if (!enablerURL) {
        enablerURL = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFSTR("sclang.app"), NULL, NULL);
    }
    if (enablerURL) {
        // If sclang or SuperCollider binary is run within the .app bundle,
        // this is how we find the Resources path.
        char relDir[PATH_MAX];
        CFStringRef rawUrlPath = CFURLCopyFileSystemPath(enablerURL, kCFURLPOSIXPathStyle);
        CFStringRef rawPath = CFStringCreateWithFormat(NULL, NULL, CFSTR("%@/.."), rawUrlPath);
        CFStringGetCString(rawPath, relDir, PATH_MAX, encoding);
        CFRelease(rawUrlPath);
        CFRelease(rawPath);
        CFRelease(enablerURL);
        ret = Path(relDir);
    } else {
        // when sclang is run from a symlink, the resource URL above will not be found,
        // so we need to find the path of the executable.
        uint32_t bufsize = PATH_MAX;
        char relDir[PATH_MAX];
        if (_NSGetExecutablePath(relDir, &bufsize) == 0) {
            ret = boost::filesystem::canonical(relDir); // resolve symlink
            ret = ret.parent_path();
        } else {
            // in case it failed, fall back to current directory
            ret = boost::filesystem::current_path();
        }
    }
    ret = boost::filesystem::canonical(ret); // resolve lingering symlink
    return ret;
}

//============= STATIC FUNCTIONS =============//

const char* getBundleName() {
    CFBundleRef mainBundle;
    mainBundle = CFBundleGetMainBundle();
    if (mainBundle) {
        CFDictionaryRef dictRef = CFBundleGetInfoDictionary(mainBundle);
        CFStringRef strRef;
        strRef = (CFStringRef)CFDictionaryGetValue(dictRef, CFSTR("CFBundleName"));
        if (strRef) {
            const char* bundleName = CFStringGetCStringPtr(strRef, CFStringGetSystemEncoding());
            if (bundleName) {
                return bundleName;
            }
        }
    }
    return SC_FOLDERNAME_APPLICATION_NAME;
}

#endif // defined(__APPLE__) && !defined(SC_IPHONE)
