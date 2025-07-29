#include <ableton/Link.hpp>

#include "SC_InterfaceTable.h"
#include "SC_Unit.h"
#include "SC_PlugIn.hpp"

static InterfaceTable* ft;

class LinkUGen : public SCUnit {
public:
    LinkUGen(): mLink(120.0) {
        // this is not rt safe :p
        mLink.enable(true);

        mMSecPerTick = std::chrono::microseconds(static_cast<int>(10e6 / sampleRate()));

        // how do we obtain the passed 'control' or 'audio' parameter in the plugin api?
        if (isControlRateIn(0)) {
            mCalcFunc = make_calc_function<LinkUGen, &LinkUGen::next_k>();
            // prime the pipe
            next_k(1);
        } else {
            mCalcFunc = make_calc_function<LinkUGen, &LinkUGen::next>();
            next(1);
        }
    }
    ~LinkUGen() {
        mLink.enable(false);
        // do we need to destroy mLink here ;?
    }

private:
    // how to allocate this not during construction?
    ableton::Link mLink;
    std::chrono::microseconds mMSecPerTick = std::chrono::microseconds::zero();

    void next_k(int numSamples) {
        auto sessionState = mLink.captureAudioSessionState();

        const float setTempo = in0(0);
        const float newTempo = in0(1);
        const float setIsPlaying = in0(2);
        const float isPlayingVal = in0(3);
        const float quantum = in0(4);
        const float delay = in0(5);

        bool tempoSet = false;
        bool playSet = false;

        const std::chrono::microseconds hardwareOffset = std::chrono::microseconds(static_cast<int>(10e6 * delay));

        // should we calculate from the beginning or from middle of the block (size)?
        const auto time = mLink.clock().micros() + hardwareOffset;

        if (setTempo > 0.0f) {
            tempoSet = true;
            sessionState.setTempo(newTempo, time);
        }
        if (setIsPlaying > 0.0f) {
            playSet = true;
            sessionState.setIsPlaying(isPlayingVal, time);
        }

        if (playSet || tempoSet) {
            mLink.commitAudioSessionState(sessionState);
        }

        // phase
        out0(0) = sessionState.phaseAtTime(time, quantum);
        // tempo
        out0(1) = sessionState.tempo();
    }

    void next(int numSamples) {
        auto sessionState = mLink.captureAudioSessionState();

        float* phase = out(0);
        float* tempo = out(1);

        const float* setTempo = in(0);
        const float* newTempo = in(1);
        const float* setIsPlaying = in(2);
        const float* isPlayingVal = in(3);
        const float* quantum = in(4);
        const float* delay = in(5);

        bool tempoSet = false;
        bool playSet = false;

        const std::chrono::microseconds hardwareOffset = std::chrono::microseconds(static_cast<int>(10e6 * delay[0]));

        for (int i = 0; i < numSamples; i++) {
            // i * mSecPerTick could be pre-calculated once - this seems to be computational intensive for some reason?
            const auto time = mLink.clock().micros() + (i * mMSecPerTick) + hardwareOffset;

            if (setTempo[i] > 0.0f && !tempoSet) {
                // only set once in a block? sample accurate though
                tempoSet = true;
                sessionState.setTempo(newTempo[i], time);
            }
            if (setIsPlaying[i] > 0.0f && !playSet) {
                // only set once in a block? sample accurate though
                playSet = true;
                sessionState.setIsPlaying(isPlayingVal[i], time);
            }

            phase[i] = sessionState.phaseAtTime(time, quantum[i]);
            tempo[i] = sessionState.tempo();
        }

        if (playSet || tempoSet) {
            mLink.commitAudioSessionState(sessionState);
        }
    }
};

PluginLoad(LinkUGen) {
    ft = inTable;
    registerUnit<LinkUGen>(ft, "LinkUGen", false);
}
