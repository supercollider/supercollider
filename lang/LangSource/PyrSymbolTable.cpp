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

#include "PyrSymbol.h"
#include "SCBase.h"
#include "PyrSymbolTable.h"
#include "clz.h"
#include <limits>
#include <stdexcept>
#include <string>
#include <stdlib.h>
#include <string.h>
#include "InitAlloc.h"
#include "VMGlobals.h"
#include "Hash.h"


static int checkedSymbolLength(std::size_t len) {
    if (len > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        throw std::length_error { "Symbol is too long." };
    return static_cast<int>(len);
}

SCLANG_DLLEXPORT_C PyrSymbol* getsym(const char* name) {
    // Hashes and measures the name in one pass.
    std::size_t len;
    const int hash = Hash(name, &len);
    const int length = checkedSymbolLength(len);
    PyrSymbol* symbol = gMainVMGlobals->symbolTable->Make(name, length, hash);
    if (!symbol) {
        fprintf(stderr, "getsym failed '%s'\n", name);
        exit(-1);
    }
    return symbol;
}

SCLANG_DLLEXPORT_C PyrSymbol* getsymlen(const char* name, std::size_t len) {
    // A symbol's name is also used as a C string, so it cannot contain a null.
    if (const auto* null = static_cast<const char*>(memchr(name, 0, len)))
        len = static_cast<std::size_t>(null - name);
    const int length = checkedSymbolLength(len);
    PyrSymbol* symbol = gMainVMGlobals->symbolTable->Make(name, length, Hash(name, length));
    if (!symbol) {
        fprintf(stderr, "getsym failed '%.*s'\n", length, name);
        exit(-1);
    }
    return symbol;
}

SCLANG_DLLEXPORT_C PyrSymbol* getmetasym(const PyrSymbol* sym) {
    std::string str { metaClassPrefix };
    str += sym->view();
    return getsymlen(str.data(), str.size());
}

SCLANG_DLLEXPORT_C PyrSymbol* getsettersym(const PyrSymbol* sym) {
    std::string str { sym->view() };
    str += '_';
    return getsymlen(str.data(), str.size());
}

SCLANG_DLLEXPORT_C PyrSymbol* findsym(const char* name) {
    std::size_t len;
    const int hash = Hash(name, &len);
    if (len > static_cast<std::size_t>(std::numeric_limits<int>::max()))
        return nullptr;
    return gMainVMGlobals->symbolTable->Find(name, static_cast<int>(len), hash);
}

SymbolSpace::SymbolSpace(AllocPool* inPool) {
    mPool = inPool;
    mStringPool.Init(inPool, STRINGCHUNK, STRINGCHUNK, STRINGCHUNK / 5);
    mSymbolPool.Init(inPool, SYMBOLCHUNK, SYMBOLCHUNK, SYMBOLCHUNK / 5);
}


PyrSymbol* SymbolSpace::NewSymbol(const char* inName, int inLength, int inHash) {
    PyrSymbol* sym;
    sym = (PyrSymbol*)mSymbolPool.Alloc(sizeof(PyrSymbol));
    MEMFAIL(sym);
    sym->name = (char*)mStringPool.Alloc(inLength + 1);
    MEMFAIL(sym->name);
    memcpy(sym->name, inName, inLength);
    sym->name[inLength] = 0;
    sym->hash = inHash;
    sym->length = inLength;
    sym->specialIndex = -1;
    sym->flags = 0;
    if (inLength > 0 && inName[0] >= 'A' && inName[0] <= 'Z')
        sym->flags |= sym_Class;
    if (inLength > 1 && inName[0] == '_')
        sym->flags |= sym_Primitive;
    if (inLength > 1 && inName[inLength - 1] == '_')
        sym->flags |= sym_Setter;
    sym->u.index = 0;
    return sym;
}


SymbolTable::SymbolTable(AllocPool* inPool, int inSize): mPool(inPool), mSpace(inPool), mMaxItems(inSize) {
    assert(ISPOWEROFTWO(inSize));

    AllocTable();
}


void SymbolTable::CopyFrom(SymbolTable& inTable) {
    MakeEmpty();
    Rehash(inTable.mTable, inTable.mMaxItems);
}

PyrSymbol* SymbolTable::Find(const char* inName, int inLength, int inHash) {
    int index = inHash & mMask;
    PyrSymbol* sym = mTable[index];
    while (sym && (sym->hash != inHash || sym->length != inLength || memcmp(inName, sym->name, inLength) != 0)) {
        index = (index + 1) & mMask;
        sym = mTable[index];
    }
    return sym;
}

void SymbolTable::Add(PyrSymbol* inSymbol) {
    if (mNumItems + 1 > (mMaxItems >> 1))
        Grow();

    int index = inSymbol->hash & mMask;
    PyrSymbol* testSymbol = mTable[index];
    while (testSymbol && testSymbol != inSymbol) {
        index = (index + 1) & mMask;
        testSymbol = mTable[index];
    }
    if (!testSymbol) { // if it is not already in the table.
        mTable[index] = inSymbol;
        mNumItems++;
    }
}

PyrSymbol* SymbolTable::MakeNew(const char* inName, int inLength, int inHash) {
    PyrSymbol* symbol = mSpace.NewSymbol(inName, inLength, inHash);
    Add(symbol);

    return symbol;
}

PyrSymbol* SymbolTable::Make(const char* inName, int inLength, int inHash) {
    PyrSymbol* symbol = Find(inName, inLength, inHash);
    if (!symbol)
        symbol = MakeNew(inName, inLength, inHash);
    return symbol;
}

void SymbolTable::MakeEmpty() {
    int size = mMaxItems * sizeof(PyrSymbol*);
    memset(mTable, 0, size);
    mNumItems = 0;
}

void SymbolTable::AllocTable() {
    int size = mMaxItems * sizeof(PyrSymbol*);
    mTable = (PyrSymbol**)mPool->Alloc(size);
    MEMFAIL(mTable);

    MakeEmpty();
    mMask = mMaxItems - 1;
}

void SymbolTable::Rehash(PyrSymbol** inTable, int inSize) {
    // rehash all entries from inTable into the new table
    for (int i = 0; i < inSize; ++i) {
        if (inTable[i])
            Add(inTable[i]);
    }
}

void SymbolTable::Grow() {
    PyrSymbol** oldtable = mTable;
    int oldsize = mMaxItems;

    // create new table
    mMaxItems += mMaxItems;
    AllocTable();

    Rehash(oldtable, oldsize);

    mPool->Free(oldtable);
}
