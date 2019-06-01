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

// BeatTrack2 UGen implemented by Nick Collins (http://www.informatics.sussex.ac.uk/users/nc81/)
// 6 Nov 2007

#include "ML.h"

// need to add bestgroove option to store groove, else remove output which is currently always straight 16ths


static const int g_numtempi = 120;
static float g_periods[g_numtempi] = { 1,
                                       0.98360655737705,
                                       0.96774193548387,
                                       0.95238095238095,
                                       0.9375,
                                       0.92307692307692,
                                       0.90909090909091,
                                       0.8955223880597,
                                       0.88235294117647,
                                       0.8695652173913,
                                       0.85714285714286,
                                       0.84507042253521,
                                       0.83333333333333,
                                       0.82191780821918,
                                       0.81081081081081,
                                       0.8,
                                       0.78947368421053,
                                       0.77922077922078,
                                       0.76923076923077,
                                       0.75949367088608,
                                       0.75,
                                       0.74074074074074,
                                       0.73170731707317,
                                       0.72289156626506,
                                       0.71428571428571,
                                       0.70588235294118,
                                       0.69767441860465,
                                       0.68965517241379,
                                       0.68181818181818,
                                       0.67415730337079,
                                       0.66666666666667,
                                       0.65934065934066,
                                       0.65217391304348,
                                       0.64516129032258,
                                       0.63829787234043,
                                       0.63157894736842,
                                       0.625,
                                       0.61855670103093,
                                       0.61224489795918,
                                       0.60606060606061,
                                       0.6,
                                       0.59405940594059,
                                       0.58823529411765,
                                       0.58252427184466,
                                       0.57692307692308,
                                       0.57142857142857,
                                       0.56603773584906,
                                       0.5607476635514,
                                       0.55555555555556,
                                       0.55045871559633,
                                       0.54545454545455,
                                       0.54054054054054,
                                       0.53571428571429,
                                       0.53097345132743,
                                       0.52631578947368,
                                       0.52173913043478,
                                       0.51724137931034,
                                       0.51282051282051,
                                       0.50847457627119,
                                       0.50420168067227,
                                       0.5,
                                       0.49586776859504,
                                       0.49180327868852,
                                       0.48780487804878,
                                       0.48387096774194,
                                       0.48,
                                       0.47619047619048,
                                       0.47244094488189,
                                       0.46875,
                                       0.46511627906977,
                                       0.46153846153846,
                                       0.45801526717557,
                                       0.45454545454545,
                                       0.45112781954887,
                                       0.44776119402985,
                                       0.44444444444444,
                                       0.44117647058824,
                                       0.43795620437956,
                                       0.43478260869565,
                                       0.43165467625899,
                                       0.42857142857143,
                                       0.42553191489362,
                                       0.42253521126761,
                                       0.41958041958042,
                                       0.41666666666667,
                                       0.41379310344828,
                                       0.41095890410959,
                                       0.40816326530612,
                                       0.40540540540541,
                                       0.40268456375839,
                                       0.4,
                                       0.39735099337748,
                                       0.39473684210526,
                                       0.3921568627451,
                                       0.38961038961039,
                                       0.38709677419355,
                                       0.38461538461538,
                                       0.38216560509554,
                                       0.37974683544304,
                                       0.37735849056604,
                                       0.375,
                                       0.37267080745342,
                                       0.37037037037037,
                                       0.3680981595092,
                                       0.36585365853659,
                                       0.36363636363636,
                                       0.36144578313253,
                                       0.35928143712575,
                                       0.35714285714286,
                                       0.35502958579882,
                                       0.35294117647059,
                                       0.35087719298246,
                                       0.34883720930233,
                                       0.34682080924855,
                                       0.3448275862069,
                                       0.34285714285714,
                                       0.34090909090909,
                                       0.33898305084746,
                                       0.33707865168539,
                                       0.33519553072626 };
// float g_tempoweight[g_numtempi]= { 0.8, 0.82581988897472, 0.83651483716701, 0.84472135955, 0.85163977794943,
// 0.85773502691896, 0.86324555320337, 0.8683130051064, 0.87302967433402, 0.87745966692415, 0.88164965809277,
// 0.88563488385777, 0.88944271909999, 0.89309493362513, 0.89660917830793, 0.9, 0.90327955589886, 0.90645812948448,
// 0.90954451150103, 0.91254628677423, 0.91547005383793, 0.91832159566199, 0.9211060141639, 0.92382783747338,
// 0.92649110640674, 0.92909944487358, 0.93165611772088, 0.93416407864999, 0.93662601021279, 0.93904435743076,
// 0.94142135623731, 0.94375905768565, 0.94605934866804, 0.94832396974191, 0.95055453054182, 0.95275252316519,
// 0.9549193338483, 0.95705625319186, 0.95916448515084, 0.96124515496597, 0.96329931618555, 0.96532795690183,
// 0.96733200530682, 0.969312334656, 0.97126976771554, 0.97320508075689, 0.97511900715418, 0.97701224063136,
// 0.97888543819998, 0.98073922282301, 0.98257418583506, 0.98439088914586, 0.98618986725025, 0.98797162906496,
// 0.9897366596101, 0.99148542155127, 0.99321835661586, 0.99493588689618, 0.99663841605004, 0.99832633040858, 1,
// 0.99832633040858, 0.99663841605004, 0.99493588689618, 0.99321835661586, 0.99148542155127, 0.9897366596101,
// 0.98797162906496, 0.98618986725025, 0.98439088914586, 0.98257418583506, 0.98073922282301, 0.97888543819998,
// 0.97701224063136, 0.97511900715418, 0.97320508075689, 0.97126976771554, 0.969312334656, 0.96733200530682,
// 0.96532795690183, 0.96329931618555, 0.96124515496597, 0.95916448515084, 0.95705625319186, 0.9549193338483,
// 0.95275252316519, 0.95055453054182, 0.94832396974191, 0.94605934866804, 0.94375905768565, 0.94142135623731,
// 0.93904435743076, 0.93662601021279, 0.93416407864999, 0.93165611772088, 0.92909944487358, 0.92649110640674,
// 0.92382783747338, 0.9211060141639, 0.91832159566199, 0.91547005383793, 0.91254628677423, 0.90954451150103,
// 0.90645812948448, 0.90327955589886, 0.9, 0.89660917830793, 0.89309493362513, 0.88944271909999, 0.88563488385777,
// 0.88164965809277, 0.87745966692415, 0.87302967433402, 0.8683130051064, 0.86324555320337, 0.85773502691896,
// 0.85163977794943, 0.84472135955, 0.83651483716701, 0.82581988897472 }; const float g_groove = 0.32;


static float g_sep[8] = { 0.0, 0.25, 0.5, 0.75, 0.0, 0.32, 0.5, 0.82 };
// weight for particular step
static float g_weight[4] = { 1.0, 0.5, 0.9, 0.6 };
// weight for blurring feature envelope locally
static float g_weight2[9] = { 0.05, 0.1, 0.3, 0.7, 1.0, 0.7, 0.3, 0.1, 0.05 };

// void BeatTrack2_dofft(BeatTrack2 *unit, uint32);
static void calculatetemplate(BeatTrack2* unit, int which, int j);
static void finaldecision(BeatTrack2* unit);

void BeatTrack2_Ctor(BeatTrack2* unit) {
    // unit->m_srate = unit->mWorld->mFullRate.mSampleRate;
    float kblocklength = unit->mWorld->mFullRate.mBufDuration; // seconds per control block
    unit->m_krlength = kblocklength;
    // N features per block over numphases*2 variants for one of 120 tempi, so need at least 120 blocks to complete

    unit->m_phaseaccuracy = ZIN0(3); // 0.02; //20 msec resolution; could be argument of UGen

    unit->m_numphases = (int*)RTAlloc(unit->mWorld, g_numtempi * sizeof(int));
    // unit->m_phases = (float**)RTAlloc(unit->mWorld, g_numtempi * sizeof(float*));

    for (int j = 0; j < g_numtempi; ++j) {
        float period = g_periods[j];

        int num = (int)(period / unit->m_phaseaccuracy); // maximum will be 1.0/0.02 = 50

        unit->m_numphases[j] = num;

        //
        //	unit->m_phases[j]= (float*)RTAlloc(unit->mWorld, unit->m_numphases[j] * sizeof(float));
        //
        //	float phase=0.0;
        //
        //	for (i=0; i<num; ++i) {
        //	unit->m_phases[j][i] = phase;
        //	phase += unit->m_phaseaccuracy;
        //	}
    }

    unit->m_numfeatures = (int)(ZIN0(1) + 0.001);


    // for efficiency
    unit->m_scores = (float*)RTAlloc(unit->mWorld, (2 * unit->m_numfeatures) * sizeof(float));

    unit->m_temporalwindowsize =
        ZIN0(2); // typically small, 2 seconds for fast reactions compared to 6 secs for BeatTrack

    unit->m_fullwindowsize = unit->m_temporalwindowsize + 1.0
        + 0.1; // plus one to cover all phases of the 60bpm based period, and a further 0.1 for indexing safety; ie
               // looking at areas around the point you're interested in

    unit->m_buffersize = (int)(unit->m_fullwindowsize / unit->m_krlength); // in control blocks


    // printf("loading test blocklength %f numfeatures %d temporal %f full %f blocks %d \n",unit->m_krlength,
    // unit->m_numfeatures, unit->m_temporalwindowsize, unit->m_fullwindowsize, unit->m_buffersize);


    // float ** m_pastfeatures;  //for each feature, a trail of last m_workingmemorysize values
    unit->m_pastfeatures = (float**)RTAlloc(unit->mWorld, unit->m_numfeatures * sizeof(float*));

    for (int j = 0; j < unit->m_numfeatures; ++j) {
        unit->m_pastfeatures[j] = (float*)RTAlloc(unit->mWorld, unit->m_buffersize * sizeof(float));

        Clear(unit->m_buffersize, unit->m_pastfeatures[j]); // set all to zero at first

        // for (i=0; i<unit->m_buffersize; ++i) {
        //	unit->m_pastfeatures[j][i] = 0.0;
        //	}
        //
    }

    // main counter
    unit->m_counter = 0;

    // could avoid allocation by having a hard limit on
    unit->bestscore = (float*)RTAlloc(unit->mWorld, 4 * unit->m_numfeatures * sizeof(float));
    unit->bestphase = (int*)RTAlloc(unit->mWorld, 4 * unit->m_numfeatures * sizeof(int));
    unit->besttempo = (int*)RTAlloc(unit->mWorld, 4 * unit->m_numfeatures * sizeof(int));
    unit->bestgroove = (int*)RTAlloc(unit->mWorld, 4 * unit->m_numfeatures * sizeof(int));

    for (int i = 0; i < 4; ++i) {
        int basepos = i * unit->m_numfeatures;

        for (int j = 0; j < unit->m_numfeatures; ++j) {
            unit->bestscore[basepos + j] = -9999.0;
            unit->bestphase[basepos + j] = 0;
            unit->besttempo[basepos + j] = 60;
            unit->bestgroove[basepos + j] = 0;
        }
    }

    unit->m_phase = 0.0;
    unit->m_period = 0.5;
    unit->m_groove = 0;
    unit->m_currtempo = 2;
    unit->m_phaseperblock = unit->m_krlength / unit->m_period;

    unit->m_predictphase = 0.4f;
    unit->m_predictperiod = 0.3f;


    unit->m_outputphase = unit->m_phase;
    unit->m_outputtempo = unit->m_currtempo;
    unit->m_outputgroove = unit->m_groove;
    unit->m_outputphaseperblock = unit->m_phaseperblock;


    unit->m_calculationperiod = 0.5; // every half second; could also be additional argument to UGen
    unit->m_calculationschedule = 0.0;

    // printf("srate %f conversion factor %f frame period %f \n", unit->m_srate, unit->m_srateconversion,
    // unit->m_frameperiod);


    int bufnum = (int)(ZIN0(5) + 0.001f);
    if (bufnum >= unit->mWorld->mNumSndBufs)
        bufnum = 0;

    if (bufnum < 0)
        unit->m_weightingscheme = bufnum < 2 ? 0 : 1;
    else {
        SndBuf* buf = unit->mWorld->mSndBufs + bufnum;
        unit->m_tempoweights = buf;
        unit->m_weightingscheme = 2;
    }

    // printf("bufnum %d weightingscheme %d check %f %f\n", bufnum, unit->m_weightingscheme, unit->m_tempoweights[0],
    // unit->m_tempoweights[119]);


    unit->halftrig = 0;
    unit->q1trig = 0;
    unit->q2trig = 0;


    unit->mCalcFunc = (UnitCalcFunc)&BeatTrack2_next;

    // initialize outputs
    ZOUT0(0) = 0.0;
    ZOUT0(1) = 0.0;
    ZOUT0(2) = 0.0;
    ZOUT0(3) = unit->m_outputtempo;
    ZOUT0(4) = unit->m_outputphase;
    ZOUT0(5) = unit->m_outputgroove;
}


void BeatTrack2_Dtor(BeatTrack2* unit) {
    RTFree(unit->mWorld, unit->m_numphases);

    RTFree(unit->mWorld, unit->m_scores);

    RTFree(unit->mWorld, unit->bestscore);
    RTFree(unit->mWorld, unit->bestphase);
    RTFree(unit->mWorld, unit->besttempo);

    for (int j = 0; j < unit->m_numfeatures; ++j)
        RTFree(unit->mWorld, unit->m_pastfeatures[j]);

    RTFree(unit->mWorld, unit->m_pastfeatures);
}


// over phases and for each groove
void calculatetemplate(BeatTrack2* unit, int which, int j) {
    int tmpindex;

    int startcounter = unit->m_startcounter;

    int numphases = unit->m_numphases[which];

    float period = g_periods[which];

    float blockconvert = unit->m_krlength;

    float windowsize = unit->m_temporalwindowsize;

    int buffersize = unit->m_buffersize; // unit->m_fullwindowsize/unit->m_krlength; //in control blocks

    float** pastfeatures = unit->m_pastfeatures;
    // unit->m_pastfeatures = (float**)RTAlloc(unit->mWorld, unit->m_numfeatures * sizeof(float*));

    int beatsfit = (int)(windowsize / period); // complete beats only, or also fit as many as possible?

    float weight; // compensation for number of events matched; may alter equation later

    switch (unit->m_weightingscheme) {
    case 0:
        weight = 1.0f; // flat
        break;
    case 1:
        weight = 1.0f / (beatsfit * 4); // compensate for number of time points tested
        break;
    case 2:
        SndBuf* buf = unit->m_tempoweights;
        if (buf->data)
            weight = buf->data[which]; // user defined temmpo biases (usually a mask on allowed tempi)
        else
            weight = 1.f;
        break;
    }

    int numfeatures = unit->m_numfeatures;

    float* scores = unit->m_scores; //[2*numfeatures];

    float* bestscore = unit->bestscore;
    int* bestphase = unit->bestphase;
    int* besttempo = unit->besttempo;
    int* bestgroove = unit->bestgroove;

    for (int i = 0; i < numphases; ++i) {
        // initialise scores
        // for (j=0; j<2; ++j)
        for (int k = 0; k < numfeatures; ++k)
            scores[2 * k + j] = 0.0;

        float phaseadd = i * unit->m_phaseaccuracy;

        // calculation for a particular phase of template
        // for (j=0; j<2; ++j) {

        for (int h = 0; h < beatsfit; ++h) {
            for (int l = 0; l < 4; ++l) {
                float sep = phaseadd + (h * period) + ((g_sep[j * 4 + l]) * period);
                float weight = g_weight[l];

                int blocks = (int)((sep / blockconvert) + 0.5); // round to nearest

                // convert sep to control periods and find appropriate point in source data
                int index = (startcounter + buffersize - blocks) % (buffersize);


                // widen over four either side
                for (int m = (-4); m < 5; ++m) {
                    int actualindex = (index + buffersize + m) % (buffersize);

                    for (int k = 0; k < numfeatures; ++k) {
                        int scoreindexnow = 2 * k + j;

                        // could widen this value here, even based on cubic interpolation etc
                        scores[scoreindexnow] += weight * (g_weight2[m + 4]) * (pastfeatures[k][actualindex]);
                    }

                    // scores[2*k+j] += weight * (pastfeatures[k][index]);
                }
            }
        }
        //}

        // update any winners from scores
        // for (j=0; j<2; ++j) {

        for (int k = 0; k < numfeatures; ++k) {
            float scorenow = (scores[2 * k + j]) * weight;

            // NEED TO STORE J IF PRESERVING SENSE OF GROOVE

            if (scorenow > bestscore[k]) {
                tmpindex = numfeatures + k;
                // shift up to make room
                bestscore[tmpindex] = bestscore[k];
                bestphase[tmpindex] = bestphase[k];
                besttempo[tmpindex] = besttempo[k];
                bestgroove[tmpindex] = bestgroove[k];

                bestscore[k] = scorenow;
                bestphase[k] = i;
                besttempo[k] = which;
                bestgroove[k] = j;

                // printf("bestscore %f bestphase %d besttempo %d bestgroove %d \n",
                // bestscore[k],bestphase[k],besttempo[k], bestgroove[k]);
            } else if (scorenow > bestscore[numfeatures + k]) {
                tmpindex = numfeatures + k;
                bestscore[tmpindex] = scorenow;
                bestphase[tmpindex] = i;
                besttempo[tmpindex] = which;
                bestgroove[tmpindex] = j;
            }
        }
        //}
    }
}


// a winner must appear at least twice, across features, and be superior to the secondbest in those features too by some
// margins a consistency check could also run to look at change from last time to this
void finaldecision(BeatTrack2* unit) {
    // int foundgood = 0;
    int bestcandidate = 0;
    int bestpreviousmatchsum = 0; //(-1);  //should be 0, but allowing different for now
    float excess; //, consistency;
                  // int exactmatches, closematches;  //can be out by a few indices on period; could match on tempo but
                  // not phase etc combine these four factors in one master score?

    for (int i = 0; i < unit->m_numfeatures; ++i) {
        int matchsum = 0;

        float secondbest = unit->bestscore[unit->m_numfeatures + i];
        excess = (secondbest != 0) ? (unit->bestscore[i] / secondbest) : unit->bestscore[i];
        int tempo = unit->besttempo[i];

        // could check consistency too by looking at phase update from last prediction in same feature

        for (int j = 0; j < unit->m_numfeatures; ++j) {
            if (j != i) {
                if (abs(unit->besttempo[j] - tempo) < 5)
                    matchsum++;
            }

            // check over all previous features
            if (abs(unit->besttempo[2 * unit->m_numfeatures + j] - tempo) < 5)
                matchsum++;
        }

        // printf("i %d matchsum %d excess %f \n",i, matchsum, excess);

        if (secondbest != 0)
            matchsum += (int)excess;

        // so must have at least one match //&& (excess>1.03)
        if ((matchsum > bestpreviousmatchsum)) {
            bestcandidate = i;
            bestpreviousmatchsum = matchsum;
            // foundgood = 1;
        }
    }


    // consistency: could require it to win twice; have a candidatepending which makes a phase prediction; only let
    // through if prediction fulfilled

    // unit->m_amortlength will be numtempi *2  = 240

    float bestphase =
        fmod(((unit->bestphase[bestcandidate] * unit->m_phaseaccuracy) + (unit->m_krlength * (unit->m_amortlength)))
                 / (unit->m_period),
             (float)1.0);

    // if(unit->m_prediction) {

    if ((fabs(bestphase - unit->m_predictphase) < ((2 * (unit->m_phaseaccuracy)) / unit->m_predictperiod))
        && (fabs((g_periods[unit->besttempo[bestcandidate]]) - unit->m_predictperiod) < 0.04)) {
        unit->m_period = unit->m_predictperiod;
        // time elapsed since a known beat is phase of winner in seconds, to calculation start point, plus time for
        // calculation (120 control blocks) divided by period, modulo 1.0
        unit->m_phase = bestphase;
        unit->m_currtempo = 1.f / unit->m_period;
        unit->m_phaseperblock = unit->m_krlength / unit->m_period;
    }

    //}

    // unit->m_prediction=false;


    // if(foundgood) {
    // if clear winner

    unit->m_predictperiod = g_periods[unit->besttempo[bestcandidate]];

    // time elapsed since a known beat is phase of winner in seconds, to calculation start point, plus time for
    // calculation (120 control blocks) divided by period, modulo 1.0
    unit->m_predictphase = fmod(((unit->bestphase[bestcandidate] * unit->m_phaseaccuracy)
                                 + (unit->m_krlength * (unit->m_amortlength)) + unit->m_calculationperiod)
                                    / (unit->m_period),
                                (float)1.0);


    // if(foundgood) {
    ////if clear winner
    //
    // unit->m_period = g_periods[unit->besttempo[bestcandidate]];
    ////time elapsed since a known beat is phase of winner in seconds, to calculation start point, plus time for
    /// calculation (120 control blocks) divided by period, modulo 1.0
    // unit->m_phase= fmod( ((unit->bestphase[bestcandidate] * unit->m_phaseaccuracy)  + (unit->m_krlength *
    // 120))/(unit->m_period), 1.0);
    //
    // unit->m_currtempo = 1.0/unit->m_period;
    // unit->m_phaseperblock = unit->m_krlength/unit->m_period;
    //}
}


void BeatTrack2_next(BeatTrack2* unit, int wrongNumSamples) {
    // keep updating feature memories
    unit->m_counter = (unit->m_counter + 1) % (unit->m_buffersize);

    int busnum = (int)(ZIN0(0) + 0.001f);

    // unit->m_features = unit->mWorld->mControlBus + busnum;

    float* features = unit->mWorld->mControlBus + busnum;

    // hmm, is this pointer guaranteed to stay the same? may have to update each time...
    for (int j = 0; j < unit->m_numfeatures; ++j) {
        unit->m_pastfeatures[j][unit->m_counter] = features[j]; // unit->m_features[j];
    }

    unit->m_calculationschedule += unit->m_krlength;

    // check for new calculation round
    if (unit->m_calculationschedule > unit->m_calculationperiod) {
        unit->m_calculationschedule -= unit->m_calculationperiod;

        // reset best scores and move old to previous slots
        for (int i = 0; i < 2; ++i) {
            int pos1 = (2 + i) * unit->m_numfeatures;
            int pos2 = i * unit->m_numfeatures;

            for (int j = 0; j < unit->m_numfeatures; ++j) {
                unit->bestscore[pos1 + j] = unit->bestscore[pos2 + j];
                unit->bestscore[pos2 + j] = -9999.0;
                unit->bestphase[pos1 + j] = unit->bestphase[pos2 + j];
                unit->bestphase[pos2 + j] = 0;
                unit->besttempo[pos1 + j] = unit->besttempo[pos2 + j];
                unit->besttempo[pos2 + j] = 60;
            }
        }

        // state 0 is do nothing
        unit->m_amortisationstate = 1;
        unit->m_amortcount = 0;
        unit->m_amortlength = g_numtempi * 2; //
                                              // unit->m_amortisationsteps=0;

        // store essential data
        unit->m_startcounter = unit->m_counter;

        unit->m_currphase = unit->m_phase;
    }


    // keeps incrementing but will be reset with each calculation run
    // unit->m_amortisationsteps=unit->m_amortisationsteps+1;

    // if state nonzero do something
    switch (unit->m_amortisationstate) {
    case 0:
        break; // do nothing case
    case 1: // calculate acf
        calculatetemplate(unit, unit->m_amortcount >> 1, unit->m_amortcount % 2);

        unit->m_amortcount = unit->m_amortcount + 1;

        if (unit->m_amortcount == unit->m_amortlength) {
            unit->m_amortisationstate = 2;
            // unit->m_amortlength=1;
            // unit->m_amortcount=0;
        }
        break;
    case 2: // done calculating template matches, now decide whether to follow through
        finaldecision(unit);
        unit->m_amortisationstate = 0;
        break;

    default:
        break;
    }


    // test if impulse to output
    unit->m_phase += unit->m_phaseperblock;

    // if(unit->m_counter%400==0) printf("phase %f period %f\n", unit->m_phase, unit->m_period);

    // if not locked, update output phase from model phase, else keep a separate output phase

    float lock = ZIN0(4);
    // printf("lock %f \n",lock);

    if (lock < 0.5f) {
        unit->m_outputphase = unit->m_phase;
        unit->m_outputtempo = unit->m_currtempo;
        unit->m_outputgroove = unit->m_groove;
        unit->m_outputphaseperblock = unit->m_phaseperblock;
    } else {
        unit->m_outputphase += unit->m_outputphaseperblock;
    }

    if (unit->m_phase >= 1.f) {
        unit->m_phase -= 1.f;
    }

    // 0 is beat, 1 is quaver, 2 is semiquaver, 3 is actual current tempo in bps
    // so no audio accuracy with beats, just asap, may as well be control rate
    ZOUT0(0) = 0.0;
    ZOUT0(1) = 0.0;
    ZOUT0(2) = 0.0;
    ZOUT0(3) = unit->m_outputtempo; //*0.016666667;
    ZOUT0(4) = unit->m_outputphase;
    ZOUT0(5) = unit->m_outputgroove;

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

    if (unit->m_outputphase >= 0.5 && unit->halftrig == 0) {
        ZOUT0(1) = 1.0;
        ZOUT0(2) = 1.0;
        unit->halftrig = 1;
    }

    float groove = unit->m_outputgroove * 0.07;

    if (unit->m_outputphase >= (0.25 + groove) && unit->q1trig == 0) {
        ZOUT0(2) = 1.0;
        unit->q1trig = 1;
    }

    if (unit->m_outputphase >= (0.75 + groove) && unit->q2trig == 0) {
        ZOUT0(2) = 1.0;
        unit->q2trig = 1;
    }
}
