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
# define MAXPATHLEN _MAX_PATH
# include "win32_utils.h"
#else
# include <sys/param.h>
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
	sc_GetResourceDirectoryFromAppDirectory(pathBuf, length)
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
		
		CFStringRef stringToFind = CFSTR("scsynth");
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
  strcat(path, "/");
#else
  strcat(path, "\\");
#endif
  strcat(path, component);
}

void sc_ResolveIfAlias(const char *path, char *returnPath, int length) 
{
#ifdef SC_DARWIN
	FSRef dirRef;
	OSStatus osStatusErr = FSPathMakeRef ((const UInt8 *) path, &dirRef, NULL);
	if ( !osStatusErr ) {
		Boolean isFolder;
		Boolean wasAliased;
		OSErr err = FSResolveAliasFile (&dirRef, true, &isFolder, &wasAliased);
		if ( !err && wasAliased && isFolder ) {
			UInt8 resolvedPath[MAXPATHLEN];
			osStatusErr = FSRefMakePath (&dirRef, resolvedPath, length);
			if ( !osStatusErr ) {
				strncpy(returnPath, (char *) resolvedPath, length);
				return;
			}
		}
	}
#endif
	strcpy(returnPath, path);
	return;
}

bool sc_DirectoryAliasExists(char *path)
{
#ifdef SC_DARWIN
	FSRef dirRef;
	OSStatus osStatusErr = FSPathMakeRef ( (const UInt8 *) path, &dirRef, NULL );
	if ( !osStatusErr ) {
		Boolean isFolder;
		Boolean wasAliased;
		OSErr err = FSResolveAliasFile ( &dirRef, true, &isFolder, &wasAliased );
		if ( !err && wasAliased && isFolder ) {
			return true;
		}
	}
#endif
	return false;
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
