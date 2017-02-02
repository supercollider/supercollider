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


//N feature stream inputs, cross correlation with template on each channel

//templates- search 120 tempi * 2 groove types (really just /2 and /3 subdivision) and no metrical discernment * a different numbers of phases depending on tempo

//assume 4 features * 120 tempi * 2 groove * 20 phase = 19200 per calculation time; say you calculate each 0.5 sec, amortise over (44100/64*0.5 >344.5) control blocks;
//calculate over 240 blocks

#include "SC_PlugIn.h"

struct BeatTrack2 : Unit {

	//adjust for any sampling rate by calculating factors; runs off of k rate inputs
	float m_srate;
	float m_phaseaccuracy;  //probably 8 control blocks!
	int m_numtempi; //= 120;
	//float * m_tempi;
	int * m_numphases;
	//float ** m_phases;

	int m_numfeatures;
	float * m_features; //pointer to control bus, assumes contiguous busnum for each consecutive feature

	//
	float m_temporalwindowsize; //typically small, 2 seconds for fast reactions compared to 6 secs for BeatTrack
	float m_fullwindowsize;
	float m_krlength;
	int m_buffersize; //in control blocks

	float ** m_pastfeatures;  //for each feature, a trail of last m_workingmemorysize values

	int m_counter, m_startcounter;

	//time positions
	float m_calculationschedule;
	float m_calculationperiod;

	//tempo
	float m_period;
	int m_groove;
	float m_currtempo;

	//phase
	float m_currphase;

	//phasor, trigger beat and modulo when >1.0
	float m_phase, m_phaseperblock;

	//phasor output separate so can have it lock and keep going when don't want to track
	float m_outputphase, m_outputtempo, m_outputgroove, m_outputphaseperblock;

	float m_predictphase, m_predictperiod;

	//amortization - more complex structure to support different rates of work
	int m_amortisationstate;
	int m_amortcount;
	int m_amortlength;
	int m_amortisationsteps;

	//efficiency trick
	float * m_scores;

	//tracking best results for each feature
	//also storing second best and previous best and second best?
	//thus keep best from previous round too for additional vote/consistency check
	float * bestscore;
	int * bestphase;
	int * besttempo;
	int * bestgroove;

	//or store all scores and resolve later?

	int halftrig;
	int q1trig;
	int q2trig;

	SndBuf * m_tempoweights;
	int m_weightingscheme;
};



extern "C"
{
	//required interface functions
	void BeatTrack2_next(BeatTrack2 *unit, int wrongNumSamples);
	void BeatTrack2_Ctor(BeatTrack2 *unit);
	void BeatTrack2_Dtor(BeatTrack2 *unit);
}
