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

#include <stdlib.h>
#include <string.h>
#include "SCBase.h"
#include "InitAlloc.h"
#include "ByteCodeArray.h"
#include "Opcodes.h"
#include <cstdint>

ByteCodes gCompilingByteCodes;
std::int64_t totalByteCodes = 0;

void initByteCodes() {
    if (gCompilingByteCodes) {
        freeByteCodes(gCompilingByteCodes);
        gCompilingByteCodes = nullptr;
    }
}

void emitByte(Byte byte) {
    if (gCompilingByteCodes == nullptr) {
        gCompilingByteCodes = allocByteCodes();
    }

    if ((gCompilingByteCodes->ptr - gCompilingByteCodes->bytes) >= gCompilingByteCodes->size) {
        reallocByteCodes(gCompilingByteCodes);
    }
    totalByteCodes++;
    *gCompilingByteCodes->ptr++ = byte;
}

void compileAndFreeByteCodes(ByteCodes byteCodes) {
    emitByteCodes(byteCodes);
    freeByteCodes(byteCodes);
}

void copyByteCodes(Byte* dest, ByteCodes byteCodes) { memcpy(dest, byteCodes->bytes, byteCodeLength(byteCodes)); }

ByteCodes getByteCodes() {
    ByteCodes curByteCodes;

    curByteCodes = gCompilingByteCodes;
    gCompilingByteCodes = nullptr;

    return curByteCodes;
}

ByteCodes saveByteCodeArray() {
    ByteCodes curByteCodes;

    curByteCodes = gCompilingByteCodes;
    gCompilingByteCodes = nullptr;

    return curByteCodes;
}

void restoreByteCodeArray(ByteCodes byteCodes) { gCompilingByteCodes = byteCodes; }

size_t byteCodeLength(ByteCodes byteCodes) {
    if (!byteCodes)
        return 0;
    return (byteCodes->ptr - byteCodes->bytes);
}

/***********************************************************************
 *
 *	Internal routines.
 *
 ***********************************************************************/

void emitByteCodes(ByteCodes byteCodes) {
    Byte* ptr;
    int i;

    if (byteCodes == nullptr)
        return;

    // postfl("[%d]\n", byteCodes->ptr - byteCodes->bytes);
    for (i = 0, ptr = byteCodes->bytes; ptr < byteCodes->ptr; ptr++, ++i) {
        emitByte(*ptr);

        // postfl("%02X ", *ptr);
        // if ((i & 15) == 15) postfl("\n");
    }
    // postfl("\n\n");
}

ByteCodes allocByteCodes() {
    ByteCodes newByteCodes;

    // pyrmalloc: I think that all bytecodes are copied to objects
    // lifetime: kill after compile
    newByteCodes = (ByteCodes)pyr_pool_compile->Alloc(sizeof(ByteCodeArray));
    MEMFAIL(newByteCodes);
    newByteCodes->bytes = (Byte*)pyr_pool_compile->Alloc(BYTE_CODE_CHUNK_SIZE);
    MEMFAIL(newByteCodes->bytes);
    newByteCodes->ptr = newByteCodes->bytes;
    newByteCodes->size = BYTE_CODE_CHUNK_SIZE;
    // postfl("allocByteCodes %0X\n", newByteCodes);
    return newByteCodes;
}

void reallocByteCodes(ByteCodes byteCodes) {
    Byte* newBytes;

    if (byteCodes->size != (byteCodes->ptr - byteCodes->bytes))
        error("reallocByteCodes called with size %d != byteCode len %d: \n", byteCodes->size,
              (byteCodes->ptr - byteCodes->bytes));

    size_t newLen = byteCodes->size << 1;
    // pyrmalloc: I think that all bytecodes are copied to objects
    // lifetime: kill after compile
    newBytes = (Byte*)pyr_pool_compile->Alloc(newLen);
    MEMFAIL(newBytes);
    memcpy(newBytes, byteCodes->bytes, byteCodes->size);
    pyr_pool_compile->Free(byteCodes->bytes);

    byteCodes->bytes = newBytes;
    byteCodes->ptr = newBytes + byteCodes->size;
    byteCodes->size = newLen;
}


void freeByteCodes(ByteCodes byteCodes) {
    // postfl("freeByteCodes %0X\n", byteCodes);
    if (byteCodes != nullptr) {
        pyr_pool_compile->Free(byteCodes->bytes);
        pyr_pool_compile->Free(byteCodes);
    }
}
