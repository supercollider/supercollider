/*
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 *  USA
 *
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef SC_WIN32
# include <direct.h>
# include "SC_Win32Utils.h"
#else
# include <dirent.h>
# include <glob.h>
# include <sys/param.h>
# include <sys/stat.h>
# include <sys/types.h>
#endif

#ifdef SC_DARWIN
# include <CoreFoundation/CFString.h>
# include <CoreFoundation/CFBundle.h>
# include <CoreServices/CoreServices.h>
//# include <CoreServices/Aliases.h>
#endif

#include <stdexcept>
#include "SC_DirUtils.h"

// Add a component to a path.

void sc_AppendToPath(char *path, const char *component)
{
#if defined(SC_WIN32)
	strncat(path, "\\", PATH_MAX);
#else
	strncat(path, "/", PATH_MAX);
#endif
	strncat(path, component, PATH_MAX);
}


// Returns TRUE iff dirname is an existing directory.

bool sc_DirectoryExists(const char *dirname) 
{
#if defined(SC_WIN32)
	DWORD attr = GetFileAttributes(dirname);
	return ((attr != INVALID_FILE_ATTRIBUTES) &&
			(attr & FILE_ATTRIBUTE_DIRECTORY));
#else
	struct stat buf;
	return ((stat(dirname, &buf) == 0) &&
			S_ISDIR(buf.st_mode));
#endif
}


bool sc_IsNonHostPlatformDir(const char *name)
{
#if defined(SC_DARWIN)
	char a[] = "linux", b[] = "windows";
#elif defined(SC_LINUX)
	char a[] = "osx", b[] = "windows";
#elif defined(SC_WIN32)
	char a[] = "osx", b[] = "linux";
#endif
	return ((strcmp(name, a) == 0) ||
			(strcmp(name, b) == 0));
}


// Returns TRUE iff 'name' is special directory '.' or '..'

inline static bool sc_IsSpecialDirectory(const char* name)
{
	return (strcmp(name, ".") == 0) || (strcmp(name, "..")  == 0);
}

// Returns TRUE iff 'name' is to be ignored during compilation.

bool sc_SkipDirectory(const char *name)
{
	return ((strcasecmp(name, "help") == 0) ||
			(strcasecmp(name, "test") == 0) ||
			(strcasecmp(name, "_darcs") == 0) ||
			sc_IsNonHostPlatformDir(name));
}


void sc_ResolveIfAlias(const char *path, char *returnPath, bool &isAlias, int length) 
{
	isAlias = false;
#if defined(SC_DARWIN)
	FSRef dirRef;
	OSStatus osStatusErr = FSPathMakeRef ((const UInt8 *) path, &dirRef, NULL);
	if ( !osStatusErr ) {
		Boolean isFolder;
		Boolean wasAliased;
		OSErr err = FSResolveAliasFile (&dirRef, true, &isFolder, &wasAliased);
		isAlias = wasAliased;
		if ( !err && wasAliased ) {
			UInt8 resolvedPath[PATH_MAX];
			osStatusErr = FSRefMakePath (&dirRef, resolvedPath, length);
			if ( !osStatusErr ) {
				strncpy(returnPath, (char *) resolvedPath, length);
//				resolvedPath[length] = 0;
				return;
			}
		}
	}
#endif
	strcpy(returnPath, path);
	return;
}

// Support for Bundles

#ifndef SC_DARWIN

void sc_GetResourceDirectory(char* pathBuf, int length)
{
	sc_GetResourceDirectoryFromAppDirectory(pathBuf, length);
}

#else	// running on OS X

void sc_GetResourceDirectory(char* pathBuf, int length)
{
	static bool haveCheckedBundleStatus = false;
	static char dirPath[PATH_MAX];
	
	CFStringEncoding encoding = kCFStringEncodingASCII;
	
	if ( !haveCheckedBundleStatus )
	{
		haveCheckedBundleStatus = true;
		
		// If scsynth is in the bundle, we'll use that, and set our home
		// path to the bundle's Resources directory.
		
		CFStringRef stringToFind = CFSTR("SCClassLibrary");
		CFURLRef enablerURL = CFBundleCopyResourceURL (
			CFBundleGetMainBundle(),
			stringToFind,
			NULL,
			NULL
		);
		
		if ( enablerURL )
		{
			// You'd think we could get an absolute path to the Resources directory. But
			// we can't, we can only get a relative path, or an absolute path to a
			// specific resource. Since we don't know the application name, we get the
			// latter, and then hack off the resource name.
			
			CFStringRef rawPath = CFURLCopyFileSystemPath(enablerURL, kCFURLPOSIXPathStyle);

			CFRange discardRange = CFStringFind (
			   CFURLCopyFileSystemPath(enablerURL, kCFURLPOSIXPathStyle),
			   stringToFind,
			   0
			);
			
			CFRange validRange;
			validRange.location = 0;
			validRange.length = discardRange.location - 1;
			
			CFStringRef dirPathCFString = CFStringCreateWithSubstring (
				kCFAllocatorDefault,
				rawPath,
				validRange
			);
			
			CFStringGetCString (
				dirPathCFString,
				dirPath,
				PATH_MAX,
				encoding
			);
		}
		
		else
		{
			sc_GetResourceDirectoryFromAppDirectory(dirPath, length);
		};
	}
	
	// Copy the static variable to the caller's buffer.
	strncpy(pathBuf, dirPath, length);	
}
#endif	// ifndef SC_DARWIN

void sc_GetResourceDirectoryFromAppDirectory(char* pathBuf, int length)
{
	getcwd(pathBuf, length);
}

// Support for Extensions

// Get the user home directory.

void sc_GetUserHomeDirectory(char *str, int size)
{
#ifndef SC_WIN32
	char *home = getenv("HOME");
	strncpy(str, home, size);
#else
	win32_GetHomeFolder(str,size);
#endif
}


// Get the System level 'Extensions' directory.

void sc_GetSystemExtensionDirectory(char *str, int size)
{
	strncpy(str,
#if defined(SC_DATA_DIR)
			SC_DATA_DIR "/Extensions",
#elif defined(SC_DARWIN)
			"/Library/Application Support/SuperCollider/Extensions",
#elif defined(SC_WIN32)
			"C:\\SuperCollider\\Extensions",
#else
			"/usr/local/share/SuperCollider/Extensions",
#endif
			size);
}


// Get the System level 'Extensions' directory.

void sc_GetUserExtensionDirectory(char *str, int size)
{
	char home[PATH_MAX];
	sc_GetUserHomeDirectory(home, PATH_MAX);
	
	snprintf(str, 
			 size, 
#ifdef SC_DARWIN
			 "%s/Library/Application Support/SuperCollider/Extensions",
#elif defined(SC_WIN32)
			"%s\\SuperCollider\\Extensions",
#else
			 "%s/share/SuperCollider/Extensions",
#endif
			 home);
}


// Directory access

struct SC_DirHandle
{
#ifdef SC_WIN32
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

#ifdef SC_WIN32
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
#ifdef SC_WIN32
	::FindClose(dir->mHandle);
#else
	closedir(dir->mHandle);
#endif
	delete dir;
}

bool sc_ReadDir(SC_DirHandle* dir, char* dirname, char* path, bool& skipEntry)
{
#ifdef SC_WIN32
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
	sc_AppendToPath(entrypathname, dir->mEntry.cFileName);

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
	sc_AppendToPath(entrypathname, dir->mEntry->d_name);

	// resolve path
	bool isAlias = false;
	sc_ResolveIfAlias(entrypathname, path, isAlias, PATH_MAX);

	return true;
#endif
}


// Globbing

struct SC_GlobHandle
{
#ifdef SC_WIN32
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

#ifdef SC_WIN32
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
#ifdef SC_DARWIN
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
#ifdef SC_WIN32
	::FindClose(glob->mHandle);
#else
	globfree(&glob->mHandle);
#endif
	delete glob;
}

const char* sc_GlobNext(SC_GlobHandle* glob)
{
#ifdef SC_WIN32
	if (glob->mAtEnd)
		return 0;
	strncpy(glob->mEntryPath, glob->mFolder, PATH_MAX);
	sc_AppendToPath(glob->mEntryPath, glob->mEntry.cFileName);
	if (!::FindNextFile(glob->mHandle, &glob->mEntry))
		glob->mAtEnd = true;
	return glob->mEntryPath;
#else
	if (glob->mEntry >= glob->mHandle.gl_pathc)
		return 0;
	return glob->mHandle.gl_pathv[glob->mEntry++];
#endif
}
