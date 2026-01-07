#include <ableton/Link.hpp>

#include "SC_InterfaceTable.h"
#include "SC_Unit.h"
#include "SC_PlugIn.hpp"

static InterfaceTable* ft;

/*! we only have one global link instance per server.
 *
 * @todo: respect hardware buffer size which delays the output
 *  of the signal - this should normally be respected but currently
 *  we do not have access to this value through the Plugin API.
 */
static ableton::Link* LINK_CLOCK = nullptr;

/*!
 * @class LinkControl
 * @brief Enables or disables a Link clock, which needs to be deferred to
 * a non-RT thread.
 */
template <bool IsEnabler> class LinkControl : public SCUnit {
public:
    LinkControl() {
        mCalcFunc = make_calc_function<LinkControl, &LinkControl::next_k>();
        if (IsEnabler ? !LINK_CLOCK->isEnabled() : LINK_CLOCK->isEnabled()) {
            ft->fDoAsynchronousCommand(mWorld, nullptr, nullptr, nullptr, &LinkControl::set_clock_status, nullptr,
                                       nullptr, &LinkControl::do_nothing, 0, nullptr);
        }
    }

private:
    /*!
     * Returns 1.0 the moment the clock has been enabled or disabled.
     * This signal can therefore be used to free the synth where
     * we enable the clock.
     */
    void next_k(int numSamples) {
        float* out = mOutBuf[0];
        if (IsEnabler) {
            *out = LINK_CLOCK->isEnabled() ? 1.0f : 0.0f;
        } else {
            *out = LINK_CLOCK->isEnabled() ? 0.0f : 1.0f;
        }
    }
    /*! Not thread safe - therefore runs in a NRT thread in stage 2 */
    static bool set_clock_status(World* inWorld, void* cmdData) {
        LINK_CLOCK->enable(IsEnabler);
        if (IsEnabler) {
            Print("Enabled Link clock\n");
        } else {
            Print("Disabled Link clock\n");
        }
        return false;
    }

    /*! necessary b/c async commands require a cleanup function */
    static void do_nothing(World* inWorld, void* cmdData) {}
};

/*!
 * @class LinkBPM
 * @brief Allows to get and set the tempo of the Link clock of the server.
 * Tempo will only be set if in0 > 0.0f - be nice to others!
 * If no Link clock is enabled on the server it will return -1.0f.
 */
class LinkBPM : public SCUnit {
public:
    LinkBPM() {
        mCalcFunc = make_calc_function<LinkBPM, &LinkBPM::next_k>();
        next_k(1);
    }

private:
    bool mWarned = false;
    void next_k(int numSamples) {
        float* out = mOutBuf[0];
        if (LINK_CLOCK->isEnabled()) {
            auto state = LINK_CLOCK->captureAudioSessionState();
            if (in0(0) > 0.0f) {
                state.setTempo(in0(1), LINK_CLOCK->clock().micros());
                LINK_CLOCK->commitAudioSessionState(state);
            }
            *out = static_cast<float>(state.tempo());
        } else {
            if (!mWarned) {
                Print("Error: Link clock has not been enabled, can not access tempo\n");
                mWarned = true;
            }
            *out = -1.0f;
        }
    }
};

/*!
 * @class LinkPhase
 * @brief Returns the phase of the clock for a given quantum of range [0, quantum).
 * If no clock is enabled on the server, it will return -1.0f.
 */
class LinkPhase : public SCUnit {
public:
    LinkPhase() { mCalcFunc = make_calc_function<LinkPhase, &LinkPhase::next_k>(); }

private:
    bool mWarned = false;

    void next_k(int numSamples) {
        float* out = mOutBuf[0];
        if (LINK_CLOCK->isEnabled()) {
            auto state = LINK_CLOCK->captureAudioSessionState();
            auto phase = state.phaseAtTime(LINK_CLOCK->clock().micros(), in0(0));
            *out = static_cast<float>(phase);
        } else {
            if (!mWarned) {
                Print("Error: Link clock has not been enabled, can not get phase\n");
                mWarned = true;
            }
            *out = -1.0f;
        }
    }
};

/*!
 * @class LinkJump
 * @brief Requests or forces a jump of all Link clients to a specific position within a given quantum.
 */
class LinkJump : public SCUnit {
public:
    LinkJump() { mCalcFunc = make_calc_function<LinkJump, &LinkJump::next_k>(); }

private:
    bool mWarned = false;

    void next_k(int numSamples) {
        float* out = mOutBuf[0];
        if (LINK_CLOCK->isEnabled()) {
            if (in0(0) > 0.0f) {
                auto state = LINK_CLOCK->captureAudioSessionState();
                if (in0(3) > 0.0f) {
                    state.forceBeatAtTime(in0(1), LINK_CLOCK->clock().micros(), in0(2));
                } else {
                    state.requestBeatAtTime(in0(1), LINK_CLOCK->clock().micros(), in0(2));
                }
                LINK_CLOCK->commitAudioSessionState(state);
            }
            *out = 0.0f;
        } else {
            if (!mWarned) {
                Print("Error: Link clock has not been enabled, can not set phase\n");
                mWarned = true;
            }
            *out = -1.0f;
        }
    }
};

PluginLoad(LinkUGen) {
    ft = inTable;
    LINK_CLOCK = new ableton::Link(60.0f);
    registerUnit<LinkControl<true>>(ft, "LinkEnabler", false);
    registerUnit<LinkControl<false>>(ft, "LinkDisabler", false);
    registerUnit<LinkBPM>(ft, "LinkBPM", false);
    registerUnit<LinkPhase>(ft, "LinkPhase", false);
    registerUnit<LinkJump>(ft, "LinkJump", false);
}

PluginUnload(LinkUGen) { delete LINK_CLOCK; }
