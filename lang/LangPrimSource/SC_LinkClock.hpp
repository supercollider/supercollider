#pragma once

#ifdef SC_ABLETON_LINK

#    include "PyrKernel.h"
#    include "PyrSched.h"
#    include "GC.h"
#    include "PyrPrimitive.h"
#    include "PyrSymbol.h"

#    include "SCBase.h"
#    include "SC_Clock.hpp"

#    include <ableton/Link.hpp>

/** TempoClock for use with Ableton Link.
 *
 * Represented in sclang as class LinkClock.
 */
class LinkClock : public TempoClock {
public:
    /// Called during PyrSched init.
    static void Init();
    static std::chrono::microseconds GetInitTime() { return LinkClock::InitTime; }

private:
    static std::chrono::microseconds InitTime;

public:
    LinkClock(VMGlobals* vmGlobals, PyrObject* tempoClockObj, double tempo, double baseBeats, double baseSeconds);
    ~LinkClock() noexcept = default;

    void SetTempoAtBeat(double tempo, double inBeats);
    void SetTempoAtTime(double tempo, double inSeconds);
    void SetAll(double tempo, double inBeats, double inSeconds);

    double BeatsToSecs(double beats) const override;
    double SecsToBeats(double secs) const override;

    void SetQuantum(double quantum) {
        mQuantum = quantum;
        mCondition.notify_one();
    }

    double GetLatency() const { return mLatency; }
    void SetLatency(double latency) { mLatency = latency; }

    void OnTempoChanged(double bpm);

    void OnStartStop(bool isPlaying) {
        gLangMutex.lock();
        g->canCallOS = false;
        ++g->sp;
        SetObject(g->sp, mTempoClockObj);
        ++g->sp;
        SetBool(g->sp, isPlaying);
        runInterpreter(g, getsym("prStartStopSync"), 2);
        g->canCallOS = false;
        gLangMutex.unlock();
    }

    void OnNumPeersChanged(std::size_t numPeers) {
        gLangMutex.lock();
        g->canCallOS = false;
        ++g->sp;
        SetObject(g->sp, mTempoClockObj);
        ++g->sp;
        SetInt(g->sp, numPeers);
        runInterpreter(g, getsym("prNumPeersChanged"), 2);
        g->canCallOS = false;
        gLangMutex.unlock();
    }

    std::size_t NumPeers() const { return mLink.numPeers(); }

private:
    void CommitTempo(ableton::Link::SessionState sessionState, double tempo) {
        mTempo = tempo;
        mBeatDur = 1. / tempo;

        mLink.commitAppSessionState(sessionState);
        mCondition.notify_one();
    }

    ableton::Link mLink;
    double mQuantum;
    double mLatency;
};

#endif // SC_ABLETON_LINK
