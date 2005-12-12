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

#ifndef SC_DIR_UTILS_H_INCLUDED
#define SC_DIR_UTILS_H_INCLUDED

#include <limits.h>

#ifdef SC_WIN32
# include <stdio.h>
# ifndef PATH_MAX
#  define PATH_MAX _MAX_PATH
# endif
# define strcasecmp stricmp
# define snprintf _snprintf
#endif

# ifndef MAXPATHLEN
#  define MAXPATHLEN PATH_MAX
# endif

// General path utilities

// Add 'component' to 'path' using the platform path separator.
void sc_AppendToPath(char *path, const char *component);

// Returns the expanded path with users home directory (also in newpath2)
char *sc_StandardizePath(const char *path, char *newpath2);

// Return TRUE iff 'dirname' is an existing directory.
bool sc_DirectoryExists(const char *dirname);

// Returns TRUE iff 'name' is a directory pertaining to another platform.
bool sc_IsNonHostPlatformDir(const char *name);

// Returns TRUE iff 'name' is to be ignored during compilation.
bool sc_SkipDirectory(const char *name);

void sc_ResolveIfAlias(const char *path, char *returnPath, bool &isAlias, int length);


// Support for Bundles

void sc_GetResourceDirectory(char* pathBuf, int length);
void sc_GetResourceDirectoryFromAppDirectory(char* pathBuf, int length);


// Support for Extensions

// Get the user home directory.
void sc_GetUserHomeDirectory(char *str, int size);

// Get the System level 'Extensions' directory.
void sc_GetSystemExtensionDirectory(char *str, int size);

// Get the System level 'Extensions' directory.
void sc_GetUserExtensionDirectory(char *str, int size);


// Directory access

// Abstract directory handle
struct SC_DirHandle;

// Open directory dirname. Return NULL on failure.
SC_DirHandle* sc_OpenDir(const char *dirname);

// Close directory dir.
void sc_CloseDir(SC_DirHandle *dir);

// Get next entry from directory 'dir' with name 'dirname' and put it into 'path'.
// Skip compilation directories iff 'skipEntry' is TRUE.
// Return TRUE iff pointing to a valid dir entry.
// Return TRUE in 'skipEntry' iff entry should be skipped.
bool sc_ReadDir(SC_DirHandle *dir, char *dirname, char *path, bool &skipEntry);


// Globbing

// Abstract glob handle
struct SC_GlobHandle;

// Create glob iterator from 'pattern'. Return NULL on failure.
SC_GlobHandle* sc_Glob(const char* pattern);

// Free glob handle.
void sc_GlobFree(SC_GlobHandle* glob);

// Return next path from glob iterator.
// Return NULL at end of stream.
const char* sc_GlobNext(SC_GlobHandle* glob);

#endif // SC_DIR_UTILS_H_INCLUDED
