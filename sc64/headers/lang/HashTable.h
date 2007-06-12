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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _HashTable_
#define _HashTable_

#include "SC_Types.h"
#include "SC_BoundsMacros.h"
#include "Hash.h"
#include <stddef.h>

template<class T, class Allocator, class KeyType>
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
		//printf("GetKey(inItem) %s\n", GetKey(inItem));
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
		if (mItems[index] != inItem) return false;
		mItems[index] = 0;
		
		FixCollisionsFrom(index);
		mNumItems--;
		return true;
	}
        
	int32 IndexFor(int32 inHashID, KeyType inKey) const
	{
		int index = inHashID & mHashMask;
		for(;;) {
			T *item = mItems[index];
			if (!item) return index;
                        if (GetHash(item) == inHashID 
                            && strcmp(inKey, GetKey(item)) == 0) return index;
			index = (index + 1) & mHashMask;
		}
	}

	T* Get(KeyType inKey) const
        {
            return Get(Hash(inKey), inKey);
        }

	T* Get(int32 inHashID, KeyType inKey) const
	{
		int32 index = IndexFor(inHashID, inKey);
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
			int newIndex = IndexFor(GetHash(oldItem), GetKey(oldItem));
			if (oldIndex != newIndex) {
				mItems[oldIndex] = mItems[newIndex];
				mItems[newIndex] = oldItem;
			}
		}
	}
};

template<class T, int kMaxItems, class KeyType>
class StaticHashTable 
{
	int32 mNumItems, mTableSize, mHashMask;
	T* mItems[kMaxItems*2];

public:

	StaticHashTable() 
	{
		mNumItems = 0;
		mTableSize = kMaxItems << 1;
		ClearTable();
		mHashMask = mTableSize - 1;
	}
	
	~StaticHashTable() {
	}
	
	int32 TableSize() const { return mTableSize; }
	int32 MaxItems() const { return kMaxItems; }
	int32 NumItems() const { return mNumItems; }
	
	void ClearTable()
	{
		for (int i=0; i<mTableSize; ++i) {
			mItems[i] = 0;
		}
	}

	bool Add(T* inItem)
	{
		if (mNumItems >= kMaxItems) return false;
		
		int32 index = IndexFor(GetHash(inItem), GetKey(inItem));
		
		T *item = mItems[index];
		if (item) return item == inItem;
		
		mItems[index] = inItem;
		mNumItems++;
		return true;	
	}
	
	bool Remove(T* inItem)
	{
		int32 index = IndexFor(GetHash(inItem), GetKey(inItem));
		if (mItems[index] != inItem) return false;
		mItems[index] = 0;
		
		FixCollisionsFrom(index);
		mNumItems--;
		return true;
	}
        
	int32 IndexFor(int32 inHashID, KeyType inKey) const
	{
		int index = inHashID & mHashMask;
		for(;;) {
			T *item = mItems[index];
			if (!item) return index;
                        if (GetHash(item) == inHashID 
                            && strcmp(inKey, GetKey(item)) == 0) return index;
			index = (index + 1) & mHashMask;
		}
	}

	T* Get(KeyType inKey) const
        {
            return Get(Hash(inKey), inKey);
        }
        
	T* Get(int32 inHashID, KeyType inKey) const
	{
		int32 index = IndexFor(inHashID, inKey);
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
			int newIndex = IndexFor(GetHash(oldItem), GetKey(oldItem));
			if (oldIndex != newIndex) {
				mItems[oldIndex] = mItems[newIndex];
				mItems[newIndex] = oldItem;
			}
		}
	}
};


#endif
