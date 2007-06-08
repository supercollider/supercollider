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


#ifndef _SC_UnorderedList_
#define _SC_UnorderedList_

template <class T, int kMaxItems>
class SC_UnorderedList 
{
    T* mList[kMaxItems];
    unsigned int mSize;
public:
    SC_UnorderedList() : mSize(0) {}
    
    unsigned int Size() const { return mSize; }
    Synth* GetAt(unsigned int inIndex) 
    {
        if (inIndex >= mSize) return 0;
        return mList[inIndex];
    }
    
    void Add(T *inItem) 
    {
        if (mSize < kMaxItems) {
            mList[mSize] = inItem; 
            SetListIndex(inItem, mSize++);
        }
    }
    
    void Remove(T *inItem) 
    { 
        --mSize;
        unsigned int index = GetListIndex(inItem);
        if (index < mSize) {
            mList[index] = mList[mSize];
            SetListIndex(mList[index], index);
        }
    }
};

#endif

