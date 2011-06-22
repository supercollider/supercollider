
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
This is based on Doug Lea's allocator but rewritten so I can read and understand it...
also features of free all-at-once pools are added.
Now uses 16 byte alignment, which does increase the minimum allocation size to 32 bytes
including the overhead.
Improved bit block scanning by using a count leading zeroes instruction.

*/

#ifndef _AllocPool_
#define _AllocPool_

#include "SC_List.h"
#include "clz.h"
#include <stdlib.h>

const int kNumAllocBins = 128;
const int kNumSmallBins = 64;
const int kMaxSmallBin = kNumSmallBins - 1;
const int kBinWidth = 8;
const int kMaxSmallBinSize = kNumSmallBins * kBinWidth;
const int kBinBlockWidth = 4;
const int kBinBlockMask = kBinBlockWidth - 1;

const size_t kAlign = 64;
const size_t kAlignMask = kAlign - 1;
const size_t kChunkFree = 0;
const size_t kChunkInUse = 1;
const size_t kSizeBits = ~kChunkInUse;

class AllocChunk;
class AllocPool;
typedef AllocChunk *AllocChunkPtr;
typedef Link<AllocChunk> AllocBin;
typedef AllocBin* AllocBinPtr;

class AllocChunk : public Link<AllocChunk>
{
	friend class AllocPool;

	size_t Size()
		{ return mSize & kSizeBits; }

	size_t PrevSize()
		{ return mPrevSize & kSizeBits; }

	AllocChunkPtr ChunkAtOffset(long inSize)
		{ return AllocChunkPtr((char*)this + inSize); }

	AllocChunkPtr NextChunk()
		{ return ChunkAtOffset(Size()); }

	AllocChunkPtr PrevChunk()
		{ return ChunkAtOffset(-(long)PrevSize()); }

	bool InUse()
		{ return (bool)(mSize & kChunkInUse); }

	bool PrevInUse()
		{ return (bool)(mPrevSize & kChunkInUse); }

	void SetSizeFree(size_t inSize)
		{ mSize = ChunkAtOffset(inSize)->mPrevSize = inSize; }

	void SetSizeInUse(size_t inSize)
		{ mSize = ChunkAtOffset(inSize)->mPrevSize = inSize | kChunkInUse; }

	void SetNeighborsInUse(size_t inOffset)
		{ mPrevSize = ChunkAtOffset(inOffset)->mSize = kChunkInUse; }

	bool IsArea()
		{ return mPrevSize == kChunkInUse && NextChunk()->mSize == kChunkInUse; }

	void* ToPtr()
		{ return (void*)((char*)this + sizeof(AllocChunk)); }

	size_t mPrevSize;
	size_t mSize;
};

class AllocArea;
typedef AllocArea* AllocAreaPtr;

class AllocAreaHdr /* for size calculations */
{
protected:
	friend class AllocPool;

	AllocAreaPtr mPrev, mNext;
	size_t mSize;
	void *mUnalignedPointerToThis;
};

class AllocArea : public AllocAreaHdr
{
public:
	void SanityCheck();

private:
	friend class AllocPool;

	AllocChunk mChunk;
};

const size_t kMinAllocSize = 2 * kAlign;
const size_t kAreaOverhead = sizeof(AllocAreaHdr) + 2 * sizeof(AllocChunk) + kAlign;

typedef void* (*NewAreaFunc)(size_t size);
typedef void (*FreeAreaFunc)(void *);

class AllocPool
{
public:
	AllocPool(NewAreaFunc allocArea, FreeAreaFunc freeArea,
					size_t areaInitSize, size_t areaMoreSize);
	~AllocPool();

	void Reinit();

	void *Alloc(size_t inBytes);
	void *Realloc(void* inPtr, size_t inBytes);
	void Free(void* inPtr);
	void FreeAll();
	void FreeAllInternal();

	// debugging
	size_t TotalFree();
	size_t LargestFreeChunk();

	void DoCheckPool();
	void DoCheckInUseChunk(AllocChunkPtr p);

	static AllocChunkPtr MemToChunk(void *inPtr)
		{ return (AllocChunkPtr)((char*)(inPtr) - sizeof(AllocChunk)); }

private:
	void InitAlloc();
	void InitBins();
	AllocAreaPtr NewArea(size_t inAreaSize);
	void FreeArea(AllocChunkPtr chunk);

	// debugging
	void DoCheckArea(AllocAreaPtr area);
	void DoCheckBin(AllocChunkPtr bin, long idx);
	void DoCheckChunk(AllocChunkPtr p);
	void DoCheckFreeChunk(AllocChunkPtr p);
	void DoCheckAllocedChunk(AllocChunkPtr p, size_t s);
	void DoGarbageFill(AllocChunkPtr p);
	void DoGarbageFill(AllocChunkPtr p, long size);

	// inlines

	static size_t RequestToSize(size_t inReqSize)
	{
		size_t sizePlusOverhead = inReqSize + sizeof(AllocChunk);
		if (sizePlusOverhead <= kMinAllocSize) return kMinAllocSize;
		else return (sizePlusOverhead + kAlignMask) & ~kAlignMask;
	}

	static int SmallBinIndex(size_t inSize)
		{ return inSize >> 4; }

	static int BinIndex2(size_t inSize)
	{
		return
		((inSize <   1024) ?       (inSize>>4):
		 (inSize <   2048) ?  56 + (inSize>>7):
		 (inSize <   4096) ?  64 + (inSize>>8):
		 (inSize <   8192) ?  72 + (inSize>>9):
		 (inSize <  16384) ?  80 + (inSize>>10):
		 (inSize <  32768) ?  88 + (inSize>>11):
		 (inSize <  65536) ?  96 + (inSize>>12):
		 (inSize < 131072) ? 104 + (inSize>>13):
		 (inSize < 262144) ? 112 + (inSize>>14):127);
	}

	static int BinIndex(size_t inSize)
	{
		if (inSize < 1024) return inSize >> 4;
		if (inSize >= 262144) return 127;
		int bits = 28 - CLZ(inSize);
		return (bits << 3) + (inSize >> bits) ;
	}

	void MarkBinBlock(int inIndex)
		{
			unsigned long word = inIndex >> 5;
			unsigned long bitPosition = inIndex & 31;
			unsigned long bitValue = 1L << bitPosition;
			mBinBlocks[word] |= bitValue;
		}

	void ClearBinBlock(int inIndex)
		{
			unsigned long word = inIndex >> 5;
			unsigned long bitPosition = inIndex & 31;
			unsigned long bitValue = 1L << bitPosition;
			mBinBlocks[word] &= ~bitValue;
		}

	int NextFullBin(int inStartingBinIndex)
		{
			if (inStartingBinIndex >= 128) return -1;
			int word = inStartingBinIndex >> 5;
			int bitPosition = inStartingBinIndex & 31;
			unsigned long bitValue = 1L << bitPosition;
			unsigned long binBits = mBinBlocks[word];

			if (binBits >= bitValue) {
				binBits = (~(bitValue - 1) & binBits);
			} else {
				for (++word; word<4 && !mBinBlocks[word]; ++word) {}
				if (word==4) return -1;
				binBits = mBinBlocks[word];
			}
			bitPosition = CTZ((int32)binBits);

			return (word << 5) + bitPosition;
		}

	void LinkFree(AllocChunkPtr inChunk);
	/*
		{
			size_t size = inChunk->Size();
			int index = BinIndex(size);

			AllocChunkPtr bin = mBins + index;

			if (index < kNumSmallBins || bin->IsEmpty()) {
				inChunk->InsertAfter(bin);
				MarkBinBlock(index);
			} else {
				AllocChunkPtr link = bin->Next();
				while (link != bin && size < link->Size()) link = link->Next();
				inChunk->InsertBefore(link);
			}
		}
	*/

	void UnlinkFree(AllocChunkPtr inChunk)
		{
			inChunk->RemoveLeaveDangling();
			size_t size = inChunk->Size();
			int index = BinIndex(size);
			AllocChunkPtr bin = mBins + index;
			if (bin->IsEmpty()) ClearBinBlock(index);
		}

	AllocChunk mBins[kNumAllocBins];
	AllocAreaPtr mAreas;
	NewAreaFunc mAllocArea;
	FreeAreaFunc mFreeArea;
	size_t mAreaInitSize, mAreaMoreSize;
	unsigned long mBinBlocks[4];
};

#endif
