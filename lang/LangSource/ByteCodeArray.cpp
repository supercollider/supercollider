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

ByteCodes gCompilingByteCodes;
long totalByteCodes = 0;

void initByteCodes() {
    if (gCompilingByteCodes) {
        freeByteCodes(gCompilingByteCodes);
        gCompilingByteCodes = NULL;
    }
}

int compileOpcode(long opcode, long operand1) {
    int retc;
    if (operand1 <= 15) {
        compileByte((opcode << 4) | operand1);
        retc = 1;
    } else {
        compileByte(opcode);
        compileByte(operand1);
        if (opcode == opSendMsg || opcode == opSendSpecialMsg || opcode == opSendSuper) {
            // these expect numKeyArgsPushed to be passed.
            compileByte(0);
        }
        retc = 2;
    }
    return retc;
}

void compileJump(long opcode, long jumplen) {
    compileByte((opSpecialOpcode << 4) | opcode);
    compileByte((jumplen >> 8) & 0xFF);
    compileByte(jumplen & 0xFF);
}

void compileByte(long byte) {
    if (gCompilingByteCodes == NULL) {
        gCompilingByteCodes = allocByteCodes();
    }

    if ((gCompilingByteCodes->ptr - gCompilingByteCodes->bytes) >= gCompilingByteCodes->size) {
        reallocByteCodes(gCompilingByteCodes);
    }
    totalByteCodes++;
    *gCompilingByteCodes->ptr++ = byte;
}

int compileNumber(unsigned long value) {
    compileByte((value >> 24) & 0xFF);
    compileByte((value >> 16) & 0xFF);
    compileByte((value >> 8) & 0xFF);
    compileByte(value & 0xFF);
    return 4;
}

int compileNumber24(unsigned long value) {
    compileByte((value >> 16) & 0xFF);
    compileByte((value >> 8) & 0xFF);
    compileByte(value & 0xFF);
    return 4;
}

void compileAndFreeByteCodes(ByteCodes byteCodes) {
    compileByteCodes(byteCodes);
    freeByteCodes(byteCodes);
}

void copyByteCodes(Byte* dest, ByteCodes byteCodes) { memcpy(dest, byteCodes->bytes, byteCodeLength(byteCodes)); }

ByteCodes getByteCodes() {
    ByteCodes curByteCodes;

    curByteCodes = gCompilingByteCodes;
    gCompilingByteCodes = NULL;

    return curByteCodes;
}

ByteCodes saveByteCodeArray() {
    ByteCodes curByteCodes;

    curByteCodes = gCompilingByteCodes;
    gCompilingByteCodes = NULL;

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

void compileByteCodes(ByteCodes byteCodes) {
    Byte* ptr;
    int i;

    if (byteCodes == NULL)
        return;

    // postfl("[%d]\n", byteCodes->ptr - byteCodes->bytes);
    for (i = 0, ptr = byteCodes->bytes; ptr < byteCodes->ptr; ptr++, ++i) {
        compileByte(*ptr);

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

    if (byteCodes->size != (byteCodes->ptr - byteCodes->bytes)) {
        error("reallocByteCodes called with size != byteCode len");
    }

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
    if (byteCodes != NULL) {
        pyr_pool_compile->Free(byteCodes->bytes);
        pyr_pool_compile->Free(byteCodes);
    }
}
