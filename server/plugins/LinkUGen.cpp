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
 * @class LinkTempo
 * @brief Allows to get and set the tempo of the Link clock of the server.
 * Tempo will only be set if in0 > 0.0f - be nice to others!
 * If no Link clock is enabled on the server it will return -1.0f.
 * Link internally uses BPM, but SuperCollider uses CPS (cycles per second),
 * so the values will be multiplied/divided by 60.
 */
class LinkTempo : public SCUnit {
public:
    LinkTempo() { set_calc_function<LinkTempo, &LinkTempo::next_k>(); }

private:
    bool mWarned = false;

    void next_k(int numSamples) {
        float* out = mOutBuf[0];
        if (LINK_CLOCK->isEnabled()) {
            auto state = LINK_CLOCK->captureAudioSessionState();
            if (in0(0) > 0.0f) {
                state.setTempo(in0(1) * 60.0f, LINK_CLOCK->clock().micros());
                LINK_CLOCK->commitAudioSessionState(state);
            }
            *out = static_cast<float>(state.tempo()) / 60.0f;
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
    LinkPhase() { set_calc_function<LinkPhase, &LinkPhase::next_k>(); }

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
    LinkJump() { set_calc_function<LinkJump, &LinkJump::next_k>(); }

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
    registerUnit<LinkTempo>(ft, "LinkTempo", false);
    registerUnit<LinkPhase>(ft, "LinkPhase", false);
    registerUnit<LinkJump>(ft, "LinkJump", false);

    ft->fDefinePlugInCmd(
        "linkclock",
        [](World* inWorld, void*, sc_msg_iter* args, void*) {
            auto startRaw = args->geti(-1);
            bool start = false;

            switch (startRaw) {
            case -1:
                Print("ERROR: Invalid linkclock message - requires integer parameter\n");
                return;
            case 0:
                start = false;
                break;
            case 1:
                start = true;
                break;
            default:
                Print("ERROR: Invalid linkclock message - integer parameter should be either 0 or 1\n");
                return;
            }

            // these callbacks need to run in a NRT thread b/c enabling/disabling the link clock blocks.
            // we use the stage2 (NRT) of fDoAsynchronousCommand to delegate these lambda functions to the NRT thread.
            AsyncStageFn activateClock = [](World*, void*) -> bool {
                LINK_CLOCK->enable(true);
                // do not continue to stage 3
                return false;
            };
            AsyncStageFn disableClock = [](World*, void*) -> bool {
                LINK_CLOCK->enable(false);
                return false;
            };

            AsyncStageFn callback = start ? activateClock : disableClock;

            ft->fDoAsynchronousCommand(
                inWorld, nullptr, nullptr, nullptr, callback, nullptr, nullptr, [](World*, void*) {}, 0, nullptr);
        },
        nullptr);
}

PluginUnload(LinkUGen) { delete LINK_CLOCK; }
