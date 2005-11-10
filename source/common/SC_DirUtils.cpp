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
# include "win32_utils.h"
#else
# include <sys/param.h>
# include <sys/types.h>
# include <sys/stat.h>
#endif

#ifdef SC_DARWIN
# include <CoreFoundation/CFString.h>
# include <CoreFoundation/CFBundle.h>
# include <CoreServices/CoreServices.h>
//# include <CoreServices/Aliases.h>
#endif

//#include "SC_AllocPool.h"
#include <stdexcept>
#include "SC_DirUtils.h"

void sc_GetResourceDirectoryFromAppDirectory(char* pathBuf, int length)
{
	getcwd(pathBuf, length);
}

#ifndef SC_DARWIN

void sc_GetResourceDirectory(char* pathBuf, int length)
{
	sc_GetResourceDirectoryFromAppDirectory(pathBuf, length);
}

#else	// running on OS X

void sc_GetResourceDirectory(char* pathBuf, int length)
{
	static bool haveCheckedBundleStatus = false;
	static char dirPath[MAXPATHLEN];
	
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
				MAXPATHLEN,
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
#endif	// ndef SC_DARWIN

// Add a component to a path.

void sc_AppendToPath(char *path, const char *component)
{
#ifndef SC_WIN32
  strncat(path, "/", MAXPATHLEN);
#else
  strncat(path, "\\", MAXPATHLEN);
#endif
  strncat(path, component, MAXPATHLEN);
}

void sc_ResolveIfAlias(const char *path, char *returnPath, bool &isAlias, int length) 
{
	isAlias = false;
#ifdef SC_DARWIN
	FSRef dirRef;
	OSStatus osStatusErr = FSPathMakeRef ((const UInt8 *) path, &dirRef, NULL);
	if ( !osStatusErr ) {
		Boolean isFolder;
		Boolean wasAliased;
		OSErr err = FSResolveAliasFile (&dirRef, true, &isFolder, &wasAliased);
		isAlias = wasAliased;
		if ( !err && wasAliased ) {
			UInt8 resolvedPath[MAXPATHLEN];
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

// Returns TRUE iff 'name' is to be ignored during compilation.

static bool sc_IsNonHostPlatformDir(const char *name)
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

static bool sc_SkipDirectory(const char *name)
{
  return ((strcmp(name, ".") == 0) || 
	  (strcmp(name, "..") == 0) ||
	  (strcasecmp(name, "help") == 0) ||
	  (strcasecmp(name, "test") == 0) ||
	  (strcasecmp(name, "_darcs") == 0) ||
	  sc_IsNonHostPlatformDir(name));
}


bool sc_ReadDir(DIR *dir, char* dirname, char *path, bool &skipEntry)
{
	struct dirent *rawDirEntry = readdir(dir);
	if (!rawDirEntry) 
		return false;
	if (sc_SkipDirectory(rawDirEntry->d_name))
	{
		skipEntry = true;
		return true;
	}
	
	// construct path from dir entry
	char *rawPath = (char*)malloc(strlen(dirname) + strlen((char *) rawDirEntry->d_name) + 2);
	strcpy(rawPath, dirname);
	strcat(rawPath, "/");
	strcat(rawPath, (char *) rawDirEntry->d_name);
	
	// resolve path
	bool isAlias = false;
	sc_ResolveIfAlias(rawPath, path, isAlias, MAXPATHLEN);
	return true;
}
	
// Returns TRUE iff dirname is an existing directory

bool sc_DirectoryExists(const char *dirname) 
{
#ifdef SC_WIN32
  return false;
#else
  struct stat buf;
  int err = stat(dirname, &buf);
  return (err == 0) && (buf.st_mode & S_IFDIR);
#endif
}

// Support for Extensions

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
#else
			"/usr/local/share/SuperCollider/Extensions",
#endif
			size);
}

// Get the System level 'Extensions' directory.

void sc_GetUserExtensionDirectory(char *str, int size)
{
	char home[MAXPATHLEN];
	sc_GetUserHomeDirectory(home, MAXPATHLEN);
	
	snprintf(str, 
			 size, 
#ifdef SC_DARWIN
			 "%s/Library/Application Support/SuperCollider/Extensions",
#else
			 "%s/share/SuperCollider/Extensions",
#endif
			 home);
}
