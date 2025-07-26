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
#include <cstddef>
#include <cstdint>
#include <type_traits>


typedef unsigned char Byte;

#define BYTE_CODE_CHUNK_SIZE 64


typedef struct {
    Byte* bytes;
    Byte* ptr;
    size_t size;
} ByteCodeArray, *ByteCodes;

extern ByteCodes gCompilingByteCodes;
extern std::int64_t totalByteCodes;

void initByteCodes();
void emitByte(Byte byte);
void compileAndFreeByteCodes(ByteCodes byteCodes);
void copyByteCodes(Byte* dest, ByteCodes byteCodes);
ByteCodes getByteCodes();
ByteCodes saveByteCodeArray();
void restoreByteCodeArray(ByteCodes byteCodes);
size_t byteCodeLength(ByteCodes byteCodes);
void emitByteCodes(ByteCodes byteCodes);
ByteCodes allocByteCodes();
void reallocByteCodes(ByteCodes byteCodes);
void freeByteCodes(ByteCodes byteCodes);
