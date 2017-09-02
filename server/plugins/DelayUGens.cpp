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

#ifdef NOVA_SIMD
#include "simd_memory.hpp"
#endif

#include "SC_PlugIn.h"
#include <cstdio>

#include <boost/align/is_aligned.hpp>

using namespace std; // for math functions


const int kMAXMEDIANSIZE = 32;

static InterfaceTable *ft;

struct ScopeOut : public Unit
{
	SndBuf *m_buf;
	SndBufUpdates *m_bufupdates;
	float m_fbufnum;
	uint32 m_framepos, m_framecount;
	float **mIn;
};

struct PlayBuf : public Unit
{
	double m_phase;
	float m_prevtrig;
	float m_fbufnum;
	float m_failedBufNum;
	SndBuf *m_buf;
};


struct Grain
{
	double phase, rate;
	double b1, y1, y2; // envelope
	float pan1, pan2;
	int counter;
	int bufnum;
	int chan;
	int interp;
};

const int kMaxGrains = 64;

struct TGrains : public Unit
{
	float mPrevTrig;
	int mNumActive;
	Grain mGrains[kMaxGrains];
};


#if NOTYET
struct SimpleLoopBuf : public Unit
{
	int m_phase;
	float m_prevtrig;
	float m_fbufnum;
	SndBuf *m_buf;
};
#endif

struct BufRd : public Unit
{
	float m_fbufnum;
	float m_failedBufNum;
	SndBuf *m_buf;
};

struct BufWr : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
};

struct RecordBuf : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
	int32 m_writepos;
	float m_recLevel, m_preLevel;
	float m_prevtrig;
	float **mIn;
};

struct Pitch : public Unit
{
	float m_values[kMAXMEDIANSIZE];
	int m_ages[kMAXMEDIANSIZE];
	float *m_buffer;

	float m_freq, m_minfreq, m_maxfreq, m_hasfreq, m_srate, m_ampthresh, m_peakthresh;
	int m_minperiod, m_maxperiod, m_execPeriod, m_index, m_readp, m_size;
	int m_downsamp, m_maxlog2bins, m_medianSize;
	int m_state;
	bool m_getClarity;
};

struct InterpolationUnit
{
	static const int minDelaySamples = 1;
};

struct CubicInterpolationUnit
{
	static const int minDelaySamples = 2;
};

struct BufDelayUnit : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
	float m_dsamp;
	float m_delaytime;
	long m_iwrphase;
	uint32 m_numoutput;
};

struct BufDelayN : public BufDelayUnit, InterpolationUnit
{};

struct BufDelayL : public BufDelayUnit, InterpolationUnit
{};

struct BufDelayC : public BufDelayUnit, CubicInterpolationUnit
{};

struct BufFeedbackDelay : public BufDelayUnit
{
	float m_feedbk, m_decaytime;
};

struct BufCombN : public BufFeedbackDelay, InterpolationUnit
{};

struct BufCombL : public BufFeedbackDelay, InterpolationUnit
{};

struct BufCombC : public BufFeedbackDelay, CubicInterpolationUnit
{};

struct BufAllpassN : public BufFeedbackDelay, InterpolationUnit
{};

struct BufAllpassL : public BufFeedbackDelay, InterpolationUnit
{};

struct BufAllpassC : public BufFeedbackDelay, CubicInterpolationUnit
{};

struct DelayUnit : public Unit
{
	float *m_dlybuf;

	float m_dsamp, m_fdelaylen;
	float m_delaytime, m_maxdelaytime;
	long m_iwrphase, m_idelaylen, m_mask;
	long m_numoutput;
};

struct DelayN : public DelayUnit, InterpolationUnit
{};

struct DelayL : public DelayUnit, InterpolationUnit
{};

struct DelayC : public DelayUnit, InterpolationUnit
{};

struct FeedbackDelay : public DelayUnit
{
	float m_feedbk, m_decaytime;
};

struct CombN : public FeedbackDelay, InterpolationUnit
{};

struct CombL : public FeedbackDelay, InterpolationUnit
{};

struct CombC : public FeedbackDelay, CubicInterpolationUnit
{};

struct AllpassN : public FeedbackDelay, InterpolationUnit
{};

struct AllpassL : public FeedbackDelay, InterpolationUnit
{};

struct AllpassC : public FeedbackDelay, CubicInterpolationUnit
{};

struct BufInfoUnit : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
};

struct Pluck : public FeedbackDelay, CubicInterpolationUnit
{
	float m_lastsamp, m_prevtrig, m_coef;
	long m_inputsamps;
};

struct LocalBuf : public Unit
{
	SndBuf *m_buf;
	void * chunk;
};

struct MaxLocalBufs : public Unit
{
};

struct SetBuf : public Unit
{};

struct ClearBuf : public Unit
{};

struct DelTapWr : public Unit
{
	SndBuf *m_buf;
	float m_fbufnum;
	uint32 m_phase;
};

struct DelTapRd : public Unit
{
	SndBuf *m_buf;
	float m_fbufnum, m_delTime;
};


//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{

	void SampleRate_Ctor(Unit *unit, int inNumSamples);
	void ControlRate_Ctor(Unit *unit, int inNumSamples);
	void SampleDur_Ctor(Unit *unit, int inNumSamples);
	void ControlDur_Ctor(Unit *unit, int inNumSamples);
	void SubsampleOffset_Ctor(Unit *unit, int inNumSamples);
	void RadiansPerSample_Ctor(Unit *unit, int inNumSamples);
	void NumInputBuses_Ctor(Unit *unit, int inNumSamples);
	void NumOutputBuses_Ctor(Unit *unit, int inNumSamples);
	void NumAudioBuses_Ctor(Unit *unit, int inNumSamples);
	void NumControlBuses_Ctor(Unit *unit, int inNumSamples);
	void NumBuffers_Ctor(Unit *unit, int inNumSamples);
	void NodeID_Ctor(Unit *unit, int inNumSamples);
	void NumRunningSynths_Ctor(Unit *unit, int inNumSamples);
	void NumRunningSynths_next(Unit *unit, int inNumSamples);

	void BufSampleRate_next(BufInfoUnit *unit, int inNumSamples);
	void BufSampleRate_Ctor(BufInfoUnit *unit, int inNumSamples);

	void BufFrames_next(BufInfoUnit *unit, int inNumSamples);
	void BufFrames_Ctor(BufInfoUnit *unit, int inNumSamples);

	void BufDur_next(BufInfoUnit *unit, int inNumSamples);
	void BufDur_Ctor(BufInfoUnit *unit, int inNumSamples);

	void BufChannels_next(BufInfoUnit *unit, int inNumSamples);
	void BufChannels_Ctor(BufInfoUnit *unit, int inNumSamples);

	void BufSamples_next(BufInfoUnit *unit, int inNumSamples);
	void BufSamples_Ctor(BufInfoUnit *unit, int inNumSamples);

	void BufRateScale_next(BufInfoUnit *unit, int inNumSamples);
	void BufRateScale_Ctor(BufInfoUnit *unit, int inNumSamples);

	void PlayBuf_next_aa(PlayBuf *unit, int inNumSamples);
	void PlayBuf_next_ak(PlayBuf *unit, int inNumSamples);
	void PlayBuf_next_ka(PlayBuf *unit, int inNumSamples);
	void PlayBuf_next_kk(PlayBuf *unit, int inNumSamples);
	void PlayBuf_Ctor(PlayBuf* unit);

	void TGrains_next(TGrains *unit, int inNumSamples);
	void TGrains_Ctor(TGrains* unit);

#if NOTYET
	void SimpleLoopBuf_next_kk(SimpleLoopBuf *unit, int inNumSamples);
	void SimpleLoopBuf_Ctor(SimpleLoopBuf* unit);
	void SimpleLoopBuf_Dtor(SimpleLoopBuf* unit);
#endif

	void BufRd_Ctor(BufRd *unit);
	void BufRd_next_4(BufRd *unit, int inNumSamples);
	void BufRd_next_2(BufRd *unit, int inNumSamples);
	void BufRd_next_1(BufRd *unit, int inNumSamples);

	void BufWr_Ctor(BufWr *unit);
	void BufWr_next(BufWr *unit, int inNumSamples);

	void RecordBuf_Ctor(RecordBuf *unit);
	void RecordBuf_Dtor(RecordBuf *unit);
	void RecordBuf_next(RecordBuf *unit, int inNumSamples);
	void RecordBuf_next_10(RecordBuf *unit, int inNumSamples);

	void Pitch_Ctor(Pitch *unit);
	void Pitch_next_a(Pitch *unit, int inNumSamples);
	void Pitch_next_k(Pitch *unit, int inNumSamples);

	void LocalBuf_Ctor(LocalBuf *unit);
	void LocalBuf_Dtor(LocalBuf *unit);

	void MaxLocalBufs_Ctor(MaxLocalBufs *unit);

	void SetBuf_Ctor(SetBuf *unit);
	void ClearBuf_Ctor(ClearBuf *unit);

	void BufDelayN_Ctor(BufDelayN *unit);
	void BufDelayN_next(BufDelayN *unit, int inNumSamples);
	void BufDelayN_next_z(BufDelayN *unit, int inNumSamples);
	void BufDelayN_next_a(BufDelayN *unit, int inNumSamples);
	void BufDelayN_next_a_z(BufDelayN *unit, int inNumSamples);

	void BufDelayL_Ctor(BufDelayL *unit);
	void BufDelayL_next(BufDelayL *unit, int inNumSamples);
	void BufDelayL_next_z(BufDelayL *unit, int inNumSamples);
	void BufDelayL_next_a(BufDelayL *unit, int inNumSamples);
	void BufDelayL_next_a_z(BufDelayL *unit, int inNumSamples);

	void BufDelayC_Ctor(BufDelayC *unit);
	void BufDelayC_next(BufDelayC *unit, int inNumSamples);
	void BufDelayC_next_z(BufDelayC *unit, int inNumSamples);
	void BufDelayC_next_a(BufDelayC *unit, int inNumSamples);
	void BufDelayC_next_a_z(BufDelayC *unit, int inNumSamples);

	void BufCombN_Ctor(BufCombN *unit);
	void BufCombN_next(BufCombN *unit, int inNumSamples);
	void BufCombN_next_z(BufCombN *unit, int inNumSamples);
	void BufCombN_next_a(BufCombN *unit, int inNumSamples);
	void BufCombN_next_a_z(BufCombN *unit, int inNumSamples);

	void BufCombL_Ctor(BufCombL *unit);
	void BufCombL_next(BufCombL *unit, int inNumSamples);
	void BufCombL_next_z(BufCombL *unit, int inNumSamples);
	void BufCombL_next_a(BufCombL *unit, int inNumSamples);
	void BufCombL_next_a_z(BufCombL *unit, int inNumSamples);

	void BufCombC_Ctor(BufCombC *unit);
	void BufCombC_next(BufCombC *unit, int inNumSamples);
	void BufCombC_next_z(BufCombC *unit, int inNumSamples);
	void BufCombC_next_a(BufCombC *unit, int inNumSamples);
	void BufCombC_next_a_z(BufCombC *unit, int inNumSamples);

	void BufAllpassN_Ctor(BufAllpassN *unit);
	void BufAllpassN_next(BufAllpassN *unit, int inNumSamples);
	void BufAllpassN_next_z(BufAllpassN *unit, int inNumSamples);
	void BufAllpassN_next_a(BufAllpassN *unit, int inNumSamples);
	void BufAllpassN_next_a_z(BufAllpassN *unit, int inNumSamples);

	void BufAllpassL_Ctor(BufAllpassL *unit);
	void BufAllpassL_next(BufAllpassL *unit, int inNumSamples);
	void BufAllpassL_next_z(BufAllpassL *unit, int inNumSamples);
	void BufAllpassL_next_a(BufAllpassL *unit, int inNumSamples);
	void BufAllpassL_next_a_z(BufAllpassL *unit, int inNumSamples);

	void BufAllpassC_Ctor(BufAllpassC *unit);
	void BufAllpassC_next(BufAllpassC *unit, int inNumSamples);
	void BufAllpassC_next_z(BufAllpassC *unit, int inNumSamples);
	void BufAllpassC_next_a(BufAllpassC *unit, int inNumSamples);
	void BufAllpassC_next_a_z(BufAllpassC *unit, int inNumSamples);

	void DelayUnit_Dtor(DelayUnit *unit);

	void DelayN_Ctor(DelayN *unit);
	void DelayN_next(DelayN *unit, int inNumSamples);
	void DelayN_next_z(DelayN *unit, int inNumSamples);
	void DelayN_next_a(DelayN *unit, int inNumSamples);
	void DelayN_next_a_z(DelayN *unit, int inNumSamples);

	void DelayL_Ctor(DelayL *unit);
	void DelayL_next(DelayL *unit, int inNumSamples);
	void DelayL_next_z(DelayL *unit, int inNumSamples);
	void DelayL_next_a(DelayL *unit, int inNumSamples);
	void DelayL_next_a_z(DelayL *unit, int inNumSamples);

	void DelayC_Ctor(DelayC *unit);
	void DelayC_next(DelayC *unit, int inNumSamples);
	void DelayC_next_z(DelayC *unit, int inNumSamples);
	void DelayC_next_a(DelayC *unit, int inNumSamples);
	void DelayC_next_a_z(DelayC *unit, int inNumSamples);

	void CombN_Ctor(CombN *unit);
	void CombN_next(CombN *unit, int inNumSamples);
	void CombN_next_z(CombN *unit, int inNumSamples);
	void CombN_next_a(CombN *unit, int inNumSamples);
	void CombN_next_a_z(CombN *unit, int inNumSamples);

	void CombL_Ctor(CombL *unit);
	void CombL_next(CombL *unit, int inNumSamples);
	void CombL_next_z(CombL *unit, int inNumSamples);
	void CombL_next_a(CombL *unit, int inNumSamples);
	void CombL_next_a_z(CombL *unit, int inNumSamples);

	void CombC_Ctor(CombC *unit);
	void CombC_next(CombC *unit, int inNumSamples);
	void CombC_next_z(CombC *unit, int inNumSamples);
	void CombC_next_a(CombC *unit, int inNumSamples);
	void CombC_next_a_z(CombC *unit, int inNumSamples);

	void AllpassN_Ctor(AllpassN *unit);
	void AllpassN_next(AllpassN *unit, int inNumSamples);
	void AllpassN_next_z(AllpassN *unit, int inNumSamples);
	void AllpassN_next_a(AllpassN *unit, int inNumSamples);
	void AllpassN_next_a_z(AllpassN *unit, int inNumSamples);

	void AllpassL_Ctor(AllpassL *unit);
	void AllpassL_next(AllpassL *unit, int inNumSamples);
	void AllpassL_next_z(AllpassL *unit, int inNumSamples);
	void AllpassL_next_a(AllpassL *unit, int inNumSamples);
	void AllpassL_next_a_z(AllpassL *unit, int inNumSamples);

	void AllpassC_Ctor(AllpassC *unit);
	void AllpassC_next(AllpassC *unit, int inNumSamples);
	void AllpassC_next_z(AllpassC *unit, int inNumSamples);
	void AllpassC_next_a(AllpassC *unit, int inNumSamples);
	void AllpassC_next_a_z(AllpassC *unit, int inNumSamples);

	void ScopeOut_next(ScopeOut *unit, int inNumSamples);
	void ScopeOut_Ctor(ScopeOut *unit);
	void ScopeOut_Dtor(ScopeOut *unit);

	void Pluck_Ctor(Pluck* unit);
	void Pluck_next_aa(Pluck *unit, int inNumSamples);
	void Pluck_next_aa_z(Pluck *unit, int inNumSamples);
	void Pluck_next_kk(Pluck *unit, int inNumSamples);
	void Pluck_next_kk_z(Pluck *unit, int inNumSamples);
	void Pluck_next_ka(Pluck *unit, int inNumSamples);
	void Pluck_next_ka_z(Pluck *unit, int inNumSamples);
	void Pluck_next_ak(Pluck *unit, int inNumSamples);
	void Pluck_next_ak_z(Pluck *unit, int inNumSamples);

	void DelTapWr_Ctor(DelTapWr* unit);
	void DelTapWr_next(DelTapWr *unit, int inNumSamples);
	void DelTapWr_next_simd(DelTapWr *unit, int inNumSamples);

	void DelTapRd_Ctor(DelTapRd* unit);
	void DelTapRd_next1_a(DelTapRd *unit, int inNumSamples);
	void DelTapRd_next2_a(DelTapRd *unit, int inNumSamples);
	void DelTapRd_next4_a(DelTapRd *unit, int inNumSamples);
	void DelTapRd_next1_k(DelTapRd *unit, int inNumSamples);
	void DelTapRd_next1_k_simd(DelTapRd *unit, int inNumSamples);
	void DelTapRd_next2_k(DelTapRd *unit, int inNumSamples);
	void DelTapRd_next4_k(DelTapRd *unit, int inNumSamples);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void SampleRate_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mSampleRate;
}


void ControlRate_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mBufRate.mSampleRate;
}


void SampleDur_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mFullRate.mSampleDur;
}

void ControlDur_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mFullRate.mBufDuration;
}

void RadiansPerSample_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mFullRate.mRadiansPerSample;
}

void BlockSize_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mFullRate.mBufLength;
}

void SubsampleOffset_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mParent->mSubsampleOffset;
}


void NumInputBuses_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mNumInputs;
}

void NumOutputBuses_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mNumOutputs;
}

void NumAudioBuses_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mNumAudioBusChannels;
}

void NumControlBuses_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mNumControlBusChannels;
}

void NumBuffers_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mNumSndBufs;
}

void NodeID_Ctor(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = (float) unit->mParent->mNode.mID;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void NumRunningSynths_Ctor(Unit *unit, int inNumSamples)
{
	if(INRATE(0) != calc_ScalarRate) { SETCALC(NumRunningSynths_next); }
	ZOUT0(0) = unit->mWorld->mNumGraphs;
}

void NumRunningSynths_next(Unit *unit, int inNumSamples)
{
	ZOUT0(0) = unit->mWorld->mNumGraphs;
}



//////////////////////////////////////////////////////////////////////////////////////////////////


#define CTOR_GET_BUF \
	float fbufnum  = ZIN0(0); \
	fbufnum = sc_max(0.f, fbufnum); \
	uint32 bufnum = (int)fbufnum; \
	World *world = unit->mWorld; \
	SndBuf *buf; \
	if (bufnum >= world->mNumSndBufs) { \
		int localBufNum = bufnum - world->mNumSndBufs; \
		Graph *parent = unit->mParent; \
		if(localBufNum <= parent->localBufNum) { \
			buf = parent->mLocalSndBufs + localBufNum; \
		} else { \
			bufnum = 0; \
			buf = world->mSndBufs + bufnum; \
		} \
	} else { \
		buf = world->mSndBufs + bufnum; \
	}

void BufSampleRate_next(BufInfoUnit *unit, int inNumSamples)
{
	SIMPLE_GET_BUF_SHARED
	ZOUT0(0) = buf->samplerate;
}

void BufSampleRate_Ctor(BufInfoUnit *unit, int inNumSamples)
{
	SETCALC(BufSampleRate_next);
	CTOR_GET_BUF
	unit->m_fbufnum = fbufnum;
	unit->m_buf = buf;
	ZOUT0(0) = buf->samplerate;
}


void BufFrames_next(BufInfoUnit *unit, int inNumSamples)
{
	SIMPLE_GET_BUF_SHARED
	ZOUT0(0) = buf->frames;
}

void BufFrames_Ctor(BufInfoUnit *unit, int inNumSamples)
{
	SETCALC(BufFrames_next);
	CTOR_GET_BUF
	unit->m_fbufnum = fbufnum;
	unit->m_buf = buf;
	ZOUT0(0) = buf->frames;
}


void BufDur_next(BufInfoUnit *unit, int inNumSamples)
{
	SIMPLE_GET_BUF_SHARED
	ZOUT0(0) = buf->frames * buf->sampledur;
}

void BufDur_Ctor(BufInfoUnit *unit, int inNumSamples)
{
	SETCALC(BufDur_next);
	CTOR_GET_BUF
	unit->m_fbufnum = fbufnum;
	unit->m_buf = buf;
	ZOUT0(0) = buf->frames * buf->sampledur;
}


void BufChannels_next(BufInfoUnit *unit, int inNumSamples)
{
	SIMPLE_GET_BUF_SHARED
	ZOUT0(0) = buf->channels;
}

void BufChannels_Ctor(BufInfoUnit *unit, int inNumSamples)
{
	SETCALC(BufChannels_next);
	CTOR_GET_BUF
	unit->m_fbufnum = fbufnum;
	unit->m_buf = buf;
	ZOUT0(0) = buf->channels;
}


void BufSamples_next(BufInfoUnit *unit, int inNumSamples)
{
	SIMPLE_GET_BUF_SHARED
	ZOUT0(0) = buf->samples;
}

void BufSamples_Ctor(BufInfoUnit *unit, int inNumSamples)
{
	SETCALC(BufSamples_next);
	CTOR_GET_BUF
	unit->m_fbufnum = fbufnum;
	unit->m_buf = buf;
	ZOUT0(0) = buf->samples;
}


void BufRateScale_next(BufInfoUnit *unit, int inNumSamples)
{
	SIMPLE_GET_BUF_SHARED
	ZOUT0(0) = buf->samplerate * unit->mWorld->mFullRate.mSampleDur;
}

void BufRateScale_Ctor(BufInfoUnit *unit, int inNumSamples)
{
	SETCALC(BufRateScale_next);
	CTOR_GET_BUF
	unit->m_fbufnum = fbufnum;
	unit->m_buf = buf;
	ZOUT0(0) = buf->samplerate * unit->mWorld->mFullRate.mSampleDur;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

inline int32 BUFMASK(int32 x)
{
	return (1 << (31 - CLZ(x))) - 1;
}

static void LocalBuf_allocBuffer(LocalBuf *unit, SndBuf *buf, int numChannels, int numFrames)
{
	int numSamples = numFrames * numChannels;
	// Print("bufnum: %i, allocating %i channels and %i frames. memsize: %i\n", (int)unit->m_fbufnum, numChannels, numFrames, numSamples * sizeof(float));
	const int alignment = 128; // in bytes
	unit->chunk = (float*)RTAlloc(unit->mWorld, numSamples * sizeof(float) + alignment);

	if (!unit->chunk) {
		if(unit->mWorld->mVerbosity > -2){
			Print("failed to allocate memory for LocalBuffer\n");
		}
		return;
	}

	buf->data = (float*) ((intptr_t)((char*)unit->chunk + (alignment - 1)) & -alignment);

	buf->channels = numChannels;
	buf->frames   = numFrames;
	buf->samples  = numSamples;
	buf->mask     = BUFMASK(numSamples); // for delay lines
	buf->mask1    = buf->mask - 1;	// for oscillators
	buf->samplerate = unit->mWorld->mSampleRate;
	buf->sampledur = 1. / buf->samplerate;
#if SUPERNOVA
	buf->isLocal  = true;
#endif
}

void LocalBuf_Ctor(LocalBuf *unit)
{
	Graph *parent = unit->mParent;

	int offset =  unit->mWorld->mNumSndBufs;
	int bufnum =  parent->localBufNum;
	float fbufnum;

	if (parent->localBufNum >= parent->localMaxBufNum) {
		fbufnum = -1.f;
		if(unit->mWorld->mVerbosity > -2)
			printf("warning: LocalBuf tried to allocate too many local buffers.\n");
	} else {
		fbufnum = (float) (bufnum + offset);
		unit->m_buf =  parent->mLocalSndBufs + bufnum;
		parent->localBufNum = parent->localBufNum + 1;

		LocalBuf_allocBuffer(unit, unit->m_buf, (int)IN0(0), (int)IN0(1));
	}

	OUT0(0) = fbufnum;
}

void LocalBuf_Dtor(LocalBuf *unit)
{
	RTFree(unit->mWorld, unit->chunk);
	if(unit->mParent->localBufNum <= 1) { // only the last time.
		for (int i = 0; i != unit->mParent->localMaxBufNum; ++i)
			unit->mParent->mLocalSndBufs[i].~SndBuf();
		RTFree(unit->mWorld, unit->mParent->mLocalSndBufs);
		unit->mParent->localMaxBufNum = 0;
	} else
		unit->mParent->localBufNum = unit->mParent->localBufNum - 1;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

void MaxLocalBufs_Ctor(MaxLocalBufs *unit)
{
	Graph *parent = unit->mParent;

	int maxBufNum = (int)(IN0(0) + .5f);
	if(!parent->localMaxBufNum) {
		parent->mLocalSndBufs = (SndBuf*)RTAlloc(unit->mWorld, maxBufNum * sizeof(SndBuf));
#ifdef SUPERNOVA
		for	(int i = 0; i != maxBufNum; ++i)
			new(&parent->mLocalSndBufs[i]) SndBuf();
#endif
		parent->localMaxBufNum = maxBufNum;
	} else
		printf("warning: MaxLocalBufs - maximum number of local buffers is already declared (%i) and must remain unchanged.\n", parent->localMaxBufNum);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void SetBuf_Ctor(SetBuf *unit)
{
	OUT0(0) = 0.f;
	CTOR_GET_BUF
	if (!buf || !buf->data) {
		if(unit->mWorld->mVerbosity > -2){
			Print("SetBuf: no valid buffer\n");
		}
		return;
	}

	int offset = (int)IN0(1);
	int numArgs = (int)IN0(2);
	int end = sc_min(buf->samples, numArgs + offset);

	int j = 3;
	for(int i=offset; i<end; ++j, ++i) {
		buf->data[i] = IN0(j);
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////

void ClearBuf_Ctor(ClearBuf *unit)
{
	OUT0(0) = 0.f;
	CTOR_GET_BUF

	if (!buf || !buf->data) {
		if(unit->mWorld->mVerbosity > -2){
			Print("ClearBuf: no valid buffer\n");
		}
		return;
	}

	Clear(buf->samples, buf->data);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double sc_loop(Unit *unit, double in, double hi, int loop)
{
	// avoid the divide if possible
	if (in >= hi) {
		if (!loop) {
			unit->mDone = true;
			return hi;
		}
		in -= hi;
		if (in < hi) return in;
	} else if (in < 0.) {
		if (!loop) {
			unit->mDone = true;
			return 0.;
		}
		in += hi;
		if (in >= 0.) return in;
	} else return in;

	return in - hi * floor(in/hi);
}

#define CHECK_BUF \
	if (!bufData) { \
		unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	}

static inline bool checkBuffer(Unit * unit, const float * bufData, uint32 bufChannels,
							   uint32 expectedChannels, int inNumSamples)
{
	if (!bufData)
		goto handle_failure;

	if (expectedChannels > bufChannels) {
		if(unit->mWorld->mVerbosity > -1 && !unit->mDone)
			Print("Buffer UGen channel mismatch: expected %i, yet buffer has %i channels\n",
				  expectedChannels, bufChannels);
		goto handle_failure;
	}
	return true;

handle_failure:
	unit->mDone = true;
	ClearUnitOutputs(unit, inNumSamples);
	return false;
}


#define SETUP_IN(offset) \
	uint32 numInputs = unit->mNumInputs - (uint32)offset; \
	if (numInputs != bufChannels) { \
		if(unit->mWorld->mVerbosity > -1 && !unit->mDone){ \
			Print("buffer-writing UGen channel mismatch: numInputs %i, yet buffer has %i channels\n", numInputs, bufChannels); \
		} \
		unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	if(!unit->mIn){ \
		unit->mIn = (float**)RTAlloc(unit->mWorld, numInputs * sizeof(float*)); \
		if (unit->mIn == NULL) { \
			unit->mDone = true; \
			ClearUnitOutputs(unit, inNumSamples); \
			return; \
		} \
	} \
	float **in = unit->mIn; \
	for (uint32 i=0; i<numInputs; ++i) { \
		in[i] = ZIN(i+offset); \
	}

#define TAKEDOWN_IN \
	if(unit->mIn){ \
		RTFree(unit->mWorld, unit->mIn); \
	}

#define LOOP_INNER_BODY_1(SAMPLE_INDEX) \
	OUT(channel)[SAMPLE_INDEX] = table1[index]; \

#define LOOP_INNER_BODY_2(SAMPLE_INDEX) \
	float b = table1[index]; \
	float c = table2[index]; \
	OUT(channel)[SAMPLE_INDEX] = b + fracphase * (c - b); \

#define LOOP_INNER_BODY_4(SAMPLE_INDEX) \
	float a = table0[index]; \
	float b = table1[index]; \
	float c = table2[index]; \
	float d = table3[index]; \
	OUT(channel)[SAMPLE_INDEX] = cubicinterp(fracphase, a, b, c, d); \


#define LOOP_BODY_4(SAMPLE_INDEX) \
		phase = sc_loop((Unit*)unit, phase, loopMax, loop); \
		int32 iphase = (int32)phase; \
		const float* table1 = bufData + iphase * bufChannels; \
		const float* table0 = table1 - bufChannels; \
		const float* table2 = table1 + bufChannels; \
		const float* table3 = table2 + bufChannels; \
		if (iphase == 0) { \
			if (loop) { \
				table0 += bufSamples; \
			} else { \
				table0 += bufChannels; \
			} \
		} else if (iphase >= guardFrame) { \
			if (iphase == guardFrame) { \
				if (loop) { \
					table3 -= bufSamples; \
				} else { \
					table3 -= bufChannels; \
				} \
			} else { \
				if (loop) { \
					table2 -= bufSamples; \
					table3 -= bufSamples; \
				} else { \
					table2 -= bufChannels; \
					table3 -= 2 * bufChannels; \
				} \
			} \
		} \
		int32 index = 0; \
		float fracphase = phase - (double)iphase; \
		if(numOutputs == bufChannels) { \
			for (uint32 channel=0; channel<numOutputs; ++channel) { \
				LOOP_INNER_BODY_4(SAMPLE_INDEX) \
				index++; \
			} \
		} else if (numOutputs < bufChannels) { \
			for (uint32 channel=0; channel<numOutputs; ++channel) { \
				LOOP_INNER_BODY_4(SAMPLE_INDEX) \
				index++; \
			} \
			index += (bufChannels - numOutputs); \
		} else { \
			for (uint32 channel=0; channel<bufChannels; ++channel) { \
				LOOP_INNER_BODY_4(SAMPLE_INDEX) \
				index++; \
			} \
			for (uint32 channel=bufChannels; channel<numOutputs; ++channel) { \
				OUT(channel)[SAMPLE_INDEX] = 0.f; \
				index++; \
			} \
		} \


#define LOOP_BODY_2(SAMPLE_INDEX) \
		phase = sc_loop((Unit*)unit, phase, loopMax, loop); \
		int32 iphase = (int32)phase; \
		const float* table1 = bufData + iphase * bufChannels; \
		const float* table2 = table1 + bufChannels; \
		if (iphase > guardFrame) { \
			if (loop) { \
				table2 -= bufSamples; \
			} else { \
				table2 -= bufChannels; \
			} \
		} \
		int32 index = 0; \
		float fracphase = phase - (double)iphase; \
		if(numOutputs == bufChannels) { \
			for (uint32 channel=0; channel<numOutputs; ++channel) { \
				LOOP_INNER_BODY_2(SAMPLE_INDEX) \
				index++; \
			} \
		} else if (numOutputs < bufChannels) { \
			for (uint32 channel=0; channel<numOutputs; ++channel) { \
				LOOP_INNER_BODY_2(SAMPLE_INDEX) \
				index++; \
			} \
			index += (bufChannels - numOutputs); \
		} else { \
			for (uint32 channel=0; channel<bufChannels; ++channel) { \
				LOOP_INNER_BODY_2(SAMPLE_INDEX) \
				index++; \
			} \
			for (uint32 channel=bufChannels; channel<numOutputs; ++channel) { \
				OUT(channel)[SAMPLE_INDEX] = 0.f; \
				index++; \
			} \
		} \


#define LOOP_BODY_1(SAMPLE_INDEX) \
		phase = sc_loop((Unit*)unit, phase, loopMax, loop); \
		int32 iphase = (int32)phase; \
		const float* table1 = bufData + iphase * bufChannels; \
		int32 index = 0; \
		if(numOutputs == bufChannels) { \
			for (uint32 channel=0; channel<numOutputs; ++channel) { \
				LOOP_INNER_BODY_1(SAMPLE_INDEX) \
				index++; \
			} \
		} else if (numOutputs < bufChannels) { \
			for (uint32 channel=0; channel<numOutputs; ++channel) { \
				LOOP_INNER_BODY_1(SAMPLE_INDEX) \
				index++; \
			} \
			index += (bufChannels - numOutputs); \
		} else { \
			for (uint32 channel=0; channel<bufChannels; ++channel) { \
				LOOP_INNER_BODY_1(SAMPLE_INDEX) \
				index++; \
			} \
			for (uint32 channel=bufChannels; channel<numOutputs; ++channel) { \
				OUT(channel)[SAMPLE_INDEX] = 0.f; \
				index++; \
			} \
		} \



#define CHECK_BUFFER_DATA \
if (!bufData) { \
	if(unit->mWorld->mVerbosity > -1 && !unit->mDone && (unit->m_failedBufNum != fbufnum)) { \
		Print("Buffer UGen: no buffer data\n"); \
		unit->m_failedBufNum = fbufnum; \
	} \
	ClearUnitOutputs(unit, inNumSamples); \
	return; \
} else { \
	if (bufChannels != numOutputs) { \
		if(unit->mWorld->mVerbosity > -1 && !unit->mDone && (unit->m_failedBufNum != fbufnum)) { \
			Print("Buffer UGen channel mismatch: expected %i, yet buffer has %i channels\n", \
				  numOutputs, bufChannels); \
			unit->m_failedBufNum = fbufnum; \
			} \
		} \
} \



void PlayBuf_Ctor(PlayBuf *unit)
{
	if (INRATE(1) == calc_FullRate) {
		if (INRATE(2) == calc_FullRate) {
			SETCALC(PlayBuf_next_aa);
		} else {
			SETCALC(PlayBuf_next_ak);
		}
	} else {
		if (INRATE(2) == calc_FullRate) {
			SETCALC(PlayBuf_next_ka);
		} else {
			SETCALC(PlayBuf_next_kk);
		}
	}

	unit->m_fbufnum = -1e9f;
	unit->m_failedBufNum = -1e9f;
	unit->m_prevtrig = 0.;
	unit->m_phase = ZIN0(3);

	ClearUnitOutputs(unit, 1);
}


void PlayBuf_next_aa(PlayBuf *unit, int inNumSamples)
{
	float *ratein  = ZIN(1);
	float *trigin  = ZIN(2);
	int32 loop     = (int32)ZIN0(4);

	float fbufnum  = ZIN0(0);
	if (fbufnum != unit->m_fbufnum) {
		uint32 bufnum = (int)fbufnum;
		World *world = unit->mWorld;
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->m_fbufnum = fbufnum;
		unit->m_buf = world->mSndBufs + bufnum;
	}
	const SndBuf *buf = unit->m_buf;
	ACQUIRE_SNDBUF_SHARED(buf);
	const float *bufData __attribute__((__unused__)) = buf->data;
	uint32 bufChannels __attribute__((__unused__)) = buf->channels;
	uint32 bufSamples __attribute__((__unused__)) = buf->samples;
	uint32 bufFrames = buf->frames;
	int mask __attribute__((__unused__)) = buf->mask;
	int guardFrame __attribute__((__unused__)) = bufFrames - 2;

	int numOutputs = unit->mNumOutputs;

	CHECK_BUFFER_DATA;

	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);
	double phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;

	for (int i=0; i<inNumSamples; ++i) {
		float trig = ZXP(trigin);
		if (trig > 0.f && prevtrig <= 0.f) {
			unit->mDone = false;
			phase = ZIN0(3);
		}
		prevtrig = trig;

		LOOP_BODY_4(i)

		phase += ZXP(ratein);
	}
	RELEASE_SNDBUF_SHARED(buf);

	if(unit->mDone)
		DoneAction((int)ZIN0(5), unit);
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
}

void PlayBuf_next_ak(PlayBuf *unit, int inNumSamples)
{
	float *ratein  = ZIN(1);
	float trig     = ZIN0(2);
	int32 loop     = (int32)ZIN0(4);

	float fbufnum  = ZIN0(0);
	if (fbufnum != unit->m_fbufnum) {
		uint32 bufnum = (int)fbufnum;
		World *world = unit->mWorld;
		if (bufnum >= world->mNumSndBufs) bufnum = 0;
		unit->m_fbufnum = fbufnum;
		unit->m_buf = world->mSndBufs + bufnum;
	}
	const SndBuf *buf = unit->m_buf;
	ACQUIRE_SNDBUF_SHARED(buf);
	const float *bufData __attribute__((__unused__)) = buf->data;
	uint32 bufChannels __attribute__((__unused__)) = buf->channels;
	uint32 bufSamples __attribute__((__unused__)) = buf->samples;
	uint32 bufFrames = buf->frames;
	int mask __attribute__((__unused__)) = buf->mask;
	int guardFrame __attribute__((__unused__)) = bufFrames - 2;

	int numOutputs = unit->mNumOutputs;

	CHECK_BUFFER_DATA

	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);
	double phase = unit->m_phase;
	if(phase == -1.) phase = bufFrames;
	if (trig > 0.f && unit->m_prevtrig <= 0.f) {
		unit->mDone = false;
		phase = ZIN0(3);
	}
	unit->m_prevtrig = trig;
	for (int i=0; i<inNumSamples; ++i) {

		LOOP_BODY_4(i)

		phase += ZXP(ratein);
	}
	RELEASE_SNDBUF_SHARED(buf);
	if(unit->mDone)
		DoneAction((int)ZIN0(5), unit);
	unit->m_phase = phase;
}

void PlayBuf_next_kk(PlayBuf *unit, int inNumSamples)
{
	float rate     = ZIN0(1);
	float trig     = ZIN0(2);
	int32 loop     = (int32)ZIN0(4);

	GET_BUF_SHARED
	int numOutputs = unit->mNumOutputs;

	CHECK_BUFFER_DATA

	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);
	double phase = unit->m_phase;
	if (trig > 0.f && unit->m_prevtrig <= 0.f) {
		unit->mDone = false;
		phase = ZIN0(3);
	}
	unit->m_prevtrig = trig;
	for (int i=0; i<inNumSamples; ++i) {
		LOOP_BODY_4(i)

		phase += rate;
	}
	if(unit->mDone)
		DoneAction((int)ZIN0(5), unit);
	unit->m_phase = phase;
}

void PlayBuf_next_ka(PlayBuf *unit, int inNumSamples)
{
	float rate     = ZIN0(1);
	float *trigin  = ZIN(2);
	int32 loop     = (int32)ZIN0(4);

	GET_BUF_SHARED
	int numOutputs = unit->mNumOutputs;

	CHECK_BUFFER_DATA

	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);
	double phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	for (int i=0; i<inNumSamples; ++i) {
		float trig = ZXP(trigin);
		if (trig > 0.f && prevtrig <= 0.f) {
			unit->mDone = false;
			if (INRATE(3) == calc_FullRate) phase = IN(3)[i];
			else phase = ZIN0(3);
		}
		prevtrig = trig;

		LOOP_BODY_4(i)

		phase += rate;
	}
	if(unit->mDone)
		DoneAction((int)ZIN0(5), unit);
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufRd_Ctor(BufRd *unit)
{
	int interp = (int)ZIN0(3);
	switch (interp) {
		case 1 : SETCALC(BufRd_next_1); break;
		case 2 : SETCALC(BufRd_next_2); break;
		default : SETCALC(BufRd_next_4); break;
	}

	unit->m_fbufnum = -1e9f;
	unit->m_failedBufNum = -1e9f;

	BufRd_next_1(unit, 1);
}

void BufRd_next_4(BufRd *unit, int inNumSamples)
{
	float *phasein = ZIN(1);
	int32 loop     = (int32)ZIN0(2);

	GET_BUF_SHARED
	uint32 numOutputs = unit->mNumOutputs;

	CHECK_BUFFER_DATA

	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);

	for (int i=0; i<inNumSamples; ++i) {
		double phase = ZXP(phasein);
		LOOP_BODY_4(i)
	}
}

void BufRd_next_2(BufRd *unit, int inNumSamples)
{
	float *phasein = ZIN(1);
	int32 loop     = (int32)ZIN0(2);

	GET_BUF_SHARED
	uint32 numOutputs = unit->mNumOutputs;

	CHECK_BUFFER_DATA

	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);

	for (int i=0; i<inNumSamples; ++i) {
		double phase = ZXP(phasein);
		LOOP_BODY_2(i)
	}
}

void BufRd_next_1(BufRd *unit, int inNumSamples)
{
	float *phasein = ZIN(1);
	int32 loop     = (int32)ZIN0(2);

	GET_BUF_SHARED
	uint32 numOutputs = unit->mNumOutputs;

	CHECK_BUFFER_DATA

	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);

	for (int i=0; i<inNumSamples; ++i) {
		double phase = ZXP(phasein);
		LOOP_BODY_1(i)
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufWr_Ctor(BufWr *unit)
{
	SETCALC(BufWr_next);

	unit->m_fbufnum = -1e9f;

	ClearUnitOutputs(unit, 1);
}

void BufWr_next(BufWr *unit, int inNumSamples)
{
	float *phasein  = ZIN(1);
	int32 loop     = (int32)ZIN0(2);

	GET_BUF
	uint32 numInputChannels = unit->mNumInputs - 3;
	if (!checkBuffer(unit, bufData, bufChannels, numInputChannels, inNumSamples))
		return;

	double loopMax = (double)(bufFrames - (loop ? 0 : 1));

	for (int32 k=0; k<inNumSamples; ++k) {
		double phase = sc_loop((Unit*)unit, ZXP(phasein), loopMax, loop);
		int32 iphase = (int32)phase;
		float* table0 = bufData + iphase * bufChannels;
		for (uint32 channel=0; channel<numInputChannels; ++channel)
			table0[channel] = IN(channel+3)[k];
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

//bufnum=0, offset=0.0, recLevel=1.0, preLevel=0.0, run=1.0, loop=1.0, trigger=1.0

void RecordBuf_Ctor(RecordBuf *unit)
{

	uint32 numInputs = unit->mNumInputs - 8;
	unit->m_fbufnum = -1e9f;
	unit->mIn = 0;
	unit->m_writepos = (int32)ZIN0(1) * numInputs;
	unit->m_recLevel = ZIN0(2);
	unit->m_preLevel = ZIN0(3);
	unit->m_prevtrig = 0.f;

	if (INRATE(2) == calc_ScalarRate && INRATE(3) == calc_ScalarRate
		&& unit->m_recLevel == 1.0 && unit->m_preLevel == 0.0)
	{
		SETCALC(RecordBuf_next_10);
	} else {
		SETCALC(RecordBuf_next);
	}

	ClearUnitOutputs(unit, 1);
}

void RecordBuf_Dtor(RecordBuf *unit)
{
	TAKEDOWN_IN
}

void RecordBuf_next(RecordBuf *unit, int inNumSamples)
{
	//printf("RecordBuf_next\n");
	GET_BUF
	CHECK_BUF
	SETUP_IN(8)

	float recLevel = ZIN0(2);
	float preLevel = ZIN0(3);
	float run      = ZIN0(4);
	int32 loop     = (int32)ZIN0(5);
	float trig     = ZIN0(6);
	//printf("loop %d  run %g\n", loop, run);

	int32 writepos = unit->m_writepos;

	float recLevel_slope = CALCSLOPE(recLevel, unit->m_recLevel);
	float preLevel_slope = CALCSLOPE(preLevel, unit->m_preLevel);

	/* reset recLevel and preLevel to use the previous value ... bug fix */
	recLevel = unit->m_recLevel;
	preLevel = unit->m_preLevel;

	if (loop) {
		if (trig > 0.f && unit->m_prevtrig <= 0.f) {
			unit->mDone = false;
			writepos = (int32)ZIN0(1) * bufChannels;
		}
		if (writepos < 0) writepos = bufSamples - bufChannels;
		else if (writepos >= (int32)bufSamples) writepos = 0;
		if (run > 0.f) {
			if (bufChannels == 1) {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]) * recLevel + table0[0] * preLevel;
					writepos += 1;
					if (writepos >= (int32)bufSamples) writepos = 0;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			} else if (bufChannels == 2 && numInputs == 2) {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]) * recLevel + table0[0] * preLevel;
					table0[1] = *++(in[1]) * recLevel + table0[1] * preLevel;
					writepos += 2;
					if (writepos >= (int32)bufSamples) writepos = 0;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			} else {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					for (uint32 i=0; i<numInputs; ++i) {
						float *samp = table0 + i;
						*samp = *++(in[i]) * recLevel + *samp * preLevel;
					}
					writepos += bufChannels;
					if (writepos >= (int32)bufSamples) writepos = 0;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			}
		} else if (run < 0.f) {
			if (bufChannels == 1) {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]) * recLevel + table0[0] * preLevel;
					writepos -= 1;
					if (writepos < 0) writepos = bufSamples - bufChannels;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			} else if (bufChannels == 2 && numInputs == 2) {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]) * recLevel + table0[0] * preLevel;
					table0[1] = *++(in[1]) * recLevel + table0[1] * preLevel;
					writepos -= 2;
					if (writepos < 0) writepos = bufSamples - bufChannels;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			} else {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					for (uint32 i=0; i<numInputs; ++i) {
						float *samp = table0 + i;
						*samp = *++(in[i]) * recLevel + *samp * preLevel;
					}
					writepos -= bufChannels;
					if (writepos < 0) writepos = bufSamples - bufChannels;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			}
		}
	} else {
		if (trig > 0.f && unit->m_prevtrig <= 0.f) {
			unit->mDone = false;
			writepos = (int32)ZIN0(1) * bufChannels;
		}
		if (run > 0.f) {
			int nsmps = bufSamples - writepos;
			nsmps = sc_clip(nsmps, 0, inNumSamples * bufChannels);
			if (bufChannels == 1) {
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]) * recLevel + table0[0] * preLevel;
					writepos += 1;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			} else if (bufChannels == 2 && numInputs == 2) {
				nsmps = nsmps/2;
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]) * recLevel + table0[0] * preLevel;
					table0[1] = *++(in[1]) * recLevel + table0[1] * preLevel;
					writepos += 2;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			} else {
				nsmps = nsmps/bufChannels;
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					for (uint32 i=0; i<numInputs; ++i) {
						float *samp = table0 + i;
						*samp = *++(in[i]) * recLevel + *samp * preLevel;
					}
					writepos += bufChannels;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			}
		} else if (run < 0.f) {
			int nsmps = writepos;
			nsmps = sc_clip(nsmps, 0, inNumSamples * bufChannels);
			if (bufChannels == 1) {
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]) * recLevel + table0[0] * preLevel;
					writepos -= bufChannels;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			} else if (bufChannels == 2 && numInputs == 2) {
				nsmps = nsmps/2;
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]) * recLevel + table0[0] * preLevel;
					table0[1] = *++(in[1]) * recLevel + table0[1] * preLevel;
					writepos -= bufChannels;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			} else {
				nsmps = nsmps/bufChannels;
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					for (uint32 i=0; i<numInputs; ++i) {
						float *samp = table0 + i;
						*samp = *++(in[i]) * recLevel + *samp * preLevel;
					}
					writepos -= bufChannels;

					recLevel += recLevel_slope;
					preLevel += preLevel_slope;
				}
			}
		}
		if (writepos >= (int32)bufSamples){
			unit->mDone = true;
			DoneAction(IN0(7), unit);
		}
	}
	unit->m_prevtrig = trig;
	unit->m_writepos = writepos;
	unit->m_recLevel = recLevel;
	unit->m_preLevel = preLevel;
}

void RecordBuf_next_10(RecordBuf *unit, int inNumSamples)
{
	// printf("RecordBuf_next_10\n");
	GET_BUF
	CHECK_BUF
	SETUP_IN(8)

	float run      = ZIN0(4);
	int32 loop     = (int32)ZIN0(5);
	float trig     = ZIN0(6);
	//printf("loop %d  run %g\n", loop, run);

	int32 writepos = unit->m_writepos;

	if (loop) {
		if (trig > 0.f && unit->m_prevtrig <= 0.f) {
			unit->mDone = false;
			writepos = (int32)ZIN0(1) * bufChannels;
		}
		if (writepos < 0) writepos = bufSamples - bufChannels;
		else if (writepos >= (int32)bufSamples) writepos = 0;
		if (run > 0.f) {
			if (bufChannels == 1) {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]);
					writepos += 1;
					if (writepos >= (int32)bufSamples) writepos = 0;
				}
			} else if (bufChannels == 2) {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]);
					table0[1] = *++(in[1]);
					writepos += 2;
					if (writepos >= (int32)bufSamples) writepos = 0;
				}
			} else {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					for (uint32 i=0; i<bufChannels; ++i) {
						float *samp = table0 + i;
						*samp = *++(in[i]);
					}
					writepos += bufChannels;
					if (writepos >= (int32)bufSamples) writepos = 0;
				}
			}
		} else if (run < 0.f) {
			if (bufChannels == 1) {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]);
					writepos -= 1;
					if (writepos < 0) writepos = bufSamples - bufChannels;
				}
			} else if (bufChannels == 2) {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]);
					table0[1] = *++(in[1]);
					writepos -= 2;
					if (writepos < 0) writepos = bufSamples - bufChannels;
				}
			} else {
				for (int32 k=0; k<inNumSamples; ++k) {
					float* table0 = bufData + writepos;
					for (uint32 i=0; i<bufChannels; ++i) {
						float *samp = table0 + i;
						*samp = *++(in[i]);
					}
					writepos -= bufChannels;
					if (writepos < 0) writepos = bufSamples - bufChannels;
				}
			}
		}
	} else {
		if (trig > 0.f && unit->m_prevtrig <= 0.f) {
			unit->mDone = false;
			writepos = (int32)ZIN0(1) * bufChannels;
		}
		if (run > 0.f) {
			int nsmps = bufSamples - writepos;
			nsmps = sc_clip(nsmps, 0, inNumSamples * bufChannels);
			if (bufChannels == 1) {
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]);
					writepos += 1;
				}
			} else if (bufChannels == 2) {
				nsmps = nsmps/2;
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]);
					table0[1] = *++(in[1]);
					writepos += 2;
				}
			} else {
				nsmps = nsmps/bufChannels;
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					for (uint32 i=0; i<bufChannels; ++i) {
						float *samp = table0 + i;
						*samp = *++(in[i]);
					}
					writepos += bufChannels;
				}
			}
		} else if (run < 0.f) {
			int nsmps = writepos;
			nsmps = sc_clip(nsmps, 0, inNumSamples * bufChannels);
			if (bufChannels == 1) {
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]);
					writepos -= 1;
				}
			} else if (bufChannels == 2) {
				nsmps = nsmps/2;
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					table0[0] = *++(in[0]);
					table0[1] = *++(in[1]);
					writepos -= 2;
				}
			} else {
				nsmps = nsmps/bufChannels;
				for (int32 k=0; k<nsmps; ++k) {
					float* table0 = bufData + writepos;
					for (uint32 i=0; i<bufChannels; ++i) {
						float *samp = table0 + i;
						*samp = *++(in[i]);
					}
					writepos -= bufChannels;
				}
			}
		}

		if (writepos >= (int32)bufSamples){
			unit->mDone = true;
			DoneAction(IN0(7), unit);
		}
	}
	unit->m_prevtrig = trig;
	unit->m_writepos = writepos;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////



static float insertMedian(float* values, int* ages, int size, float value)
{
	int pos=-1;

	// keeps a sorted list of the previous n=size values
	// the oldest is removed and the newest is inserted.
	// values between the oldest and the newest are shifted over by one.

	// values and ages are both arrays that are 'size' int.
	// the median value is always values[size>>1]

	int last = size - 1;
	// find oldest bin and age the other bins.
	for (int i=0; i<size; ++i) {
		if (ages[i] == last) { // is it the oldest bin ?
			pos = i;
		} else {
			ages[i]++;	// age the bin
		}
	}
	// move values to fill in place of the oldest and make a space for the newest
	// search lower if value is too small for the open space
	while (pos != 0 && value < values[pos-1]) {
		values[pos] = values[pos-1];
		ages[pos] = ages[pos-1];
		pos--;
	}
	// search higher if value is too big for the open space
	while (pos != last && value > values[pos+1]) {
		values[pos] = values[pos+1];
		ages[pos] = ages[pos+1];
		pos++;
	}
	values[pos] = value;
	ages[pos] = 0;		// this is the newest bin, age = 0
	return values[size>>1];
}

static void initMedian(float* values, int* ages, int size, float value)
{
	// initialize the arrays with the first value
	for (int i=0; i<size; ++i) {
		values[i] = value;
		ages[i] = i;
	}
}




enum {
	kPitchIn,
	kPitchInitFreq,
	kPitchMinFreq,
	kPitchMaxFreq,
	kPitchExecFreq,
	kPitchMaxBins,
	kPitchMedian,
	kPitchAmpThreshold,
	kPitchPeakThreshold,
	kPitchDownsamp,
	kPitchGetClarity
};

void Pitch_Ctor(Pitch *unit)
{
	unit->m_freq = ZIN0(kPitchInitFreq);
	unit->m_minfreq = ZIN0(kPitchMinFreq);
	unit->m_maxfreq = ZIN0(kPitchMaxFreq);

	float execfreq = ZIN0(kPitchExecFreq);
	execfreq = sc_clip(execfreq, unit->m_minfreq, unit->m_maxfreq);

	int maxbins = (int)ZIN0(kPitchMaxBins);
	unit->m_maxlog2bins = LOG2CEIL(maxbins);

	unit->m_medianSize = sc_clip((int)ZIN0(kPitchMedian), 0, kMAXMEDIANSIZE);
	unit->m_ampthresh = ZIN0(kPitchAmpThreshold);
	unit->m_peakthresh = ZIN0(kPitchPeakThreshold);

	int downsamp = (int)ZIN0(kPitchDownsamp);

	if (INRATE(kPitchIn) == calc_FullRate) {
		SETCALC(Pitch_next_a);
		unit->m_downsamp = sc_clip(downsamp, 1, unit->mWorld->mFullRate.mBufLength);
		unit->m_srate = FULLRATE / (float)unit->m_downsamp;
	} else {
		SETCALC(Pitch_next_k);
	 	unit->m_downsamp = sc_max(downsamp, 1);
		unit->m_srate = FULLRATE / (float) (unit->mWorld->mFullRate.mBufLength*unit->m_downsamp);
	}

	unit->m_minperiod = (long)(unit->m_srate / unit->m_maxfreq);
	unit->m_maxperiod = (long)(unit->m_srate / unit->m_minfreq);

	unit->m_execPeriod = (int)(unit->m_srate / execfreq);
	unit->m_execPeriod = sc_max(unit->m_execPeriod, unit->mWorld->mFullRate.mBufLength);

	unit->m_size = sc_max(unit->m_maxperiod << 1, unit->m_execPeriod);

	unit->m_buffer = (float*)RTAlloc(unit->mWorld, unit->m_size * sizeof(float));

	unit->m_index = 0;
	unit->m_readp = 0;
	unit->m_hasfreq = 0.f;

	initMedian(unit->m_values, unit->m_ages, unit->m_medianSize, unit->m_freq);

	unit->m_getClarity = ZIN0(kPitchGetClarity) > 0.f;

	ZOUT0(0) = 0.f;
	ZOUT0(1) = 0.f;
}

void Pitch_Dtor(Pitch *unit)
{
	RTFree(unit->mWorld, unit->m_buffer);
}

void Pitch_next_a(Pitch *unit, int inNumSamples)
{
	bool foundPeak;

	float* in = ZIN(kPitchIn);
	uint32 size = unit->m_size;
	uint32 index = unit->m_index;
	int downsamp = unit->m_downsamp;
	int readp = unit->m_readp;
	int ksamps = unit->mWorld->mFullRate.mBufLength;

	float *bufData = unit->m_buffer;

	float freq = unit->m_freq;
	float hasfreq = unit->m_hasfreq;
	//printf("> %d %d readp %d ksamps %d ds %d\n", index, size, readp, ksamps, downsamp);
	do {
		float z = in[readp];
		bufData[index++] = z;
		readp += downsamp;

		if (index >= size) {
			float ampthresh = unit->m_ampthresh;
			bool ampok = false;

			hasfreq = 0.f; // assume failure

			int minperiod = unit->m_minperiod;
			int maxperiod = unit->m_maxperiod;
			//float maxamp = 0.f;
			// check for amp threshold
			for (int j = 0; j < maxperiod; ++j) {
				if (fabs(bufData[j]) >= ampthresh) {
					ampok = true;
					break;
				}
				//if (fabs(bufData[j]) > maxamp) maxamp = fabs(bufData[j]);
			}
			//printf("ampok %d  maxperiod %d  maxamp %g\n", ampok, maxperiod,  maxamp);

			// if amplitude is too small then don't even look for pitch
			float ampsum;
			if (ampok) {
				int maxlog2bins = unit->m_maxlog2bins;
				int octave;
				// calculate the zero lag value and compute the threshold based on that
				float zerolagval = 0.f;
				for (int j = 0; j < maxperiod; ++j) {
					zerolagval += bufData[j] * bufData[j];
				}
				float threshold = zerolagval * unit->m_peakthresh;

				// skip until drop below threshold
				int binstep, peakbinstep = 0;
				int i;
				for (i = 1; i <= maxperiod; i += binstep) {
					// compute sum of one lag
					ampsum = 0.f;
					for (int j = 0; j < maxperiod; ++j) {
						ampsum += bufData[i+j] * bufData[j];
					}
					if (ampsum < threshold) break;

					octave = LOG2CEIL(i);
					if (octave <= maxlog2bins) {
						binstep = 1;
					} else {
						binstep = 1L << (octave - maxlog2bins);
					}
				}
				int startperiod = i;
				int period = startperiod;
				//printf("startperiod %d\n", startperiod);

				// find the first peak
				float maxsum = threshold;
				foundPeak = false;
				for (i = startperiod; i <= maxperiod; i += binstep) {
					if (i >= minperiod) {
						ampsum = 0.f;
						for (int j = 0; j < maxperiod; ++j) {
							ampsum += bufData[i+j] * bufData[j];
						}
						if (ampsum > threshold) {
							if (ampsum > maxsum) {
								foundPeak = true;
								maxsum = ampsum;
								peakbinstep = binstep;
								period = i;
							}
						} else if (foundPeak) break;
					}
					octave = LOG2CEIL(i);
					if (octave <= maxlog2bins) {
						binstep = 1;
					} else {
						binstep = 1L << (octave - maxlog2bins);
					}
				}

				//printf("found %d  thr %g  maxs %g  per %d  bs %d\n", foundPeak, threshold, maxsum, period, peakbinstep);
				if (foundPeak) {
					float prevampsum, nextampsum;

					// find amp sums immediately surrounding max
					prevampsum = 0.f;
					if (period > 0) {
						i = period - 1;
						for (int j = 0; j < maxperiod; ++j) {
							prevampsum += bufData[i+j] * bufData[j];
						}
					}

					nextampsum = 0.f;
					if (period < maxperiod) {
						i = period + 1;
						for (int j = 0; j < maxperiod; ++j) {
							nextampsum += bufData[i+j] * bufData[j];
						}
					}

					//printf("prevnext %g %g %g   %d\n", prevampsum, maxsum, nextampsum, period);
					// not on a peak yet. This can happen if binstep > 1
					while (prevampsum > maxsum && period > 0) {
						nextampsum = maxsum;
						maxsum = prevampsum;
						period--;
						i = period - 1;
						prevampsum = 0.f;
						for (int j = 0; j < maxperiod; ++j) {
							prevampsum += bufData[i+j] * bufData[j];
						}
						//printf("slide left %g %g %g   %d\n", prevampsum, maxsum, nextampsum, period);
					}
					while (nextampsum > maxsum && period < maxperiod) {
						prevampsum = maxsum;
						maxsum = nextampsum;
						period++;
						i = period + 1;
						nextampsum = 0.f;
						for (int j = 0; j < maxperiod; ++j) {
							nextampsum += bufData[i+j] * bufData[j];
						}
						//printf("slide right %g %g %g   %d\n", prevampsum, maxsum, nextampsum, period);
					}

					// make a fractional period
					float beta = 0.5f * (nextampsum - prevampsum);
					float gamma = 2.f  * maxsum - nextampsum - prevampsum;
					float fperiod = (float)period + (beta/gamma);

					// calculate frequency
					float tempfreq = unit->m_srate / fperiod;

					//printf("freq %g   %g / %g    %g %g  %d\n", tempfreq, unit->m_srate, fperiod,
					//	unit->m_minfreq, unit->m_maxfreq,
					//  tempfreq >= unit->m_minfreq && tempfreq <= unit->m_maxfreq);

					if (tempfreq >= unit->m_minfreq && tempfreq <= unit->m_maxfreq) {
						freq = tempfreq;

						// median filter
						if (unit->m_medianSize > 1) {
							freq = insertMedian(unit->m_values, unit->m_ages, unit->m_medianSize, freq);
						}
						if(unit->m_getClarity)
							hasfreq = maxsum / zerolagval; // "clarity" measure is normalised size of first peak
						else
							hasfreq = 1.f;

						startperiod = (ksamps+downsamp-1)/downsamp;
					}
				}
			}/* else {
                printf("amp too low \n");
            }*/

			// shift buffer for next fill
			int execPeriod = unit->m_execPeriod;
			int interval = size - execPeriod;
			//printf("interval %d  sz %d ep %d\n", interval, size, execPeriod);
			for (int i = 0; i < interval; i++) {
				bufData[i] = bufData[i + execPeriod];
			}
			index = interval;
		}
	} while (readp < ksamps);

	ZOUT0(0) = freq;
	ZOUT0(1) = hasfreq;
	unit->m_readp = readp - ksamps;
	unit->m_index = index;
	unit->m_freq = freq;
	unit->m_hasfreq = hasfreq;
}


// control rate pitch tracking (nescivi 11/2008)
void Pitch_next_k(Pitch *unit, int inNumSamples)
{
	bool foundPeak;

	float in = ZIN0(kPitchIn); // one sample, current input
	uint32 size = unit->m_size;
	uint32 index = unit->m_index;
	int downsamp = unit->m_downsamp;
	int readp = unit->m_readp;
//  	int ksamps = unit->mWorld->mFullRate.mBufLength;

	float *bufData = unit->m_buffer;

	float freq = unit->m_freq;
	float hasfreq = unit->m_hasfreq;
// 	printf("> %d %d readp %d downsamp %d exec %d\n", index, size, readp, downsamp, unit->m_execPeriod);
	readp++;
	if ( readp == downsamp ){
// 	do {
// 		float z = in[readp];
		float z = in;
		bufData[index++] = z;
		readp = 0;
// 		readp += downsamp;

		if (index >= size) {
			float ampthresh = unit->m_ampthresh;
			bool ampok = false;

			hasfreq = 0.f; // assume failure

			int minperiod = unit->m_minperiod;
			int maxperiod = unit->m_maxperiod;
			//float maxamp = 0.f;
			// check for amp threshold
			for (int j = 0; j < maxperiod; ++j) {
				if (fabs(bufData[j]) >= ampthresh) {
					ampok = true;
					break;
				}
				//if (fabs(bufData[j]) > maxamp) maxamp = fabs(bufData[j]);
			}
			//printf("ampok %d  maxperiod %d  maxamp %g\n", ampok, maxperiod,  maxamp);

			// if amplitude is too small then don't even look for pitch
			float ampsum;
			if (ampok) {
				int maxlog2bins = unit->m_maxlog2bins;
				int octave;
				// calculate the zero lag value and compute the threshold based on that
				float zerolagval = 0.f;
				for (int j = 0; j < maxperiod; ++j) {
					zerolagval += bufData[j] * bufData[j];
				}
				float threshold = zerolagval * unit->m_peakthresh;

				// skip until drop below threshold
				int binstep, peakbinstep = 0;
				int i;
				for (i = 1; i <= maxperiod; i += binstep) {
					// compute sum of one lag
					ampsum = 0.f;
					for (int j = 0; j < maxperiod; ++j) {
						ampsum += bufData[i+j] * bufData[j];
					}
					if (ampsum < threshold) break;

					octave = LOG2CEIL(i);
					if (octave <= maxlog2bins) {
						binstep = 1;
					} else {
						binstep = 1L << (octave - maxlog2bins);
					}
				}
				int startperiod = i;
				int period = startperiod;
				//printf("startperiod %d\n", startperiod);

				// find the first peak
				float maxsum = threshold;
				foundPeak = false;
				for (i = startperiod; i <= maxperiod; i += binstep) {
					if (i >= minperiod) {
						ampsum = 0.f;
						for (int j = 0; j < maxperiod; ++j) {
							ampsum += bufData[i+j] * bufData[j];
						}
						if (ampsum > threshold) {
							if (ampsum > maxsum) {
								foundPeak = true;
								maxsum = ampsum;
								peakbinstep = binstep;
								period = i;
							}
						} else if (foundPeak) break;
					}
					octave = LOG2CEIL(i);
					if (octave <= maxlog2bins) {
						binstep = 1;
					} else {
						binstep = 1L << (octave - maxlog2bins);
					}
				}

				//printf("found %d  thr %g  maxs %g  per %d  bs %d\n", foundPeak, threshold, maxsum, period, peakbinstep);
				if (foundPeak) {
					float prevampsum, nextampsum;

					// find amp sums immediately surrounding max
					prevampsum = 0.f;
					if (period > 0) {
						i = period - 1;
						for (int j = 0; j < maxperiod; ++j) {
							prevampsum += bufData[i+j] * bufData[j];
						}
					}

					nextampsum = 0.f;
					if (period < maxperiod) {
						i = period + 1;
						for (int j = 0; j < maxperiod; ++j) {
							nextampsum += bufData[i+j] * bufData[j];
						}
					}

					//printf("prevnext %g %g %g   %d\n", prevampsum, maxsum, nextampsum, period);
					// not on a peak yet. This can happen if binstep > 1
					while (prevampsum > maxsum && period > 0) {
						nextampsum = maxsum;
						maxsum = prevampsum;
						period--;
						i = period - 1;
						prevampsum = 0.f;
						for (int j = 0; j < maxperiod; ++j) {
							prevampsum += bufData[i+j] * bufData[j];
						}
						//printf("slide left %g %g %g   %d\n", prevampsum, maxsum, nextampsum, period);
					}
					while (nextampsum > maxsum && period < maxperiod) {
						prevampsum = maxsum;
						maxsum = nextampsum;
						period++;
						i = period + 1;
						nextampsum = 0.f;
						for (int j = 0; j < maxperiod; ++j) {
							nextampsum += bufData[i+j] * bufData[j];
						}
						//printf("slide right %g %g %g   %d\n", prevampsum, maxsum, nextampsum, period);
					}

					// make a fractional period
					float beta = 0.5 * (nextampsum - prevampsum);
					float gamma = 2.0  * maxsum - nextampsum - prevampsum;
					float fperiod = (float)period + (beta/gamma);

					// calculate frequency
					float tempfreq = unit->m_srate / fperiod;

					//printf("freq %g   %g / %g    %g %g  %d\n", tempfreq, unit->m_srate, fperiod,
					//	unit->m_minfreq, unit->m_maxfreq,
					//  tempfreq >= unit->m_minfreq && tempfreq <= unit->m_maxfreq);

					if (tempfreq >= unit->m_minfreq && tempfreq <= unit->m_maxfreq) {
						freq = tempfreq;

						// median filter
						if (unit->m_medianSize > 1) {
							freq = insertMedian(unit->m_values, unit->m_ages, unit->m_medianSize, freq);
						}
						if(unit->m_getClarity)
							hasfreq = maxsum / zerolagval; // "clarity" measure is normalised size of first peak
						else
							hasfreq = 1.f;

						// nescivi: not sure about this one?
						startperiod = 1; // (ksamps+downsamp-1)/downsamp;
					}
				}
			}/* else {
                printf("amp too low \n");
            }*/

			// shift buffer for next fill
			int execPeriod = unit->m_execPeriod;
			int interval = size - execPeriod;
			//printf("interval %d  sz %d ep %d\n", interval, size, execPeriod);
			for (int i = 0; i < interval; i++) {
				bufData[i] = bufData[i + execPeriod];
			}
			index = interval;
		}
	}
//while (readp < ksamps);

	ZOUT0(0) = freq;
	ZOUT0(1) = hasfreq;
// 	unit->m_readp = readp - ksamps;
	unit->m_readp = readp;
	unit->m_index = index;
	unit->m_freq = freq;
	unit->m_hasfreq = hasfreq;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


#if 0
void DelayUnit_AllocDelayLine(DelayUnit *unit)
{
	long delaybufsize = (long)ceil(unit->m_maxdelaytime * SAMPLERATE + 1.f);
	delaybufsize = delaybufsize + BUFLENGTH;
	delaybufsize = NEXTPOWEROFTWO(delaybufsize);  // round up to next power of two
	unit->m_fdelaylen = unit->m_idelaylen = delaybufsize;

	RTFree(unit->mWorld, unit->m_dlybuf);
	int size = delaybufsize * sizeof(float);
	//Print("->RTAlloc %d\n", size);
	unit->m_dlybuf = (float*)RTAlloc(unit->mWorld, size);
	//Print("<-RTAlloc %p\n", unit->m_dlybuf);
	unit->m_mask = delaybufsize - 1;
}
#endif


template <typename Unit>
static float BufCalcDelay(const Unit * unit, int bufSamples, float delayTime)
{
	float minDelay = Unit::minDelaySamples;
	return sc_clip(delayTime * (float)SAMPLERATE, minDelay, (float)(PREVIOUSPOWEROFTWO(bufSamples))-1);
}

template <typename Unit>
static void BufDelayUnit_Reset(Unit *unit)
{
	//Print("->DelayUnit_Reset\n");
	//unit->m_maxdelaytime = ZIN0(1);
	unit->m_delaytime = ZIN0(2);
	//Print("unit->m_delaytime %g\n", unit->m_delaytime);
	//unit->m_dlybuf = 0;
	unit->m_fbufnum = -1e9f;

	//DelayUnit_AllocDelayLine(unit);
	//Print("->GET_BUF\n");
	GET_BUF
	//Print("<-GET_BUF\n");
	unit->m_dsamp = BufCalcDelay(unit, bufSamples, unit->m_delaytime);
	unit->m_numoutput = 0;
	unit->m_iwrphase = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Unit>
static void BufFeedbackDelay_Reset(Unit *unit)
{
	BufDelayUnit_Reset(unit);

	unit->m_decaytime = ZIN0(3);
	unit->m_feedbk = sc_CalcFeedback(unit->m_delaytime, unit->m_decaytime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace {

/* helper classes for delay functionality */
template <bool Checked = false>
struct DelayN_helper
{
	static const bool checked = false;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, long mask)
	{
		long irdphase = iwrphase - idsamp;
		bufData[iwrphase & mask] = ZXP(in);
		ZXP(out) = bufData[irdphase & mask];
		iwrphase++;
	}

	/* the frac argument is unneeded. the compiler should make sure, that it won't be computed */
	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask)
	{
		perform(in, out, bufData, iwrphase, idsamp, mask);
	}
};

template <>
struct DelayN_helper<true>
{
	static const bool checked = true;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, long mask)
	{
		long irdphase = iwrphase - idsamp;

		bufData[iwrphase & mask] = ZXP(in);
		if (irdphase < 0)
			ZXP(out) = 0.f;
		else
			ZXP(out) = bufData[irdphase & mask];

		iwrphase++;
	}

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask)
	{
		perform(in, out, bufData, iwrphase, idsamp, mask);
	}
};

template <bool initializing>
static inline void DelayN_delay_loop(float * out, const float * in, long & iwrphase, float dsamp, long mask,
									 float * dlybuf, int inNumSamples, int idelaylen)
{
	long irdphase = iwrphase - (long)dsamp;
	float* dlybuf1 = dlybuf - ZOFF;
	float* dlyrd   = dlybuf1 + (irdphase & mask);
	float* dlywr   = dlybuf1 + (iwrphase & mask);
	float* dlyN    = dlybuf1 + idelaylen;
	long remain = inNumSamples;
	while (remain) {
		long rdspace = dlyN - dlyrd;
		long wrspace = dlyN - dlywr;
		if (initializing) {
			long nsmps = sc_min(rdspace, wrspace);
			nsmps = sc_min(remain, nsmps);
			remain -= nsmps;
			if (irdphase < 0) {
				if ((dlywr - dlyrd) > nsmps) {
#ifdef NOVA_SIMD
					if ((nsmps & (nova::vec<float>::size - 1)) == 0) {
						nova::copyvec_nn_simd(dlywr + ZOFF, in + ZOFF, nsmps);
						nova::zerovec_na_simd(out + ZOFF, nsmps);
					} else
#endif
					{
						ZCopy(nsmps, dlywr, in);
						ZClear(nsmps, out);
					}
					out += nsmps;
					in += nsmps;
					dlyrd += nsmps;
					dlywr += nsmps;
				} else {
					LOOP(nsmps,
						ZXP(dlywr) = ZXP(in);
						ZXP(out) = 0.f;
					);
					dlyrd += nsmps;
				}
			} else {
				LOOP(nsmps,
					ZXP(dlywr) = ZXP(in);
					ZXP(out) = ZXP(dlyrd);
				);
			}
			irdphase += nsmps;
			if (dlyrd == dlyN) dlyrd = dlybuf1;
			if (dlywr == dlyN) dlywr = dlybuf1;
		}
		else {
			long nsmps = sc_min(rdspace, wrspace);
			nsmps = sc_min(remain, nsmps);
			remain -= nsmps;

			if (std::abs((float)(dlyrd - dlywr)) > nsmps) {
#ifdef NOVA_SIMD
				if ((nsmps & 15) == 0) {
					nova::copyvec_nn_simd(dlywr + ZOFF, in + ZOFF, nsmps);
					nova::copyvec_nn_simd(out + ZOFF, dlyrd + ZOFF, nsmps);
				} else
#endif
				{
					ZCopy(nsmps, dlywr, in);
					ZCopy(nsmps, out, dlyrd);
				}
				out += nsmps;
				in += nsmps;
				dlyrd += nsmps;
				dlywr += nsmps;
			} else
				LOOP(nsmps,
					ZXP(dlywr) = ZXP(in);
					ZXP(out) = ZXP(dlyrd);
				);
			if (dlyrd == dlyN) dlyrd = dlybuf1;
			if (dlywr == dlyN) dlywr = dlybuf1;
		}
	}
	iwrphase += inNumSamples;
}


template <bool Checked = false>
struct DelayL_helper
{
	static const bool checked = false;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask)
	{
		bufData[iwrphase & mask] = ZXP(in);
		long irdphase = iwrphase - idsamp;
		long irdphaseb = irdphase - 1;
		float d1 = bufData[irdphase & mask];
		float d2 = bufData[irdphaseb & mask];
		ZXP(out) = lininterp(frac, d1, d2);
		iwrphase++;
	}
};

template <>
struct DelayL_helper<true>
{
	static const bool checked = true;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask)
	{
		bufData[iwrphase & mask] = ZXP(in);
		long irdphase = iwrphase - idsamp;
		long irdphaseb = irdphase - 1;

		if (irdphase < 0) {
			ZXP(out) = 0.f;
		} else if (irdphaseb < 0) {
			float d1 = bufData[irdphase & mask];
			ZXP(out) = d1 - frac * d1;
		} else {
			float d1 = bufData[irdphase & mask];
			float d2 = bufData[irdphaseb & mask];
			ZXP(out) = lininterp(frac, d1, d2);
		}
		iwrphase++;
	}
};

template <bool Checked = false>
struct DelayC_helper
{
	static const bool checked = false;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask)
	{
		bufData[iwrphase & mask] = ZXP(in);
		long irdphase1 = iwrphase - idsamp;
		long irdphase2 = irdphase1 - 1;
		long irdphase3 = irdphase1 - 2;
		long irdphase0 = irdphase1 + 1;
		float d0 = bufData[irdphase0 & mask];
		float d1 = bufData[irdphase1 & mask];
		float d2 = bufData[irdphase2 & mask];
		float d3 = bufData[irdphase3 & mask];
		ZXP(out) = cubicinterp(frac, d0, d1, d2, d3);
		iwrphase++;
	}
};

template <>
struct DelayC_helper<true>
{
	static const bool checked = true;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask)
	{
		long irdphase1 = iwrphase - idsamp;
		long irdphase2 = irdphase1 - 1;
		long irdphase3 = irdphase1 - 2;
		long irdphase0 = irdphase1 + 1;

		bufData[iwrphase & mask] = ZXP(in);
		if (irdphase0 < 0) {
			ZXP(out) = 0.f;
		} else {
			float d0, d1, d2, d3;
			if (irdphase1 < 0) {
				d1 = d2 = d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
			} else if (irdphase2 < 0) {
				d1 = d2 = d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
			} else if (irdphase3 < 0) {
				d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
				d2 = bufData[irdphase2 & mask];
			} else {
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
				d2 = bufData[irdphase2 & mask];
				d3 = bufData[irdphase3 & mask];
			}
			ZXP(out) = cubicinterp(frac, d0, d1, d2, d3);
		}
		iwrphase++;
	}
};

template <bool Checked = false>
struct CombN_helper
{
	static const bool checked = false;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, long mask, float feedbk)
	{
		long irdphase = iwrphase - idsamp;
		float value = bufData[irdphase & mask];
		bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
		ZXP(out) = value;
		++iwrphase;
	}

	/* the frac argument is unneeded. the compiler should make sure, that it won't be computed */
	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		perform(in, out, bufData, iwrphase, idsamp, mask, feedbk);
	}
};

template <>
struct CombN_helper<true>
{
	static const bool checked = true;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, long mask, float feedbk)
	{
		long irdphase = iwrphase - idsamp;

		if (irdphase < 0) {
			bufData[iwrphase & mask] = ZXP(in);
			ZXP(out) = 0.f;
		} else {
			float value = bufData[irdphase & mask];
			bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
			ZXP(out) = value;
		}

		iwrphase++;
	}

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		perform(in, out, bufData, iwrphase, idsamp, mask, feedbk);
	}
};

template <bool Checked = false>
struct CombL_helper
{
	static const bool checked = false;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		long irdphase = iwrphase - idsamp;
		long irdphaseb = irdphase - 1;
		float d1 = bufData[irdphase & mask];
		float d2 = bufData[irdphaseb & mask];
		float value = lininterp(frac, d1, d2);
		bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
		ZXP(out) = value;
		iwrphase++;
	}
};

template <>
struct CombL_helper<true>
{
	static const bool checked = true;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		long irdphase = iwrphase - idsamp;
		long irdphaseb = irdphase - 1;

		float zin = ZXP(in);
		if (irdphase < 0) {
			bufData[iwrphase & mask] = zin;
			ZXP(out) = 0.f;
		} else if (irdphaseb < 0) {
			float d1 = bufData[irdphase & mask];
			float value = d1 - frac * d1;
			bufData[iwrphase & mask] = zin + feedbk * value;
			ZXP(out) = value;
		} else {
			float d1 = bufData[irdphase & mask];
			float d2 = bufData[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			bufData[iwrphase & mask] = zin + feedbk * value;
			ZXP(out) = value;
		}
		iwrphase++;
	}
};

template <bool Checked = false>
struct CombC_helper
{
	static const bool checked = false;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		long irdphase1 = iwrphase - idsamp;
		long irdphase2 = irdphase1 - 1;
		long irdphase3 = irdphase1 - 2;
		long irdphase0 = irdphase1 + 1;
		float d0 = bufData[irdphase0 & mask];
		float d1 = bufData[irdphase1 & mask];
		float d2 = bufData[irdphase2 & mask];
		float d3 = bufData[irdphase3 & mask];
		float value = cubicinterp(frac, d0, d1, d2, d3);
		bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
		ZXP(out) = value;
		iwrphase++;
	}
};

template <>
struct CombC_helper<true>
{
	static const bool checked = true;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		long irdphase1 = iwrphase - idsamp;
		long irdphase2 = irdphase1 - 1;
		long irdphase3 = irdphase1 - 2;
		long irdphase0 = irdphase1 + 1;

		if (irdphase0 < 0) {
			bufData[iwrphase & mask] = ZXP(in);
			ZXP(out) = 0.f;
		} else {
			float d0, d1, d2, d3;
			if (irdphase1 < 0) {
				d1 = d2 = d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
			} else if (irdphase2 < 0) {
				d1 = d2 = d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
			} else if (irdphase3 < 0) {
				d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
				d2 = bufData[irdphase2 & mask];
			} else {
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
				d2 = bufData[irdphase2 & mask];
				d3 = bufData[irdphase3 & mask];
			}
			float value = cubicinterp(frac, d0, d1, d2, d3);
			bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
			ZXP(out) = value;
		}
		iwrphase++;
	}
};

template <bool Checked = false>
struct AllpassN_helper
{
	static const bool checked = false;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, long mask, float feedbk)
	{
		long irdphase = iwrphase - idsamp;
		float value = bufData[irdphase & mask];
		float dwr = value * feedbk + ZXP(in);
		bufData[iwrphase & mask] = dwr;
		ZXP(out) = value - feedbk * dwr;
		++iwrphase;
	}

	/* the frac argument is unneeded. the compiler should make sure, that it won't be computed */
	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		perform(in, out, bufData, iwrphase, idsamp, mask, feedbk);
	}
};

template <>
struct AllpassN_helper<true>
{
	static const bool checked = true;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, long mask, float feedbk)
	{
		long irdphase = iwrphase - idsamp;

		if (irdphase < 0) {
			float dwr = ZXP(in);
			bufData[iwrphase & mask] = dwr;
			ZXP(out) = -feedbk * dwr;
		} else {
			float value = bufData[irdphase & mask];
			float dwr = feedbk * value + ZXP(in);
			bufData[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;
		}
		++iwrphase;
	}

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		perform(in, out, bufData, iwrphase, idsamp, mask, feedbk);
	}
};

template <bool Checked = false>
struct AllpassL_helper
{
	static const bool checked = false;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		long irdphase = iwrphase - idsamp;
		long irdphaseb = irdphase - 1;
		float d1 = bufData[irdphase & mask];
		float d2 = bufData[irdphaseb & mask];
		float value = lininterp(frac, d1, d2);
		float dwr = ZXP(in) + feedbk * value;
		bufData[iwrphase & mask] = dwr;
		ZXP(out) = value - feedbk * dwr;
		iwrphase++;
	}
};

template <>
struct AllpassL_helper<true>
{
	static const bool checked = true;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		long irdphase = iwrphase - idsamp;
		long irdphaseb = irdphase - 1;

		float zin = ZXP(in);
		if (irdphase < 0) {
			bufData[iwrphase & mask] = zin;
			ZXP(out) = - feedbk * zin;
		} else if (irdphaseb < 0) {
			float d1 = bufData[irdphase & mask];
			float value = d1 - frac * d1;
			float dwr = zin + feedbk * value;
			bufData[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;
		} else {
			float d1 = bufData[irdphase & mask];
			float d2 = bufData[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			float dwr = zin + feedbk * value;
			bufData[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;
		}
		iwrphase++;
	}
};

template <bool Checked = false>
struct AllpassC_helper
{
	static const bool checked = false;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		long irdphase1 = iwrphase - idsamp;
		long irdphase2 = irdphase1 - 1;
		long irdphase3 = irdphase1 - 2;
		long irdphase0 = irdphase1 + 1;
		float d0 = bufData[irdphase0 & mask];
		float d1 = bufData[irdphase1 & mask];
		float d2 = bufData[irdphase2 & mask];
		float d3 = bufData[irdphase3 & mask];
		float value = cubicinterp(frac, d0, d1, d2, d3);
		float dwr = ZXP(in) + feedbk * value;
		bufData[iwrphase & mask] = dwr;
		ZXP(out) = value - feedbk * dwr;
		iwrphase++;
	}
};

template <>
struct AllpassC_helper<true>
{
	static const bool checked = true;

	static inline void perform(const float *& in, float *& out, float * bufData,
							   long & iwrphase, long idsamp, float frac, long mask, float feedbk)
	{
		long irdphase1 = iwrphase - idsamp;
		long irdphase2 = irdphase1 - 1;
		long irdphase3 = irdphase1 - 2;
		long irdphase0 = irdphase1 + 1;

		if (irdphase0 < 0) {
			bufData[iwrphase & mask] = ZXP(in);
			ZXP(out) = 0.f;
		} else {
			float d0, d1, d2, d3;
			if (irdphase1 < 0) {
				d1 = d2 = d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
			} else if (irdphase2 < 0) {
				d1 = d2 = d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
			} else if (irdphase3 < 0) {
				d3 = 0.f;
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
				d2 = bufData[irdphase2 & mask];
			} else {
				d0 = bufData[irdphase0 & mask];
				d1 = bufData[irdphase1 & mask];
				d2 = bufData[irdphase2 & mask];
				d3 = bufData[irdphase3 & mask];
			}
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float dwr = ZXP(in) + feedbk * value;
			bufData[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;
		}
		iwrphase++;
	}
};

}

////////////////////////////////////////////////////////////////////////////////////////////////////////

/* template function to generate buffer-based delay ugen function, control-rate delay time */
template <typename PerformClass,
		  typename BufDelayX
		 >
inline void BufDelayX_perform(BufDelayX *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(1);
	float delaytime = ZIN0(2);

	GET_BUF
	CHECK_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP1(inNumSamples,
			PerformClass::perform(in, out, bufData, iwrphase, idsamp, frac, mask);
		);
	} else {
		float next_dsamp = BufCalcDelay(unit, bufSamples, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			PerformClass::perform(in, out, bufData, iwrphase, idsamp, frac, mask);
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}

	unit->m_iwrphase = iwrphase;

	if (PerformClass::checked) {
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= bufSamples)
			unit->mCalcFunc = resetFunc;
	}
}


/* template function to generate buffer-based delay ugen function, audio-rate delay time */
template <typename PerformClass,
		  typename BufDelayX
		 >
inline void BufDelayX_perform_a(BufDelayX *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(1);
	float * delaytime = ZIN(2);

	GET_BUF
	CHECK_BUF
	long iwrphase = unit->m_iwrphase;

	LOOP1(inNumSamples,
		float dsamp = BufCalcDelay(unit, bufSamples, ZXP(delaytime));
		long idsamp = (long)dsamp;

		float frac = dsamp - idsamp;
		PerformClass::perform(in, out, bufData, iwrphase, idsamp, frac, mask);
	);

	unit->m_iwrphase = iwrphase;

	if (PerformClass::checked)
	{
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= bufSamples)
			unit->mCalcFunc = resetFunc;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufDelayN_Ctor(BufDelayN *unit)
{
	if(INRATE(2) == calc_FullRate)
		SETCALC(BufDelayN_next_a_z);
	else
		SETCALC(BufDelayN_next_z);
	BufDelayUnit_Reset(unit);
	ZOUT0(0) = 0.f;
}

void BufDelayN_next(BufDelayN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(1);
	float delaytime = ZIN0(2);

	GET_BUF
	CHECK_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;

	if (delaytime == unit->m_delaytime) {
		DelayN_delay_loop<false>(out, in, iwrphase, dsamp, mask, bufData, inNumSamples, PREVIOUSPOWEROFTWO(bufSamples));
	} else {
		float next_dsamp = BufCalcDelay(unit, bufSamples, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			DelayN_helper<false>::perform(in, out, bufData, iwrphase, idsamp, mask);
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}

	unit->m_iwrphase = iwrphase;
}


void BufDelayN_next_z(BufDelayN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(1);
	float delaytime = ZIN0(2);

	GET_BUF
	CHECK_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;

	if (delaytime == unit->m_delaytime) {
		DelayN_delay_loop<true>(out, in, iwrphase, dsamp, mask, bufData, inNumSamples, PREVIOUSPOWEROFTWO(bufSamples));
	} else {
		float next_dsamp = BufCalcDelay(unit, bufSamples, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			DelayN_helper<true>::perform(in, out, bufData, iwrphase, idsamp, mask);
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}

	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples)
		SETCALC(BufDelayN_next);
}

template <bool checked>
inline void BufDelayN_perform_a(BufDelayN *unit, int inNumSamples)
{
	BufDelayX_perform_a<DelayN_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufDelayN_next_a);
}

void BufDelayN_next_a(BufDelayN *unit, int inNumSamples)
{
	BufDelayN_perform_a<false>(unit, inNumSamples);
}

void BufDelayN_next_a_z(BufDelayN *unit, int inNumSamples)
{
	BufDelayN_perform_a<true>(unit, inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufDelayL_Ctor(BufDelayL *unit)
{
	BufDelayUnit_Reset(unit);
	if(INRATE(2) == calc_FullRate)
		SETCALC(BufDelayL_next_a_z);
	else
		SETCALC(BufDelayL_next_z);
	ZOUT0(0) = 0.f;
}


template <bool checked>
inline void BufDelayL_perform(BufDelayL *unit, int inNumSamples)
{
	BufDelayX_perform<DelayL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufDelayL_next);
}

void BufDelayL_next(BufDelayL *unit, int inNumSamples)
{
	BufDelayL_perform<false>(unit, inNumSamples);
}

void BufDelayL_next_z(BufDelayL *unit, int inNumSamples)
{
	BufDelayL_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void BufDelayL_perform_a(BufDelayL *unit, int inNumSamples)
{
	BufDelayX_perform_a<DelayL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufDelayL_next_a);
}

void BufDelayL_next_a(BufDelayL *unit, int inNumSamples)
{
	BufDelayL_perform_a<false>(unit, inNumSamples);
}

void BufDelayL_next_a_z(BufDelayL *unit, int inNumSamples)
{
	BufDelayL_perform_a<true>(unit, inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufDelayC_Ctor(BufDelayC *unit)
{
	BufDelayUnit_Reset(unit);
	if(INRATE(2) == calc_FullRate)
		SETCALC(BufDelayC_next_a_z);
	else
		SETCALC(BufDelayC_next_z);
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void BufDelayC_perform(BufDelayC *unit, int inNumSamples)
{
	BufDelayX_perform<DelayC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufDelayC_next);
}

void BufDelayC_next(BufDelayC *unit, int inNumSamples)
{
	BufDelayC_perform<false>(unit, inNumSamples);
}

void BufDelayC_next_z(BufDelayC *unit, int inNumSamples)
{
	BufDelayC_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void BufDelayC_perform_a(BufDelayC *unit, int inNumSamples)
{
	BufDelayX_perform_a<DelayC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufDelayL_next_a);
}

void BufDelayC_next_a(BufDelayC *unit, int inNumSamples)
{
	BufDelayC_perform_a<false>(unit, inNumSamples);
}

void BufDelayC_next_a_z(BufDelayC *unit, int inNumSamples)
{
	BufDelayC_perform_a<true>(unit, inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename PerformClass,
		  typename BufCombX
		 >
inline void BufFilterX_perform(BufCombX *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);

	GET_BUF
	CHECK_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP1(inNumSamples,
			PerformClass::perform(in, out, bufData, iwrphase, idsamp, frac, mask, feedbk);
		);
	} else {
		float next_dsamp = BufCalcDelay(unit, bufSamples, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			PerformClass::perform(in, out, bufData, iwrphase, idsamp, frac, mask, feedbk);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;

	if (PerformClass::checked) {
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= bufSamples)
			unit->mCalcFunc = resetFunc;
	}
}

template <typename PerformClass,
		  typename BufCombX
		 >
inline void BufFilterX_perform_a(BufCombX *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(1);
	float * delaytime = ZIN(2);
	float decaytime = ZIN0(3);

	GET_BUF
	CHECK_BUF
	long iwrphase = unit->m_iwrphase;

	LOOP1(inNumSamples,
		float del = ZXP(delaytime);
		float dsamp = BufCalcDelay(unit, bufSamples, del);
		float feedbk = sc_CalcFeedback(del, decaytime);

		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		PerformClass::perform(in, out, bufData, iwrphase, idsamp, frac, mask, feedbk);
	);

	unit->m_iwrphase = iwrphase;

	if (PerformClass::checked)
	{
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= bufSamples)
			unit->mCalcFunc = resetFunc;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufCombN_Ctor(BufCombN *unit)
{
	BufFeedbackDelay_Reset(unit);
	if(INRATE(2) == calc_FullRate)
		SETCALC(BufCombN_next_a_z);
	else
		SETCALC(BufCombN_next_z);
	ZOUT0(0) = 0.f;
}

void BufCombN_next(BufCombN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);

	GET_BUF
	CHECK_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	//postbuf("BufCombN_next %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = bufData - ZOFF;
		float* dlyrd   = dlybuf1 + (irdphase & mask);
		float* dlywr   = dlybuf1 + (iwrphase & mask);
		float* dlyN    = dlybuf1 + PREVIOUSPOWEROFTWO(bufSamples);
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;
				LOOP1(nsmps,
					float value = ZXP(dlyrd);
					ZXP(dlywr) = value * feedbk + ZXP(in);
					ZXP(out) = value;
				);
				if (dlyrd == dlyN) dlyrd = dlybuf1;
				if (dlywr == dlyN) dlywr = dlybuf1;
			}
		} else {
			float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;

				LOOP1(nsmps,
					float value = ZXP(dlyrd);
					ZXP(dlywr) = value * feedbk + ZXP(in);
					ZXP(out) = value;
					feedbk += feedbk_slope;
				);
				if (dlyrd == dlyN) dlyrd = dlybuf1;
				if (dlywr == dlyN) dlywr = dlybuf1;
			}
			unit->m_feedbk = feedbk;
			unit->m_decaytime = decaytime;
		}
		iwrphase += inNumSamples;
	} else {
		float next_dsamp = BufCalcDelay(unit, bufSamples, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			CombN_helper<false>::perform(in, out, bufData, iwrphase, (long)dsamp, mask, feedbk);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;
}

void BufCombN_next_z(BufCombN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);

	GET_BUF
	CHECK_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	//Print("BufCombN_next_z %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = bufData - ZOFF;
		float* dlyN    = dlybuf1 + PREVIOUSPOWEROFTWO(bufSamples);
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				float* dlywr = dlybuf1 + (iwrphase & mask);
				float* dlyrd = dlybuf1 + (irdphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;
				if (irdphase < 0) {
					LOOP1(nsmps,
						ZXP(dlywr) = ZXP(in);
						ZXP(out) = 0.f;
					);
				} else {
					LOOP1(nsmps,
						float value = ZXP(dlyrd);
						ZXP(dlywr) = value * feedbk + ZXP(in);
						ZXP(out) = value;
					);
				}
				iwrphase += nsmps;
				irdphase += nsmps;
			}
		} else {
			float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				float* dlyrd = dlybuf1 + (irdphase & mask);
				float* dlywr = dlybuf1 + (iwrphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;

				if (irdphase < 0) {
					feedbk += nsmps * feedbk_slope;
					dlyrd += nsmps;
					LOOP1(nsmps,
						ZXP(dlywr) = ZXP(in);
						ZXP(out) = 0.f;
					);
				} else {
					LOOP1(nsmps,
						float value = ZXP(dlyrd);
						ZXP(dlywr) = value * feedbk + ZXP(in);
						ZXP(out) = value;
						feedbk += feedbk_slope;
					);
				}
				iwrphase += nsmps;
				irdphase += nsmps;
			}
			unit->m_feedbk = feedbk;
			unit->m_decaytime = decaytime;
		}
	} else {
		float next_dsamp = BufCalcDelay(unit, bufSamples, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			CombN_helper<true>::perform(in, out, bufData, iwrphase, (long)dsamp, mask, feedbk);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples)
		SETCALC(BufCombN_next);
}

template <bool checked>
inline void BufCombN_perform_a(BufCombN *unit, int inNumSamples)
{
	BufFilterX_perform_a<CombN_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufCombN_next_a);
}

void BufCombN_next_a(BufCombN *unit, int inNumSamples)
{
	BufCombN_perform_a<false>(unit, inNumSamples);
}

void BufCombN_next_a_z(BufCombN *unit, int inNumSamples)
{
	BufCombN_perform_a<true>(unit, inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufCombL_Ctor(BufCombL *unit)
{
	BufFeedbackDelay_Reset(unit);
	if(INRATE(2) == calc_FullRate)
		SETCALC(BufCombL_next_a_z);
	else
		SETCALC(BufCombL_next_z);
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void BufCombL_perform(BufCombL *unit, int inNumSamples)
{
	BufFilterX_perform<CombL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufCombL_next);
}

void BufCombL_next(BufCombL *unit, int inNumSamples)
{
	BufCombL_perform<false>(unit, inNumSamples);
}

void BufCombL_next_z(BufCombL *unit, int inNumSamples)
{
	BufCombL_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void BufCombL_perform_a(BufCombL *unit, int inNumSamples)
{
	BufFilterX_perform_a<CombL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufCombL_next_a);
}

void BufCombL_next_a(BufCombL *unit, int inNumSamples)
{
	BufCombL_perform_a<false>(unit, inNumSamples);
}

void BufCombL_next_a_z(BufCombL *unit, int inNumSamples)
{
	BufCombL_perform_a<true>(unit, inNumSamples);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufCombC_Ctor(BufCombC *unit)
{
	BufFeedbackDelay_Reset(unit);
	if(INRATE(2) == calc_FullRate)
		SETCALC(BufCombC_next_a_z);
	else
		SETCALC(BufCombC_next_z);
	ZOUT0(0) = 0.f;
}


template <bool checked>
inline void BufCombC_perform(BufCombC *unit, int inNumSamples)
{
	BufFilterX_perform<CombC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufCombC_next);
}

void BufCombC_next(BufCombC *unit, int inNumSamples)
{
	BufCombC_perform<false>(unit, inNumSamples);
}

void BufCombC_next_z(BufCombC *unit, int inNumSamples)
{
	BufCombC_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void BufCombC_perform_a(BufCombC *unit, int inNumSamples)
{
	BufFilterX_perform_a<CombC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufCombC_next_a);
}

void BufCombC_next_a(BufCombC *unit, int inNumSamples)
{
	BufCombC_perform_a<false>(unit, inNumSamples);
}

void BufCombC_next_a_z(BufCombC *unit, int inNumSamples)
{
	BufCombC_perform_a<true>(unit, inNumSamples);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufAllpassN_Ctor(BufAllpassN *unit)
{
	BufFeedbackDelay_Reset(unit);
	if(INRATE(2) == calc_FullRate)
		SETCALC(BufAllpassN_next_a_z);
	else
		SETCALC(BufAllpassN_next_z);
	ZOUT0(0) = 0.f;
}

void BufAllpassN_next(BufAllpassN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);

	GET_BUF
	CHECK_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	//postbuf("BufAllpassN_next %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = bufData - ZOFF;
		float* dlyrd   = dlybuf1 + (irdphase & mask);
		float* dlywr   = dlybuf1 + (iwrphase & mask);
		float* dlyN    = dlybuf1 + PREVIOUSPOWEROFTWO(bufSamples);
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;
				LOOP1(nsmps,
					float value = ZXP(dlyrd);
					float dwr = value * feedbk + ZXP(in);
					ZXP(dlywr) = dwr;
					ZXP(out) = value - feedbk * dwr;
				);
				if (dlyrd == dlyN) dlyrd = dlybuf1;
				if (dlywr == dlyN) dlywr = dlybuf1;
			}
		} else {
			float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;

				LOOP1(nsmps,
					float value = ZXP(dlyrd);
					float dwr = value * feedbk + ZXP(in);
					ZXP(dlywr) = dwr;
					ZXP(out) = value - feedbk * dwr;
					feedbk += feedbk_slope;
				);
				if (dlyrd == dlyN) dlyrd = dlybuf1;
				if (dlywr == dlyN) dlywr = dlybuf1;
			}
			unit->m_feedbk = feedbk;
			unit->m_decaytime = decaytime;
		}
		iwrphase += inNumSamples;
	} else {
		float next_dsamp = BufCalcDelay(unit, bufSamples, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			AllpassN_helper<false>::perform(in, out, bufData, iwrphase, (long)dsamp, mask, feedbk);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;
}


void BufAllpassN_next_z(BufAllpassN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);

	GET_BUF
	CHECK_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	//postbuf("BufAllpassN_next_z %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = bufData - ZOFF;
		float* dlyN    = dlybuf1 + PREVIOUSPOWEROFTWO(bufSamples);
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				float* dlywr = dlybuf1 + (iwrphase & mask);
				float* dlyrd = dlybuf1 + (irdphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;
				if (irdphase < 0) {
					feedbk = -feedbk;
					LOOP1(nsmps,
						float dwr = ZXP(in);
						ZXP(dlywr) = dwr;
						ZXP(out) = feedbk * dwr;
					);
					feedbk = -feedbk;
				} else {
					LOOP1(nsmps,
						float x1 = ZXP(dlyrd);
						float dwr = x1 * feedbk + ZXP(in);
						ZXP(dlywr) = dwr;
						ZXP(out) = x1 - feedbk * dwr;
					);
				}
				iwrphase += nsmps;
				irdphase += nsmps;
			}
		} else {
			float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				float* dlyrd = dlybuf1 + (irdphase & mask);
				float* dlywr = dlybuf1 + (iwrphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;

				if (irdphase < 0) {
					dlyrd += nsmps;
					LOOP1(nsmps,
						float dwr = ZXP(in);
						ZXP(dlywr) = dwr;
						ZXP(out) = -feedbk * dwr;
						feedbk += feedbk_slope;
					);
				} else {
					LOOP1(nsmps,
						float x1 = ZXP(dlyrd);
						float dwr = x1 * feedbk + ZXP(in);
						ZXP(dlywr) = dwr;
						ZXP(out) = x1 - feedbk * dwr;
						feedbk += feedbk_slope;
					);
				}
				iwrphase += nsmps;
				irdphase += nsmps;
			}
			unit->m_feedbk = feedbk;
			unit->m_decaytime = decaytime;
		}
	} else {
		float next_dsamp = BufCalcDelay(unit, bufSamples, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			AllpassN_helper<true>::perform(in, out, bufData, iwrphase, (long)dsamp, mask, feedbk);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples)
		SETCALC(BufAllpassN_next);
}

template <bool checked>
inline void BufAllpassN_perform_a(BufAllpassN *unit, int inNumSamples)
{
	BufFilterX_perform_a<AllpassN_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufAllpassN_next_a);
}

void BufAllpassN_next_a(BufAllpassN *unit, int inNumSamples)
{
	BufAllpassN_perform_a<false>(unit, inNumSamples);
}

void BufAllpassN_next_a_z(BufAllpassN *unit, int inNumSamples)
{
	BufAllpassN_perform_a<true>(unit, inNumSamples);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufAllpassL_Ctor(BufAllpassL *unit)
{
	BufFeedbackDelay_Reset(unit);
	if(INRATE(2) == calc_FullRate)
		SETCALC(BufAllpassL_next_a_z);
	else
		SETCALC(BufAllpassL_next_z);
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void BufAllpassL_perform(BufAllpassL *unit, int inNumSamples)
{
	BufFilterX_perform<AllpassL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufAllpassL_next);
}

void BufAllpassL_next(BufAllpassL *unit, int inNumSamples)
{
	BufAllpassL_perform<false>(unit, inNumSamples);
}

void BufAllpassL_next_z(BufAllpassL *unit, int inNumSamples)
{
	BufAllpassL_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void BufAllpassL_perform_a(BufAllpassL *unit, int inNumSamples)
{
	BufFilterX_perform_a<AllpassL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufAllpassL_next_a);
}

void BufAllpassL_next_a(BufAllpassL *unit, int inNumSamples)
{
	BufAllpassL_perform_a<false>(unit, inNumSamples);
}

void BufAllpassL_next_a_z(BufAllpassL *unit, int inNumSamples)
{
	BufAllpassL_perform_a<true>(unit, inNumSamples);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufAllpassC_Ctor(BufAllpassC *unit)
{
	BufFeedbackDelay_Reset(unit);
	if(INRATE(2) == calc_FullRate)
		SETCALC(BufAllpassC_next_a_z);
	else
		SETCALC(BufAllpassC_next_z);
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void BufAllpassC_perform(BufAllpassC *unit, int inNumSamples)
{
	BufFilterX_perform<AllpassC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufAllpassC_next);
}

void BufAllpassC_next(BufAllpassC *unit, int inNumSamples)
{
	BufAllpassC_perform<false>(unit, inNumSamples);
}

void BufAllpassC_next_z(BufAllpassC *unit, int inNumSamples)
{
	BufAllpassC_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void BufAllpassC_perform_a(BufAllpassC *unit, int inNumSamples)
{
	BufFilterX_perform_a<AllpassC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)BufAllpassC_next_a);
}

void BufAllpassC_next_a(BufAllpassC *unit, int inNumSamples)
{
	BufAllpassC_perform_a<false>(unit, inNumSamples);
}

void BufAllpassC_next_a_z(BufAllpassC *unit, int inNumSamples)
{
	BufAllpassC_perform_a<true>(unit, inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

static bool DelayUnit_AllocDelayLine(DelayUnit *unit, const char * className)
{
	long delaybufsize = (long)ceil(unit->m_maxdelaytime * SAMPLERATE + 1.f);
	delaybufsize = delaybufsize + BUFLENGTH;
	delaybufsize = NEXTPOWEROFTWO(delaybufsize);  // round up to next power of two
	unit->m_fdelaylen = unit->m_idelaylen = delaybufsize;

	if (unit->m_dlybuf)
		RTFree(unit->mWorld, unit->m_dlybuf);
	unit->m_dlybuf = (float*)RTAlloc(unit->mWorld, delaybufsize * sizeof(float));

#if 0 // for debugging we may want to fill the buffer with nans
	std::fill_n(unit->m_dlybuf, delaybufsize, std::numeric_limits<float>::signaling_NaN());
#endif

	if (unit->m_dlybuf == NULL) {
		SETCALC(ft->fClearUnitOutputs);
		ClearUnitOutputs(unit, 1);

		if(unit->mWorld->mVerbosity > -2)
			Print("Failed to allocate memory for %s ugen.\n", className);
	}

	unit->m_mask = delaybufsize - 1;
	return (unit->m_dlybuf != NULL);
}

template <typename Unit>
static float CalcDelay(Unit *unit, float delaytime)
{
	float minDelay = Unit::minDelaySamples;
	float next_dsamp = delaytime * (float)SAMPLERATE;
	return sc_clip(next_dsamp, minDelay, unit->m_fdelaylen);
}

template <typename Unit>
static bool DelayUnit_Reset(Unit *unit, const char * className)
{
	unit->m_maxdelaytime = ZIN0(1);
	unit->m_delaytime = ZIN0(2);
	unit->m_dlybuf = 0;

	if (!DelayUnit_AllocDelayLine(unit, className))
		return false;

	unit->m_dsamp = CalcDelay(unit, unit->m_delaytime);

	unit->m_numoutput = 0;
	unit->m_iwrphase = 0;
	return true;
}


void DelayUnit_Dtor(DelayUnit *unit)
{
	RTFree(unit->mWorld, unit->m_dlybuf);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename Unit>
static bool FeedbackDelay_Reset(Unit *unit, const char * className)
{
	unit->m_decaytime = ZIN0(3);

	bool allocationSucessful = DelayUnit_Reset(unit, className);
	if (!allocationSucessful)
		return false;

	unit->m_feedbk = sc_CalcFeedback(unit->m_delaytime, unit->m_decaytime);
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

/* template function to generate delay ugen function, control-rate delay time */
template <typename PerformClass,
		  typename DelayX
		 >
inline void DelayX_perform(DelayX *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float delaytime = ZIN0(2);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP1(inNumSamples,
			PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask);
		);
	} else {
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask);
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}

	unit->m_iwrphase = iwrphase;

	if (PerformClass::checked) {
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= unit->m_idelaylen)
			unit->mCalcFunc = resetFunc;
	}
}

/* template function to generate delay ugen function, audio-rate delay time */
template <typename PerformClass,
		  typename DelayX
		 >
inline void DelayX_perform_a(DelayX *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float * delaytime = ZIN(2);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;

	LOOP1(inNumSamples,
		float dsamp = CalcDelay(unit, ZXP(delaytime));
		long idsamp = (long)dsamp;

		float frac = dsamp - idsamp;
		PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask);
	);

	unit->m_iwrphase = iwrphase;

	if (PerformClass::checked)
	{
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= unit->m_idelaylen)
			unit->mCalcFunc = resetFunc;
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void Delay_next_0(DelayUnit *unit, int inNumSamples)
{
	float *out = OUT(0);
	const float *in = IN(0);

	memcpy(out, in, inNumSamples * sizeof(float));
}

void Delay_next_0_nop(DelayUnit *unit, int inNumSamples)
{}

#ifdef NOVA_SIMD
void Delay_next_0_nova(DelayUnit *unit, int inNumSamples)
{
	nova::copyvec_simd(OUT(0), IN(0), inNumSamples);
}
#endif

static bool DelayUnit_init_0(DelayUnit *unit)
{
	if (INRATE(2) == calc_ScalarRate && ZIN0(2) == 0) {
		if (ZIN(0) == ZOUT(0))
			SETCALC(Delay_next_0_nop);
#ifdef NOVA_SIMD
		else if (boost::alignment::is_aligned( BUFLENGTH, 16 ))
			SETCALC(Delay_next_0_nova);
#endif
		else
			SETCALC(Delay_next_0);

		ZOUT0(0) = ZIN0(0);
		return true;
	} else
		return false;
}

enum {
	initializationComplete,
	initializationIncomplete
};

template <typename Delay>
static int Delay_Ctor(Delay *unit, const char *className)
{
	bool allocationSucessful = DelayUnit_Reset(unit, className);
	if (!allocationSucessful)
		return initializationComplete;

	// optimize for a constant delay of zero
	if (DelayUnit_init_0(unit))
		return initializationComplete;
	return initializationIncomplete;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void DelayN_Ctor(DelayN *unit)
{
	if (Delay_Ctor(unit, "DelayN") == initializationComplete)
		return;

	if (INRATE(2) == calc_FullRate)
		SETCALC(DelayN_next_a_z);
	else
		SETCALC(DelayN_next_z);
	ZOUT0(0) = 0.f;
}

void DelayN_next(DelayN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float delaytime = ZIN0(2);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;

	//Print("DelayN_next %p %g %g  %d %d\n", unit, delaytime, dsamp, mask, iwrphase);
	if (delaytime == unit->m_delaytime) {
		DelayN_delay_loop<false>(out, in, iwrphase, dsamp, mask, dlybuf, inNumSamples, unit->m_idelaylen);
	} else {
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			DelayN_helper<false>::perform(in, out, dlybuf, iwrphase, (long)dsamp, mask);
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}

	unit->m_iwrphase = iwrphase;
}


void DelayN_next_z(DelayN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float delaytime = ZIN0(2);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;

	if (delaytime == unit->m_delaytime) {
		DelayN_delay_loop<true>(out, in, iwrphase, dsamp, mask, dlybuf, inNumSamples, unit->m_idelaylen);
	} else {
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			DelayN_helper<true>::perform(in, out, dlybuf, iwrphase, (long)dsamp, mask);
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}

	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen)
		SETCALC(DelayN_next);
}

template <bool checked>
inline void DelayN_perform_a(DelayN *unit, int inNumSamples)
{
	DelayX_perform_a<DelayN_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)DelayN_next_a);
}

void DelayN_next_a(DelayN *unit, int inNumSamples)
{
	DelayN_perform_a<false>(unit, inNumSamples);
}

void DelayN_next_a_z(DelayN *unit, int inNumSamples)
{
	DelayN_perform_a<true>(unit, inNumSamples);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void DelayL_Ctor(DelayL *unit)
{
	if (Delay_Ctor(unit, "DelayL") == initializationComplete)
		return;

	if (INRATE(2) == calc_FullRate)
		SETCALC(DelayL_next_a_z);
	else
		SETCALC(DelayL_next_z);
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void DelayL_perform(DelayL *unit, int inNumSamples)
{
	DelayX_perform<DelayL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)DelayL_next);
}

void DelayL_next(DelayL *unit, int inNumSamples)
{
	DelayL_perform<false>(unit, inNumSamples);
}

void DelayL_next_z(DelayL *unit, int inNumSamples)
{
	DelayL_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void DelayL_perform_a(DelayL *unit, int inNumSamples)
{
	DelayX_perform_a<DelayL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)DelayL_next_a);
}

void DelayL_next_a(DelayL *unit, int inNumSamples)
{
	DelayL_perform_a<false>(unit, inNumSamples);
}

void DelayL_next_a_z(DelayL *unit, int inNumSamples)
{
	DelayL_perform_a<true>(unit, inNumSamples);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void DelayC_Ctor(DelayC *unit)
{
	if (Delay_Ctor(unit, "DelayC") == initializationComplete)
		return;

	if (INRATE(2) == calc_FullRate)
		SETCALC(DelayC_next_a_z);
	else
		SETCALC(DelayC_next_z);
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void DelayC_perform(DelayC *unit, int inNumSamples)
{
	DelayX_perform<DelayC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)DelayC_next);
}

void DelayC_next(DelayC *unit, int inNumSamples)
{
	DelayC_perform<false>(unit, inNumSamples);
}

void DelayC_next_z(DelayC *unit, int inNumSamples)
{
	DelayC_perform<true>(unit, inNumSamples);
}


template <bool checked>
inline void DelayC_perform_a(DelayC *unit, int inNumSamples)
{
	DelayX_perform_a<DelayC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)DelayC_next_a);
}

void DelayC_next_a(DelayC *unit, int inNumSamples)
{
	DelayC_perform_a<false>(unit, inNumSamples);
}

void DelayC_next_a_z(DelayC *unit, int inNumSamples)
{
	DelayC_perform_a<true>(unit, inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename PerformClass,
		  typename BufCombX
		 >
inline void FilterX_perform(BufCombX *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP1(inNumSamples,
			PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask, feedbk);
		);
	} else {
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask, feedbk);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;

	if (PerformClass::checked) {
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= unit->m_idelaylen)
			unit->mCalcFunc = resetFunc;
	}
}

template <typename PerformClass,
		  typename CombX
		 >
inline void FilterX_perform_a(CombX *unit, int inNumSamples, UnitCalcFunc resetFunc)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float * delaytime = ZIN(2);
	float decaytime = ZIN0(3);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	long mask = unit->m_mask;

	LOOP1(inNumSamples,
		float del = ZXP(delaytime);
		float dsamp = CalcDelay(unit, del);
		float feedbk = sc_CalcFeedback(del, decaytime);

		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		PerformClass::perform(in, out, dlybuf, iwrphase, idsamp, frac, mask, feedbk);
	);

	unit->m_iwrphase = iwrphase;

	if (PerformClass::checked)
	{
		unit->m_numoutput += inNumSamples;
		if (unit->m_numoutput >= unit->m_idelaylen)
			unit->mCalcFunc = resetFunc;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void CombN_Ctor(CombN *unit)
{
	bool allocationSucessful = FeedbackDelay_Reset(unit, "CombN");
	if (!allocationSucessful)
		return;

	if(INRATE(2) == calc_FullRate)
		SETCALC(CombN_next_a_z);
	else
		SETCALC(CombN_next_z);
	ZOUT0(0) = 0.f;
}

void CombN_next(CombN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;

	//postbuf("CombN_next %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = dlybuf - ZOFF;
		float* dlyrd   = dlybuf1 + (irdphase & mask);
		float* dlywr   = dlybuf1 + (iwrphase & mask);
		float* dlyN    = dlybuf1 + unit->m_idelaylen;
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;
				LOOP(nsmps,
					float value = ZXP(dlyrd);
					ZXP(dlywr) = value * feedbk + ZXP(in);
					ZXP(out) = value;
				);
				if (dlyrd == dlyN) dlyrd = dlybuf1;
				if (dlywr == dlyN) dlywr = dlybuf1;
			}
		} else {
			float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;

				LOOP(nsmps,
					float value = ZXP(dlyrd);
					ZXP(dlywr) = value * feedbk + ZXP(in);
					ZXP(out) = value;
					feedbk += feedbk_slope;
				);
				if (dlyrd == dlyN) dlyrd = dlybuf1;
				if (dlywr == dlyN) dlywr = dlybuf1;
			}
			unit->m_feedbk = feedbk;
			unit->m_decaytime = decaytime;
		}
		iwrphase += inNumSamples;
	} else {
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			CombN_helper<false>::perform(in, out, dlybuf, iwrphase, (long)dsamp, mask, feedbk);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;
}


void CombN_next_z(CombN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;

	//postbuf("CombN_next_z %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = dlybuf - ZOFF;
		float* dlyN    = dlybuf1 + unit->m_idelaylen;
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				float* dlywr = dlybuf1 + (iwrphase & mask);
				float* dlyrd = dlybuf1 + (irdphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;
				if (irdphase < 0) {
					LOOP(nsmps,
						ZXP(dlywr) = ZXP(in);
						ZXP(out) = 0.f;
					);
				} else {
					LOOP(nsmps,
						float value = ZXP(dlyrd);
						ZXP(dlywr) = value * feedbk + ZXP(in);
						ZXP(out) = value;
					);
				}
				iwrphase += nsmps;
				irdphase += nsmps;
			}
		} else {
			float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				float* dlyrd = dlybuf1 + (irdphase & mask);
				float* dlywr = dlybuf1 + (iwrphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;

				if (irdphase < 0) {
					feedbk += nsmps * feedbk_slope;
					dlyrd += nsmps;
					LOOP(nsmps,
						ZXP(dlywr) = ZXP(in);
						ZXP(out) = 0.f;
					);
				} else {
					LOOP(nsmps,
						float value = ZXP(dlyrd);
						ZXP(dlywr) = value * feedbk + ZXP(in);
						ZXP(out) = value;
						feedbk += feedbk_slope;
					);
				}
				iwrphase += nsmps;
				irdphase += nsmps;
			}
			unit->m_feedbk = feedbk;
			unit->m_decaytime = decaytime;
		}
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			CombN_helper<true>::perform(in, out, dlybuf, iwrphase, (long)dsamp, mask, feedbk);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen)
		SETCALC(CombN_next);
}

template <bool checked>
inline void CombN_perform_a(CombN *unit, int inNumSamples)
{
	FilterX_perform_a<CombN_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)CombN_next_a);
}

void CombN_next_a(CombN *unit, int inNumSamples)
{
	CombN_perform_a<false>(unit, inNumSamples);
}

void CombN_next_a_z(CombN *unit, int inNumSamples)
{
	CombN_perform_a<true>(unit, inNumSamples);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void CombL_Ctor(CombL *unit)
{
	bool allocationSucessful = FeedbackDelay_Reset(unit, "CombL");
	if (!allocationSucessful)
		return;

	if(INRATE(2) == calc_FullRate)
		SETCALC(CombL_next_a_z);
	else
		SETCALC(CombL_next_z);
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void CombL_perform(CombL *unit, int inNumSamples)
{
	FilterX_perform<CombL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)CombL_next);
}

void CombL_next(CombL *unit, int inNumSamples)
{
	CombL_perform<false>(unit, inNumSamples);
}

void CombL_next_z(CombL *unit, int inNumSamples)
{
	CombL_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void CombL_perform_a(CombL *unit, int inNumSamples)
{
	FilterX_perform_a<CombL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)CombL_next_a);
}

void CombL_next_a(CombL *unit, int inNumSamples)
{
	CombL_perform_a<false>(unit, inNumSamples);
}

void CombL_next_a_z(CombL *unit, int inNumSamples)
{
	CombL_perform_a<true>(unit, inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void CombC_Ctor(CombC *unit)
{
	bool allocationSucessful = FeedbackDelay_Reset(unit, "CombC");
	if (!allocationSucessful)
		return;

	if(INRATE(2) == calc_FullRate)
		SETCALC(CombC_next_a_z);
	else
		SETCALC(CombC_next_z);
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void CombC_perform(CombC *unit, int inNumSamples)
{
	FilterX_perform<CombC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)CombC_next);
}

void CombC_next(CombC *unit, int inNumSamples)
{
	CombC_perform<false>(unit, inNumSamples);
}

void CombC_next_z(CombC *unit, int inNumSamples)
{
	CombC_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void CombC_perform_a(CombC *unit, int inNumSamples)
{
	FilterX_perform_a<CombC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)CombC_next_a);
}

void CombC_next_a(CombC *unit, int inNumSamples)
{
	CombC_perform_a<false>(unit, inNumSamples);
}

void CombC_next_a_z(CombC *unit, int inNumSamples)
{
	CombC_perform_a<true>(unit, inNumSamples);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void AllpassN_Ctor(AllpassN *unit)
{
	bool allocationSucessful = FeedbackDelay_Reset(unit, "AllpassN");
	if (!allocationSucessful)
		return;

	if(INRATE(2) == calc_FullRate)
		SETCALC(AllpassN_next_a_z);
	else
		SETCALC(AllpassN_next_z);
	ZOUT0(0) = 0.f;
}

void AllpassN_next(AllpassN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;

	//postbuf("AllpassN_next %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = dlybuf - ZOFF;
		float* dlyrd   = dlybuf1 + (irdphase & mask);
		float* dlywr   = dlybuf1 + (iwrphase & mask);
		float* dlyN    = dlybuf1 + unit->m_idelaylen;
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;
				LOOP(nsmps,
					float value = ZXP(dlyrd);
					float dwr = value * feedbk + ZXP(in);
					ZXP(dlywr) = dwr;
					ZXP(out) = value - feedbk * dwr;
				);
				if (dlyrd == dlyN) dlyrd = dlybuf1;
				if (dlywr == dlyN) dlywr = dlybuf1;
			}
		} else {
			float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;

				LOOP(nsmps,
					float value = ZXP(dlyrd);
					float dwr = value * feedbk + ZXP(in);
					ZXP(dlywr) = dwr;
					ZXP(out) = value - feedbk * dwr;
					feedbk += feedbk_slope;
				);
				if (dlyrd == dlyN) dlyrd = dlybuf1;
				if (dlywr == dlyN) dlywr = dlybuf1;
			}
			unit->m_feedbk = feedbk;
			unit->m_decaytime = decaytime;
		}
		iwrphase += inNumSamples;
	} else {
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			AllpassN_helper<false>::perform(in, out, dlybuf, iwrphase, (long)dsamp, mask, feedbk);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;
}


void AllpassN_next_z(AllpassN *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	const float *in = ZIN(0);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;

	//postbuf("AllpassN_next_z %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = dlybuf - ZOFF;
		float* dlyN    = dlybuf1 + unit->m_idelaylen;
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				float* dlywr = dlybuf1 + (iwrphase & mask);
				float* dlyrd = dlybuf1 + (irdphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;
				if (irdphase < 0) {
					feedbk = -feedbk;
					LOOP(nsmps,
						float dwr = ZXP(in);
						ZXP(dlywr) = dwr;
						ZXP(out) = feedbk * dwr;
					);
					feedbk = -feedbk;
				} else {
					LOOP(nsmps,
						float x1 = ZXP(dlyrd);
						float dwr = x1 * feedbk + ZXP(in);
						ZXP(dlywr) = dwr;
						ZXP(out) = x1 - feedbk * dwr;
					);
				}
				iwrphase += nsmps;
				irdphase += nsmps;
			}
		} else {
			float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				float* dlyrd = dlybuf1 + (irdphase & mask);
				float* dlywr = dlybuf1 + (iwrphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				nsmps = sc_min(remain, nsmps);
				remain -= nsmps;

				if (irdphase < 0) {
					dlyrd += nsmps;
					LOOP(nsmps,
						float dwr = ZXP(in);
						ZXP(dlywr) = dwr;
						ZXP(out) = -feedbk * dwr;
						feedbk += feedbk_slope;
					);
				} else {
					LOOP(nsmps,
						float x1 = ZXP(dlyrd);
						float dwr = x1 * feedbk + ZXP(in);
						ZXP(dlywr) = dwr;
						ZXP(out) = x1 - feedbk * dwr;
						feedbk += feedbk_slope;
					);
				}
				iwrphase += nsmps;
				irdphase += nsmps;
			}
			unit->m_feedbk = feedbk;
			unit->m_decaytime = decaytime;
		}
	} else {
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP1(inNumSamples,
			dsamp += dsamp_slope;
			feedbk += feedbk_slope;
			AllpassN_helper<true>::perform(in, out, dlybuf, iwrphase, (long)dsamp, mask, feedbk);
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen)
		SETCALC(AllpassN_next);
}

template <bool checked>
inline void AllpassN_perform_a(AllpassN *unit, int inNumSamples)
{
	FilterX_perform_a<AllpassN_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)AllpassN_next_a);
}

void AllpassN_next_a(AllpassN *unit, int inNumSamples)
{
	AllpassN_perform_a<false>(unit, inNumSamples);
}

void AllpassN_next_a_z(AllpassN *unit, int inNumSamples)
{
	AllpassN_perform_a<true>(unit, inNumSamples);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void AllpassL_Ctor(AllpassL *unit)
{
	bool allocationSucessful = FeedbackDelay_Reset(unit, "AllpassL");
	if (!allocationSucessful)
		return;

	if(INRATE(2) == calc_FullRate)
		SETCALC(AllpassL_next_a_z);
	else
		SETCALC(AllpassL_next_z);
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void AllpassL_perform(AllpassL *unit, int inNumSamples)
{
	FilterX_perform<AllpassL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)AllpassL_next);
}

void AllpassL_next(AllpassL *unit, int inNumSamples)
{
	AllpassL_perform<false>(unit, inNumSamples);
}

void AllpassL_next_z(AllpassL *unit, int inNumSamples)
{
	AllpassL_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void AllpassL_perform_a(AllpassL *unit, int inNumSamples)
{
	FilterX_perform_a<AllpassL_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)AllpassL_next_a);
}

void AllpassL_next_a(AllpassL *unit, int inNumSamples)
{
	AllpassL_perform_a<false>(unit, inNumSamples);
}

void AllpassL_next_a_z(AllpassL *unit, int inNumSamples)
{
	AllpassL_perform_a<true>(unit, inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void AllpassC_Ctor(AllpassC *unit)
{
	bool allocationSucessful = FeedbackDelay_Reset(unit, "AllpassC");
	if (!allocationSucessful)
		return;

	if(INRATE(2) == calc_FullRate)
		SETCALC(AllpassC_next_a_z);
	else
		SETCALC(AllpassC_next_z);
	ZOUT0(0) = 0.f;
}

template <bool checked>
inline void AllpassC_perform(AllpassC *unit, int inNumSamples)
{
	FilterX_perform<AllpassC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)AllpassC_next);
}

void AllpassC_next(AllpassC *unit, int inNumSamples)
{
	AllpassC_perform<false>(unit, inNumSamples);
}

void AllpassC_next_z(AllpassC *unit, int inNumSamples)
{
	AllpassC_perform<true>(unit, inNumSamples);
}

template <bool checked>
inline void AllpassC_perform_a(AllpassC *unit, int inNumSamples)
{
	FilterX_perform_a<AllpassC_helper<checked> >(unit, inNumSamples, (UnitCalcFunc)AllpassC_next_a);
}

void AllpassC_next_a(AllpassC *unit, int inNumSamples)
{
	AllpassC_perform_a<false>(unit, inNumSamples);
}

void AllpassC_next_a_z(AllpassC *unit, int inNumSamples)
{
	AllpassC_perform_a<true>(unit, inNumSamples);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


inline double sc_loop1(int32 in, int32 lo, int32 hi)
{
	// avoid the divide if possible
	if (in >= hi) {
		in -= hi;
		if (in < hi) return in;
	} else if (in < lo) {
		in += hi;
		if (in >= lo) return in;
	} else return in;

	int32 range = hi - lo;
	return lo + range * (in-lo) / range;
}

#if NOTYET

void SimpleLoopBuf_next_kk(SimpleLoopBuf *unit, int inNumSamples)
{
	float trig     = ZIN0(1);
	double loopstart  = (double)ZIN0(2);
	double loopend    = (double)ZIN0(3);
	GET_BUF

	int numOutputs = unit->mNumOutputs;
	if (!checkBuffer(unit, bufData, bufChannels, numOutputs, inNumSamples))
		return;


	loopend = sc_max(loopend, bufFrames);
	int32 phase = unit->m_phase;
	if (trig > 0.f && unit->m_prevtrig <= 0.f) {
		unit->mDone = false;
		phase = (int32)ZIN0(2);
	}
	unit->m_prevtrig = trig;
	for (int i=0; i<inNumSamples; ++i) {
		phase = sc_loop1(phase, loopstart, loopend);
		int32 iphase = (int32)phase;
		float* table1 = bufData + iphase * bufChannels;
		int32 index = 0;
		for (uint32 channel=0; channel<bufChannels; ++channel) {
			OUT(channel[i]) = table1[index++];
		}

		phase++;
	}
	unit->m_phase = phase;
}


void SimpleLoopBuf_Ctor(SimpleLoopBuf *unit)
{
	SETCALC(SimpleLoopBuf_next_kk);

	unit->m_fbufnum = -1e9f;
	unit->m_prevtrig = 0.;
	unit->m_phase = ZIN0(2);

	ClearUnitOutputs(unit, 1);
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////

#define GET_SCOPEBUF \
	float fbufnum  = ZIN0(0); \
	if (fbufnum != unit->m_fbufnum) { \
		World *world = unit->mWorld; \
		if (!world->mNumSndBufs) { \
			ClearUnitOutputs(unit, inNumSamples); \
			return; \
		} \
		uint32 bufnum = (int)fbufnum; \
		if (bufnum >= world->mNumSndBufs) bufnum = 0; \
		unit->m_fbufnum = fbufnum; \
		unit->m_buf = world->mSndBufs + bufnum; \
		unit->m_bufupdates = world->mSndBufUpdates + bufnum; \
	} \
	SndBuf *buf = unit->m_buf; \
	LOCK_SNDBUF(buf); \
	SndBufUpdates *bufupdates = unit->m_bufupdates; \
	float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufFrames __attribute__((__unused__)) = buf->frames; \

void ScopeOut_next(ScopeOut *unit, int inNumSamples)
{
	GET_SCOPEBUF

	if (!bufData) {
		unit->m_framepos = 0;
		return;
	}

	SETUP_IN(1)

	uint32 framepos = unit->m_framepos;
	if (framepos >= bufFrames) {
		unit->m_framepos = 0;
	}

	if (bufupdates->reads != bufupdates->writes) {
		unit->m_framepos += inNumSamples;
		return;
	}

	bufData += framepos * bufChannels;

	int remain = (bufFrames - framepos), wrap = 0;

	if(inNumSamples <= remain) {
		remain = inNumSamples;
		wrap = 0;
	}
	else
		wrap = inNumSamples - remain;

	if (bufChannels > 2) {
		for (int j=0; j<remain; ++j) {
			for (uint32 i=0; i<bufChannels; ++i) {
				*bufData++ = *++(in[i]);
			}
		}

		bufData = buf->data;

		for (int j=0; j<wrap; ++j) {
			for (uint32 i=0; i<bufChannels; ++i) {
				*bufData++ = *++(in[i]);
			}
		}
	} else if (bufChannels == 2) {
		float *in0 = in[0];
		float *in1 = in[1];
		for (int j=0; j<remain; ++j) {
			*bufData++ = *++in0;
			*bufData++ = *++in1;
		}

		bufData = buf->data;

		for (int j=0; j<wrap; ++j) {
			*bufData++ = *++in0;
			*bufData++ = *++in1;
		}
	} else {
		float *in0 = in[0];
		for (int j=0; j<remain; ++j) {
			*bufData++ = *++in0;
		}

		bufData = buf->data;

		for (int j=0; j<wrap; ++j) {
			*bufData++ = *++in0;
		}
	}

	unit->m_framepos += inNumSamples;
	unit->m_framecount += inNumSamples;

	if (unit->m_framecount >= bufFrames) {
		bufupdates->writes++;
		unit->m_framecount = 0;
	}
}



void ScopeOut_Ctor(ScopeOut *unit)
{

	unit->m_fbufnum = -1e9;
	unit->m_framepos = 0;
	unit->m_framecount = 0;
	unit->mIn = 0;
	SETCALC(ScopeOut_next);
}

void ScopeOut_Dtor(ScopeOut *unit)
{
	TAKEDOWN_IN
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ScopeOut2 : public Unit
{
	ScopeBufferHnd m_buffer;
	float **m_inBuffers;
	int m_maxPeriod;
	uint32 m_phase;
};


void ScopeOut2_next(ScopeOut2 *unit, int inNumSamples)
{
	if( !unit->m_buffer ) return;

	const int inputOffset = 3;
	int numChannels = unit->mNumInputs - inputOffset;

	uint32 period = (uint32)ZIN0(2);
	uint32 framepos = unit->m_phase;

	period = std::max((uint32)inNumSamples, std::min(unit->m_buffer.maxFrames, period));

	if( framepos >= period ) framepos = 0;

	int remain = period - framepos, wrap = 0;

	if(inNumSamples <= remain)
		remain = inNumSamples;
	else
		wrap = inNumSamples - remain;

	for (int i = 0; i != numChannels; ++i) {
		float * inBuf = unit->m_buffer.channel_data(i);
		const float * in = IN(inputOffset + i);

		memcpy(inBuf + framepos, in, remain * sizeof(float));
	}

	if(framepos + inNumSamples >= period)
		(*ft->fPushScopeBuffer)(unit->mWorld, unit->m_buffer, period);

	if (wrap) {
		for (int i = 0; i != numChannels; ++i) {
			float * inBuf = unit->m_buffer.channel_data(i);
			const float * in = IN(inputOffset + i);
			memcpy(inBuf, in + remain, wrap * sizeof(float));
		}
	}

	framepos += inNumSamples;
	if (framepos >= period)
		framepos = wrap;

	unit->m_phase = framepos;
}

void ScopeOut2_Ctor(ScopeOut2 *unit)
{
	uint32 numChannels = unit->mNumInputs - 3;
	uint32 scopeNum = (uint32)ZIN0(0);
	uint32 maxFrames = (uint32)ZIN0(1);

	bool ok = (*ft->fGetScopeBuffer)(unit->mWorld, scopeNum, numChannels, maxFrames, unit->m_buffer);

	if( !ok ) {
		if( unit->mWorld->mVerbosity > -1 && !unit->mDone)
			Print("ScopeOut2: Requested scope buffer unavailable! (index: %d, channels: %d, size: %d)\n",
				  scopeNum, numChannels, maxFrames);
	}
	else {
		unit->m_phase = 0;
	}

	SETCALC(ScopeOut2_next);
}

void ScopeOut2_Dtor(ScopeOut2 *unit)
{
	if( unit->m_buffer )
		(*ft->fReleaseScopeBuffer)(unit->mWorld, unit->m_buffer);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

struct PitchShift : public Unit
{
	float *dlybuf;
	float dsamp1, dsamp1_slope, ramp1, ramp1_slope;
	float dsamp2, dsamp2_slope, ramp2, ramp2_slope;
	float dsamp3, dsamp3_slope, ramp3, ramp3_slope;
	float dsamp4, dsamp4_slope, ramp4, ramp4_slope;
	float fdelaylen, slope;
	long iwrphase, idelaylen, mask;
	long counter, stage, numoutput, framesize;
};

void PitchShift_next(PitchShift *unit, int inNumSamples);
void PitchShift_next(PitchShift *unit, int inNumSamples)
{
	float *out, *in, *dlybuf;
	float disppchratio, pchratio, pchratio1, value;
	float dsamp1, dsamp1_slope, ramp1, ramp1_slope;
	float dsamp2, dsamp2_slope, ramp2, ramp2_slope;
	float dsamp3, dsamp3_slope, ramp3, ramp3_slope;
	float dsamp4, dsamp4_slope, ramp4, ramp4_slope;
	float fdelaylen, d1, d2, frac, slope, samp_slope, startpos, winsize, pchdisp, timedisp;
	long remain, nsmps, idelaylen, irdphase, irdphaseb, iwrphase, mask, idsamp;
	long counter, stage, framesize;

	RGET

	out = ZOUT(0);
	in = ZIN(0);

	pchratio = ZIN0(2);
	winsize = ZIN0(1);
	pchdisp = ZIN0(3);
	timedisp = ZIN0(4);
	timedisp = sc_clip(timedisp, 0.f, winsize) * SAMPLERATE;

	dlybuf = unit->dlybuf;
	fdelaylen = unit->fdelaylen;
	idelaylen = unit->idelaylen;
	iwrphase = unit->iwrphase;

	counter = unit->counter;
	stage = unit->stage;
	mask = unit->mask;
	framesize = unit->framesize;

	dsamp1 = unit->dsamp1;
	dsamp2 = unit->dsamp2;
	dsamp3 = unit->dsamp3;
	dsamp4 = unit->dsamp4;

	dsamp1_slope = unit->dsamp1_slope;
	dsamp2_slope = unit->dsamp2_slope;
	dsamp3_slope = unit->dsamp3_slope;
	dsamp4_slope = unit->dsamp4_slope;

	ramp1 = unit->ramp1;
	ramp2 = unit->ramp2;
	ramp3 = unit->ramp3;
	ramp4 = unit->ramp4;

	ramp1_slope = unit->ramp1_slope;
	ramp2_slope = unit->ramp2_slope;
	ramp3_slope = unit->ramp3_slope;
	ramp4_slope = unit->ramp4_slope;

	slope = unit->slope;

	remain = inNumSamples;
	while (remain) {
		if (counter <= 0) {
			counter = framesize >> 2;
			unit->stage = stage = (stage + 1) & 3;
			disppchratio = pchratio;
			if (pchdisp != 0.f) {
				disppchratio += (pchdisp * frand2(s1,s2,s3));
			}
			disppchratio = sc_clip(disppchratio, 0.f, 4.f);
			pchratio1 = disppchratio - 1.f;
			samp_slope = -pchratio1;
			startpos = pchratio1 < 0.f ? 2.f : framesize * pchratio1 + 2.f;
			startpos += (timedisp * frand(s1,s2,s3));
			switch(stage) {
				case 0 :
					unit->dsamp1_slope = dsamp1_slope = samp_slope;
					dsamp1 = startpos;
					ramp1 = 0.0;
					unit->ramp1_slope = ramp1_slope =  slope;
					unit->ramp3_slope = ramp3_slope = -slope;
					break;
				case 1 :
					unit->dsamp2_slope = dsamp2_slope = samp_slope;
					dsamp2 = startpos;
					ramp2 = 0.0;
					unit->ramp2_slope = ramp2_slope =  slope;
					unit->ramp4_slope = ramp4_slope = -slope;
					break;
				case 2 :
					unit->dsamp3_slope = dsamp3_slope = samp_slope;
					dsamp3 = startpos;
					ramp3 = 0.0;
					unit->ramp3_slope = ramp3_slope =  slope;
					unit->ramp1_slope = ramp1_slope = -slope;
					break;
				case 3 :
					unit->dsamp4_slope = dsamp4_slope = samp_slope;
					dsamp4 = startpos;
					ramp4 = 0.0;
					unit->ramp2_slope = ramp2_slope = -slope;
					unit->ramp4_slope = ramp4_slope =  slope;
					break;
			}
		/*Print("%d %d    %g %g %g %g    %g %g %g %g    %g %g %g %g\n",
			counter, stage, dsamp1_slope, dsamp2_slope, dsamp3_slope, dsamp4_slope,
				dsamp1, dsamp2, dsamp3, dsamp4,
				ramp1, ramp2, ramp3, ramp4);*/

		}

		nsmps = sc_min(remain, counter);
		remain -= nsmps;
		counter -= nsmps;

		LOOP(nsmps,
			iwrphase = (iwrphase + 1) & mask;

			dsamp1 += dsamp1_slope;
			idsamp = (long)dsamp1;
			frac = dsamp1 - idsamp;
			irdphase = (iwrphase - idsamp) & mask;
			irdphaseb = (irdphase - 1) & mask;
			d1 = dlybuf[irdphase];
			d2 = dlybuf[irdphaseb];
			value = (d1 + frac * (d2 - d1)) * ramp1;
			ramp1 += ramp1_slope;

			dsamp2 += dsamp2_slope;
			idsamp = (long)dsamp2;
			frac = dsamp2 - idsamp;
			irdphase = (iwrphase - idsamp) & mask;
			irdphaseb = (irdphase - 1) & mask;
			d1 = dlybuf[irdphase];
			d2 = dlybuf[irdphaseb];
			value += (d1 + frac * (d2 - d1)) * ramp2;
			ramp2 += ramp2_slope;

			dsamp3 += dsamp3_slope;
			idsamp = (long)dsamp3;
			frac = dsamp3 - idsamp;
			irdphase = (iwrphase - idsamp) & mask;
			irdphaseb = (irdphase - 1) & mask;
			d1 = dlybuf[irdphase];
			d2 = dlybuf[irdphaseb];
			value += (d1 + frac * (d2 - d1)) * ramp3;
			ramp3 += ramp3_slope;

			dsamp4 += dsamp4_slope;
			idsamp = (long)dsamp4;
			frac = dsamp4 - idsamp;
			irdphase = (iwrphase - idsamp) & mask;
			irdphaseb = (irdphase - 1) & mask;
			d1 = dlybuf[irdphase];
			d2 = dlybuf[irdphaseb];
			value += (d1 + frac * (d2 - d1)) * ramp4;
			ramp4 += ramp4_slope;

			dlybuf[iwrphase] = ZXP(in);
			ZXP(out) = value *= 0.5;
		);
	}

	unit->counter = counter;

	unit->dsamp1 = dsamp1;
	unit->dsamp2 = dsamp2;
	unit->dsamp3 = dsamp3;
	unit->dsamp4 = dsamp4;

	unit->ramp1 = ramp1;
	unit->ramp2 = ramp2;
	unit->ramp3 = ramp3;
	unit->ramp4 = ramp4;

	unit->iwrphase = iwrphase;

	RPUT
}




void PitchShift_next_z(PitchShift *unit, int inNumSamples);
void PitchShift_next_z(PitchShift *unit, int inNumSamples)
{
	float *out, *in, *dlybuf;
	float disppchratio, pchratio, pchratio1, value;
	float dsamp1, dsamp1_slope, ramp1, ramp1_slope;
	float dsamp2, dsamp2_slope, ramp2, ramp2_slope;
	float dsamp3, dsamp3_slope, ramp3, ramp3_slope;
	float dsamp4, dsamp4_slope, ramp4, ramp4_slope;
	float fdelaylen, d1, d2, frac, slope, samp_slope, startpos, winsize, pchdisp, timedisp;
	long remain, nsmps, idelaylen, irdphase, irdphaseb, iwrphase;
	long mask, idsamp;
	long counter, stage, framesize, numoutput;

	RGET

	out = ZOUT(0);
	in = ZIN(0);
	pchratio = ZIN0(2);
	winsize = ZIN0(1);
	pchdisp = ZIN0(3);
	timedisp = ZIN0(4);
	timedisp = sc_clip(timedisp, 0.f, winsize) * SAMPLERATE;

	dlybuf = unit->dlybuf;
	fdelaylen = unit->fdelaylen;
	idelaylen = unit->idelaylen;
	iwrphase = unit->iwrphase;
	numoutput = unit->numoutput;

	counter = unit->counter;
	stage = unit->stage;
	mask = unit->mask;
	framesize = unit->framesize;

	dsamp1 = unit->dsamp1;
	dsamp2 = unit->dsamp2;
	dsamp3 = unit->dsamp3;
	dsamp4 = unit->dsamp4;

	dsamp1_slope = unit->dsamp1_slope;
	dsamp2_slope = unit->dsamp2_slope;
	dsamp3_slope = unit->dsamp3_slope;
	dsamp4_slope = unit->dsamp4_slope;

	ramp1 = unit->ramp1;
	ramp2 = unit->ramp2;
	ramp3 = unit->ramp3;
	ramp4 = unit->ramp4;

	ramp1_slope = unit->ramp1_slope;
	ramp2_slope = unit->ramp2_slope;
	ramp3_slope = unit->ramp3_slope;
	ramp4_slope = unit->ramp4_slope;

	slope = unit->slope;

	remain = inNumSamples;
	while (remain) {
		if (counter <= 0) {
			counter = framesize >> 2;
			unit->stage = stage = (stage + 1) & 3;
			disppchratio = pchratio;
			if (pchdisp != 0.f) {
				disppchratio += (pchdisp * frand2(s1,s2,s3));
			}
			disppchratio = sc_clip(disppchratio, 0.f, 4.f);
			pchratio1 = disppchratio - 1.f;
			samp_slope = -pchratio1;
			startpos = pchratio1 < 0.f ? 2.f : framesize * pchratio1 + 2.f;
			startpos += (timedisp * frand(s1,s2,s3));
			switch(stage) {
				case 0 :
					unit->dsamp1_slope = dsamp1_slope = samp_slope;
					dsamp1 = startpos;
					ramp1 = 0.0;
					unit->ramp1_slope = ramp1_slope =  slope;
					unit->ramp3_slope = ramp3_slope = -slope;
					break;
				case 1 :
					unit->dsamp2_slope = dsamp2_slope = samp_slope;
					dsamp2 = startpos;
					ramp2 = 0.0;
					unit->ramp2_slope = ramp2_slope =  slope;
					unit->ramp4_slope = ramp4_slope = -slope;
					break;
				case 2 :
					unit->dsamp3_slope = dsamp3_slope = samp_slope;
					dsamp3 = startpos;
					ramp3 = 0.0;
					unit->ramp3_slope = ramp3_slope =  slope;
					unit->ramp1_slope = ramp1_slope = -slope;
					break;
				case 3 :
					unit->dsamp4_slope = dsamp4_slope = samp_slope;
					dsamp4 = startpos;
					ramp4 = 0.0;
					unit->ramp2_slope = ramp2_slope = -slope;
					unit->ramp4_slope = ramp4_slope =  slope;
					break;
			}
		/*Print("z %d %d    %g %g %g %g    %g %g %g %g    %g %g %g %g\n",
			counter, stage, dsamp1_slope, dsamp2_slope, dsamp3_slope, dsamp4_slope,
				dsamp1, dsamp2, dsamp3, dsamp4,
				ramp1, ramp2, ramp3, ramp4);*/
		}
		nsmps = sc_min(remain, counter);
		remain -= nsmps;
		counter -= nsmps;

		while (nsmps--) {
			numoutput++;
			iwrphase = (iwrphase + 1) & mask;

			dsamp1 += dsamp1_slope;
			idsamp = (long)dsamp1;
			frac = dsamp1 - idsamp;
			irdphase = (iwrphase - idsamp) & mask;
			irdphaseb = (irdphase - 1) & mask;
			if (numoutput < idelaylen) {
				if (irdphase > iwrphase) {
					value = 0.f;
				} else if (irdphaseb > iwrphase) {
					d1 = dlybuf[irdphase];
					value = (d1 - frac * d1) * ramp1;
				} else {
					d1 = dlybuf[irdphase];
					d2 = dlybuf[irdphaseb];
					value = (d1 + frac * (d2 - d1)) * ramp1;
				}
			} else {
				d1 = dlybuf[irdphase];
				d2 = dlybuf[irdphaseb];
				value = (d1 + frac * (d2 - d1)) * ramp1;
			}
			ramp1 += ramp1_slope;

			dsamp2 += dsamp2_slope;
			idsamp = (long)dsamp2;
			frac = dsamp2 - idsamp;
			irdphase = (iwrphase - idsamp) & mask;
			irdphaseb = (irdphase - 1) & mask;
			if (numoutput < idelaylen) {
				if (irdphase > iwrphase) {
					//value += 0.f;
				} else if (irdphaseb > iwrphase) {
					d1 = dlybuf[irdphase];
					value += (d1 - frac * d1) * ramp2;
				} else {
					d1 = dlybuf[irdphase];
					d2 = dlybuf[irdphaseb];
					value += (d1 + frac * (d2 - d1)) * ramp2;
				}
			} else {
				d1 = dlybuf[irdphase];
				d2 = dlybuf[irdphaseb];
				value += (d1 + frac * (d2 - d1)) * ramp2;
			}
			ramp2 += ramp2_slope;

			dsamp3 += dsamp3_slope;
			idsamp = (long)dsamp3;
			frac = dsamp3 - idsamp;
			irdphase = (iwrphase - idsamp) & mask;
			irdphaseb = (irdphase - 1) & mask;
			if (numoutput < idelaylen) {
				if (irdphase > iwrphase) {
					//value += 0.f;
				} else if (irdphaseb > iwrphase) {
					d1 = dlybuf[irdphase];
					value += (d1 - frac * d1) * ramp3;
				} else {
					d1 = dlybuf[irdphase];
					d2 = dlybuf[irdphaseb];
					value += (d1 + frac * (d2 - d1)) * ramp3;
				}
			} else {
				d1 = dlybuf[irdphase];
				d2 = dlybuf[irdphaseb];
				value += (d1 + frac * (d2 - d1)) * ramp3;
			}
			ramp3 += ramp3_slope;

			dsamp4 += dsamp4_slope;
			idsamp = (long)dsamp4;
			frac = dsamp4 - idsamp;
			irdphase = (iwrphase - idsamp) & mask;
			irdphaseb = (irdphase - 1) & mask;

			if (numoutput < idelaylen) {
				if (irdphase > iwrphase) {
					//value += 0.f;
				} else if (irdphaseb > iwrphase) {
					d1 = dlybuf[irdphase];
					value += (d1 - frac * d1) * ramp4;
				} else {
					d1 = dlybuf[irdphase];
					d2 = dlybuf[irdphaseb];
					value += (d1 + frac * (d2 - d1)) * ramp4;
				}
			} else {
				d1 = dlybuf[irdphase];
				d2 = dlybuf[irdphaseb];
				value += (d1 + frac * (d2 - d1)) * ramp4;
			}
			ramp4 += ramp4_slope;

			dlybuf[iwrphase] = ZXP(in);
			ZXP(out) = value *= 0.5;
		}
	}

	unit->counter = counter;
	unit->stage = stage;
	unit->mask = mask;

	unit->dsamp1 = dsamp1;
	unit->dsamp2 = dsamp2;
	unit->dsamp3 = dsamp3;
	unit->dsamp4 = dsamp4;

	unit->ramp1 = ramp1;
	unit->ramp2 = ramp2;
	unit->ramp3 = ramp3;
	unit->ramp4 = ramp4;

	unit->numoutput = numoutput;
	unit->iwrphase = iwrphase;

	if (numoutput >= idelaylen) {
		SETCALC(PitchShift_next);
	}

	RPUT
}


void PitchShift_Ctor(PitchShift *unit);
void PitchShift_Ctor(PitchShift *unit)
{
	long delaybufsize;
	float *out, *in, *dlybuf;
	float winsize, pchratio;
	float fdelaylen, slope;
	long framesize, last;

	out = ZOUT(0);
	in = ZIN(0);
	pchratio = ZIN0(2);
	winsize = ZIN0(1);

	delaybufsize = (long)ceil(winsize * SAMPLERATE * 3.f + 3.f);
	fdelaylen = delaybufsize - 3;

	delaybufsize = delaybufsize + BUFLENGTH;
	delaybufsize = NEXTPOWEROFTWO(delaybufsize);  // round up to next power of two
	dlybuf = (float*)RTAlloc(unit->mWorld, delaybufsize * sizeof(float));

	SETCALC(PitchShift_next_z);

	*dlybuf = ZIN0(0);
	ZOUT0(0) = 0.f;

	unit->dlybuf = dlybuf;
	unit->idelaylen = delaybufsize;
	unit->fdelaylen = fdelaylen;
	unit->iwrphase = 0;
	unit->numoutput = 0;
	unit->mask = last = (delaybufsize - 1);

	unit->framesize = framesize = ((long)(winsize * SAMPLERATE) + 2) & ~3;
	unit->slope = slope = 2.f / framesize;
	unit->stage = 3;
	unit->counter = framesize >> 2;
	unit->ramp1 = 0.5;
	unit->ramp2 = 1.0;
	unit->ramp3 = 0.5;
	unit->ramp4 = 0.0;

	unit->ramp1_slope = -slope;
	unit->ramp2_slope = -slope;
	unit->ramp3_slope =  slope;
	unit->ramp4_slope =  slope;

	dlybuf[last  ] = 0.f; // put a few zeroes where we start the read heads
	dlybuf[last-1] = 0.f;
	dlybuf[last-2] = 0.f;

	unit->numoutput = 0;

	// start all read heads 2 samples behind the write head
	unit->dsamp1 = unit->dsamp2 = unit->dsamp3 = unit->dsamp4 = 2.f;
	// pch ratio is initially zero for the read heads
	unit->dsamp1_slope = unit->dsamp2_slope = unit->dsamp3_slope = unit->dsamp4_slope = 1.f;
}



void PitchShift_Dtor(PitchShift *unit)
{
	RTFree(unit->mWorld, unit->dlybuf);
}




typedef struct graintap1 {
	float pos, rate, level, slope, curve;
	long counter;
	struct graintap1 *next;
} GrainTap1;

#define MAXDGRAINS 32

struct GrainTap : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;

	float fdelaylen;
	long bufsize, iwrphase;
	long nextTime;
	GrainTap1 grains[MAXDGRAINS];
	GrainTap1 *firstActive, *firstFree;
};


// coefs: pos, rate, level, slope, curve, counter

void GrainTap_next(GrainTap *unit, int inNumSamples);
void GrainTap_next(GrainTap *unit, int inNumSamples)
{
	float *out, *out0;
	const float * dlybuf;
	float sdur, rdur, rdur2;
	float dsamp, dsamp_slope, fdelaylen, d1, d2, frac;
	float level, slope, curve;
	float maxpitch, pitch, maxtimedisp, timedisp, density;
	long remain, nsmps, irdphase, irdphaseb, iwrphase, iwrphase0;
	long idsamp, koffset;
	long counter;
	uint32 bufsize;
	GrainTap1 *grain, *prevGrain, *nextGrain;

	GET_BUF_SHARED

	RGET

	out0 = ZOUT(0);

	// bufnum, grainDur, pchRatio, pchDisp, timeDisp, overlap
	// 0       1         2         3        4         5

	density = ZIN0(5);
	density = sc_max(0.0001, density);

	bufsize = unit->bufsize;
	if (bufsize != bufSamples) {
		ClearUnitOutputs(unit, inNumSamples);
		return;
	}

	dlybuf = bufData;
	fdelaylen = unit->fdelaylen;
	iwrphase0 = unit->iwrphase;

	// initialize buffer to zero
	out = out0;
	LOOP1(inNumSamples, ZXP(out) = 0.f;);

	// do all current grains
	prevGrain = NULL;
	grain = unit->firstActive;
	while (grain) {

		dsamp = grain->pos;
		dsamp_slope = grain->rate;
		level = grain->level;
		slope = grain->slope;
		curve = grain->curve;
		counter = grain->counter;

		nsmps = sc_min(counter, inNumSamples);
		iwrphase = iwrphase0;
		out = out0;
		LOOP(nsmps,
			dsamp += dsamp_slope;
			idsamp = (long)dsamp;
			frac = dsamp - idsamp;
			iwrphase = (iwrphase + 1) & mask;
			irdphase = (iwrphase - idsamp) & mask;
			irdphaseb = (irdphase - 1) & mask;
			d1 = dlybuf[irdphase];
			d2 = dlybuf[irdphaseb];
			ZXP(out) += (d1 + frac * (d2 - d1)) * level;
			level += slope;
			slope += curve;
		);
		grain->pos = dsamp;
		grain->level = level;
		grain->slope = slope;
		grain->counter -= nsmps;

		nextGrain = grain->next;
		if (grain->counter <= 0) {
			// unlink from active list
			if (prevGrain) prevGrain->next = nextGrain;
			else unit->firstActive = nextGrain;

			// link onto free list
			grain->next = unit->firstFree;
			unit->firstFree = grain;
		} else {
			prevGrain = grain;
		}
		grain = nextGrain;
	}
	// start new grains
	remain = inNumSamples;
	while (unit->nextTime <= remain) {
		remain -= unit->nextTime;
		sdur = ZIN0(1) * SAMPLERATE;
		sdur = sc_max(sdur, 4.f);

		grain = unit->firstFree;
		if (grain) {
			unit->firstFree = grain->next;
			grain->next = unit->firstActive;
			unit->firstActive = grain;

			koffset = inNumSamples - remain;
			iwrphase = (iwrphase0 + koffset) & mask;

			grain->counter = (long)sdur;

			timedisp = ZIN0(4);
			timedisp = sc_max(timedisp, 0.f);
			timedisp = frand(s1,s2,s3) * timedisp * SAMPLERATE;

			pitch = ZIN0(2) + frand2(s1,s2,s3) * ZIN0(3);
			if (pitch >= 1.f) {
				maxpitch = 1.f + (fdelaylen/sdur);
				pitch = sc_min(pitch, maxpitch);

				dsamp_slope = 1.f - pitch;
				grain->rate = dsamp_slope;

				maxtimedisp = fdelaylen + sdur * dsamp_slope;
				timedisp = sc_min(timedisp, maxtimedisp);

				dsamp = BUFLENGTH + koffset + 2.f + timedisp - sdur * dsamp_slope;
				dsamp = sc_min(dsamp, fdelaylen);
			} else {
				maxpitch = -(1.f + (fdelaylen/sdur));
				pitch = sc_max(pitch, maxpitch);

				dsamp_slope = 1.f - pitch;
				grain->rate = dsamp_slope;

				maxtimedisp = fdelaylen - sdur * dsamp_slope;
				timedisp = sc_min(timedisp, maxtimedisp);

				dsamp = BUFLENGTH + koffset + 2.f + timedisp;
				dsamp = sc_min(dsamp, fdelaylen);
			}

			grain->pos = dsamp;
			//postbuf("ds %g %g %g\n", dsamp_slope, dsamp, fdelaylen);

			rdur = 1.f / sdur;
			rdur2 = rdur * rdur;
			grain->level = level = 0.f;
			grain->slope = slope = 4.0 * (rdur - rdur2);	// ampslope
			grain->curve = curve = -8.0 * rdur2;			// ampcurve

			nsmps = remain;
			out = out0 + koffset;
			LOOP(nsmps,
				dsamp += dsamp_slope;
				idsamp = (long)dsamp;
				frac = dsamp - idsamp;
				iwrphase = (iwrphase + 1) & mask;
				irdphase = (iwrphase - idsamp) & mask;
				irdphaseb = (irdphase - 1) & mask;
				d1 = dlybuf[irdphase];
				d2 = dlybuf[irdphaseb];
				ZXP(out) += (d1 + frac * (d2 - d1)) * level;
				level += slope;
				slope += curve;
			);
			grain->pos = dsamp;
			grain->level = level;
			grain->slope = slope;
			grain->counter -= nsmps;

			if (grain->counter <= 0) {
				// unlink from active list
				unit->firstActive = grain->next;

				// link onto free list
				grain->next = unit->firstFree;
				unit->firstFree = grain;
			}
		}
		unit->nextTime = (long)(sdur / density);
		if (unit->nextTime < 1) unit->nextTime = 1;

		/*if (grain == NULL) {
			postbuf("nextTime %d %g %g %p %p %p\n", unit->nextTime, sdur, density,
				grain, unit->firstActive, unit->firstFree);
		}*/
	}
	iwrphase = (iwrphase0 + BUFLENGTH) & mask;
	unit->nextTime -= remain;
	if (unit->nextTime < 0) unit->nextTime = 0;

	unit->iwrphase = iwrphase;

	RPUT
}


void GrainTap_Ctor(GrainTap *unit);
void GrainTap_Ctor(GrainTap *unit)
{
	float fdelaylen;
	float maxdelaytime;

	GET_BUF

	if (!ISPOWEROFTWO(bufSamples)) {
		Print("GrainTap buffer size not a power of two.\n");
		SETCALC(*ClearUnitOutputs);
		return;
	}

	fdelaylen = bufSamples - 2 * BUFLENGTH - 3;
	maxdelaytime = fdelaylen * SAMPLEDUR;

	SETCALC(GrainTap_next);

	ZOUT0(0) = 0.f;

	unit->bufsize = bufSamples;
	unit->fdelaylen = fdelaylen;
	unit->iwrphase = 0;
	unit->nextTime = 0;
	for (int i=0; i<MAXDGRAINS-1; ++i) {
		unit->grains[i].next = unit->grains + (i + 1);
	}
	unit->grains[MAXDGRAINS-1].next = NULL;
	unit->firstFree = unit->grains;
	unit->firstActive = NULL;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////


#define GRAIN_BUF \
    const SndBuf *buf = NULL;                                           \
    if (bufnum >= world->mNumSndBufs) {                                 \
        int localBufNum = bufnum - numBufs;                             \
        Graph *parent = unit->mParent;                                  \
        if(localBufNum <= parent->localBufNum) {                        \
            buf = parent->mLocalSndBufs + localBufNum;                  \
        } else {                                                        \
            continue;                                                   \
        }                                                               \
    } else {                                                            \
        buf = bufs + bufnum;                                            \
    }                                                                   \
    LOCK_SNDBUF_SHARED(buf);                                            \
    const float *bufData __attribute__((__unused__)) = buf->data;       \
    uint32 bufChannels __attribute__((__unused__)) = buf->channels;     \
    uint32 bufSamples __attribute__((__unused__)) = buf->samples;       \
    uint32 bufFrames = buf->frames;                                     \
    int guardFrame __attribute__((__unused__)) = bufFrames - 2;         \

inline float IN_AT(Unit* unit, int index, int offset)
{
	if (INRATE(index) == calc_FullRate) return IN(index)[offset];
	if (INRATE(index) == calc_DemandRate) return DEMANDINPUT_A(index, offset + 1);
	return ZIN0(index);
}

inline double sc_gloop(double in, double hi)
{
	// avoid the divide if possible
	if (in >= hi) {
		in -= hi;
		if (in < hi) return in;
	} else if (in < 0.) {
		in += hi;
		if (in >= 0.) return in;
	} else return in;

	return in - hi * floor(in/hi);
}

#define GRAIN_LOOP_BODY_4 \
		float amp = y1 * y1; \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		const float* table1 = bufData + iphase; \
		const float* table0 = table1 - 1; \
		const float* table2 = table1 + 1; \
		const float* table3 = table1 + 2; \
		if (iphase == 0) { \
			table0 += bufSamples; \
		} else if (iphase >= guardFrame) { \
			if (iphase == guardFrame) { \
				table3 -= bufSamples; \
			} else { \
				table2 -= bufSamples; \
				table3 -= bufSamples; \
			} \
		} \
		float fracphase = phase - (double)iphase; \
		float a = table0[0]; \
		float b = table1[0]; \
		float c = table2[0]; \
		float d = table3[0]; \
		float outval = amp * cubicinterp(fracphase, a, b, c, d); \
		ZXP(out1) += outval * pan1; \
		if (numOutputs > 1) { ZXP(out2) += outval * pan2; } \
		double y0 = b1 * y1 - y2; \
		y2 = y1; \
		y1 = y0; \


#define GRAIN_LOOP_BODY_2 \
		float amp = y1 * y1; \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		const float* table1 = bufData + iphase; \
		const float* table2 = table1 + 1; \
		if (iphase > guardFrame) { \
			table2 -= bufSamples; \
		} \
		float fracphase = phase - (double)iphase; \
		float b = table1[0]; \
		float c = table2[0]; \
		float outval = amp * (b + fracphase * (c - b)); \
		ZXP(out1) += outval * pan1; \
		if (numOutputs > 1) { ZXP(out2) += outval * pan2; } \
		double y0 = b1 * y1 - y2; \
		y2 = y1; \
		y1 = y0; \


#define GRAIN_LOOP_BODY_1 \
		float amp = y1 * y1; \
		phase = sc_gloop(phase, loopMax); \
		int32 iphase = (int32)phase; \
		float outval = amp * bufData[iphase]; \
		ZXP(out1) += outval * pan1; \
		ZXP(out2) += outval * pan2; \
		double y0 = b1 * y1 - y2; \
		y2 = y1; \
		y1 = y0; \


void TGrains_next(TGrains *unit, int inNumSamples)
{
	float *trigin = IN(0);
	float prevtrig = unit->mPrevTrig;

	uint32 numOutputs = unit->mNumOutputs;
	ClearUnitOutputs(unit, inNumSamples);
	float *out[16];
	for (uint32 i=0; i<numOutputs; ++i) out[i] = ZOUT(i);

	World *world = unit->mWorld;
	SndBuf *bufs = world->mSndBufs;
	uint32 numBufs = world->mNumSndBufs;

	for (int i=0; i < unit->mNumActive; ) {
		Grain *grain = unit->mGrains + i;
		uint32 bufnum = grain->bufnum;

		GRAIN_BUF

		if (bufChannels != 1) {
			 ++i;
			 continue;
		}

		double loopMax = (double)bufFrames;

		float pan1 = grain->pan1;
		float pan2 = grain->pan2;
		double rate = grain->rate;
		double phase = grain->phase;
		double b1 = grain->b1;
		double y1 = grain->y1;
		double y2 = grain->y2;

		uint32 chan1 = grain->chan;
		uint32 chan2 = chan1 + 1;
		if (chan2 >= numOutputs) chan2 = 0;

		float *out1 = out[chan1];
		float *out2 = out[chan2];
		//printf("B chan %d %d  %p %p", chan1, chan2, out1, out2);

		int nsmps = sc_min(grain->counter, inNumSamples);
		if (grain->interp >= 4) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BODY_4;
				phase += rate;
			}
		} else if (grain->interp >= 2) {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BODY_2;
				phase += rate;
			}
		} else {
			for (int j=0; j<nsmps; ++j) {
				GRAIN_LOOP_BODY_1;
				phase += rate;
			}
		}

		grain->phase = phase;
		grain->y1 = y1;
		grain->y2 = y2;

		grain->counter -= nsmps;
		if (grain->counter <= 0) {
			// remove grain
			*grain = unit->mGrains[--unit->mNumActive];
		} else ++i;
	}

	int trigSamples = INRATE(0) == calc_FullRate ? inNumSamples : 1;

	for (int i=0; i<trigSamples; ++i) {
		float trig = trigin[i];

		if (trig > 0.f && prevtrig <= 0.f) {
			// start a grain
			if (unit->mNumActive+1 >= kMaxGrains) break;
			uint32 bufnum = (uint32)IN_AT(unit, 1, i);
			GRAIN_BUF

			if (bufChannels != 1) continue;

			float bufSampleRate = buf->samplerate;
			float bufRateScale = bufSampleRate * SAMPLEDUR;
			double loopMax = (double)bufFrames;

			Grain *grain = unit->mGrains + unit->mNumActive++;
			grain->bufnum = bufnum;

			double counter = floor(IN_AT(unit, 4, i) * SAMPLERATE);
			counter = sc_max(4., counter);
			grain->counter = (int)counter;

			double rate = grain->rate = IN_AT(unit, 2, i) * bufRateScale;
			double centerPhase = IN_AT(unit, 3, i) * bufSampleRate;
			double phase = centerPhase - 0.5 * counter * rate;

			float pan = IN_AT(unit, 5, i);
			float amp = IN_AT(unit, 6, i);
			grain->interp = (int)IN_AT(unit, 7, i);

			float panangle;
			float pan1, pan2;
			if (numOutputs > 1) {
				if (numOutputs > 2) {
					pan = sc_wrap(pan * 0.5f, 0.f, 1.f);
					float cpan = numOutputs * pan + 0.5f;
					float ipan = floor(cpan);
					float panfrac = cpan - ipan;
					panangle = panfrac * pi2_f;
					grain->chan = (int)ipan;
					if (grain->chan >= (int)numOutputs)
						grain->chan -= numOutputs;
				} else {
					grain->chan = 0;
					pan = sc_clip(pan * 0.5f + 0.5f, 0.f, 1.f);
					panangle = pan * pi2_f;
				}
				pan1 = grain->pan1 = amp * cos(panangle);
				pan2 = grain->pan2 = amp * sin(panangle);
			} else {
				grain->chan = 0;
				pan1 = grain->pan1 = amp;
				pan2 = grain->pan2 = 0.;
			}
			double w = pi / counter;
			double b1 = grain->b1 = 2. * cos(w);
			double y1 = sin(w);
			double y2 = 0.;

			uint32 chan1 = grain->chan;
			uint32 chan2 = chan1 + 1;
			if (chan2 >= numOutputs) chan2 = 0;

			float *out1 = out[chan1] + i;
			float *out2 = out[chan2] + i;

			int nsmps = sc_min(grain->counter, inNumSamples - i);
			if (grain->interp >= 4) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BODY_4;
					phase += rate;
				}
			} else if (grain->interp >= 2) {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BODY_2;
					phase += rate;
				}
			} else {
				for (int j=0; j<nsmps; ++j) {
					GRAIN_LOOP_BODY_1;
					phase += rate;
				}
			}

			grain->phase = phase;
			grain->y1 = y1;
			grain->y2 = y2;

			grain->counter -= nsmps;
			if (grain->counter <= 0) {
				// remove grain
				*grain = unit->mGrains[--unit->mNumActive];
			}
		}
		prevtrig = trig;
	}

	unit->mPrevTrig = prevtrig;
}

void TGrains_Ctor(TGrains *unit)
{
	SETCALC(TGrains_next);

	unit->mNumActive = 0;
	unit->mPrevTrig = 0.;

	ClearUnitOutputs(unit, 1);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
Pluck - Karplus-Strong
*/
void Pluck_Ctor(Pluck *unit)
{
	unit->m_maxdelaytime = IN0(2);
	unit->m_delaytime = IN0(3);
	unit->m_decaytime = IN0(4);
	unit->m_dlybuf = 0;
	bool allocationSucessful = DelayUnit_AllocDelayLine(unit, "Pluck");
	if (!allocationSucessful)
		return;

	unit->m_dsamp = CalcDelay(unit, unit->m_delaytime);

	unit->m_numoutput = 0;
	unit->m_iwrphase = 0;
	unit->m_feedbk = sc_CalcFeedback(unit->m_delaytime, unit->m_decaytime);

	if (INRATE(1) == calc_FullRate) {
		if(INRATE(5) == calc_FullRate){
			SETCALC(Pluck_next_aa_z);
		} else {
			SETCALC(Pluck_next_ak_z); //ak
		}
	} else {
		if(INRATE(5) == calc_FullRate){
			SETCALC(Pluck_next_ka_z); //ka
		} else {
			SETCALC(Pluck_next_kk_z); //kk
		}
	}
	OUT0(0) = unit->m_lastsamp = 0.f;
	unit->m_prevtrig = 0.f;
	unit->m_inputsamps = 0;
	unit->m_coef = IN0(5);
}

void Pluck_next_aa(Pluck *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float *trig = IN(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float *coef = IN(5);
	float lastsamp = unit->m_lastsamp;
	unsigned long inputsamps = unit->m_inputsamps;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float thisin, curtrig;
	float prevtrig = unit->m_prevtrig;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++){
			curtrig = trig[i];
			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
				inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
			}
			prevtrig = curtrig;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		for(int i = 0; i < inNumSamples; i++){
			curtrig = trig[i];
			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
				inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
			}
			prevtrig = curtrig;
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			feedbk += feedbk_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_prevtrig = prevtrig;
	unit->m_inputsamps = inputsamps;
	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

}


void Pluck_next_aa_z(Pluck *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float *trig = IN(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float *coef = IN(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;
	float thisin, curtrig;
	unsigned long inputsamps = unit->m_inputsamps;
	float prevtrig = unit->m_prevtrig;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++){
			curtrig = trig[i];
			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
				inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
			}
			prevtrig = curtrig;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
				float thiscoef = coef[i];
				float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
				dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
				out[i] = lastsamp = onepole;
			}
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		for(int i = 0; i < inNumSamples; i++) {
			curtrig = trig[i];
			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
				inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
			}
			prevtrig = curtrig;
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
				float thiscoef = coef[i];
				float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
				dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
				out[i] = lastsamp = onepole;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_inputsamps = inputsamps;
	unit->m_prevtrig = prevtrig;
	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(Pluck_next_aa);
	}
}

void Pluck_next_kk(Pluck *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float trig = IN0(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float coef = IN0(5);
	float lastsamp = unit->m_lastsamp;
	unsigned long inputsamps = unit->m_inputsamps;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float thisin;

	if ((unit->m_prevtrig <= 0.f) && (trig > 0.f)) {
		inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
	}
	unit->m_prevtrig = trig;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime && coef == unit->m_coef) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;

		for(int i = 0; i < inNumSamples; i++){
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
			out[i] = lastsamp = onepole; //value;
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		float curcoef = unit->m_coef;
		float coef_slope = CALCSLOPE(coef, curcoef);

		for(int i = 0; i < inNumSamples; i++){
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
			out[i] = lastsamp = onepole; //value;
			feedbk += feedbk_slope;
			curcoef += coef_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_coef = coef;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_inputsamps = inputsamps;
	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

}


void Pluck_next_kk_z(Pluck *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float trig = IN0(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float coef = IN0(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;
	float thisin;
	unsigned long inputsamps = unit->m_inputsamps;

	if ((unit->m_prevtrig <= 0.f) && (trig > 0.f)) {
		inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
	}
	unit->m_prevtrig = trig;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime && coef == unit->m_coef) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++){

			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
				float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
				dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
				out[i] = lastsamp = onepole; //value;
			}
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		float curcoef = unit->m_coef;
		float coef_slope = CALCSLOPE(coef, curcoef);

		for(int i = 0; i < inNumSamples; i++) {
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
				float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
				dlybuf[iwrphase & mask] = thisin + (feedbk * onepole);
				out[i] = lastsamp = onepole; //value;
			}
			feedbk += feedbk_slope;
			curcoef += coef_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
		unit->m_coef = coef;
	}

	unit->m_inputsamps = inputsamps;
	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(Pluck_next_kk);
	}
}

void Pluck_next_ak(Pluck *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float *trig = IN(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float coef = IN0(5);
	float lastsamp = unit->m_lastsamp;
	unsigned long inputsamps = unit->m_inputsamps;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float thisin, curtrig;
	float prevtrig = unit->m_prevtrig;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++){
			curtrig = trig[i];
			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
				inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
			}
			prevtrig = curtrig;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		float curcoef = unit->m_coef;
		float coef_slope = CALCSLOPE(coef, curcoef);

		for(int i = 0; i < inNumSamples; i++){
			curtrig = trig[i];
			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
				inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
			}
			prevtrig = curtrig;
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			feedbk += feedbk_slope;
			curcoef += coef_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
		unit->m_coef = coef;
	}

	unit->m_prevtrig = prevtrig;
	unit->m_inputsamps = inputsamps;
	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

}


void Pluck_next_ak_z(Pluck *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float *trig = IN(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float coef = IN0(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;
	float thisin, curtrig;
	unsigned long inputsamps = unit->m_inputsamps;
	float prevtrig = unit->m_prevtrig;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime && coef == unit->m_coef) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++){
			curtrig = trig[i];
			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
				inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
			}
			prevtrig = curtrig;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
				float onepole = ((1. - fabs(coef)) * value) + (coef * lastsamp);
				dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
				out[i] = lastsamp = onepole;
			}
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		float curcoef = unit->m_coef;
		float coef_slope = CALCSLOPE(coef, curcoef);

		for(int i = 0; i < inNumSamples; i++) {
			curtrig = trig[i];
			if ((prevtrig <= 0.f) && (curtrig > 0.f)) {
				inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
			}
			prevtrig = curtrig;
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
				float onepole = ((1. - fabs(curcoef)) * value) + (curcoef * lastsamp);
				dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
				out[i] = lastsamp = onepole;
			}
			feedbk += feedbk_slope;
			curcoef +=coef_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
		unit->m_coef = coef;
	}

	unit->m_inputsamps = inputsamps;
	unit->m_prevtrig = prevtrig;
	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(Pluck_next_ak);
	}
}


void Pluck_next_ka(Pluck *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float trig = IN0(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float *coef = IN(5);
	float lastsamp = unit->m_lastsamp;
	unsigned long inputsamps = unit->m_inputsamps;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float thisin;

	if ((unit->m_prevtrig <= 0.f) && (trig > 0.f)) {
		inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
	}
	unit->m_prevtrig = trig;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++){
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		for(int i = 0; i < inNumSamples; i++){
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float thiscoef = coef[i];
			float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
			dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
			out[i] = lastsamp = onepole;
			feedbk += feedbk_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_inputsamps = inputsamps;
	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

}


void Pluck_next_ka_z(Pluck *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float trig = IN0(1);
	float delaytime = IN0(3);
	float decaytime = IN0(4);
	float *coef = IN(5);
	float lastsamp = unit->m_lastsamp;

	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;
	float thisin;
	unsigned long inputsamps = unit->m_inputsamps;

	if ((unit->m_prevtrig <= 0.f) && (trig > 0.f)) {
		inputsamps = (long)(delaytime * unit->mRate->mSampleRate + .5f);
	}

	unit->m_prevtrig = trig;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		for(int i = 0; i < inNumSamples; i++){
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
				float thiscoef = coef[i];
				float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
				dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
				out[i] = lastsamp = onepole;
			}
			iwrphase++;
		};
	} else {

		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		float next_feedbk = sc_CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		for(int i = 0; i < inNumSamples; i++) {
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			if (inputsamps > 0) {
				thisin = in[i];
				--inputsamps;
			} else {
				thisin = 0.f;
			}
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = thisin;
				out[i] = 0.f;
			} else {
				if (irdphase1 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
				} else if (irdphase2 < 0) {
					d1 = d2 = d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
				} else if (irdphase3 < 0) {
					d3 = 0.f;
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
				} else {
					d0 = dlybuf[irdphase0 & mask];
					d1 = dlybuf[irdphase1 & mask];
					d2 = dlybuf[irdphase2 & mask];
					d3 = dlybuf[irdphase3 & mask];
				}
				float value = cubicinterp(frac, d0, d1, d2, d3);
				float thiscoef = coef[i];
				float onepole = ((1. - fabs(thiscoef)) * value) + (thiscoef * lastsamp);
				dlybuf[iwrphase & mask] = thisin + feedbk * onepole;
				out[i] = lastsamp = onepole;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		};
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}

	unit->m_inputsamps = inputsamps;
	unit->m_lastsamp = zapgremlins(lastsamp);
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(Pluck_next_ka);
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


#define DELTAP_BUF \
	World *world = unit->mWorld;\
	if (bufnum >= world->mNumSndBufs) { \
		int localBufNum = bufnum - world->mNumSndBufs; \
		Graph *parent = unit->mParent; \
		if(localBufNum <= parent->localBufNum) { \
			unit->m_buf = parent->mLocalSndBufs + localBufNum; \
		} else { \
			bufnum = 0; \
			unit->m_buf = world->mSndBufs + bufnum; \
		} \
	} else { \
		unit->m_buf = world->mSndBufs + bufnum; \
	} \
	SndBuf *buf = unit->m_buf; \
	float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufSamples = buf->samples; \
	uint32 bufFrames = buf->frames; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2; \
	double loopMax __attribute__((__unused__)) = (double)bufSamples;

#define CHECK_DELTAP_BUF \
	if ((!bufData) || (bufChannels != 1)) { \
				unit->mDone = true; \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	}


static void DelTapWr_first(DelTapWr *unit, int inNumSamples)
{
	float fbufnum  = IN0(0);
	uint32 bufnum = (uint32)fbufnum;
	float* in = IN(1);
	float* out = OUT(0);

	uint32 phase = unit->m_phase;

	DELTAP_BUF
	CHECK_DELTAP_BUF

	// zero out the buffer!
#ifdef NOVA_SIMD
	if (nova::vec<float>::is_aligned(bufData)) {
		uint32 unroll = bufSamples & (~(nova::vec<float>::size - 1));
		nova::zerovec_simd(bufData, unroll);

		uint32 remain = bufSamples - unroll;
		Clear(remain, bufData + unroll);
	} else
		Clear(bufSamples, bufData);
#else
	Clear(bufSamples, bufData);
#endif

	out[0] = (float)phase;
	bufData[phase] = in[0];
	phase++;
	if(phase == bufSamples)
		phase -= bufSamples;

	unit->m_phase = phase;
}

void DelTapWr_Ctor(DelTapWr *unit)
{
	if (BUFLENGTH & 15)
		SETCALC(DelTapWr_next);
	else
		SETCALC(DelTapWr_next_simd);
	unit->m_phase = 0;
	unit->m_fbufnum = -1e9f;
	DelTapWr_first(unit, 1);
}

template <bool simd>
static inline void DelTapWr_perform(DelTapWr *unit, int inNumSamples)
{
	float fbufnum  = IN0(0);
	uint32 bufnum = (uint32)fbufnum;
	const float* in = ZIN(1);
	float* out = ZOUT(0);
	uint32 * phase_out = (uint32*)out;

	uint32 phase = unit->m_phase;

	DELTAP_BUF
	CHECK_DELTAP_BUF

	LOCK_SNDBUF(buf);
	int buf_remain = (int)(bufSamples - phase);
	if (inNumSamples < buf_remain)
	{
		/* fast-path */
#ifdef NOVA_SIMD
		if (simd)
			nova::copyvec_an_simd(bufData+phase, IN(1), inNumSamples);
		else
#endif
			Copy(inNumSamples, bufData + phase, IN(1));
		LOOP1 (inNumSamples,
			ZXP(phase_out) = phase++;
		)
	} else {
		LOOP1 (inNumSamples,
			bufData[phase] = ZXP(in);
			ZXP(phase_out) = phase++;
			if(phase == bufSamples)
				phase -= bufSamples;
		)
	}

	unit->m_phase = phase;
}

void DelTapWr_next(DelTapWr *unit, int inNumSamples)
{
	DelTapWr_perform<false>(unit, inNumSamples);
}

void DelTapWr_next_simd(DelTapWr *unit, int inNumSamples)
{
	DelTapWr_perform<true>(unit, inNumSamples);
}


#define SETUP_TAPDELK \
	float delTime = unit->m_delTime; \
	float newDelTime = IN0(2) * (float)SAMPLERATE; \
	float delTimeInc = CALCSLOPE(newDelTime, delTime); \
	float * fPhaseIn = IN(1); \
	uint32 * iPhaseIn = (uint32*)fPhaseIn; \
	uint32 phaseIn = *iPhaseIn; \
	float fbufnum  = IN0(0); \
	uint32 bufnum = (uint32)fbufnum; \
	float* out __attribute__((__unused__)) = ZOUT(0); \

#define SETUP_TAPDELA \
	float* delTime = ZIN(2); \
	float * fPhaseIn = IN(1); \
	uint32 * iPhaseIn = (uint32*)fPhaseIn; \
	uint32 phaseIn = *iPhaseIn; \
	float fbufnum  = IN0(0); \
	uint32 bufnum = (uint32)fbufnum; \
	float* out = ZOUT(0); \

void DelTapRd_Ctor(DelTapRd *unit)
{
	unit->m_fbufnum = -1e9f;
	unit->m_delTime = IN0(2) * SAMPLERATE;
	int interp = (int)IN0(3);
	if (INRATE(2) == calc_FullRate) {
		if (interp == 2)
			SETCALC(DelTapRd_next2_a);
		else if (interp == 4)
			SETCALC(DelTapRd_next4_a);
		else
			SETCALC(DelTapRd_next1_a);
	} else {
		if (interp == 2)
			SETCALC(DelTapRd_next2_k);
		else if (interp == 4)
			SETCALC(DelTapRd_next4_k);
		else
			if (BUFLENGTH & 15)
				SETCALC(DelTapRd_next1_k);
			else {
				SETCALC(DelTapRd_next1_k_simd);
				DelTapRd_next1_k(unit, 1);
				return;
			}
	}
	(unit->mCalcFunc)(unit, 1);
}


void DelTapRd_next1_a(DelTapRd *unit, int inNumSamples)
{
	SETUP_TAPDELA
	DELTAP_BUF
	CHECK_DELTAP_BUF

	LOCK_SNDBUF_SHARED(buf);
	LOOP1(inNumSamples,
		double curDelTimeSamps = ZXP(delTime) * SAMPLERATE;
		double phase = phaseIn - curDelTimeSamps;
		if(phase < 0.) phase += loopMax;
		if(phase >=loopMax) phase -= loopMax;
		int32 iphase = (int32)phase;
		ZXP(out) = bufData[iphase];
		phaseIn += 1.;
	)
}

template <bool simd>
inline void DelTapRd_perform1_k(DelTapRd *unit, int inNumSamples)
{
	SETUP_TAPDELK
	DELTAP_BUF
	CHECK_DELTAP_BUF
	float * zout = ZOUT(0);

	LOCK_SNDBUF_SHARED(buf);
	if (delTime == newDelTime)
	{
		double phase = (double)phaseIn - delTime;
		int32 iphase = (int32)phase;
		if ( (iphase >= 0) // lower bound
			&& iphase + inNumSamples < (bufSamples - 1)) //upper bound
			{
#ifdef NOVA_SIMD
				if (simd)
					nova::copyvec_na_simd(OUT(0), bufData + iphase, inNumSamples);
				else
#endif
					Copy(inNumSamples, OUT(0), bufData + iphase);
			}
		else
			LOOP1(inNumSamples,
				if(iphase < 0) iphase += bufSamples;
				if(iphase >= bufSamples) iphase -= bufSamples;
				ZXP(zout) = bufData[iphase];
				++iphase;
			)
	} else {
		LOOP1(inNumSamples,
			double phase = (double)phaseIn - delTime;
			if(phase < 0.) phase += loopMax;
			if(phase >=loopMax) phase -= loopMax;
			int32 iphase = (int32)phase;
			ZXP(zout) = bufData[iphase];
			delTime += delTimeInc;
			++phaseIn;
		)
		unit->m_delTime = delTime;
	}
}

void DelTapRd_next1_k(DelTapRd *unit, int inNumSamples)
{
	DelTapRd_perform1_k<false>(unit, inNumSamples);
}

void DelTapRd_next1_k_simd(DelTapRd *unit, int inNumSamples)
{
	DelTapRd_perform1_k<true>(unit, inNumSamples);
}

void DelTapRd_next2_k(DelTapRd *unit, int inNumSamples)
{
	SETUP_TAPDELK
	DELTAP_BUF
	CHECK_DELTAP_BUF

	int32 iloopMax = (int32)bufSamples;

	LOCK_SNDBUF_SHARED(buf);

	if (delTime == newDelTime)
	{
		double phase = (double)phaseIn - delTime;
		double dphase;
		float fracphase = std::modf(phase, &dphase);
		int32 iphase = (int32)dphase;

		if ( (phase >= 0) // lower bound
			&& phase + inNumSamples < (loopMax - 2)) //upper bound
		{
			LOOP1(inNumSamples,
				int32 iphase1 = iphase + 1;
				float b = bufData[iphase];
				float c = bufData[iphase1];
				ZXP(out) = (b + fracphase * (c - b));
				iphase += 1;
			);
		} else {
			LOOP1(inNumSamples,
				if(iphase < 0) iphase += iloopMax;
				else if(iphase >= bufSamples) phase -= iloopMax;
				int32 iphase1 = iphase + 1;
				if(iphase1 >= iloopMax) iphase1 -= iloopMax;
				float b = bufData[iphase];
				float c = bufData[iphase1];
				ZXP(out) = (b + fracphase * (c - b));
				++iphase;
			);
		}
	} else {
		LOOP1(inNumSamples,
			double phase = (double)phaseIn - delTime;
			if(phase < 0.) phase += loopMax;
			if(phase >= loopMax) phase -= loopMax;
			int32 iphase = (int32)phase;
			int32 iphase1 = iphase + 1;
			if(iphase1 >= iloopMax) iphase1 -= iloopMax;
			float fracphase = phase - (double)iphase;
			float b = bufData[iphase];
			float c = bufData[iphase1];
			ZXP(out) = (b + fracphase * (c - b));
			delTime += delTimeInc;
			++phaseIn;
		);
		unit->m_delTime = delTime;
	}
}

void DelTapRd_next2_a(DelTapRd *unit, int inNumSamples)
{
	SETUP_TAPDELA
	DELTAP_BUF
	CHECK_DELTAP_BUF

	int32 iloopMax = (int32)bufSamples;

	LOCK_SNDBUF_SHARED(buf);
	LOOP1(inNumSamples,
		double curDelTimeSamps = ZXP(delTime) * SAMPLERATE;
		double phase = (double)phaseIn - curDelTimeSamps;
		if(phase < 0.) phase += loopMax;
		if(phase >= loopMax) phase -= loopMax;
		int32 iphase = (int32)phase;
		int32 iphase1 = iphase + 1;
		if(iphase1 >= iloopMax) iphase1 -= iloopMax;
		float fracphase = phase - (double)iphase;
		float b = bufData[iphase];
		float c = bufData[iphase1];
		ZXP(out) = (b + fracphase * (c - b));
		++phaseIn;
	);
}

void DelTapRd_next4_k(DelTapRd *unit, int inNumSamples)
{
	SETUP_TAPDELK
	DELTAP_BUF
	CHECK_DELTAP_BUF

	int32 iloopMax = (int32)loopMax;

	LOCK_SNDBUF_SHARED(buf);


	if (delTime == newDelTime)
	{
		double phase = (double)phaseIn - delTime;
		double dphase;
		float fracphase = std::modf(phase, &dphase);
		int32 iphase = (int32)dphase;

		if ( (iphase >= 1) // lower bound
			&& iphase + inNumSamples < (iloopMax - 4)) //upper bound
		{
			LOOP1(inNumSamples,
				int32 iphase0 = iphase - 1;
				int32 iphase1 = iphase + 1;
				int32 iphase2 = iphase + 2;

				float a = bufData[iphase0];
				float b = bufData[iphase];
				float c = bufData[iphase1];
				float d = bufData[iphase2];
				ZXP(out) = cubicinterp(fracphase, a, b, c, d);
				++iphase;
			);
		} else {
			LOOP1(inNumSamples,
				if(iphase < 0) iphase += iloopMax;
				else if(iphase >= iloopMax) iphase -= iloopMax;
				int32 iphase0 = iphase - 1;
				int32 iphase1 = iphase + 1;
				int32 iphase2 = iphase + 2;

				if(iphase0 < 0) iphase0 += iloopMax;
				if(iphase1 > iloopMax) iphase1 -=iloopMax;
				if(iphase2 > iloopMax) iphase2 -=iloopMax;

				float a = bufData[iphase0];
				float b = bufData[iphase];
				float c = bufData[iphase1];
				float d = bufData[iphase2];
				ZXP(out) = cubicinterp(fracphase, a, b, c, d);
				++iphase;
			);
		}
	} else {
		LOOP1(inNumSamples,
			double phase = (double)phaseIn - delTime;
			double dphase;
			float fracphase = std::modf(phase, &dphase);
			int32 iphase = (int32)dphase;

			if(iphase < 0.) iphase += iloopMax;
			if(iphase >= iloopMax) iphase -= iloopMax;
			int32 iphase0 = iphase - 1;
			int32 iphase1 = iphase + 1;
			int32 iphase2 = iphase + 2;

			if(iphase0 < 0) iphase0 += iloopMax;
			if(iphase1 > iloopMax) iphase1 -=iloopMax;
			if(iphase2 > iloopMax) iphase2 -=iloopMax;

			float a = bufData[iphase0];
			float b = bufData[iphase];
			float c = bufData[iphase1];
			float d = bufData[iphase2];
			ZXP(out) = cubicinterp(fracphase, a, b, c, d);
			delTime += delTimeInc;
			++phaseIn;
		);
		unit->m_delTime = delTime;
	}
}

void DelTapRd_next4_a(DelTapRd *unit, int inNumSamples)
{
	SETUP_TAPDELA
	DELTAP_BUF
	CHECK_DELTAP_BUF

	int32 iloopMax = (int32)loopMax;

	LOCK_SNDBUF_SHARED(buf);
	LOOP1(inNumSamples,
		double curDelTimeSamps = ZXP(delTime) * SAMPLERATE;
		double phase = (double)phaseIn - curDelTimeSamps;
		if(phase < 0.) phase += loopMax;
		if(phase >= loopMax) phase -= loopMax;
		int32 iphase = (int32)phase;
		int32 iphase0 = iphase - 1;
		int32 iphase1 = iphase + 1;
		int32 iphase2 = iphase + 2;

		if(iphase0 < 0) iphase0 += iloopMax;
		if(iphase1 > iloopMax) iphase1 -=iloopMax;
		if(iphase2 > iloopMax) iphase2 -=iloopMax;

		float fracphase = phase - (double)iphase;
		float a = bufData[iphase0];
		float b = bufData[iphase];
		float c = bufData[iphase1];
		float d = bufData[iphase2];
		ZXP(out) = cubicinterp(fracphase, a, b, c, d);
		++phaseIn;
	);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(Delay)
{
	ft = inTable;

#define DefineInfoUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(Unit), (UnitCtorFunc)&name##_Ctor, 0, 0);

	DefineInfoUnit(ControlRate);
	DefineInfoUnit(SampleRate);
	DefineInfoUnit(SampleDur);
	DefineInfoUnit(ControlDur);
	DefineInfoUnit(SubsampleOffset);
	DefineInfoUnit(RadiansPerSample);
	DefineInfoUnit(BlockSize);
	DefineInfoUnit(NumInputBuses);
	DefineInfoUnit(NumOutputBuses);
	DefineInfoUnit(NumAudioBuses);
	DefineInfoUnit(NumControlBuses);
	DefineInfoUnit(NumBuffers);
	DefineInfoUnit(NumRunningSynths);
	DefineInfoUnit(NodeID);

#define DefineBufInfoUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(BufInfoUnit), (UnitCtorFunc)&name##_Ctor, 0, 0);

	DefineBufInfoUnit(BufSampleRate);
	DefineBufInfoUnit(BufRateScale);
	DefineBufInfoUnit(BufSamples);
	DefineBufInfoUnit(BufFrames);
	DefineBufInfoUnit(BufChannels);
	DefineBufInfoUnit(BufDur);

	DefineSimpleCantAliasUnit(PlayBuf);
#if NOTYET
	DefineSimpleUnit(SimpleLoopBuf);
#endif
	DefineDtorUnit(RecordBuf);
	DefineSimpleUnit(BufRd);
	DefineSimpleUnit(BufWr);
	DefineDtorUnit(Pitch);

	DefineSimpleUnit(BufDelayN);
	DefineSimpleUnit(BufDelayL);
	DefineSimpleUnit(BufDelayC);
	DefineSimpleUnit(BufCombN);
	DefineSimpleUnit(BufCombL);
	DefineSimpleUnit(BufCombC);
	DefineSimpleUnit(BufAllpassN);
	DefineSimpleUnit(BufAllpassL);
	DefineSimpleUnit(BufAllpassC);

#define DefineDelayUnit(name) \
	(*ft->fDefineUnit)(#name, sizeof(name), (UnitCtorFunc)&name##_Ctor, \
	(UnitDtorFunc)&DelayUnit_Dtor, 0);

	DefineDelayUnit(DelayN);
	DefineDelayUnit(DelayL);
	DefineDelayUnit(DelayC);
	DefineDelayUnit(CombN);
	DefineDelayUnit(CombL);
	DefineDelayUnit(CombC);
	DefineDelayUnit(AllpassN);
	DefineDelayUnit(AllpassL);
	DefineDelayUnit(AllpassC);

	DefineDtorUnit(PitchShift);
	DefineSimpleUnit(GrainTap);
	DefineSimpleCantAliasUnit(TGrains);
	DefineDtorUnit(ScopeOut);
	DefineDtorUnit(ScopeOut2);
	DefineDelayUnit(Pluck);

	DefineSimpleUnit(DelTapWr);
	DefineSimpleUnit(DelTapRd);

	DefineDtorUnit(LocalBuf);
	DefineSimpleUnit(MaxLocalBufs);
	DefineSimpleUnit(SetBuf);
	DefineSimpleUnit(ClearBuf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
