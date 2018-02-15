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

#ifndef SC_CLOCK_INCLUDED
#define SC_CLOCK_INCLUDED

#include "PyrKernel.h"
#include "PyrSched.h"
#include "GC.h"
#include "PyrPrimitive.h"
#include "PyrSymbol.h"

// heaps use an integer timestamp to ensure stable heap order
struct PyrHeap:
	PyrObjectHdr
{
	PyrSlot count;    // stability count
	PyrSlot slots[0]; // slots
};

//if ABLETON LINK functionality is on, we must define some methods virtual
#ifdef SC_ABLETON_LINK
#define CLOCK_VIRTUAL_METHOD virtual
#else
#define CLOCK_VIRTUAL_METHOD
#endif

class TempoClock
{
public:
	TempoClock(VMGlobals *inVMGlobals, PyrObject* inTempoClockObj,
				double inTempo, double inBaseBeats, double inBaseSeconds);
	CLOCK_VIRTUAL_METHOD ~TempoClock() {}
	void StopReq();
	void Stop();
	void StopAndDelete()
	{
		Stop();
		delete this;
	}

	void* Run();

	void Add(double inBeats, PyrSlot* inTask);
	void SetTempoAtBeat(double inTempo, double inBeats);
	void SetTempoAtTime(double inTempo, double inSeconds);
	void SetAll(double inTempo, double inBeats, double inSeconds);
	double ElapsedBeats();
	void Clear();
	//void Flush();
	double GetTempo() const { return mTempo; }
	double GetBeatDur() const { return mBeatDur; }
	CLOCK_VIRTUAL_METHOD double BeatsToSecs(double beats) const
		{ return (beats - mBaseBeats) * mBeatDur + mBaseSeconds; }
	CLOCK_VIRTUAL_METHOD double SecsToBeats(double secs) const
		{ return (secs - mBaseSeconds) * mTempo + mBaseBeats; }
	void Dump();

//protected:
	VMGlobals* g;
	PyrObject* mTempoClockObj;
	PyrHeap* mQueue;

	double mTempo; // beats per second
	double mBeatDur; // 1/tempo
	double mBeats; // beats
	double mBaseSeconds;
	double mBaseBeats;
	bool mRun;
	SC_Thread mThread;
	condition_variable_any mCondition;
	TempoClock *mPrev, *mNext;

	static TempoClock *sAll;

};

// Clock primitives
template <typename Clock>
int prClock_New(struct VMGlobals *g, int numArgsPushed);
template <typename Clock>
int prClock_New(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 3;
	PyrSlot *b = g->sp - 2;
	PyrSlot *c = g->sp - 1;
	PyrSlot *d = g->sp;

	double tempo;
	int err = slotDoubleVal(b, &tempo);
	if (err) tempo = 1.;
	if (tempo <= 0.) {
		error("invalid tempo %g\n", tempo);
		SetPtr(slotRawObject(a)->slots+1, NULL);
		return errFailed;
	}

	double beats;
	err = slotDoubleVal(c, &beats);
	if (err) beats = 0.;

	double seconds;
	err = slotDoubleVal(d, &seconds);
	if (err) {
		err = slotDoubleVal(&g->thread->seconds, &seconds);
		if (err) return err;
	}

	Clock* clock = new Clock(g, slotRawObject(a), tempo, beats, seconds);
	SetPtr(slotRawObject(a)->slots+1, clock);
	return errNone;
}

template <typename Clock>
int prClock_SetBeats(struct VMGlobals *g, int numArgsPushed);
template <typename Clock>
int prClock_SetBeats(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;

	Clock *clock = (Clock*)slotRawPtr(&slotRawObject(a)->slots[1]);
	if (!clock) {
		error("clock is not running.\n");
		return errFailed;
	}

	double beats, seconds;
	int err;

	err = slotDoubleVal(b, &beats);
	if (err) return errWrongType;

	err = slotDoubleVal(&g->thread->seconds, &seconds);
	if (err) return errWrongType;

	clock->SetAll(clock->mTempo, beats, seconds);

	return errNone;
}

template <typename Clock>
int prClock_SetTempoAtBeat(struct VMGlobals *g, int numArgsPushed);
template <typename Clock>
int prClock_SetTempoAtBeat(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	Clock *clock = (Clock*)slotRawPtr(&slotRawObject(a)->slots[1]);
	if (!clock) {
		error("clock is not running.\n");
		return errFailed;
	}
	if(clock->mTempo <= 0.f) {
		error("cannot set tempo from this method. The message 'etempo_' can be used instead.\n");
		// prTempoClock_SetTempoAtTime can be used.
		return errFailed;
	}

	double tempo, beat;
	int err = slotDoubleVal(b, &tempo);
	if (err) return errFailed;
	if (tempo <= 0.) {
		error("invalid tempo %g. The message 'etempo_' can be used instead.\n", tempo);
		// prTempoClock_SetTempoAtTime can be used.
		return errFailed;
	}

	err = slotDoubleVal(c, &beat);
	if (err) return errFailed;

	clock->SetTempoAtBeat(tempo, beat);

	return errNone;
}

template <typename Clock>
int prClock_SetAll(struct VMGlobals *g, int numArgsPushed);
template <typename Clock>
int prClock_SetAll(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 3;
	PyrSlot *b = g->sp - 2;
	PyrSlot *c = g->sp - 1;
	PyrSlot *d = g->sp;

	Clock *clock = (Clock*)slotRawPtr(&slotRawObject(a)->slots[1]);
	if (!clock) {
		error("clock is not running.\n");
		return errFailed;
	}

	double tempo, beat, secs;
	int err = slotDoubleVal(b, &tempo);
	if (err) return errFailed;

	err = slotDoubleVal(c, &beat);
	if (err) return errFailed;

	err = slotDoubleVal(d, &secs);
	if (err) return errFailed;

	clock->SetAll(tempo, beat, secs);

	return errNone;
}

template <typename Clock>
int prClock_SetTempoAtTime(struct VMGlobals *g, int numArgsPushed);
template <typename Clock>
int prClock_SetTempoAtTime(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 2;
	PyrSlot *b = g->sp - 1;
	PyrSlot *c = g->sp;

	Clock *clock = (Clock*)slotRawPtr(&slotRawObject(a)->slots[1]);
	if (!clock) {
		error("clock is not running.\n");
		return errFailed;
	}

	double tempo, sec;
	int err = slotDoubleVal(b, &tempo);
	if (err) return errFailed;

	err = slotDoubleVal(c, &sec);
	if (err) return errFailed;

	clock->SetTempoAtTime(tempo, sec);

	return errNone;
}


template <typename DurationType>
inline double DurToFloat(DurationType dur)
{
	using namespace std::chrono;
	seconds secs         = duration_cast<seconds>(dur);
	nanoseconds nanosecs = dur - secs;

	return secs.count() + 1.0e-9 * nanosecs.count();
}

#ifdef SC_ABLETON_LINK
void initLink();
#endif

#endif
