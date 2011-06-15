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

#ifndef _SC_Types_
#define _SC_Types_

#include <sys/types.h>

#if !defined(__cplusplus)
# include <stdbool.h>
#endif // __cplusplus

typedef int SCErr;

#ifdef _WIN32
typedef __int64	int64;
typedef unsigned __int64 uint64;
#else
typedef long long int64;
typedef unsigned long long uint64;
#endif

typedef int int32;
typedef unsigned int uint32;

typedef short int16;
typedef unsigned short uint16;

typedef signed char int8;
typedef unsigned char uint8;

typedef float float32;
typedef double float64;

typedef union {
	uint32 u;
	int32 i;
	float32 f;
} elem32;

typedef union {
	uint64 u;
	int64 i;
	float64 f;
} elem64;

const unsigned int kSCNameLen = 8;
const unsigned int kSCNameByteLen = 8 * sizeof(int32);

#ifdef __GXX_EXPERIMENTAL_CXX0X__
#define sc_typeof_cast(x) (decltype(x))
#elif defined(__GNUC__)
#define sc_typeof_cast(x) (__typeof__(x))
#else
#define sc_typeof_cast(x) /* (typeof(x)) */
#endif

#endif

