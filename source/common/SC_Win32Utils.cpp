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

#ifdef SC_WIN32

#include <cstdio>
#include <cstring>

#include "SC_Win32Utils.h"

void win32_ReplaceCharInString(char* string, int len, char src, char dst)
{
  for( int i = 0; i < len; i ++)
    if(string[i] == src)
      string[i] = dst;
}

void win32_ExtractContainingFolder(char* folder,const char* pattern,int maxChars)
{
  strcpy(folder,pattern);
  bool backSlashFound = false;
  int pathLen = strlen(pattern);
  for( int i = pathLen-2; i >= 0; --i) {
    if( pattern[i] == '\\') {
      folder[i+1] = 0;
      backSlashFound = true;
      break;
    }
  }
  if( !backSlashFound )
    folder[0] = 0;
}

void win32_gettimeofday(timeval* tv, void*)
{
	int secBetween1601and1970 = 11644473600;
	FILETIME fileTime;
	GetSystemTimeAsFileTime(&fileTime);
	tv->tv_sec = (* (unsigned __int64 *) &fileTime / 10000000) - secBetween1601and1970;
	tv->tv_usec = ((* (unsigned __int64 *) &fileTime / 10) % 1000000) - secBetween1601and1970*1000000; 
}

void win32_GetHomeFolder(char* homeFolder, int bufLen)
{
  char homeFolder_[_MAX_PATH];
  const char *h = 0;
  if (!(h = ::getenv("home"))) 
    h = ::getenv("HOME");
 
  if (h) 
    strcpy(homeFolder_,h);
  else {
    // for Windows NT HOME might be defined as either $(HOMESHARE)/$(HOMEPATH)
    //                                         or     $(HOMEDRIVE)/$(HOMEPATH)
    h = ::getenv("HOMESHARE");
    if (!h)  
      h = ::getenv("HOMEDRIVE");
    if (h) {
      strcpy(homeFolder_,h);
      h = ::getenv("HOMEPATH");
      if (h) 
        strcat(homeFolder_,h);
    }
  }
  size_t len = strlen(homeFolder_);
  if (bufLen < len + 1)
		fprintf(stderr, "the buffer given to win32_GetHomeFolder(...) is too small\n");
  strncpy(homeFolder,homeFolder_,len);
  homeFolder[len]= 0;
}

char* win32_basename(char* path)
{ 
  int pathLen = strlen(path);
  int lastPathSepFoundPos = -1;
  int pos = 0;
  while (path[pathLen-1] == '\\' || path[pathLen-1] == '/') { 
	  path[pathLen-1]=0; pathLen--;
  }; 
  while(path[pos] != 0) {
    if (path[pos] == '\\' || path[pos] == '/') {
      lastPathSepFoundPos = pos;
    }
	pos++;
  }
  if (lastPathSepFoundPos == -1)
    return path;
  else
    return path + lastPathSepFoundPos + 1;
}

char* win32_dirname(char* path)
{
  int pathLen = strlen(path);
  int lastPathSepFoundPos = -1;
  int pos = 0;
  while (path[pathLen-1] == '\\' || path[pathLen-1] == '/') { 
	  path[pathLen-1]=0; pathLen--;
  };
  while(path[pos] != 0) {
    if (path[pos] == '\\' || path[pos] == '/') {
      lastPathSepFoundPos = pos;
    }
	pos++;
  }
  if (lastPathSepFoundPos != -1)
	path[lastPathSepFoundPos]=0;

  return path;
}

int win32_nanosleep (const struct timespec *requested_time,
		  struct timespec *remaining)
{
  DWORD milliseconds = requested_time->tv_sec * 1000
  	+ (requested_time->tv_nsec) / 1000000;
  if (requested_time->tv_nsec < 0 || requested_time->tv_nsec > 1000000) {
  	errno = EINVAL;
    return -1;
  }
  Sleep (milliseconds);
  remaining->tv_sec = 0;
  remaining->tv_nsec = 0;
  return 0;
}

#endif 
// SC_WIN32
