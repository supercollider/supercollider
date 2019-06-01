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
/*

AdvancingAllocPool implements a simple advancing pointer allocation scheme.
There is no Free(). All objects in the pool are freed at once with FreeAll().
Thus it is very fast.

*/

#pragma once

#include <stdexcept>
#include <stdlib.h>

class AllocPool;

struct AdvancingAllocPoolChunk;

typedef int int32;

inline void FailNil(void* ptr) {
    if (!ptr)
        throw std::runtime_error("alloc failed");
}

struct AdvancingAllocPoolChunkHdr {
    AdvancingAllocPoolChunk* mNext;
    size_t mSize;
    int32 mPad1, mPad2;
};

struct AdvancingAllocPoolChunk {
    AdvancingAllocPoolChunk* mNext;
    size_t mSize;
    int32 mPad1, mPad2;
    char mSpace[16];
};

class AdvancingAllocPool {
public:
    AdvancingAllocPool();
    ~AdvancingAllocPool() { FreeAll(); }

    void Init(AllocPool* inAllocPool, size_t initSize, size_t growSize, size_t tooBigSize);

    void* Alloc(size_t inBytes);
    void FreeAll();

    bool SanityCheck();

private:
    void AddChunk(size_t inSize);

    AllocPool* mAllocPool;
    size_t mInitSize;
    size_t mGrowSize;
    size_t mCurSize;
    size_t mTooBig;
    AdvancingAllocPoolChunk* mChunks;
    AdvancingAllocPoolChunk* mFatties;
};
