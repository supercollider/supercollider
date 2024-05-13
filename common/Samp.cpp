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

#include "Samp.hpp"
#include "SC_Constants.h"

float32 gSine[kSineSize + 1];
float32 gPMSine[kSineSize + 1];
float32 gInvSine[kSineSize + 1];
float32 gSineWavetable[2 * kSineSize];

void SignalAsWavetable(float32* signal, float32* wavetable, long inSize) {
    float32 val1, val2;

    float32* in = signal;
    float32* out = wavetable - 1;
    for (int i = 0; i < inSize - 1; ++i) {
        val1 = in[i];
        val2 = in[i + 1];
        *++out = 2.f * val1 - val2;
        *++out = val2 - val1;
    }
    val1 = in[inSize - 1];
    val2 = in[0];
    *++out = 2.f * val1 - val2;
    *++out = val2 - val1;
}

void WavetableAsSignal(float32* wavetable, float32* signal, long inSize) {
    float32* in = wavetable - 1;
    float32* out = signal - 1;
    for (int i = 0; i < inSize; ++i) {
        float32 a = *++in;
        float32 b = *++in;
        *++out = a + b;
    }
}

class SynthLibInit {
public:
    SynthLibInit() { FillTables(); }

    void FillTables() {
        double sineIndexToPhase = twopi / kSineSize;
        double pmf = (1L << 29) / twopi;
        for (int i = 0; i <= kSineSize; ++i) {
            double phase = i * sineIndexToPhase;
            float32 d = sin(phase);
            gSine[i] = d;
            gInvSine[i] = 1. / d;
            gPMSine[i] = d * pmf;
        }
        SignalAsWavetable(gSine, gSineWavetable, kSineSize);

        gInvSine[0] = gInvSine[kSineSize / 2] = gInvSine[kSineSize] = kBadValue;
        int sz = kSineSize;
        int sz2 = sz >> 1;
        for (int i = 1; i <= 8; ++i) {
            gInvSine[i] = gInvSine[sz - i] = kBadValue;
            gInvSine[sz2 - i] = gInvSine[sz2 + i] = kBadValue;
        }
    }
};

static SynthLibInit gSynthLibInit;
