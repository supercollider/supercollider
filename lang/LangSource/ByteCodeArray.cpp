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
#include "ByteCodeArray.h"
#include "Opcodes.h"
#include "DebugTable.h"

ByteCodes gCompilingByteCodes;
long totalByteCodes = 0;

void ByteCodesBase::append(const Byte& byte, const DebugTableEntry& dbEntry) {
    mDebugTable.addEntry(dbEntry);
    mByteCodes.push_back(byte);
}

void ByteCodesBase::append(const Byte& byte) { append(byte, DebugTable::getPosition()); }

void ByteCodesBase::append(ByteCodesRef inByteCodes) {
    SC_ASSERT(inByteCodes->mDebugTable.size() == inByteCodes->mByteCodes.size());

    auto bytecodeIt = inByteCodes->mByteCodes.begin();
    auto debugIt = inByteCodes->mDebugTable.entries().begin();

    for (; bytecodeIt < inByteCodes->mByteCodes.end(); ++bytecodeIt, ++debugIt) {
        append(*bytecodeIt, *debugIt);
    }
}

void ByteCodesBase::setByte(size_t index, const Byte& byte) {
    if (SC_COND_ASSERT(index < mByteCodes.size())) {
        mByteCodes[index] = byte;
        mDebugTable.setEntry(index, DebugTable::getPosition());
    }
}

size_t ByteCodesBase::length() const { return mByteCodes.size(); }

void ByteCodesBase::copyTo(Byte* byteArray) const {
    Byte* iterPtr = byteArray;
    for (auto& byte : mByteCodes) {
        *iterPtr = byte;
        iterPtr++;
    }
}

void ByteCodesBase::copyDebugTo(PyrInt32Array& array, int lineOffset, int charOffset) const {
    SC_ASSERT(array.size == mDebugTable.size() * mDebugTable.kEntrySize);
    int tableI = 0;
    for (auto entry : mDebugTable.entries()) {
        array.i[tableI++] = entry.getLine() + lineOffset;
        array.i[tableI++] = entry.getCharacter() + charOffset;
    }
}

void initByteCodes() { gCompilingByteCodes.reset(new ByteCodesBase); }

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
        gCompilingByteCodes.reset(new ByteCodesBase);
    }

    totalByteCodes++;
    gCompilingByteCodes->append(byte);
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
    freeByteCodes(std::move(byteCodes));
}

void copyByteCodes(Byte* dest, ByteCodesRef byteCodes) { byteCodes->copyTo(dest); }

ByteCodes getByteCodes() {
    ByteCodes curByteCodes;

    curByteCodes = std::move(gCompilingByteCodes);
    SC_ASSERT(gCompilingByteCodes == NULL);

    return curByteCodes;
}

ByteCodes saveByteCodeArray() {
    ByteCodes curByteCodes;

    curByteCodes = std::move(gCompilingByteCodes);
    SC_ASSERT(gCompilingByteCodes == NULL);

    return curByteCodes;
}

void restoreByteCodeArray(ByteCodes byteCodes) {
    SC_ASSERT(!gCompilingByteCodes);
    gCompilingByteCodes = std::move(byteCodes);
}

size_t byteCodeLength(ByteCodesRef byteCodes) {
    if (!byteCodes) {
        return 0;
    } else {
        return byteCodes->length();
    }
}

/***********************************************************************
 *
 *	Internal routines.
 *
 ***********************************************************************/

void compileByteCodes(ByteCodesRef byteCodes) {
    SC_ASSERT(byteCodes != NULL);

    if (gCompilingByteCodes == NULL) {
        gCompilingByteCodes.reset(new ByteCodesBase);
    }

    totalByteCodes += byteCodes->length();
    gCompilingByteCodes->append(byteCodes);
}

ByteCodes allocByteCodes() {
    ByteCodes newByteCodes(new ByteCodesBase);
    return newByteCodes;
}

void freeByteCodes(ByteCodes byteCodes) {
    SC_ASSERT(byteCodes != NULL);
    byteCodes.reset(); // memory is free'd here
}
