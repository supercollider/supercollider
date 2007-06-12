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

#ifndef _MsgFifo_
#define _MsgFifo_

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
		long next = NextPos(mWriteHead);
		if (next == mFreeHead) return false; // fifo is full
		mItems[next] = data;
		mWriteHead = next;
		return true;
	}

	void Perform() // get next and advance
	{
		while (HasData()) {
			long next = NextPos(mReadHead);
			mItems[next].Perform();
			mReadHead = next;
		}
	}
	void Free() // reclaim messages
	{
		while (NeedsFree()) {
			long next = NextPos(mFreeHead);
			mItems[next].Free();
			mFreeHead = next;
		}
	}

private:
	int NextPos(int inPos) { return (inPos + 1) & (N - 1); }

	volatile int mReadHead, mWriteHead, mFreeHead;
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
		long next = NextPos(mWriteHead);
		if (next == mReadHead) return false; // fifo is full
		mItems[next] = data;
		mWriteHead = next;
		return true;
	}

	void Perform() // get next and advance
	{
		while (HasData()) {
			long next = NextPos(mReadHead);
			mItems[next].Perform();
			mReadHead = next;
		}
	}

private:
	int NextPos(int inPos) { return (inPos + 1) & (N - 1); }

	volatile int mReadHead, mWriteHead;
	MsgType mItems[N];
};

/////////////////////////////////////////////////////////////////////


#endif


