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
/*

A Fifo for sending/receiving some type of object.

*/

#ifndef _FIFOT_
#define _FIFOT_

template <class T, int N> class FIFOT
{
public:
	FIFOT()
			: mMask(N-1), mReadHead(0), mWriteHead(0)
		{
		}
	void MakeEmpty() { mReadHead = mWriteHead; }
	bool IsEmpty() { return mReadHead == mWriteHead; }
	int CanGet() { 
		int diff = mWriteHead - mReadHead;
		return diff >= 0 ? diff : N - diff; 
	}
	int CanPut() { return N-1-CanGet(); }
	
	int NextPos(int inPos) { return (inPos + 1) & mMask; }
	
	bool Put(const T& inItem)
		{
			long next = NextPos(mWriteHead); 
			if (next == mReadHead) return false; // fifo is full
			mItems[next] = inItem;
			mWriteHead = next;
			return true;
		}
		
	bool Get(T& outItem) // get next and advance
		{
			if (IsEmpty()) return false;
			long next = NextPos(mReadHead); 
			outItem = mItems[next];
			mReadHead = next;
			return true;
		}
	void DebugDump() 
		{
			post("FIFO N %d  mMask %d  mReadHead %d  mWriteHead%d\n", 
				N, mMask, mReadHead, mWriteHead);
		}
		
private:
	long mMask;
	volatile long mReadHead, mWriteHead;// mReclaimHead;
	T mItems[N];
};

#endif