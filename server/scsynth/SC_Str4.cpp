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


#include "SC_Str4.h"

void str4cpy(int32* dst, const char* src) {
    char* cdst0 = (char*)(dst);
    char* cdst = cdst0;
    while (*src) {
        *cdst++ = *src++;
    }
    int charlen = cdst - cdst0;
    int pad = 4 - (charlen & 3);
    for (int i = 0; i < pad; ++i) {
        *cdst++ = 0;
    }
}

void mem4cpy(int32* dst, const char* src, int charlen) {
    dst[str4len(charlen) - 1] = 0;
    char* cdst0 = (char*)(dst);
    char* cdst = cdst0;
    for (int i = 0; i < charlen; ++i) {
        *cdst++ = *src++;
    }
}
