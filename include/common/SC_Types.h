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

#include <stddef.h>
#include <stdint.h>

#if __cplusplus
#    define SC_INLINE inline
#else
#    define SC_INLINE static inline
#endif

#ifdef __cplusplus
// unfortunately, we have to use 'bool' for source compatibility with
// existing  C++ plugins, but we make sure that it is indeed 1 byte long.
typedef bool SCBool;
static_assert(sizeof(SCBool) == 1, "unexpected size of 'bool'");
#else
// let's take the chance and use a well-defined type for SCBool.
typedef uint8_t SCBool;
#endif // __cplusplus

enum { kSCTrue = 1, kSCFalse = 0 };

typedef int32_t SCErr;

typedef int64_t int64;
typedef uint64_t uint64;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int16_t int16;
typedef uint16_t uint16;

typedef int8_t int8;
typedef uint8_t uint8;

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

#ifdef __cplusplus
const unsigned int kSCNameLen = 8;
const unsigned int kSCNameByteLen = 8 * sizeof(int32);
#else
#    define kSCNameLen 8
#    define kSCNameByteLen (8 * sizeof(int32))
#endif
