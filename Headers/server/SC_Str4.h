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

/*  a 4 byte aligned and zero padded string allows faster string operations. */

#ifndef _SC_Str4_
#define _SC_Str4_

#include "Hash.h"
#include <stdio.h>
#include <limits.h>

#ifndef _LASTCHAR_
#define _LASTCHAR_
#if BYTE_ORDER == LITTLE_ENDIAN
const int32 kLASTCHAR = 0xFF000000;
#else
const int32 kLASTCHAR = 0x000000FF;
#endif
#endif


void str4cpy(int32 *dst, const char *src);
void mem4cpy(int32 *dst, const char *src, int charlen);

// returns the number of pad bytes to add to a string of a given length
inline int str4padbytes(int charlen)
{
    return 4 - (charlen & 3);
}

// converts length in bytes to length in words
inline int str4len(int charlen)
{
	return (charlen + 4) >> 2;
}

// returns length in words of a char *
inline int str4len(const char *src)
{
    const char *src0 = src;
    while (*src) { src++; }
    return str4len(src - src0);
}

// returns length in words of a int32 *
inline int str4len(const int32 *src)
{
    const int32 *src0 = src;
    while (*src++ & kLASTCHAR) {}
    int wordlen = src - src0;
    return wordlen;
}

// returns length in words of a int32 *
inline bool str4eq(const int32 *a, const int32 *b)
{
	while(true) {
		if (*a != *b) return false;
		if ((*a & kLASTCHAR) == 0) return true;
		a++; b++;
	}
}

// copy an int32 *
inline void str4cpy(int32 *dst, const int32 *src)
{
	int32 c;
	do {
		*dst++ = c = *src++;
	} while (c & kLASTCHAR);
}

inline int sc_atoi(const char *string)
{
	int value = 0;
	if (*string == 0) return -1;
	uint32 c;
	while ((c = *string++ - '0') <= 9) {
		value = value * 10 + c;
	}
	return value;
}


#endif
