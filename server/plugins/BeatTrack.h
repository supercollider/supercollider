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


#define DFFRAMELENGTH 512
#define DFSTORE 700
//store last 700 to avoid problems during amortisation
#define LAGS 128


struct BeatTrack : Unit {

	//corrections for different sampling rates: support 44100 as native, and 48000, 88200, 96000 by frequency domain down sampling
	//assumes 1024 FFT for first two, 2048 for second two
	float m_srate;
	float m_srateconversion;
	float m_frameperiod;


	//FFT data
	//int m_bufWritePos;
	//float * m_prepareFFTBuf;
	float * m_FFTBuf;

	//fftwf_plan planTime2FFT;

	float * m_prevmag;
	float * m_prevphase;
	float * m_predict;

	//vDSP
	//unsigned long m_vlog2n;
	//COMPLEX_SPLIT m_vA;
	//FFTSetup m_vsetup;


	//time positions
	long m_frame;

	//df
	float m_df[DFSTORE];
	int m_dfcounter;

	//for peak pick scorer
	int m_dfmemorycounter;
	float m_dfmemory[15];


	//autocorrelation results on df
	float m_acf[DFFRAMELENGTH];

	//float* m_M;
	float m_mg[LAGS];
	float m_besttorsum;
	int m_bestcolumn;

	float m_phaseweights[LAGS];

	float m_tor;
	int m_torround;

	float m_periodp;
	float m_periodg;

	int m_flagstep;
	float m_prevperiodp[3];

	//amortisation search for best phase from 0 to m_torround-1
	float m_bestphasescore;
	int m_bestphase;

	//tempo
	float m_currtempo;

	//phase
	float m_currphase;

	//phasor, trigger beat and modulo when >1.0
	float m_phase, m_phaseperblock;

	//phasor output separate so can have it lock and keep going when don't want to track
	float m_outputphase, m_outputtempo, m_outputphaseperblock;

	int halftrig;
	int q1trig;
	int q2trig;

	//amortization - more complex structure to support different rates of work
	int m_amortisationstate;
	int m_amortcount;
	int m_amortlength;
	int m_amortisationsteps;

	//model states
	int m_stateflag;
	int m_timesig;

	int m_storedfcounter;
	int m_storedfcounterend;


};



extern "C"
{
	//required interface functions
	void BeatTrack_next(BeatTrack *unit, int wrongNumSamples);
	void BeatTrack_Ctor(BeatTrack *unit);
	void BeatTrack_Dtor(BeatTrack *unit);
}

