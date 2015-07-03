/*
 *  Copyright (c) 2005 Tim Walters. All rights reserved.
 *  Created by Tim Walters on 10/19/05.
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301
 *  USA
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdexcept>

#ifdef __APPLE__
#import <Foundation/Foundation.h>
#endif

#ifdef _WIN32
# include "SC_Win32Utils.h"
# include <windows.h>
# include <direct.h>
# include <shlobj.h>
#else
# include <unistd.h>
# include <dirent.h>
# include <glob.h>
# include <sys/param.h>
# include <sys/stat.h>
# include <sys/types.h>
#endif

#include "SC_DirUtils.h"
#if defined(__APPLE__) || defined(SC_IPHONE)
#ifndef _SC_StandAloneInfo_
# include "SC_StandAloneInfo_Darwin.h"
#endif
# include <CoreFoundation/CFString.h>
# include <CoreFoundation/CFBundle.h>
#ifndef SC_IPHONE
# include <CoreServices/CoreServices.h>
#endif
#endif

const char * gIdeName = "none";

// Add a component to a path.

void sc_AppendToPath(char *path, size_t max_size, const char *component)
{
	size_t currentLength = strlen(path);
	if (currentLength >= max_size-1)
		return;
	path[currentLength] = SC_PATH_DELIMITER[0];
	path[currentLength+1] = 0;
	++currentLength;

	char * tail = path + currentLength;
	size_t remain = max_size - currentLength;

	strncat(tail, component, remain);
}


char *sc_StandardizePath(const char *path, char *newpath2)
{
	char newpath1[MAXPATHLEN];

	newpath1[0] = '\0';
	newpath2[0] = '\0';

	size_t pathLen = strlen(path);

	if ((pathLen >= 2) && (path[0] == '~') && ((path[1] == '/') || (path[1] == '\\'))) {
		char home[PATH_MAX];
		sc_GetUserHomeDirectory(home, PATH_MAX);

		if (home[0] != 0) {
			if ((pathLen - 1 + strlen(home)) >= MAXPATHLEN) {
				return 0;
			}
			strcpy(newpath1, home);
			strcat(newpath1, path + 1);
		} else {
			if (pathLen >= MAXPATHLEN) {
				return 0;
			}
			strcpy(newpath1, path);
			newpath1[0] = '.';
		}
	} else {
		if (pathLen >= MAXPATHLEN) {
			return 0;
		}
		strcpy(newpath1, path);
	}

	bool isAlias = false;
	if(sc_ResolveIfAlias(newpath1, newpath2, isAlias, PATH_MAX)!=0) {
		strcpy(newpath2, newpath1);
	}

	return newpath2;
}


// Returns TRUE iff dirname is an existing directory.

bool sc_DirectoryExists(const char *dirname)
{
#if defined(_WIN32)
	DWORD attr = GetFileAttributes(dirname);
	return ((attr != INVALID_FILE_ATTRIBUTES) &&
			(attr & FILE_ATTRIBUTE_DIRECTORY));
#else
	struct stat buf;
	return ((stat(dirname, &buf) == 0) &&
			S_ISDIR(buf.st_mode));
#endif
}

bool sc_IsSymlink(const char* path)
{
#if defined(_WIN32)
	// FIXME
	return false;
#else
	struct stat buf;

	return ((stat(path, &buf) == 0) &&
			S_ISLNK(buf.st_mode));
#endif
}

bool sc_IsNonHostPlatformDir(const char *name)
{
#if defined(SC_IPHONE)
	const char a[] = "linux", b[] = "windows", c[]="osx";
#elif defined(__APPLE__)
	const char a[] = "linux", b[] = "windows", c[]="iphone";
#elif defined(__linux__)
	const char a[] = "osx", b[] = "windows", c[]="iphone";
#elif defined(__FreeBSD__) || defined(__OpenBSD__) || defined(__NetBSD__)
	const char a[] = "osx", b[] = "windows", c[]="iphone";
#elif defined(_WIN32)
	const char a[] = "osx", b[] = "linux", c[]="iphone";
#endif
	return ((strcmp(name, a) == 0) ||
			(strcmp(name, b) == 0) ||
			(strcmp(name, c) == 0));
}


// Returns TRUE iff 'name' is special directory '.' or '..'

inline static bool sc_IsSpecialDirectory(const char* name)
{
	return (strcmp(name, ".") == 0) || (strcmp(name, "..")  == 0);
}

// Returns TRUE iff 'name' is to be ignored during compilation.

bool sc_SkipDirectory(const char *name)
{
	return (stringCaseCompare(name, "help")   ||
			stringCaseCompare(name, "ignore") ||
			(strcmp(name, ".svn") == 0) ||
			(strcmp(name, ".git") == 0) ||
			(strcmp(name, "_darcs") == 0) ||
			((strncmp(name, "scide_", 6) == 0) && (strcmp(name+6, gIdeName) != 0)) ||
			sc_IsNonHostPlatformDir(name));
}


int sc_ResolveIfAlias(const char *path, char *returnPath, bool &isAlias, int length)
{
	isAlias = false;
#if defined(__APPLE__) && !defined(SC_IPHONE)
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	NSString *nsstringPath = [NSString stringWithCString: path encoding: NSUTF8StringEncoding];
	BOOL isDirectory;
	// does the file exist? If not just copy and bail
	if([[NSFileManager defaultManager] fileExistsAtPath: nsstringPath isDirectory: &isDirectory]) {
		NSError *error;

		NSData *bookmark = [NSURL bookmarkDataWithContentsOfURL: [NSURL fileURLWithPath:nsstringPath isDirectory: isDirectory] error: &error];

		// is it an alias? If not just copy and bail
		if(bookmark) {
			NSError *resolvedURLError;
			BOOL isStale;
			NSURL *resolvedURL = [NSURL URLByResolvingBookmarkData: bookmark options: NSURLBookmarkResolutionWithoutUI relativeToURL: nil bookmarkDataIsStale: &isStale error: &resolvedURLError];
			// does it actually lead to something?
			if(isStale || (resolvedURL == NULL) ) {
				printf("Error: Target missing for alias at %s\n", path);
				return -1;
			}

			NSString *resolvedString = [resolvedURL path];

			const char *resolvedPath = [resolvedString cStringUsingEncoding: NSUTF8StringEncoding];
			isAlias = true;
			strncpy(returnPath, resolvedPath, length);
			return 0;
		}
	}
    [pool release];

#endif
	strcpy(returnPath, path);
	return 0;
}

// Support for Bundles

#if defined(__APPLE__) && !defined(SC_IPHONE)	// running on OS X

// Support for stand-alone applications

bool sc_IsStandAlone()
{
	return SC_StandAloneInfo::IsStandAlone();
}

void sc_GetResourceDirectory(char* pathBuf, int length)
{
	SC_StandAloneInfo::GetResourceDir(pathBuf, length);
}


void sc_AppendBundleName(char *str, int size)
{
	CFBundleRef mainBundle;
	mainBundle = CFBundleGetMainBundle();
	if(mainBundle){
		CFDictionaryRef dictRef = CFBundleGetInfoDictionary(mainBundle);
		CFStringRef strRef;
		strRef = (CFStringRef)CFDictionaryGetValue(dictRef, CFSTR("CFBundleName"));
		if(strRef){
			const char *bundleName = CFStringGetCStringPtr(strRef, CFStringGetSystemEncoding());
			if(bundleName) {
				sc_AppendToPath(str, size, bundleName);
				return;
			}
		}
	}
	sc_AppendToPath(str, size, "SuperCollider");
}

#elif defined(SC_IPHONE)

bool sc_IsStandAlone()
{
	return false;
}

void sc_GetResourceDirectory(char* pathBuf, int length)
{
	sc_GetUserAppSupportDirectory(pathBuf, length);
}

#elif defined(__unix__)

bool sc_IsStandAlone()
{
	return false;
}

void sc_GetResourceDirectory(char* pathBuf, int length)
{
#ifdef SC_DATA_DIR
	strncpy(pathBuf, SC_DATA_DIR, length);
#else
	strncpy(pathBuf, "/usr/share/SuperCollider", length);
#endif
}

#elif defined(_WIN32)

bool sc_IsStandAlone()
{
	return false;
}

void sc_GetResourceDirectory(char* dest, int length)
{
	char buf[PATH_MAX];
	GetModuleFileName( NULL, buf, length );
	char *path = win32_dirname(buf);
	strcpy(dest, path);
}

#else

bool sc_IsStandAlone()
{
	return false;
}

static void sc_GetResourceDirectoryFromAppDirectory(char* pathBuf, int length)
{
	char * result = getcwd(pathBuf, length);
	if (result != pathBuf)
		throw std::runtime_error("cannot get current working directory");
}


void sc_GetResourceDirectory(char* pathBuf, int length)
{
	return sc_GetResourceDirectoryFromAppDirectory(pathBuf, length);
}

#endif

// Support for Extensions

// Get the user home directory.

void sc_GetUserHomeDirectory(char *str, int size)
{
#ifndef _WIN32
	const char *home = getenv("HOME");
	if(home!=NULL){
		strncpy(str, home, size);
	}else{
		// cwd is not the user home directory; but this is better than leaving mem uninitialised.
		strcpy(str, "./");
	}
#else
	win32_GetKnownFolderPath(CSIDL_PROFILE, str, size);
#endif
}


// Get the System level data directory.

void sc_GetSystemAppSupportDirectory(char *str, int size)
{
#ifdef _WIN32
	win32_GetKnownFolderPath(CSIDL_COMMON_APPDATA, str, size);
	sc_AppendToPath(str, size, "SuperCollider");
#else

	strncpy(str,
#if defined(SC_DATA_DIR)
			SC_DATA_DIR,
#elif defined(SC_IPHONE)
			"/",
#elif defined(__APPLE__)
			"/Library/Application Support",
#else
			"/usr/local/share/SuperCollider",
#endif
			size);

#if defined(__APPLE__)
	// Get the main bundle name for the app from the enclosed Info.plist 
	sc_AppendBundleName(str, size);
#endif

#endif
}


// Get the User level data directory.

void sc_GetUserAppSupportDirectory(char *str, int size)
{
	// XDG support according to http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
	const char * xdg_data_home = getenv("XDG_DATA_HOME");
	if (xdg_data_home) {
		strncpy(str, xdg_data_home, size);
		sc_AppendToPath(str, size, "SuperCollider");
		return;
	}

#if defined(_WIN32)
	win32_GetKnownFolderPath(CSIDL_LOCAL_APPDATA, str, size);
	sc_AppendToPath(str, size, "SuperCollider");
#else

	sc_GetUserHomeDirectory(str, size);

#if defined(SC_IPHONE)
	sc_AppendToPath(str, size, "Documents");
#elif defined(__APPLE__)
	// Get the main bundle name for the app
	sc_AppendToPath(str, size, "Library/Application Support");
	sc_AppendBundleName(str, size);
#else
	sc_AppendToPath(str, size, ".local/share/SuperCollider");
#endif

#endif
}


// Get the System level 'Extensions' directory.

void sc_GetSystemExtensionDirectory(char *str, int size)
{
	sc_GetSystemAppSupportDirectory(str, size);
	sc_AppendToPath(str, size, "Extensions");
}


// Get the System level 'Extensions' directory.

void sc_GetUserExtensionDirectory(char *str, int size)
{
	sc_GetUserAppSupportDirectory(str, size);
	sc_AppendToPath(str, size, "Extensions");
}

// Get the directory for the configuration files.
void sc_GetUserConfigDirectory(char *str, int size)
{
	// XDG support according to http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html
	const char * xdg_config_home = getenv("XDG_CONFIG_HOME");
	if (xdg_config_home) {
		strncpy(str, xdg_config_home, size);
		sc_AppendToPath(str, size, "SuperCollider");
		return;
	}

#if defined(__linux__) || defined(__freebsd__)
	sc_GetUserHomeDirectory(str, size);
	sc_AppendToPath(str, size, ".config/SuperCollider");
#else
	sc_GetUserAppSupportDirectory(str, size);
#endif
}


// Directory access

struct SC_DirHandle
{
#ifdef _WIN32
	HANDLE mHandle;
	WIN32_FIND_DATA mEntry;
	bool mAtEnd;
#else
	DIR* mHandle;
	struct dirent* mEntry;
#endif
};

SC_DirHandle* sc_OpenDir(const char* dirname)
{
	SC_DirHandle* dir = new SC_DirHandle;
	memset(dir, 0, sizeof(SC_DirHandle));

#ifdef _WIN32
	char allInDir[PATH_MAX];
	snprintf(allInDir, PATH_MAX, "%s\\*.*", dirname);

	dir->mHandle = ::FindFirstFile(allInDir, &dir->mEntry);
	if (dir->mHandle == INVALID_HANDLE_VALUE) {
		delete dir;
		return 0;
	}

	dir->mAtEnd = false;
#else
	dir->mHandle = opendir(dirname);
	if (!dir->mHandle) {
		delete dir;
		return 0;
	}
#endif

	return dir;
}

void sc_CloseDir(SC_DirHandle* dir)
{
#ifdef _WIN32
	::FindClose(dir->mHandle);
#else
	closedir(dir->mHandle);
#endif
	delete dir;
}

bool sc_ReadDir(SC_DirHandle* dir, const char* dirname, char* path, bool& skipEntry)
{
#ifdef _WIN32
	bool success = true;

	if (dir->mAtEnd)
		return false;

	const char* entry = dir->mEntry.cFileName;

	if (sc_IsSpecialDirectory(entry) || (skipEntry && sc_SkipDirectory(entry))) {
		skipEntry = true;
		success = true;
	} else {
		skipEntry = false;
		success = true;
	}

    char entrypathname[PATH_MAX];
	strncpy(entrypathname, dirname, PATH_MAX);
	sc_AppendToPath(entrypathname, PATH_MAX, dir->mEntry.cFileName);

	bool isAlias = false;
	sc_ResolveIfAlias(entrypathname, path, isAlias, PATH_MAX);

	if (!::FindNextFile(dir->mHandle, &dir->mEntry)) {
		dir->mAtEnd = true;
	}

	return true;
#else
	if (!dir->mHandle)
		return false;

	dir->mEntry = readdir(dir->mHandle);
	if (!dir->mEntry)
		return false;

	const char* entry = dir->mEntry->d_name;

	if (sc_IsSpecialDirectory(entry) || (skipEntry && sc_SkipDirectory(entry))) {
		skipEntry = true;
		return true;
	} else {
		skipEntry = false;
	}

	// construct path from dir entry
	char entrypathname[PATH_MAX];
	strncpy(entrypathname, dirname, PATH_MAX);
	sc_AppendToPath(entrypathname, PATH_MAX, dir->mEntry->d_name);

	// resolve path
	bool isAlias = false;
	if (sc_ResolveIfAlias(entrypathname, path, isAlias, strlen(entrypathname))<0)
	{
		skipEntry = true;
	}

	return true;
#endif
}


// Globbing

struct SC_GlobHandle
{
#ifdef _WIN32
	HANDLE mHandle;
	char mFolder[PATH_MAX];
	WIN32_FIND_DATA mEntry;
	char mEntryPath[PATH_MAX];
	bool mAtEnd;
#else
	glob_t mHandle;
	size_t mEntry;
#endif
};

SC_GlobHandle* sc_Glob(const char* pattern)
{
	SC_GlobHandle* glob = new SC_GlobHandle;

#ifdef _WIN32
	char patternWin[1024];

	strncpy(patternWin, pattern, 1024);
	patternWin[1023] = 0;
	win32_ReplaceCharInString(patternWin, 1024, '/', '\\');

	win32_ExtractContainingFolder(glob->mFolder, patternWin, PATH_MAX);

	glob->mHandle = ::FindFirstFile(patternWin, &glob->mEntry);
	if (glob->mHandle == INVALID_HANDLE_VALUE) {
		delete glob;
		return 0;
	}

	glob->mAtEnd = false;
#else
	int flags = GLOB_MARK | GLOB_TILDE;
#ifdef __APPLE__
	flags |= GLOB_QUOTE;
#endif

	int err = ::glob(pattern, flags, NULL, &glob->mHandle);
	if (err < 0) {
		delete glob;
		return 0;
	}

	glob->mEntry = 0;
#endif

	return glob;
}

void sc_GlobFree(SC_GlobHandle* glob)
{
#ifdef _WIN32
	::FindClose(glob->mHandle);
#else
	globfree(&glob->mHandle);
#endif
	delete glob;
}

const char* sc_GlobNext(SC_GlobHandle* glob)
{
#ifdef _WIN32
	if (glob->mAtEnd)
		return 0;
	strncpy(glob->mEntryPath, glob->mFolder, PATH_MAX);
	sc_AppendToPath(glob->mEntryPath, PATH_MAX, glob->mEntry.cFileName);
	if (!::FindNextFile(glob->mHandle, &glob->mEntry))
		glob->mAtEnd = true;
	return glob->mEntryPath;
#else
	if (glob->mEntry >= glob->mHandle.gl_pathc)
		return 0;
	return glob->mHandle.gl_pathv[glob->mEntry++];
#endif
}
