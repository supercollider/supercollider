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

#pragma once

#include <stdio.h>
#include <math.h>
#include <limits>

#define SANITYCHECK 0

const int64 kMaxInt64 = std::numeric_limits<int64>::max();

template <class Event, int N> class PriorityQueueT {
public:
    PriorityQueueT() { Empty(); }

    bool Add(Event& inEvent) {
        if (mSize >= N)
            return false;

        inEvent.mStabilityCount = mStabilityCounter++;

        long mom = mSize++;
        long me = mom;
        for (; mom > 0;) { /* percolate up heap */
            mom = (mom - 1) >> 1;
            if (inEvent.key() < mEvents[mom].key()) {
                mEvents[me] = mEvents[mom];
                me = mom;
            } else
                break;
        }
        mEvents[me] = inEvent;
#if SANITYCHECK
        SanityCheck();
#endif
        return true;
    }
    void Perform(int64 inTime) {
        while (NextTime() <= inTime) {
            Event event = Remove();
            event.Perform();
        }
    }

    int64 NextTime() { return mEvents[0].mTime; }
    bool Ready(int64 inTime) { return NextTime() <= inTime; }
    void Flush() { Perform(kMaxInt64); }
    void Empty() {
        mSize = 0;
        SetEmptyTime();
    }
    void SetEmptyTime() {
        mEvents[0].mTime = kMaxInt64;
        mStabilityCounter = 0;
    }
    int Size() { return mSize; }

    Event Remove() {
        Event event = mEvents[0];
        if (--mSize == 0)
            SetEmptyTime();
        else {
            Event temp = mEvents[mSize];
            long mom = 0;
            long me = 1;
            for (; me < mSize;) { /* demote heap */
                if (me + 1 < mSize && mEvents[me].key() > mEvents[me + 1].key()) {
                    me++;
                }
                if (temp.key() > mEvents[me].key()) {
                    mEvents[mom] = mEvents[me];
                    mom = me;
                    me = (me << 1) + 1;
                } else
                    break;
            }
            mEvents[mom] = temp;
        }
#if SANITYCHECK
        SanityCheck();
#endif
        return event;
    }

    void SanityCheck() {
        for (int i = 0; i < mSize; ++i) {
            // int j = (i<<1)+1;
            // if (j<mSize && mEvents[i].mTime > mEvents[j].mTime) throw std::runtime_error("priority queue unsorted");
            // if (k<mSize && mEvents[i].mTime > mEvents[k].mTime) throw std::runtime_error("priority queue unsorted");
        }
    }
    void DebugDump() {
        for (int i = 0; i < mSize; ++i) {
            printf("%d %016llX\n", i, mEvents[i].mTime);
        }
    }

private:
    int mSize;
    Event mEvents[N];
    int64 mStabilityCounter;
};
