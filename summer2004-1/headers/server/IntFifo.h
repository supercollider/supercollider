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

#ifndef _IntFifo_
#define _IntFifo_

#ifdef SC_DARWIN
# include <CoreServices/CoreServices.h>
#endif

template <int N>
class IntFifo
{
public:
	IntFifo()
			: mMask(N - 1), mReadHead(0), mWriteHead(0)
	{}

	void MakeEmpty() { mReadHead = mWriteHead; }
	bool IsEmpty() { return mReadHead == mWriteHead; }
	bool HasData() { return mReadHead != mWriteHead; }

	bool Put(int data)
	{
		long next = NextPos(mWriteHead);
		if (next == mReadHead) return false; // fifo is full
		mItems[next] = data;
#ifdef SC_DARWIN
		// we don't really need a compare and swap, but this happens to call 
		// the PowerPC memory barrier instruction lwsync.
		CompareAndSwap(mWriteHead, next, &mWriteHead);
#elif defined SC_WIN32
    InterlockedExchange(reinterpret_cast<volatile LONG*>(&mWriteHead),next);
#else
		mWriteHead = next;
#endif
		return true;
	}

	int32 Get()
	{
		//assert(HasData());
		long next = NextPos(mReadHead);
		out = mItems[next].Perform();
#ifdef SC_DARWIN
		// we don't really need a compare and swap, but this happens to call 
		// the PowerPC memory barrier instruction lwsync.
		CompareAndSwap(mReadHead, next, &mReadHead);
#elif defined SC_WIN32
    InterlockedExchange(reinterpret_cast<volatile LONG*>(&mReadHead),next);
#else
		mReadHead = next;
#endif
	}

private:
	int NextPos(int inPos) { return (inPos + 1) & mMask; }

	long mMask;
#ifdef SC_DARWIN
	UInt32 mReadHead, mWriteHead;
#else
	volatile int mReadHead, mWriteHead;
#endif
	int32 mItems[N];
};

#endif


