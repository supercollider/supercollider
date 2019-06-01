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

#include <string.h>
#include <stdexcept>
#include "SC_AllocPool.h"
#include "SC_BoundsMacros.h"
#include <assert.h>
#include <string>

/*
   Requests are `small' if both the corresponding and the next bin are small
*/

#ifdef ENABLE_MEMORY_CHECKS
#    define check_pool() DoCheckPool()
#    define check_free_chunk(P) DoCheckFreeChunk(P)
#    define check_inuse_chunk(P) DoCheckInUseChunk(P)
#    define check_chunk(P) DoCheckChunk(P)
#    define check_malloced_chunk(P, N) DoCheckAllocedChunk(P, N)
#    define garbage_fill(P) DoGarbageFill(P)
#else
#    define check_pool()
#    define check_free_chunk(P)
#    define check_inuse_chunk(P)
#    define check_chunk(P)
#    define check_malloced_chunk(P, N)
#    define garbage_fill(P)
#endif

#define aligned_OK(m) ((((size_t)(m)) & kAlignMask) == 0)

/*
void* allocmem(AllocPool *pool, int32 size);
void* allocmem(AllocPool *pool, int32 size)
{
    return pool->Alloc(size);
}

void* reallocmem(AllocPool *pool, void* ptr, int32 size);
void* reallocmem(AllocPool *pool, void* ptr, int32 size)
{
    return pool->Realloc(ptr, size);
}

void freemem(AllocPool *pool, void* ptr);
void freemem(AllocPool *pool, void* ptr)
{
    pool->Free(ptr);
}
*/
void AllocPool::InitAlloc() {
    if (mAreaInitSize == 0)
        return;

    /* alloc initial area */
    NewArea(mAreaInitSize);
    /* get chunk */
    AllocAreaPtr area = mAreas;
    AllocChunkPtr chunk = &area->mChunk;
    LinkFree(chunk);

    check_pool();
}

void AllocPool::InitBins() {
    for (int i = 0; i < kNumAllocBins; ++i) {
        mBins[i].BeEmpty();
    }
    for (int i = 0; i < 4; ++i) {
        mBinBlocks[i] = 0;
    }
}

AllocPool::AllocPool(NewAreaFunc inAllocArea, FreeAreaFunc inFreeArea, size_t inAreaInitSize, size_t inAreaMoreSize) {
    InitBins();
    mAreaInitSize = inAreaInitSize;
    mAreaMoreSize = inAreaMoreSize;
    mAllocArea = inAllocArea;
    mFreeArea = inFreeArea;
    mAreas = 0;
    check_pool();

    InitAlloc();
}

AllocPool::~AllocPool() { FreeAll(); }

void AllocPool::FreeAll() {
    check_pool();
    AllocAreaPtr area = mAreas;
    if (area) {
        AllocAreaPtr firstArea = area;
        do {
            AllocAreaPtr nextarea = area->mNext;
            (mFreeArea)(area->mUnalignedPointerToThis);
            area = nextarea;
        } while (area != firstArea);
        mAreas = NULL;
    }
    InitBins();
    check_pool();
}

void AllocPool::FreeAllInternal() {
    check_pool();
    InitBins();

    AllocAreaPtr area = mAreas;
    if (area) {
        AllocAreaPtr firstArea = area;
        do {
            AllocAreaPtr nextarea = area->mNext;
            size_t size = area->mSize;
            AllocChunkPtr chunk = &area->mChunk;
            chunk->SetSizeFree(size);
            chunk->SetNeighborsInUse(size);
            LinkFree(chunk);
            area = nextarea;
        } while (area != firstArea);
    }
    check_pool();
}

void AllocPool::Reinit() {
    FreeAll();
    InitAlloc();
}

void AllocPool::Free(void* inPtr) {
#ifdef DISABLE_MEMORY_POOLS
    free(inPtr);
    return;
#endif

    check_pool();
    if (inPtr == 0)
        return; /* free(0) has no effect */

    AllocChunkPtr chunk = MemToChunk(inPtr);

    check_inuse_chunk(chunk);
    garbage_fill(chunk);

    size_t size = chunk->Size();

    if (!chunk->PrevInUse()) /* consolidate backward */
    {
        size_t prevSize = chunk->PrevSize();
        chunk = chunk->ChunkAtOffset(0L - prevSize);
        size += prevSize;
        UnlinkFree(chunk);
    }

    AllocChunkPtr next = chunk->ChunkAtOffset(size);
    if (!next->InUse()) /* consolidate forward */
    {
        size += next->Size();
        UnlinkFree(next);
    }

    chunk->SetSizeFree(size);
    if (mAreaMoreSize && chunk->IsArea()) {
        // whole area is free
        FreeArea(chunk);
    } else {
        LinkFree(chunk);
    }
    check_pool();
}


AllocAreaPtr AllocPool::NewArea(size_t inAreaSize) {
    void* ptr = (AllocAreaPtr)(mAllocArea)(inAreaSize + kAreaOverhead);

    if (ptr == NULL)
        throw std::runtime_error(std::string("Could not allocate new area"));

    // AllocAreaPtr area = (AllocAreaPtr)((unsigned long)ptr & ~kAlignMask);
    AllocAreaPtr area = (AllocAreaPtr)(((size_t)ptr + kAlignMask) & ~kAlignMask);
    assert((area >= ptr) && ((void*)((size_t)area & ~kAlignMask) == area));

    area->mUnalignedPointerToThis = ptr;

    /* link in area */
    if (mAreas) {
        area->mNext = mAreas;
        area->mPrev = mAreas->mPrev;
        area->mNext->mPrev = area;
        area->mPrev->mNext = area;
    } else {
        area->mNext = area;
        area->mPrev = area;
    }
    mAreas = area;

    /* set area size */
    area->mSize = inAreaSize;
    area->mChunk.BeEmpty();
    area->mChunk.SetNeighborsInUse(inAreaSize);
    area->mChunk.SetSizeFree(inAreaSize);

    return area;
}

void AllocPool::FreeArea(AllocChunkPtr chunk) {
    AllocAreaPtr area = (AllocAreaPtr)((char*)chunk - sizeof(AllocAreaHdr));

    if (area->mNext == area) {
        mAreas = NULL;
    } else {
        /* unlink area */
        mAreas = area->mPrev->mNext = area->mNext;
        area->mNext->mPrev = area->mPrev;
    }

    (mFreeArea)(area->mUnalignedPointerToThis);
}


size_t AllocPool::TotalFree() {
    size_t total = 0;
    for (int i = 0; i < kNumAllocBins; ++i) {
        AllocChunkPtr bin = mBins + i;
        if (bin->Prev() != bin) {
            for (AllocChunkPtr candidate = bin->Prev(); candidate != bin; candidate = candidate->Prev()) {
                total += candidate->Size();
            }
        }
    }
    return total;
}

size_t AllocPool::LargestFreeChunk() {
    int word = 0;
    for (int i = 3; i >= 0; --i) {
        if (mBinBlocks[i]) {
            word = i;
            break;
        }
    }
    int binBits = (int)mBinBlocks[word];
    int bitPosition = NUMBITS(binBits) - 1;
    int index = (word << 5) + bitPosition;
    AllocChunkPtr bin = mBins + index;
    // postbuf("** %p %p %p %p\n", mBinBlocks[0], mBinBlocks[1], mBinBlocks[2], mBinBlocks[3]);
    // postbuf("%d %d %d %p    %p %p\n", word, bitPosition, index, binBits, bin->Prev(), bin->Next());

    AllocChunkPtr candidate;
    size_t maxsize = 0;
    for (candidate = bin->Prev(); candidate != bin; candidate = candidate->Prev()) {
        size_t candidate_size = candidate->Size();
        maxsize = sc_max(maxsize, candidate_size);
        // postbuf("  %d %d\n", maxsize, candidate_size);
    }

    /*for (int i=0; i<kNumAllocBins; ++i) {
        bin = mBins + i;
        if (bin->Prev() != bin) {
            postbuf("*  %d %d\n", i, bin->Prev()->Size());
        }
    }*/
    return maxsize;
}

void* AllocPool::Alloc(size_t inReqSize) {
#ifdef DISABLE_MEMORY_POOLS
    return malloc(inReqSize);
#endif

    // OK it has a lot of gotos, but these remove a whole lot of common code
    // that was obfuscating the original version of this function.
    // So here I am choosing the OnceAndOnlyOnce principle over the caveats on gotos.
    // The gotos only jump forward and only to the exit paths of the function

    // The old bin block scheme has been replaced by 4 x 32 bit words so that each bin has a bit
    // and the next bin is found using a count leading zeroes instruction. Much faster.
    // Also now each bin's flag can be kept accurate. This simplifies the searching code quite a bit.

    // Also fwiw, changed 'victim' in the original code to 'candidate'. 'victim' just bothered me.


    AllocChunkPtr candidate; /* inspected/selected chunk */
    size_t candidate_size; /* its size */
    AllocChunkPtr remainder; /* remainder from a split */
    int32 remainder_size; /* its size */
    AllocAreaPtr area;
    size_t areaSize;

    size_t size = RequestToSize(inReqSize);
    int index = BinIndex(size);
    assert(index < 128);
    AllocChunkPtr bin = mBins + index;

    check_pool();

    /* Check for exact match in a bin */
    if (index < kMaxSmallBin) { /* Faster version for small requests */
        /* No traversal or size check necessary for small bins.  */
        candidate = bin->Prev();

        /* Also scan the next one, since it would have a remainder < kMinAllocSize */
        if (candidate == bin)
            candidate = (++bin)->Prev();
        if (candidate != bin) {
            candidate_size = candidate->Size();
            goto found_exact_fit;
        }

        index += 2; /* Set for bin scan below. We've already scanned 2 bins. */
    } else {
        for (candidate = bin->Prev(); candidate != bin; candidate = candidate->Prev()) {
            candidate_size = candidate->Size();
            remainder_size = (int)(candidate_size - size);
            if (remainder_size >= (int32)kMinAllocSize) { /* too big */
                --index; /* adjust to rescan below after checking last remainder */
                break;
            } else if (remainder_size >= 0) { /* exact fit */
                goto found_exact_fit;
            }
        }
        ++index;
    }

    for (; (index = NextFullBin(index)) >= 0; ++index) {
        bin = mBins + index;

        /* Find and use first big enough chunk ... */
        for (candidate = bin->Prev(); candidate != bin; candidate = candidate->Prev()) {
            candidate_size = candidate->Size();
            remainder_size = (int)(candidate_size - size);
            if (remainder_size >= (int32)kMinAllocSize) { /* split */
                UnlinkFree(candidate);
                goto found_bigger_fit;
            } else if (remainder_size >= 0)
                goto found_exact_fit;
        }
    }
    check_pool();

    if (mAreaMoreSize == 0) { /* pool has a non-growable area */
        if (mAreas != NULL /* fixed size area exhausted */
            || size > mAreaInitSize) /* too big anyway */
            goto found_nothing;
        areaSize = mAreaInitSize;
        goto split_new_area;
    }

    if (size > mAreaMoreSize) {
        areaSize = size;
        goto whole_new_area;
    } else {
        areaSize = mAreaMoreSize;
        goto split_new_area;
    }

// exit paths:
found_nothing:
    // ipostbuf("alloc failed. size: %d\n", inReqSize);
    throw std::runtime_error(std::string("alloc failed, increase server's memory allocation (e.g. via ServerOptions)"));

whole_new_area:
    // ipostbuf("whole_new_area\n");
    area = NewArea(areaSize);
    if (!area)
        return 0;
    candidate = &area->mChunk;
    candidate_size = candidate->Size();
    goto return_chunk;

split_new_area:
    // ipostbuf("split_new_area\n");
    area = NewArea(areaSize);
    if (!area)
        return 0;
    candidate = &area->mChunk;
    candidate_size = candidate->Size();
    remainder_size = (int)(areaSize - size);
    //	FALL THROUGH
found_bigger_fit:
    // ipostbuf("found_bigger_fit\n");
    remainder = candidate->ChunkAtOffset(size);
    remainder->SetSizeFree(remainder_size);
    candidate_size -= remainder_size;
    LinkFree(remainder);
    goto return_chunk;

found_exact_fit:
    check_pool();
    UnlinkFree(candidate);
    //	FALL THROUGH
return_chunk:

    candidate->SetSizeInUse(candidate_size);
    check_malloced_chunk(candidate, candidate_size);
    check_pool();
    garbage_fill(candidate);
    return candidate->ToPtr();
}


void* AllocPool::Realloc(void* inPtr, size_t inReqSize) {
#ifdef DISABLE_MEMORY_POOLS
    return realloc(inPtr, inReqSize);
#endif


    void* outPtr;
    AllocChunkPtr prev;
    check_pool();
    bool docopy = false;

    /* realloc of null is supposed to be same as malloc */
    if (inPtr == 0)
        return Alloc(inReqSize);

    AllocChunkPtr oldChunk = MemToChunk(inPtr);
    AllocChunkPtr newChunk = oldChunk;
    size_t oldsize = oldChunk->Size();
    size_t newsize = oldsize;
    size_t size = RequestToSize(inReqSize);
    size_t nextsize, prevsize;
    check_inuse_chunk(oldChunk);

    if (oldsize < size) {
        /* Try expanding forward */
        AllocChunkPtr next = oldChunk->NextChunk();
        if (!next->InUse()) {
            nextsize = next->Size();
            /* Forward into next chunk */
            if (nextsize + newsize >= size) {
                UnlinkFree(next);
                newsize += nextsize;
                goto split;
            }
        } else {
            next = 0;
            nextsize = 0;
        }


        /* Try shifting backwards. */
        prev = oldChunk->PrevChunk();
        if (!prev->InUse()) {
            prevsize = prev->Size();

            /* try forward + backward first to save a later consolidation */
            if (next != 0) {
                /* into next chunk */
                if (nextsize + prevsize + newsize >= size) {
                    newsize += nextsize + prevsize;
                    UnlinkFree(next);
                    goto alloc_prev;
                }
            }

            /* backward only */
            if (prev != 0 && prevsize + newsize >= size) {
                newsize += prevsize;
                goto alloc_prev;
            }
        }

        /* Must allocate */

        outPtr = Alloc(inReqSize);

        check_pool();
        if (outPtr == 0) {
            // ipostbuf("realloc failed. size: %d\n", inReqSize);
            throw std::runtime_error(
                std::string("realloc failed, increase server's memory allocation (e.g. via ServerOptions)"));
        }

        /* Otherwise copy, free, and exit */
        memcpy(outPtr, inPtr, oldsize - sizeof(AllocChunk));
        Free(inPtr);
        return outPtr;
    } else
        goto split;

alloc_prev:
    UnlinkFree(prev);
    newChunk = prev;
    docopy = true;
    // FALL THROUGH
split: /* split off extra room in old or expanded chunk */
    // check_pool();
    if (newsize - size >= kMinAllocSize) { /* split off remainder */
        size_t remainder_size = newsize - size;
        AllocChunkPtr remainder = newChunk->ChunkAtOffset(size);
        remainder->SetSizeInUse(remainder_size);
        newChunk->SetSizeInUse(size);
        Free(remainder->ToPtr()); /* let free() deal with it */
    } else {
        newChunk->SetSizeInUse(newsize);
    }
    outPtr = newChunk->ToPtr();
    if (docopy) {
        memmove(outPtr, inPtr, oldsize - sizeof(AllocChunk));
    }
    check_inuse_chunk(newChunk);
    check_pool();
    garbage_fill(newChunk);
    return outPtr;
}

void AllocPool::LinkFree(AllocChunkPtr inChunk) {
    size_t size = inChunk->Size();
    size_t index = BinIndex(size);

    AllocChunkPtr bin = mBins + index;

    if (index < kNumSmallBins || bin->IsEmpty()) {
        inChunk->InsertAfter(bin);
        MarkBinBlock(index);
    } else {
        AllocChunkPtr link = bin->Next();
        while (link != bin && size < link->Size())
            link = link->Next();
        inChunk->InsertBefore(link);
    }
}

void AllocPool::DoCheckArea(AllocAreaPtr area) {
    assert(area->mChunk.PrevInUse());

    AllocChunkPtr p = &area->mChunk;
    while (p->mSize != kChunkInUse) {
        if (p->InUse()) {
            DoCheckInUseChunk(p);
        } else {
            DoCheckFreeChunk(p);
        }
        p = p->NextChunk();
    }
}

void AllocPool::DoCheckBin(AllocChunkPtr bin, long index) {
    AllocChunkPtr p = bin->Next();

    while (p != bin) {
        assert(BinIndex(p->Size()) == index);
        DoCheckFreeChunk(p);
        p = p->Next();
    }
}


void AllocPool::DoCheckPool() {
    AllocAreaPtr area = mAreas;
    if (area) {
        do {
            AllocAreaPtr nextarea = area->mNext;
            DoCheckArea(area);
            area = nextarea;
        } while (area != mAreas);
    }

    for (int i = 0; i < kNumAllocBins; ++i) {
        AllocChunkPtr b = mBins + i;
        DoCheckBin(b, i);
    }
}


void AllocPool::DoCheckChunk(AllocChunkPtr p) {
#ifndef NDEBUG
    size_t size = p->Size();
    // size_t maxsize = mAreaInitSize > mAreaMoreSize ? mAreaInitSize : mAreaMoreSize;
    // assert(size < maxsize);

    AllocChunkPtr next = p->ChunkAtOffset(size);
#endif
    assert(p->mSize == next->mPrevSize);
}


void AllocPool::DoCheckFreeChunk(AllocChunkPtr p) {
    size_t size = p->Size();
#ifndef NDEBUG
    AllocChunkPtr next = p->ChunkAtOffset(size);
#endif
    DoCheckChunk(p);

    /* Check whether it claims to be free ... */
    assert(!p->InUse());

    /* Unless an end marker, must have OK fields */
    if (size >= kMinAllocSize) {
        assert((size & kAlignMask) == 0);
        assert(aligned_OK(p->ToPtr()));
        /* ... and is fully consolidated */
        assert(p->PrevInUse());
        assert(next->InUse());

        /* ... and has minimally sane links */
        assert(p->Next()->Prev() == p);
        assert(p->Prev()->Next() == p);
    } else /* end markers are always of size 0 */
        assert(size == 0);
}

void AllocPool::DoCheckInUseChunk(AllocChunkPtr p) {
    size_t size = p->Size();
    AllocChunkPtr next = p->NextChunk();

    DoCheckChunk(p);

    /* Check whether it claims to be in use ... */
    assert(p->InUse());

    /* ... and is surrounded by OK chunks.
    Since more things can be checked with free chunks than inuse ones,
    if an inuse chunk borders them and debug is on, it's worth doing them.
    */
    if (!p->PrevInUse()) {
        size_t prevsize = p->PrevSize();
        if (prevsize > 0) {
            AllocChunkPtr prv = p->PrevChunk();
            assert(prv->NextChunk() == p);
            DoCheckFreeChunk(prv);
        }
    }
    if (!p->ChunkAtOffset(size)->InUse()) {
        DoCheckFreeChunk(next);
    }
}

void AllocPool::DoCheckAllocedChunk(AllocChunkPtr p, size_t s) {
#ifndef NDEBUG
    size_t size = p->Size();
    long room = size - s;
#endif

    DoCheckInUseChunk(p);

    /* Legal size ... */
    assert(size >= kMinAllocSize);
    assert((size & kAlignMask) == 0);
    assert(room >= 0);
    assert(room < kMinAllocSize);

    /* ... and alignment */
    assert(aligned_OK(p->ToPtr()));


    /* ... and was allocated at front of an available chunk */
    assert(p->PrevInUse()); // huh??  - jmc
}

void AllocPool::DoGarbageFill(AllocChunkPtr p) {
    long size = (p->Size() - sizeof(AllocChunk));
    DoGarbageFill(p, size);
}

void AllocPool::DoGarbageFill(AllocChunkPtr p, long size) {
    size /= sizeof(long);
    long* ptr = (long*)p->ToPtr();
    for (int i = 0; i < size; ++i) {
        ptr[i] = 0xA3A56955;
    }
}
