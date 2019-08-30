/*

Spectral statistics UGens for SuperCollider, by Dan Stowell.
Copyright (c) Dan Stowell 2006-2007.
Now part of SuperCollider 3, (c) James McCartney.

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
#include "SCComplex.h"
#include "FFT_UGens.h"

#include "ML.h"

//////////////////////////////////////////////////////////////////////////////////////////////////

/*
struct FFTAnalyser_Unit : Unit
{
    float outval;

    // Not always used: multipliers which convert from bin indices to freq vals, and vice versa.
    // See also the macros for deriving these.
    float m_bintofreq; // , m_freqtobin;
};

struct FFTAnalyser_OutOfPlace : FFTAnalyser_Unit
{
    int m_numbins;
    float *m_tempbuf;
};

struct SpecFlatness : FFTAnalyser_Unit
{
};

struct SpecPcile : FFTAnalyser_OutOfPlace
{
    bool m_interpolate;
};

struct SpecCentroid : FFTAnalyser_Unit
{
};
*/

//////////////////////////////////////////////////////////////////////////////////////////////////


// for operation on one buffer
// just like PV_GET_BUF except it outputs unit->outval rather than -1 when FFT not triggered
#define FFTAnalyser_GET_BUF                                                                                            \
    float fbufnum = ZIN0(0);                                                                                           \
    if (fbufnum < 0.f) {                                                                                               \
        ZOUT0(0) = unit->outval;                                                                                       \
        return;                                                                                                        \
    }                                                                                                                  \
    ZOUT0(0) = fbufnum;                                                                                                \
    uint32 ibufnum = (uint32)fbufnum;                                                                                  \
    World* world = unit->mWorld;                                                                                       \
    SndBuf* buf;                                                                                                       \
    if (!(ibufnum < world->mNumSndBufs)) {                                                                             \
        int localBufNum = ibufnum - world->mNumSndBufs;                                                                \
        Graph* parent = unit->mParent;                                                                                 \
        if (!(localBufNum > parent->localBufNum)) {                                                                    \
            buf = parent->mLocalSndBufs + localBufNum;                                                                 \
        } else {                                                                                                       \
            buf = world->mSndBufs;                                                                                     \
        }                                                                                                              \
    } else {                                                                                                           \
        buf = world->mSndBufs + ibufnum;                                                                               \
    }                                                                                                                  \
    LOCK_SNDBUF(buf);                                                                                                  \
    int numbins = (buf->samples - 2) >> 1;

// Copied from FFT_UGens.cpp
#define GET_BINTOFREQ                                                                                                  \
    if (unit->m_bintofreq == 0.f) {                                                                                    \
        unit->m_bintofreq = world->mFullRate.mSampleRate / buf->samples;                                               \
    }                                                                                                                  \
    float bintofreq = unit->m_bintofreq;
/*
#define GET_FREQTOBIN \
    if(unit->m_freqtobin==0.f){ \
        unit->m_freqtobin = buf->samples / world->mFullRate.mSampleRate; \
    } \
    float freqtobin = unit->m_freqtobin;
*/

//////////////////////////////////////////////////////////////////////////////////////////////////
/*
extern "C"
{
    void SpecFlatness_Ctor(SpecFlatness *unit);
    void SpecFlatness_next(SpecFlatness *unit, int inNumSamples);

    void SpecPcile_Ctor(SpecPcile *unit);
    void SpecPcile_next(SpecPcile *unit, int inNumSamples);
    void SpecPcile_Dtor(SpecPcile *unit);

    void SpecCentroid_Ctor(SpecCentroid *unit);
    void SpecCentroid_next(SpecCentroid *unit, int inNumSamples);

}
*/
/*
SCPolarBuf* ToPolarApx(SndBuf *buf)
{
    if (buf->coord == coord_Complex) {
        SCComplexBuf* p = (SCComplexBuf*)buf->data;
        int numbins = buf->samples - 2 >> 1;
        for (int i=0; i<numbins; ++i) {
            p->bin[i].ToPolarApxInPlace();
        }
        buf->coord = coord_Polar;
    }

    return (SCPolarBuf*)buf->data;
}

SCComplexBuf* ToComplexApx(SndBuf *buf)
{
    if (buf->coord == coord_Polar) {
        SCPolarBuf* p = (SCPolarBuf*)buf->data;
        int numbins = buf->samples - 2 >> 1;
        for (int i=0; i<numbins; ++i) {
            p->bin[i].ToComplexApxInPlace();
        }
        buf->coord = coord_Complex;
    }
    return (SCComplexBuf*)buf->data;
}

InterfaceTable *ft;

void init_SCComplex(InterfaceTable *inTable);
*/
//////////////////////////////////////////////////////////////////////////////////////////////////

void SpecFlatness_Ctor(SpecFlatness* unit) {
    SETCALC(SpecFlatness_next);
    ZOUT0(0) = unit->outval = 0.;
    unit->m_oneovern = 0.;
}

void SpecFlatness_next(SpecFlatness* unit, int inNumSamples) {
    FFTAnalyser_GET_BUF if (unit->m_oneovern == 0.) unit->m_oneovern = 1. / (numbins + 2);

    SCComplexBuf* p = ToComplexApx(buf);

    // Spectral Flatness Measure is geometric mean divided by arithmetic mean.
    //
    // In order to calculate geom mean without hitting the precision limit,
    //  we use the trick of converting to log, taking the average, then converting back from log.
    double geommean = std::log(sc_abs(p->dc)) + std::log(sc_abs(p->nyq));
    double mean = sc_abs(p->dc) + sc_abs(p->nyq);

    for (int i = 0; i < numbins; ++i) {
        float rabs = (p->bin[i].real);
        float iabs = (p->bin[i].imag);
        float amp = std::sqrt((rabs * rabs) + (iabs * iabs));
        if (amp != 0.f) { // zeroes lead to NaNs
            geommean += std::log(amp);
            mean += amp;
        }
    }

    double oneovern = unit->m_oneovern;
    geommean = exp(geommean * oneovern); // Average and then convert back to linear
    mean *= oneovern;

    // Store the val for output in future calls
    unit->outval = (mean == 0.f ? 0.8f : (geommean / mean));
    // Note: for silence the value is undefined.
    // Here, for silence we instead output an empirical value based on very quiet white noise.

    ZOUT0(0) = unit->outval;
}

////////////////////////////////////////////////////////////////////////////////////

void SpecPcile_Ctor(SpecPcile* unit) {
    SETCALC(SpecPcile_next);

    unit->m_interpolate = ZIN0(2) > 0.f;

    ZOUT0(0) = unit->outval = 0.;
    unit->m_tempbuf = 0;
}

void SpecPcile_next(SpecPcile* unit, int inNumSamples) {
    FFTAnalyser_GET_BUF

        // Used to be MAKE_TEMP_BUF but we can handle it more cleanly in this specific case:
        if (!unit->m_tempbuf) {
        unit->m_tempbuf = (float*)RTAlloc(unit->mWorld, numbins * sizeof(float));
        unit->m_numbins = numbins;
        unit->m_halfnyq_over_numbinsp2 = ((float)unit->mWorld->mSampleRate) * 0.5f / (float)(numbins + 2);
    }
    else if (numbins != unit->m_numbins) return;

    // Percentile value as a fraction. eg: 0.5 == 50-percentile (median).
    float fraction = ZIN0(1);
    bool interpolate = unit->m_interpolate;

    // The magnitudes in *p will be converted to cumulative sum values and stored in *q temporarily
    SCComplexBuf* p = ToComplexApx(buf);
    float* q = (float*)unit->m_tempbuf;

    float cumul = sc_abs(p->dc);

    for (int i = 0; i < numbins; ++i) {
        float real = p->bin[i].real;
        float imag = p->bin[i].imag;
        cumul += std::sqrt(real * real + imag * imag);

        // A convenient place to store the mag values...
        q[i] = cumul;
    }

    cumul += sc_abs(p->nyq);

    float target = cumul * fraction; // The target cumul value, stored somewhere in q

    float bestposition = 0; // May be linear-interpolated between bins, but not implemented yet
                            // NB If nothing beats the target (e.g. if fraction is -1) zero Hz is returned
    float binpos;
    for (int i = 0; i < numbins; ++i) {
        // Print("Testing %g, at position %i", q->bin[i].real, i);
        if (!(q[i] < target)) { // this is a ">=" comparison, done more efficiently as "!(<)"
            if (interpolate && i != 0) {
                binpos = ((float)i) + 1.f - (q[i] - target) / (q[i] - q[i - 1]);
            } else {
                binpos = ((float)i) + 1.f;
            }
            bestposition = binpos * unit->m_halfnyq_over_numbinsp2;
            // Print("Target %g beaten by %g (at position %i), equating to freq %g\n",
            //				target, p->bin[i].real, i, bestposition);
            break;
        }
    }

    // Store the val for output in future calls
    unit->outval = bestposition;

    ZOUT0(0) = unit->outval;
}


void SpecPcile_Dtor(SpecPcile* unit) {
    if (unit->m_tempbuf)
        RTFree(unit->mWorld, unit->m_tempbuf);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void SpecCentroid_Ctor(SpecCentroid* unit) {
    SETCALC(SpecCentroid_next);
    ZOUT0(0) = unit->outval = 0.;

    unit->m_bintofreq = 0.f;
}

void SpecCentroid_next(SpecCentroid* unit, int inNumSamples) {
    FFTAnalyser_GET_BUF

        SCPolarBuf* p = ToPolarApx(buf);

    GET_BINTOFREQ


    double num = sc_abs(p->nyq) * (numbins + 1);
    double denom = sc_abs(p->nyq);

    for (int i = 0; i < numbins; ++i) {
        num += sc_abs(p->bin[i].mag) * (i + 1);
        denom += sc_abs(p->bin[i].mag);
    }

    ZOUT0(0) = unit->outval = denom == 0.0 ? 0.f : (float)(bintofreq * num / denom);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
/*
void load(InterfaceTable *inTable)
{
    ft= inTable;

    //(*ft->fDefineUnit)("SpecFlatness", sizeof(FFTAnalyser_Unit), (UnitCtorFunc)&SpecFlatness_Ctor, 0, 0);
    //(*ft->fDefineUnit)("SpecPcile", sizeof(SpecPcile_Unit), (UnitCtorFunc)&SpecPcile_Ctor,
(UnitDtorFunc)&SpecPcile_Dtor, 0);

    DefineSimpleUnit(SpecFlatness);
    DefineDtorUnit(SpecPcile);
    DefineSimpleUnit(SpecCentroid);
}
*/
