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

#ifndef _MsgFifo_
#define _MsgFifo_

#ifdef __APPLE__
# include <libkern/OSAtomic.h>
#endif

#ifdef _WIN32
# include <windows.h>
#endif

/////////////////////////////////////////////////////////////////////

template <class MsgType, int N>
class MsgFifo
{
public:
	MsgFifo()
			: mReadHead(0), mWriteHead(0), mFreeHead(0)
	{}

	void MakeEmpty() { mFreeHead = mReadHead = mWriteHead; }
	bool IsEmpty() { return mReadHead == mWriteHead; }
	bool HasData() { return mReadHead != mWriteHead; }
	bool NeedsFree() { return mFreeHead != mReadHead; }

	bool Write(MsgType& data)
	{
		unsigned int next = NextPos(mWriteHead);
		if (next == mFreeHead) return false; // fifo is full
		mItems[next] = data;
#ifdef __APPLE__
		// we don't really need a compare and swap, but this happens to call
		// the PowerPC memory barrier instruction lwsync.
		OSAtomicCompareAndSwap32Barrier(mWriteHead, next, &mWriteHead);
#elif defined(_WIN32)
		InterlockedExchange(reinterpret_cast<volatile LONG*>(&mWriteHead),next);
#else
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) && ( !defined(__INTEL_COMPILER) || defined(__ia64) )
		__sync_synchronize();
#endif
		mWriteHead = next;
#endif
		return true;
	}

	void Perform() // get next and advance
	{
		while (HasData()) {
			unsigned int next = NextPos(mReadHead);
			mItems[next].Perform();
#ifdef __APPLE__
			// we don't really need a compare and swap, but this happens to call
			// the PowerPC memory barrier instruction lwsync.
			OSAtomicCompareAndSwap32Barrier(mReadHead, next, &mReadHead);
#elif defined(_WIN32)
      InterlockedExchange(reinterpret_cast<volatile LONG*>(&mReadHead),next);
#else
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) && ( !defined(__INTEL_COMPILER) || defined(__ia64) )
			__sync_synchronize();
#endif
			mReadHead = next;
#endif
		}
	}
	void Free() // reclaim messages
	{
		while (NeedsFree()) {
			unsigned int next = NextPos(mFreeHead);
			mItems[next].Free();
#ifdef __APPLE__
			// we don't really need a compare and swap, but this happens to call
			// the PowerPC memory barrier instruction lwsync.
			OSAtomicCompareAndSwap32Barrier(mFreeHead, next, &mFreeHead);
#elif defined(_WIN32)
			InterlockedExchange(reinterpret_cast<volatile LONG*>(&mFreeHead),next);
#else
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) && ( !defined(__INTEL_COMPILER) || defined(__ia64) )
			__sync_synchronize();
#endif
			mFreeHead = next;
#endif
		}
	}

private:
	int NextPos(int inPos) { return (inPos + 1) & (N - 1); }

#ifdef __APPLE__
	int32_t mReadHead, mWriteHead, mFreeHead;
#else
	volatile int mReadHead, mWriteHead, mFreeHead;
#endif
	MsgType mItems[N];
};

/////////////////////////////////////////////////////////////////////

template <class MsgType, int N>
class MsgFifoNoFree
{
public:
	MsgFifoNoFree()
			: mReadHead(0), mWriteHead(0)
	{
	}

	void MakeEmpty() { mReadHead = mWriteHead; }
	bool IsEmpty() { return mReadHead == mWriteHead; }
	bool HasData() { return mReadHead != mWriteHead; }

	bool Write(MsgType& data)
	{
		unsigned int next = NextPos(mWriteHead);
		if (next == mReadHead) return false; // fifo is full
		mItems[next] = data;
#ifdef __APPLE__
			// we don't really need a compare and swap, but this happens to call
			// the PowerPC memory barrier instruction lwsync.
			OSAtomicCompareAndSwap32Barrier(mWriteHead, next, &mWriteHead);
#elif defined(_WIN32)
			InterlockedExchange(reinterpret_cast<volatile LONG*>(&mWriteHead),next);
#else
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) && ( !defined(__INTEL_COMPILER) || defined(__ia64) )
			__sync_synchronize();
#endif
			mWriteHead = next;
#endif
		return true;
	}

	void Perform() // get next and advance
	{
		while (HasData()) {
			unsigned int next = NextPos(mReadHead);
			mItems[next].Perform();
#ifdef __APPLE__
			// we don't really need a compare and swap, but this happens to call
			// the PowerPC memory barrier instruction lwsync.
			OSAtomicCompareAndSwap32Barrier(mReadHead, next, &mReadHead);
#elif defined(_WIN32)
			InterlockedExchange(reinterpret_cast<volatile LONG*>(&mReadHead),next);
#else
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 1) && ( !defined(__INTEL_COMPILER) || defined(__ia64) )
			__sync_synchronize();
#endif
			mReadHead = next;
#endif
		}
	}

private:
	int NextPos(int inPos) { return (inPos + 1) & (N - 1); }
#ifdef __APPLE__
	int32_t mReadHead, mWriteHead;
#else
	volatile int mReadHead, mWriteHead;
#endif
	MsgType mItems[N];
};

/////////////////////////////////////////////////////////////////////


#endif


