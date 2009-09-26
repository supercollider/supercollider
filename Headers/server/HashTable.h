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

#ifndef _HashTable_
#define _HashTable_

#include "SC_Types.h"
#include "SC_BoundsMacros.h"
#include "SC_Str4.h"
#include "Hash.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

template<class T, class Allocator>
class HashTable
{
	Allocator *mPool;
	int32 mNumItems, mMaxItems, mTableSize, mHashMask;
	T** mItems;
	bool mCanResize;

public:

	HashTable(Allocator *inPool, int32 inMaxItems, bool inCanResize = true)
		: mPool(inPool)
	{
		mNumItems = 0;
		mMaxItems = inMaxItems;
		mTableSize = mMaxItems << 1;
		mItems = AllocTable(mTableSize);
		mHashMask = mTableSize - 1;
		mCanResize = inCanResize;
	}

	~HashTable() {
		mPool->Free(mItems);
	}

	int32 TableSize() const { return mTableSize; }
	int32 MaxItems() const { return mMaxItems; }
	int32 NumItems() const { return mNumItems; }

	T** AllocTable(int inTableSize)
	{
		size_t size = inTableSize * sizeof(T*);
		T** items = static_cast<T**>(mPool->Alloc(size));
		for (int i=0; i<inTableSize; ++i) {
			items[i] = 0;
		}
		return items;
	}

	void MakeEmpty()
	{
		for (int i=0; i<mTableSize; ++i) {
			mItems[i] = 0;
		}
		mNumItems = 0;
	}

	void Resize()
	{
		int32 newSize = sc_max(mTableSize << 1, 32);
		int32 oldSize = mTableSize;
		T** oldItems = mItems;
		mItems = AllocTable(newSize);
		mTableSize = newSize;
		mMaxItems = mTableSize >> 1;
		mHashMask = mTableSize - 1;
		mNumItems = 0;
		for (int i=0; i<oldSize; ++i) {
			T* item = oldItems[i];
			if (item) Add(item);
		}
		mPool->Free(oldItems);
		//printf("mMaxItems %d   mTableSize %d   newSize %d\n", mMaxItems, mTableSize, newSize);
	}

	bool Add(T* inItem)
	{
		//printf("mNumItems %d\n", mNumItems);
		//printf("mMaxItems %d\n", mMaxItems);
		//printf("mCanResize %d\n", mCanResize);
		if (mNumItems >= mMaxItems) {
			if (!mCanResize) return false;
			Resize();
		}

		//printf("GetHash(inItem) %d\n", GetHash(inItem));
		//printf("GetKey(inItem) %s\n", GetKey(inItem));
		int32 index = IndexFor(GetHash(inItem), (int32*)GetKey(inItem));
		//printf("index %d\n", index);

		T *item = mItems[index];
		if (item) return item == inItem;

		mItems[index] = inItem;
		mNumItems++;
		return true;
	}

	bool Remove(T* inItem)
	{
		int32 index = IndexFor(GetHash(inItem), (int32*)GetKey(inItem));
		if (mItems[index] != inItem) return false;
		mItems[index] = 0;

		FixCollisionsFrom(index);
		mNumItems--;
		return true;
	}

	bool RemoveKey(int32* inKey)
	{
		T* item = Get(inKey);
		if (!item) return false;
		return Remove(item);
	}

	int32 IndexFor(int32 inHashID, int32* inKey) const
	{
		int index = inHashID & mHashMask;
		for(;;) {
			T *item = mItems[index];
			if (!item) return index;
            if (GetHash(item) == inHashID && str4eq(inKey, GetKey(item))) return index;
			index = (index + 1) & mHashMask;
		}
	}

	T* Get(int32* inKey) const
        {
            return Get(Hash(inKey), inKey);
        }

	T* Get(int32 inHashID, int32* inKey) const
	{
        //printf("Get hash %d %s\n", inHashID, inKey);
		int32 index = IndexFor(inHashID, inKey);
		//printf("index %d\n", index);
		return mItems[index];
	}

	bool Includes(T* inItem) const
	{
		return Get(GetHash(inItem), GetKey(inItem)) == inItem;
	}

	T* AtIndex(int32 inIndex) const
	{
		return mItems[inIndex];
	}

private:
	void FixCollisionsFrom(int32 inIndex)
	{
		int oldIndex = inIndex;
		for (;;) {
			oldIndex = (oldIndex + 1) & mHashMask;
			T *oldItem = mItems[oldIndex];
			if (!oldItem) break;
			int newIndex = IndexFor(GetHash(oldItem), (int32*)GetKey(oldItem));
			if (oldIndex != newIndex) {
				mItems[oldIndex] = mItems[newIndex];
				mItems[newIndex] = oldItem;
			}
		}
	}
};


template<class T, class Allocator>
class IntHashTable
{
	Allocator *mPool;
	int32 mNumItems, mMaxItems, mTableSize, mHashMask;
	T** mItems;
	bool mCanResize;

public:

	IntHashTable(Allocator *inPool, int32 inMaxItems, bool inCanResize = true)
		: mPool(inPool)
	{
		mNumItems = 0;
		mMaxItems = inMaxItems;
		mTableSize = mMaxItems << 1;
		mItems = AllocTable(mTableSize);
		mHashMask = mTableSize - 1;
		mCanResize = inCanResize;
	}

	~IntHashTable() {
		mPool->Free(mItems);
	}

	int32 TableSize() const { return mTableSize; }
	int32 MaxItems() const { return mMaxItems; }
	int32 NumItems() const { return mNumItems; }

	T** AllocTable(int inTableSize)
	{
		size_t size = inTableSize * sizeof(T*);
		T** items = static_cast<T**>(mPool->Alloc(size));
		for (int i=0; i<inTableSize; ++i) {
			items[i] = 0;
		}
		return items;
	}

	void Resize()
	{
		int32 newSize = sc_max(mTableSize << 1, 32);
		T** oldItems = mItems;
		mItems = AllocTable(newSize);
		for (int i=0; i<mTableSize; ++i) {
			T* item = oldItems[i];
			if (item) Add(item);
		}
		mTableSize = newSize;
		mMaxItems = mTableSize >> 1;
		mHashMask = mTableSize - 1;
		mPool->Free(oldItems);
		//printf("mMaxItems %d   mTableSize %d   newSize %d\n", mMaxItems, mTableSize, newSize);
	}

	bool Add(T* inItem)
	{
		//printf("mNumItems %d\n", mNumItems);
		//printf("mMaxItems %d\n", mMaxItems);
		//printf("mCanResize %d\n", mCanResize);
		if (mNumItems >= mMaxItems) {
			if (!mCanResize) return false;
			Resize();
		}

		//printf("GetHash(inItem) %d\n", GetHash(inItem));
		//printf("GetKey(inItem) %d\n", GetKey(inItem));
		int32 index = IndexFor(GetHash(inItem), GetKey(inItem));
		//printf("index %d\n", index);

		T *item = mItems[index];
		if (item) return item == inItem;

		mItems[index] = inItem;
		mNumItems++;
		return true;
	}

	bool Remove(T* inItem)
	{
		int32 index = IndexFor(GetHash(inItem), GetKey(inItem));
		//printf("rmv index %d hash %d key %d\n", index, GetHash(inItem), GetKey(inItem));
		if (mItems[index] != inItem) return false;
		mItems[index] = 0;

		FixCollisionsFrom(index);
		mNumItems--;
		return true;
	}

	bool RemoveKey(int32 inKey)
	{
		T* item = Get(inKey);
		if (!item) return false;
		return Remove(item);
	}

	int32 IndexFor(int32 inHashID, int32 inKey) const
	{
		int index = inHashID & mHashMask;
		for(;;) {
			T *item = mItems[index];
			if (!item) return index;
            if (GetHash(item) == inHashID && inKey == GetKey(item)) return index;
			index = (index + 1) & mHashMask;
		}
	}

	T* Get(int32 inKey) const
        {
        	//printf("Get key %d\n", inKey);
            return Get(Hash(inKey), inKey);
        }

	T* Get(int32 inHashID, int32 inKey) const
	{
		int32 index = IndexFor(inHashID, inKey);
        //printf("Get index %d  hash %d  key %d\n", index, inHashID, inKey);
		return mItems[index];
	}

	bool Includes(T* inItem) const
	{
		return Get(GetHash(inItem), GetKey(inItem)) == inItem;
	}

	T* AtIndex(int32 inIndex) const
	{
		return mItems[inIndex];
	}

	void Dump()
	{
		for (int i=0; i<mTableSize; ++i) {
			T* item = mItems[i];
			if (item) {
				printf("%4d %4d %08X %08X\n", i, GetKey(item), GetHash(item), item);
			}
		}
	}

private:
	void FixCollisionsFrom(int32 inIndex)
	{
		//printf("FixCollisionsFrom %d\n", inIndex);
		int oldIndex = inIndex;
		for (;;) {
			oldIndex = (oldIndex + 1) & mHashMask;
			T *oldItem = mItems[oldIndex];
			if (!oldItem) break;
			int newIndex = IndexFor(GetHash(oldItem), GetKey(oldItem));
			if (oldIndex != newIndex) {
				//printf("swap %d %d\n", oldIndex, newIndex);
				mItems[oldIndex] = mItems[newIndex];
				mItems[newIndex] = oldItem;
			}
		}
	}
};

struct Malloc
{
	void Free(void* ptr) { free(ptr); }
	void* Alloc(size_t size) { return malloc(size); }
};

#endif
