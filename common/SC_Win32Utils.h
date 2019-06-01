/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#pragma once

#ifdef _WIN32

#    ifdef IN
#        define SC_IN IN
#        undef IN
#    endif

#    ifdef OUT
#        define SC_OUT OUT
#        undef OUT
#    endif

#    include <stdio.h>
#    include <winsock2.h>
#    undef IN
#    undef OUT
#    ifdef SC_IN
#        define IN SC_IN
#    endif

#    ifdef SC_OUT
#        define OUT SC_OUT
#    endif

// wrappers for unix replacements
#    define basename win32_basename
#    define dirname win32_dirname
#    define pipe win32_pipe

#    if _MSC_VER
typedef int pid_t;
#        define snprintf _snprintf
#    endif

void win32_ReplaceCharInString(char* string, int len, char src, char dst);
// Finds the parent folder of a specified path pattern (including trailing slash)
void win32_ExtractContainingFolder(char* folder, const char* pattern, int maxChars);
void win32_GetKnownFolderPath(int folderId, char* dest, int size);
void win32_synctimes();
char* win32_basename(char* path);
char* win32_dirname(char* path);
int win32_pipe(int handles[2]);
int win32_piperead(int s, char* buf, int len);
int win32_pipewrite(int s, char* buf, int len);

// alloca
#    include <malloc.h>
#    ifndef alloca // MinGW has alloca defined in malloc.h, MSVC not
#        define alloca _alloca
#    endif

#endif //_WIN32
