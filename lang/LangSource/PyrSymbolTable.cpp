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
#include "PyrSymbol.h"
#include "InitAlloc.h"
#include "VMGlobals.h"
#include "Hash.h"

#include <cstring>

SCLANG_DLLEXPORT_C PyrSymbol* getsym(const char* name) { return gMainVMGlobals->symbolTable->Make(name); }

SCLANG_DLLEXPORT_C PyrSymbol* getmetasym(const char* name) {
    // TODO: make this function is safe!
    // If the class name is longer than 250 characters it will be truncated,
    //      which means there could be a name collision here (which would be very bad for sclang).
    // However, if this throws, a long class name will crash the interpreter without a good warning.

    // Therefore, the following lines are omited.

    // if (strlen(name) > 250)
    //      throw std::runtime_error("Class name too long, max 250 characters. Received: " + std::string(name) + '\n');

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

PyrSymbol* SymbolSpace::NewSymbol(const char* inName, hash_t inHash, size_t inLength) {
    auto* sym_ptr = mSymbolPool.Alloc(sizeof(PyrSymbol));
    if (sym_ptr == nullptr)
        throw std::runtime_error(exceptionMsgs::outOfMemory);

    char* name_ptr = reinterpret_cast<char*>(mStringPool.Alloc(inLength + 1));
    if (name_ptr == nullptr)
        throw std::runtime_error(exceptionMsgs::outOfMemory);
    strcpy(name_ptr, inName);

    return new (sym_ptr) PyrSymbol(name_ptr, inHash, inLength, -1);
}

// if given zero, this returns zero.
// if given a power of two, returns input
[[nodiscard]] constexpr inline uint32 next_power_of_two(uint32 n) noexcept {
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n++;
    return n;
}

SymbolTable::SymbolTable(AllocPool* inPool, uint32 inSize):
    mPool(inPool),
    mSpace(inPool),
    mMaxItemsPowerOfTwo(next_power_of_two(std::max(inSize, uint32(2)))),
    mMaxSizeModuloMask(mMaxItemsPowerOfTwo - 1) {
    if (inSize >= mMaxCapacity)
        throw std::runtime_error("Exceeded maximum size of symbol table.");
    AllocTable();
}

void SymbolTable::CopyFrom(SymbolTable& inTable) {
    MakeEmpty();
    Rehash(inTable.mTable, inTable.mMaxItemsPowerOfTwo);
}

PyrSymbol* SymbolTable::Find(const char* inName) { return Find(inName, Hash(inName)); }

PyrSymbol* SymbolTable::Find(const char* inName, hash_t inHash) {
    uint32 index = inHash & mMaxSizeModuloMask;
    PyrSymbol* sym = mTable[index];
    while (sym && (sym->hash != inHash || strcmp(inName, sym->name) != 0)) {
        index = (index + 1) & mMaxSizeModuloMask;
        sym = mTable[index];
    }
    return sym;
}

void SymbolTable::Add(PyrSymbol* inSymbol) {
    if (mNumItems + 1 > mMaxItemsPowerOfTwo / 2)
        Grow();

    uint32 index = inSymbol->hash & mMaxSizeModuloMask;
    PyrSymbol* testSymbol = mTable[index];
    while (testSymbol && testSymbol != inSymbol) {
        index = (index + 1) & mMaxSizeModuloMask;
        testSymbol = mTable[index];
    }
    if (!testSymbol) { // if it is not already in the table.
        mTable[index] = inSymbol;
        mNumItems++;
    }
}

PyrSymbol* SymbolTable::MakeNew(const char* inName, hash_t inHash, size_t inLength) {
    PyrSymbol* symbol = mSpace.NewSymbol(inName, inHash, inLength);
    Add(symbol);
    return symbol;
}

PyrSymbol* SymbolTable::Make(const char* inName) {
    const auto [hash, length] = HashWithSize(inName);
    return Find(inName, hash) ?: MakeNew(inName, hash, length);
}

void SymbolTable::MakeEmpty() {
    const auto size = mMaxItemsPowerOfTwo * sizeof(PyrSymbol*);
    memset(mTable, 0, size);
    mNumItems = 0;
}

void SymbolTable::AllocTable() {
    const auto size = mMaxItemsPowerOfTwo * sizeof(PyrSymbol*);
    mTable = (PyrSymbol**)mPool->Alloc(size);
    if (mTable == nullptr)
        throw std::runtime_error(exceptionMsgs::outOfMemory);
    memset(mTable, 0, size);
    mNumItems = 0;
    mMaxSizeModuloMask = mMaxItemsPowerOfTwo - 1;
}

void SymbolTable::Rehash(PyrSymbol** oldTable, uint32 inSize) {
    for (auto i = 0; i < inSize; ++i) {
        if (oldTable[i])
            Add(oldTable[i]);
    }
}

void SymbolTable::RehashAssertCapacitySufficient(PyrSymbol** oldTable, uint32 inSize) {
    for (auto i = 0; i < inSize; ++i) {
        if (oldTable[i]) {
            uint32 index = oldTable[i]->hash & mMaxSizeModuloMask;
            PyrSymbol* testSymbol = mTable[index];
            while (testSymbol && testSymbol != oldTable[i]) {
                index = (index + 1) & mMaxSizeModuloMask;
                testSymbol = mTable[index];
            }
            if (!testSymbol) { // if it is not already in the table.
                mTable[index] = oldTable[i];
                mNumItems++;
            }
        }
    }
}

void SymbolTable::Grow() {
    if (mMaxItemsPowerOfTwo >= mMaxCapacity)
        throw std::runtime_error("Exceeded maximum size of symbol table.");

    PyrSymbol** oldtable = mTable;
    const auto oldsize = mMaxItemsPowerOfTwo;

    // create new table
    mMaxItemsPowerOfTwo *= 2;
    AllocTable();

    RehashAssertCapacitySufficient(oldtable, oldsize);

    mPool->Free(oldtable);
}

void SymbolTable::CheckSymbols() {
    for (auto i = 0; i < TableSize(); ++i) {
        PyrSymbol* symbol = Get(i);

        if (symbol && symbol->u.index == 0) {
            const auto c = symbol->name[0];
            if (c == '_') {
                post("WARNING: Primitive '%s' used but not bound\n", symbol->name);
            } else if (std::isupper(static_cast<unsigned char>(c))) {
                post("WARNING: Symbol '%s' used but not defined as a Class\n", symbol->name);
            } else if ((symbol->flags & sym_Called) && !(symbol->flags & sym_Selector)) {
                post("WARNING: Method '%s' called but not defined\n", symbol->name);
            }
        }
    }
}
