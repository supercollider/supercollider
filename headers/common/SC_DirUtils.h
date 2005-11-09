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

#ifdef SC_WIN32
# include <stdlib>
# define MAXPATHLEN _MAX_PATH
#endif

extern "C" {
	void sc_GetResourceDirectory(char* pathBuf, int length);
	void sc_GetResourceDirectoryFromAppDirectory(char* pathBuf, int length);
	void sc_AppendToPath(char *path, const char *component);
	void sc_ResolveIfAlias(const char *path, char *returnPath, bool &isAlias, int length);
	// Returns TRUE iff dirname is an existing directory
	bool sc_DirectoryExists(const char *dirname);
	bool sc_DirectoryAliasExists(char *path);
	
	void sc_GetUserHomeDirectory(char *str, int size);
	void sc_GetSystemExtensionDirectory(char *str, int size);
	void sc_GetUserExtensionDirectory(char *str, int size);
}

#endif // SC_DIR_UTILS_H_INCLUDED
