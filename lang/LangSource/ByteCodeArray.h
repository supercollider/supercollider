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

#include "InitAlloc.h"
#include "DebugTable.h"
#include "SC_Assert.h"
#include <deque>

template <class T> class CompilePoolAllocator {
public:
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    template <typename OtherT> struct rebind { typedef CompilePoolAllocator<OtherT> other; };

    CompilePoolAllocator() {}

    template <class U> CompilePoolAllocator(const CompilePoolAllocator<U>& other) {}

    pointer address(reference x) const { return &x; }
    const_pointer address(const_reference x) const { return &x; }
    CompilePoolAllocator<T>& operator=(const CompilePoolAllocator<T>&) { return *this; }

    void construct(pointer p, const T& val) { new ((T*)p) T(val); }
    void destroy(pointer p) { p->~T(); }

    size_type max_size() const { return size_t(-1); }

    T* allocate(std::size_t num) {
        T* newObj = (T*)pyr_pool_compile->Alloc(sizeof(T) * num);
        MEMFAIL(newObj);
        return newObj;
    }

    void deallocate(T* p, std::size_t n) {
        if (SC_COND_ASSERT(p != NULL)) {
            pyr_pool_compile->Free(p);
        }
    }
};

// ByteCodes ownership:
//	ByteCodesBase is always accessed via a unique_ptr, ByteCodes. ByteCodes objects can have only one
//  owner at a time. std::move always signifies a transfer of ownership, and must be used for functions
//  which accept a ByteCodes as an argument. After an std::move(byteCodes), byteCodes is NULL and
//  is no longer accessible. ByteCodesRef represents a non-ownership-transferring argument - no std::move
//  is required in this case.
class ByteCodesBase;
typedef unsigned char Byte;
typedef std::unique_ptr<ByteCodesBase> ByteCodes;
typedef const ByteCodes& ByteCodesRef; // ownership retained by caller
typedef std::deque<Byte, CompilePoolAllocator<Byte>> ByteArray;

class ByteCodesBase {
public:
    void append(const Byte& byte);

    void append(const Byte& byte, const DebugTableEntry& dbEntry);

    // append bytecodes onto the end of this collection
    void append(ByteCodesRef inByteCodes);

    void setByte(size_t index, const Byte& byte);

    size_t length() const;

    void copyTo(Byte* byteArray) const;

    void copyDebugTo(PyrInt32Array& array, int lineOffset = 0, int charOffset = 0) const;

private:
    ByteArray mByteCodes;
    DebugTable mDebugTable;
};

extern ByteCodes gCompilingByteCodes;
extern long totalByteCodes;

void initByteCodes();
void compileByte(long byte);
void compileAndFreeByteCodes(ByteCodes byteCodes);
void copyByteCodes(Byte* dest, ByteCodesRef byteCodes);
ByteCodes getByteCodes();
ByteCodes saveByteCodeArray();
void restoreByteCodeArray(ByteCodes byteCodes);
size_t byteCodeLength(ByteCodesRef byteCodes);
void compileByteCodes(ByteCodesRef byteCodes);
ByteCodes allocByteCodes();
void reallocByteCodes(ByteCodes byteCodes);
void freeByteCodes(ByteCodes byteCodes);
int compileOpcode(long opcode, long operand1);
void compileJump(long opcode, long jumplen);
int compileNumber(unsigned long value);
int compileNumber24(unsigned long value);
