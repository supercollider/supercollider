/*
    SuperCollider real time audio synthesis system
    Copyright (c) 2002 James McCartney. All rights reserved.
    http://www.audiosynth.com

    Copyright (c) 2013 Tim Blechmann

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

#pragma once

#include <atomic>

/////////////////////////////////////////////////////////////////////

namespace SCMsgFifo {

using std::memory_order_relaxed;
using std::memory_order_release;

template <class MsgType, int N> class MsgFifo {
public:
    MsgFifo(): mReadHead(0), mWriteHead(0), mFreeHead(0) {}

    void MakeEmpty() {
        int writeHead = mWriteHead.load();
        mReadHead.store(writeHead, memory_order_relaxed);
        mFreeHead.store(writeHead, memory_order_release);
    }
    bool IsEmpty() { return mReadHead.load(memory_order_relaxed) == mWriteHead.load(memory_order_relaxed); }
    bool HasData() { return mReadHead.load(memory_order_relaxed) != mWriteHead.load(memory_order_relaxed); }
    bool NeedsFree() { return mFreeHead.load(memory_order_relaxed) != mReadHead.load(memory_order_relaxed); }

    bool Write(MsgType& data) {
        unsigned int next = NextPos(mWriteHead.load(memory_order_relaxed));
        if (next == mFreeHead.load(memory_order_relaxed))
            return false; // fifo is full
        mItems[next] = data;

        mWriteHead.store(next, memory_order_release);

        return true;
    }

    void Perform() // get next and advance
    {
        while (HasData()) {
            unsigned int next = NextPos(mReadHead.load(memory_order_relaxed));
            mItems[next].Perform();
            mReadHead.store(next, memory_order_release);
        }
    }
    void Free() // reclaim messages
    {
        while (NeedsFree()) {
            unsigned int next = NextPos(mFreeHead.load(memory_order_relaxed));
            mItems[next].Free();
            mFreeHead.store(next, memory_order_release);
        }
    }

private:
    int NextPos(int inPos) { return (inPos + 1) & (N - 1); }

    std::atomic<int> mReadHead, mWriteHead, mFreeHead;
    MsgType mItems[N];
};

/////////////////////////////////////////////////////////////////////

template <class MsgType, int N> class MsgFifoNoFree {
public:
    MsgFifoNoFree(): mReadHead(0), mWriteHead(0) {}

    void MakeEmpty() { mReadHead.store(mWriteHead.load()); }
    bool IsEmpty() { return mReadHead.load(memory_order_relaxed) == mWriteHead.load(memory_order_relaxed); }
    bool HasData() { return mReadHead.load(memory_order_relaxed) != mWriteHead.load(memory_order_relaxed); }

    bool Write(MsgType& data) {
        unsigned int next = NextPos(mWriteHead.load(memory_order_relaxed));
        if (next == mReadHead.load(memory_order_relaxed))
            return false; // fifo is full
        mItems[next] = data;

        mWriteHead.store(next, memory_order_release);

        return true;
    }

    void Perform() // get next and advance
    {
        while (HasData()) {
            unsigned int next = NextPos(mReadHead.load(memory_order_relaxed));
            mItems[next].Perform();
            mReadHead.store(next, memory_order_release);
        }
    }

private:
    int NextPos(int inPos) { return (inPos + 1) & (N - 1); }

    std::atomic<int> mReadHead, mWriteHead;
    MsgType mItems[N];
};

}

using SCMsgFifo::MsgFifo;
using SCMsgFifo::MsgFifoNoFree;

/////////////////////////////////////////////////////////////////////
