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

#include "PyrSymbol.h"
#include "AdvancingAllocPool.h"
#include "SC_Export.h"

#define STRINGCHUNK 32000
#define SYMBOLCHUNK 32000

SCLANG_DLLEXPORT_C PyrSymbol* getsym(const char* name);
SCLANG_DLLEXPORT_C PyrSymbol* findsym(const char* name);

class SymbolSpace {
public:
    SymbolSpace(AllocPool* inPool);
    // inName must be a valid c string (null terminated)
    PyrSymbol* NewSymbol(const char* inName, hash_t inHash, size_t inLength);

private:
    AllocPool* mPool;
    AdvancingAllocPool mStringPool;
    AdvancingAllocPool mSymbolPool;
};

class SymbolTable {
    // Size of the symbol is size_t.
    // Num elements is uint32 (this was int).
public:
    SymbolTable(AllocPool* inPool, uint32 inSize);

    void CopyFrom(SymbolTable& inTable);

    [[nodiscard]] uint32 NumItems() const { return mNumItems; }
    [[nodiscard]] uint32 TableSize() const { return mMaxItemsPowerOfTwo; }
    PyrSymbol* Get(uint32 inIndex) { return mTable[inIndex]; }

    void CheckSymbols();

private:
    friend PyrSymbol* getsym(const char* name);
    friend PyrSymbol* findsym(const char* name);

    PyrSymbol* Find(const char* inName);
    PyrSymbol* Make(const char* inName);
    PyrSymbol* MakeNew(const char* inName, hash_t inHash, size_t inLength);

    void AllocTable();
    void Grow();
    PyrSymbol* Find(const char* inName, hash_t inHash);
    void Add(PyrSymbol* inSymbol);
    void Rehash(PyrSymbol** oldTable, uint32 inSize);
    void RehashAssertCapacitySufficient(PyrSymbol** oldTable, uint32 inSize);
    void MakeEmpty();

    AllocPool* mPool;
    SymbolSpace mSpace;
    PyrSymbol** mTable = nullptr;
    uint32 mNumItems = 0, mMaxItemsPowerOfTwo, mMaxSizeModuloMask;
    static constexpr auto mMaxCapacity = std::numeric_limits<decltype(mNumItems)>::max() / 2;
};
