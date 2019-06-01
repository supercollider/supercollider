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

// global data

// helpful constants
//#define PI 3.1415926535898f
//#define TWOPI 6.28318530717952646f

extern InterfaceTable* ft;

// shared data
extern int eqlbandbins[43];
extern int eqlbandsizes[42];
extern float contours[42][11];
extern double phons[11];

//#include "KeyTrack.h"
#include "BeatTrack.h"
//#include "Loudness.h"
#include "Onsets.h"
//#include "MFCC.h"
#include "BeatTrack2.h"

struct Loudness : Unit {
    // FFT data
    int m_numbands;
    float* m_ERBbands;

    // float m_phontotal;
    // final output
    float m_sones;
};


struct KeyTrack : Unit {
    // FFT data
    float* m_FFTBuf;

    // coping with different sampling rates
    float m_srate; // use as a flag to check sample rate is correct
    float* m_weights; // will point to the sample rate specific data
    int* m_bins;
    float m_frameperiod;

    // counter
    // uint32 m_frame;

    // experimental transient avoidance
    // float m_prevphase[720]; //60*12
    // float m_leaknote[60];

    float m_chroma[12];
    float m_key[24];

    float m_histogram[24]; // key histogram
                           // float m_keyleak; //fade parameter for histogram
    // int m_triggerid;

    int m_currentKey;
};


struct MFCC : Unit {
    // MFCC
    int m_numcoefficients;
    float* m_mfcc;
    // ERB
    int m_numbands;
    float* m_bands;

    // sampling rate specific data
    float m_srate;
    int* m_startbin;
    int* m_endbin;
    int* m_cumulindex;
    float* m_bandweights;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

struct FFTAnalyser_Unit : Unit {
    float outval;

    // Not always used: multipliers which convert from bin indices to freq vals, and vice versa.
    // See also the macros for deriving these.
    float m_bintofreq /* , m_freqtobin */;
    float m_halfnyq_over_numbinsp2;
};

struct FFTAnalyser_OutOfPlace : FFTAnalyser_Unit {
    int m_numbins;
    float* m_tempbuf;
};

struct SpecFlatness : FFTAnalyser_Unit {
    double m_oneovern;
};

struct SpecPcile : FFTAnalyser_OutOfPlace {
    bool m_interpolate;
};

struct SpecCentroid : FFTAnalyser_Unit {};


extern "C" {
// required interface functions
void Loudness_next(Loudness* unit, int wrongNumSamples);
void Loudness_Ctor(Loudness* unit);
void Loudness_Dtor(Loudness* unit);

void KeyTrack_next(KeyTrack* unit, int wrongNumSamples);
void KeyTrack_Ctor(KeyTrack* unit);
void KeyTrack_Dtor(KeyTrack* unit);

void MFCC_next(MFCC* unit, int wrongNumSamples);
void MFCC_Ctor(MFCC* unit);
void MFCC_Dtor(MFCC* unit);

void SpecFlatness_Ctor(SpecFlatness* unit);
void SpecFlatness_next(SpecFlatness* unit, int inNumSamples);
//
void SpecPcile_Ctor(SpecPcile* unit);
void SpecPcile_next(SpecPcile* unit, int inNumSamples);
void SpecPcile_Dtor(SpecPcile* unit);
//
void SpecCentroid_Ctor(SpecCentroid* unit);
void SpecCentroid_next(SpecCentroid* unit, int inNumSamples);
}
