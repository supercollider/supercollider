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

// BeatTrack UGen implemented by Nick Collins (http://www.informatics.sussex.ac.uk/users/nc81/)
// post FFT UGen version 1 Nov 2007

// conversion of Matthew Davies autocorrelation beat tracking model, adapted for real-time use
// currently using QMUL complex domain onset detection function model

//#include "SC_PlugIn.h"
//#include <vecLib/vecLib.h>
//#include <string.h>
//#include <math.h>
//#include <stdlib.h>
//#include <stdio.h>

#include "ML.h"

// FFT data
//#define N 1024  //FFT size
// FFT size over 2
#define NOVER2 512
//#define NOVER4 256  //FFT size
//#define OVERLAP 512
//#define OVERLAPINDEX 512
//#define HOPSIZE 512

//#define FS 44100 //assumes fixed sampling rate
//#define FRAMESR 86.1328
// converted for different sampling rates
#define FRAMEPERIOD 0.01161
#define SKIP 128
//#define TIMEELAPSED 1.48608

// this data assumes LAGS is 128
static float g_m[128] = {
    0.00054069, 0.00108050, 0.00161855, 0.00215399, 0.00268594, 0.00321356, 0.00373600, 0.00425243, 0.00476204,
    0.00526404, 0.00575765, 0.00624213, 0.00671675, 0.00718080, 0.00763362, 0.00807455, 0.00850299, 0.00891836,
    0.00932010, 0.00970771, 0.01008071, 0.01043866, 0.01078115, 0.01110782, 0.01141834, 0.01171242, 0.01198982,
    0.01225033, 0.01249378, 0.01272003, 0.01292899, 0.01312061, 0.01329488, 0.01345182, 0.01359148, 0.01371396,
    0.01381939, 0.01390794, 0.01397980, 0.01403520, 0.01407439, 0.01409768, 0.01410536, 0.01409780, 0.01407534,
    0.01403838, 0.01398734, 0.01392264, 0.01384474, 0.01375410, 0.01365120, 0.01353654, 0.01341062, 0.01327397,
    0.01312710, 0.01297054, 0.01280484, 0.01263053, 0.01244816, 0.01225827, 0.01206139, 0.01185807, 0.01164884,
    0.01143424, 0.01121478, 0.01099099, 0.01076337, 0.01053241, 0.01029861, 0.01006244, 0.00982437, 0.00958484,
    0.00934429, 0.00910314, 0.00886181, 0.00862067, 0.00838011, 0.00814049, 0.00790214, 0.00766540, 0.00743057,
    0.00719793, 0.00696778, 0.00674036, 0.00651591, 0.00629466, 0.00607682, 0.00586256, 0.00565208, 0.00544551,
    0.00524301, 0.00504470, 0.00485070, 0.00466109, 0.00447597, 0.00429540, 0.00411944, 0.00394813, 0.00378151,
    0.00361959, 0.00346238, 0.00330989, 0.00316210, 0.00301899, 0.00288053, 0.00274669, 0.00261741, 0.00249266,
    0.00237236, 0.00225646, 0.00214488, 0.00203755, 0.00193440, 0.00183532, 0.00174025, 0.00164909, 0.00156174,
    0.00147811, 0.00139810, 0.00132161, 0.00124854, 0.00117880, 0.00111228, 0.00104887, 0.00098848, 0.00093100,
    0.00087634, 0.00082438,
};
static float g_mg[257] = {
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000004, 0.00000055, 0.00000627, 0.00005539, 0.00037863, 0.00200318, 0.00820201, 0.02599027, 0.06373712,
    0.12096648, 0.17767593, 0.20196826, 0.17767593, 0.12096648, 0.06373712, 0.02599027, 0.00820201, 0.00200318,
    0.00037863, 0.00005539, 0.00000627, 0.00000055, 0.00000004, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
    0.00000000, 0.00000000, 0.00000000, 0.00000000, 0.00000000,
};

// other functions
static void BeatTrack_dofft(BeatTrack* unit, uint32);
static void complexdf(BeatTrack* unit);
static void finaldecision(BeatTrack* unit);

// amortisation
static void autocorr(BeatTrack* unit, int j);
static void beatperiod(BeatTrack* unit, int j, int whichm);
static float findtor(BeatTrack* unit);

// as many amortisation steps as tor
static void findphase(BeatTrack* unit, int j, int gaussflag, int predicted);

static int detectperiodchange(BeatTrack* unit);
static void findmeter(BeatTrack* unit);
static void setupphaseexpectation(BeatTrack* unit); // create Gaussian focussed matrix for phase


void BeatTrack_Ctor(BeatTrack* unit) {
    ///////
    // check sampling rate and establish multipliers on estimates and FFT window size
    // down sampling by factor of two automatic

    unit->m_srate = unit->mWorld->mFullRate.mSampleRate;

    // if sample rate is 88200 or 96000, assume taking double size FFT to start with
    if (unit->m_srate > (44100.0 * 1.5))
        unit->m_srate = unit->m_srate * 0.5;

    unit->m_srateconversion = unit->m_srate / 44100.0;

    // assumes base of 1024 FFT
    unit->m_frameperiod =
        (FRAMEPERIOD / unit->m_srateconversion); // in seconds //(int) ((FRAMEPERIOD/unit->m_srateconversion) +0.5);

    printf("srate %f conversion factor %f frame period %f \n", unit->m_srate, unit->m_srateconversion,
           unit->m_frameperiod);

    unit->m_prevmag = (float*)RTAlloc(unit->mWorld, NOVER2 * sizeof(float));
    unit->m_prevphase = (float*)RTAlloc(unit->mWorld, NOVER2 * sizeof(float));
    unit->m_predict = (float*)RTAlloc(unit->mWorld, NOVER2 * sizeof(float));

    ////////time positions//////////
    unit->m_frame = 1; // don't decide immediately, wait for maximum period!

    /////////df////////
    unit->m_dfcounter = DFSTORE - 1;
    // random uncorrelated noise df store for initialisation
    // RGen& rgen = *unit->mParent->mRGen;

    // don't want this noise, want consistent starting point!
    for (int j = 0; j < DFSTORE; ++j) {
        unit->m_df[j] = 0.0; //(2*rgen.frand() - 1.0);
    }

    unit->m_dfmemorycounter = 14;

    Clear(15, unit->m_dfmemory);

    /////////tempo assess///////////
    unit->m_currtempo = 2;

    ////////phase assess///////////

    unit->m_currphase = 0.0;

    unit->m_phase = 0.0;

    // default of 2bps
    unit->m_phaseperblock = ((float)unit->mWorld->mFullRate.mBufLength * 2) / ((float)unit->mWorld->mSampleRate);

    unit->m_outputphase = unit->m_phase;
    unit->m_outputtempo = unit->m_currtempo;
    unit->m_outputphaseperblock = unit->m_phaseperblock;

    unit->halftrig = 0;
    unit->q1trig = 0;
    unit->q2trig = 0;

    // amortisation and states
    unit->m_amortisationstate = 0; // off
    unit->m_stateflag = 0;
    unit->m_timesig = 4;
    unit->m_flagstep = 0;

    unit->mCalcFunc = (UnitCalcFunc)&BeatTrack_next;

    // initialize outputs
    ZOUT0(0) = 0.0;
    ZOUT0(1) = 0.0;
    ZOUT0(2) = 0.0;
    ZOUT0(3) = unit->m_outputtempo;
}


void BeatTrack_Dtor(BeatTrack* unit) {
    RTFree(unit->mWorld, unit->m_prevmag);
    RTFree(unit->mWorld, unit->m_prevphase);
    RTFree(unit->mWorld, unit->m_predict);
}


void BeatTrack_next(BeatTrack* unit, int wrongNumSamples) {
    // float *in = IN(0);

    // printf("%d \n",wrongNumSamples);
    // int numSamples = unit->mWorld->mFullRate.mBufLength;

    // conditions in reverse order to avoid immediate spillover
    // printf("state %d \n",unit->m_amortisationstate);

    // keeps incrementing but will be reset with each calculation run
    unit->m_amortisationsteps = unit->m_amortisationsteps + 1;

    // if state nonzero do something
    switch (unit->m_amortisationstate) {
    case 0:
        break; // do nothing case
    case 1: // calculate acf
        autocorr(unit, unit->m_amortcount);

        unit->m_amortcount = unit->m_amortcount + 1;

        if (unit->m_amortcount == unit->m_amortlength) {
            unit->m_amortisationstate = 2;
            unit->m_amortlength = 128;
            unit->m_amortcount = 0;

            unit->m_bestcolumn = 0;
            unit->m_besttorsum = -1000.0;
        }

        break;
    case 2: // periodp
        beatperiod(unit, unit->m_amortcount, 0);

        unit->m_amortcount = unit->m_amortcount + 1;

        if (unit->m_amortcount == unit->m_amortlength) {
            unit->m_periodp = findtor(unit);

            if (unit->m_stateflag == 1) {
                unit->m_amortisationstate = 3;
                unit->m_amortlength = 128;
                unit->m_amortcount = 0;

                unit->m_bestcolumn = 0;
                unit->m_besttorsum = -1000.0;

            } else {
                unit->m_periodg = -1000; // will always trigger initially
                unit->m_amortisationstate = 4;
            }
        }

        break;
    case 3: // periodg
        beatperiod(unit, unit->m_amortcount, 1);
        unit->m_amortcount = unit->m_amortcount + 1;

        if (unit->m_amortcount == unit->m_amortlength) {
            unit->m_periodg = findtor(unit);

            unit->m_amortisationstate = 4;
        }

        break;
    case 4: // stepdetect/constdetect

        if (detectperiodchange(unit)) {
            unit->m_amortisationstate = 5;
            unit->m_amortlength = 128;
            unit->m_amortcount = 0;

            unit->m_bestcolumn = 0;
            unit->m_besttorsum = -1000.0;

            unit->m_stateflag = 1;
            findmeter(unit);

            // set up Gaussian weighting centred on periodp
            int startindex = 128 - ((int)(unit->m_periodp + 0.5));

            float* mg = unit->m_mg;

            for (int ii = 0; ii < 128; ++ii) {
                mg[ii] = g_mg[startindex + ii];
            }

        } else {
            if (unit->m_stateflag == 1)
                unit->m_tor = unit->m_periodg;
            else
                unit->m_tor = unit->m_periodp;

            unit->m_torround = int(unit->m_tor + 0.5);

            unit->m_amortisationstate = 7;
            unit->m_amortlength = unit->m_torround;
            unit->m_amortcount = 0;
        }

        break;

    case 5: // redo periodg calculation
        beatperiod(unit, unit->m_amortcount, 1);
        unit->m_amortcount = unit->m_amortcount + 1;

        if (unit->m_amortcount == unit->m_amortlength) {
            unit->m_periodg = findtor(unit);

            unit->m_tor = unit->m_periodg;
            unit->m_torround = int(unit->m_tor + 0.5f);

            unit->m_amortisationstate = 6;
            unit->m_amortlength = unit->m_torround;
            unit->m_amortcount = 0;

            setupphaseexpectation(unit);

            // don't need to reset change flag since it isn't stored
        }

        break;
    case 6: // flat phase calc after move to context, avoids bias
        findphase(unit, unit->m_amortcount, 0, 0);
        unit->m_amortcount = unit->m_amortcount + 1;

        if (unit->m_amortcount == unit->m_amortlength) {
            unit->m_amortisationstate = 8; // final state
        }

        break;

    case 7: // phase calc with possible gaussian narrowing of the allowed phases

        findphase(unit, unit->m_amortcount, unit->m_stateflag, (int)(unit->m_currphase * unit->m_torround + 0.5f));
        unit->m_amortcount = unit->m_amortcount + 1;

        if (unit->m_amortcount == unit->m_amortlength) {
            unit->m_amortisationstate = 8; // final state
        }

        break;
    case 8:

        finaldecision(unit);
        unit->m_amortisationstate = 0;
        break;

    default:
        break;
    }


    // MUST CHECK IF INCIDENT FFT IS >1, if so update buffer with appropriate coefficients

    float fbufnum = ZIN0(0);

    // next FFT bufffer ready, update
    // assuming at this point that buffer precalculated for any resampling
    if (!(fbufnum < 0)) {
        unit->m_frame = unit->m_frame + 1;
        BeatTrack_dofft(unit, (uint32)fbufnum);
    }


    // test if impulse to output
    unit->m_phase += unit->m_phaseperblock;

    // if not locked, update output phase from model phase, else keep a separate output phase

    float lock = ZIN0(1);
    // printf("lock %f \n",lock);

    if (lock < 0.5f) {
        unit->m_outputphase = unit->m_phase;
        unit->m_outputtempo = unit->m_currtempo;
        unit->m_outputphaseperblock = unit->m_phaseperblock;
    } else
        unit->m_outputphase += unit->m_outputphaseperblock;

    if (unit->m_phase >= 1.f)
        unit->m_phase -= 1.f;

    // 0 is beat, 1 is quaver, 2 is semiquaver, 3 is actual current tempo in bps
    // so no audio accuracy with beats, just asap, may as well be control rate
    ZOUT0(0) = 0.0;
    ZOUT0(1) = 0.0;
    ZOUT0(2) = 0.0;
    ZOUT0(3) = unit->m_outputtempo; //*0.016666667;

    // output beat
    if (unit->m_outputphase >= 1.f) {
        // printf("beat \n");

        unit->m_outputphase -= 1.f;
        ZOUT0(0) = 1.0;
        ZOUT0(1) = 1.0;
        ZOUT0(2) = 1.0;
        unit->halftrig = 0;
        unit->q1trig = 0;
        unit->q2trig = 0;
    }

    if (unit->m_outputphase >= 0.5f && unit->halftrig == 0) {
        ZOUT0(1) = 1.0;
        ZOUT0(2) = 1.0;
        unit->halftrig = 1;
    }

    if (unit->m_outputphase >= 0.25f && unit->q1trig == 0) {
        ZOUT0(2) = 1.0;
        unit->q1trig = 1;
    }

    if (unit->m_outputphase >= 0.75f && unit->q2trig == 0) {
        ZOUT0(2) = 1.0;
        unit->q2trig = 1;
    }
}


//

// calculation function once FFT data ready
void BeatTrack_dofft(BeatTrack* unit, uint32 ibufnum) {
    World* world = unit->mWorld;
    SndBuf* buf;
    if (ibufnum >= world->mNumSndBufs) {
        int localBufNum = ibufnum - world->mNumSndBufs;
        Graph* parent = unit->mParent;
        if (localBufNum <= parent->localBufNum) {
            buf = parent->mLocalSndBufs + localBufNum;
        } else {
            buf = world->mSndBufs;
        }
    } else {
        buf = world->mSndBufs + ibufnum;
    }
    LOCK_SNDBUF(buf);
    // int numbins = buf->samples - 2 >> 1;

    unit->m_FFTBuf = buf->data; // just assign it!

    // transfer data to fftbuf in the format expected by this plugin

    // ideally, should do this part separate to plug-in as well, so can compare different detection functions;
    // also, can run multiple in parallel with own autocorrelations; committee? Committee.ar(period1, phase1, period2,
    // phase2, period3, phase3)... chooses predominant estimate? feature detection function
    complexdf(unit);

    if (unit->m_frame % SKIP == 0) {
        // printf("amortisation time \n");

        // amortisation- 8 control periods in a frame
        // have 2000 calcs to do, split over 100 control periods = 6400 samples, ie one tempo per control period

        unit->m_bestcolumn = 0;
        unit->m_besttorsum = -1000.0;

        unit->m_bestphasescore = -1000.0;
        unit->m_bestphase = 0;

        // state 0 is do nothing
        unit->m_amortisationstate = 1;
        unit->m_amortcount = 0;
        unit->m_amortlength = 128;
        unit->m_amortisationsteps = 0;

        // fix time reference for calculations, so it doesn't update during the amortisation- this is the beginning of
        // the df frame
        unit->m_storedfcounter = unit->m_dfcounter + DFSTORE - DFFRAMELENGTH;

        // ref for phase calculations
        unit->m_storedfcounterend = unit->m_dfcounter;

        // unit->m_fftstoreposhold= unit->m_fftstorepos;

        unit->m_currphase = unit->m_phase;
    }
}


void autocorr(BeatTrack* unit, int j) {
    int baseframe = unit->m_storedfcounter + DFSTORE;
    float* df = unit->m_df;
    float* acf = unit->m_acf;

    // work out four lags each time
    for (int k = 0; k < 4; ++k) {
        int lag = 4 * j + k;

        int correction = abs(lag - DFFRAMELENGTH);

        float sum = 0.0;

        for (int i = lag; i < DFFRAMELENGTH; ++i) {
            float val1 = df[(i + baseframe) % DFSTORE];
            float val2 = df[(i + baseframe - lag) % DFSTORE];

            sum += val1 * val2;
        }

        acf[lag] = sum * correction;
    }
}


// timesig 4 has one more sum term
// indices as MATLAB but need to correct maxinds to be in range of tested, not in global range
float findtor(BeatTrack* unit) {
    float maxval, val;
    int ind2, ind3, ind4;

    // put into MATLAB indexing, from 1 to 512
    int ind = unit->m_bestcolumn + 1;

    float* acf = unit->m_acf - 1;

    ind2 = 0;
    maxval = -1000;

    for (int i = 2 * ind - 1; i <= (2 * ind + 1); ++i) {
        val = acf[i];

        if (val > maxval) {
            maxval = val;
            ind2 = i - (2 * ind - 1) + 1;
        }
    }

    //[val2,ind2] = max(acf(2*ind-1:2*ind+1));
    ind2 = ind2 + 2 * (ind + 1) - 2;

    ind3 = 0;
    maxval = -1000;

    for (int i = 3 * ind - 2; i <= (3 * ind + 2); ++i) {
        val = acf[i];

        if (val > maxval) {
            maxval = val;
            ind3 = i - (3 * ind - 2) + 1;
        }
    }

    //[val3,ind3] = max(acf(3*ind-2:3*ind+2));
    ind3 = ind3 + 3 * ind - 4;

    float period;

    if (unit->m_timesig == 4) {
        ind4 = 0;
        maxval = -1000;

        for (int i = 4 * ind - 3; i <= 4 * ind + 3; ++i) {
            val = acf[i];

            if (val > maxval) {
                maxval = val;
                ind4 = i - (4 * ind - 3) + 1;
            }
        }

        //[val4,ind4] = max(acf(4*ind-3:4*ind+3));
        ind4 = ind4 + 4 * ind - 9;

        period = (ind + ind2 * 0.5 + ind3 / 3.f + ind4 * 0.25) * 0.25;

    } else

        period = (ind + ind2 * 0.5 + ind3 / 3.f) * 0.3333333;


    // printf("period %f ind %d ind2 %d ind3 %d ind4 %d \n",period, ind,ind2,ind3,ind4);

    // unit->m_tor=period;
    // unit->m_torround= int(period+0.5);
    //

    return period;
}


// 128 calculation calls for multiplying M and acf, calculates M as it goes apart from precalculated Gaussian or Raleigh
// distribution
void beatperiod(BeatTrack* unit, int j, int whichm) {
    float* acf = unit->m_acf;

    // int startindex= 512*j;
    // int endindex=startindex+512;

    float sum = 0.0;

    // unit->m_timesig harmonics
    for (int i = 1; i <= (unit->m_timesig); ++i) {
        int num = 2 * i - 1;
        float wt = 1.0 / (float)num;

        for (int k = 0; k < num; ++k) {
            int pos = k + (i * j);

            // m[startindex+pos]
            if (pos < 512)
                sum += acf[pos] * wt;
        }
    }

    // assumes Mg appropriately rotated already
    float* m;

    if (whichm)
        m = g_m; // Gaussian weighted context model
    else
        m = unit->m_mg; // general model even weighting

    sum = sum * m[j];

    if (sum > unit->m_besttorsum) {
        unit->m_besttorsum = sum;
        unit->m_bestcolumn = j;
    }
}


// j out of unit->m_torround
// differs to Davies original in that weight the most recent events more- want minimum reaction time
void findphase(BeatTrack* unit, int j, int gaussflag, int predicted) {
    float* df = unit->m_df;

    int period = unit->m_torround;
    int baseframe = unit->m_storedfcounterend + DFSTORE;

    int numfit = -1;

    if (period != 0)
        // round down
        numfit = (int)(DFFRAMELENGTH / period) - 1;

    // testing backwards from the baseframe, weighting goes down as 1/k
    float sum = 0.0;

    for (int k = 0; k < numfit; ++k) {
        // j is phase to test
        int location = (baseframe - (period * k) - j) % DFSTORE;

        sum += df[location] / ((float)(k + 1));
    }

    // Gaussian focus weighting if desired
    if (gaussflag) {
        // difference of predicted from j, min distance within period
        int diff = sc_min(abs(predicted - j), abs(period - predicted + j));

        sum *= unit->m_phaseweights[diff];
    }

    if (sum > unit->m_bestphasescore) {
        unit->m_bestphasescore = sum;
        unit->m_bestphase = j;
    }
}

//, int predicted
void setupphaseexpectation(BeatTrack* unit) // create Gaussian focussed matrix for phase
{
    float* wts = unit->m_phaseweights;

    float sigma = unit->m_torround * 0.25f;
    // float mu=period;

    float mult = 1.0 / (2.5066283 * sigma);
    float mult2 = 1.0 / (2.0 * sigma * sigma);

    // unit->m_torround
    for (int i = 0; i < 128; ++i) {
        wts[i] = mult * (exp(-(i * i) * mult2));
    }
}


// why force a countdown each time? Why not keep a continuous buffer of previous periodp, periodg?
int detectperiodchange(BeatTrack* unit) {
    // stepthresh = 3.9017;

    if (unit->m_flagstep == 0) {
        if (fabs(unit->m_periodg - unit->m_periodp) > 3.9017f) {
            unit->m_flagstep = 3;
        }

    } else {
        unit->m_flagstep = unit->m_flagstep - 1;
    }

    if (unit->m_flagstep) {
        unit->m_prevperiodp[unit->m_flagstep - 1] = unit->m_periodp;
    }

    if (unit->m_flagstep == 1) {
        unit->m_flagstep = 0;

        if (fabs(2 * unit->m_prevperiodp[0] - unit->m_prevperiodp[1] - unit->m_prevperiodp[2]) < 7.8034f) //(2*3.9017)
            return 1;
    }

    return 0;
}

// add test
void findmeter(BeatTrack* unit) {
    // int i;

    // float * acf= unit->m_acf;

    //	float * acf= unit->m_acf-1;
    //
    //
    //	int period = ((int)(unit->m_periodp+0.5));
    //
    //	float three_energy=0.0;
    //	float four_energy=0.0;
    //
    //	for(i=(3*period-2);i<(3*period+3);++i)
    //		three_energy += acf[i];
    //
    //	for(i=(4*period-2);i<(4*period+3);++i)
    //		four_energy += acf[i];
    //
    //	if((6*period+2)<512) {
    //
    //		for(i=(6*period-2);i<(6*period+3);++i)
    //			three_energy += acf[i];
    //
    //		for(i=(2*period-2);i<(2*period+3);++i)
    //			four_energy += acf[i];
    //	}
    //
    //	if (three_energy > four_energy)
    //		unit->m_timesig = 3;
    //	else

    // worked better in evaluation without any 3/4 at all!
    unit->m_timesig = 4;

    // printf("time sig %d \n",unit->m_timesig);
}


// period is unit->m_tor, phase is unit->m_bestphase
//	float m_tor; int m_torround;
void finaldecision(BeatTrack* unit) {
    // int i,j;

    unit->m_currtempo = 1.0 / (unit->m_tor * unit->m_frameperiod);

    unit->m_phaseperblock =
        ((float)unit->mWorld->mFullRate.mBufLength * (unit->m_currtempo)) / ((float)unit->mWorld->mSampleRate);

    // printf("SAMPLErate %f %f %f", unit->mWorld->mSampleRate,unit->m_phaseperblock,unit->m_currtempo);

    // unit->m_amortisationstate control periods worth = 512/64 = 8
    // float frameselapsed= 0.125*unit->m_amortisationstate;
    // float timeelapsed= frameselapsed*unit->m_frameperiod;

    float timeelapsed = ((float)(unit->m_amortisationsteps) * (unit->mWorld->mFullRate.mBufLength)
                         / ((float)unit->mWorld->mSampleRate));

    timeelapsed += 7 * unit->m_frameperiod; // compensation for detection function being delayed by 7 frames

    float phaseelapsed = timeelapsed * (unit->m_currtempo);

    float phasebeforeamort = ((float)unit->m_bestphase / unit->m_torround);

    // add phase to compensate for ELAPSEDTIME
    unit->m_currphase = unit->m_phase = fmod(phasebeforeamort + phaseelapsed, (float)1.0);
}


// Now the format is standardised for the SC FFT UGen as
// dc, nyquist and then real/imag pairs for each bin going up successively in frequency

void complexdf(BeatTrack* unit) {
    float* fftbuf = unit->m_FFTBuf;

    float* prevmag = unit->m_prevmag;
    float* prevphase = unit->m_prevphase;
    float* predict = unit->m_predict;

    float sum = 0.0;

    // printf("complex df time \n");

    // sum bins 2 to 256
    for (int k = 1; k < NOVER2; ++k) {
        // Change to fftw
        int index = 2 * k; // k; //2*k;

        float real = fftbuf[index];
        // N=1024 conventionally here
        float imag = fftbuf[index + 1]; // fftbuf[N-index];

        float mag = sqrt(
            real * real
            + imag * imag); // was  0.5*sqrt(real*real+ imag*imag); reduce by factor of 2 because of altivec side effect
        float qmag = prevmag[k];

        prevmag[k] = mag;

        float phase = atan2(imag, real);
        float oldphase = predict[k];

        predict[k] = 2 * phase - prevphase[k];
        prevphase[k] = phase;

        float phasediff = phase - oldphase;

        // if(k==2) printf("%f %f\n",phase, phasediff);

        // tables for cos/sin/sqrt speeds up? sqrt(1-c*c) slower than sin

        float realpart = (qmag - (mag * cos(phasediff)));
        float imagpart = (mag * sin(phasediff)); // no need for negative

        float detect = sqrt(realpart * realpart + imagpart * imagpart);

        // detect is always positive
        // if(k==1)
        sum += detect; //(fmod(phase+(16*pi),twopi)); //detect;

        // if(k==1) sum+=mag;
    }


    // smoothing and peak picking operation, delay of 8 frames, must be taken account of in final phase correction

    unit->m_dfmemorycounter = (unit->m_dfmemorycounter + 1) % 15;
    unit->m_dfmemory[unit->m_dfmemorycounter] = sum; // divide by num of bands to get a dB answer

    float rating = 0.0;

    float* dfmemory = unit->m_dfmemory;

    int refpos = unit->m_dfmemorycounter + 15;
    int centrepos = (refpos - 7) % 15;
    float centreval = dfmemory[centrepos];

    for (int k = 0; k < 15; ++k) {
        int pos = (refpos - k) % 15;

        float nextval = centreval - dfmemory[pos];

        if (nextval < 0.0)
            nextval = nextval * 10;

        rating += nextval;
    }

    if (rating < 0.0)
        rating = 0.0;

    // increment first so this frame is unit->m_loudnesscounterdfcounter
    unit->m_dfcounter = (unit->m_dfcounter + 1) % DFSTORE;

    unit->m_df[unit->m_dfcounter] = rating * 0.1f; // sum //divide by num of bands to get a dB answer
}
