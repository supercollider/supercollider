/*
 *  TestUGens.cpp
 *  Plugins
 *  Copyright (c) 2007 Scott Wilson <i@scottwilson.ca>. All rights reserved.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 *  Created by Scott Wilson on 22/06/2007.
 *  Modified by James Harkins on 28/07/2007.
 *
 *
 */

#include "SC_PlugIn.h"
#include <cstdio>

//////////////////////////////////////////////////////////////////////////////////////////////////

static InterfaceTable* ft;

struct CheckBadValues : public Unit {
    long sameCount;
    int prevclass;
};

struct Sanitize : public Unit {};

// declare unit generator functions
static void CheckBadValues_Ctor(CheckBadValues* unit);
static void CheckBadValues_next(CheckBadValues* unit, int inNumSamples);

static const char* CheckBadValues_fpclassString(int fpclass);
inline int CheckBadValues_fold_fpclasses(int fpclass);

static void Sanitize_Ctor(Sanitize* unit);
static void Sanitize_next_aa(Sanitize* unit, int inNumSamples);
static void Sanitize_next_ak(Sanitize* unit, int inNumSamples);
static void Sanitize_next_kk(Sanitize* unit, int inNumSamples);

//////////////////////////////////////////////////////////////////////////////////////////////////

void CheckBadValues_Ctor(CheckBadValues* unit) {
    unit->prevclass = FP_NORMAL;
    unit->sameCount = 0;
    SETCALC(CheckBadValues_next);
    CheckBadValues_next(unit, 1);
}


void CheckBadValues_next(CheckBadValues* unit, int inNumSamples) {
    float* in = ZIN(0);
    float* out = ZOUT(0);
    float id = ZIN0(1);
    int post = (int)ZIN0(2);

    float samp;
    int classification;

    switch (post) {
    case 1: // post a line on every bad value
        LOOP1(
            inNumSamples, samp = ZXP(in); classification = std::fpclassify(samp); switch (classification) {
                case FP_INFINITE:
                    Print("Infinite number found in Synth %d, ID: %d\n", unit->mParent->mNode.mID, (int)id);
                    ZXP(out) = 2;
                    break;
                case FP_NAN:
                    Print("NaN found in Synth %d, ID: %d\n", unit->mParent->mNode.mID, (int)id);
                    ZXP(out) = 1;
                    break;
                case FP_SUBNORMAL:
                    Print("Denormal found in Synth %d, ID: %d\n", unit->mParent->mNode.mID, (int)id);
                    ZXP(out) = 3;
                    break;
                default:
                    ZXP(out) = 0;
            };);
        break;
    case 2:
        LOOP1(
            inNumSamples, samp = ZXP(in); classification = CheckBadValues_fold_fpclasses(std::fpclassify(samp));
            if (classification != unit->prevclass) {
                if (unit->sameCount == 0) {
                    Print("CheckBadValues: %s found in Synth %d, ID %d\n", CheckBadValues_fpclassString(classification),
                          unit->mParent->mNode.mID, (int)id);
                } else {
                    Print("CheckBadValues: %s found in Synth %d, ID %d (previous %d values were %s)\n",
                          CheckBadValues_fpclassString(classification), unit->mParent->mNode.mID, (int)id,
                          (int)unit->sameCount, CheckBadValues_fpclassString(unit->prevclass));
                };
                unit->sameCount = 0;
            };
            switch (classification) {
                case FP_INFINITE:
                    ZXP(out) = 2;
                    break;
                case FP_NAN:
                    ZXP(out) = 1;
                    break;
                case FP_SUBNORMAL:
                    ZXP(out) = 3;
                    break;
                default:
                    ZXP(out) = 0;
            };
            unit->sameCount++; unit->prevclass = classification;);
        break;
    default: // no post
        LOOP1(
            inNumSamples, samp = ZXP(in); classification = std::fpclassify(samp); switch (classification) {
                case FP_INFINITE:
                    ZXP(out) = 2;
                    break;
                case FP_NAN:
                    ZXP(out) = 1;
                    break;
                case FP_SUBNORMAL:
                    ZXP(out) = 3;
                    break;
                default:
                    ZXP(out) = 0;
            };);
        break;
    }
}

const char* CheckBadValues_fpclassString(int fpclass) {
    switch (fpclass) {
    case FP_NORMAL:
        return "normal";
    case FP_NAN:
        return "NaN";
    case FP_INFINITE:
        return "infinity";
    case FP_ZERO:
        return "zero";
    case FP_SUBNORMAL:
        return "denormal";
    default:
        return "unknown";
    }
}

inline int CheckBadValues_fold_fpclasses(int fpclass) {
    switch (fpclass) {
    case FP_ZERO:
        return FP_NORMAL; // a bit hacky. we mean "zero is fine too".
    default:
        return fpclass;
    }
}

///////////////////////////////////////////////////////////////////////////////////////

void Sanitize_Ctor(Sanitize* unit) {
    if (INRATE(0) == calc_FullRate) {
        if (INRATE(1) == calc_FullRate) {
            SETCALC(Sanitize_next_aa);
        } else {
            SETCALC(Sanitize_next_ak);
        }
    } else {
        SETCALC(Sanitize_next_kk);
    }
    Sanitize_next_kk(unit, 1);
}


void Sanitize_next_aa(Sanitize* unit, int inNumSamples) {
    float* in = IN(0);
    float* replace = IN(1);
    float* out = OUT(0);

    float samp;
    for (int i = 0; i < inNumSamples; i++) {
        samp = in[i];
        switch (std::fpclassify(samp)) {
        case FP_INFINITE:
        case FP_NAN:
        case FP_SUBNORMAL:
            out[i] = replace[i];
            break;
        default:
            out[i] = samp;
        };
    };
}

void Sanitize_next_ak(Sanitize* unit, int inNumSamples) {
    float* in = IN(0);
    float replace = IN0(1);
    float* out = OUT(0);

    float samp;
    for (int i = 0; i < inNumSamples; i++) {
        samp = in[i];
        switch (std::fpclassify(samp)) {
        case FP_INFINITE:
        case FP_NAN:
        case FP_SUBNORMAL:
            out[i] = replace;
            break;
        default:
            out[i] = samp;
        };
    };
}

void Sanitize_next_kk(Sanitize* unit, int inNumSamples) {
    float samp = IN0(0);
    float replace = IN0(1);

    switch (std::fpclassify(samp)) {
    case FP_INFINITE:
    case FP_NAN:
    case FP_SUBNORMAL:
        OUT0(0) = replace;
        break;
    default:
        OUT0(0) = samp;
    };
}

////////////////////////////////////////////////////////////////////

// the load function is called by the host when the plug-in is loaded
PluginLoad(Test) {
    ft = inTable;
    DefineSimpleUnit(CheckBadValues);
    DefineSimpleUnit(Sanitize);
}
