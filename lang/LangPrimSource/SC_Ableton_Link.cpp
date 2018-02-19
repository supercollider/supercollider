#ifdef SC_ABLETON_LINK

#include "PyrKernel.h"
#include "PyrSched.h"
#include "GC.h"
#include "PyrPrimitive.h"
#include "PyrSymbol.h"

#include "SCBase.h"
#include "SC_Clock.hpp"

#include <ableton/Link.hpp>

PyrSymbol* s_LinkTempoChanged;

static std::chrono::microseconds linkTimeOfInitialization;
void initLink()
{
	linkTimeOfInitialization   = ableton::link::platform::Clock().micros();
}

inline std::chrono::microseconds hrToLinkTime(double secs){
	auto time = std::chrono::duration<double>(secs);
	return std::chrono::duration_cast<std::chrono::microseconds>(time) + linkTimeOfInitialization;
}

inline double linkToHrTime(std::chrono::microseconds micros){
	return DurToFloat(micros - linkTimeOfInitialization);
}

class LinkClock : public TempoClock
{
public:
	LinkClock(VMGlobals *inVMGlobals, PyrObject* inTempoClockObj,
				double inTempo, double inBaseBeats, double inBaseSeconds);

	~LinkClock() {}

	void SetTempoAtBeat(double inTempo, double inBeats);
	void SetTempoAtTime(double inTempo, double inSeconds);
	void SetAll(double inTempo, double inBeats, double inSeconds);
	double BeatsToSecs(double beats) const
	{
		auto timeline = mLink.captureAppTimeline();
		double secs = linkToHrTime(timeline.timeAtBeat(beats, mQuantum));
		return secs;
	}
	double SecsToBeats(double secs) const
	{
		auto timeline = mLink.captureAppTimeline();
		double beats = timeline.beatAtTime(hrToLinkTime(secs), mQuantum);
		return beats;
	}

	void SetQuantum(double quantum);
	std::size_t NumPeers() const { return mLink.numPeers(); }

private:
	ableton::Link mLink;
	double mQuantum;
};

LinkClock::LinkClock(VMGlobals *inVMGlobals, PyrObject* inTempoClockObj,
							double inTempo, double inBaseBeats, double inBaseSeconds)
	: TempoClock(inVMGlobals, inTempoClockObj, inTempo, inBaseBeats, inBaseSeconds),
	mLink(inTempo * 60.)
{
	//quantum = beatsPerBar
	int err = slotDoubleVal(&inTempoClockObj->slots[2], &mQuantum);
	if(err) throw err;

	mLink.enable(true);
	mLink.setTempoCallback([this](double bpm) {
		double secs = elapsedTime();
		double tempo = bpm / 60.;

		auto timeline = mLink.captureAppTimeline();
		double beats = timeline.beatAtTime(hrToLinkTime(secs), mQuantum);
		mLink.commitAppTimeline(timeline);

		mTempo = tempo;
		mBeatDur = 1. / tempo;
		mCondition.notify_one();

		//call sclang callback
		gLangMutex.lock();
		g->canCallOS = false;
		++g->sp;
		SetObject(g->sp, mTempoClockObj);
		++g->sp;
		SetFloat(g->sp, mTempo);
		++g->sp;
		SetFloat(g->sp, beats);
		++g->sp;
		SetFloat(g->sp, secs);
		++g->sp;
		SetObject(g->sp, mTempoClockObj);
		runInterpreter(g, s_LinkTempoChanged, 5);
		g->canCallOS = false;
		gLangMutex.unlock();
	});

	auto timeline = mLink.captureAppTimeline();
	auto linkTime = hrToLinkTime(inBaseSeconds);
	timeline.requestBeatAtTime(inBaseBeats, linkTime, mQuantum);
	mLink.commitAppTimeline(timeline);
}

void LinkClock::SetAll(double inTempo, double inBeats, double inSeconds)
{

	auto timeline = mLink.captureAppTimeline();
	auto linkTime = hrToLinkTime(inSeconds);
	timeline.setTempo(inTempo * 60., linkTime);
	timeline.requestBeatAtTime(inBeats,linkTime, mQuantum);

	mTempo = inTempo;
	mBeatDur = 1. / inTempo;

	mLink.commitAppTimeline(timeline);
	mCondition.notify_one();
}

void LinkClock::SetTempoAtBeat(double inTempo, double inBeats)
{
	auto timeline = mLink.captureAppTimeline();
	auto time = timeline.timeAtBeat(inBeats, mQuantum);
	timeline.setTempo(inTempo*60., time);
	mLink.commitAppTimeline(timeline);
}

void LinkClock::SetTempoAtTime(double inTempo, double inSeconds)
{
	auto timeline = mLink.captureAppTimeline();
	timeline.setTempo(inTempo*60., hrToLinkTime(inSeconds));
	mLink.commitAppTimeline(timeline);
}

void LinkClock::SetQuantum(double quantum)
{
	mQuantum = quantum;
	mCondition.notify_one();
}

//Primitives
int prLinkClock_NumPeers(struct VMGlobals *g, int numArgsPushed);
int prLinkClock_NumPeers(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp;
	LinkClock *clock = (LinkClock*)slotRawPtr(&slotRawObject(a)->slots[1]);

	SetInt(a, clock->NumPeers());

	return errNone;
}


int prLinkClock_SetQuantum(struct VMGlobals *g, int numArgsPushed);
int prLinkClock_SetQuantum(struct VMGlobals *g, int numArgsPushed)
{
	PyrSlot *a = g->sp - 1;
	PyrSlot *b = g->sp;
	LinkClock *clock = (LinkClock*)slotRawPtr(&slotRawObject(a)->slots[1]);

	double quantum;
	int err = slotDoubleVal(b, &quantum);
	if(err) return errFailed;

	clock->SetQuantum(quantum);

	return errNone;
}

void initLinkPrimitives()
{
	int base, index=0;

	base = nextPrimitiveIndex();

	s_LinkTempoChanged = getsym("prTempoChanged");

	definePrimitive(base, index++, "_LinkClock_New", prClock_New<LinkClock>, 4, 0);
	definePrimitive(base, index++, "_LinkClock_SetBeats", prClock_SetBeats<LinkClock>, 2, 0);
	definePrimitive(base, index++, "_LinkClock_SetTempoAtBeat", prClock_SetTempoAtBeat<LinkClock>, 3, 0);
	definePrimitive(base, index++, "_LinkClock_SetTempoAtTime", prClock_SetTempoAtTime<LinkClock>, 3, 0);
	definePrimitive(base, index++, "_LinkClock_SetAll", prClock_SetAll<LinkClock>, 4, 0);
	definePrimitive(base, index++, "_LinkClock_NumPeers", prLinkClock_NumPeers, 1, 0);
	definePrimitive(base, index++, "_LinkClock_SetQuantum", prLinkClock_SetQuantum, 2, 0);
}

#endif
