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

#include "SCBase.h"
#include "PyrSymbolTable.h"
#include "clz.h"
#include <stdlib.h>
#include <string.h>
#include "InitAlloc.h"
#include "VMGlobals.h"
#include "Hash.h"

SCLANG_DLLEXPORT_C PyrSymbol* getsym(const char* name) {
    PyrSymbol* symbol = gMainVMGlobals->symbolTable->Make(name);
    if (!symbol) {
        fprintf(stderr, "getsym failed '%s'\n", name);
        exit(-1);
    }
    return symbol;
}

SCLANG_DLLEXPORT_C PyrSymbol* getmetasym(const char* name) {
    char str[256];
    strcpy(str, "Meta_");
    strncat(str, name, 250);
    return getsym(str);
}

SCLANG_DLLEXPORT_C PyrSymbol* findsym(const char* name) {
    PyrSymbol* symbol = gMainVMGlobals->symbolTable->Find(name);
    return symbol;
}

SymbolSpace::SymbolSpace(AllocPool* inPool) {
    mPool = inPool;
    mStringPool.Init(inPool, STRINGCHUNK, STRINGCHUNK, STRINGCHUNK / 5);
    mSymbolPool.Init(inPool, SYMBOLCHUNK, SYMBOLCHUNK, SYMBOLCHUNK / 5);
}


PyrSymbol* SymbolSpace::NewSymbol(const char* inName, int inHash, int inLength) {
    PyrSymbol* sym;
    sym = (PyrSymbol*)mSymbolPool.Alloc(sizeof(PyrSymbol));
    MEMFAIL(sym);
    sym->name = (char*)mStringPool.Alloc(inLength + 1);
    MEMFAIL(sym->name);
    strcpy(sym->name, inName);
    sym->hash = inHash;
    sym->length = inLength;
    sym->specialIndex = -1;
    sym->flags = 0;
    if (inName[0] >= 'A' && inName[0] <= 'Z')
        sym->flags |= sym_Class;
    if (inLength > 1 && inName[0] == '_')
        sym->flags |= sym_Primitive;
    if (inLength > 1 && inName[inLength - 1] == '_')
        sym->flags |= sym_Setter;
    sym->u.index = 0;
    sym->classdep = NULL;
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

int SymbolTable::StrHash(const char* inName, size_t* outLength) { return Hash(inName, outLength); }

PyrSymbol* SymbolTable::Find(const char* inName) {
    size_t length;
    int hash = StrHash(inName, &length);
    return Find(inName, hash);
}

PyrSymbol* SymbolTable::Find(const char* inName, int inHash) {
    int index = inHash & mMask;
    PyrSymbol* sym = mTable[index];
    while (sym && (sym->hash != inHash || strcmp(inName, sym->name) != 0)) {
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

PyrSymbol* SymbolTable::MakeNew(const char* inName, int inHash, int inLength) {
    PyrSymbol* symbol = mSpace.NewSymbol(inName, inHash, inLength);
    Add(symbol);

    return symbol;
}

PyrSymbol* SymbolTable::Make(const char* inName) {
    size_t length;
    int hash = StrHash(inName, &length);
    PyrSymbol* symbol = Find(inName, hash);
    if (!symbol)
        symbol = MakeNew(inName, hash, length);
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

void SymbolTable::CheckSymbols() {
    for (int i = 0; i < TableSize(); ++i) {
        PyrSymbol* symbol = Get(i);
        if (symbol && symbol->u.index == 0) {
            int c;
            c = symbol->name[0];
            if (c == '_') {
                post("WARNING: Primitive '%s' used but not bound\n", symbol->name);
            } else if (c >= 'A' && c <= 'Z') {
                post("WARNING: Symbol '%s' used but not defined as a Class\n", symbol->name);
            } else if ((symbol->flags & sym_Called) && !(symbol->flags & sym_Selector)) {
                post("WARNING: Method '%s' called but not defined\n", symbol->name);
            }
        }
    }
}
