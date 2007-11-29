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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _SC_WIN32UTILS_H
#define _SC_WIN32UTILS_H

#ifdef SC_WIN32
#include <stdio.h>
#include <winsock2.h>
#include <pthread.h>

// wrappers for unix replacements
#define gettimeofday win32_gettimeofday
#define basename win32_basename
#define dirname win32_dirname
#define nanosleep win32_nanosleep

void win32_ReplaceCharInString(char* string, int len, char src, char dst);
void win32_ExtractContainingFolder(char* folder,const char* pattern,int maxChars);
void win32_gettimeofday(timeval* tv, void*);
void win32_GetHomeFolder(char* homeFolder, int bufLen);
char* win32_basename(char* path);
char* win32_dirname(char* path);
int win32_nanosleep (const struct timespec *requested_time, struct timespec *remaining);


#endif //SC_WIN32
#endif // SC_WIN32UTILS_H
