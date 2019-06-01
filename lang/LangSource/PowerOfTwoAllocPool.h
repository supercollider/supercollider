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

// Implements a power of two size class allocator.
// There is no consolidation of free space. Once a chunk is allocated it
// remains at that size from then on whether free or allocated.
// It uses AdvancingAllocPool as its parent allocator.
// It is very fast. This is used to allocate Unit output buffers.

#pragma once

#include <stdexcept>
#include <stdlib.h>
#include "clz.h"
#include "AdvancingAllocPool.h"
#include "SC_AllocPool.h"
void post(const char* fmt, ...);
void postbuf(const char* fmt, ...);

template <class Hdr, class Obj, class Elem, int LargeObjSizeClass, int Align> class PowerOfTwoAllocPool {
public:
    PowerOfTwoAllocPool(::AllocPool* inPool, size_t initSize = 64 * 1024, size_t growSize = 64 * 1024) {
        mLargeObjPool = inPool;
        mNumLargeObjects = 0;
        mNumAlloc = 0;
        mNumFree = 0;
        size_t tooBigSize = (sizeof(Hdr) + (sizeof(Elem) << (LargeObjSizeClass - 1))) + 1;
        mSmallObjPool.Init(inPool, initSize, growSize, tooBigSize);
        Init();
        // assert(SanityCheck());
    }
    ~PowerOfTwoAllocPool() {
        // assert(SanityCheck());
        // assert(mNumLargeObjects == 0); // you have to free the big ones yourself
        mSmallObjPool.FreeAll();
    }

    Obj* Alloc(int32 inNumElems) {
        // mNumAlloc++;
        // assert(SanityCheck());
        int sizeclass = LOG2CEIL(inNumElems);
        if (sizeclass >= LargeObjSizeClass) {
            mNumLargeObjects++;
            size_t size = sizeof(Hdr) + (sizeof(Elem) * inNumElems);
            return (Obj*)mLargeObjPool->Alloc(size);
        }

        // get from free list
        Obj* obj = mFreeLists[sizeclass];
        if (obj != NULL) {
            // set free list to next element.
            mFreeLists[sizeclass] = *(Obj**)obj;
        } else {
            // have to allocate it
            size_t size = mSizes[sizeclass];
            obj = (Obj*)mSmallObjPool.Alloc(size);
            if (!obj)
                throw runtime_error("PowerOfTwoAllocPool out of memory");
        }
        // obj->mMagic = 'magk';
        // assert(SanityCheck());
        return obj;
    }
    void Free(Obj* inObjPtr) {
        // mNumFree++;
        // assert(SanityCheck());
        if (inObjPtr == 0)
            return; /* free(0) has no effect */
        /*if (inObjPtr->mMagic != 'magk') {
            postbuf("bad object\n");
            throw runtime_error("bad object");
        }*/
        int sizeclass = inObjPtr->SizeClass();
        if (sizeclass >= LargeObjSizeClass) {
            mLargeObjPool->Free(inObjPtr);
            mNumLargeObjects--;
        } else {
            Obj* nextfree = mFreeLists[sizeclass];
            mFreeLists[sizeclass] = inObjPtr;
            *(Obj**)inObjPtr = nextfree;
        }
        // assert(SanityCheck());
    }
    void FreeAll() {
        // assert(mNumLargeObjects == 0); // you have to free the big ones yourself
        mSmallObjPool.FreeAll();
        Init();
    }

    bool SanityCheck() {
        // postbuf("PowerOfTwoAllocPool::SanityCheck %d %d\n", mNumAlloc, mNumFree);
        mLargeObjPool->DoCheckPool();
        mSmallObjPool.SanityCheck();
        for (int i = 0; i < LargeObjSizeClass; ++i) {
            Obj* obj = mFreeLists[i];
            for (int j = 0; obj; ++j) {
                if (j >= 1000) {
                    post("linked loop??\n");
                    throw runtime_error("linked loop??\n");
                    return false;
                }
                obj = *(Obj**)obj;
            }
        }
        return true;
    }

private:
    void Init() {
        for (int i = 0; i < LargeObjSizeClass; ++i) {
            mFreeLists[i] = NULL;
            size_t size = sizeof(Hdr) + (sizeof(Elem) << i);
            mSizes[i] = (size + (Align - 1)) & ~(Align - 1); // alignment
        }
    }

    Obj* mFreeLists[LargeObjSizeClass];
    size_t mSizes[LargeObjSizeClass];
    AllocPool* mLargeObjPool;
    AdvancingAllocPool mSmallObjPool;
    int mNumLargeObjects;
    int mNumAlloc, mNumFree;
};
