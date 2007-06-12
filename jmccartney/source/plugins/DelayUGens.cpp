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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/


#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct PlayBuf : public Unit
{
	double m_phase;
	float m_prevtrig;
	float m_fbufnum;
	SndBuf *m_buf;
};

struct BufRd : public Unit
{
	float m_fbufnum;
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
};

struct Pitch : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;

	float m_values[10];
	int m_ages[10];
	float m_dsamp, m_x1, m_y1;
	float m_delaytime;
	int m_iwrphase;
	int m_numoutput;
	
	float m_freq, m_minfreq, m_maxfreq, m_hasfreq, m_srate, m_ampthresh, m_peakthresh;
	int m_maxperiod, m_execPeriod, m_index, m_readp, m_size;
	int m_downsamp, m_maxlog2bins, m_medianSize;
	int m_state;
};

struct BufDelayUnit : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
	float m_dsamp, m_x1, m_y1;
	float m_delaytime;
	long m_iwrphase;
	long m_numoutput;
};

struct BufDelayN : public BufDelayUnit
{
};

struct BufDelayL : public BufDelayUnit
{
};

struct BufDelayC : public BufDelayUnit
{
};

struct BufFeedbackDelay : public BufDelayUnit
{
	float m_feedbk, m_decaytime;
};

struct BufCombN : public BufFeedbackDelay
{
};

struct BufCombL : public BufFeedbackDelay
{
};

struct BufCombC : public BufFeedbackDelay
{
};

struct BufAllpassN : public BufFeedbackDelay
{
};

struct BufAllpassL : public BufFeedbackDelay
{
};

struct BufAllpassC : public BufFeedbackDelay
{
};


struct DelayUnit : public Unit
{
	float *m_dlybuf;
	SndBuf *m_buf;

	float m_dsamp, m_fdelaylen, m_x1, m_y1;
	float m_delaytime, m_maxdelaytime;
	long m_iwrphase, m_idelaylen, m_mask;
	long m_numoutput;
};

struct DelayN : public DelayUnit
{
};

struct DelayL : public DelayUnit
{
};

struct DelayC : public DelayUnit
{
};

struct FeedbackDelay : public DelayUnit
{
	float m_feedbk, m_decaytime;
};

struct CombN : public FeedbackDelay
{
};

struct CombL : public FeedbackDelay
{
};

struct CombC : public FeedbackDelay
{
};

struct AllpassN : public FeedbackDelay
{
};

struct AllpassL : public FeedbackDelay
{
};

struct AllpassC : public FeedbackDelay
{
};


//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

	void PlayBuf_next_aa(PlayBuf *unit, int inNumSamples);
	void PlayBuf_next_ak(PlayBuf *unit, int inNumSamples);
	void PlayBuf_next_ka(PlayBuf *unit, int inNumSamples);
	void PlayBuf_next_kk(PlayBuf *unit, int inNumSamples);
	void PlayBuf_Ctor(PlayBuf* unit);

	void BufRd_Ctor(BufRd *unit);
	void BufRd_next_4(BufRd *unit, int inNumSamples);
	void BufRd_next_2(BufRd *unit, int inNumSamples);
	void BufRd_next_1(BufRd *unit, int inNumSamples);

	void BufWr_Ctor(BufWr *unit);
	void BufWr_next(BufWr *unit, int inNumSamples);

	void RecordBuf_Ctor(RecordBuf *unit);
	void RecordBuf_next(RecordBuf *unit, int inNumSamples);
	void RecordBuf_next_10(RecordBuf *unit, int inNumSamples);

	void Pitch_Ctor(Pitch *unit);
	void Pitch_next(Pitch *unit, int inNumSamples);
	
	void BufDelayUnit_Reset(BufDelayUnit *unit);
	void BufFeedbackDelay_Reset(BufFeedbackDelay *unit);
	
	void BufDelayN_Ctor(BufDelayN *unit);
	void BufDelayN_next(BufDelayN *unit, int inNumSamples);
	void BufDelayN_next_z(BufDelayN *unit, int inNumSamples);
	
	void BufDelayL_Ctor(BufDelayL *unit);
	void BufDelayL_next(BufDelayL *unit, int inNumSamples);
	void BufDelayL_next_z(BufDelayL *unit, int inNumSamples);
	
	void BufDelayC_Ctor(BufDelayC *unit);
	void BufDelayC_next(BufDelayC *unit, int inNumSamples);
	void BufDelayC_next_z(BufDelayC *unit, int inNumSamples);
	
	void BufCombN_Ctor(BufCombN *unit);
	void BufCombN_next(BufCombN *unit, int inNumSamples);
	void BufCombN_next_z(BufCombN *unit, int inNumSamples);
	
	void BufCombL_Ctor(BufCombL *unit);
	void BufCombL_next(BufCombL *unit, int inNumSamples);
	void BufCombL_next_z(BufCombL *unit, int inNumSamples);
	
	void BufCombC_Ctor(BufCombC *unit);
	void BufCombC_next(BufCombC *unit, int inNumSamples);
	void BufCombC_next_z(BufCombC *unit, int inNumSamples);
	
	void BufAllpassN_Ctor(BufAllpassN *unit);
	void BufAllpassN_next(BufAllpassN *unit, int inNumSamples);
	void BufAllpassN_next_z(BufAllpassN *unit, int inNumSamples);
	
	void BufAllpassL_Ctor(BufAllpassL *unit);
	void BufAllpassL_next(BufAllpassL *unit, int inNumSamples);
	void BufAllpassL_next_z(BufAllpassL *unit, int inNumSamples);
	
	void BufAllpassC_Ctor(BufAllpassC *unit);
	void BufAllpassC_next(BufAllpassC *unit, int inNumSamples);
	void BufAllpassC_next_z(BufAllpassC *unit, int inNumSamples);

	void DelayUnit_Reset(DelayUnit *unit);
	void DelayUnit_Dtor(DelayUnit *unit);
	void DelayUnit_AllocDelayLine(DelayUnit *unit);
	void FeedbackDelay_Reset(FeedbackDelay *unit);
	
	void DelayN_Ctor(DelayN *unit);
	void DelayN_next(DelayN *unit, int inNumSamples);
	void DelayN_next_z(DelayN *unit, int inNumSamples);
	
	void DelayL_Ctor(DelayL *unit);
	void DelayL_next(DelayL *unit, int inNumSamples);
	void DelayL_next_z(DelayL *unit, int inNumSamples);
	
	void DelayC_Ctor(DelayC *unit);
	void DelayC_next(DelayC *unit, int inNumSamples);
	void DelayC_next_z(DelayC *unit, int inNumSamples);
	
	void CombN_Ctor(CombN *unit);
	void CombN_next(CombN *unit, int inNumSamples);
	void CombN_next_z(CombN *unit, int inNumSamples);
	
	void CombL_Ctor(CombL *unit);
	void CombL_next(CombL *unit, int inNumSamples);
	void CombL_next_z(CombL *unit, int inNumSamples);
	
	void CombC_Ctor(CombC *unit);
	void CombC_next(CombC *unit, int inNumSamples);
	void CombC_next_z(CombC *unit, int inNumSamples);
	
	void AllpassN_Ctor(AllpassN *unit);
	void AllpassN_next(AllpassN *unit, int inNumSamples);
	void AllpassN_next_z(AllpassN *unit, int inNumSamples);
	
	void AllpassL_Ctor(AllpassL *unit);
	void AllpassL_next(AllpassL *unit, int inNumSamples);
	void AllpassL_next_z(AllpassL *unit, int inNumSamples);
	
	void AllpassC_Ctor(AllpassC *unit);
	void AllpassC_next(AllpassC *unit, int inNumSamples);
	void AllpassC_next_z(AllpassC *unit, int inNumSamples);

}

//////////////////////////////////////////////////////////////////////////////////////////////////

static float cubicinterp(float x, float y0, float y1, float y2, float y3)
{
	// 4-point, 3rd-order Hermite (x-form)
	float c0 = y1;
	float c1 = 0.5f * (y2 - y0);
	float c2 = y0 - 2.5f * y1 + 2.f * y2 - 0.5f * y3;
	float c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);

	return ((c3 * x + c2) * x + c1) * x + c0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

inline float CalcFeedback(float delaytime, float decaytime)
{
	if (delaytime == 0.f) {
		return 0.f;
	} else if (decaytime > 0.f) {
		return exp(log001 * delaytime / decaytime);
	} else if (decaytime < 0.f) {
		return -exp(log001 * delaytime / -decaytime);
	} else {
		return 0.f;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double sc_loop(double in, double hi, int loop) 
{
	// avoid the divide if possible
	if (in >= hi) {
		if (!loop) return hi;
		in -= hi;
		if (in < hi) return in;
	} else if (in < 0.) {
		if (!loop) return 0.;
		in += hi;
		if (in >= 0.) return in;
	} else return in;
	
	return in - hi * floor(in/hi); 
}

#define GET_BUF \
	float fbufnum  = ZIN0(0); \
	if (fbufnum != unit->m_fbufnum) { \
		int bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum < 0 || bufnum >= world->mNumSndBufs) bufnum = 0; \
		unit->m_fbufnum = fbufnum; \
		unit->m_buf = world->mSndBufs + bufnum; \
	} \
	SndBuf *buf = unit->m_buf; \
	int bufChannels = buf->channels; \
	int bufSamples = buf->samples; \
	int bufFrames = buf->frames; \
	int mask = buf->mask; \
	int guardFrame = bufFrames - 2; \
	float *bufData = buf->data;

#define SETUP_OUT \
	if (unit->mNumOutputs != bufChannels) { \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *out[16]; \
	for (int i=0; i<bufChannels; ++i) out[i] = ZOUT(i); 

#define SETUP_IN(offset) \
	if ((unit->mNumInputs - offset) != bufChannels) { \
		ClearUnitOutputs(unit, inNumSamples); \
		return; \
	} \
	float *in[16]; \
	for (int i=0; i<bufChannels; ++i) in[i] = ZIN(i+offset); 
		

#define LOOP_BODY_4 \
		phase = sc_loop(phase, bufFrames, loop); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase * bufChannels; \
		float* table0 = table1 - bufChannels; \
		float* table2 = table1 + bufChannels; \
		float* table3 = table2 + bufChannels; \
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
		int32 index = 0; \
		float fracphase = phase - (double)iphase; \
		for (int i=0; i<bufChannels; ++i) { \
			float a = table0[index]; \
			float b = table1[index]; \
			float c = table2[index]; \
			float d = table3[index]; \
			*++(out[i]) = cubicinterp(fracphase, a, b, c, d); \
			index++; \
		}

#define LOOP_BODY_2 \
		phase = sc_loop(phase, bufFrames, loop); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase * bufChannels; \
		float* table2 = table1 + bufChannels; \
		if (iphase > guardFrame) { \
			table2 -= bufSamples; \
		} \
		int32 index = 0; \
		float fracphase = phase - (double)iphase; \
		for (int i=0; i<bufChannels; ++i) { \
			float b = table1[index]; \
			float c = table2[index]; \
			*++(out[i]) = b + fracphase * (c - b); \
			index++; \
		}

#define LOOP_BODY_1 \
		phase = sc_loop(phase, bufFrames, loop); \
		int32 iphase = (int32)phase; \
		float* table1 = bufData + iphase * bufChannels; \
		int32 index = 0; \
		for (int i=0; i<bufChannels; ++i) { \
			*++(out[i]) = table1[index++]; \
		}


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
	
	unit->m_fbufnum = -1.;
	unit->m_prevtrig = 0.;
	unit->m_phase = ZIN0(3);
	
	ClearUnitOutputs(unit, 1);
}

void PlayBuf_next_aa(PlayBuf *unit, int inNumSamples)
{
	float *ratein  = ZIN(1);
	float *trigin  = ZIN(2);
	int32 loop     = (int32)ZIN0(4);
	
	GET_BUF
	SETUP_OUT	

	double phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	for (int i=0; i<inNumSamples; ++i) {
		float trig = ZXP(trigin);
		if (trig > 0.f && prevtrig <= 0.f) {
			phase = ZIN0(3);
		}
		prevtrig = trig;
		
		LOOP_BODY_4
		
		phase += ZXP(ratein);
	}
	unit->m_phase = phase;
	unit->m_prevtrig = prevtrig;
}

void PlayBuf_next_ak(PlayBuf *unit, int inNumSamples)
{
	float *ratein  = ZIN(1);
	float trig     = ZIN0(2);
	int32 loop     = (int32)ZIN0(4);
	
	GET_BUF
	SETUP_OUT	
	
	double phase = unit->m_phase;
	if (trig > 0.f && unit->m_prevtrig <= 0.f) {
		phase = ZIN0(3);
	}
	unit->m_prevtrig = trig;
	for (int i=0; i<inNumSamples; ++i) {
		
		LOOP_BODY_4
		
		phase += ZXP(ratein);
	}
	unit->m_phase = phase;
}

void PlayBuf_next_kk(PlayBuf *unit, int inNumSamples)
{
	float rate     = ZIN0(1);
	float trig     = ZIN0(2);
	int32 loop     = (int32)ZIN0(4);
	
	GET_BUF
	SETUP_OUT	
	
	double phase = unit->m_phase;
	if (trig > 0.f && unit->m_prevtrig <= 0.f) {
		phase = ZIN0(3);
	}
	unit->m_prevtrig = trig;
	for (int i=0; i<inNumSamples; ++i) {
		
		LOOP_BODY_4
		
		phase += rate;
	}
	unit->m_phase = phase;
}

void PlayBuf_next_ka(PlayBuf *unit, int inNumSamples)
{
	float rate     = ZIN0(1);
	float *trigin  = ZIN(2);
	int32 loop     = (int32)ZIN0(4);
	
	GET_BUF
	SETUP_OUT	
	
	double phase = unit->m_phase;
	float prevtrig = unit->m_prevtrig;
	for (int i=0; i<inNumSamples; ++i) {
		float trig = ZXP(trigin);
		if (trig > 0.f && prevtrig <= 0.f) {
			if (INRATE(3) == calc_FullRate) phase = IN(3)[i];
			else phase = ZIN0(3);
		}
		prevtrig = trig;
		
		LOOP_BODY_4
		
		phase += rate;
	}
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
	
	unit->m_fbufnum = -1.;
	
	ClearUnitOutputs(unit, 1);
}

void BufRd_next_4(BufRd *unit, int inNumSamples)
{
	float *phasein = ZIN(1);
	int32 loop     = (int32)ZIN0(2);
	
	GET_BUF
	SETUP_OUT
		
	for (int i=0; i<inNumSamples; ++i) {
		double phase = ZXP(phasein);
		
		LOOP_BODY_4
		
	}
}

void BufRd_next_2(BufRd *unit, int inNumSamples)
{
	float *phasein = ZIN(1);
	int32 loop     = (int32)ZIN0(2);
	
	GET_BUF
	SETUP_OUT
		
	for (int i=0; i<inNumSamples; ++i) {
		double phase = ZXP(phasein);
		
		LOOP_BODY_2
		
	}
}

void BufRd_next_1(BufRd *unit, int inNumSamples)
{
	float *phasein = ZIN(1);
	int32 loop     = (int32)ZIN0(2);
	
	GET_BUF
	SETUP_OUT
		
	for (int i=0; i<inNumSamples; ++i) {
		double phase = ZXP(phasein);
		
		LOOP_BODY_1
		
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufWr_Ctor(BufWr *unit)
{	
	SETCALC(BufWr_next);
	
	unit->m_fbufnum = -1.;
	
	ClearUnitOutputs(unit, 1);
}

void BufWr_next(BufWr *unit, int inNumSamples)
{
	float *phasein  = ZIN(1);
	int32 loop     = (int32)ZIN0(2);
	
	GET_BUF
	SETUP_IN(3)

	for (int32 k=0; k<inNumSamples; ++k) { 
		double phase = sc_loop(ZXP(phasein), bufFrames, loop);
		int32 iphase = (int32)phase;
		float* table0 = bufData + iphase * bufChannels;
		for (int i=0; i<bufChannels; ++i) {
			table0[i] = *++(in[i]);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void RecordBuf_Ctor(RecordBuf *unit)
{	
	SETCALC(RecordBuf_next);
	
	unit->m_fbufnum = -1.;
	unit->m_recLevel = ZIN0(1);
	unit->m_preLevel = ZIN0(2);
	unit->m_writepos = 0;
		
	ClearUnitOutputs(unit, 1);
}

void RecordBuf_next(RecordBuf *unit, int inNumSamples)
{	
	GET_BUF
	SETUP_IN(5)

	float recLevel = ZIN0(1);
	float preLevel = ZIN0(2);
	float run      = ZIN0(3);
	int32 loop     = (int32)ZIN0(4);

	int32 writepos = unit->m_writepos;
	
	float recLevel_slope = CALCSLOPE(recLevel, unit->m_recLevel);
	float preLevel_slope = CALCSLOPE(preLevel, unit->m_preLevel);
	
	if (loop) {
		if (writepos < 0) writepos = bufSamples - bufChannels;
		else if (writepos >= bufSamples) writepos = 0;
		if (run > 0.f) {
			for (int32 k=0; k<inNumSamples; ++k) { 
				float* table0 = bufData + writepos;
				for (int i=0; i<bufChannels; ++i) {
					float *buf = table0 + i;
					*buf = *++(in[i]) * recLevel + *buf * preLevel;
				}
				writepos += bufChannels;
				if (writepos >= bufSamples) writepos = 0;
				
				recLevel += recLevel_slope;
				preLevel += preLevel_slope;					
			}
		} else if (run < 0.f) {
			for (int32 k=0; k<inNumSamples; ++k) { 
				float* table0 = bufData + writepos;
				for (int i=0; i<bufChannels; ++i) {
					float *buf = table0 + i;
					*buf = *++(in[i]) * recLevel + *buf * preLevel;
				}
				writepos -= bufChannels;
				if (writepos < 0) writepos = bufSamples - bufChannels;
				
				recLevel += recLevel_slope;
				preLevel += preLevel_slope;
			}
		}
	} else {
		if (run > 0.f) {
			for (int32 k=0; k<inNumSamples; ++k) { 
				if (writepos >= 0 && writepos < bufSamples) {
					float* table0 = bufData + writepos;
					for (int i=0; i<bufChannels; ++i) {
						float *buf = table0 + i;
						*buf = *++(in[i]) * recLevel + *buf * preLevel;
					}
					writepos += bufChannels;
				}
				
				recLevel += recLevel_slope;
				preLevel += preLevel_slope;					
			}
		} else if (run < 0.f) {
			for (int32 k=0; k<inNumSamples; ++k) { 
				if (writepos >= 0 && writepos < bufSamples) {
					float* table0 = bufData + writepos;
					for (int i=0; i<bufChannels; ++i) {
						float *buf = table0 + i;
						*buf = *++(in[i]) * recLevel + *buf * preLevel;
					}
					writepos -= bufChannels;
				}
				
				recLevel += recLevel_slope;
				preLevel += preLevel_slope;
			}
		}
	}
	unit->m_writepos = writepos;
	unit->m_recLevel = recLevel;
	unit->m_preLevel = preLevel;
}

void RecordBuf_next_10(RecordBuf *unit, int inNumSamples)
{	
	GET_BUF
	SETUP_IN(5)

	float run      = ZIN0(3);
	int32 loop     = (int32)ZIN0(4);

	int32 writepos = unit->m_writepos;
	
	if (loop) {
		if (writepos < 0) writepos = bufSamples - bufChannels;
		else if (writepos >= bufSamples) writepos = 0;
		if (run > 0.f) {
			for (int32 k=0; k<inNumSamples; ++k) { 
				float* table0 = bufData + writepos;
				for (int i=0; i<bufChannels; ++i) {
					float *buf = table0 + i;
					*buf = *++(in[i]);
				}
				writepos += bufChannels;
				if (writepos >= bufSamples) writepos = 0;
			}
		} else if (run < 0.f) {
			for (int32 k=0; k<inNumSamples; ++k) { 
				float* table0 = bufData + writepos;
				for (int i=0; i<bufChannels; ++i) {
					float *buf = table0 + i;
					*buf = *++(in[i]);
				}
				writepos -= bufChannels;
				if (writepos < 0) writepos = bufSamples - bufChannels;
			}
		}
	} else {
		if (run > 0.f) {
			for (int32 k=0; k<inNumSamples; ++k) { 
				if (writepos >= 0 && writepos < bufSamples) {
					float* table0 = bufData + writepos;
					for (int i=0; i<bufChannels; ++i) {
						float *buf = table0 + i;
						*buf = *++(in[i]);
					}
					writepos += bufChannels;
				}
			}
		} else if (run < 0.f) {
			for (int32 k=0; k<inNumSamples; ++k) { 
				if (writepos >= 0 && writepos < bufSamples) {
					float* table0 = bufData + writepos;
					for (int i=0; i<bufChannels; ++i) {
						float *buf = table0 + i;
						*buf = *++(in[i]);
					}
					writepos -= bufChannels;
				}
			}
		}
	}
	unit->m_writepos = writepos;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////


float insertMedian(float* values, int* ages, int size, float value);
void initMedian(float* values, int* ages, int size, float value);


float insertMedian(float* values, int* ages, int size, float value);
float insertMedian(float* values, int* ages, int size, float value)
{
	int i, last, pos=-1;
	
	// keeps a sorted list of the previous n=size values
	// the oldest is removed and the newest is inserted.
	// values between the oldest and the newest are shifted over by one.
	
	// values and ages are both arrays that are 'size' int.
	// the median value is always values[size>>1]
	
	last = size - 1;
	// find oldest bin and age the other bins.
	for (i=0; i<size; ++i) {
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

void initMedian(float* values, int* ages, int size, float value);
void initMedian(float* values, int* ages, int size, float value)
{
	int i;

	// initialize the arrays with the first value
	for (i=0; i<size; ++i) {
		values[i] = value;
		ages[i] = i;
	}
}

#if 0
void Pitch_AllocDelayLine(Pitch *unit);
void Pitch_AllocDelayLine(Pitch *unit)
{
	int delaybufsize = (int)ceil(unit->m_maxdelaytime * SAMPLERATE * 3.f + 3.f);
	delaybufsize = delaybufsize + BUFLENGTH;
	delaybufsize = NEXTPOWEROFTWO(delaybufsize);  // round up to next power of two
	unit->m_fdelaylen = unit->m_idelaylen = delaybufsize;
	
	RTFree(unit->mWorld, unit->m_dlybuf);
	unit->m_dlybuf = (float*)RTAlloc(unit->mWorld, delaybufsize * sizeof(float));
	unit->m_mask = delaybufsize - 1;
}
#endif

enum {
	kPitchBuffer,
	kPitchIn,
	kPitchInitFreq,
	kPitchMinFreq,
	kPitchMaxFreq,
	kPitchExecFreq,
	kPitchMaxBins,
	kPitchMedian,
	kPitchAmpThreshold,
	kPitchPeakThreshold,
	kPitchDownsamp
};

void Pitch_Ctor(Pitch *unit)
{
	SETCALC(Pitch_next);
//	unit->m_maxdelaytime = ZIN0(1);
//	Pitch_AllocDelayLine(unit);

	unit->m_freq = ZIN0(kPitchInitFreq);
	unit->m_minfreq = ZIN0(kPitchMinFreq);
	unit->m_maxfreq = ZIN0(kPitchMaxFreq);

	float execfreq = ZIN0(kPitchExecFreq);
	execfreq = sc_clip(execfreq, unit->m_minfreq, unit->m_maxfreq);

	int maxbins = (int)ZIN0(kPitchMaxBins);
	unit->m_maxlog2bins = LOG2CEIL(maxbins);

	unit->m_medianSize = (int)ZIN0(kPitchMedian);
	unit->m_ampthresh = ZIN0(kPitchAmpThreshold);
	unit->m_peakthresh = ZIN0(kPitchPeakThreshold);
	
	int downsamp = (int)ZIN0(kPitchDownsamp);
	unit->m_downsamp = sc_clip(downsamp, 1, BUFLENGTH);


    unit->m_srate = SAMPLERATE / (float)unit->m_downsamp;
    
    unit->m_maxperiod = (long)(unit->m_srate / unit->m_minfreq);

	unit->m_execPeriod = (int)(unit->m_srate / execfreq);
	unit->m_execPeriod = sc_max(unit->m_execPeriod, BUFLENGTH);
	
	unit->m_size = unit->m_maxperiod << 1;
	
	unit->m_numoutput = 0;
	unit->m_index = 0;
	unit->m_readp = 0;
	unit->m_hasfreq = 0.f;
	
	ZOUT0(0) = 0.f;
	ZOUT0(1) = 0.f;
}

void Pitch_next(Pitch *unit, int inNumSamples)
{
	// force in to arate.
	//in = ARate((UGenx_rec*)raw_ptr, 0, 0);
	bool foundPeak;
	
	GET_BUF
	float* in = ZIN(1);
	int size = unit->m_size;
	int index = unit->m_index;
	int downsamp = unit->m_downsamp;
	int readp = unit->m_readp;
	int ksamps = BUFLENGTH;

	if (bufSamples < size) {
		ClearUnitOutputs(unit, 1);
		return;
	}
	
	float freq = unit->m_freq;
	float hasfreq = unit->m_hasfreq;
	//postbuf("> %d %d %d %d\n", index, size, readp, downsamp);
	do {
		bufData[index++] = in[readp];
		readp += downsamp;
		
		if (index >= size) {
			float ampthresh = unit->m_ampthresh;
			bool ampok = false;
			
			hasfreq = 0.f; // assume failure
			
			int maxperiod = unit->m_maxperiod;
			
			// check for amp threshold
			for (int j = 0; j < maxperiod; ++j) {	
				if (fabs(bufData[j]) >= ampthresh) {
					ampok = true;
					break;
				}
			}
			
			// if amplitude is too small then don't even look for pitch
			float ampsum;
			if (ampok) {
				int maxlog2bins = unit->m_maxlog2bins;
				int octave;
				// calculate the zero lag value and compute the threshold based on that
				float threshold = 0.f;
				for (int j = 0; j < maxperiod; ++j) {	
					threshold += bufData[j] * bufData[j];
				}
				threshold *= unit->m_peakthresh;
				
				// skip until drop below threshold
				int binstep;
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
				int period;
				//postbuf("startperiod %d\n", startperiod);
				
				// find the first peak
				float maxsum = threshold;
				foundPeak = false;
				for (i = startperiod; i <= maxperiod; i += binstep) {
					ampsum = 0.f;
					for (int j = 0; j < maxperiod; ++j) {	
						ampsum += bufData[i+j] * bufData[j];
					}
					if (ampsum > threshold) {
						if (ampsum > maxsum) {
							foundPeak = true;
							maxsum = ampsum;
							//peakbinstep = binstep;
							period = i;
						}
					} else if (foundPeak) break;
					
					octave = LOG2CEIL(i); 
					if (octave <= maxlog2bins) {
						binstep = 1;
					} else {
						binstep = 1L << (octave - maxlog2bins);
					}
				}
				
				//postbuf("found %d  thr %g  maxs %g  per %d  bs %d\n", foundPeak, threshold, maxsum, period, peakbinstep);
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
					
					//postbuf("prevnext %g %g %g   %d\n", prevampsum, maxsum, nextampsum, period);
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
						//postbuf("slide left %g %g %g   %d\n", prevampsum, maxsum, nextampsum, period);
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
						//postbuf("slide right %g %g %g   %d\n", prevampsum, maxsum, nextampsum, period);
					}
					
					// make a fractional period
/*
					float fperiod = period;
					if (prevampsum < nextampsum) {
						fperiod += 0.5 * (nextampsum - prevampsum) / (maxsum - prevampsum);
					} else if (nextampsum < prevampsum) {
						fperiod -= 0.5 * (prevampsum - nextampsum) / (maxsum - nextampsum);
					}
*/
					// make a fractional period
					float fperiod = (float)period;
					float a = 0.5 * (prevampsum + nextampsum) + maxsum;
					float b = -2. * fperiod + a + maxsum - prevampsum;
					fperiod = (-b * 0.5) / a;
					
					// calculate frequency
					float tempfreq = unit->m_srate / fperiod;
					
					//postbuf("freq %g   %g / %g    %g %g  %d\n", tempfreq, unit->m_srate, fperiod,
					//	unit->m_minfreq, unit->m_maxfreq, 
					//	tempfreq >= unit->m_minfreq && tempfreq <= unit->m_maxfreq);
						
					if (tempfreq >= unit->m_minfreq && tempfreq <= unit->m_maxfreq) {
						freq = tempfreq;
						
						// median filter
						if (unit->m_medianSize > 1) {
							freq = insertMedian(unit->m_values, unit->m_ages, unit->m_medianSize, freq);
						}
						hasfreq = 1.f;
						
						startperiod = (ksamps+downsamp-1)/downsamp;
					}
				}
			}
			
			// shift buffer for next fill
			int execPeriod = unit->m_execPeriod;
			int interval = size - execPeriod;
			//postbuf("interval %d  sz %d ep %d\n", interval, size, execPeriod);
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
	//Print("<-RTAlloc %08X\n", unit->m_dlybuf);
	unit->m_mask = delaybufsize - 1;
}
#endif

#define BufCalcDelay(delaytime) (sc_clip(delaytime * SAMPLERATE, 1.f, (float)bufSamples))

void BufDelayUnit_Reset(BufDelayUnit *unit)
{
	Print("->DelayUnit_Reset\n");
	//unit->m_maxdelaytime = ZIN0(1);
	unit->m_delaytime = ZIN0(2);
	Print("unit->m_delaytime %g\n", unit->m_delaytime);
	//unit->m_dlybuf = 0;
	unit->m_fbufnum = -1.;
	
	//DelayUnit_AllocDelayLine(unit);
	Print("->GET_BUF\n");
	GET_BUF
	Print("<-GET_BUF\n");
	unit->m_dsamp = BufCalcDelay(unit->m_delaytime);
	unit->m_numoutput = 0;
	unit->m_iwrphase = 0;
	unit->m_x1 = 0.f;
	unit->m_y1 = 0.f;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufFeedbackDelay_Reset(BufFeedbackDelay *unit)
{
	BufDelayUnit_Reset(unit);
	
	unit->m_decaytime = ZIN0(3);
	unit->m_feedbk = CalcFeedback(unit->m_delaytime, unit->m_decaytime);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufDelayN_Ctor(BufDelayN *unit)
{
	SETCALC(BufDelayN_next_z);
	BufDelayUnit_Reset(unit);
	ZOUT0(0) = 0.f;
}

void BufDelayN_next(BufDelayN *unit, int inNumSamples)
{	
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;

	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = bufData - ZOFF;
		float* dlyrd   = dlybuf1 + (irdphase & mask);
		float* dlywr   = dlybuf1 + (iwrphase & mask);
		float* dlyN    = dlybuf1 + bufSamples;
		long remain = inNumSamples;
		while (remain) {
			long rdspace = dlyN - dlyrd;
			long wrspace = dlyN - dlywr;
			long nsmps = sc_min(rdspace, wrspace);
			if (nsmps == 0) GraphEnd(unit->mParent); // buffer not allocated.
			nsmps = sc_min(remain, nsmps);
			remain -= nsmps;
			LOOP(nsmps,
				float value = ZXP(dlyrd);
				ZXP(dlywr) = ZXP(in);
				ZXP(out) = value;
			);
			if (dlyrd == dlyN) dlyrd = dlybuf1;
			if (dlywr == dlyN) dlywr = dlybuf1;
		}
		iwrphase += inNumSamples;
	} else {
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			++iwrphase;
			long irdphase = iwrphase - (long)dsamp;
			float value = bufData[irdphase & mask];
			bufData[iwrphase & mask] = ZXP(in);
			ZXP(out) = value;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void BufDelayN_next_z(BufDelayN *unit, int inNumSamples)
{	
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;

	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = bufData - ZOFF;
		float* dlyN    = dlybuf1 + bufSamples;
		long remain = inNumSamples;
		while (remain) {
			float* dlywr = dlybuf1 + (iwrphase & mask);
			float* dlyrd = dlybuf1 + (irdphase & mask);
			long rdspace = dlyN - dlyrd;
			long wrspace = dlyN - dlywr;
			long nsmps = sc_min(rdspace, wrspace);
			if (nsmps == 0) GraphEnd(unit->mParent); // buffer not allocated.
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
					ZXP(dlywr) = ZXP(in);
					ZXP(out) = value;
				);
			}
			iwrphase += nsmps;
			irdphase += nsmps;
		}
	} else {
		
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long irdphase = iwrphase - (long)dsamp;
			
			if (irdphase < 0) {
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
			} else {
				float value = bufData[irdphase & mask];
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = value;
			}
			iwrphase++;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples) {
		SETCALC(BufDelayN_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufDelayL_Ctor(BufDelayL *unit)
{
	BufDelayUnit_Reset(unit);
	SETCALC(BufDelayL_next_z);
	ZOUT0(0) = 0.f;
}

void BufDelayL_next(BufDelayL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = bufData[irdphase & mask];
			float d2 = bufData[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			bufData[iwrphase & mask] = ZXP(in);
			ZXP(out) = value;
			iwrphase++;
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = bufData[irdphase & mask];
			float d2 = bufData[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			bufData[iwrphase & mask] = ZXP(in);
			ZXP(out) = value;
			iwrphase++;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void BufDelayL_next_z(BufDelayL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			
			float zin = ZXP(in);
			if (irdphase < 0) {
				bufData[iwrphase & mask] = zin;
				ZXP(out) = 0.f;
			} else if (irdphaseb < 0) {
				float d1 = bufData[irdphase & mask];
				float value = d1 - frac * d1;
				bufData[iwrphase & mask] = zin;
				ZXP(out) = value;
			} else {
				float d1 = bufData[irdphase & mask];
				float d2 = bufData[irdphaseb & mask];
				float value = lininterp(frac, d1, d2);
				bufData[iwrphase & mask] = zin;
				ZXP(out) = value;
			}
			iwrphase++;
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;

			float zin = ZXP(in);
			if (irdphase < 0) {
				ZXP(out) = 0.f;
				bufData[iwrphase & mask] = zin;
			} else if (irdphaseb < 0) {
				float d1 = bufData[irdphase & mask];
				float value = d1 - frac * d1;
				bufData[iwrphase & mask] = zin;
				ZXP(out) = value;
			} else {
				float d1 = bufData[irdphase & mask];
				float d2 = bufData[irdphaseb & mask];
				float value = lininterp(frac, d1, d2);
				bufData[iwrphase & mask] = zin;
				ZXP(out) = value;
			}
			iwrphase++;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples) {
		SETCALC(BufDelayL_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufDelayC_Ctor(BufDelayC *unit)
{
	BufDelayUnit_Reset(unit);
	SETCALC(BufDelayC_next_z);
	ZOUT0(0) = 0.f;
}

void BufDelayC_next(BufDelayC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = bufData[irdphase0 & mask];
			float d1 = bufData[irdphase1 & mask];
			float d2 = bufData[irdphase2 & mask];
			float d3 = bufData[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			bufData[iwrphase & mask] = ZXP(in);
			ZXP(out) = value;
			iwrphase++;
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = bufData[irdphase0 & mask];
			float d1 = bufData[irdphase1 & mask];
			float d2 = bufData[irdphase2 & mask];
			float d3 = bufData[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			bufData[iwrphase & mask] = ZXP(in);
			ZXP(out) = value;
			iwrphase++;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void BufDelayC_next_z(BufDelayC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float d0, d1, d2, d3;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			
			if (irdphase0 < 0) {
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
			} else {
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
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = value;
			}
			iwrphase++;
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;

			if (irdphase0 < 0) {
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
			} else {
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
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = value;
			}
			iwrphase++;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples) {
		SETCALC(BufDelayC_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufCombN_Ctor(BufCombN *unit)
{
	SETCALC(BufCombN_next_z);
	BufFeedbackDelay_Reset(unit);
	ZOUT0(0) = 0.f;
}

void BufCombN_next(BufCombN *unit, int inNumSamples)
{	
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	//postbuf("BufCombN_next %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = bufData - ZOFF;
		float* dlyrd   = dlybuf1 + (irdphase & mask);
		float* dlywr   = dlybuf1 + (iwrphase & mask);
		float* dlyN    = dlybuf1 + bufSamples;
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				if (nsmps == 0) GraphEnd(unit->mParent); // buffer not allocated.
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
			float next_feedbk = CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				if (nsmps == 0) GraphEnd(unit->mParent); // buffer not allocated.
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
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			++iwrphase;
			long irdphase = iwrphase - (long)dsamp;
			float value = bufData[irdphase & mask];
			bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
			ZXP(out) = value;
			feedbk += feedbk_slope;
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
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	//Print("BufCombN_next_z %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = bufData - ZOFF;
		float* dlyN    = dlybuf1 + bufSamples;
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				float* dlywr = dlybuf1 + (iwrphase & mask);
				float* dlyrd = dlybuf1 + (irdphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				if (nsmps == 0) GraphEnd(unit->mParent); // buffer not allocated.
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
			float next_feedbk = CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				float* dlyrd = dlybuf1 + (irdphase & mask);
				float* dlywr = dlybuf1 + (iwrphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				if (nsmps == 0) GraphEnd(unit->mParent); // buffer not allocated.
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
		
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long irdphase = iwrphase - (long)dsamp;
			
			if (irdphase < 0) {
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
			} else {
				float value = bufData[irdphase & mask];
				bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
				ZXP(out) = value;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples) {
		SETCALC(BufCombN_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufCombL_Ctor(BufCombL *unit)
{
	BufFeedbackDelay_Reset(unit);
	SETCALC(BufCombL_next_z);
	ZOUT0(0) = 0.f;
}

void BufCombL_next(BufCombL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = bufData[irdphase & mask];
			float d2 = bufData[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
			ZXP(out) = value;
			iwrphase++;
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = bufData[irdphase & mask];
			float d2 = bufData[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			bufData[iwrphase & mask] = ZXP(in) + feedbk * value;
			ZXP(out) = value;
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void BufCombL_next_z(BufCombL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
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
				//postbuf("A %d d1 %g fr %g v %g in %g fb %g\n", irdphase, d1, frac, value, zin, feedbk);
				ZXP(out) = value;
			} else {
				float d1 = bufData[irdphase & mask];
				float d2 = bufData[irdphaseb & mask];
				float value = lininterp(frac, d1, d2);
				bufData[iwrphase & mask] = zin + feedbk * value;
				//postbuf("B %d d1 %g d2 %g fr %g v %g in %g fb %g\n", irdphase, d1, d2, frac, value, zin, feedbk);
				ZXP(out) = value;
			}
			iwrphase++;
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;

			float zin = ZXP(in);
			if (irdphase < 0) {
				ZXP(out) = 0.f;
				bufData[iwrphase & mask] = zin;
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
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples) {
		SETCALC(BufCombL_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufCombC_Ctor(BufCombC *unit)
{
	BufFeedbackDelay_Reset(unit);
	SETCALC(BufCombC_next_z);
	ZOUT0(0) = 0.f;
}

void BufCombC_next(BufCombC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
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
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
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
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void BufCombC_next_z(BufCombC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	float d0, d1, d2, d3;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			
			if (irdphase0 < 0) {
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
			} else {
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
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;

			if (irdphase0 < 0) {
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
			} else {
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
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples) {
		SETCALC(BufCombC_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufAllpassN_Ctor(BufAllpassN *unit)
{
	SETCALC(BufAllpassN_next_z);
	BufFeedbackDelay_Reset(unit);
	ZOUT0(0) = 0.f;
}

void BufAllpassN_next(BufAllpassN *unit, int inNumSamples)
{	
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	//postbuf("BufAllpassN_next %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = bufData - ZOFF;
		float* dlyrd   = dlybuf1 + (irdphase & mask);
		float* dlywr   = dlybuf1 + (iwrphase & mask);
		float* dlyN    = dlybuf1 + bufSamples;
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				if (nsmps == 0) GraphEnd(unit->mParent); // buffer not allocated.
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
			float next_feedbk = CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				if (nsmps == 0) GraphEnd(unit->mParent); // buffer not allocated.
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
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			++iwrphase;
			long irdphase = iwrphase - (long)dsamp;
			float value = bufData[irdphase & mask];
			float dwr = value * feedbk + ZXP(in);
			bufData[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;
			feedbk += feedbk_slope;
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
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	//postbuf("BufAllpassN_next_z %g %g %g %g %d %d %d\n", delaytime, decaytime, feedbk, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = bufData - ZOFF;
		float* dlyN    = dlybuf1 + bufSamples;
		if (decaytime == unit->m_decaytime) {
			long remain = inNumSamples;
			while (remain) {
				float* dlywr = dlybuf1 + (iwrphase & mask);
				float* dlyrd = dlybuf1 + (irdphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				if (nsmps == 0) GraphEnd(unit->mParent); // buffer not allocated.
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
			float next_feedbk = CalcFeedback(delaytime, decaytime);
			float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
			long remain = inNumSamples;
			while (remain) {
				float* dlyrd = dlybuf1 + (irdphase & mask);
				float* dlywr = dlybuf1 + (iwrphase & mask);
				long rdspace = dlyN - dlyrd;
				long wrspace = dlyN - dlywr;
				long nsmps = sc_min(rdspace, wrspace);
				if (nsmps == 0) GraphEnd(unit->mParent); // buffer not allocated.
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
		
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long irdphase = iwrphase - (long)dsamp;
			
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
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples) {
		SETCALC(BufAllpassN_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufAllpassL_Ctor(BufAllpassL *unit)
{
	BufFeedbackDelay_Reset(unit);
	SETCALC(BufAllpassL_next_z);
	ZOUT0(0) = 0.f;
}

void BufAllpassL_next(BufAllpassL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = bufData[irdphase & mask];
			float d2 = bufData[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			float dwr = ZXP(in) + feedbk * value;
			bufData[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;

			
			iwrphase++;
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = bufData[irdphase & mask];
			float d2 = bufData[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			float dwr = ZXP(in) + feedbk * value;
			bufData[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void BufAllpassL_next_z(BufAllpassL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
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
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
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
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples) {
		SETCALC(BufAllpassL_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void BufAllpassC_Ctor(BufAllpassC *unit)
{
	BufFeedbackDelay_Reset(unit);
	SETCALC(BufAllpassC_next_z);
	ZOUT0(0) = 0.f;
}

void BufAllpassC_next(BufAllpassC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
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
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
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
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void BufAllpassC_next_z(BufAllpassC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(1);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	GET_BUF
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	float d0, d1, d2, d3;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			
			if (irdphase0 < 0) {
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
			} else {
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
		);
	} else {
	
		float next_dsamp = BufCalcDelay(delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;

			if (irdphase0 < 0) {
				bufData[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
			} else {
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
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= bufSamples) {
		SETCALC(BufAllpassC_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void DelayUnit_AllocDelayLine(DelayUnit *unit)
{
	long delaybufsize = (long)ceil(unit->m_maxdelaytime * SAMPLERATE + 1.f);
	delaybufsize = delaybufsize + BUFLENGTH;
	delaybufsize = NEXTPOWEROFTWO(delaybufsize);  // round up to next power of two
	unit->m_fdelaylen = unit->m_idelaylen = delaybufsize;
	
	RTFree(unit->mWorld, unit->m_dlybuf);
	unit->m_dlybuf = (float*)RTAlloc(unit->mWorld, delaybufsize * sizeof(float));
	unit->m_mask = delaybufsize - 1;
}

void DelayUnit_Reset(DelayUnit *unit)
{
	unit->m_dlybuf = 0;
	
	DelayUnit_AllocDelayLine(unit);
	
	unit->m_numoutput = 0;
	unit->m_iwrphase = 0;
	unit->m_x1 = 0.f;
	unit->m_y1 = 0.f;
}


void DelayUnit_Dtor(DelayUnit *unit)
{
	RTFree(unit->mWorld, unit->m_dlybuf);
}

float CalcDelay(DelayUnit *unit, float delaytime);
float CalcDelay(DelayUnit *unit, float delaytime)
{
	float next_dsamp = delaytime * SAMPLERATE;
	return sc_clip(next_dsamp, 1.f, unit->m_fdelaylen);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void FeedbackDelay_Reset(FeedbackDelay *unit)
{
	unit->m_maxdelaytime = ZIN0(1);
	unit->m_delaytime = ZIN0(2);
	unit->m_decaytime = ZIN0(3);
	
	DelayUnit_Reset(unit);
	
	unit->m_dsamp = CalcDelay(unit, unit->m_delaytime);
	unit->m_feedbk = CalcFeedback(unit->m_delaytime, unit->m_decaytime);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void DelayN_Ctor(DelayN *unit)
{
	SETCALC(DelayN_next_z);
	DelayUnit_Reset(unit);
	ZOUT0(0) = 0.f;
}

void DelayN_next(DelayN *unit, int inNumSamples)
{	
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float delaytime = ZIN0(2);
	
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;

	//postbuf("DelayN_next %g %g %g %g %d %d %d\n", delaytime, dsamp, mask, iwrphase, zorg);
	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = dlybuf - ZOFF;
		float* dlyrd   = dlybuf1 + (irdphase & mask);
		float* dlywr   = dlybuf1 + (iwrphase & mask);
		float* dlyN    = dlybuf1 + unit->m_idelaylen;
		long remain = inNumSamples;
		while (remain) {
			long rdspace = dlyN - dlyrd;
			long wrspace = dlyN - dlywr;
			long nsmps = sc_min(rdspace, wrspace);
			nsmps = sc_min(remain, nsmps);
			remain -= nsmps;
			LOOP(nsmps,
				float value = ZXP(dlyrd);
				ZXP(dlywr) = ZXP(in);
				ZXP(out) = value;
			);
			if (dlyrd == dlyN) dlyrd = dlybuf1;
			if (dlywr == dlyN) dlywr = dlybuf1;
		}
		iwrphase += inNumSamples;
	} else {
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			++iwrphase;
			long irdphase = iwrphase - (long)dsamp;
			float value = dlybuf[irdphase & mask];
			dlybuf[iwrphase & mask] = ZXP(in);
			ZXP(out) = value;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void DelayN_next_z(DelayN *unit, int inNumSamples)
{	
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float delaytime = ZIN0(2);
	
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;

	if (delaytime == unit->m_delaytime) {
		long irdphase = iwrphase - (long)dsamp;
		float* dlybuf1 = dlybuf - ZOFF;
		float* dlyN    = dlybuf1 + unit->m_idelaylen;
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
					ZXP(dlywr) = ZXP(in);
					ZXP(out) = value;
				);
			}
			iwrphase += nsmps;
			irdphase += nsmps;
		}
	} else {
		
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long irdphase = iwrphase - (long)dsamp;
			
			if (irdphase < 0) {
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
			} else {
				float value = dlybuf[irdphase & mask];
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = value;
			}
			iwrphase++;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(DelayN_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void DelayL_Ctor(DelayL *unit)
{
	DelayUnit_Reset(unit);
	SETCALC(DelayL_next_z);
	ZOUT0(0) = 0.f;
}

void DelayL_next(DelayL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float delaytime = ZIN0(2);
	
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = dlybuf[irdphase & mask];
			float d2 = dlybuf[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			dlybuf[iwrphase & mask] = ZXP(in);
			ZXP(out) = value;
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = dlybuf[irdphase & mask];
			float d2 = dlybuf[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			dlybuf[iwrphase & mask] = ZXP(in);
			ZXP(out) = value;
			iwrphase++;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void DelayL_next_z(DelayL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float delaytime = ZIN0(2);
	
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			
			float zin = ZXP(in);
			if (irdphase < 0) {
				dlybuf[iwrphase & mask] = zin;
				ZXP(out) = 0.f;
			} else if (irdphaseb < 0) {
				float d1 = dlybuf[irdphase & mask];
				float value = d1 - frac * d1;
				dlybuf[iwrphase & mask] = zin;
				//postbuf("A %d d1 %g fr %g v %g in %g fb %g\n", irdphase, d1, frac, value, zin, feedbk);
				ZXP(out) = value;
			} else {
				float d1 = dlybuf[irdphase & mask];
				float d2 = dlybuf[irdphaseb & mask];
				float value = lininterp(frac, d1, d2);
				dlybuf[iwrphase & mask] = zin;
				//postbuf("B %d d1 %g d2 %g fr %g v %g in %g fb %g\n", irdphase, d1, d2, frac, value, zin, feedbk);
				ZXP(out) = value;
			}
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;

			float zin = ZXP(in);
			if (irdphase < 0) {
				ZXP(out) = 0.f;
				dlybuf[iwrphase & mask] = zin;
			} else if (irdphaseb < 0) {
				float d1 = dlybuf[irdphase & mask];
				float value = d1 - frac * d1;
				dlybuf[iwrphase & mask] = zin;
				ZXP(out) = value;
			} else {
				float d1 = dlybuf[irdphase & mask];
				float d2 = dlybuf[irdphaseb & mask];
				float value = lininterp(frac, d1, d2);
				dlybuf[iwrphase & mask] = zin;
				ZXP(out) = value;
			}
			iwrphase++;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(DelayL_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void DelayC_Ctor(DelayC *unit)
{
	DelayUnit_Reset(unit);
	SETCALC(DelayC_next_z);
	ZOUT0(0) = 0.f;
}

void DelayC_next(DelayC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float delaytime = ZIN0(2);
	
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			dlybuf[iwrphase & mask] = ZXP(in);
			ZXP(out) = value;
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			dlybuf[iwrphase & mask] = ZXP(in);
			ZXP(out) = value;
			iwrphase++;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void DelayC_next_z(DelayC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float delaytime = ZIN0(2);
	
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;

	if (delaytime == unit->m_delaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
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
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = value;
			}
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;

			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
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
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = value;
			}
			iwrphase++;
		);
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(DelayC_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void CombN_Ctor(CombN *unit)
{
	SETCALC(CombN_next_z);
	FeedbackDelay_Reset(unit);
	ZOUT0(0) = 0.f;
}

void CombN_next(CombN *unit, int inNumSamples)
{	
	float *out = ZOUT(0);
	float *in = ZIN(0);
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
			float next_feedbk = CalcFeedback(delaytime, decaytime);
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
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			++iwrphase;
			long irdphase = iwrphase - (long)dsamp;
			float value = dlybuf[irdphase & mask];
			dlybuf[iwrphase & mask] = ZXP(in) + feedbk * value;
			ZXP(out) = value;
			feedbk += feedbk_slope;
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
	float *in = ZIN(0);
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
			float next_feedbk = CalcFeedback(delaytime, decaytime);
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
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long irdphase = iwrphase - (long)dsamp;
			
			if (irdphase < 0) {
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
			} else {
				float value = dlybuf[irdphase & mask];
				dlybuf[iwrphase & mask] = ZXP(in) + feedbk * value;
				ZXP(out) = value;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(CombN_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void CombL_Ctor(CombL *unit)
{
	FeedbackDelay_Reset(unit);
	SETCALC(CombL_next_z);
	ZOUT0(0) = 0.f;
}

void CombL_next(CombL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
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
		LOOP(inNumSamples,
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = dlybuf[irdphase & mask];
			float d2 = dlybuf[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			dlybuf[iwrphase & mask] = ZXP(in) + feedbk * value;
			ZXP(out) = value;
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = dlybuf[irdphase & mask];
			float d2 = dlybuf[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			dlybuf[iwrphase & mask] = ZXP(in) + feedbk * value;
			ZXP(out) = value;
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void CombL_next_z(CombL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
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
		LOOP(inNumSamples,
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			
			float zin = ZXP(in);
			if (irdphase < 0) {
				dlybuf[iwrphase & mask] = zin;
				ZXP(out) = 0.f;
			} else if (irdphaseb < 0) {
				float d1 = dlybuf[irdphase & mask];
				float value = d1 - frac * d1;
				dlybuf[iwrphase & mask] = zin + feedbk * value;
				//postbuf("A %d d1 %g fr %g v %g in %g fb %g\n", irdphase, d1, frac, value, zin, feedbk);
				ZXP(out) = value;
			} else {
				float d1 = dlybuf[irdphase & mask];
				float d2 = dlybuf[irdphaseb & mask];
				float value = lininterp(frac, d1, d2);
				dlybuf[iwrphase & mask] = zin + feedbk * value;
				//postbuf("B %d d1 %g d2 %g fr %g v %g in %g fb %g\n", irdphase, d1, d2, frac, value, zin, feedbk);
				ZXP(out) = value;
			}
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;

			float zin = ZXP(in);
			if (irdphase < 0) {
				ZXP(out) = 0.f;
				dlybuf[iwrphase & mask] = zin;
			} else if (irdphaseb < 0) {
				float d1 = dlybuf[irdphase & mask];
				float value = d1 - frac * d1;
				dlybuf[iwrphase & mask] = zin + feedbk * value;
				ZXP(out) = value;
			} else {
				float d1 = dlybuf[irdphase & mask];
				float d2 = dlybuf[irdphaseb & mask];
				float value = lininterp(frac, d1, d2);
				dlybuf[iwrphase & mask] = zin + feedbk * value;
				ZXP(out) = value;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(CombL_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void CombC_Ctor(CombC *unit)
{
	FeedbackDelay_Reset(unit);
	SETCALC(CombC_next_z);
	ZOUT0(0) = 0.f;
}

void CombC_next(CombC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
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
		LOOP(inNumSamples,
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			dlybuf[iwrphase & mask] = ZXP(in) + feedbk * value;
			ZXP(out) = value;
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			dlybuf[iwrphase & mask] = ZXP(in) + feedbk * value;
			ZXP(out) = value;
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void CombC_next_z(CombC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
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
				dlybuf[iwrphase & mask] = ZXP(in) + feedbk * value;
				ZXP(out) = value;
			}
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;

			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
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
				dlybuf[iwrphase & mask] = ZXP(in) + feedbk * value;
				ZXP(out) = value;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(CombC_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void AllpassN_Ctor(AllpassN *unit)
{
	SETCALC(AllpassN_next_z);
	FeedbackDelay_Reset(unit);
	ZOUT0(0) = 0.f;
}

void AllpassN_next(AllpassN *unit, int inNumSamples)
{	
	float *out = ZOUT(0);
	float *in = ZIN(0);
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
			float next_feedbk = CalcFeedback(delaytime, decaytime);
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
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);
		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			++iwrphase;
			long irdphase = iwrphase - (long)dsamp;
			float value = dlybuf[irdphase & mask];
			float dwr = value * feedbk + ZXP(in);
			dlybuf[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;
			feedbk += feedbk_slope;
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
	float *in = ZIN(0);
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
			float next_feedbk = CalcFeedback(delaytime, decaytime);
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
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long irdphase = iwrphase - (long)dsamp;
			
			if (irdphase < 0) {
				float dwr = ZXP(in);
				dlybuf[iwrphase & mask] = dwr;
				ZXP(out) = -feedbk * dwr;
			} else {
				float value = dlybuf[irdphase & mask];
				float dwr = feedbk * value + ZXP(in);
				dlybuf[iwrphase & mask] = dwr;
				ZXP(out) = value - feedbk * dwr;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(AllpassN_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void AllpassL_Ctor(AllpassL *unit)
{
	FeedbackDelay_Reset(unit);
	SETCALC(AllpassL_next_z);
	ZOUT0(0) = 0.f;
}

void AllpassL_next(AllpassL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
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
		LOOP(inNumSamples,
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = dlybuf[irdphase & mask];
			float d2 = dlybuf[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			float dwr = ZXP(in) + feedbk * value;
			dlybuf[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;

			
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			float d1 = dlybuf[irdphase & mask];
			float d2 = dlybuf[irdphaseb & mask];
			float value = lininterp(frac, d1, d2);
			float dwr = ZXP(in) + feedbk * value;
			dlybuf[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void AllpassL_next_z(AllpassL *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
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
		LOOP(inNumSamples,
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;
			
			float zin = ZXP(in);
			if (irdphase < 0) {
				dlybuf[iwrphase & mask] = zin;
				ZXP(out) = - feedbk * zin;
			} else if (irdphaseb < 0) {
				float d1 = dlybuf[irdphase & mask];
				float value = d1 - frac * d1;
				float dwr = zin + feedbk * value;
				dlybuf[iwrphase & mask] = dwr;
				ZXP(out) = value - feedbk * dwr;
			} else {
				float d1 = dlybuf[irdphase & mask];
				float d2 = dlybuf[irdphaseb & mask];
				float value = lininterp(frac, d1, d2);
				float dwr = zin + feedbk * value;
				dlybuf[iwrphase & mask] = dwr;
				ZXP(out) = value - feedbk * dwr;
			}
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase = iwrphase - idsamp;
			long irdphaseb = irdphase - 1;

			float zin = ZXP(in);
			if (irdphase < 0) {
				dlybuf[iwrphase & mask] = zin;
				ZXP(out) = - feedbk * zin;
			} else if (irdphaseb < 0) {
				float d1 = dlybuf[irdphase & mask];
				float value = d1 - frac * d1;
				float dwr = zin + feedbk * value;
				dlybuf[iwrphase & mask] = dwr;
				ZXP(out) = value - feedbk * dwr;
			} else {
				float d1 = dlybuf[irdphase & mask];
				float d2 = dlybuf[irdphaseb & mask];
				float value = lininterp(frac, d1, d2);
				float dwr = zin + feedbk * value;
				dlybuf[iwrphase & mask] = dwr;
				ZXP(out) = value - feedbk * dwr;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(AllpassL_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void AllpassC_Ctor(AllpassC *unit)
{
	FeedbackDelay_Reset(unit);
	SETCALC(AllpassC_next_z);
	ZOUT0(0) = 0.f;
}

void AllpassC_next(AllpassC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
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
		LOOP(inNumSamples,
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float dwr = ZXP(in) + feedbk * value;
			dlybuf[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			float d0 = dlybuf[irdphase0 & mask];
			float d1 = dlybuf[irdphase1 & mask];
			float d2 = dlybuf[irdphase2 & mask];
			float d3 = dlybuf[irdphase3 & mask];
			float value = cubicinterp(frac, d0, d1, d2, d3);
			float dwr = ZXP(in) + feedbk * value;
			dlybuf[iwrphase & mask] = dwr;
			ZXP(out) = value - feedbk * dwr;
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

}


void AllpassC_next_z(AllpassC *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float delaytime = ZIN0(2);
	float decaytime = ZIN0(3);
	
	float *dlybuf = unit->m_dlybuf;
	long iwrphase = unit->m_iwrphase;
	float dsamp = unit->m_dsamp;
	float feedbk = unit->m_feedbk;
	long mask = unit->m_mask;
	float d0, d1, d2, d3;

	if (delaytime == unit->m_delaytime && decaytime == unit->m_decaytime) {
		long idsamp = (long)dsamp;
		float frac = dsamp - idsamp;
		LOOP(inNumSamples,
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;
			
			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
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
				float dwr = ZXP(in) + feedbk * value;
				dlybuf[iwrphase & mask] = dwr;
				ZXP(out) = value - feedbk * dwr;
			}
			iwrphase++;
		);
	} else {
	
		float next_dsamp = CalcDelay(unit, delaytime);
		float dsamp_slope = CALCSLOPE(next_dsamp, dsamp);
		
		float next_feedbk = CalcFeedback(delaytime, decaytime);
		float feedbk_slope = CALCSLOPE(next_feedbk, feedbk);

		LOOP(inNumSamples,
			dsamp += dsamp_slope;
			long idsamp = (long)dsamp;
			float frac = dsamp - idsamp;
			long irdphase1 = iwrphase - idsamp;
			long irdphase2 = irdphase1 - 1;
			long irdphase3 = irdphase1 - 2;
			long irdphase0 = irdphase1 + 1;

			if (irdphase0 < 0) {
				dlybuf[iwrphase & mask] = ZXP(in);
				ZXP(out) = 0.f;
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
				float dwr = ZXP(in) + feedbk * value;
				dlybuf[iwrphase & mask] = dwr;
				ZXP(out) = value - feedbk * dwr;
			}
			feedbk += feedbk_slope;
			iwrphase++;
		);
		unit->m_feedbk = feedbk;
		unit->m_dsamp = dsamp;
		unit->m_delaytime = delaytime;
		unit->m_decaytime = decaytime;
	}
	
	unit->m_iwrphase = iwrphase;

	unit->m_numoutput += inNumSamples;
	if (unit->m_numoutput >= unit->m_idelaylen) {
		SETCALC(AllpassC_next);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineSimpleUnit(PlayBuf);
	DefineSimpleUnit(RecordBuf);
	DefineSimpleUnit(BufRd);
	DefineSimpleUnit(BufWr);
	DefineSimpleUnit(Pitch);
	
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
	(UnitDtorFunc)&DelayUnit_Dtor);
	
	DefineDelayUnit(DelayN);
	DefineDelayUnit(DelayL);
	DefineDelayUnit(DelayC);
	DefineDelayUnit(CombN);
	DefineDelayUnit(CombL);
	DefineDelayUnit(CombC);
	DefineDelayUnit(AllpassN);
	DefineDelayUnit(AllpassL);
	DefineDelayUnit(AllpassC);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
