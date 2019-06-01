/*
 *  GrainUGens.cpp
 *  xSC3plugins
 *
 *  Created by Josh Parmenter on 2/4/05.
 *  Copyright 2005 __MyCompanyName__. All rights reserved.
 *
 */
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


#include "SC_PlugIn.h"

#include "function_attributes.h"

static InterfaceTable* ft;

const int kMaxGrains = 64;

struct GrainInG {
    double b1, y1, y2, curamp, winPos, winInc; // envelope
    int counter, chan;
    float pan1, pan2, winType;
};

struct GrainIn : public Unit {
    int mNumActive, m_channels, mMaxGrains;
    float curtrig;
    bool mFirst;
    GrainInG* mGrains;
};

struct GrainSinG {
    double b1, y1, y2, curamp, winPos, winInc; // envelope
    int counter, chan;
    float pan1, pan2, winType;
    int32 oscphase; // the phase of the osc inside this grain
    int32 freq; // the freq of the osc inside this grain in phase inc
};

struct GrainSin : public Unit {
    int mNumActive, m_channels, mMaxGrains;
    uint32 m_lomask;
    float curtrig;
    bool mFirst;
    double m_cpstoinc, m_radtoinc;
    GrainSinG* mGrains;
};

struct GrainFMG {
    int32 coscphase, moscphase; // the phase of the osc inside this grain
    int32 mfreq; // the freq of the osc inside this grain in phase inc
    double b1, y1, y2, curamp, winPos, winInc; // envelope
    float deviation, carbase, pan1, pan2, winType;
    int counter, chan;
};


struct GrainFM : public Unit {
    int mNumActive, m_channels, mMaxGrains;
    uint32 m_lomask;
    float curtrig;
    bool mFirst;
    double m_cpstoinc, m_radtoinc;
    GrainFMG* mGrains;
};

struct GrainBufG {
    double phase, rate;
    double b1, y1, y2, curamp, winPos, winInc;
    float pan1, pan2, winType;
    int counter, chan, bufnum, interp;
};

struct GrainBuf : public Unit {
    int mNumActive, m_channels, mMaxGrains;
    float curtrig;
    GrainBufG* mGrains;
};

struct WarpWinGrain {
    double phase, rate;
    double winPos, winInc, b1, y1, y2, curamp; // tells the grain where to look in the winBuf for an amp value
    int counter, bufnum, interp;
    float winType;
};

struct Warp1 : public Unit {
    float m_fbufnum;
    SndBuf* m_buf;
    int mNumActive[16];
    int mNextGrain[16];
    WarpWinGrain mGrains[16][kMaxGrains];
};

////////////////////////////////////////////////////////////////////////

extern "C" {

void GrainIn_Ctor(GrainIn* unit);
void GrainIn_Dtor(GrainIn* unit);
void GrainIn_next_a(GrainIn* unit, int inNumSamples);
void GrainIn_next_k(GrainIn* unit, int inNumSamples);

void GrainSin_Ctor(GrainSin* unit);
void GrainSin_Dtor(GrainSin* unit);
void GrainSin_next_a(GrainSin* unit, int inNumSamples);
void GrainSin_next_k(GrainSin* unit, int inNumSamples);

void GrainFM_Ctor(GrainFM* unit);
void GrainFM_Dtor(GrainFM* unit);
void GrainFM_next_a(GrainFM* unit, int inNumSamples);
void GrainFM_next_k(GrainFM* unit, int inNumSamples);

void GrainBuf_Ctor(GrainBuf* unit);
void GrainBuf_Dtor(GrainBuf* unit);

void Warp1_next(Warp1* unit, int inNumSamples);
void Warp1_Ctor(Warp1* unit);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////// Granular UGens ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

inline float GRAIN_IN_AT(Unit* unit, int index, int offset) {
    if (INRATE(index) == calc_FullRate)
        return IN(index)[offset];
    if (INRATE(index) == calc_DemandRate)
        return DEMANDINPUT_A(index, offset + 1);
    return IN0(index);
}

template <bool full_rate> inline float grain_in_at(Unit* unit, int index, int offset) {
    if (full_rate)
        return GRAIN_IN_AT(unit, index, offset);

    if (INRATE(index) == calc_DemandRate)
        return DEMANDINPUT_A(index, offset + 1);
    else
        return IN0(index);
}

inline double sc_gloop(double in, double hi) {
    // avoid the divide if possible
    if (in >= hi) {
        in -= hi;
        if (in < hi)
            return in;
    } else if (in < 0.) {
        in += hi;
        if (in >= 0.)
            return in;
    } else
        return in;

    return in - hi * floor(in / hi);
}

#define GRAIN_BUF                                                                                                      \
    const SndBuf* buf;                                                                                                 \
    if (bufnum >= world->mNumSndBufs) {                                                                                \
        int localBufNum = bufnum - world->mNumSndBufs;                                                                 \
        Graph* parent = unit->mParent;                                                                                 \
        if (localBufNum <= parent->localBufNum) {                                                                      \
            buf = parent->mLocalSndBufs + localBufNum;                                                                 \
        } else {                                                                                                       \
            bufnum = 0;                                                                                                \
            buf = world->mSndBufs + bufnum;                                                                            \
        }                                                                                                              \
    } else {                                                                                                           \
        if (bufnum < 0) {                                                                                              \
            bufnum = 0;                                                                                                \
        }                                                                                                              \
        buf = world->mSndBufs + bufnum;                                                                                \
    }                                                                                                                  \
    LOCK_SNDBUF_SHARED(buf);                                                                                           \
    const float* bufData __attribute__((__unused__)) = buf->data;                                                      \
    uint32 bufChannels __attribute__((__unused__)) = buf->channels;                                                    \
    uint32 bufSamples __attribute__((__unused__)) = buf->samples;                                                      \
    uint32 bufFrames = buf->frames;                                                                                    \
    int guardFrame __attribute__((__unused__)) = bufFrames - 2;

#define DECLARE_WINDOW                                                                                                 \
    double winPos, winInc, w, b1, y1, y2, y0;                                                                          \
    float amp;                                                                                                         \
    winPos = winInc = w = b1 = y1 = y2 = y0 = amp = 0.;                                                                \
    SndBuf* window;                                                                                                    \
    const float* windowData __attribute__((__unused__)) = 0;                                                           \
    uint32 windowSamples __attribute__((__unused__)) = 0;                                                              \
    uint32 windowFrames __attribute__((__unused__)) = 0;                                                               \
    int windowGuardFrame = 0;


#define CHECK_BUF                                                                                                      \
    if (!bufData) {                                                                                                    \
        unit->mDone = true;                                                                                            \
        ClearUnitOutputs(unit, inNumSamples);                                                                          \
        return;                                                                                                        \
    }

#define GET_GRAIN_WIN_RELAXED(WINTYPE)                                                                                 \
    do {                                                                                                               \
        assert(WINTYPE < unit->mWorld->mNumSndBufs);                                                                   \
        window = unit->mWorld->mSndBufs + (int)WINTYPE;                                                                \
        windowData = window->data;                                                                                     \
        windowSamples = window->samples;                                                                               \
        windowFrames = window->frames;                                                                                 \
        windowGuardFrame = windowFrames - 1;                                                                           \
    } while (0);


static inline bool getGrainWin(Unit* unit, float wintype, SndBuf*& window, const float*& windowData,
                               uint32& windowSamples, uint32& windowFrames, int& windowGuardFrame) {
    if (wintype >= unit->mWorld->mNumSndBufs) {
        Print("Envelope buffer out of range!\n");
        return false;
    }

    assert(wintype < unit->mWorld->mNumSndBufs);

    if (wintype < 0)
        return true; // use default hann window

    window = unit->mWorld->mSndBufs + (int)wintype;
    windowData = window->data;
    if (!windowData)
        return false;

    windowSamples = window->samples;
    windowFrames = window->frames;
    windowGuardFrame = windowFrames - 1;

    return true;
}

#define GRAIN_LOOP_BODY_4                                                                                              \
    float amp = y1 * y1;                                                                                               \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    float* table1 = bufData + iphase;                                                                                  \
    float* table0 = table1 - 1;                                                                                        \
    float* table2 = table1 + 1;                                                                                        \
    float* table3 = table1 + 2;                                                                                        \
    if (iphase == 0) {                                                                                                 \
        table0 += bufSamples;                                                                                          \
    } else if (iphase >= guardFrame) {                                                                                 \
        if (iphase == guardFrame) {                                                                                    \
            table3 -= bufSamples;                                                                                      \
        } else {                                                                                                       \
            table2 -= bufSamples;                                                                                      \
            table3 -= bufSamples;                                                                                      \
        }                                                                                                              \
    }                                                                                                                  \
    float fracphase = phase - (double)iphase;                                                                          \
    float a = table0[0];                                                                                               \
    float b = table1[0];                                                                                               \
    float c = table2[0];                                                                                               \
    float d = table3[0];                                                                                               \
    float outval = amp * cubicinterp(fracphase, a, b, c, d) ZXP(out1) += outval;                                       \
    double y0 = b1 * y1 - y2;                                                                                          \
    y2 = y1;                                                                                                           \
    y1 = y0;

#define GRAIN_LOOP_BODY_2                                                                                              \
    float amp = y1 * y1;                                                                                               \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    float* table1 = bufData + iphase;                                                                                  \
    float* table2 = table1 + 1;                                                                                        \
    if (iphase > guardFrame) {                                                                                         \
        table2 -= bufSamples;                                                                                          \
    }                                                                                                                  \
    double fracphase = phase - (double)iphase;                                                                         \
    float b = table1[0];                                                                                               \
    float c = table2[0];                                                                                               \
    float outval = amp * (b + fracphase * (c - b));                                                                    \
    ZXP(out1) += outval;                                                                                               \
    double y0 = b1 * y1 - y2;                                                                                          \
    y2 = y1;                                                                                                           \
    y1 = y0;


#define GRAIN_LOOP_BODY_1                                                                                              \
    float amp = y1 * y1;                                                                                               \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    float outval = amp * bufData[iphase];                                                                              \
    ZXP(out1) += outval;                                                                                               \
    double y0 = b1 * y1 - y2;                                                                                          \
    y2 = y1;                                                                                                           \
    y1 = y0;

#define BUF_GRAIN_AMP                                                                                                  \
    winPos += winInc;                                                                                                  \
    int iWinPos = (int)winPos;                                                                                         \
    double winFrac = winPos - (double)iWinPos;                                                                         \
    float* winTable1 = windowData + iWinPos;                                                                           \
    float* winTable2 = winTable1 + 1;                                                                                  \
    if (winPos > windowGuardFrame) {                                                                                   \
        winTable2 -= windowSamples;                                                                                    \
    }                                                                                                                  \
    amp = lininterp(winFrac, winTable1[0], winTable2[0]);

#define BUF_GRAIN_LOOP_BODY_4                                                                                          \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    float* table1 = bufData + iphase;                                                                                  \
    float* table0 = table1 - 1;                                                                                        \
    float* table2 = table1 + 1;                                                                                        \
    float* table3 = table1 + 2;                                                                                        \
    if (iphase == 0) {                                                                                                 \
        table0 += bufSamples;                                                                                          \
    } else if (iphase >= guardFrame) {                                                                                 \
        if (iphase == guardFrame) {                                                                                    \
            table3 -= bufSamples;                                                                                      \
        } else {                                                                                                       \
            table2 -= bufSamples;                                                                                      \
            table3 -= bufSamples;                                                                                      \
        }                                                                                                              \
    }                                                                                                                  \
    float fracphase = phase - (double)iphase;                                                                          \
    float a = table0[0];                                                                                               \
    float b = table1[0];                                                                                               \
    float c = table2[0];                                                                                               \
    float d = table3[0];                                                                                               \
    float outval = amp * cubicinterp(fracphase, a, b, c, d);                                                           \
    ZXP(out1) += outval;

#define BUF_GRAIN_LOOP_BODY_2                                                                                          \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    float* table1 = bufData + iphase;                                                                                  \
    float* table2 = table1 + 1;                                                                                        \
    if (iphase > guardFrame) {                                                                                         \
        table2 -= bufSamples;                                                                                          \
    }                                                                                                                  \
    float fracphase = phase - (double)iphase;                                                                          \
    float b = table1[0];                                                                                               \
    float c = table2[0];                                                                                               \
    float outval = amp * (b + fracphase * (c - b));                                                                    \
    ZXP(out1) += outval;

// amp needs to be calculated by looking up values in window

#define BUF_GRAIN_LOOP_BODY_1                                                                                          \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    float outval = amp * bufData[iphase];                                                                              \
    ZXP(out1) += outval;


#define SETUP_OUT                                                                                                      \
    uint32 numOutputs = unit->mNumOutputs;                                                                             \
    if (numOutputs > bufChannels) {                                                                                    \
        unit->mDone = true;                                                                                            \
        ClearUnitOutputs(unit, inNumSamples);                                                                          \
        return;                                                                                                        \
    }                                                                                                                  \
    float* out[16];                                                                                                    \
    for (uint32 i = 0; i < numOutputs; ++i)                                                                            \
        out[i] = ZOUT(i);


#define CALC_GRAIN_PAN                                                                                                 \
    float panangle, pan1, pan2;                                                                                        \
    float *out1, *out2;                                                                                                \
    if (numOutputs > 1) {                                                                                              \
        if (numOutputs > 2) {                                                                                          \
            pan = sc_wrap(pan * 0.5f, 0.f, 1.f);                                                                       \
            float cpan = numOutputs * pan + 0.5f;                                                                      \
            float ipan = floor(cpan);                                                                                  \
            float panfrac = cpan - ipan;                                                                               \
            panangle = panfrac * pi2_f;                                                                                \
            grain->chan = (int)ipan;                                                                                   \
            if (grain->chan >= (int)numOutputs)                                                                        \
                grain->chan -= numOutputs;                                                                             \
        } else {                                                                                                       \
            grain->chan = 0;                                                                                           \
            pan = sc_clip(pan * 0.5f + 0.5f, 0.f, 1.f);                                                                \
            panangle = pan * pi2_f;                                                                                    \
        }                                                                                                              \
        pan1 = grain->pan1 = cos(panangle);                                                                            \
        pan2 = grain->pan2 = sin(panangle);                                                                            \
    } else {                                                                                                           \
        grain->chan = 0;                                                                                               \
        pan1 = grain->pan1 = 1.;                                                                                       \
        pan2 = grain->pan2 = 0.;                                                                                       \
    }

#define GET_GRAIN_INIT_AMP                                                                                             \
    if (grain->winType < 0.) {                                                                                         \
        w = pi / counter;                                                                                              \
        b1 = grain->b1 = 2. * cos(w);                                                                                  \
        y1 = sin(w);                                                                                                   \
        y2 = 0.;                                                                                                       \
        amp = y1 * y1;                                                                                                 \
    } else {                                                                                                           \
        amp = windowData[0];                                                                                           \
        winPos = grain->winPos = 0.f;                                                                                  \
        winInc = grain->winInc = (double)windowSamples / counter;                                                      \
    }

#define CALC_NEXT_GRAIN_AMP_INTERNAL                                                                                   \
    do {                                                                                                               \
        y0 = b1 * y1 - y2;                                                                                             \
        y2 = y1;                                                                                                       \
        y1 = y0;                                                                                                       \
        amp = y1 * y1;                                                                                                 \
    } while (0)

#define CALC_NEXT_GRAIN_AMP_CUSTOM                                                                                     \
    do {                                                                                                               \
        winPos += winInc;                                                                                              \
        int iWinPos = (int)winPos;                                                                                     \
        double winFrac = winPos - (double)iWinPos;                                                                     \
        const float* winTable1 = windowData + iWinPos;                                                                 \
        const float* winTable2 = winTable1 + 1;                                                                        \
        if (!windowData)                                                                                               \
            break;                                                                                                     \
        if (winPos > windowGuardFrame)                                                                                 \
            winTable2 -= windowSamples;                                                                                \
        amp = lininterp(winFrac, winTable1[0], winTable2[0]);                                                          \
    } while (0);                                                                                                       \
    if (!windowData)                                                                                                   \
        break;

#define CALC_NEXT_GRAIN_AMP                                                                                            \
    if (grain->winType < 0.) {                                                                                         \
        CALC_NEXT_GRAIN_AMP_INTERNAL;                                                                                  \
    } else {                                                                                                           \
        CALC_NEXT_GRAIN_AMP_CUSTOM;                                                                                    \
    }


#define GET_GRAIN_AMP_PARAMS                                                                                           \
    if (grain->winType < 0.) {                                                                                         \
        b1 = grain->b1;                                                                                                \
        y1 = grain->y1;                                                                                                \
        y2 = grain->y2;                                                                                                \
        amp = grain->curamp;                                                                                           \
    } else {                                                                                                           \
        GET_GRAIN_WIN_RELAXED(grain->winType);                                                                         \
        if (!windowData)                                                                                               \
            break;                                                                                                     \
        winPos = grain->winPos;                                                                                        \
        winInc = grain->winInc;                                                                                        \
        amp = grain->curamp;                                                                                           \
    }

#define SAVE_GRAIN_AMP_PARAMS                                                                                          \
    grain->y1 = y1;                                                                                                    \
    grain->y2 = y2;                                                                                                    \
    grain->winPos = winPos;                                                                                            \
    grain->winInc = winInc;                                                                                            \
    grain->curamp = amp;                                                                                               \
    grain->counter -= nsmps;

#define WRAP_CHAN(offset)                                                                                              \
    out1 = OUT(grain->chan) + offset;                                                                                  \
    if (numOutputs > 1) {                                                                                              \
        if ((grain->chan + 1) >= (int)numOutputs)                                                                      \
            out2 = OUT(0) + offset;                                                                                    \
        else                                                                                                           \
            out2 = OUT(grain->chan + 1) + offset;                                                                      \
    }

#define GET_PAN_PARAMS                                                                                                 \
    float pan1 = grain->pan1;                                                                                          \
    uint32 chan1 = grain->chan;                                                                                        \
    float* out1 = OUT(chan1);                                                                                          \
    if (numOutputs > 1) {                                                                                              \
        pan2 = grain->pan2;                                                                                            \
        uint32 chan2 = chan1 + 1;                                                                                      \
        if (chan2 >= numOutputs)                                                                                       \
            chan2 = 0;                                                                                                 \
        out2 = OUT(chan2);                                                                                             \
    }


//////////////////// InGrain ////////////////////

inline void GrainIn_next_play_active(GrainIn* unit, int inNumSamples) {
    const uint32 numOutputs = unit->mNumOutputs;
    float* in = IN(2);
    for (int i = 0; i < unit->mNumActive;) {
        GrainInG* grain = unit->mGrains + i;
        DECLARE_WINDOW
        GET_GRAIN_AMP_PARAMS
        // begin add //

        float pan2 = 0.f;
        float* out2;

        GET_PAN_PARAMS
        // end add //
        int nsmps = sc_min(grain->counter, inNumSamples);
        for (int j = 0; j < nsmps; ++j) {
            float outval = amp * in[j];
            // begin change / add //
            out1[j] += outval * pan1;
            if (numOutputs > 1)
                out2[j] += outval * pan2;
            // end change //

            CALC_NEXT_GRAIN_AMP
        }

        SAVE_GRAIN_AMP_PARAMS

        if (grain->counter <= 0)
            *grain = unit->mGrains[--unit->mNumActive]; // remove grain
        else
            ++i;
    }
}

template <bool full_rate> inline void GrainIn_next_start_new(GrainIn* unit, int inNumSamples, int position) {
    if (unit->mNumActive + 1 >= unit->mMaxGrains) {
        Print("Too many grains!\n");
        return;
    }

    float winType = grain_in_at<full_rate>(unit, 4, position);
    DECLARE_WINDOW
    bool success = getGrainWin(unit, winType, window, windowData, windowSamples, windowFrames, windowGuardFrame);
    if (!success)
        return;

    GrainInG* grain = unit->mGrains + unit->mNumActive++;
    float winSize = grain_in_at<full_rate>(unit, 1, position);
    double counter = winSize * SAMPLERATE;
    counter = sc_max(4., counter);
    grain->counter = (int)counter;
    grain->winType = winType;

    GET_GRAIN_INIT_AMP

    const uint32 numOutputs = unit->mNumOutputs;
    float* in = IN(2);

    float* in1 = in + position;
    // begin add //
    float pan = grain_in_at<full_rate>(unit, 3, position);

    CALC_GRAIN_PAN

    WRAP_CHAN(position)

    int nsmps = sc_min(grain->counter, inNumSamples - position);
    for (int j = 0; j < nsmps; ++j) {
        float outval = amp * in1[j];
        // begin add / change
        out1[j] += outval * pan1;
        if (numOutputs > 1)
            out2[j] += outval * pan2;
        // end add / change
        CALC_NEXT_GRAIN_AMP
    }

    SAVE_GRAIN_AMP_PARAMS

    if (grain->counter <= 0)
        *grain = unit->mGrains[--unit->mNumActive]; // remove grain
}

void GrainIn_next_a(GrainIn* unit, int inNumSamples) {
    ClearUnitOutputs(unit, inNumSamples);
    // begin add
    // end add

    GrainIn_next_play_active(unit, inNumSamples);

    float* trig = IN(0);
    for (int i = 0; i < inNumSamples; ++i) {
        if ((unit->curtrig <= 0) && (trig[i] > 0.0))
            GrainIn_next_start_new<true>(unit, inNumSamples, i);
        unit->curtrig = trig[i];
    }
}

void GrainIn_next_k(GrainIn* unit, int inNumSamples) {
    ClearUnitOutputs(unit, inNumSamples);
    if (unit->mFirst) {
        unit->mFirst = false;
        float maxGrains = IN0(5);
        unit->mMaxGrains = (int)maxGrains;
        unit->mGrains = (GrainInG*)RTAlloc(unit->mWorld, unit->mMaxGrains * sizeof(GrainInG));
    }

    GrainIn_next_play_active(unit, inNumSamples);

    float trig = IN0(0);

    if ((unit->curtrig <= 0) && (trig > 0.0))
        GrainIn_next_start_new<false>(unit, inNumSamples, 0);
    unit->curtrig = trig;
}


void GrainIn_Ctor(GrainIn* unit) {
    if (INRATE(0) == calc_FullRate)
        SETCALC(GrainIn_next_a);
    else
        SETCALC(GrainIn_next_k);
    unit->mFirst = true;
    unit->mNumActive = 0;
    unit->curtrig = 0.f;
    GrainIn_next_k(unit, 1);
}

void GrainIn_Dtor(GrainIn* unit) { RTFree(unit->mWorld, unit->mGrains); }

inline void GrainSin_next_play_active(GrainSin* unit, int inNumSamples) {
    const unsigned int numOutputs = unit->mNumOutputs;

    float* table0 = ft->mSineWavetable;
    float* table1 = table0 + 1;

    for (int i = 0; i < unit->mNumActive;) {
        GrainSinG* grain = unit->mGrains + i;
        DECLARE_WINDOW
        GET_GRAIN_AMP_PARAMS

        // begin add //

        float pan2 = 0.f;
        float* out2;

        GET_PAN_PARAMS

        // end add //

        int32 thisfreq = grain->freq;
        int32 oscphase = grain->oscphase;

        int nsmps = sc_min(grain->counter, inNumSamples);
        for (int j = 0; j < nsmps; ++j) {
            float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
            // begin change / add //
            out1[j] += outval * pan1;
            if (numOutputs > 1)
                out2[j] += outval * pan2;
            // end change //
            CALC_NEXT_GRAIN_AMP

            oscphase += thisfreq;
        }

        SAVE_GRAIN_AMP_PARAMS

        grain->oscphase = oscphase;
        if (grain->counter <= 0)
            *grain = unit->mGrains[--unit->mNumActive]; // remove grain
        else
            ++i;
    }
}

template <bool full_rate> inline void GrainSin_next_start_new(GrainSin* unit, int inNumSamples, int position) {
    if (unit->mNumActive + 1 >= unit->mMaxGrains) {
        Print("Too many grains!\n");
        return;
    }

    float winType = grain_in_at<full_rate>(unit, 4, position);
    DECLARE_WINDOW
    bool success = getGrainWin(unit, winType, window, windowData, windowSamples, windowFrames, windowGuardFrame);
    if (!success)
        return;

    GrainSinG* grain = unit->mGrains + unit->mNumActive++;
    float freq = grain_in_at<full_rate>(unit, 2, position);
    float winSize = grain_in_at<full_rate>(unit, 1, position);
    int32 thisfreq = grain->freq = (int32)(unit->m_cpstoinc * freq);
    int32 oscphase = 0;
    double counter = winSize * SAMPLERATE;
    counter = sc_max(4., counter);
    grain->counter = (int)counter;
    grain->winType = winType;

    GET_GRAIN_INIT_AMP

    const uint32 numOutputs = unit->mNumOutputs;

    float* table0 = ft->mSineWavetable;
    float* table1 = table0 + 1;

    // begin add //
    float pan = grain_in_at<full_rate>(unit, 3, position);

    CALC_GRAIN_PAN

    WRAP_CHAN(position)

    // end add //

    int nsmps = sc_min(grain->counter, inNumSamples - position);
    for (int j = 0; j < nsmps; ++j) {
        float outval = amp * lookupi1(table0, table1, oscphase, unit->m_lomask);
        // begin add / change
        out1[j] += outval * pan1;
        if (numOutputs > 1)
            out2[j] += outval * pan2;
        // end add / change
        CALC_NEXT_GRAIN_AMP

        oscphase += thisfreq;
    }
    grain->oscphase = oscphase;

    SAVE_GRAIN_AMP_PARAMS

    if (grain->counter <= 0)
        *grain = unit->mGrains[--unit->mNumActive]; // remove grain
}

void GrainSin_next_a(GrainSin* unit, int inNumSamples) {
    ClearUnitOutputs(unit, inNumSamples);

    GrainSin_next_play_active(unit, inNumSamples);

    float* trig = IN(0);
    for (int i = 0; i < inNumSamples; ++i) {
        if ((unit->curtrig <= 0) && (trig[i] > 0.0))
            GrainSin_next_start_new<true>(unit, inNumSamples, i);
        unit->curtrig = trig[i];
    }
}

void GrainSin_next_k(GrainSin* unit, int inNumSamples) {
    ClearUnitOutputs(unit, inNumSamples);
    if (unit->mFirst) {
        unit->mFirst = false;
        float maxGrains = IN0(5);
        unit->mMaxGrains = (int)maxGrains;
        unit->mGrains = (GrainSinG*)RTAlloc(unit->mWorld, unit->mMaxGrains * sizeof(GrainSinG));
    }

    GrainSin_next_play_active(unit, inNumSamples);

    float trig = IN0(0);


    if ((unit->curtrig <= 0) && (trig > 0.0))
        GrainSin_next_start_new<false>(unit, inNumSamples, 0);
    unit->curtrig = trig;
}

void GrainSin_Ctor(GrainSin* unit) {
    if (INRATE(0) == calc_FullRate)
        SETCALC(GrainSin_next_a);
    else
        SETCALC(GrainSin_next_k);
    int tableSizeSin = ft->mSineSize;
    unit->m_lomask = (tableSizeSin - 1) << 3;
    unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
    unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
    unit->curtrig = 0.f;
    unit->mNumActive = 0;
    unit->mFirst = true;
    GrainSin_next_k(unit, 1);
}

void GrainSin_Dtor(GrainSin* unit) { RTFree(unit->mWorld, unit->mGrains); }


inline void GrainFM_next_play_active(GrainFM* unit, int inNumSamples) {
    const uint32 numOutputs = unit->mNumOutputs;
    // end add

    float* table0 = ft->mSineWavetable;
    float* table1 = table0 + 1;

    for (int i = 0; i < unit->mNumActive;) {
        GrainFMG* grain = unit->mGrains + i;
        DECLARE_WINDOW
        GET_GRAIN_AMP_PARAMS

        int32 mfreq = grain->mfreq;
        int32 moscphase = grain->moscphase;
        int32 coscphase = grain->coscphase;
        float deviation = grain->deviation;
        float carbase = grain->carbase;
        // begin add //

        float pan2 = 0.f;
        float* out2;

        GET_PAN_PARAMS

        // end add //
        int nsmps = sc_min(grain->counter, inNumSamples);
        for (int j = 0; j < nsmps; ++j) {
            float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
            float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
            // begin change / add //
            out1[j] += outval * pan1;
            if (numOutputs > 1)
                out2[j] += outval * pan2;
            // end change //
            CALC_NEXT_GRAIN_AMP

            int32 cfreq = (int32)(unit->m_cpstoinc * (carbase + thismod)); // needs to be calced in the loop!
            coscphase += cfreq;
            moscphase += mfreq;
        } // need to save float carbase, int32 mfreq, float deviation
        grain->coscphase = coscphase;
        grain->moscphase = moscphase;

        SAVE_GRAIN_AMP_PARAMS

        if (grain->counter <= 0)
            *grain = unit->mGrains[--unit->mNumActive]; // remove grain
        else
            ++i;
    }
}

template <bool full_rate> inline void GrainFM_next_start_new(GrainFM* unit, int inNumSamples, int position) {
    if (unit->mNumActive + 1 >= unit->mMaxGrains) {
        Print("Too many grains!\n");
        return;
    }

    float winType = grain_in_at<full_rate>(unit, 6, position);
    DECLARE_WINDOW
    bool success = getGrainWin(unit, winType, window, windowData, windowSamples, windowFrames, windowGuardFrame);
    if (!success)
        return;

    GrainFMG* grain = unit->mGrains + unit->mNumActive++;
    float winSize = GRAIN_IN_AT(unit, 1, position);
    float carfreq = GRAIN_IN_AT(unit, 2, position);
    float modfreq = GRAIN_IN_AT(unit, 3, position);
    float index = GRAIN_IN_AT(unit, 4, position);
    float deviation = grain->deviation = index * modfreq;
    int32 mfreq = grain->mfreq = (int32)(unit->m_cpstoinc * modfreq);
    grain->carbase = carfreq;
    int32 coscphase = 0;
    int32 moscphase = 0;
    double counter = winSize * SAMPLERATE;
    counter = sc_max(4., counter);
    grain->counter = (int)counter;
    grain->winType = winType; // GRAIN_IN_AT(unit, 6, i);

    GET_GRAIN_INIT_AMP
    const uint32 numOutputs = unit->mNumOutputs;

    float* table0 = ft->mSineWavetable;
    float* table1 = table0 + 1;

    // begin add //
    float pan = GRAIN_IN_AT(unit, 5, position);

    CALC_GRAIN_PAN

    WRAP_CHAN(position)

    // end add //
    int nsmps = sc_min(grain->counter, inNumSamples - position);
    for (int j = 0; j < nsmps; ++j) {
        float thismod = lookupi1(table0, table1, moscphase, unit->m_lomask) * deviation;
        float outval = amp * lookupi1(table0, table1, coscphase, unit->m_lomask);
        // begin add / change
        out1[j] += outval * pan1;
        if (numOutputs > 1)
            out2[j] += outval * pan2;
        // end add / change
        CALC_NEXT_GRAIN_AMP

        int32 cfreq = (int32)(unit->m_cpstoinc * (carfreq + thismod)); // needs to be calced in the loop!
        coscphase += cfreq;
        moscphase += mfreq;
    } // need to save float carbase, int32 mfreq, float deviation
    grain->coscphase = coscphase;
    grain->moscphase = moscphase;

    SAVE_GRAIN_AMP_PARAMS

    if (grain->counter <= 0)
        *grain = unit->mGrains[--unit->mNumActive]; // remove grain
}

void GrainFM_next_a(GrainFM* unit, int inNumSamples) {
    ClearUnitOutputs(unit, inNumSamples);

    GrainFM_next_play_active(unit, inNumSamples);

    float* trig = IN(0);
    for (int i = 0; i < inNumSamples; ++i) {
        if ((unit->curtrig <= 0) && (trig[i] > 0.0))
            GrainFM_next_start_new<true>(unit, inNumSamples, i);
        unit->curtrig = trig[i];
    }
}

void GrainFM_next_k(GrainFM* unit, int inNumSamples) {
    ClearUnitOutputs(unit, inNumSamples);

    if (unit->mFirst) {
        unit->mFirst = false;
        float maxGrains = IN0(7);
        unit->mMaxGrains = (int)maxGrains;
        unit->mGrains = (GrainFMG*)RTAlloc(unit->mWorld, unit->mMaxGrains * sizeof(GrainFMG));
    }

    GrainFM_next_play_active(unit, inNumSamples);

    float trig = IN0(0);
    if ((unit->curtrig <= 0) && (trig > 0.0))
        GrainFM_next_start_new<false>(unit, inNumSamples, 0);

    unit->curtrig = trig;
}

void GrainFM_Ctor(GrainFM* unit) {
    if (INRATE(0) == calc_FullRate)
        SETCALC(GrainFM_next_a);
    else
        SETCALC(GrainFM_next_k);
    int tableSizeSin = ft->mSineSize;
    unit->m_lomask = (tableSizeSin - 1) << 3;
    unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
    unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
    unit->curtrig = 0.f;
    unit->mNumActive = 0;
    unit->mFirst = true;
    GrainFM_next_k(unit, 1);
}

void GrainFM_Dtor(GrainFM* unit) { RTFree(unit->mWorld, unit->mGrains); }

#define GRAIN_BUF_LOOP_BODY_4_MONO                                                                                     \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    const float* table1 = bufData + iphase;                                                                            \
    const float* table0 = table1 - 1;                                                                                  \
    const float* table2 = table1 + 1;                                                                                  \
    const float* table3 = table1 + 2;                                                                                  \
    if (iphase == 0) {                                                                                                 \
        table0 += bufSamples;                                                                                          \
    } else if (iphase >= guardFrame) {                                                                                 \
        if (iphase == guardFrame) {                                                                                    \
            table3 -= bufSamples;                                                                                      \
        } else {                                                                                                       \
            table2 -= bufSamples;                                                                                      \
            table3 -= bufSamples;                                                                                      \
        }                                                                                                              \
    }                                                                                                                  \
    float fracphase = phase - (double)iphase;                                                                          \
    float a = table0[0];                                                                                               \
    float b = table1[0];                                                                                               \
    float c = table2[0];                                                                                               \
    float d = table3[0];                                                                                               \
    float outval = amp * cubicinterp(fracphase, a, b, c, d);                                                           \
    out1[j] += outval * pan1;

#define GRAIN_BUF_LOOP_BODY_4_STEREO                                                                                   \
    GRAIN_BUF_LOOP_BODY_4_MONO                                                                                         \
    out2[j] += outval * pan2;

#define GRAIN_BUF_LOOP_BODY_2_MONO                                                                                     \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    const float* table1 = bufData + iphase;                                                                            \
    const float* table2 = table1 + 1;                                                                                  \
    if (iphase > guardFrame) {                                                                                         \
        table2 -= bufSamples;                                                                                          \
    }                                                                                                                  \
    float fracphase = phase - (double)iphase;                                                                          \
    float b = table1[0];                                                                                               \
    float c = table2[0];                                                                                               \
    float outval = amp * (b + fracphase * (c - b));                                                                    \
    out1[j] += outval * pan1;

#define GRAIN_BUF_LOOP_BODY_2_STEREO                                                                                   \
    GRAIN_BUF_LOOP_BODY_2_MONO                                                                                         \
    out2[j] += outval * pan2;

#define GRAIN_BUF_LOOP_BODY_1_MONO                                                                                     \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    float outval = amp * bufData[iphase];                                                                              \
    out1[j] += outval * pan1;

#define GRAIN_BUF_LOOP_BODY_1_STEREO                                                                                   \
    GRAIN_BUF_LOOP_BODY_1_MONO                                                                                         \
    out2[j] += outval * pan2;


#define GRAIN_BUF_PLAY_GRAIN(WINDOW)                                                                                   \
    do {                                                                                                               \
        if (numOutputs == 1) {                                                                                         \
            if (grain->interp >= 4) {                                                                                  \
                for (int j = 0; j < nsmps; j++) {                                                                      \
                    GRAIN_BUF_LOOP_BODY_4_MONO                                                                         \
                    CALC_NEXT_GRAIN_AMP_##WINDOW;                                                                      \
                    phase += rate;                                                                                     \
                }                                                                                                      \
            } else if (grain->interp >= 2) {                                                                           \
                for (int j = 0; j < nsmps; j++) {                                                                      \
                    GRAIN_BUF_LOOP_BODY_2_MONO                                                                         \
                    CALC_NEXT_GRAIN_AMP_##WINDOW;                                                                      \
                    phase += rate;                                                                                     \
                }                                                                                                      \
            } else {                                                                                                   \
                for (int j = 0; j < nsmps; j++) {                                                                      \
                    GRAIN_BUF_LOOP_BODY_1_MONO                                                                         \
                    CALC_NEXT_GRAIN_AMP_##WINDOW;                                                                      \
                    phase += rate;                                                                                     \
                }                                                                                                      \
            }                                                                                                          \
        } else {                                                                                                       \
            if (grain->interp >= 4) {                                                                                  \
                for (int j = 0; j < nsmps; j++) {                                                                      \
                    GRAIN_BUF_LOOP_BODY_4_STEREO                                                                       \
                    CALC_NEXT_GRAIN_AMP_##WINDOW;                                                                      \
                    phase += rate;                                                                                     \
                }                                                                                                      \
            } else if (grain->interp >= 2) {                                                                           \
                for (int j = 0; j < nsmps; j++) {                                                                      \
                    GRAIN_BUF_LOOP_BODY_2_STEREO                                                                       \
                    CALC_NEXT_GRAIN_AMP_##WINDOW;                                                                      \
                    phase += rate;                                                                                     \
                }                                                                                                      \
            } else {                                                                                                   \
                for (int j = 0; j < nsmps; j++) {                                                                      \
                    GRAIN_BUF_LOOP_BODY_1_STEREO                                                                       \
                    CALC_NEXT_GRAIN_AMP_##WINDOW;                                                                      \
                    phase += rate;                                                                                     \
                }                                                                                                      \
            }                                                                                                          \
        }                                                                                                              \
    } while (0)


static inline bool GrainBuf_grain_cleanup(GrainBuf* unit, GrainBufG* grain) {
    if (grain->counter <= 0) {
        *grain = unit->mGrains[--unit->mNumActive]; // remove grain
        return true;
    } else
        return false;
}

template <bool IsMono> static inline void GrainBuf_next_play_active(GrainBuf* unit, int inNumSamples) {
    const uint32 numOutputs = IsMono ? 1 : unit->mNumOutputs;

    World* world = unit->mWorld;

    for (int i = 0; i < unit->mNumActive;) {
        GrainBufG* grain = unit->mGrains + i;
        int bufnum = grain->bufnum;

        GRAIN_BUF

        if (!bufData || (bufChannels != 1)) {
            grain->counter -= inNumSamples;
            if (!GrainBuf_grain_cleanup(unit, grain))
                ++i;
            continue;
        }

        double loopMax = (double)bufFrames;
        double rate = grain->rate;
        double phase = grain->phase;

        DECLARE_WINDOW
        GET_GRAIN_AMP_PARAMS


        // begin add //
        float pan2 = 0.f;
        float* out2;
        GET_PAN_PARAMS
        // end add //
        int nsmps = sc_min(grain->counter, inNumSamples);

        if (grain->winType < 0.)
            GRAIN_BUF_PLAY_GRAIN(INTERNAL);
        else
            GRAIN_BUF_PLAY_GRAIN(CUSTOM);

        if (GrainBuf_grain_cleanup(unit, grain))
            continue;
        ++i;

        grain->phase = phase;

        SAVE_GRAIN_AMP_PARAMS
    }
}

template <bool full_rate, bool IsMono>
static inline void GrainBuf_next_start_new(GrainBuf* unit, int inNumSamples, int position) {
    World* world = unit->mWorld;

    if (unit->mNumActive + 1 >= unit->mMaxGrains) {
        Print("Too many grains!\n");
        return;
    }

    float winType = grain_in_at<full_rate>(unit, 7, position);
    DECLARE_WINDOW
    bool success = getGrainWin(unit, winType, window, windowData, windowSamples, windowFrames, windowGuardFrame);
    if (!success)
        return;

    GrainBufG* grain = unit->mGrains + unit->mNumActive++;
    int32 bufnum = grain_in_at<full_rate>(unit, 2, position);
    grain->bufnum = bufnum;

    GRAIN_BUF

    if ((bufChannels != 1) || (!bufData)) {
        GrainBuf_grain_cleanup(unit, grain);
        return;
    }

    float bufSampleRate = buf->samplerate;
    float bufRateScale = bufSampleRate * SAMPLEDUR;
    double loopMax = (double)bufFrames;

    double rate = grain->rate = grain_in_at<full_rate>(unit, 3, position) * bufRateScale;
    double phase = grain_in_at<full_rate>(unit, 4, position) * bufFrames;
    if (!sc_isfinite(phase)) {
        GrainBuf_grain_cleanup(unit, grain);
        return;
    }

    double counter = grain_in_at<full_rate>(unit, 1, position) * SAMPLERATE;
    counter = sc_max(4., counter);
    grain->counter = (int)counter;

    grain->interp = (int)grain_in_at<full_rate>(unit, 5, position);
    grain->winType = winType;

    GET_GRAIN_INIT_AMP

    const uint32 numOutputs = IsMono ? 1 : unit->mNumOutputs;

    // begin add //
    float pan = grain_in_at<full_rate>(unit, 6, position);

    CALC_GRAIN_PAN

    WRAP_CHAN(position)

    // end add //

    int nsmps = sc_min(grain->counter, inNumSamples - position);

    if (grain->winType < 0.)
        GRAIN_BUF_PLAY_GRAIN(INTERNAL);
    else
        GRAIN_BUF_PLAY_GRAIN(CUSTOM);

    grain->phase = phase;

    SAVE_GRAIN_AMP_PARAMS

    GrainBuf_grain_cleanup(unit, grain);
}


template <bool MultiChannel> static void GrainBuf_next_a(GrainBuf* unit, int inNumSamples) {
    ClearUnitOutputs(unit, inNumSamples);

    GrainBuf_next_play_active<MultiChannel>(unit, inNumSamples);

    float* trig = IN(0);
    for (int i = 0; i < inNumSamples; i++) {
        if ((trig[i] > 0) && (unit->curtrig <= 0))
            GrainBuf_next_start_new<true, MultiChannel>(unit, inNumSamples, i);
        unit->curtrig = trig[i];
    }
}

template <bool MultiChannel> static void GrainBuf_next_k(GrainBuf* unit, int inNumSamples) {
    ClearUnitOutputs(unit, inNumSamples);

    GrainBuf_next_play_active<MultiChannel>(unit, inNumSamples);

    float trig = IN0(0);
    if ((trig > 0) && (unit->curtrig <= 0))
        GrainBuf_next_start_new<false, MultiChannel>(unit, inNumSamples, 0);
    unit->curtrig = trig;
}

FLATTEN static void GrainBuf_next_k_1(GrainBuf* unit, int inNumSamples) { GrainBuf_next_k<true>(unit, inNumSamples); }

FLATTEN static void GrainBuf_next_k_2(GrainBuf* unit, int inNumSamples) { GrainBuf_next_k<false>(unit, inNumSamples); }

FLATTEN static void GrainBuf_next_a_1(GrainBuf* unit, int inNumSamples) { GrainBuf_next_a<true>(unit, inNumSamples); }

FLATTEN static void GrainBuf_next_a_2(GrainBuf* unit, int inNumSamples) { GrainBuf_next_a<false>(unit, inNumSamples); }

void GrainBuf_Ctor(GrainBuf* unit) {
    unit->mNumActive = 0;
    unit->curtrig = 0.f;

    float maxGrains = IN0(8);
    unit->mMaxGrains = (int)maxGrains;
    unit->mGrains = (GrainBufG*)RTAlloc(unit->mWorld, unit->mMaxGrains * sizeof(GrainBufG));

    if (unit->mNumOutputs == 1) {
        if (INRATE(0) == calc_FullRate)
            SETCALC(GrainBuf_next_a_1);
        else
            SETCALC(GrainBuf_next_k_1);
    } else {
        if (INRATE(0) == calc_FullRate)
            SETCALC(GrainBuf_next_a_2);
        else
            SETCALC(GrainBuf_next_k_2);
    }

    (unit->mCalcFunc)(unit, 1);
}

void GrainBuf_Dtor(GrainBuf* unit) { RTFree(unit->mWorld, unit->mGrains); }


#define BUF_GRAIN_LOOP_BODY_4_N                                                                                        \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    const float* table1 = bufData + iphase * bufChannels;                                                              \
    const float* table0 = table1 - bufChannels;                                                                        \
    const float* table2 = table1 + bufChannels;                                                                        \
    const float* table3 = table2 + bufChannels;                                                                        \
    if (iphase == 0) {                                                                                                 \
        table0 += bufSamples;                                                                                          \
    } else if (iphase >= guardFrame) {                                                                                 \
        if (iphase == guardFrame) {                                                                                    \
            table3 -= bufSamples;                                                                                      \
        } else {                                                                                                       \
            table2 -= bufSamples;                                                                                      \
            table3 -= bufSamples;                                                                                      \
        }                                                                                                              \
    }                                                                                                                  \
    float fracphase = phase - (double)iphase;                                                                          \
    float a = table0[n];                                                                                               \
    float b = table1[n];                                                                                               \
    float c = table2[n];                                                                                               \
    float d = table3[n];                                                                                               \
    float outval = amp * cubicinterp(fracphase, a, b, c, d);                                                           \
    ZXP(out1) += outval;

#define BUF_GRAIN_LOOP_BODY_2_N                                                                                        \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    const float* table1 = bufData + iphase * bufChannels;                                                              \
    const float* table2 = table1 + bufChannels;                                                                        \
    if (iphase > guardFrame) {                                                                                         \
        table2 -= bufSamples;                                                                                          \
    }                                                                                                                  \
    float fracphase = phase - (double)iphase;                                                                          \
    float b = table1[n];                                                                                               \
    float c = table2[n];                                                                                               \
    float outval = amp * (b + fracphase * (c - b));                                                                    \
    ZXP(out1) += outval;

// amp needs to be calculated by looking up values in window

#define BUF_GRAIN_LOOP_BODY_1_N                                                                                        \
    phase = sc_gloop(phase, loopMax);                                                                                  \
    int32 iphase = (int32)phase;                                                                                       \
    float outval = amp * bufData[iphase + n];                                                                          \
    ZXP(out1) += outval;


#define GET_WARP_WIN_RELAXED(WINTYPE)                                                                                  \
    do {                                                                                                               \
        assert(WINTYPE < unit->mWorld->mNumSndBufs);                                                                   \
        window = unit->mWorld->mSndBufs + (int)WINTYPE;                                                                \
        while (!TRY_ACQUIRE_SNDBUF_SHARED(window)) {                                                                   \
            RELEASE_SNDBUF_SHARED(buf);                                                                                \
            ACQUIRE_SNDBUF_SHARED(buf);                                                                                \
        }                                                                                                              \
        windowData = window->data;                                                                                     \
        if (windowData == NULL)                                                                                        \
            RELEASE_SNDBUF_SHARED(window);                                                                             \
        windowSamples = window->samples;                                                                               \
        windowFrames = window->frames;                                                                                 \
        windowGuardFrame = windowFrames - 1;                                                                           \
    } while (0);

#define GET_WARP_AMP_PARAMS                                                                                            \
    if (grain->winType < 0.) {                                                                                         \
        b1 = grain->b1;                                                                                                \
        y1 = grain->y1;                                                                                                \
        y2 = grain->y2;                                                                                                \
        amp = grain->curamp;                                                                                           \
    } else {                                                                                                           \
        GET_WARP_WIN_RELAXED(grain->winType);                                                                          \
        if (!windowData)                                                                                               \
            break;                                                                                                     \
        winPos = grain->winPos;                                                                                        \
        winInc = grain->winInc;                                                                                        \
        amp = grain->curamp;                                                                                           \
    }


void Warp1_next(Warp1* unit, int inNumSamples) {
    ClearUnitOutputs(unit, inNumSamples);

    GET_BUF
    SETUP_OUT
    CHECK_BUF

    for (uint32 n = 0; n < numOutputs; n++) {
        int nextGrain = unit->mNextGrain[n];
        for (int i = 0; i < unit->mNumActive[n];) {
            WarpWinGrain* grain = unit->mGrains[n] + i;

            double loopMax = (double)bufFrames;

            double rate = grain->rate;
            double phase = grain->phase;
            DECLARE_WINDOW
            GET_GRAIN_AMP_PARAMS
            float* out1 = out[n];
            int nsmps = sc_min(grain->counter, inNumSamples);
            if (grain->interp >= 4) {
                for (int j = 0; j < nsmps; ++j) {
                    BUF_GRAIN_LOOP_BODY_4_N
                    CALC_NEXT_GRAIN_AMP
                    phase += rate;
                }
            } else if (grain->interp >= 2) {
                for (int j = 0; j < nsmps; ++j) {
                    BUF_GRAIN_LOOP_BODY_2_N
                    CALC_NEXT_GRAIN_AMP
                    phase += rate;
                }
            } else {
                for (int j = 0; j < nsmps; ++j) {
                    BUF_GRAIN_LOOP_BODY_1_N
                    CALC_NEXT_GRAIN_AMP
                    phase += rate;
                }
            }

            grain->phase = phase;
            SAVE_GRAIN_AMP_PARAMS
            if (grain->counter <= 0)
                *grain = unit->mGrains[n][--unit->mNumActive[n]]; // remove grain
            else
                ++i;
        }

        for (int i = 0; i < inNumSamples; ++i) {
            --nextGrain;
            if (nextGrain == 0) {
                // start a grain
                if (unit->mNumActive[n] + 1 >= kMaxGrains)
                    break;
                //			uint32 bufnum = (uint32)GRAIN_IN_AT(unit, 0, i);
                //			if (bufnum >= numBufs) break;

                float bufSampleRate = buf->samplerate;
                float bufRateScale = bufSampleRate * SAMPLEDUR;
                double loopMax = (double)bufFrames;

                WarpWinGrain* grain = unit->mGrains[n] + unit->mNumActive[n]++;
                //			grain->bufnum = bufnum;

                float overlaps = GRAIN_IN_AT(unit, 5, i);
                float counter = GRAIN_IN_AT(unit, 3, i) * SAMPLERATE;
                double winrandamt = unit->mParent->mRGen->frand2() * (double)GRAIN_IN_AT(unit, 6, i);
                counter = sc_max(4., floor(counter + (counter * winrandamt)));
                grain->counter = (int)counter;

                nextGrain = (int)(counter / overlaps);

                unit->mNextGrain[n] = nextGrain;

                float rate = grain->rate = GRAIN_IN_AT(unit, 2, i) * bufRateScale;
                float phase = GRAIN_IN_AT(unit, 1, i) * (float)bufFrames;
                grain->interp = (int)GRAIN_IN_AT(unit, 7, i);
                float winType = grain->winType = (int)GRAIN_IN_AT(unit, 4, i); // the buffer that holds the grain shape
                DECLARE_WINDOW

                if (winType >= unit->mWorld->mNumSndBufs) {
                    Print("Envelope buffer out of range!\n");
                    break;
                }

                GET_GRAIN_WIN_RELAXED(winType)
                if (windowData || (winType < 0.)) {
                    GET_GRAIN_INIT_AMP

                    float* out1 = out[n] + i;

                    int nsmps = sc_min(grain->counter, inNumSamples - i);
                    if (grain->interp >= 4) {
                        for (int j = 0; j < nsmps; ++j) {
                            BUF_GRAIN_LOOP_BODY_4_N
                            CALC_NEXT_GRAIN_AMP
                            phase += rate;
                        }
                    } else if (grain->interp >= 2) {
                        for (int j = 0; j < nsmps; ++j) {
                            BUF_GRAIN_LOOP_BODY_2_N
                            CALC_NEXT_GRAIN_AMP
                            phase += rate;
                        }
                    } else {
                        for (int j = 0; j < nsmps; ++j) {
                            BUF_GRAIN_LOOP_BODY_1_N
                            CALC_NEXT_GRAIN_AMP
                            phase += rate;
                        }
                    }

                    grain->phase = phase;
                    SAVE_GRAIN_AMP_PARAMS
                    // end change
                    if (grain->counter <= 0)
                        *grain = unit->mGrains[n][--unit->mNumActive[n]]; // remove grain
                }
            }
        }

        unit->mNextGrain[n] = nextGrain;
    }
}

void Warp1_Ctor(Warp1* unit) {
    SETCALC(Warp1_next);

    for (int i = 0; i < 16; i++) {
        unit->mNumActive[i] = 0;
        unit->mNextGrain[i] = 1;
    }

    ClearUnitOutputs(unit, 1);
    unit->m_fbufnum = -1e9f;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(Grain) {
    ft = inTable;

    DefineDtorCantAliasUnit(GrainIn);
    DefineDtorCantAliasUnit(GrainSin);
    DefineDtorCantAliasUnit(GrainFM);
    DefineDtorCantAliasUnit(GrainBuf);
    DefineSimpleCantAliasUnit(Warp1);
}
