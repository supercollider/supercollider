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

// third party Phase Vocoder UGens


#include "FFT_UGens.h"


extern "C" {
void PV_ConformalMap_Ctor(PV_Unit* unit);
void PV_ConformalMap_next(PV_Unit* unit, int inNumSamples);
}


void PV_ConformalMap_Ctor(PV_Unit* unit) {
    SETCALC(PV_ConformalMap_next);
    ZOUT0(0) = ZIN0(0);
}


void PV_ConformalMap_next(PV_Unit* unit, int inNumSamples) {
    PV_GET_BUF

    SCComplexBuf* p = ToComplexApx(buf);

    float real2 = ZIN0(1);
    float imag2 = ZIN0(2);

    for (int i = 0; i < numbins; ++i) {
        float real1 = p->bin[i].real;
        float imag1 = p->bin[i].imag;

        // apply conformal map z-> z-a/(1-za*) where z is the existing complex number in the bin and a is defined by
        // inputs 1 and 2
        float numr = real1 - real2;
        float numi = imag1 - imag2;
        float denomr = 1.f - (real1 * real2 + imag1 * imag2);
        float denomi = (real1 * imag2 - real2 * imag1);

        numr = numr * denomr + numi * denomi;
        numi = numi * denomr - numr * denomi;

        // squared modulus
        denomr = denomr * denomr + denomi * denomi;

        // avoid possible divide by zero
        if (denomr < 0.001f)
            denomr = 0.001f;
        denomr = 1.f / denomr;

        p->bin[i].real = numr * denomr;
        p->bin[i].imag = numi * denomr;
    }
}


#define DefinePVUnit(name) (*ft->fDefineUnit)(#name, sizeof(PV_Unit), (UnitCtorFunc)&name##_Ctor, 0, 0);


// void initPV_ThirdParty(InterfaceTable *it);
void initPV_ThirdParty(InterfaceTable* it) { DefinePVUnit(PV_ConformalMap); }
