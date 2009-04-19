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

#ifndef SC_FIFO_H_INCLUDED
#define SC_FIFO_H_INCLUDED

#ifdef SC_DARWIN
# include <CoreServices/CoreServices.h>
#endif

template <class T, int N>
class SC_FIFO
{
public:
	SC_FIFO()
		: mMask(N - 1),
		  mReadHead(0),
		  mWriteHead(0)
	{}

	void MakeEmpty() { mReadHead = mWriteHead; }
	bool IsEmpty() { return mReadHead == mWriteHead; }
	bool HasData() { return mReadHead != mWriteHead; }

	bool Put(T data)
	{
		long next = NextPos(mWriteHead);
		if (next == mReadHead) return false; // fifo is full
		mItems[next] = data;
#ifdef SC_DARWIN
		// we don't really need a compare and swap, but this happens to call 
		// the PowerPC memory barrier instruction lwsync.
		CompareAndSwap(mWriteHead, next, &mWriteHead);
#elif defined SC_WIN32 && !defined(__MINGW32__)
		InterlockedExchange(reinterpret_cast<volatile LONG*>(&mWriteHead),next);
#else
		mWriteHead = next;
#endif
		return true;
	}

	T Get()
	{
		//assert(HasData());
		long next = NextPos(mReadHead);
		T out = mItems[next];
#ifdef SC_DARWIN
		// we don't really need a compare and swap, but this happens to call 
		// the PowerPC memory barrier instruction lwsync.
		CompareAndSwap(mReadHead, next, &mReadHead);
#elif defined SC_WIN32 && !defined(__MINGW32__)
		InterlockedExchange(reinterpret_cast<volatile LONG*>(&mReadHead),next);
#else
		mReadHead = next;
#endif
		return out;
	}

	int Capacity() const { return N; }

private:
	int NextPos(int inPos) { return (inPos + 1) & mMask; }

	long mMask;
#ifdef SC_DARWIN
	UInt32 mReadHead, mWriteHead;
#else
	volatile int mReadHead, mWriteHead;
#endif
	T mItems[N];
};

#endif // SC_FIFO_H_INCLUDED
