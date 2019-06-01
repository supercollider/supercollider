#ifdef SC_ABLETON_LINK

#    include "SC_LinkClock.hpp"

inline std::chrono::microseconds hrToLinkTime(double secs) {
    auto time = std::chrono::duration<double>(secs);
    return std::chrono::duration_cast<std::chrono::microseconds>(time) + LinkClock::GetInitTime();
}

inline double linkToHrTime(std::chrono::microseconds micros) { return DurToFloat(micros - LinkClock::GetInitTime()); }

std::chrono::microseconds LinkClock::InitTime;

LinkClock::LinkClock(VMGlobals* vmGlobals, PyrObject* tempoClockObj, double tempo, double baseBeats,
                     double baseSeconds):
    TempoClock(vmGlobals, tempoClockObj, tempo, baseBeats, baseSeconds),
    mLink(tempo * 60.) {
    // quantum = beatsPerBar
    int err = slotDoubleVal(&tempoClockObj->slots[2], &mQuantum);
    if (err)
        throw err;

    mLatency = 0.; // default, user should override

    mLink.enable(true);
    mLink.enableStartStopSync(true);
    mLink.setTempoCallback([this](double bpm) { OnTempoChanged(bpm); });
    mLink.setStartStopCallback([this](bool isPlaying) { OnStartStop(isPlaying); });
    mLink.setNumPeersCallback([this](std::size_t numPeers) { OnNumPeersChanged(numPeers); });

    auto sessionState = mLink.captureAppSessionState();
    auto linkTime = hrToLinkTime(baseSeconds);
    sessionState.requestBeatAtTime(baseBeats, linkTime, mQuantum);
    mLink.commitAppSessionState(sessionState);
}

void LinkClock::Init() { InitTime = ableton::link::platform::Clock().micros(); }

void LinkClock::SetAll(double tempo, double inBeats, double inSeconds) {
    auto sessionState = mLink.captureAppSessionState();
    auto linkTime = hrToLinkTime(inSeconds);
    sessionState.setTempo(tempo * 60., linkTime);
    sessionState.requestBeatAtTime(inBeats, linkTime, mQuantum);
    CommitTempo(sessionState, tempo);
}

double LinkClock::BeatsToSecs(double beats) const {
    auto sessionState = mLink.captureAppSessionState();
    double secs = linkToHrTime(sessionState.timeAtBeat(beats, mQuantum)) - mLatency;
    return secs;
}

double LinkClock::SecsToBeats(double secs) const {
    auto sessionState = mLink.captureAppSessionState();
    double beats = sessionState.beatAtTime(hrToLinkTime(secs + mLatency), mQuantum);
    return beats;
}

void LinkClock::SetTempoAtBeat(double tempo, double inBeats) {
    auto sessionState = mLink.captureAppSessionState();
    auto time = sessionState.timeAtBeat(inBeats, mQuantum);
    sessionState.setTempo(tempo * 60., time);
    CommitTempo(sessionState, tempo);
}

void LinkClock::SetTempoAtTime(double tempo, double inSeconds) {
    auto sessionState = mLink.captureAppSessionState();
    sessionState.setTempo(tempo * 60., hrToLinkTime(inSeconds));
    CommitTempo(sessionState, tempo);
}

void LinkClock::OnTempoChanged(double bpm) {
    double secs = elapsedTime();
    double tempo = bpm / 60.;

    auto sessionState = mLink.captureAppSessionState();
    double beats = sessionState.beatAtTime(hrToLinkTime(secs), mQuantum);

    mTempo = tempo;
    mBeatDur = 1. / tempo;
    mCondition.notify_one();

    // call sclang callback
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
    runInterpreter(g, getsym("prTempoChanged"), 5);
    g->canCallOS = false;
    gLangMutex.unlock();
}

// Primitives -------------------------------------------------------------------------------------

int prLinkClock_NumPeers(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    LinkClock* clock = (LinkClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    SetInt(a, clock->NumPeers());

    return errNone;
}

int prLinkClock_SetQuantum(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;
    LinkClock* clock = (LinkClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    double quantum;
    int err = slotDoubleVal(b, &quantum);
    if (err)
        return errWrongType;

    clock->SetQuantum(quantum);

    return errNone;
}

int prLinkClock_GetLatency(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp;
    LinkClock* clock = (LinkClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    double latency = clock->GetLatency();
    SetFloat(g->sp, latency);
    return errNone;
}

int prLinkClock_SetLatency(struct VMGlobals* g, int numArgsPushed) {
    PyrSlot* a = g->sp - 1;
    PyrSlot* b = g->sp;
    LinkClock* clock = (LinkClock*)slotRawPtr(&slotRawObject(a)->slots[1]);
    if (!clock) {
        error("clock is not running.\n");
        return errFailed;
    }

    double latency;
    int err = slotDoubleVal(b, &latency);
    if (err)
        return errWrongType;

    clock->SetLatency(latency);

    return errNone;
}

void initLinkPrimitives() {
    int base, index = 0;

    base = nextPrimitiveIndex();

    definePrimitive(base, index++, "_LinkClock_New", prClock_New<LinkClock>, 4, 0);
    definePrimitive(base, index++, "_LinkClock_SetBeats", prClock_SetBeats<LinkClock>, 2, 0);
    definePrimitive(base, index++, "_LinkClock_SetTempoAtBeat", prClock_SetTempoAtBeat<LinkClock>, 3, 0);
    definePrimitive(base, index++, "_LinkClock_SetTempoAtTime", prClock_SetTempoAtTime<LinkClock>, 3, 0);
    definePrimitive(base, index++, "_LinkClock_SetAll", prClock_SetAll<LinkClock>, 4, 0);
    definePrimitive(base, index++, "_LinkClock_NumPeers", prLinkClock_NumPeers, 1, 0);
    definePrimitive(base, index++, "_LinkClock_SetQuantum", prLinkClock_SetQuantum, 2, 0);
    definePrimitive(base, index++, "_LinkClock_GetLatency", prLinkClock_GetLatency, 1, 0);
    definePrimitive(base, index++, "_LinkClock_SetLatency", prLinkClock_SetLatency, 2, 0);
}

#endif
