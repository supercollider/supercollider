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
    PyrSymbol* NewSymbol(const char* inName, int inHash, int inLength);

private:
    AllocPool* mPool;
    AdvancingAllocPool mStringPool;
    AdvancingAllocPool mSymbolPool;
};

class SymbolTable {
public:
    SymbolTable(AllocPool* inPool, int inSize);

    void CopyFrom(SymbolTable& inTable);

    int NumItems() { return mNumItems; }
    int TableSize() { return mMaxItems; }
    PyrSymbol* Get(int inIndex) { return mTable[inIndex]; }

    void CheckSymbols();

private:
    friend PyrSymbol* getsym(const char* name);
    friend PyrSymbol* findsym(const char* name);

    PyrSymbol* Find(const char* inName);
    PyrSymbol* Make(const char* inName);
    PyrSymbol* MakeNew(const char* inName, int inHash, int inLength);

    int StrHash(const char* inName, size_t* outLength);
    void AllocTable();
    void Grow();
    PyrSymbol* Find(const char* inName, int inHash);
    void Add(PyrSymbol* inSymbol);
    void Rehash(PyrSymbol** inTable, int inSize);
    void MakeEmpty();

    AllocPool* mPool;
    SymbolSpace mSpace;
    PyrSymbol** mTable;
    int mNumItems, mMaxItems, mMask;
};
