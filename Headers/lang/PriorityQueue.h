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

#ifndef _PriorityQueue_
#define _PriorityQueue_

#include <limits>

template <class Event, class TimeType, int N> 
class PriorityQueueT
{
public:
	PriorityQueueT() {
		Empty();
	}
	
	bool Add(Event& inEvent)
		{
			if (mSize >= N) return false;
			long mom = mSize++;
			long me = mom;
			for (; mom>0;) {	/* percolate up heap */
				mom = mom - 1 >> 1;
				if (inEvent.mTime < mEvents[mom].mTime) {
					mEvents[me] = mEvents[mom];
					me = mom;
				} else break;
			}
			mEvents[me] = inEvent;
			return true;
		}
	void Perform(TimeType inNow)
		{
			while (NextTime() <= inNow) {
				Event event = Remove();
				event.Perform();
			}
		}
	TimeType NextTime() { return mEvents[0].mTime; }
	bool Ready(TimeType inTime) { return NextTime() <= inTime; }
	void Flush() { Perform(std::numeric_limits<TimeType>::max()); }
	void Empty() { mSize = 0; SetEmptyTime(); }
	void SetEmptyTime() { mEvents[0].mTime = std::numeric_limits<TimeType>::max(); }
	
	Event Remove()
		{
			Event event = mEvents[0];
			if (--mSize == 0) SetEmptyTime();
			else {
				Event temp = mEvents[mSize];
				long mom = 0;
				long me = 1;
				for (;me < mSize;) { /* demote heap */
					if (me+1 < mSize && mEvents[me].mTime > mEvents[me+1].mTime) {
						me ++;
					}
					if (temp.mTime > mEvents[me].mTime) {
						mEvents[mom] = mEvents[me];
						mom = me;
						me = (me << 1) + 1;
					} else break;
				}
				mEvents[mom] = temp;
			}
			return event;
		}
		
private:
	long mSize;
	Event mEvents[N];
};

#endif