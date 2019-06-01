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


#include "AdvancingAllocPool.h"
#include "SC_AllocPool.h"
//#include <assert.h>


AdvancingAllocPool::AdvancingAllocPool() {
    mAllocPool = 0;
    mInitSize = 0;
    mGrowSize = 0;
    mTooBig = 0;
    mCurSize = 0;
    mChunks = NULL;
    mFatties = NULL;
}

void AdvancingAllocPool::Init(AllocPool* inAllocPool, size_t initSize, size_t growSize, size_t tooBigSize) {
    mAllocPool = inAllocPool;
    mInitSize = initSize;
    mGrowSize = growSize;
    mTooBig = tooBigSize;
    mChunks = NULL;
    AddChunk(initSize);
    mFatties = NULL;
    // assert(SanityCheck());
}

void AdvancingAllocPool::AddChunk(size_t inSize) {
    size_t chunkSize = sizeof(AdvancingAllocPoolChunkHdr) + inSize;
    AdvancingAllocPoolChunk* chunk = (AdvancingAllocPoolChunk*)mAllocPool->Alloc(chunkSize);
    FailNil(chunk);
    chunk->mNext = mChunks;
    mChunks = chunk;
    chunk->mSize = mGrowSize;
    mCurSize = 0;
}

void* AdvancingAllocPool::Alloc(size_t reqsize) {
    // assert(SanityCheck());
    // assert(mAllocPool);
    size_t size = (reqsize + 15) & ~15; // round up to 16 byte alignment
    if (size < mTooBig) {
        if (!mChunks)
            AddChunk(mInitSize);
        else if (mCurSize + size > mChunks->mSize)
            AddChunk(mGrowSize);
        char* space = mChunks->mSpace + mCurSize;
        mCurSize += size;

        // assert(SanityCheck());
        return (void*)space;
    } else {
        size_t chunkSize = sizeof(AdvancingAllocPoolChunkHdr) + size;
        AdvancingAllocPoolChunk* fatty = (AdvancingAllocPoolChunk*)mAllocPool->Alloc(chunkSize);
        FailNil(fatty);
        fatty->mNext = mFatties;
        mFatties = fatty;
        fatty->mSize = size;

        // assert(SanityCheck());
        return (void*)fatty->mSpace;
    }
}

void AdvancingAllocPool::FreeAll() {
    // assert(SanityCheck());
    AdvancingAllocPoolChunk *chunk, *next;
    for (chunk = mChunks; chunk; chunk = next) {
        next = chunk->mNext;
        mAllocPool->Free(chunk);
    }
    for (chunk = mFatties; chunk; chunk = next) {
        next = chunk->mNext;
        mAllocPool->Free(chunk);
    }
    mChunks = NULL;
    mFatties = NULL;
    mCurSize = 0;
    // assert(SanityCheck());
}

bool AdvancingAllocPool::SanityCheck() {
    AdvancingAllocPoolChunk *chunk, *next;
    for (chunk = mChunks; chunk; chunk = next) {
        next = chunk->mNext;
        mAllocPool->DoCheckInUseChunk(AllocPool::MemToChunk(chunk));
    }
    for (chunk = mFatties; chunk; chunk = next) {
        next = chunk->mNext;
        mAllocPool->DoCheckInUseChunk(AllocPool::MemToChunk(chunk));
    }
    return true;
}
