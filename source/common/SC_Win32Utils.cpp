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
#include <cstdio>
#include <cstring>

#ifdef SC_WIN32
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

#endif 
// SC_WIN32
