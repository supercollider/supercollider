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

#include <limits>

// NaNs are not equal to any floating point number
static const float uninitializedControl = std::numeric_limits<float>::quiet_NaN();

#define PI 3.1415926535898f

#define PUSH_LOOPVALS \
int tmp_floops = unit->mRate->mFilterLoops; \
int tmp_fremain = unit->mRate->mFilterRemain; \
unit->mRate->mFilterLoops = 0; \
unit->mRate->mFilterRemain = 1;

#define POP_LOOPVALS \
unit->mRate->mFilterLoops = tmp_floops; \
unit->mRate->mFilterRemain = tmp_fremain;

using namespace std; // for math functions

static InterfaceTable *ft;

struct Ramp : public Unit
{
	double m_level, m_slope;
	int m_counter;
};

struct Lag : public Unit
{
	float m_lag;
	double m_b1, m_y1;
};

struct Lag2 : public Unit
{
	float m_lag;
	double m_b1, m_y1a, m_y1b;
};

struct Lag3 : public Unit
{
	float m_lag;
	double m_b1, m_y1a, m_y1b, m_y1c;
};

struct LagUD : public Unit
{
	float m_lagu, m_lagd;
	double m_b1u, m_b1d, m_y1;
};

struct Lag2UD : public Unit
{
	float m_lagu, m_lagd;
	double m_b1u, m_b1d, m_y1a, m_y1b;
};

struct Lag3UD : public Unit
{
	float m_lagu, m_lagd;
	double m_b1u, m_b1d, m_y1a, m_y1b, m_y1c;
};

struct VarLag : public Unit
{
	double m_level, m_slope;
	int m_counter;
	float m_in, m_lagTime;
};

struct OnePole : public Unit
{
	double m_b1, m_y1;
};

struct OneZero : public Unit
{
	double m_b1, m_x1;
};

struct Integrator : public Unit
{
	double m_b1, m_y1;
};

struct Decay : public Unit
{
	float m_decayTime;
	double m_y1, m_b1;
};

struct Decay2 : public Unit
{
	float m_attackTime;
	double m_y1a, m_b1a;
	float m_decayTime;
	double m_y1b, m_b1b;
};

struct LeakDC : public Unit
{
	double m_b1, m_x1, m_y1;
};

struct TwoPole : public Unit
{
	float m_freq, m_reson;
	double m_y1, m_y2, m_b1, m_b2;
};

struct APF : public Unit
{
	float m_freq, m_reson;
	double m_y1, m_y2, m_x1, m_x2, m_b1, m_b2;
};

struct TwoZero : public Unit
{
	float m_freq, m_reson;
	double m_x1, m_x2, m_b1, m_b2;
};

struct LPZ1 : public Unit
{
	double m_x1;
};

struct HPZ1 : public Unit
{
	double m_x1;
};

struct HPZ2 : public Unit
{
	double m_x1, m_x2;
};

struct BPZ2 : public Unit
{
	double m_x1, m_x2;
};

struct BRZ2 : public Unit
{
	double m_x1, m_x2;
};

struct LPZ2 : public Unit
{
	double m_x1, m_x2;
};

struct Flip : public Unit
{
};

struct Delay1 : public Unit
{
	float m_x1;
};

struct Delay2 : public Unit
{
	float m_x1, m_x2;
};

struct Slope : public Unit
{
	double m_x1;
};

struct Slew : public Unit
{
	double mLevel;
};

struct RLPF : public Unit
{
	float m_freq, m_reson;
	double m_y1, m_y2, m_a0, m_b1, m_b2;
};

struct RHPF : public Unit
{
	float m_freq, m_reson;
	double m_y1, m_y2, m_a0, m_b1, m_b2;
};

struct LPF : public Unit
{
	float m_freq;
	double m_y1, m_y2, m_a0, m_b1, m_b2;
};

struct HPF : public Unit
{
	float m_freq;
	double m_y1, m_y2, m_a0, m_b1, m_b2;
};

struct BPF : public Unit
{
	float m_freq, m_bw;
	double m_y1, m_y2, m_a0, m_b1, m_b2;
};

struct BRF : public Unit
{
	float m_freq, m_bw;
	double m_y1, m_y2, m_a0, m_a1, m_b2;
};

struct MidEQ : public Unit
{
	float m_freq, m_bw, m_db;
	double m_y1, m_y2, m_a0, m_b1, m_b2;
};

struct Resonz : public Unit
{
	float m_freq, m_rq;
	double m_y1, m_y2, m_a0, m_b1, m_b2;
};

struct Ringz : public Unit
{
	float m_freq, m_decayTime;
	double m_y1, m_y2, m_b1, m_b2;
};

struct FOS : public Unit
{
	double m_y1, m_a0, m_a1, m_b1;
};

struct SOS : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
};

struct Formlet : public Unit
{
	float m_freq, m_decayTime, m_attackTime;
	double m_y01, m_y02, m_b01, m_b02;
	double m_y11, m_y12, m_b11, m_b12;
};

const int kMAXMEDIANSIZE = 32;

struct Median : public Unit
{
	float m_medianValue[kMAXMEDIANSIZE];
	long m_medianAge[kMAXMEDIANSIZE];
	long m_medianSize, m_medianIndex;
};

struct Compander : public Unit
{
	float m_prevmaxval, m_gain, m_clamp, m_clampcoef, m_relax, m_relaxcoef;
};

struct Normalizer : public Unit
{
	float *m_table, *m_xinbuf, *m_xoutbuf, *m_xmidbuf;
	long m_flips, m_pos, m_bufsize;
	float m_slope, m_level, m_curmaxval, m_prevmaxval, m_slopefactor;
};

struct Limiter : public Unit
{
	float *m_table, *m_xinbuf, *m_xoutbuf, *m_xmidbuf;
	long m_flips, m_pos, m_bufsize;
	float m_slope, m_level, m_curmaxval, m_prevmaxval, m_slopefactor;
};


struct Amplitude : public Unit
{
	float m_previn, m_clampcoef, m_relaxcoef, m_clamp_in, m_relax_in;
};

struct DetectSilence : public Unit
{
	float mThresh;
	int32 mCounter, mEndCounter;
};

struct Hilbert : public Unit
{
	double m_coefs[12];
	double m_y1[12];
};

struct FreqShift : public Unit
{
	float m_coefs[12];
	float m_y1[12];
	int32 m_phase;
	int32 m_phaseoffset, m_lomask;
	double m_cpstoinc, m_radtoinc, m_phasein;

};

struct MoogFF : public Unit
{
	float m_freq, m_k;
	double m_b0, m_a1; // Resonant freq and corresponding vals; stored because we need to compare against prev vals

	double m_s1, m_s2, m_s3, m_s4; // 1st order filter states
};

//////////////////////////////////////////////////////////////////////////////////////////////////


extern "C"
{

	void Ramp_next(Ramp *unit, int inNumSamples);
	void Ramp_next_1(Ramp *unit, int inNumSamples);
	void Ramp_Ctor(Ramp* unit);

	void Lag_next(Lag *unit, int inNumSamples);
	void Lag_Ctor(Lag* unit);

	void Lag2_Ctor(Lag2* unit);

	void Lag3_next(Lag3 *unit, int inNumSamples);
	void Lag3_Ctor(Lag3* unit);

	void LagUD_next(LagUD *unit, int inNumSamples);
	void LagUD_Ctor(LagUD* unit);

	void Lag2UD_next(Lag2UD *unit, int inNumSamples);
	void Lag2UD_Ctor(Lag2UD* unit);

	void Lag3UD_next(Lag3UD *unit, int inNumSamples);
	void Lag3UD_Ctor(Lag3UD* unit);

	void VarLag_next(VarLag *unit, int inNumSamples);
	void VarLag_Ctor(VarLag* unit);

	void OnePole_next_a(OnePole *unit, int inNumSamples);
	void OnePole_next_k(OnePole *unit, int inNumSamples);
	void OnePole_Ctor(OnePole* unit);

	void OneZero_next(OneZero* unit, int inNumSamples);
	void OneZero_Ctor(OneZero* unit);

	void Integrator_next(Integrator *unit, int inNumSamples);
	void Integrator_next_i(Integrator *unit, int inNumSamples);
	void Integrator_Ctor(Integrator* unit);

	void Decay_next(Decay *unit, int inNumSamples);
	void Decay_Ctor(Decay* unit);

	void Decay2_next(Decay2 *unit, int inNumSamples);
	void Decay2_Ctor(Decay2* unit);

	void LeakDC_next(LeakDC *unit, int inNumSamples);
	void LeakDC_next_1(LeakDC* unit, int inNumSamples);
	void LeakDC_Ctor(LeakDC* unit);

	void TwoPole_next(TwoPole *unit, int inNumSamples);
	void TwoPole_Ctor(TwoPole* unit);

	void TwoZero_next(TwoZero *unit, int inNumSamples);
	void TwoZero_Ctor(TwoZero* unit);

	void APF_next(APF *unit, int inNumSamples);
	void APF_Ctor(APF* unit);

	void LPZ1_next(LPZ1 *unit, int inNumSamples);
	void LPZ1_Ctor(LPZ1* unit);

	void HPZ1_next(HPZ1 *unit, int inNumSamples);
	void HPZ1_Ctor(HPZ1* unit);

	void Slope_next(Slope *unit, int inNumSamples);
	void Slope_Ctor(Slope* unit);

	void Delay1_next(Delay1 *unit, int inNumSamples);
	void Delay1_Ctor(Delay1* unit);

	void Flip_Ctor(Flip* unit);
	void Flip_next_even(Flip *unit, int inNumSamples);
	void Flip_next_odd(Flip *unit, int inNumSamples);

	void Delay2_next(Delay2 *unit, int inNumSamples);
	void Delay2_Ctor(Delay2* unit);

	void LPZ2_next(LPZ2 *unit, int inNumSamples);
	void LPZ2_Ctor(LPZ2* unit);

	void HPZ2_next(HPZ2 *unit, int inNumSamples);
	void HPZ2_Ctor(HPZ2* unit);

	void BPZ2_next(BPZ2 *unit, int inNumSamples);
	void BPZ2_Ctor(BPZ2* unit);

	void BRZ2_next(BRZ2 *unit, int inNumSamples);
	void BRZ2_Ctor(BRZ2* unit);

	void Slew_next(Slew *unit, int inNumSamples);
	void Slew_Ctor(Slew* unit);

	void RLPF_next(RLPF *unit, int inNumSamples);
	void RLPF_next_1(RLPF *unit, int inNumSamples);
	void RLPF_Ctor(RLPF* unit);

	void RHPF_next(RHPF *unit, int inNumSamples);
	void RHPF_next_1(RHPF *unit, int inNumSamples);
	void RHPF_Ctor(RHPF* unit);

	void LPF_next(LPF *unit, int inNumSamples);
	void LPF_next_1(LPF *unit, int inNumSamples);
	void LPF_Ctor(LPF* unit);

	void HPF_next(HPF *unit, int inNumSamples);
	void HPF_next_1(HPF *unit, int inNumSamples);
	void HPF_Ctor(HPF* unit);

	void BPF_next(BPF *unit, int inNumSamples);
	void BPF_next_1(BPF *unit, int inNumSamples);
	void BPF_Ctor(BPF* unit);

	void BRF_next(BRF *unit, int inNumSamples);
	void BRF_next_1(BRF *unit, int inNumSamples);
	void BRF_Ctor(BRF* unit);

	void Median_next(Median *unit, int inNumSamples);
	void Median_Ctor(Median* unit);

	void MidEQ_next(MidEQ *unit, int inNumSamples);
	void MidEQ_Ctor(MidEQ* unit);

	void Resonz_next(Resonz *unit, int inNumSamples);
	void Resonz_Ctor(Resonz* unit);

	void Ringz_next(Ringz *unit, int inNumSamples);
	void Ringz_Ctor(Ringz* unit);

	void Formlet_next(Formlet *unit, int inNumSamples);
	void Formlet_next_1(Formlet *unit, int inNumSamples);
	void Formlet_Ctor(Formlet* unit);

	void FOS_next_k(FOS *unit, int inNumSamples);
	void FOS_next_a(FOS *unit, int inNumSamples);
	void FOS_next_1(FOS *unit, int inNumSamples);
	void FOS_Ctor(FOS* unit);

	void SOS_next_i(SOS *unit, int inNumSamples);
	void SOS_next_k(SOS *unit, int inNumSamples);
	void SOS_next_a(SOS *unit, int inNumSamples);
	void SOS_next_1(SOS *unit, int inNumSamples);
	void SOS_Ctor(SOS* unit);

	void Normalizer_next(Normalizer *unit, int inNumSamples);
	void Normalizer_Ctor(Normalizer* unit);
	void Normalizer_Dtor(Normalizer* unit);

	void Limiter_next(Limiter *unit, int inNumSamples);
	void Limiter_Ctor(Limiter* unit);
	void Limiter_Dtor(Limiter* unit);

	void Compander_next(Compander *unit, int inNumSamples);
	void Compander_Ctor(Compander* unit);

	void Amplitude_next(Amplitude *unit, int inNumSamples);
	void Amplitude_next_kk(Amplitude *unit, int inNumSamples);
	void Amplitude_next_atok(Amplitude *unit, int inNumSamples);
	void Amplitude_next_atok_kk(Amplitude *unit, int inNumSamples);
	void Amplitude_Ctor(Amplitude* unit);

	void DetectSilence_next(DetectSilence *unit, int inNumSamples);
	void DetectSilence_next_k(DetectSilence *unit, int inNumSamples);
	//void DetectSilence_done(DetectSilence *unit, int inNumSamples);
	void DetectSilence_Ctor(DetectSilence* unit);

	void Hilbert_Ctor(Hilbert* unit);
	void Hilbert_next(Hilbert *unit, int inNumSamples);

	void FreqShift_Ctor(FreqShift* unit);
	void FreqShift_next_kk(FreqShift *unit, int inNumSamples);
	void FreqShift_next_aa(FreqShift *unit, int inNumSamples);
	void FreqShift_next_ak(FreqShift *unit, int inNumSamples);
	void FreqShift_next_ka(FreqShift *unit, int inNumSamples);

	void MoogFF_next(MoogFF *unit, int inNumSamples);
	void MoogFF_Ctor(MoogFF* unit);

/*
	void Lag_next(Lag *unit, int inNumSamples);
	void Lag_Ctor(Lag* unit);

	void Lag_next(Lag *unit, int inNumSamples);
	void Lag_Ctor(Lag* unit);
*/
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void Ramp_next(Ramp *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = IN(0);
	float period = ZIN0(1);

	double slope = unit->m_slope;
	double level = unit->m_level;
	int counter = unit->m_counter;
	int remain = inNumSamples;
	while (remain) {
		int nsmps = sc_min(remain, counter);
		LOOP(nsmps,
			ZXP(out) = level;
			level += slope;
		);
		in += nsmps;
		counter -= nsmps;
		remain -= nsmps;
		if (counter <= 0) {
			counter = (int)(period * SAMPLERATE);
			counter = sc_max(1, counter);
			slope = (*in - level) / counter;
		}
	}
	unit->m_level = level;
	unit->m_slope = slope;
	unit->m_counter = counter;
}

void Ramp_next_1(Ramp *unit, int inNumSamples)
{
	float *out = OUT(0);

	*out = unit->m_level;
	unit->m_level += unit->m_slope;
	if (--unit->m_counter <= 0) {
		float in = ZIN0(0);
		float period = ZIN0(1);
		int counter = (int)(period * SAMPLERATE);
		unit->m_counter = counter = sc_max(1, counter);
		unit->m_slope = (in - unit->m_level) / counter;
	}
}

void Ramp_Ctor(Ramp* unit)
{
	if (BUFLENGTH == 1) {
		SETCALC(Ramp_next_1);
	} else {
		SETCALC(Ramp_next);
	}

	unit->m_counter = 1;
	unit->m_level = ZIN0(0);
	unit->m_slope = 0.f;
	ZOUT0(0) = unit->m_level;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void Lag_next(Lag *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float lag = ZIN0(1);

	double y1 = unit->m_y1;
	double b1 = unit->m_b1;

	if (lag == unit->m_lag) {
		LOOP1(inNumSamples,
			double y0 = ZXP(in);
			ZXP(out) = y1 = y0 + b1 * (y1 - y0);
		);
	} else {
		unit->m_b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
		double b1_slope = CALCSLOPE(unit->m_b1, b1);
		unit->m_lag = lag;
		LOOP1(inNumSamples,
			b1 += b1_slope;
			double y0 = ZXP(in);
			ZXP(out) = y1 = y0 + b1 * (y1 - y0);
		);
	}
	unit->m_y1 = zapgremlins(y1);
}

void Lag_next_1(Lag *unit, int inNumSamples)
{
	float *out = OUT(0);
	float *in = IN(0);
	float lag = IN0(1);

	double y1 = unit->m_y1;
	double b1 = unit->m_b1;

	if (lag == unit->m_lag) {
		double y0 = *in;
		*out = y1 = y0 + b1 * (y1 - y0);
	} else {
		unit->m_b1 = b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
		unit->m_lag = lag;
		double y0 = *in;
		*out = y1 = y0 + b1 * (y1 - y0);
	}
	unit->m_y1 = zapgremlins(y1);
}

void Lag_Ctor(Lag* unit)
{
	if (BUFLENGTH == 1)
		SETCALC(Lag_next_1);
	else
		SETCALC(Lag_next);

	unit->m_lag = uninitializedControl;
	unit->m_b1 = 0.f;
	unit->m_y1 = ZIN0(0);
	Lag_next(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
void LagUD_next(LagUD *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float lagu = ZIN0(1);
	float lagd = ZIN0(2);

	double y1 = unit->m_y1;
	double b1u = unit->m_b1u;
	double b1d = unit->m_b1d;

	if ( (lagu == unit->m_lagu) && (lagd == unit->m_lagd) ) {
		LOOP1(inNumSamples,
			double y0 = ZXP(in);
			if ( y0 > y1 )
				ZXP(out) = y1 = y0 + b1u * (y1 - y0);
			else
				ZXP(out) = y1 = y0 + b1d * (y1 - y0);
		);
	} else {
		unit->m_b1u = lagu == 0.f ? 0.f : exp(log001 / (lagu * unit->mRate->mSampleRate));
		double b1u_slope = CALCSLOPE(unit->m_b1u, b1u);
		unit->m_lagu = lagu;
		unit->m_b1d = lagd == 0.f ? 0.f : exp(log001 / (lagd * unit->mRate->mSampleRate));
		double b1d_slope = CALCSLOPE(unit->m_b1d, b1d);
		unit->m_lagd = lagd;
		LOOP1(inNumSamples,
			b1u += b1u_slope;
			b1d += b1d_slope;
			double y0 = ZXP(in);
			if ( y0 > y1 )
				ZXP(out) = y1 = y0 + b1u * (y1 - y0);
			else
				ZXP(out) = y1 = y0 + b1d * (y1 - y0);
		);
	}
	unit->m_y1 = zapgremlins(y1);
}

void LagUD_Ctor(LagUD* unit)
{
	SETCALC(LagUD_next);

	unit->m_lagu = uninitializedControl;
	unit->m_lagd = uninitializedControl;
	unit->m_b1u = 0.f;
	unit->m_b1d = 0.f;
	unit->m_y1 = ZIN0(0);
	LagUD_next(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

static void Lag2_next_k(Lag2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float lag = ZIN0(1);

	double y1a = unit->m_y1a;
	double y1b = unit->m_y1b;
	double b1 = unit->m_b1;

	if (lag == unit->m_lag) {
		LOOP1(inNumSamples,
			double y0a = ZXP(in);
			y1a = y0a + b1 * (y1a - y0a);
			y1b = y1a + b1 * (y1b - y1a);
			ZXP(out) = y1b;
		);
	} else {
		unit->m_b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
		double b1_slope = CALCSLOPE(unit->m_b1, b1);
		unit->m_lag = lag;
		LOOP1(inNumSamples,
			b1 += b1_slope;
			double y0a = ZXP(in);
			y1a = y0a + b1 * (y1a - y0a);
			y1b = y1a + b1 * (y1b - y1a);
			ZXP(out) = y1b;
		);
	}
	unit->m_y1a = zapgremlins(y1a);
	unit->m_y1b = zapgremlins(y1b);
}

static void Lag2_next_i(Lag2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);

	double y1a = unit->m_y1a;
	double y1b = unit->m_y1b;
	double b1 = unit->m_b1;

	LOOP1(inNumSamples,
			double y0a = ZXP(in);
			y1a = y0a + b1 * (y1a - y0a);
			y1b = y1a + b1 * (y1b - y1a);
			ZXP(out) = y1b;
	);
	unit->m_y1a = zapgremlins(y1a);
	unit->m_y1b = zapgremlins(y1b);
}

static void Lag2_next_1_i(Lag2 *unit, int inNumSamples)
{
	double y1a = unit->m_y1a;
	double y1b = unit->m_y1b;
	double b1 = unit->m_b1;

	float y0a = ZIN0(0);
	y1a = y0a + b1 * (y1a - y0a);
	y1b = y1a + b1 * (y1b - y1a);
	ZOUT0(0) = y1b;

	unit->m_y1a = zapgremlins(y1a);
	unit->m_y1b = zapgremlins(y1b);
}

void Lag2_Ctor(Lag2* unit)
{
	switch (INRATE(1)) {
	case calc_FullRate:
	case calc_BufRate:
		SETCALC(Lag2_next_k);
		break;

	default:
		if (BUFLENGTH == 1)
			SETCALC(Lag2_next_1_i);
		else
			SETCALC(Lag2_next_i);
		break;
	}

	unit->m_lag = uninitializedControl;
	unit->m_b1 = 0.f;
	unit->m_y1a = unit->m_y1b = ZIN0(0);
	Lag2_next_k(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void Lag2UD_next(Lag2UD *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float lagu = ZIN0(1);
	float lagd = ZIN0(2);

	double y1a = unit->m_y1a;
	double y1b = unit->m_y1b;
	double b1u = unit->m_b1u;
	double b1d = unit->m_b1d;

	if ( (lagu == unit->m_lagu) && (lagd == unit->m_lagd) ) {
		LOOP1(inNumSamples,
			double y0a = ZXP(in);
			if ( y0a > y1a ) {
				y1a = y0a + b1u * (y1a - y0a);
			} else {
				y1a = y0a + b1d * (y1a - y0a);
			}
			if ( y1a > y1b )
				y1b = y1a + b1u * (y1b - y1a);
			else
				y1b = y1a + b1d * (y1b - y1a);
			ZXP(out) = y1b;
		);
	} else {
		unit->m_b1u = lagu == 0.f ? 0.f : exp(log001 / (lagu * unit->mRate->mSampleRate));
		double b1u_slope = CALCSLOPE(unit->m_b1u, b1u);
		unit->m_lagu = lagu;
		unit->m_b1d = lagd == 0.f ? 0.f : exp(log001 / (lagd * unit->mRate->mSampleRate));
		double b1d_slope = CALCSLOPE(unit->m_b1d, b1d);
		unit->m_lagd = lagd;
		LOOP1(inNumSamples,
			b1u += b1u_slope;
			b1d += b1d_slope;
			double y0a = ZXP(in);
			if ( y0a > y1a ) {
				y1a = y0a + b1u * (y1a - y0a);
			} else {
				y1a = y0a + b1d * (y1a - y0a);
			}
			if ( y1a > y1b )
				y1b = y1a + b1u * (y1b - y1a);
			else
				y1b = y1a + b1d * (y1b - y1a);
			ZXP(out) = y1b;
		);
	}
	unit->m_y1a = zapgremlins(y1a);
	unit->m_y1b = zapgremlins(y1b);
}

void Lag2UD_Ctor(Lag2UD* unit)
{
	SETCALC(Lag2UD_next);

	unit->m_lagu = 0.f;
	unit->m_lagd = 0.f;
	unit->m_b1u = 0.f;
	unit->m_b1d = 0.f;
	unit->m_y1a = unit->m_y1b = ZIN0(0);
	Lag2UD_next(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void Lag3_next(Lag3 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float lag = ZIN0(1);

	double y1a = unit->m_y1a;
	double y1b = unit->m_y1b;
	double y1c = unit->m_y1c;
	double b1 = unit->m_b1;

	if (lag == unit->m_lag) {
		LOOP1(inNumSamples,
			double y0a = ZXP(in);
			y1a = y0a + b1 * (y1a - y0a);
			y1b = y1a + b1 * (y1b - y1a);
			y1c = y1b + b1 * (y1c - y1b);
			ZXP(out) = y1c;
		);
	} else {
		unit->m_b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
		double b1_slope = CALCSLOPE(unit->m_b1, b1);
		unit->m_lag = lag;
		LOOP1(inNumSamples,
			b1 += b1_slope;
			double y0a = ZXP(in);
			y1a = y0a + b1 * (y1a - y0a);
			y1b = y1a + b1 * (y1b - y1a);
			y1c = y1b + b1 * (y1c - y1b);
			ZXP(out) = y1c;
		);
	}
	unit->m_y1a = zapgremlins(y1a);
	unit->m_y1b = zapgremlins(y1b);
	unit->m_y1c = zapgremlins(y1c);
}

static void Lag3_next_1_i(Lag3 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);

	double y1a = unit->m_y1a;
	double y1b = unit->m_y1b;
	double y1c = unit->m_y1c;
	double b1 = unit->m_b1;

	double y0a = ZXP(in);
	y1a = y0a + b1 * (y1a - y0a);
	y1b = y1a + b1 * (y1b - y1a);
	y1c = y1b + b1 * (y1c - y1b);
	ZXP(out) = y1c;

	unit->m_y1a = zapgremlins(y1a);
	unit->m_y1b = zapgremlins(y1b);
	unit->m_y1c = zapgremlins(y1c);
}

void Lag3_Ctor(Lag3* unit)
{
	switch (INRATE(1)) {
	case calc_FullRate:
	case calc_BufRate:
		SETCALC(Lag3_next);
		break;

	default:
		if (BUFLENGTH == 1)
			SETCALC(Lag3_next_1_i);
		else
			SETCALC(Lag3_next);
		break;
	}

	unit->m_lag = uninitializedControl;
	unit->m_b1 = 0.f;
	unit->m_y1a = unit->m_y1b = unit->m_y1c = ZIN0(0);
	Lag3_next(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void Lag3UD_next(Lag3UD *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float lagu = ZIN0(1);
	float lagd = ZIN0(2);

	double y1a = unit->m_y1a;
	double y1b = unit->m_y1b;
	double y1c = unit->m_y1c;
	double b1u = unit->m_b1u;
	double b1d = unit->m_b1d;

	if ( (lagu == unit->m_lagu) && (lagd == unit->m_lagd) ) {
		LOOP1(inNumSamples,
			double y0a = ZXP(in);
			if ( y0a > y1a ) {
				y1a = y0a + b1u * (y1a - y0a);
			} else {
				y1a = y0a + b1d * (y1a - y0a);
			}
			if ( y1a > y1b ) {
				y1b = y1a + b1u * (y1b - y1a);
			} else {
				y1b = y1a + b1d * (y1b - y1a);
			}
			if ( y1a > y1b ) {
				y1c = y1b + b1u * (y1c - y1b);
			} else {
				y1c = y1b + b1d * (y1c - y1b);
			}
			ZXP(out) = y1c;
		);
	} else {
		unit->m_b1u = lagu == 0.f ? 0.f : exp(log001 / (lagu * unit->mRate->mSampleRate));
		double b1u_slope = CALCSLOPE(unit->m_b1u, b1u);
		unit->m_lagu = lagu;
		unit->m_b1d = lagd == 0.f ? 0.f : exp(log001 / (lagd * unit->mRate->mSampleRate));
		double b1d_slope = CALCSLOPE(unit->m_b1d, b1d);
		unit->m_lagd = lagd;
		LOOP1(inNumSamples,
			b1u += b1u_slope;
			b1d += b1d_slope;
			double y0a = ZXP(in);
			if ( y0a > y1a ) {
				y1a = y0a + b1u * (y1a - y0a);
			} else {
				y1a = y0a + b1d * (y1a - y0a);
			}
			if ( y1a > y1b ) {
				y1b = y1a + b1u * (y1b - y1a);
			} else {
				y1b = y1a + b1d * (y1b - y1a);
			}
			if ( y1a > y1b ) {
				y1c = y1b + b1u * (y1c - y1b);
			} else {
				y1c = y1b + b1d * (y1c - y1b);
			}
			ZXP(out) = y1c;
		);
	}
	unit->m_y1a = zapgremlins(y1a);
	unit->m_y1b = zapgremlins(y1b);
	unit->m_y1c = zapgremlins(y1c);
}

void Lag3UD_Ctor(Lag3UD* unit)
{
	SETCALC(Lag3UD_next);

	unit->m_lagu = uninitializedControl;
	unit->m_lagd = uninitializedControl;
	unit->m_b1u = 0.f;
	unit->m_b1d = 0.f;

	unit->m_y1a = unit->m_y1b = unit->m_y1c = ZIN0(0);
	Lag3UD_next(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void VarLag_next(VarLag *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = IN(0);
	float lagTime = ZIN0(1);
	double slope = unit->m_slope;
	double level = unit->m_level;
	int counter = unit->m_counter;
	int remain = inNumSamples;

	if ( *in != unit->m_in) {
		counter = (int)(lagTime * SAMPLERATE);
		counter = unit->m_counter = sc_max(1, counter);
		slope = unit->m_slope = ( *in - unit->m_level) / counter;
		unit->m_in = *in;
		unit->m_lagTime = lagTime;
	} else {
		if (lagTime != unit->m_lagTime) {
			float scaleFactor = lagTime/unit->m_lagTime;
			counter = (int) (unit->m_counter * scaleFactor);
			counter = unit->m_counter = sc_max(1, counter);
			slope = unit->m_slope / scaleFactor;
			unit->m_lagTime = lagTime;
		}
	}
	if(counter >0) {
		LOOP(remain,
			 ZXP(out) = level;
			 if( counter > 0) {
			 level += slope; --counter;
			 } else {
			 level = unit->m_in;
			 };
			 )
	} else {
		LOOP(remain, ZXP(out) = level );
	}

	unit->m_level = level;
	unit->m_slope = slope;
	unit->m_counter = counter;
}


void VarLag_next_1(VarLag *unit, int inNumSamples)
{
	float *out = OUT(0);
	float in = *IN(0);
	float lagTime = ZIN0(1);
	int counter = unit->m_counter;
	if ( in != unit->m_in) {
		counter = (int)(lagTime * SAMPLERATE);
		unit->m_counter = counter = sc_max(1, counter);
		unit->m_slope = ( in - unit->m_level) / counter;
		unit->m_in = in;
		unit->m_lagTime = lagTime;
	} {
		if (lagTime != unit->m_lagTime) {
			if (counter != 0) {
				double scaleFactor = lagTime/unit->m_lagTime;
				counter = (int) (unit->m_counter * scaleFactor);
				unit->m_counter = counter = sc_max(1, counter);
			unit->m_slope = unit->m_slope / scaleFactor; }
		unit->m_lagTime = lagTime; }
	}
	*out = unit->m_level;

	if (unit->m_counter > 0) {
		unit->m_level += unit->m_slope;
		--unit->m_counter;
	} else {
		unit->m_level = unit->m_in;
	}

}

void VarLag_Ctor(VarLag* unit)
{
	if (BUFLENGTH == 1) {
		SETCALC(VarLag_next_1);
	} else {
		SETCALC(VarLag_next);
	}
	float in = *IN(0);
	float lagTime = ZIN0(1);
	unit->m_level = ZIN0(2);
	int counter = (int)(lagTime * SAMPLERATE);
	unit->m_counter = counter = sc_max(1, counter);
	unit->m_slope = ( in - unit->m_level) / counter;
	unit->m_in = in;
	unit->m_lagTime = lagTime;
	ZOUT0(0) = unit->m_level;
}


//////////////////////////////////////////////////////////////////////////////////////////////////

void OnePole_next_a(OnePole *unit, int inNumSamples)
{
	//printf("OnePole_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *b1p = ZIN(1);

	double y1 = unit->m_y1;

	LOOP1(inNumSamples,
		double y0 = ZXP(in);
		double b1 = ZXP(b1p);
		ZXP(out) = y1 = y0 + b1 * (y1 - y0);
	);
	unit->m_y1 = zapgremlins(y1);
}

void OnePole_next_k(OnePole *unit, int inNumSamples)
{
	//printf("OnePole_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	double b1 = unit->m_b1;
	unit->m_b1 = ZIN0(1);

	double y1 = unit->m_y1;

	if (b1 == unit->m_b1) {
		if (b1 >= 0.f) {
			LOOP1(inNumSamples,
				double y0 = ZXP(in);
				ZXP(out) = y1 = y0 + b1 * (y1 - y0);
			);
		} else {
			LOOP1(inNumSamples,
				double y0 = ZXP(in);
				ZXP(out) = y1 = y0 + b1 * (y1 + y0);
			);
		}
	} else {
		double b1_slope = CALCSLOPE(unit->m_b1, b1);
		if (b1 >= 0.f && unit->m_b1 >= 0) {
			LOOP1(inNumSamples,
				double y0 = ZXP(in);
				ZXP(out) = y1 = y0 + b1 * (y1 - y0);
				b1 += b1_slope;
			);
		} else if (b1 <= 0.f && unit->m_b1 <= 0) {
			LOOP1(inNumSamples,
				double y0 = ZXP(in);
				ZXP(out) = y1 = y0 + b1 * (y1 + y0);
				b1 += b1_slope;
			);
		} else {
			LOOP1(inNumSamples,
				double y0 = ZXP(in);
				ZXP(out) = y1 = (1.f - std::abs(b1)) * y0 + b1 * y1;
				b1 += b1_slope;
			);
		}
	}
	unit->m_y1 = zapgremlins(y1);
}

void OnePole_Ctor(OnePole* unit)
{
	if (INRATE(1) == calc_FullRate) {
		SETCALC(OnePole_next_a);
	} else {
		SETCALC(OnePole_next_k);
	}
	unit->m_b1 = ZIN0(1);
	unit->m_y1 = 0.f;
	OnePole_next_a(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void OneZero_Ctor(OneZero* unit)
{
	SETCALC(OneZero_next);
	unit->m_b1 = ZIN0(1);
	unit->m_x1 = ZIN0(0);
	OneZero_next(unit, 1);
}


void OneZero_next(OneZero* unit, int inNumSamples)
{
	//printf("OneZero::next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	double b1 = unit->m_b1;
	unit->m_b1 = ZIN0(1);

	double x1 = unit->m_x1;
	if (b1 == unit->m_b1) {
		if (b1 >= 0.f) {
			LOOP1(inNumSamples,
				double x0 = ZXP(in);
				ZXP(out) = x0 + b1 * (x1 - x0);
				x1 = x0;
			);
		} else {
			LOOP1(inNumSamples,
				double x0 = ZXP(in);
				ZXP(out) = x0 + b1 * (x1 + x0);
				x1 = x0;
			);
		}
	} else {
		double b1_slope = CALCSLOPE(unit->m_b1, b1);
		if (b1 >= 0.f && unit->m_b1 >= 0) {
			LOOP1(inNumSamples,
				double x0 = ZXP(in);
				ZXP(out) = x0 + b1 * (x1 - x0);
				x1 = x0;
				b1 += b1_slope;
			);
		} else if (b1 <= 0.f && unit->m_b1 <= 0) {
			LOOP1(inNumSamples,
				double x0 = ZXP(in);
				ZXP(out) = x0 + b1 * (x1 + x0);
				x1 = x0;
				b1 += b1_slope;
			);
		} else {
			LOOP1(inNumSamples,
				double x0 = ZXP(in);
				ZXP(out) = (1.f - std::abs(b1)) * x0 + b1 * x1;
				x1 = x0;
				b1 += b1_slope;
			);
		}
	}
	unit->m_x1 = x1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Integrator_Ctor(Integrator* unit)
{
	if (INRATE(1) == calc_ScalarRate)
		SETCALC(Integrator_next_i);
	else
		SETCALC(Integrator_next);
	unit->m_b1 = ZIN0(1);
	unit->m_y1 = 0.f;
	Integrator_next(unit, 1);
}

void Integrator_next_i(Integrator* unit, int inNumSamples)
{
	float *out   = ZOUT(0);
	float *in    = ZIN(0);

	double b1 = unit->m_b1;
	double y1 = unit->m_y1;

	if (b1 == 1.f) {
		LOOP1(inNumSamples,
			double y0 = ZXP(in);
			ZXP(out) = y1 = y0 + y1;
		);
	} else if (b1 == 0.f) {
		LOOP1(inNumSamples,
			double y0 = ZXP(in);
			ZXP(out) = y1 = y0;
		);
	} else {
		LOOP1(inNumSamples,
			double y0 = ZXP(in);
			ZXP(out) = y1 = y0 + b1 * y1;
		);
	}
	unit->m_y1 = zapgremlins(y1);
}

void Integrator_next(Integrator* unit, int inNumSamples)
{
	float *out   = ZOUT(0);
	float *in    = ZIN(0);
	double newB1 = ZIN0(1);

	double b1 = unit->m_b1;
	double y1 = unit->m_y1;

	if (b1 == newB1) {
		if (b1 == 1.f) {
			LOOP1(inNumSamples,
				double y0 = ZXP(in);
				ZXP(out) = y1 = y0 + y1;
			);
		} else if (b1 == 0.f) {
			LOOP1(inNumSamples,
				double y0 = ZXP(in);
				ZXP(out) = y1 = y0;
			);
		} else {
			LOOP1(inNumSamples,
				double y0 = ZXP(in);
				ZXP(out) = y1 = y0 + b1 * y1;
			);
		}
	} else {
		double b1_slope = CALCSLOPE(newB1, b1);
		LOOP1(inNumSamples,
			double y0 = ZXP(in);
			ZXP(out) = y1 = y0 + b1 * y1;
			b1 += b1_slope;
		);

		unit->m_b1 = newB1;
	}
	unit->m_y1 = zapgremlins(y1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Decay_Ctor(Decay* unit)
{
	SETCALC(Decay_next);
	unit->m_decayTime = uninitializedControl;
	unit->m_b1 = 0.f;
	unit->m_y1 = 0.f;
	Decay_next(unit, 1);
}

void Decay_next(Decay* unit, int inNumSamples)
{
	//printf("Decay_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float decayTime = ZIN0(1);

	double y1 = unit->m_y1;
	double b1 = unit->m_b1;
	if (decayTime == unit->m_decayTime) {
		if (b1 == 0.f) {
			LOOP1(inNumSamples,
				double y0 = ZXP(in);
				ZXP(out) = y1 = y0;
			);
		} else {
			LOOP1(inNumSamples,
				double y0 = ZXP(in);
				ZXP(out) = y1 = y0 + b1 * y1;
			);
		}
	} else {
		unit->m_b1 = decayTime == 0.f ? 0.f : exp(log001 / (decayTime * SAMPLERATE));
		unit->m_decayTime = decayTime;
		double b1_slope = CALCSLOPE(unit->m_b1, b1);
		//printf("decayTime %g  %g %g\n", unit->m_decayTime, next_b1, b1);
		LOOP1(inNumSamples,
			double y0 = ZXP(in);
			ZXP(out) = y1 = y0 + b1 * y1;
			b1 += b1_slope;
		);
	}
	unit->m_y1 = zapgremlins(y1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void Decay2_Ctor(Decay2 *unit)
{
	SETCALC(Decay2_next);

	float attackTime = ZIN0(1);
	float decayTime = ZIN0(2);
	unit->m_b1a = decayTime  == 0.f ? 0.f : exp(log001 / (decayTime * SAMPLERATE));
	unit->m_b1b = attackTime == 0.f ? 0.f : exp(log001 / (attackTime * SAMPLERATE));
	unit->m_decayTime = decayTime;
	unit->m_attackTime = attackTime;

	float y0 = ZIN0(0);
	unit->m_y1a = y0;
	unit->m_y1b = y0;
	ZOUT0(0) = 0.f;
}

void Decay2_next(Decay2* unit, int inNumSamples)
{
	//printf("Decay2_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float attackTime = ZIN0(1);
	float decayTime = ZIN0(2);

	double y1a = unit->m_y1a;
	double y1b = unit->m_y1b;
	double b1a = unit->m_b1a;
	double b1b = unit->m_b1b;
	if (decayTime == unit->m_decayTime && attackTime == unit->m_attackTime) {
		LOOP1(inNumSamples,
			double y0 = ZXP(in);
			y1a = y0 + b1a * y1a;
			y1b = y0 + b1b * y1b;
			ZXP(out) = y1a - y1b;
		);
	} else {
		unit->m_decayTime = decayTime;
		unit->m_attackTime = attackTime;
		double next_b1a = decayTime  == 0.f ? 0.f : exp(log001 / (decayTime * SAMPLERATE));
		double next_b1b = attackTime == 0.f ? 0.f : exp(log001 / (attackTime * SAMPLERATE));
		unit->m_decayTime = decayTime;
		double b1a_slope = CALCSLOPE(next_b1a, b1a);
		double b1b_slope = CALCSLOPE(next_b1b, b1b);
		unit->m_b1a = next_b1a;
		unit->m_b1b = next_b1b;
		LOOP1(inNumSamples,
			double y0 = ZXP(in);
			y1a = y0 + b1a * y1a;
			y1b = y0 + b1b * y1b;
			ZXP(out) = y1a - y1b;
			b1a += b1a_slope;
			b1b += b1b_slope;
		);
	}
	unit->m_y1a = y1a;
	unit->m_y1b = y1b;
	unit->m_b1a = b1a;
	unit->m_b1b = b1b;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void LeakDC_next_i_4(LeakDC* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	double b1 = unit->m_b1;
	double y1 = unit->m_y1;
	double x1 = unit->m_x1;

	LOOP1(inNumSamples/4,
		double x00 = ZXP(in);
		double x01 = ZXP(in);
		double x02 = ZXP(in);
		double x03 = ZXP(in);
		float out0 = y1 = x00 - x1 + b1 * y1;
		float out1 = y1 = x01 - x00 + b1 * y1;
		float out2 = y1 = x02 - x01 + b1 * y1;
		float out3 = y1 = x03 - x02 + b1 * y1;

		ZXP(out) = out0;
		ZXP(out) = out1;
		ZXP(out) = out2;
		ZXP(out) = out3;

		x1 = x03;
	);
	unit->m_x1 = x1;
	unit->m_y1 = zapgremlins(y1);
}

void LeakDC_next_i(LeakDC* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	double b1 = unit->m_b1;
	double y1 = unit->m_y1;
	double x1 = unit->m_x1;

	LOOP1(inNumSamples,
		double x0 = ZXP(in);
		ZXP(out) = y1 = x0 - x1 + b1 * y1;
		x1 = x0;
	);
	unit->m_x1 = x1;
	unit->m_y1 = zapgremlins(y1);
}

void LeakDC_next(LeakDC* unit, int inNumSamples)
{
	if (ZIN0(1) == unit->m_b1) {
		if ((inNumSamples & 3) == 0)
			LeakDC_next_i_4(unit, inNumSamples);
		else
			LeakDC_next_i(unit, inNumSamples);
	} else {
		float *out = ZOUT(0);
		float *in = ZIN(0);
		double b1 = unit->m_b1;
		unit->m_b1 = ZIN0(1);

		double y1 = unit->m_y1;
		double x1 = unit->m_x1;

		double b1_slope = CALCSLOPE(unit->m_b1, b1);
		LOOP1(inNumSamples,
			double x0 = ZXP(in);
			ZXP(out) = y1 = x0 - x1 + b1 * y1;
			x1 = x0;
			b1 += b1_slope;
		);
		unit->m_x1 = x1;
		unit->m_y1 = zapgremlins(y1);
	}
}


void LeakDC_next_1(LeakDC* unit, int inNumSamples)
{
	double b1 = unit->m_b1 = ZIN0(1);

	double y1 = unit->m_y1;
	double x1 = unit->m_x1;

	double x0 = ZIN0(0);
	ZOUT0(0) = y1 = x0 - x1 + b1 * y1;
	x1 = x0;

	unit->m_x1 = x1;
	unit->m_y1 = zapgremlins(y1);
}

void LeakDC_Ctor(LeakDC *unit)
{
	//printf("LeakDC_Ctor\n");
	if (BUFLENGTH == 1)
		SETCALC(LeakDC_next_1);
	else {
		if (INRATE(1) == calc_ScalarRate) {
			if ((BUFLENGTH & 3) == 0)
				SETCALC(LeakDC_next_i_4);
			else
				SETCALC(LeakDC_next_i);
		} else
			SETCALC(LeakDC_next);
	}
	unit->m_b1 = 0.0;
	unit->m_x1 = ZIN0(0);
	unit->m_y1 = 0.0;
	LeakDC_next_1(unit, 1);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void TwoPole_Ctor(TwoPole *unit)
{
	//printf("TwoPole_Reset\n");
	SETCALC(TwoPole_next);
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = uninitializedControl;
	unit->m_reson = uninitializedControl;
	PUSH_LOOPVALS
	TwoPole_next(unit, 1);
	POP_LOOPVALS
}


void TwoPole_next(TwoPole* unit, int inNumSamples)
{
	//printf("TwoPole_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float reson = ZIN0(2);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;

	if (freq != unit->m_freq || reson != unit->m_reson) {
		double b1 = unit->m_b1;
		double b2 = unit->m_b2;
		double b1_next = 2 * reson * cos(freq * unit->mRate->mRadiansPerSample);
		double b2_next = -(reson * reson);
		double b1_slope = (b1_next - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (b2_next - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			ZXP(out) = y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = y2 = ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = y1 = ZXP(in) + b1 * y2 + b2 * y0;

			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			ZXP(out) = y0 = ZXP(in) + b1 * y1 + b2 * y2;
			y2 = y1;
			y1 = y0;
		);

		unit->m_freq = freq;
		unit->m_reson = reson;
		unit->m_b1 = b1_next;
		unit->m_b2 = b2_next;
	} else {
		double b1 = unit->m_b1;
		double b2 = unit->m_b2;
		LOOP(unit->mRate->mFilterLoops,
			ZXP(out) = y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = y2 = ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = y1 = ZXP(in) + b1 * y2 + b2 * y0;
		);
		LOOP(unit->mRate->mFilterRemain,
			ZXP(out) = y0 = ZXP(in) + b1 * y1 + b2 * y2;
			y2 = y1;
			y1 = y0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


void TwoZero_Ctor(TwoZero* unit)
{
	//printf("TwoZero_Reset\n");
	SETCALC(TwoZero_next);
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_x1 = 0.f;
	unit->m_x2 = 0.f;
	unit->m_freq = uninitializedControl;
	unit->m_reson = uninitializedControl;
	PUSH_LOOPVALS
	TwoZero_next(unit, 1);
	POP_LOOPVALS
}

void TwoZero_next(TwoZero* unit, int inNumSamples)
{
	//printf("TwoZero_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float reson = ZIN0(2);

	double x0;
	double x1 = unit->m_x1;
	double x2 = unit->m_x2;

	if (freq != unit->m_freq || reson != unit->m_reson) {
		double b1 = unit->m_b1;
		double b2 = unit->m_b2;
		double b1_next = -2 * reson * cos(freq * unit->mRate->mRadiansPerSample);
		double b2_next = (reson * reson);
		double b1_slope = (b1_next - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (b2_next - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			x0 = ZXP(in);
			ZXP(out) = x0 + b1 * x1 + b2 * x2;
			x2 = ZXP(in);
			ZXP(out) = x2 + b1 * x0 + b2 * x1;
			x1 = ZXP(in);
			ZXP(out) = x1 + b1 * x2 + b2 * x0;

			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			x0 = ZXP(in);
			ZXP(out) = x0 + b1 * x1 + b2 * x2;
			x2 = x1;
			x1 = x0;
		);

		unit->m_freq = freq;
		unit->m_reson = reson;
		unit->m_b1 = b1_next;
		unit->m_b2 = b2_next;
	} else {
		double b1 = unit->m_b1;
		double b2 = unit->m_b2;
		LOOP(unit->mRate->mFilterLoops,
			x0 = ZXP(in);
			ZXP(out) = x0 + b1 * x1 + b2 * x2;
			x2 = ZXP(in);
			ZXP(out) = x2 + b1 * x0 + b2 * x1;
			x1 = ZXP(in);
			ZXP(out) = x1 + b1 * x2 + b2 * x0;
		);
		LOOP(unit->mRate->mFilterRemain,
			x0 = ZXP(in);
			ZXP(out) = x0 + b1 * x1 + b2 * x2;
			x2 = x1;
			x1 = x0;
		);
	}
	unit->m_x1 = x1;
	unit->m_x2 = x2;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////


void APF_Ctor(APF* unit)
{
	//printf("APF_Reset\n");
	SETCALC(APF_next);
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_x1 = 0.f;
	unit->m_x2 = 0.f;
	unit->m_freq = uninitializedControl;
	unit->m_reson = uninitializedControl;
	PUSH_LOOPVALS
	APF_next(unit, 1);
	POP_LOOPVALS
}


void APF_next(APF* unit, int inNumSamples)
{
	//printf("APF_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float reson = ZIN0(2);

	double x0, y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double x1 = unit->m_x1;
	double x2 = unit->m_x2;

	if (freq != unit->m_freq || reson != unit->m_reson) {
		double b1 = unit->m_b1;
		double b2 = unit->m_b2;
		double b1_next = 2.f * reson * cos(freq * unit->mRate->mRadiansPerSample);
		double b2_next = -(reson * reson);
		double b1_slope = (b1_next - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (b2_next - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			x0 = ZXP(in);
			ZXP(out) = y0 = x0 + b1 * (y1 - x1) + b2 * (y2 - x2);
			x2 = ZXP(in);
			ZXP(out) = y2 = x2 + b1 * (y0 - x0) + b2 * (y2 - x1);
			x1 = ZXP(in);
			ZXP(out) = y1 = x1 + b1 * (y2 - x2) + b2 * (y2 - x0);

			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			x0 = ZXP(in);
			ZXP(out) = y0 = x0 + b1 * (y1 - x1) + b2 * (y2 - x2);
			y2 = y1;
			y1 = y0;
			x2 = x1;
			x1 = x0;
		);

		unit->m_freq = freq;
		unit->m_reson = reson;
		unit->m_b1 = b1_next;
		unit->m_b2 = b2_next;
	} else {
		double b1 = unit->m_b1;
		double b2 = unit->m_b2;
		LOOP(unit->mRate->mFilterLoops,
			float in0 = ZXP(in);
			float in1 = ZXP(in);
			float in2 = ZXP(in);
			x0 = in0;
			float out0 = y0 = x0 + b1 * (y1 - x1) + b2 * (y2 - x2);
			x2 = in1;
			float out1 = y2 = x2 + b1 * (y0 - x0) + b2 * (y2 - x1);
			x1 = in2;
			float out2 = y1 = x1 + b1 * (y2 - x2) + b2 * (y2 - x0);
			ZXP(out) = out0;
			ZXP(out) = out1;
			ZXP(out) = out2;
		);
		LOOP(unit->mRate->mFilterRemain,
			x0 = ZXP(in);
			ZXP(out) = y0 = x0 + b1 * (y1 - x1) + b2 * (y2 - x2);
			y2 = y1;
			y1 = y0;
			x2 = x1;
			x1 = x0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
	unit->m_x1 = x1;
	unit->m_x2 = x2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void LPZ1_Ctor(LPZ1* unit)
{
	//printf("LPZ1_Reset\n");
	SETCALC(LPZ1_next);
	unit->m_x1 = ZIN0(0);
	LPZ1_next(unit, 1);
}


void LPZ1_next(LPZ1* unit, int inNumSamples)
{
	//printf("LPZ1_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);

	double x0;
	double x1 = unit->m_x1;

	LOOP(inNumSamples >> 2,
		x0 = ZXP(in);
		float out0 = 0.5 * (x0 + x1);
		x1 = ZXP(in);
		float out1 = 0.5 * (x1 + x0);
		x0 = ZXP(in);
		float out2 = 0.5 * (x0 + x1);
		x1 = ZXP(in);
		float out3 = 0.5 * (x1 + x0);

		ZXP(out) = out0;
		ZXP(out) = out1;
		ZXP(out) = out2;
		ZXP(out) = out3;
	);
	LOOP(inNumSamples & 3,
		x0 = ZXP(in);
		ZXP(out) = 0.5 * (x0 + x1);
		x1 = x0;
	);

	unit->m_x1 = x1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void HPZ1_Ctor(HPZ1* unit)
{
	//printf("HPZ1_Reset\n");
	SETCALC(HPZ1_next);
	unit->m_x1 = ZIN0(0);
	HPZ1_next(unit, 1);
}


void HPZ1_next(HPZ1* unit, int inNumSamples)
{
	//printf("HPZ1_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);

	double x0;
	double x1 = unit->m_x1;

	LOOP(inNumSamples >> 2,
		x0 = ZXP(in);
		float out0 = 0.5 * (x0 - x1);
		x1 = ZXP(in);
		float out1 = 0.5 * (x1 - x0);
		x0 = ZXP(in);
		float out2 = 0.5 * (x0 - x1);
		x1 = ZXP(in);
		float out3 = 0.5 * (x1 - x0);

		ZXP(out) = out0;
		ZXP(out) = out1;
		ZXP(out) = out2;
		ZXP(out) = out3;
		);
	LOOP(inNumSamples & 3,
		x0 = ZXP(in);
		//printf("%d %d %g %g\n", this, inNumSamples, x0, x1);
		ZXP(out) = 0.5f * (x0 - x1);
		x1 = x0;
	);

	unit->m_x1 = x1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Slope_Ctor(Slope* unit)
{
	//printf("Slope_Reset\n");
	SETCALC(Slope_next);
	unit->m_x1 = ZIN0(0);
	//printf("Slope_Reset %g\n", unit->m_x1);
	Slope_next(unit, 1);
}


void Slope_next(Slope* unit, int inNumSamples)
{
	//printf("Slope_next_a %g\n", unit->m_x1);

	float *out = ZOUT(0);
	float *in = ZIN(0);

	double x1 = unit->m_x1;

	double sr = SAMPLERATE;
	LOOP1(inNumSamples,
		double x0 = ZXP(in);
		ZXP(out) = sr * (x0 - x1);
		x1 = x0;
	);

	unit->m_x1 = x1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Delay1_Ctor(Delay1* unit)
{
	//printf("Delay1_Reset\n");
	SETCALC(Delay1_next);
	unit->m_x1 = ZIN0(0);
	Delay1_next(unit, 1);
}


void Delay1_next(Delay1* unit, int inNumSamples)
{
	//printf("Delay1_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);

	float x0;
	float x1 = unit->m_x1;

	LOOP(inNumSamples >> 2,
		x0 = ZXP(in);
		ZXP(out) = x1;
		x1 = ZXP(in);
		ZXP(out) = x0;
		x0 = ZXP(in);
		ZXP(out) = x1;
		x1 = ZXP(in);
		ZXP(out) = x0;
	);
	LOOP(inNumSamples & 3,
		x0 = ZXP(in);
		ZXP(out) = x1;
		x1 = x0;
	);

	unit->m_x1 = x1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Flip_Ctor(Flip* unit)
{
	if (unit->mBufLength & 1) {
		SETCALC(Flip_next_odd);
		Flip_next_odd(unit, 1);
	} else {
		SETCALC(Flip_next_even);
		ZOUT0(0) = ZIN0(0);
	}
}


void Flip_next_even(Flip* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);

	LOOP(inNumSamples >> 1,
		ZXP(out) = -ZXP(in);
		ZXP(out) =  ZXP(in);
	);
}

void Flip_next_odd(Flip* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);

	if (unit->mWorld->mBufCounter & 1) {
		ZXP(out) = ZXP(in);
		LOOP(inNumSamples >> 1,
			ZXP(out) = -ZXP(in);
			ZXP(out) =  ZXP(in);
		);
	} else {
		LOOP(inNumSamples >> 1,
			ZXP(out) = -ZXP(in);
			ZXP(out) =  ZXP(in);
		);
		ZXP(out) = -ZXP(in);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Delay2_Ctor(Delay2* unit)
{
	SETCALC(Delay2_next);
	unit->m_x1 = ZIN0(0);
	ZOUT0(0) = 0.f;
}


void Delay2_next(Delay2* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);

	double x0;
	double x1 = unit->m_x1;
	double x2 = unit->m_x2;

	LOOP(unit->mRate->mFilterLoops,
		x0 = ZXP(in);
		ZXP(out) = x2;
		x2 = ZXP(in);
		ZXP(out) = x1;
		x1 = ZXP(in);
		ZXP(out) = x0;
	);
	LOOP(unit->mRate->mFilterRemain,
		x0 = ZXP(in);
		ZXP(out) = x2;
		x2 = x1;
		x1 = x0;
	);

	unit->m_x1 = x1;
	unit->m_x2 = x2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void LPZ2_Ctor(LPZ2* unit)
{
	//printf("LPZ2_Reset\n");
	SETCALC(LPZ2_next);
	unit->m_x1 = unit->m_x2 = ZIN0(0);
	PUSH_LOOPVALS
	LPZ2_next(unit, 1);
	POP_LOOPVALS
}


void LPZ2_next(LPZ2* unit, int inNumSamples)
{
	//printf("LPZ2_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);

	double x0;
	double x1 = unit->m_x1;
	double x2 = unit->m_x2;

	LOOP(unit->mRate->mFilterLoops,
		x0 = ZXP(in);
		float out0  = (x0 + 2.0 * x1 + x2) * 0.25;
		x2 = ZXP(in);
		float out1 = (x2 + 2.0 * x0 + x1) * 0.25;
		x1 = ZXP(in);
		float out2 = (x1 + 2.0 * x2 + x0) * 0.25;

		ZXP(out) = out0;
		ZXP(out) = out1;
		ZXP(out) = out2;
	);
	LOOP(unit->mRate->mFilterRemain,
		x0 = ZXP(in);
		ZXP(out) = (x0 + 2. * x1 + x2) * 0.25;
		x2 = x1;
		x1 = x0;
	);

	unit->m_x1 = x1;
	unit->m_x2 = x2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void HPZ2_Ctor(HPZ2* unit)
{
	//printf("HPZ2_Reset\n");
	SETCALC(HPZ2_next);
	unit->m_x1 = unit->m_x2 = ZIN0(0);
	PUSH_LOOPVALS
	HPZ2_next(unit, 1);
	POP_LOOPVALS
}


void HPZ2_next(HPZ2* unit, int inNumSamples)
{
	//printf("HPZ2_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);

	double x0;
	double x1 = unit->m_x1;
	double x2 = unit->m_x2;

	LOOP(unit->mRate->mFilterLoops,
		x0 = ZXP(in);
		float out0 = (x0 - 2.0 * x1 + x2) * 0.25;
		x2 = ZXP(in);
		float out1 = (x2 - 2.0 * x0 + x1) * 0.25;
		x1 = ZXP(in);
		float out2 = (x1 - 2.0 * x2 + x0) * 0.25;
		ZXP(out) = out0;
		ZXP(out) = out1;
		ZXP(out) = out2;
	);
	LOOP(unit->mRate->mFilterRemain,
		x0 = ZXP(in);
		ZXP(out) = (x0 - 2.0 * x1 + x2) * 0.25;
		x2 = x1;
		x1 = x0;
	);

	unit->m_x1 = x1;
	unit->m_x2 = x2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BPZ2_Ctor(BPZ2* unit)
{
	//printf("BPZ2_Reset\n");
	SETCALC(BPZ2_next);
	unit->m_x1 = unit->m_x2 = ZIN0(0);
	PUSH_LOOPVALS
	BPZ2_next(unit, 1);
	POP_LOOPVALS
}


void BPZ2_next(BPZ2* unit, int inNumSamples)
{
	//printf("BPZ2_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);

	double x0;
	double x1 = unit->m_x1;
	double x2 = unit->m_x2;

	LOOP(unit->mRate->mFilterLoops,
		x0 = ZXP(in);
		ZXP(out) = (x0 - x2) * 0.5;
		x2 = ZXP(in);
		ZXP(out) = (x2 - x1) * 0.5;
		x1 = ZXP(in);
		ZXP(out) = (x1 - x0) * 0.5;
	);
	LOOP(unit->mRate->mFilterRemain,
		x0 = ZXP(in);
		ZXP(out) = (x0 - x2) * 0.5;
		x2 = x1;
		x1 = x0;
	);

	unit->m_x1 = x1;
	unit->m_x2 = x2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BRZ2_Ctor(BRZ2* unit)
{
	//printf("BRZ2_Reset\n");
	SETCALC(BRZ2_next);
	unit->m_x1 = unit->m_x2 = ZIN0(0);
	PUSH_LOOPVALS
	BRZ2_next(unit, 1);
	POP_LOOPVALS
}


void BRZ2_next(BRZ2* unit, int inNumSamples)
{
	//printf("BRZ2_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);

	double x0;
	double x1 = unit->m_x1;
	double x2 = unit->m_x2;

	LOOP(unit->mRate->mFilterLoops,
		x0 = ZXP(in);
		ZXP(out) = (x0 + x2) * 0.5;
		x2 = ZXP(in);
		ZXP(out) = (x2 + x1) * 0.5;
		x1 = ZXP(in);
		ZXP(out) = (x1 + x0) * 0.5;
	);
	LOOP(unit->mRate->mFilterRemain,
		x0 = ZXP(in);
		ZXP(out) = (x0 + x2) * 0.5;
		x2 = x1;
		x1 = x0;
	);

	unit->m_x1 = x1;
	unit->m_x2 = x2;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Slew_Ctor(Slew* unit)
{
	//printf("Slew_Reset\n");
	SETCALC(Slew_next);
	unit->mLevel = ZIN0(0);
	Slew_next(unit, 1);
}


void Slew_next(Slew* unit, int inNumSamples)
{
	//printf("Slew_next_a\n");

	float sampleDur = unit->mRate->mSampleDur;
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float upf = ZIN0(1) * sampleDur;
	float dnf = 0.f - ZIN0(2) * sampleDur;
	double level = unit->mLevel;
	LOOP1(inNumSamples,
		double slope = ZXP(in) - level;
		level += sc_clip(slope, dnf, upf);
		ZXP(out) = level;
	);
	unit->mLevel = level;
}

/*void Slew_next_1(Slew* unit, int inNumSamples)
{
	//printf("Slew_next_a\n");

	float *out = ZOUT(0);
	float in = ZIN0(0);
	float upf = ZIN0(1);
	float dnf = ZIN0(2);
	float level = unit->mLevel;
	float slope = ZXP(in) - level;
	if (slope > upf) slope = upf;
	else if (slope < dnf) slope = dnf;
	level += slope;
	ZXP(out) = level;
	unit->mLevel = level;

}*/

////////////////////////////////////////////////////////////////////////////////////////////////////////

void RLPF_Ctor(RLPF* unit)
{
	//printf("RLPF_Reset\n");
	if (unit->mBufLength == 1) {
		SETCALC(RLPF_next_1);
	} else {
		SETCALC(RLPF_next);
	}

	unit->m_a0 = 0.f;
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = uninitializedControl;
	unit->m_reson = uninitializedControl;
	RLPF_next_1(unit, 1);
}


void RLPF_next(RLPF* unit, int inNumSamples)
{
	//printf("RLPF_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float reson = ZIN0(2);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq || reson != unit->m_reson) {

		double qres = sc_max(0.001, reson);
		double pfreq = freq * unit->mRate->mRadiansPerSample;

		double D = tan(pfreq * qres * 0.5);
		double C = ((1.0-D)/(1.0+D));
		double cosf = cos(pfreq);

		double next_b1 = (1.0 + C) * cosf;
		double next_b2 = -C;
		double next_a0 = (1.0 + C - next_b1) * .25;
		//post("%g %g %g   %g %g   %g %g %g   %g %g\n", *freq, pfreq, qres, D, C, cosf, next_b1, next_b2, next_a0, y1, y2);

		double a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		double b1_slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = y0 + 2.0 * y1 + y2;

			y2 = a0 * ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = y2 + 2.0 * y0 + y1;

			y1 = a0 * ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = y1 + 2.0 * y2 + y0;

			a0 += a0_slope;
			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = y0 + 2.0 * y1 + y2;
			y2 = y1;
			y1 = y0;
		);

		unit->m_freq = freq;
		unit->m_reson = reson;
		unit->m_a0 = next_a0;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = y0 + 2.0 * y1 + y2;

			y2 = a0 * ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = y2 + 2.0 * y0 + y1;

			y1 = a0 * ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = y1 + 2.0 * y2 + y0;
		);
		LOOP(unit->mRate->mFilterRemain,
			y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = y0 + 2.0 * y1 + y2;
			y2 = y1;
			y1 = y0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void RLPF_next_1(RLPF* unit, int inNumSamples)
{
	//printf("RLPF_next_1\n");

	float in = ZIN0(0);
	float freq = ZIN0(1);
	float reson = ZIN0(2);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq || reson != unit->m_reson) {
		double qres = sc_max(0.001f, reson);
		double pfreq = freq * unit->mRate->mRadiansPerSample;

		double D = tan(pfreq * qres * 0.5f);
		double C = ((1.f-D)/(1.f+D));
		double cosf = cos(pfreq);

		b1 = (1.0 + C) * cosf;
		b2 = -C;
		a0 = (1.0 + C - b1) * .25;

		y0 = a0 * in + b1 * y1 + b2 * y2;
		ZOUT0(0) = y0 + 2.0 * y1 + y2;
		y2 = y1;
		y1 = y0;

		unit->m_freq = freq;
		unit->m_reson = reson;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {
		y0 = a0 * in + b1 * y1 + b2 * y2;
		ZOUT0(0) = y0 + 2.0 * y1 + y2;
		y2 = y1;
		y1 = y0;
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);

}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void RHPF_Ctor(RHPF* unit)
{
	if (unit->mBufLength == 1)
		SETCALC(RHPF_next_1);
	else
		SETCALC(RHPF_next);

	unit->m_a0 = 0.;
	unit->m_b1 = 0.;
	unit->m_b2 = 0.;
	unit->m_y1 = 0.;
	unit->m_y2 = 0.;
	unit->m_freq = uninitializedControl;
	unit->m_reson = uninitializedControl;
	RHPF_next_1(unit, 1);
}


void RHPF_next(RHPF* unit, int inNumSamples)
{
	//printf("RHPFs_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float reson = ZIN0(2);

	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq || reson != unit->m_reson) {
		double qres = sc_max(0.001f, reson);
		double pfreq = freq * unit->mRate->mRadiansPerSample;

		double D = tan(pfreq * qres * 0.5f);
		double C = ((1.0-D)/(1.0+D));
		double cosf = cos(pfreq);

		double next_b1 = (1.0 + C) * cosf;
		double next_b2 = -C;
		double next_a0 = (1.0 + C + next_b1) * .25;

		//post("%g %g %g   %g %g   %g %g %g   %g %g\n", *freq, pfreq, qres, D, C, cosf, next_b1, next_b2, next_a0, y1, y2);

		double a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		double b1_slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			double y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = y0 - 2.0 * y1 + y2;

			y2 = a0 * ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = y2 - 2.0 * y0 + y1;

			y1 = a0 * ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = y1 - 2.0 * y2 + y0;

			a0 += a0_slope;
			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			double y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = y0 - 2.0 * y1 + y2;
			y2 = y1;
			y1 = y0;
		);

		unit->m_freq = freq;
		unit->m_reson = reson;
		unit->m_a0 = next_a0;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			double y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = y0 - 2.0 * y1 + y2;

			y2 = a0 * ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = y2 - 2.0 * y0 + y1;

			y1 = a0 * ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = y1 - 2.0 * y2 + y0;
		);
		LOOP(unit->mRate->mFilterRemain,
			double y0 = a0 * ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = y0 - 2.0 * y1 + y2;
			y2 = y1;
			y1 = y0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void RHPF_next_1(RHPF* unit, int inNumSamples)
{
	//printf("RHPFs_next_1\n");

	float in = ZIN0(0);
	float freq = ZIN0(1);
	float reson = ZIN0(2);

	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq || reson != unit->m_reson) {
		double qres = sc_max(0.001f, reson);
		double pfreq = freq * unit->mRate->mRadiansPerSample;

		double D = tan(pfreq * qres * 0.5f);
		double C = ((1.-D)/(1.+D));
		double cosf = cos(pfreq);

		b1 = (1. + C) * cosf;
		b2 = -C;
		a0 = (1. + C + b1) * .25;

		double y0 = a0 * in + b1 * y1 + b2 * y2;
		ZOUT0(0) = y0 - 2.0 * y1 + y2;
		y2 = y1;
		y1 = y0;

		unit->m_freq = freq;
		unit->m_reson = reson;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {
		double y0 = a0 * in + b1 * y1 + b2 * y2;
		ZOUT0(0) = y0 - 2.0 * y1 + y2;
		y2 = y1;
		y1 = y0;
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void LPF_Ctor(LPF* unit)
{
	if (unit->mBufLength == 1)
		SETCALC(LPF_next_1);
	else
		SETCALC(LPF_next);

	unit->m_a0 = 0.f;
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = uninitializedControl;
	LPF_next_1(unit, 1);
}


void LPF_next(LPF* unit, int inNumSamples)
{
	//printf("LPF_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq) {

		double pfreq = freq * unit->mRate->mRadiansPerSample * 0.5;

		double C = 1.f / tan(pfreq);
		double C2 = C * C;
		double sqrt2C = C * sqrt2_f;
		double next_a0 = 1.f / (1.f + sqrt2C + C2);
		double next_b1 = -2.f * (1.f - C2) * next_a0 ;
		double next_b2 = -(1.f - sqrt2C + C2) * next_a0;

		//post("%g %g %g   %g %g   %g %g %g   %g %g\n", *freq, pfreq, qres, D, C, cosf, next_b1, next_b2, next_a0, y1, y2);

		double a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		double b1_slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 + 2. * y1 + y2);

			y2 = ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = a0 * (y2 + 2. * y0 + y1);

			y1 = ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = a0 * (y1 + 2.f * y2 + y0);

			a0 += a0_slope;
			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 + 2. * y1 + y2);
			y2 = y1;
			y1 = y0;
		);

		unit->m_freq = freq;
		unit->m_a0 = next_a0;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			float out0 = a0 * (y0 + 2. * y1 + y2);

			y2 = ZXP(in) + b1 * y0 + b2 * y1;
			float out1 = a0 * (y2 + 2. * y0 + y1);

			y1 = ZXP(in) + b1 * y2 + b2 * y0;
			float out2 = a0 * (y1 + 2. * y2 + y0);
			ZXP(out) = out0;
			ZXP(out) = out1;
			ZXP(out) = out2;
		);
		LOOP(unit->mRate->mFilterRemain,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 + 2. * y1 + y2);
			y2 = y1;
			y1 = y0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void LPF_next_1(LPF* unit, int inNumSamples)
{
	//printf("LPF_next\n");
	float in = ZIN0(0);
	float freq = ZIN0(1);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq) {

		double pfreq = freq * unit->mRate->mRadiansPerSample * 0.5;

		double C = 1.f / tan(pfreq);
		double C2 = C * C;
		double sqrt2C = C * sqrt2_f;
		a0 = 1.f / (1.f + sqrt2C + C2);
		b1 = -2.f * (1.f - C2) * a0 ;
		b2 = -(1.f - sqrt2C + C2) * a0;

		y0 = in + b1 * y1 + b2 * y2;
		ZOUT0(0) = a0 * (y0 + 2. * y1 + y2);
		y2 = y1;
		y1 = y0;

		unit->m_freq = freq;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {

		y0 = in + b1 * y1 + b2 * y2;
		ZOUT0(0) = a0 * (y0 + 2. * y1 + y2);
		y2 = y1;
		y1 = y0;

	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void HPF_Ctor(HPF* unit)
{
	if (unit->mBufLength == 1)
		SETCALC(HPF_next_1);
	else
		SETCALC(HPF_next);
	unit->m_a0 = 0.;
	unit->m_b1 = 0.;
	unit->m_b2 = 0.;
	unit->m_y1 = 0.;
	unit->m_y2 = 0.;
	unit->m_freq = uninitializedControl;

	HPF_next_1(unit, 1);
}


void HPF_next(HPF* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);

	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq) {
		double pfreq = freq * unit->mRate->mRadiansPerSample * 0.5;

		double C = tan(pfreq);
		double C2 = C * C;
		double sqrt2C = C * sqrt2_f;
		double next_a0 = 1. / (1. + sqrt2C + C2);
		double next_b1 = 2. * (1. - C2) * next_a0 ;
		double next_b2 = -(1. - sqrt2C + C2) * next_a0;

		//post("%g %g %g   %g %g   %g %g %g   %g %g\n", *freq, pfreq, qres, D, C, cosf, next_b1, next_b2, next_a0, y1, y2);

		double a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		double b1_slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			double in0 = ZXP(in);
			double in1 = ZXP(in);
			double in2 = ZXP(in);

			double y0 = in0 + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - 2. * y1 + y2);

			y2 = in1 + b1 * y0 + b2 * y1;
			ZXP(out) = a0 * (y2 - 2. * y0 + y1);

			y1 = in2 + b1 * y2 + b2 * y0;
			ZXP(out) = a0 * (y1 - 2. * y2 + y0);

			a0 += a0_slope;
			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			double y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - 2. * y1 + y2);
			y2 = y1;
			y1 = y0;
		);

		unit->m_freq = freq;
		unit->m_a0 = next_a0;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			double in0 = ZXP(in);
			double in1 = ZXP(in);
			double in2 = ZXP(in);

			double y0 = in0 + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - 2. * y1 + y2);

			y2 = in1 + b1 * y0 + b2 * y1;
			ZXP(out) = a0 * (y2 - 2. * y0 + y1);

			y1 = in2 + b1 * y2 + b2 * y0;
			ZXP(out) = a0 * (y1 - 2. * y2 + y0);
		);
		LOOP(unit->mRate->mFilterRemain,
			double y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - 2. * y1 + y2);
			y2 = y1;
			y1 = y0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void HPF_next_1(HPF* unit, int inNumSamples)
{
	double in = ZIN0(0);
	double freq = ZIN0(1);

	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq) {
		double pfreq = freq * unit->mRate->mRadiansPerSample * 0.5f;

		double C = tan(pfreq);
		double C2 = C * C;
		double sqrt2C = C * sqrt2_f;
		a0 = 1. / (1. + sqrt2C + C2);
		b1 = 2. * (1. - C2) * a0 ;
		b2 = -(1. - sqrt2C + C2) * a0;

		double y0 = in + b1 * y1 + b2 * y2;
		ZOUT0(0) = a0 * (y0 - 2. * y1 + y2);
		y2 = y1;
		y1 = y0;

		unit->m_freq = freq;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {
		double y0 = in + b1 * y1 + b2 * y2;
		ZOUT0(0) = a0 * (y0 - 2. * y1 + y2);
		y2 = y1;
		y1 = y0;
	}

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BPF_Ctor(BPF* unit)
{
	//printf("BPF_Reset\n");
	if (unit->mBufLength == 1) {
		SETCALC(BPF_next_1);
	} else {
		SETCALC(BPF_next);
	};
	unit->m_a0 = 0.f;
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = uninitializedControl;
	unit->m_bw = uninitializedControl;

	BPF_next_1(unit, 1);
}

void BPF_next(BPF* unit, int inNumSamples)
{
	//printf("BPF_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float bw = ZIN0(2);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq || bw != unit->m_bw) {

		double pfreq = freq * unit->mRate->mRadiansPerSample;
		double pbw   = bw   * pfreq * 0.5f;

		double C = 1.f / tan(pbw);
		double D = 2.f * cos(pfreq);

		double next_a0 = 1.f / (1.f + C);
		double next_b1 = C * D * next_a0 ;
		double next_b2 = (1.f - C) * next_a0;

		double a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		double b1_slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);

			y2 = ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = a0 * (y2 - y1);

			y1 = ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = a0 * (y1 - y0);

			a0 += a0_slope;
			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);
			y2 = y1;
			y1 = y0;
		);

		unit->m_freq = freq;
		unit->m_bw = bw;
		unit->m_a0 = next_a0;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);

			y2 = ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = a0 * (y2 - y1);

			y1 = ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = a0 * (y1 - y0);
		);
		LOOP(unit->mRate->mFilterRemain,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);
			y2 = y1;
			y1 = y0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void BPF_next_1(BPF* unit, int inNumSamples)
{
	//printf("BPF_next_1\n");

	float in = ZIN0(0);
	float freq = ZIN0(1);
	float bw = ZIN0(2);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq || bw != unit->m_bw) {

		double pfreq = freq * unit->mRate->mRadiansPerSample;
		double pbw   = bw   * pfreq * 0.5;

		double C = 1.f / tan(pbw);
		double D = 2.f * cos(pfreq);

		double a0 = 1.f / (1.f + C);
		double b1 = C * D * a0 ;
		double b2 = (1.f - C) * a0;

		y0 = in + b1 * y1 + b2 * y2;
		ZOUT0(0) = a0 * (y0 - y2);
		y2 = y1;
		y1 = y0;

		unit->m_freq = freq;
		unit->m_bw = bw;
		unit->m_a0 = a0;
		unit->m_b1 = b1;
		unit->m_b2 = b2;
	} else {
		y0 = in + b1 * y1 + b2 * y2;
		ZOUT0(0) = a0 * (y0 - y2);
		y2 = y1;
		y1 = y0;
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void BRF_Ctor(BRF* unit)
{
	//printf("BRF_Reset\n");
	if (unit->mBufLength == 1) {
		SETCALC(BRF_next_1);
	} else {
		SETCALC(BRF_next);
	};
	unit->m_a0 = 0.f;
	unit->m_a1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = uninitializedControl;
	unit->m_bw = uninitializedControl;
	BRF_next_1(unit, 1);
}


void BRF_next(BRF* unit, int inNumSamples)
{
	//printf("BRF_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float bw = ZIN0(2);

	double ay;
	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double a1 = unit->m_a1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq || bw != unit->m_bw) {
		double pfreq = freq * unit->mRate->mRadiansPerSample;
		double pbw   = bw   * pfreq * 0.5f;

		double C = tan(pbw);
		double D = 2.f * cos(pfreq);

		double next_a0 = 1.f / (1.f + C);
		double next_a1 = -D * next_a0;
		double next_b2 = (1.f - C) * next_a0;

		double a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		double a1_slope = (next_a1 - a1) * unit->mRate->mFilterSlope;
		double b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;

		LOOP(unit->mRate->mFilterLoops,
			ay = a1 * y1;
			y0 = ZXP(in) - ay - b2 * y2;
			ZXP(out) = a0 * (y0 + y2) + ay;

			ay = a1 * y0;
			y2 = ZXP(in) - ay - b2 * y1;
			ZXP(out) = a0 * (y2 + y1) + ay;

			ay = a1 * y2;
			y1 = ZXP(in) - ay - b2 * y0;
			ZXP(out) = a0 * (y1 + y0) + ay;

			a0 += a0_slope;
			a1 += a1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			ay = a1 * y1;
			y0 = ZXP(in) - ay - b2 * y2;
			ZXP(out) = a0 * (y0 + y2) + ay;
			y2 = y1;
			y1 = y0;
		);

		unit->m_freq = freq;
		unit->m_bw = bw;
		unit->m_a0 = next_a0;
		unit->m_a1 = next_a1;
		unit->m_b2 = next_b2;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			ay = a1 * y1;
			y0 = ZXP(in) - ay - b2 * y2;
			ZXP(out) = a0 * (y0 + y2) + ay;

			ay = a1 * y0;
			y2 = ZXP(in) - ay - b2 * y1;
			ZXP(out) = a0 * (y2 + y1) + ay;

			ay = a1 * y2;
			y1 = ZXP(in) - ay - b2 * y0;
			ZXP(out) = a0 * (y1 + y0) + ay;
		);
		LOOP(unit->mRate->mFilterRemain,
			ay = a1 * y1;
			y0 = ZXP(in) - ay - b2 * y2;
			ZXP(out) = a0 * (y0 + y2) + ay;
			y2 = y1;
			y1 = y0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);

}


void BRF_next_1(BRF* unit, int inNumSamples)
{
	//printf("BRF_next_1\n");

	float in = ZIN0(0);
	float freq = ZIN0(1);
	float bw = ZIN0(2);

	double ay;
	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double a1 = unit->m_a1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq || bw != unit->m_bw) {
		double pfreq = freq * unit->mRate->mRadiansPerSample;
		double pbw   = bw   * pfreq * 0.5f;

		double C = tan(pbw);
		double D = 2.f * cos(pfreq);

		double a0 = 1.f / (1.f + C);
		double a1 = -D * a0;
		double b2 = (1.f - C) * a0;

		ay = a1 * y1;
		y0 = in - ay - b2 * y2;
		ZOUT0(0) = a0 * (y0 + y2) + ay;
		y2 = y1;
		y1 = y0;

		unit->m_freq = freq;
		unit->m_bw = bw;
		unit->m_a0 = a0;
		unit->m_a1 = a1;
		unit->m_b2 = b2;
	} else {

		ay = a1 * y1;
		y0 = in - ay - b2 * y2;
		ZOUT0(0) = a0 * (y0 + y2) + ay;
		y2 = y1;
		y1 = y0;

	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void MidEQ_Ctor(MidEQ* unit)
{
	//printf("MidEQ_Reset\n");
	SETCALC(MidEQ_next);
	unit->m_a0 = 0.f;
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = uninitializedControl;
	unit->m_bw = uninitializedControl;
	unit->m_db = uninitializedControl;
	PUSH_LOOPVALS
	MidEQ_next(unit, 1);
	POP_LOOPVALS
}


void MidEQ_next(MidEQ* unit, int inNumSamples)
{
	//printf("MidEQ_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float bw = ZIN0(2);
	float db = ZIN0(3);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;
	if (freq != unit->m_freq || bw != unit->m_bw || db != unit->m_db) {

		double amp = sc_dbamp(db) - 1.0f;
		double pfreq = freq * unit->mRate->mRadiansPerSample;
		double pbw   = bw   * pfreq * 0.5f;

		double C = 1.f / tan(pbw);
		double D = 2.f * cos(pfreq);

		double next_a0 = 1.f / (1.f + C);
		double next_b1 = C * D * next_a0 ;
		double next_b2 = (1.f - C) * next_a0;
		next_a0 *= amp;
		double a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		double b1_slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		double zin;
		LOOP(unit->mRate->mFilterLoops,
			zin = ZXP(in);
			y0 = zin + b1 * y1 + b2 * y2;
			ZXP(out) = zin + a0 * (y0 - y2);

			zin = ZXP(in);
			y2 = zin + b1 * y0 + b2 * y1;
			ZXP(out) = zin + a0 * (y2 - y1);

			zin = ZXP(in);
			y1 = zin + b1 * y2 + b2 * y0;
			ZXP(out) = zin + a0 * (y1 - y0);

			a0 += a0_slope;
			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			zin = ZXP(in);
			y0 = zin + b1 * y1 + b2 * y2;
			ZXP(out) = zin + a0 * (y0 - y2);
			y2 = y1;
			y1 = y0;
		);

		unit->m_freq = freq;
		unit->m_bw = bw;
		unit->m_db = db;
		unit->m_a0 = next_a0;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else {
		double zin;
		LOOP(unit->mRate->mFilterLoops,
			zin = ZXP(in);
			y0 = zin + b1 * y1 + b2 * y2;
			ZXP(out) = zin + a0 * (y0 - y2);

			zin = ZXP(in);
			y2 = zin + b1 * y0 + b2 * y1;
			ZXP(out) = zin + a0 * (y2 - y1);

			zin = ZXP(in);
			y1 = zin + b1 * y2 + b2 * y0;
			ZXP(out) = zin + a0 * (y1 - y0);
		);
		LOOP(unit->mRate->mFilterRemain,
			zin = ZXP(in);
			y0 = zin + b1 * y1 + b2 * y2;
			ZXP(out) = zin + a0 * (y0 - y2);
			y2 = y1;
			y1 = y0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

static void Median_InitMedian(Median* unit, long size, float value);
static float Median_InsertMedian(Median* unit, float value);

void Median_Ctor(Median* unit)
{
	//printf("Median_Reset\n");
	SETCALC(Median_next);
	float in = ZIN0(1);
	unit->m_medianSize = sc_clip((int)ZIN0(0), 0, kMAXMEDIANSIZE);
	Median_InitMedian(unit, unit->m_medianSize, in);
	ZOUT0(0) = Median_InsertMedian(unit, in);
}


float Median_InsertMedian(Median* unit, float value)
{
	long pos=-1;

	// keeps a sorted list of the previous n=size values
	// the oldest is removed and the newest is inserted.
	// values between the oldest and the newest are shifted over by one.

	// values and ages are both arrays that are 'size' long.
	// the median value is always values[size>>1]

	long last = unit->m_medianSize - 1;
	// find oldest bin and age the other bins.
	for (int i=0; i<unit->m_medianSize; ++i) {
		if (unit->m_medianAge[i] == last) { // is it the oldest bin ?
			pos = i;
		} else {
			unit->m_medianAge[i]++;	// age the bin
		}
	}
	// move values to fill in place of the oldest and make a space for the newest
	// search lower if value is too small for the open space
	while (pos != 0 && value < unit->m_medianValue[pos-1]) {
		unit->m_medianValue[pos] = unit->m_medianValue[pos-1];
		unit->m_medianAge[pos] = unit->m_medianAge[pos-1];
		pos--;
	}
	// search higher if value is too big for the open space
	while (pos != last && value > unit->m_medianValue[pos+1]) {
		unit->m_medianValue[pos] = unit->m_medianValue[pos+1];
		unit->m_medianAge[pos] = unit->m_medianAge[pos+1];
		pos++;
	}
	unit->m_medianValue[pos] = value;
	unit->m_medianAge[pos] = 0;		// this is the newest bin, age = 0
	return unit->m_medianValue[unit->m_medianSize>>1];
}

void Median_InitMedian(Median* unit, long size, float value)
{
	// initialize the arrays with the first value
	unit->m_medianSize = size;
	for (int i=0; i<size; ++i) {
		unit->m_medianValue[i] = value;
		unit->m_medianAge[i] = i;
	}
}

void Median_next(Median* unit, int inNumSamples)
{
	//printf("Median_next_a\n");

	float *out = ZOUT(0);
	float *in = ZIN(1);

	LOOP1(inNumSamples,
		ZXP(out) = Median_InsertMedian(unit, ZXP(in));
	);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void Resonz_Ctor(Resonz* unit)
{
	//printf("Resonz_Reset\n");
	SETCALC(Resonz_next);
	unit->m_a0 = 0.f;
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = uninitializedControl;
	unit->m_rq = 0.f;
	PUSH_LOOPVALS
	Resonz_next(unit, 1);
	POP_LOOPVALS
}


void Resonz_next(Resonz* unit, int inNumSamples)
{
	//printf("Resonz_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float rq = ZIN0(2);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq || rq != unit->m_rq) {
		double ffreq = freq * unit->mRate->mRadiansPerSample;
		double B = ffreq * rq;
		double R = 1.f - B * 0.5f;
		double twoR = 2.f * R;
		double R2 = R * R;
		double cost = (twoR * cos(ffreq)) / (1.f + R2);
		double b1_next = twoR * cost;
		double b2_next = -R2;
		double a0_next = (1.f - R2) * 0.5f;
		double a0_slope = (a0_next - a0) * unit->mRate->mFilterSlope;
		double b1_slope = (b1_next - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (b2_next - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);

			y2 = ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = a0 * (y2 - y1);

			y1 = ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = a0 * (y1 - y0);

			a0 += a0_slope;
			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);
			y2 = y1;
			y1 = y0;
		);

		unit->m_freq = freq;
		unit->m_rq = rq;
		unit->m_a0 = a0_next;
		unit->m_b1 = b1_next;
		unit->m_b2 = b2_next;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);

			y2 = ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = a0 * (y2 - y1);

			y1 = ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = a0 * (y1 - y0);
		);
		LOOP(unit->mRate->mFilterRemain,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);
			y2 = y1;
			y1 = y0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void Ringz_Ctor(Ringz* unit)
{
	//printf("Ringz_ctor\n");
	SETCALC(Ringz_next);
	unit->m_b1 = 0.f;
	unit->m_b2 = 0.f;
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_freq = uninitializedControl;
	unit->m_decayTime = 0.f;
	PUSH_LOOPVALS
	Ringz_next(unit, 1);
	POP_LOOPVALS
}


void Ringz_next(Ringz* unit, int inNumSamples)
{
	//printf("Ringz_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float decayTime = ZIN0(2);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = 0.5f;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;

	if (freq != unit->m_freq || decayTime != unit->m_decayTime) {
		double ffreq = freq * unit->mRate->mRadiansPerSample;
		double R = decayTime == 0.f ? 0.f : exp(log001/(decayTime * SAMPLERATE));
		double twoR = 2.f * R;
		double R2 = R * R;
		double cost = (twoR * cos(ffreq)) / (1.f + R2);
		double b1_next = twoR * cost;
		double b2_next = -R2;
		double b1_slope = (b1_next - b1) * unit->mRate->mFilterSlope;
		double b2_slope = (b2_next - b2) * unit->mRate->mFilterSlope;
		LOOP(unit->mRate->mFilterLoops,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);

			y2 = ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = a0 * (y2 - y1);

			y1 = ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = a0 * (y1 - y0);

			b1 += b1_slope;
			b2 += b2_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);
			y2 = y1;
			y1 = y0;
		);

		unit->m_freq = freq;
		unit->m_decayTime = decayTime;
		unit->m_b1 = b1_next;
		unit->m_b2 = b2_next;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);

			y2 = ZXP(in) + b1 * y0 + b2 * y1;
			ZXP(out) = a0 * (y2 - y1);

			y1 = ZXP(in) + b1 * y2 + b2 * y0;
			ZXP(out) = a0 * (y1 - y0);
		);
		LOOP(unit->mRate->mFilterRemain,
			y0 = ZXP(in) + b1 * y1 + b2 * y2;
			ZXP(out) = a0 * (y0 - y2);
			y2 = y1;
			y1 = y0;
		);
	}
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);

}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Formlet_Ctor(Formlet* unit)
{
	//printf("Formlet_Reset\n");
	if (unit->mBufLength == 1) {
		SETCALC(Formlet_next_1);
	} else {
		SETCALC(Formlet_next);
	};
	unit->m_b01 = 0.f;
	unit->m_b02 = 0.f;
	unit->m_y01 = 0.f;
	unit->m_y02 = 0.f;
	unit->m_b11 = 0.f;
	unit->m_b12 = 0.f;
	unit->m_y11 = 0.f;
	unit->m_y12 = 0.f;
	unit->m_freq = uninitializedControl;
	unit->m_attackTime = uninitializedControl;
	unit->m_decayTime = uninitializedControl;
	Formlet_next_1(unit, 1);
}

void Formlet_next(Formlet* unit, int inNumSamples)
{
	//printf("Formlet_next\n");

	float *out = ZOUT(0);
	float *in = ZIN(0);
	float freq = ZIN0(1);
	float attackTime = ZIN0(2);
	float decayTime = ZIN0(3);

	double y00;
	double y10;
	double y01 = unit->m_y01;
	double y11 = unit->m_y11;
	double y02 = unit->m_y02;
	double y12 = unit->m_y12;

	double b01 = unit->m_b01;
	double b11 = unit->m_b11;
	double b02 = unit->m_b02;
	double b12 = unit->m_b12;
	double ain;

	if (freq != unit->m_freq || decayTime != unit->m_decayTime || attackTime != unit->m_attackTime) {
		double ffreq = freq * unit->mRate->mRadiansPerSample;

		double R = decayTime == 0.f ? 0.f : exp(log001/(decayTime * SAMPLERATE));
		double twoR = 2.f * R;
		double R2 = R * R;
		double cost = (twoR * cos(ffreq)) / (1.f + R2);
		double b01_next = twoR * cost;
		double b02_next = -R2;
		double b01_slope = (b01_next - b01) * unit->mRate->mFilterSlope;
		double b02_slope = (b02_next - b02) * unit->mRate->mFilterSlope;

		R = attackTime == 0.f ? 0.f : exp(log001/(attackTime * SAMPLERATE));
		twoR = 2.f * R;
		R2 = R * R;
		cost = (twoR * cos(ffreq)) / (1.f + R2);
		double b11_next = twoR * cost;
		double b12_next = -R2;
		double b11_slope = (b11_next - b11) * unit->mRate->mFilterSlope;
		double b12_slope = (b12_next - b12) * unit->mRate->mFilterSlope;

		LOOP(unit->mRate->mFilterLoops,
			ain = ZXP(in);
			y00 = ain + b01 * y01 + b02 * y02;
			y10 = ain + b11 * y11 + b12 * y12;
			ZXP(out) = 0.25 * ((y00 - y02) - (y10 - y12));

			ain = ZXP(in);
			y02 = ain + b01 * y00 + b02 * y01;
			y12 = ain + b11 * y10 + b12 * y11;
			ZXP(out) = 0.25 * ((y02 - y01) - (y12 - y11));

			ain = ZXP(in);
			y01 = ain + b01 * y02 + b02 * y00;
			y11 = ain + b11 * y12 + b12 * y10;
			ZXP(out) = 0.25 * ((y01 - y00) - (y11 - y10));

			b01 += b01_slope;
			b02 += b02_slope;
			b11 += b11_slope;
			b12 += b12_slope;
		);
		LOOP(unit->mRate->mFilterRemain,
			ain = ZXP(in);
			y00 = ain + b01 * y01 + b02 * y02;
			y10 = ain + b11 * y11 + b12 * y12;
			ZXP(out) = 0.25 * ((y00 - y02) - (y10 - y12));
			y02 = y01;
			y01 = y00;
			y12 = y11;
			y11 = y10;
		);

		unit->m_freq = freq;
		unit->m_attackTime = attackTime;
		unit->m_decayTime = decayTime;
		unit->m_b01 = b01_next;
		unit->m_b02 = b02_next;
		unit->m_b11 = b11_next;
		unit->m_b12 = b12_next;
	} else {
		LOOP(unit->mRate->mFilterLoops,
			ain = ZXP(in);
			y00 = ain + b01 * y01 + b02 * y02;
			y10 = ain + b11 * y11 + b12 * y12;
			ZXP(out) = 0.25 * ((y00 - y02) - (y10 - y12));

			ain = ZXP(in);
			y02 = ain + b01 * y00 + b02 * y01;
			y12 = ain + b11 * y10 + b12 * y11;
			ZXP(out) = 0.25 * ((y02 - y01) - (y12 - y11));

			ain = ZXP(in);
			y01 = ain + b01 * y02 + b02 * y00;
			y11 = ain + b11 * y12 + b12 * y10;
			ZXP(out) = 0.25 * ((y01 - y00) - (y11 - y10));
		);
		LOOP(unit->mRate->mFilterRemain,
			ain = ZXP(in);
			y00 = ain + b01 * y01 + b02 * y02;
			y10 = ain + b11 * y11 + b12 * y12;
			ZXP(out) = 0.25 * ((y00 - y02) - (y10 - y12));
			y02 = y01;
			y01 = y00;
			y12 = y11;
			y11 = y10;
		);
	}
	unit->m_y01 = y01;
	unit->m_y02 = y02;
	unit->m_y11 = y11;
	unit->m_y12 = y12;
}

void Formlet_next_1(Formlet* unit, int inNumSamples)
{
	//printf("Formlet_next\n");

	float in = ZIN0(0);
	float freq = ZIN0(1);
	float attackTime = ZIN0(2);
	float decayTime = ZIN0(3);

	double y00;
	double y10;
	double y01 = unit->m_y01;
	double y11 = unit->m_y11;
	double y02 = unit->m_y02;
	double y12 = unit->m_y12;

	double b01 = unit->m_b01;
	double b11 = unit->m_b11;
	double b02 = unit->m_b02;
	double b12 = unit->m_b12;
	double ain;

	if (freq != unit->m_freq || decayTime != unit->m_decayTime || attackTime != unit->m_attackTime) {
		double ffreq = freq * unit->mRate->mRadiansPerSample;

		double R = decayTime == 0.f ? 0.f : exp(log001/(decayTime * SAMPLERATE));
		double twoR = 2.f * R;
		double R2 = R * R;
		double cost = (twoR * cos(ffreq)) / (1.f + R2);
		b01 = twoR * cost;
		b02 = -R2;

		R = attackTime == 0.f ? 0.f : exp(log001/(attackTime * SAMPLERATE));
		twoR = 2.f * R;
		R2 = R * R;
		cost = (twoR * cos(ffreq)) / (1.f + R2);
		b11 = twoR * cost;
		b12 = -R2;

			ain = in;
			y00 = ain + b01 * y01 + b02 * y02;
			y10 = ain + b11 * y11 + b12 * y12;
			ZOUT0(0) = 0.25f * ((y00 - y02) - (y10 - y12));

			y02 = y01;
			y01 = y00;
			y12 = y11;
			y11 = y10;

		unit->m_freq = freq;
		unit->m_attackTime = attackTime;
		unit->m_decayTime = decayTime;
		unit->m_b01 = b01;
		unit->m_b02 = b02;
		unit->m_b11 = b11;
		unit->m_b12 = b12;
	} else {
			ain = in;
			y00 = ain + b01 * y01 + b02 * y02;
			y10 = ain + b11 * y11 + b12 * y12;
			ZOUT0(0) = 0.25f * ((y00 - y02) - (y10 - y12));

			y02 = y01;
			y01 = y00;
			y12 = y11;
			y11 = y10;
	}
	unit->m_y01 = y01;
	unit->m_y02 = y02;
	unit->m_y11 = y11;
	unit->m_y12 = y12;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void FOS_Ctor(FOS* unit)
{
	//printf("FOS_Reset\n");
	if (unit->mBufLength == 1) {
		SETCALC(FOS_next_1);
	} else {
		if (INRATE(1) == calc_FullRate
				&& INRATE(2) == calc_FullRate
				&& INRATE(3) == calc_FullRate) {
			SETCALC(FOS_next_a);
		} else {
			SETCALC(FOS_next_k);
		}
	};
	unit->m_y1 = 0.f;
	unit->m_a0 = ZIN0(1);
	unit->m_a1 = ZIN0(2);
	unit->m_b1 = ZIN0(3);
	FOS_next_1(unit, 1);
}

void FOS_next_a(FOS* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *a0 = ZIN(1);
	float *a1 = ZIN(2);
	float *b1 = ZIN(3);

	double y1 = unit->m_y1;
	LOOP1(inNumSamples,
		double y0 = ZXP(in) + ZXP(b1) * y1;
		ZXP(out) = ZXP(a0) * y0 + ZXP(a1) * y1;
		y1 = y0;
	);
	unit->m_y1 = zapgremlins(y1);
}

void FOS_next_1(FOS* unit, int inNumSamples)
{
	float in = ZIN0(0);
	float a0 = ZIN0(1);
	float a1 = ZIN0(2);
	float b1 = ZIN0(3);

	double y1 = unit->m_y1;

	double y0 = in + b1 * y1;
	ZOUT0(0) = a0 * y0 + a1 * y1;
	y1 = y0;

	unit->m_y1 = zapgremlins(y1);
}


void FOS_next_k(FOS* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_a0 = ZIN0(1);
	float next_a1 = ZIN0(2);
	float next_b1 = ZIN0(3);

	double y1 = unit->m_y1;
	double a0 = unit->m_a0;
	double a1 = unit->m_a1;
	double b1 = unit->m_b1;
	double a0_slope = CALCSLOPE(next_a0, a0);
	double a1_slope = CALCSLOPE(next_a1, a1);
	double b1_slope = CALCSLOPE(next_b1, b1);
	LOOP1(inNumSamples,
		double y0 = ZXP(in) + b1 * y1;
		ZXP(out) = a0 * y0 + a1 * y1;
		y1 = y0;

		a0 += a0_slope;
		a1 += a1_slope;
		b1 += b1_slope;
	);
	unit->m_y1 = zapgremlins(y1);
	unit->m_a0 = next_a0;
	unit->m_a1 = next_a1;
	unit->m_b1 = next_b1;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void SOS_Ctor(SOS* unit)
{
	// printf("SOS_Reset\n");
	if (unit->mBufLength != 1) {
		if (INRATE(1) == calc_FullRate
			&& INRATE(2) == calc_FullRate
			&& INRATE(3) == calc_FullRate
			&& INRATE(4) == calc_FullRate
			&& INRATE(5) == calc_FullRate) {
			SETCALC(SOS_next_a);
		} else if (INRATE(1) == calc_ScalarRate
				   && INRATE(2) == calc_ScalarRate
				   && INRATE(3) == calc_ScalarRate
				   && INRATE(4) == calc_ScalarRate
				   && INRATE(5) == calc_ScalarRate) {
			SETCALC(SOS_next_i);
		} else {
			SETCALC(SOS_next_k);
		}
	} else {
		SETCALC(SOS_next_1);
	//	printf("SOS_next_1\n");
	}
	unit->m_y1 = 0.f;
	unit->m_y2 = 0.f;
	unit->m_a0 = ZIN0(1);
	unit->m_a1 = ZIN0(2);
	unit->m_a2 = ZIN0(3);
	unit->m_b1 = ZIN0(4);
	unit->m_b2 = ZIN0(5);
	SOS_next_1(unit, 1);
}

void SOS_next_a(SOS *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *a0 = ZIN(1);
	float *a1 = ZIN(2);
	float *a2 = ZIN(3);
	float *b1 = ZIN(4);
	float *b2 = ZIN(5);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	LOOP(unit->mRate->mFilterLoops,
		y0 = ZXP(in) + ZXP(b1) * y1 + ZXP(b2) * y2;
		ZXP(out) = ZXP(a0) * y0 + ZXP(a1) * y1 + ZXP(a2) * y2;

		y2 = ZXP(in) + ZXP(b1) * y0 + ZXP(b2) * y1;
		ZXP(out) = ZXP(a0) * y2 + ZXP(a1) * y0 + ZXP(a2) * y1;

		y1 = ZXP(in) + ZXP(b1) * y2 + ZXP(b2) * y0;
		ZXP(out) = ZXP(a0) * y1 + ZXP(a1) * y2 + ZXP(a2) * y0;
	);
	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + ZXP(b1) * y1 + ZXP(b2) * y2;
		ZXP(out) = ZXP(a0) * y0 + ZXP(a1) * y1 + ZXP(a2) * y2;
		y2 = y1;
		y1 = y0;
	);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void SOS_next_k(SOS *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float next_a0 = ZIN0(1);
	float next_a1 = ZIN0(2);
	float next_a2 = ZIN0(3);
	float next_b1 = ZIN0(4);
	float next_b2 = ZIN0(5);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double a1 = unit->m_a1;
	double a2 = unit->m_a2;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;
	double a0_slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
	double a1_slope = (next_a1 - a1) * unit->mRate->mFilterSlope;
	double a2_slope = (next_a2 - a2) * unit->mRate->mFilterSlope;
	double b1_slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
	double b2_slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
	LOOP(unit->mRate->mFilterLoops,
		float in0 = ZXP(in);
		float in1 = ZXP(in);
		float in2 = ZXP(in);
		y0 = in0 + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = in1 + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = in2 + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;

		a0 += a0_slope;
		a1 += a1_slope;
		a2 += a2_slope;
		b1 += b1_slope;
		b2 += b2_slope;
	);
	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
	);

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void SOS_next_i(SOS *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;
	double a0 = unit->m_a0;
	double a1 = unit->m_a1;
	double a2 = unit->m_a2;
	double b1 = unit->m_b1;
	double b2 = unit->m_b2;
	LOOP(unit->mRate->mFilterLoops,
		float in0 = ZXP(in);
		float in1 = ZXP(in);
		float in2 = ZXP(in);
		y0 = in0 + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = in1 + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = in2 + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	);
	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
	);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void SOS_next_1(SOS *unit, int inNumSamples)	// optimized for SOS.kr
{
	float in = ZIN0(0);
	float a0 = ZIN0(1);
	float a1 = ZIN0(2);
	float a2 = ZIN0(3);
	float b1 = ZIN0(4);
	float b2 = ZIN0(5);

	double y0;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;

	y0 = in + b1 * y1 + b2 * y2;
	ZOUT0(0) = a0 * y0 + a1 * y1 + a2 * y2;
	y2 = y1;
	y1 = y0;

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Compander_Ctor(Compander* unit)
{
	//printf("Compander_Reset\n");
	SETCALC(Compander_next);
	unit->m_clamp = 0.f;
	unit->m_clampcoef = 0.f;
	unit->m_relax = 0.f;
	unit->m_relaxcoef = 0.f;
	unit->m_prevmaxval = 0.f;
	unit->m_gain = 0.f;
	Compander_next(unit, 1);
}

void Compander_next(Compander* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *control = ZIN(1);
	float thresh = ZIN0(2);
	float slope_below = ZIN0(3);
	float slope_above = ZIN0(4);
	float clamp = ZIN0(5);
	float relax = ZIN0(6);

	if (clamp != unit->m_clamp) {
		unit->m_clampcoef  = clamp == 0.0 ? 0.0 : exp(log1/(clamp * SAMPLERATE));
		unit->m_clamp = clamp;
	}
	if (relax != unit->m_relax) {
		unit->m_relaxcoef = relax == 0.0 ? 0.0 : exp(log1/(relax *  SAMPLERATE));
		unit->m_relax = relax;
	}

	float gain = unit->m_gain;

	float relaxcoef = unit->m_relaxcoef;
	float clampcoef = unit->m_clampcoef;

	float prevmaxval = unit->m_prevmaxval;

	float val;
	LOOP1(inNumSamples,
		val = std::abs(ZXP(control));
		if (val < prevmaxval) {
			val = val + (prevmaxval - val) * relaxcoef;
		} else {
			val = val + (prevmaxval - val) * clampcoef;
		}
		prevmaxval = val;
	);

	unit->m_prevmaxval = prevmaxval;

	float next_gain;//,absx;
	if (prevmaxval < thresh) {
		if (slope_below == 1.f) {
			next_gain = 1.f;
		} else {
			next_gain = pow(prevmaxval / thresh, slope_below - 1.f);
			//blows up here
			float32 absx = std::abs(next_gain);
			//zap gremlins, but returns 0. if gain is too small and 1. if gain is too big
			next_gain =
				(absx < (float32)1e-15) ? (float32)0. :
				(absx > (float32)1e15) ? (float32)1. : next_gain;
		}
	} else {
		if (slope_above == 1.f) {
			next_gain = 1.f;
		} else {
			next_gain = pow(prevmaxval / thresh, slope_above - 1.f);
		}
	}

	float gain_slope = CALCSLOPE(next_gain, gain);
	LOOP1(inNumSamples, ZXP(out) = ZXP(in) * gain; gain += gain_slope;);
	unit->m_gain = gain;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void Normalizer_Dtor(Normalizer* unit)
{
	RTFree(unit->mWorld, unit->m_table);
}

void Normalizer_Ctor(Normalizer* unit)
{
	SETCALC(Normalizer_next);
	//printf("Normalizer_Reset\n");

	float dur = ZIN0(2);
	unit->m_bufsize = (long)(ceil(dur * SAMPLERATE));
	long allocsize = unit->m_bufsize * 3;
	//allocsize = NEXTPOWEROFTWO(allocsize);

	unit->m_table = (float*)RTAlloc(unit->mWorld, allocsize * sizeof(float));

	unit->m_pos = 0;
	unit->m_flips = 0;
	unit->m_level = 1.f;
	unit->m_slope = 0.f;
	unit->m_prevmaxval = 0.0;
	unit->m_curmaxval = 0.0;
	unit->m_slopefactor = 1.f / unit->m_bufsize;

	unit->m_xinbuf = unit->m_table - ZOFF;
	unit->m_xmidbuf = unit->m_xinbuf + unit->m_bufsize;
	unit->m_xoutbuf = unit->m_xmidbuf + unit->m_bufsize;
	Normalizer_next(unit, 1);
}


void Normalizer_next(Normalizer* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float amp = ZIN0(1);

	long pos = unit->m_pos;
	float slope = unit->m_slope;
	float level = unit->m_level;
	float curmaxval = unit->m_curmaxval;
	float val;

	long bufsize = unit->m_bufsize;
	long buf_remain = bufsize - pos;

	long remain = inNumSamples;
	while (remain > 0) {
		long nsmps = sc_min(remain, buf_remain);
		float* xinbuf = unit->m_xinbuf + pos;
		float* xoutbuf = unit->m_xoutbuf + pos;
		if (unit->m_flips >= 2) {
			LOOP(nsmps,
				ZXP(xinbuf) = val = ZXP(in);
				ZXP(out) = level * ZXP(xoutbuf);
				level += slope;
				val = std::abs(val);
				if (val > curmaxval) curmaxval = val;
			);
		} else {
			LOOP(nsmps,
				ZXP(xinbuf) = val = ZXP(in);
				ZXP(out) = 0.f;
				level += slope;
				val = std::abs(val);
				if (val > curmaxval) curmaxval = val;
			);
		}
		pos += nsmps;
		if (pos >= bufsize) {
			pos = 0;
			buf_remain = bufsize;

			float maxval2 = sc_max(unit->m_prevmaxval, curmaxval);
			unit->m_prevmaxval = curmaxval;
			unit->m_curmaxval = curmaxval = 0.f;

			float next_level;
			if (maxval2 <= 0.00001f) next_level = 100000.f * amp;
			else next_level = amp / maxval2;

			slope = unit->m_slope = (next_level - level) * unit->m_slopefactor;

			float* temp = unit->m_xoutbuf;
			unit->m_xoutbuf = unit->m_xmidbuf;
			unit->m_xmidbuf = unit->m_xinbuf;
			unit->m_xinbuf = temp;

			unit->m_flips++;
		}
		remain -= nsmps;
	}

	unit->m_pos = pos;
	unit->m_level = level;
	unit->m_curmaxval = curmaxval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Limiter_Dtor(Limiter* unit)
{
	RTFree(unit->mWorld, unit->m_table);
}

void Limiter_Ctor(Limiter* unit)
{
	//printf("Limiter_Reset\n");
	SETCALC(Limiter_next);

	float dur = ZIN0(2);
	unit->m_bufsize = (long)(ceil(dur * SAMPLERATE));
	long allocsize = unit->m_bufsize * 3;
	allocsize = NEXTPOWEROFTWO(allocsize);

	unit->m_table = (float*)RTAlloc(unit->mWorld, allocsize * sizeof(float));

	unit->m_flips = 0;
	unit->m_pos = 0;
	unit->m_slope = 0.f;
	unit->m_level = 1.f;
	unit->m_prevmaxval = 0.0;
	unit->m_curmaxval = 0.0;
	unit->m_slopefactor = 1.f / unit->m_bufsize;

	unit->m_xinbuf = unit->m_table - ZOFF;
	unit->m_xmidbuf = unit->m_xinbuf + unit->m_bufsize;
	unit->m_xoutbuf = unit->m_xmidbuf + unit->m_bufsize;
	Limiter_next(unit, 1);
}


void Limiter_next(Limiter* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float amp = ZIN0(1);

	long pos = unit->m_pos;
	float slope = unit->m_slope;
	float level = unit->m_level;
	float curmaxval = unit->m_curmaxval;
	float val;

	long bufsize = unit->m_bufsize;
	long buf_remain = bufsize - pos;

	long remain = inNumSamples;
	while (remain > 0) {
		long nsmps = sc_min(remain, buf_remain);
		float* xinbuf = unit->m_xinbuf + pos;
		float* xoutbuf = unit->m_xoutbuf + pos;
		if (unit->m_flips >= 2) {
			LOOP(nsmps,
				ZXP(xinbuf) = val = ZXP(in);
				ZXP(out) = level * ZXP(xoutbuf);
				level += slope;
				val = std::abs(val);
				if (val > curmaxval) curmaxval = val;
			);
		} else {
			LOOP(nsmps,
				ZXP(xinbuf) = val = ZXP(in);
				ZXP(out) = 0.f;
				level += slope;
				val = std::abs(val);
				if (val > curmaxval) curmaxval = val;
			);
		}
		pos += nsmps;
		if (pos >= bufsize) {
			pos = 0;
			buf_remain = bufsize;

			float maxval2 = sc_max(unit->m_prevmaxval, curmaxval);
			unit->m_prevmaxval = curmaxval;
			unit->m_curmaxval = curmaxval = 0.f;

			float next_level;
			if (maxval2 > amp) next_level = amp / maxval2;
			else next_level = 1.0;

			slope = unit->m_slope = (next_level - level) * unit->m_slopefactor;

			float* temp = unit->m_xoutbuf;
			unit->m_xoutbuf = unit->m_xmidbuf;
			unit->m_xmidbuf = unit->m_xinbuf;
			unit->m_xinbuf = temp;

			unit->m_flips++;
		}
		remain -= nsmps;
	}

	unit->m_pos = pos;
	unit->m_level = level;
	unit->m_curmaxval = curmaxval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Amplitude_Ctor(Amplitude* unit)
{
	if(INRATE(1) != calc_ScalarRate || INRATE(2) != calc_ScalarRate) {
		if(INRATE(0) == calc_FullRate && unit->mCalcRate == calc_BufRate){
			SETCALC(Amplitude_next_atok_kk);
		} else {
			SETCALC(Amplitude_next_kk);
		}

	} else {
		if(INRATE(0) == calc_FullRate && unit->mCalcRate == calc_BufRate){
			SETCALC(Amplitude_next_atok);
		} else {
			SETCALC(Amplitude_next);
		}
	}

	float clamp = ZIN0(1);
	unit->m_clampcoef  = clamp == 0.0 ? 0.0 : exp(log1/(clamp * SAMPLERATE));

	float relax = ZIN0(2);
	unit->m_relaxcoef = relax == 0.0 ? 0.0 : exp(log1/(relax * SAMPLERATE));

	unit->m_previn = std::abs(ZIN0(0));
	Amplitude_next(unit, 1);
}

void Amplitude_next(Amplitude* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);

	float relaxcoef = unit->m_relaxcoef;
	float clampcoef = unit->m_clampcoef;
	float previn = unit->m_previn;

	float val;
	LOOP1(inNumSamples,
		val = std::abs(ZXP(in));
		if (val < previn) {
			val = val + (previn - val) * relaxcoef;
		} else {
			val = val + (previn - val) * clampcoef;
		}
		ZXP(out) = previn = val;
	);

	unit->m_previn = previn;
}

void Amplitude_next_atok(Amplitude* unit, int inNumSamples)
{
	float *in = ZIN(0);

	float relaxcoef = unit->m_relaxcoef;
	float clampcoef = unit->m_clampcoef;
	float previn = unit->m_previn;

	float val;
	LOOP1(FULLBUFLENGTH,
		val = std::abs(ZXP(in));
		if (val < previn) {
			val = val + (previn - val) * relaxcoef;
		} else {
			val = val + (previn - val) * clampcoef;
		}
		previn = val;
	);
	ZOUT0(0) = val;

	unit->m_previn = previn;
}

void Amplitude_next_kk(Amplitude* unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float relaxcoef, clampcoef;

	if(ZIN0(1) != unit->m_clamp_in) {
		clampcoef = unit->m_clampcoef = exp(log1/(ZIN0(1) * SAMPLERATE));
		unit->m_clamp_in = ZIN0(1);
	} else {
		clampcoef = unit->m_clampcoef;
	}

	if(ZIN0(2) != unit->m_relax_in) {
		relaxcoef = unit->m_relaxcoef = exp(log1/(ZIN0(2) * SAMPLERATE));
		unit->m_relax_in = ZIN0(2);
	} else {
		relaxcoef = unit->m_relaxcoef;
	}

	float previn = unit->m_previn;

	float val;
	LOOP1(inNumSamples,
		val = std::abs(ZXP(in));
		if (val < previn) {
			val = val + (previn - val) * relaxcoef;
		} else {
			val = val + (previn - val) * clampcoef;
		}
		ZXP(out) = previn = val;
	);

	unit->m_previn = previn;
}

void Amplitude_next_atok_kk(Amplitude* unit, int inNumSamples)
{
	float *in = ZIN(0);
	float relaxcoef, clampcoef;

	if(ZIN0(1) != unit->m_clamp_in) {
		clampcoef = unit->m_clampcoef = exp(log1/(ZIN0(1) * SAMPLERATE));
		unit->m_clamp_in = ZIN0(1);
	} else {
		clampcoef = unit->m_clampcoef;
	}

	if(ZIN0(2) != unit->m_relax_in) {
		relaxcoef = unit->m_relaxcoef = exp(log1/(ZIN0(2) * SAMPLERATE));
		unit->m_relax_in = ZIN0(2);
	} else {
		relaxcoef = unit->m_relaxcoef;
	}

	float previn = unit->m_previn;

	float val;
	LOOP1(FULLBUFLENGTH,
		val = std::abs(ZXP(in));
		if (val < previn) {
			val = val + (previn - val) * relaxcoef;
		} else {
			val = val + (previn - val) * clampcoef;
		}
		previn = val;
	);
	ZOUT0(0) = val;

	unit->m_previn = previn;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void DetectSilence_Ctor(DetectSilence* unit)
{
	//printf("DetectSilence_Reset\n");
	if(INRATE(1) == calc_ScalarRate && INRATE(2) == calc_ScalarRate) {
		unit->mThresh = ZIN0(1);
		unit->mEndCounter = (int32)(SAMPLERATE * ZIN0(2));
		SETCALC(DetectSilence_next);
	} else {
		SETCALC(DetectSilence_next_k);
	}
	unit->mCounter = -1;
	ClearUnitOutputs(unit, 1);
}


void DetectSilence_next(DetectSilence* unit, int inNumSamples)
{
	float thresh = unit->mThresh;
	int counter = unit->mCounter;
	float val;

	float *in = IN(0);
	float *out = OUT(0);
	for (int i=0; i<inNumSamples; ++i) {
		val = std::abs(*in++);
		if (val > thresh) {
			counter = 0;
			*out++ = 0.f;
		} else if (counter >= 0) {
			if (++counter >= unit->mEndCounter) {
				DoneAction((int)ZIN0(3), unit);
				*out++ = 1.f;
			} else {
				*out++ = 0.f;
			}
		}
		else
			*out++ = 0.f;
	}
	unit->mCounter = counter;
}

void DetectSilence_next_k(DetectSilence* unit, int inNumSamples)
{
	float thresh = ZIN0(1);
	int endCounter = (int32)(SAMPLERATE * ZIN0(2));
	int counter = unit->mCounter;
	float val;
	float *in = IN(0);
	float *out = OUT(0);

	for (int i=0; i<inNumSamples; ++i) {
		val = std::abs(*in++);
		if (val > thresh) {
			counter = 0;
			*out++ = 0.f;
		} else if (counter >= 0) {
			if (++counter >= endCounter) {
				DoneAction((int)ZIN0(3), unit);
				*out++ = 1.f;
			} else {
				*out++ = 0.f;
			}
		}
		else
			*out++ = 0.f;
	}
	unit->mCounter = counter;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
// Based on HilbertIIR from SC2
// a 12 pole (6 per side) Hilbert IIR filter
// based on Sean Costello and Bernie Hutchins
// created by jl anderson - 7 jan 2001
// UGen created by Josh Parmenter

#define HILBERT_FILTER \
    y0_1 = thisin - (coefs[0]) * y1[0]; \
    ay1 = coefs[0] * y0_1 + 1 * y1[0]; \
    y1[0] = y0_1; \
    y0_2 = ay1 - (coefs[1]) * y1[1]; \
    ay2 = coefs[1] * y0_2 + 1 * y1[1]; \
    y1[1] = y0_2; \
    y0_3 = ay2 - (coefs[2]) * y1[2]; \
    ay3 = coefs[2] * y0_3 + 1 * y1[2]; \
    y1[2] = y0_3; \
    y0_4 = ay3 - (coefs[3]) * y1[3]; \
    ay4 =coefs[3] *  y0_4 + 1 * y1[3]; \
    y1[3] = y0_4; \
    y0_5 = ay4 - (coefs[4]) * y1[4]; \
    ay5 = coefs[4] * y0_5 + 1 * y1[4]; \
    y1[4] = y0_5; \
    y0_6 = ay5 - (coefs[5]) * y1[5]; \
    ay6 = coefs[5] * y0_6 + 1 * y1[5]; \
    y1[5] = y0_6; \
    \
    y0_7 = thisin - (coefs[6]) * y1[6]; \
    ay7 = coefs[6] * y0_7 + 1 * y1[6]; \
    y1[6] = y0_7; \
    y0_8 = ay7 - (coefs[7]) * y1[7]; \
    ay8 = coefs[7] * y0_8 + 1 * y1[7]; \
    y1[7] = y0_8; \
    y0_9 = ay8 - (coefs[8]) * y1[8]; \
    ay9 = coefs[8] * y0_9 + 1 * y1[8]; \
    y1[8] = y0_9; \
    y0_10 = ay9 - (coefs[9]) * y1[9]; \
    ay10 = coefs[9] * y0_10 + 1 * y1[9]; \
    y1[9] = y0_10; \
    y0_11 = ay10 - (coefs[10]) * y1[10]; \
    ay11 = coefs[10] * y0_11  + 1 * y1[10]; \
    y1[10] = y0_11; \
    y0_12 = ay11 - (coefs[11]) * y1[11]; \
    ay12 = coefs[11] * y0_12 + 1 * y1[11]; \
    y1[11] = y0_12; \


void Hilbert_Ctor(Hilbert *unit)
{
	// calculate coefs based on SampleRate, store in the struct
	SETCALC(Hilbert_next);

	double gamconst = (15.0f * pi_f) / SAMPLERATE;
	double gamma01 = gamconst * 0.3609f;
	double gamma02 = gamconst * 2.7412f;
	double gamma03 = gamconst * 11.1573f;
	double gamma04 = gamconst * 44.7581f;
	double gamma05 = gamconst * 179.6242f;
	double gamma06 = gamconst * 798.4578f;
	double gamma07 = gamconst * 1.2524f;
	double gamma08 = gamconst * 5.5671f;
	double gamma09 = gamconst * 22.3423f;
	double gamma10 = gamconst * 89.6271f;
	double gamma11 = gamconst * 364.7914f;
	double gamma12 = gamconst * 2770.1114f;
	unit->m_coefs[0] = (gamma01 - 1.f) / (gamma01 + 1.f);
	unit->m_coefs[1] = (gamma02 - 1.f) / (gamma02 + 1.f);
	unit->m_coefs[2] = (gamma03 - 1.f) / (gamma03 + 1.f);
	unit->m_coefs[3] = (gamma04 - 1.f) / (gamma04 + 1.f);
	unit->m_coefs[4] = (gamma05 - 1.f) / (gamma05 + 1.f);
	unit->m_coefs[5] = (gamma06 - 1.f) / (gamma06 + 1.f);
	unit->m_coefs[6] = (gamma07 - 1.f) / (gamma07 + 1.f);
	unit->m_coefs[7] = (gamma08 - 1.f) / (gamma08 + 1.f);
	unit->m_coefs[8] = (gamma09 - 1.f) / (gamma09 + 1.f);
	unit->m_coefs[9] = (gamma10 - 1.f) / (gamma10 + 1.f);
	unit->m_coefs[10] = (gamma11 - 1.f) / (gamma11 + 1.f);
	unit->m_coefs[11] = (gamma12 - 1.f) / (gamma12 + 1.f);
	Clear(12, unit->m_y1);
	Hilbert_next(unit, 1);
}

void Hilbert_next(Hilbert *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *outcos = ZOUT(0);
	float *outsin = ZOUT(1);
	float y1[12];
	float coefs[12];

	// each filter's last sample
	for(int i = 0; i < 12; ++i){
		y1[i] = unit->m_y1[i];
		coefs[i] = unit->m_coefs[i];
	}

	double ay1, ay2, ay3, ay4, ay5, ay6;
	double ay7, ay8, ay9, ay10, ay11, ay12;

	double y0_1, y0_2, y0_3, y0_4, y0_5, y0_6;
	double y0_7, y0_8, y0_9, y0_10, y0_11, y0_12;

	LOOP1(inNumSamples,
		float thisin = ZXP(in);

		HILBERT_FILTER
		ZXP(outcos) = ay6;
		ZXP(outsin) = ay12;
	)

	for(int i = 0; i < 12; ++i)
		unit->m_y1[i] = zapgremlins(y1[i]);
}


/* Hilbert based SSB FreqShifter */

void FreqShift_Ctor(FreqShift *unit)
{
	// calculate coefs based on SampleRate, store in the struct
//    SETCALC(FreqShift_next_kk);
	unit->m_phase = 0;
	if(INRATE(1) == calc_FullRate) {
		if(INRATE(2) == calc_FullRate)
			SETCALC(FreqShift_next_aa);
		else
			SETCALC(FreqShift_next_ak);
	} else {
		if(INRATE(2) == calc_FullRate)
			SETCALC(FreqShift_next_ka);
		else {
			SETCALC(FreqShift_next_kk);
			unit->m_phase = (int32)(unit->m_radtoinc * IN0(2));
		}
	}
	// set up for the oscil for the modualtion

	int tableSizeSin = ft->mSineSize;
	unit->m_lomask = (tableSizeSin - 1) << 3;
	unit->m_radtoinc = tableSizeSin * (rtwopi * 65536.);
	unit->m_cpstoinc = tableSizeSin * SAMPLEDUR * 65536.;
	unit->m_phasein = IN0(2);

	double gamconst = (15.0 * pi) / SAMPLERATE;
	double gamma01 = gamconst * 0.3609f;
	double gamma02 = gamconst * 2.7412f;
	double gamma03 = gamconst * 11.1573f;
	double gamma04 = gamconst * 44.7581f;
	double gamma05 = gamconst * 179.6242f;
	double gamma06 = gamconst * 798.4578f;
	double gamma07 = gamconst * 1.2524f;
	double gamma08 = gamconst * 5.5671f;
	double gamma09 = gamconst * 22.3423f;
	double gamma10 = gamconst * 89.6271f;
	double gamma11 = gamconst * 364.7914f;
	double gamma12 = gamconst * 2770.1114f;
	unit->m_coefs[0] = (gamma01 - 1.f) / (gamma01 + 1.f);
	unit->m_coefs[1] = (gamma02 - 1.f) / (gamma02 + 1.f);
	unit->m_coefs[2] = (gamma03 - 1.f) / (gamma03 + 1.f);
	unit->m_coefs[3] = (gamma04 - 1.f) / (gamma04 + 1.f);
	unit->m_coefs[4] = (gamma05 - 1.f) / (gamma05 + 1.f);
	unit->m_coefs[5] = (gamma06 - 1.f) / (gamma06 + 1.f);
	unit->m_coefs[6] = (gamma07 - 1.f) / (gamma07 + 1.f);
	unit->m_coefs[7] = (gamma08 - 1.f) / (gamma08 + 1.f);
	unit->m_coefs[8] = (gamma09 - 1.f) / (gamma09 + 1.f);
	unit->m_coefs[9] = (gamma10 - 1.f) / (gamma10 + 1.f);
	unit->m_coefs[10] = (gamma11 - 1.f) / (gamma11 + 1.f);
	unit->m_coefs[11] = (gamma12 - 1.f) / (gamma12 + 1.f);

	Clear(12, unit->m_y1);
	FreqShift_next_kk(unit, 1);
}

void FreqShift_next_kk(FreqShift *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float phasein = ZIN0(2);
	float freqin = ZIN0(1);
	float outcos, outsin; // the sample by sample output of the Hilbert
	float outsinosc, outsinoscHalfPi; // the samples from the oscil.
	int32 halfPi = (int32)(unit->m_radtoinc * (0.5 * pi));
	double y1[12];
	double coefs[12];

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	int32 lomask = unit->m_lomask;
	int32 phase = unit->m_phase;

	int32 freq = (int32)(unit->m_cpstoinc * freqin);
	int32 phaseinc = freq + (int32)(CALCSLOPE(phasein, unit->m_phasein) * unit->m_radtoinc);
	unit->m_phasein = phasein;

	// each filter's last sample
	for(int i = 0; i < 12; ++i) {
		y1[i] = unit->m_y1[i];
		coefs[i] = unit->m_coefs[i];
	}

	double ay1, ay2, ay3, ay4, ay5, ay6;
	double ay7, ay8, ay9, ay10, ay11, ay12;

	double y0_1, y0_2, y0_3, y0_4, y0_5, y0_6;
	double y0_7, y0_8, y0_9, y0_10, y0_11, y0_12;

	LOOP1(inNumSamples,
		float thisin = ZXP(in);

		HILBERT_FILTER

		outcos = ay6;
		outsin = ay12;

		outsinosc = lookupi1(table0, table1, phase, lomask);
		outsinoscHalfPi = lookupi1(table0, table1, phase + halfPi, lomask);

		ZXP(out) = (outcos * outsinoscHalfPi) + (outsinosc * outsin);

		phase += phaseinc;
	)

	unit->m_phase = phase;

	for(int i = 0; i < 12; ++i)
		unit->m_y1[i] = zapgremlins(y1[i]);
}

void FreqShift_next_aa(FreqShift *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float *phasein = ZIN(2);
	float *freqin = ZIN(1);
	float outcos, outsin; // the sample by sample output of the Hilbert
	float outsinosc, outsinoscHalfPi; // the samples from the oscil.
	int32 halfPi = (int32)(unit->m_radtoinc * (0.5 * pi));
	double y1[12];
	double coefs[12];

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	int32 lomask = unit->m_lomask;
	int32 phase = unit->m_phase;

	// each filter's last sample
	for(int i = 0; i < 12; ++i){
		y1[i] = unit->m_y1[i];
		coefs[i] = unit->m_coefs[i];
	}

	double ay1, ay2, ay3, ay4, ay5, ay6;
	double ay7, ay8, ay9, ay10, ay11, ay12;

	double y0_1, y0_2, y0_3, y0_4, y0_5, y0_6;
	double y0_7, y0_8, y0_9, y0_10, y0_11, y0_12;

	LOOP1(inNumSamples,
		float thisin = ZXP(in);

		HILBERT_FILTER

		outcos = ay6;
		outsin = ay12;

		int32 phaseoffset = phase + (int32)(unit->m_radtoinc * ZXP(phasein));
		outsinosc = lookupi1(table0, table1, phaseoffset, lomask);
		outsinoscHalfPi = lookupi1(table0, table1, phaseoffset + halfPi, lomask);
		phase += (int32)(unit->m_cpstoinc * ZXP(freqin));

		ZXP(out) = (outcos * outsinoscHalfPi) + (outsinosc * outsin);
	)

	unit->m_phase = phase;

	for(int i = 0; i < 12; ++i)
		unit->m_y1[i] = zapgremlins(y1[i]);
}

void FreqShift_next_ak(FreqShift *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float phasein = ZIN0(2);
	float *freqin = ZIN(1);
	float outcos, outsin; // the sample by sample output of the Hilbert
	float outsinosc, outsinoscHalfPi; // the samples from the oscil.
	int32 halfPi = (int32)(unit->m_radtoinc * (0.5 * pi));
	double y1[12];
	double coefs[12];

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	int32 lomask = unit->m_lomask;
	int32 phase = unit->m_phase;
	float phasemod = unit->m_phasein;
	float phaseslope = CALCSLOPE(phasein, phasemod);

	// each filter's last sample
	for(int i = 0; i < 12; ++i) {
		y1[i] = unit->m_y1[i];
		coefs[i] = unit->m_coefs[i];
	}

	double ay1, ay2, ay3, ay4, ay5, ay6;
	double ay7, ay8, ay9, ay10, ay11, ay12;

	double y0_1, y0_2, y0_3, y0_4, y0_5, y0_6;
	double y0_7, y0_8, y0_9, y0_10, y0_11, y0_12;

	LOOP1(inNumSamples,
		float thisin = ZXP(in);

		HILBERT_FILTER

		outcos = ay6;
		outsin = ay12;

		int32 pphase = phase + (int32)(unit->m_radtoinc * phasemod);
		phasemod += phaseslope;
		outsinosc = lookupi1(table0, table1, pphase, lomask);
		outsinoscHalfPi = lookupi1(table0, table1, pphase + halfPi, lomask);
		phase += (int32)(unit->m_cpstoinc * ZXP(freqin));

		ZXP(out) = (outcos * outsinoscHalfPi) + (outsinosc * outsin);
	)

	unit->m_phase = phase;
	unit->m_phasein = phasein;

	for(int i = 0; i < 12; ++i)
		unit->m_y1[i] = zapgremlins(y1[i]);
}

void FreqShift_next_ka(FreqShift *unit, int inNumSamples)
{
	float *in = ZIN(0);
	float *out = ZOUT(0);
	float *phasein = ZIN(2);
	float freqin = ZIN0(1);
	float outcos, outsin; // the sample by sample output of the Hilbert
	float outsinosc, outsinoscHalfPi; // the samples from the oscil.
	int32 halfPi = (int32)(unit->m_radtoinc * (0.5 * pi));
	double y1[12];
	double coefs[12];

	float *table0 = ft->mSineWavetable;
	float *table1 = table0 + 1;
	int32 lomask = unit->m_lomask;
	int32 phase = unit->m_phase;

	int32 freq = (int32)(unit->m_cpstoinc * freqin);

	// each filter's last sample
	for(int i = 0; i < 12; ++i) {
		y1[i] = unit->m_y1[i];
		coefs[i] = unit->m_coefs[i];
	}

	double ay1, ay2, ay3, ay4, ay5, ay6;
	double ay7, ay8, ay9, ay10, ay11, ay12;

	double y0_1, y0_2, y0_3, y0_4, y0_5, y0_6;
	double y0_7, y0_8, y0_9, y0_10, y0_11, y0_12;

	LOOP1(inNumSamples,
		float thisin = ZXP(in);

		HILBERT_FILTER

		outcos = ay6;
		outsin = ay12;

		int32 phaseoffset = phase + (int32)(unit->m_radtoinc * ZXP(phasein));
		outsinosc = lookupi1(table0, table1, phaseoffset, lomask);
		outsinoscHalfPi = lookupi1(table0, table1, phaseoffset + halfPi, lomask);
		phase += freq;

		ZXP(out) = (outcos * outsinoscHalfPi) + (outsinosc * outsin);
	)

	unit->m_phase = phase;

	for(int i = 0; i < 12; ++i)
		unit->m_y1[i] = zapgremlins(y1[i]);
}


/**
"MoogFF" - Moog VCF digital implementation.
As described in the paper entitled
"Preserving the Digital Structure of the Moog VCF"
by Federico Fontana
appeared in the Proc. ICMC07, Copenhagen, 25-31 August 2007

Original Java code Copyright F. Fontana - August 2007
federico.fontana@univr.it

Ported to C++ for SuperCollider by Dan Stowell - August 2007
http://www.mcld.co.uk/

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
//////////////////////////////////////////////////////////////////

void MoogFF_Ctor(MoogFF* unit)
{
	SETCALC(MoogFF_next);

	// initialize the unit generator state variables.
	unit->m_freq = uninitializedControl;
	unit->m_k    = IN0(2);
	unit->m_s1 = 0.f;
	unit->m_s2 = 0.f;
	unit->m_s3 = 0.f;
	unit->m_s4 = 0.f;

	// calculate one sample of output.
	MoogFF_next(unit, 1);
}

void MoogFF_next(MoogFF *unit, int inNumSamples)
{
	float k = IN0(2);
	k = sc_clip(k, 0.f, 4.f);

	// Load state from the struct
	double s1 = unit->m_s1;
	double s2 = unit->m_s2;
	double s3 = unit->m_s3;
	double s4 = unit->m_s4;

	// Reset filter state if requested
	if(IN0(3)>0)
		s1 = s2 = s3 = s4 = 0.f;

	double a1 = unit->m_a1, b0 = unit->m_b0; // Filter coefficient parameters

	// Update filter coefficients, but only if freq changes since it involves some expensive operations

	float freqIn = IN0(1);
	if(unit->m_freq != freqIn) {
		//Print("Updated freq to %g\n", freq);
		double T = SAMPLEDUR;
		double wcD = 2.0 * tan ( T * PI * freqIn ) * SAMPLERATE;
		if(wcD<0)
			wcD = 0; // Protect against negative cutoff freq
		double TwcD = T*wcD;
		b0 = TwcD/(TwcD + 2.);
		a1 = (TwcD - 2.)/(TwcD + 2.);
		unit->m_freq = freqIn;
		unit->m_b0 = b0;
		unit->m_a1 = a1;
	}

	float *out = ZOUT(0);
	float *in = ZIN(0);

	if (unit->m_k == k) {
		LOOP1(inNumSamples,
			// compute loop values
			double o = s4 + b0*(s3 + b0*(s2 + b0*s1));
			double ins = ZXP(in);
			double outs = (b0*b0*b0*b0*ins + o) * sc_reciprocal(1.0 + b0*b0*b0*b0*k);
			ZXP(out) = outs;
			double u = ins - k*outs;

			// update 1st order filter states
			double past = u;
			double future = b0*past + s1;
			s1 = b0*past - a1*future;

			past = future;
			future = b0*past + s2;
			s2 = b0*past - a1*future;

			past = future;
			future = b0*past + s3;
			s3 = b0*past - a1*future;

			s4 = b0*future - a1*outs;
		)
	} else {
		float new_k = k;
		float old_k = unit->m_k;
		float slope_k = CALCSLOPE(new_k, old_k);
		k = old_k;

		LOOP1(inNumSamples,
			// compute loop values
			double o = s4 + b0*(s3 + b0*(s2 + b0*s1));
			double ins = ZXP(in);
			double outs = (b0*b0*b0*b0*ins + o) * sc_reciprocal(1.0 + b0*b0*b0*b0*k);
			ZXP(out) = outs;
			double u = ins - k*outs;

			// update 1st order filter states
			double past = u;
			double future = b0*past + s1;
			s1 = b0*past - a1*future;

			past = future;
			future = b0*past + s2;
			s2 = b0*past - a1*future;

			past = future;
			future = b0*past + s3;
			s3 = b0*past - a1*future;

			s4 = b0*future - a1*outs;
			k += slope_k;
		);
		unit->m_k = new_k;
	}

	// Store state
	unit->m_s1 = zapgremlins(s1);
	unit->m_s2 = zapgremlins(s2);
	unit->m_s3 = zapgremlins(s3);
	unit->m_s4 = zapgremlins(s4);
}

/* BEQSuite */


static void inline BPerformFilterLoop(const Unit * unit, float * out, const float * in,
									  double & y1, double & y2, double a0, double a1, double a2,
									  double b1, double b2)
{
	LOOP(unit->mRate->mFilterLoops,
		double in0 = ZXP(in);
		double in1 = ZXP(in);
		double in2 = ZXP(in);

		double y0 = in0 + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = in1 + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = in2 + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	);
	LOOP(unit->mRate->mFilterRemain,
		double y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
	);
}

static void inline BPerformFilterLoop(const Unit * unit, float * out, const float * in,
									  double & y1, double & y2, double a0, double a1, double a2,
									  double b1, double b2,
									  double a0slope, double a1slope, double a2slope,
									  double b1slope, double b2slope)
{
	LOOP(unit->mRate->mFilterLoops,
		double in0 = ZXP(in);
		double in1 = ZXP(in);
		double in2 = ZXP(in);

		double y0 = in0 + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = in1 + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = in2 + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;

		a0 += a0slope;
		a1 += a1slope;
		a2 += a2slope;
		b1 += b1slope;
		b2 += b2slope;
	);
	LOOP(unit->mRate->mFilterRemain,
		double y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
	);
}

struct BLowPass : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rq;
};

extern "C"
{
	void BLowPass_next_ii(BLowPass *unit, int inNumSamples);
	void BLowPass_next_kk(BLowPass *unit, int inNumSamples);
	void BLowPass_next_aa(BLowPass *unit, int inNumSamples);
	void BLowPass_Ctor(BLowPass* unit);
}


void BLowPass_Ctor(BLowPass* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate))
		SETCALC(BLowPass_next_aa);
	else {
		if ((INRATE(1) == calc_ScalarRate) && (INRATE(2) == calc_ScalarRate))
			SETCALC(BLowPass_next_ii);
		else
			SETCALC(BLowPass_next_kk);
	}
	float rq = unit->m_rq = ZIN0(2);
	float freq = unit->m_freq = ZIN0(1);
	double w0 = twopi * (double)freq * SAMPLEDUR;
	double cosw0 = cos(w0);
	double i = 1. - cosw0;
	double alpha = sin(w0) * 0.5 * (double)rq;
	double b0rz = 1. / (1. + alpha);
	double a0 = unit->m_a0 = i * 0.5 * b0rz;
	unit->m_a1 = i * b0rz;
	unit->m_a2 = a0;
	unit->m_b1 = cosw0 * 2. * b0rz;
	unit->m_b2 = (1. - alpha) * -b0rz;
	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	PUSH_LOOPVALS
	BLowPass_next_kk(unit, 1);
	POP_LOOPVALS
}

void BLowPass_next_aa(BLowPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rq = ZIN(2);

	double a0, a1, a2, b1, b2, w0, cosw0, i, alpha, b0rz;
	double y0, y1, y2;
	float nextfreq, nextrq;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	LOOP(unit->mRate->mFilterLoops,
		nextfreq = ZXP(freq);
		nextrq = ZXP(rq);
		if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)) {
			w0 = twopi * (double)nextfreq * SAMPLEDUR;
			cosw0 = cos(w0);
			i = 1. - cosw0;
			alpha = sin(w0) * 0.5 * (double)nextrq;
			b0rz = 1. / (1. + alpha);
			a0 = i * 0.5 * b0rz;
			a1 = i * b0rz;
			a2 = a0;
			b1 = cosw0 * 2. * b0rz;
			b2 = (1. - alpha) * -b0rz;
			unit->m_freq = nextfreq;
			unit->m_rq = nextrq;
		}
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = ZXP(in) + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = ZXP(in) + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
		);

	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
		unit->m_freq = ZXP(freq);
		unit->m_rq = ZXP(rq);
	);

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BLowPass_next_kk(BLowPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrq = ZIN0(2);

	double a0, a1, a2, b1, b2;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)){
		double a0slope, a1slope, a2slope, b1slope, b2slope;
		double next_a0, next_a1, next_a2, next_b1, next_b2, next_w0, next_cosw0, next_i, next_alpha, next_b0rz;

		next_w0 = twopi * (double)nextfreq * SAMPLEDUR;
		next_cosw0 = cos(next_w0);
		next_i = 1. - next_cosw0;
		next_alpha = sin(next_w0) * 0.5 * (double)nextrq;
		next_b0rz = 1. / (1. + next_alpha);
		next_a0 = next_i * 0.5 * next_b0rz;
		next_a1 = next_i * next_b0rz;
		next_a2 = next_a0;
		next_b1 = next_cosw0 * 2. * next_b0rz;
		next_b2 = ((1. - next_alpha) * -next_b0rz);
		a0slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		a1slope = (next_a1 - a1) * unit->mRate->mFilterSlope;
		a2slope = (next_a2 - a2) * unit->mRate->mFilterSlope;
		b1slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		b2slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		unit->m_freq = nextfreq;
		unit->m_rq = nextrq;
		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2, a0slope, a1slope, a2slope, b1slope, b2slope);

		unit->m_a0 = next_a0;
		unit->m_a1 = next_a1;
		unit->m_a2 = next_a2;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else
		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void BLowPass_next_ii(BLowPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);

	double a0, a1, a2, b1, b2;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


struct BHiPass : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rq;
};


extern "C"
{
	void BHiPass_next_ii(BHiPass *unit, int inNumSamples);
	void BHiPass_next_kk(BHiPass *unit, int inNumSamples);
	void BHiPass_next_aa(BHiPass *unit, int inNumSamples);
	void BHiPass_Ctor(BHiPass* unit);
}

void BHiPass_Ctor(BHiPass* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate))
		SETCALC(BHiPass_next_aa);
	else {
		if ((INRATE(1) == calc_ScalarRate) && (INRATE(2) == calc_ScalarRate))
			SETCALC(BHiPass_next_ii);
		else
			SETCALC(BHiPass_next_kk);
	}
	float freq = unit->m_freq = ZIN0(1);
	float rq = unit->m_rq = ZIN0(2);
	double w0 = twopi * (double)freq * SAMPLEDUR;
	double cosw0 = cos(w0);
	double i = 1. + cosw0;
	double alpha = sin(w0) * 0.5 * (double)rq;
	double b0rz = 1. / (1. + alpha);
	double a0 = unit->m_a0 = i * 0.5 * b0rz;
	unit->m_a1 = -i * b0rz;
	unit->m_a2 = a0;
	unit->m_b1 = cosw0 * 2. * b0rz;
	unit->m_b2 = (1. - alpha) * -b0rz;
	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	PUSH_LOOPVALS
	BHiPass_next_kk(unit, 1);
	POP_LOOPVALS
}

void BHiPass_next_aa(BHiPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rq = ZIN(2);

	double a0, a1, a2, b1, b2, w0, cosw0, i, alpha, b0rz;
	double y0, y1, y2;
	float nextfreq, nextrq;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	LOOP(unit->mRate->mFilterLoops,
		nextfreq = ZXP(freq);
		nextrq = ZXP(rq);
		if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)) {
			w0 = twopi * (double)nextfreq * SAMPLEDUR;
			cosw0 = cos(w0);
			i = 1. + cosw0;
			alpha = sin(w0) * 0.5 * (double)nextrq;
			b0rz = 1. / (1. + alpha);
			a0 = i * 0.5 * b0rz;
			a1 = -i * b0rz;
			a2 = a0;
			b1 = cosw0 * 2. * b0rz;
			b2 = (1. - alpha) * -b0rz;
			unit->m_freq = nextfreq;
			unit->m_rq = nextrq;
		}
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = ZXP(in) + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = ZXP(in) + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	);
	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
	);

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BHiPass_next_kk(BHiPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrq = ZIN0(2);

	double a0, a1, a2, b1, b2;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)){
		double a0slope, a1slope, a2slope, b1slope, b2slope;
		double next_a0, next_a1, next_a2, next_b1, next_b2, next_w0, next_cosw0, next_i, next_alpha, next_b0rz;

		next_w0 = twopi * (double)nextfreq * SAMPLEDUR;
		next_cosw0 = cos(next_w0);
		next_i = 1. + next_cosw0;
		next_alpha = sin(next_w0) * 0.5 * (double)nextrq;
		next_b0rz = 1. / (1. + next_alpha);
		next_a0 = next_i * 0.5 * next_b0rz;
		next_a1 = -next_i * next_b0rz;
		next_a2 = next_a0;
		next_b1 = next_cosw0 * 2. * next_b0rz;
		next_b2 = (1. - next_alpha) * -next_b0rz;
		a0slope = (next_a0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0);
		a1slope = (next_a1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1);
		a2slope = (next_a2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2);
		b1slope = (next_b1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1);
		b2slope = (next_b2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2);
		unit->m_freq = nextfreq;
		unit->m_rq = nextrq;

		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2, a0slope, a1slope, a2slope, b1slope, b2slope);

		unit->m_a0 = next_a0;
		unit->m_a1 = next_a1;
		unit->m_a2 = next_a2;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else
		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void BHiPass_next_ii(BHiPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);

	double a0, a1, a2, b1, b2;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


/* BBandPass */

struct BBandPass : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_bw;
};

extern "C"
{
	void BBandPass_next_kk(BBandPass *unit, int inNumSamples);
	void BBandPass_next_aa(BBandPass *unit, int inNumSamples);
	void BBandPass_Ctor(BBandPass* unit);
}

void BBandPass_Ctor(BBandPass* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate))
		SETCALC(BBandPass_next_aa);
	else
		SETCALC(BBandPass_next_kk);
	float freq = unit->m_freq = ZIN0(1);
	float bw = unit->m_bw = ZIN0(2);

	double w0 = twopi * (double)freq * SAMPLEDUR;
	double sinw0 = sin(w0);
	double alpha = sinw0 * (sinh((0.34657359027997 * (double)bw * w0) / sinw0));
	double b0rz = 1. / (1. + alpha);
	double a0 = unit->m_a0 = alpha * b0rz;
	unit->m_a1 = 0.0f;
	unit->m_a2 = -a0;
	unit->m_b1 = cos(w0) * 2. * b0rz;
	unit->m_b2 = (1. - alpha) * -b0rz;

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	PUSH_LOOPVALS
	BBandPass_next_kk(unit, 1);
	POP_LOOPVALS
}

void BBandPass_next_aa(BBandPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *bw = ZIN(2);

	double a0, a1, a2, b1, b2, w0, sinw0, alpha, b0rz;
	double y0, y1, y2;
	float nextfreq, nextbw;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	LOOP(unit->mRate->mFilterLoops,
		nextfreq = ZXP(freq);
		nextbw = ZXP(bw);
		if ((unit->m_freq != nextfreq) || (unit->m_bw != nextbw)) {
			w0 = twopi * (double)nextfreq * SAMPLEDUR;
			sinw0 = sin(w0);
			alpha = sinw0 * (sinh((0.34657359027997 * (double)nextbw * w0) / sinw0));
			b0rz = 1. / (1. + alpha);
			a0 = alpha * b0rz;
			a1 = 0.0f;
			a2 = -a0;
			b1 = cos(w0) * 2. * b0rz;
			b2 = (1. - alpha) * -b0rz;
			unit->m_freq = ZXP(freq);
			unit->m_bw = ZXP(bw);
		}
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = ZXP(in) + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = ZXP(in) + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	);
	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
		unit->m_freq = nextfreq;
		unit->m_bw = nextbw;
	);

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BBandPass_next_kk(BBandPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextbw = ZIN0(2);

	double a0, a1, a2, b1, b2;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;
	if ((unit->m_freq != nextfreq) || (unit->m_bw != nextbw)) {
		double a0slope, a1slope, a2slope, b1slope, b2slope;
		double next_a0, next_a1, next_a2, next_b1, next_b2, next_w0, next_alpha, next_b0rz;

		next_w0 = twopi * (double)nextfreq * SAMPLEDUR;
		next_alpha = sin(next_w0) * (sinh((0.34657359027997 * (double)nextbw * next_w0) / sin(next_w0)));
		next_b0rz = 1. / (1. + next_alpha);
		next_a0 = next_alpha * next_b0rz;
		next_a1 = 0.;
		next_a2 = -next_a0;
		next_b1 = cos(next_w0) * 2. * next_b0rz;
		next_b2 = ((1. - next_alpha) * -next_b0rz);
		a0slope = (next_a0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0);
		a1slope = (next_a1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1);
		a2slope = (next_a2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2);
		b1slope = (next_b1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1);
		b2slope = (next_b2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2);
		unit->m_freq = nextfreq;
		unit->m_bw = nextbw;

		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2, a0slope, a1slope, a2slope, b1slope, b2slope);

		unit->m_a0 = next_a0;
		unit->m_a1 = next_a1;
		unit->m_a2 = next_a2;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else
		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

/* BBandStop */
struct BBandStop : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_bw;
};

extern "C"
{
	void BBandStop_next_kk(BBandStop *unit, int inNumSamples);
	void BBandStop_next_aa(BBandStop *unit, int inNumSamples);
	void BBandStop_Ctor(BBandStop* unit);
}

void BBandStop_Ctor(BBandStop* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate))
		SETCALC(BBandStop_next_aa);
	else
		SETCALC(BBandStop_next_kk);
	float freq = unit->m_freq = ZIN0(1);
	float bw = unit->m_bw = ZIN0(2);

	double w0 = twopi * (double)freq * SAMPLEDUR;
	double sinw0 = sin(w0);
	double alpha = sinw0 * (sinh((0.34657359027997 * (double)bw * w0) / sinw0));
	double b0rz = 1. / (1. + alpha);
	double b1 = unit->m_b1 = 2. * b0rz * cos(w0);
	unit->m_a0 = b0rz;
	unit->m_a1 = -b1;
	unit->m_a2 = b0rz;
	unit->m_b2 = (1. - alpha) * -b0rz;

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	PUSH_LOOPVALS
	BBandStop_next_kk(unit, 1);
	POP_LOOPVALS
}

void BBandStop_next_aa(BBandStop *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *bw = ZIN(2);

	double a0, a1, a2, b1, b2, w0, sinw0, alpha, b0rz;
	double y0, y1, y2;
	float nextfreq, nextbw;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	LOOP(unit->mRate->mFilterLoops,
		nextfreq = ZXP(freq);
		nextbw = ZXP(bw);
		if ((unit->m_freq != nextfreq) || (unit->m_bw != nextbw)) {
			w0 = twopi * (double)nextfreq * SAMPLEDUR;
			sinw0 = sin(w0);
			alpha = sinw0 * (sinh((0.34657359027997 * (double)nextbw * w0) / sinw0));
			b0rz = 1. / (1. + alpha);
			b1 = 2. * b0rz * cos(w0);
			a0 = b0rz;
			a1 = -b1;
			a2 = b0rz;
			b2 = (1. - alpha) * -b0rz;
			unit->m_freq = ZXP(freq);
			unit->m_bw = ZXP(bw);
		}
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = ZXP(in) + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = ZXP(in) + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	);
	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
		unit->m_freq = nextfreq;
		unit->m_bw = nextbw;
	);

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BBandStop_next_kk(BBandStop *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextbw = ZIN0(2);

	double a0, a1, a2, b1, b2;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_bw != nextbw)){
		double a0slope, a1slope, a2slope, b1slope, b2slope;
		double next_a0, next_a1, next_a2, next_b1, next_b2, next_w0, next_alpha, next_b0rz;

		next_w0 = twopi * (double)nextfreq * SAMPLEDUR;
		next_alpha = sin(next_w0) * (sinh((0.34657359027997 * (double)nextbw * next_w0) / sin(next_w0)));
		next_b0rz = 1. / (1. + next_alpha);
		next_b1 = cos(next_w0) * 2. * next_b0rz;
		next_a0 = next_b0rz;
		next_a1 = -next_b1;
		next_a2 = next_b0rz;
		next_b2 = ((1. - next_alpha) * -next_b0rz);
		a0slope = (next_a0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0);
		a1slope = (next_a1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1);
		a2slope = (next_a2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2);
		b1slope = (next_b1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1);
		b2slope = (next_b2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2);
		unit->m_freq = nextfreq;
		unit->m_bw = nextbw;

		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2, a0slope, a1slope, a2slope, b1slope, b2slope);

		unit->m_a0 = next_a0;
		unit->m_a1 = next_a1;
		unit->m_a2 = next_a2;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else
		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

/* BPeakEQ */
struct BPeakEQ : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rq, m_db;
};

extern "C"
{
	void BPeakEQ_next_kkk(BPeakEQ *unit, int inNumSamples);
	void BPeakEQ_next_aaa(BPeakEQ *unit, int inNumSamples);
	void BPeakEQ_Ctor(BPeakEQ* unit);
}

void BPeakEQ_Ctor(BPeakEQ* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate) && (INRATE(3) == calc_FullRate))
		SETCALC(BPeakEQ_next_aaa);
	else
	SETCALC(BPeakEQ_next_kkk);

	float freq = unit->m_freq = ZIN0(1);
	float rq = unit->m_rq = ZIN0(2);
	float db = unit->m_db = ZIN0(3);
	double a = pow(10., (double)db * 0.025);
	double w0 = twopi * (double)freq * SAMPLEDUR;
	double alpha = sin(w0) * 0.5 * (double)rq;
	double b0rz = 1. / (1. + (alpha / a));
	double b1 = unit->m_b1 = 2. * b0rz * cos(w0);
	unit->m_a0 = (1. + (alpha * a)) * b0rz;
	unit->m_a1 = -b1;
	unit->m_a2 = (1. - (alpha * a)) * b0rz;
	unit->m_b2 = (1. - (alpha / a)) * -b0rz;

	unit->m_y1 = 0.;
	unit->m_y2 = 0.;
	PUSH_LOOPVALS
	BPeakEQ_next_kkk(unit, 1);
	POP_LOOPVALS
}

void BPeakEQ_next_aaa(BPeakEQ *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rq = ZIN(2);
	float *db = ZIN(3);

	double a0, a1, a2, b1, b2, a, w0, alpha, b0rz;
	double y0, y1, y2;
	float nextfreq, nextrq, nextdb;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;


	LOOP(unit->mRate->mFilterLoops,
		nextfreq = ZXP(freq);
		nextrq = ZXP(rq);
		nextdb = ZXP(db);
		if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq) || (unit->m_db != nextdb)) {
			a = pow(10., (double)nextdb * 0.025);
			w0 = twopi * (double)nextfreq * SAMPLEDUR;
			alpha = sin(w0) * 0.5 * (double)nextrq;
			b0rz = 1. / (1. + (alpha / a));
			b1 = 2. * b0rz * cos(w0);
			a0 = (1. + (alpha * a)) * b0rz;
			a1 = -b1;
			a2 = (1. - (alpha * a)) * b0rz;
			b2 = (1. - (alpha / a)) * -b0rz;
			unit->m_freq = nextfreq;
			unit->m_rq = nextrq;
			unit->m_db = nextdb;
		}
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = ZXP(in) + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = ZXP(in) + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	);
	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
		unit->m_freq = ZXP(freq);
		unit->m_rq = ZXP(rq);
		unit->m_db = ZXP(db);
	);

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BPeakEQ_next_kkk(BPeakEQ *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrq = ZIN0(2);
	float nextdb = ZIN0(3);

	double a0, a1, a2, b1, b2, a, w0, alpha, b0rz;
	double y1, y2;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq) || (unit->m_db != nextdb)) {
		double a0slope, a1slope, a2slope, b1slope, b2slope;
		double next_a0, next_a1, next_a2, next_b1, next_b2;

		a = pow(10., (double)nextdb * 0.025);
		w0 = twopi * (double)nextfreq * SAMPLEDUR;
		alpha = sin(w0) * 0.5 * (double)nextrq;
		b0rz = 1. / (1. + (alpha / a));
		next_b1 = 2. * b0rz * cos(w0);
		next_a0 = (1. + (alpha * a)) * b0rz;
		next_a1 = -next_b1;
		next_a2 = (1. - (alpha * a)) * b0rz;
		next_b2 = (1. - (alpha / a)) * -b0rz;
		a0slope = (next_a0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0);
		a1slope = (next_a1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1);
		a2slope = (next_a2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2);
		b1slope = (next_b1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1);
		b2slope = (next_b2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2);
		unit->m_freq = nextfreq;
		unit->m_db = nextdb;
		unit->m_rq = nextrq;

		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2, a0slope, a1slope, a2slope, b1slope, b2slope);

		unit->m_a0 = next_a0;
		unit->m_a1 = next_a1;
		unit->m_a2 = next_a2;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;
	} else
		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

/* BAllPass */
struct BAllPass : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rq;
};

extern "C"
{
	void BAllPass_next_kk(BAllPass *unit, int inNumSamples);
	void BAllPass_next_aa(BAllPass *unit, int inNumSamples);
	void BAllPass_Ctor(BAllPass* unit);
}

void BAllPass_Ctor(BAllPass* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate))
		SETCALC(BAllPass_next_aa);
	else
		SETCALC(BAllPass_next_kk);

	float freq = unit->m_freq = ZIN0(1);
	float rq = unit->m_rq = ZIN0(2);

	double w0 = twopi * (double)freq * SAMPLEDUR;
	double alpha = sin(w0) * 0.5 * (double)rq;
	double b0rz = 1. / (1. + alpha);
	double a0 = unit->m_a0 = (1. - alpha) * b0rz;
	double b1 = unit->m_b1 = 2. * b0rz * cos(w0);
	unit->m_a1 = -b1;
	unit->m_a2 = 1.;
	unit->m_b2 = -a0;

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	PUSH_LOOPVALS
	BAllPass_next_kk(unit, 1);
	POP_LOOPVALS
}

void BAllPass_next_aa(BAllPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rq = ZIN(2);

	double a0, a1, a2, b1, b2, w0, alpha, b0rz;
	double y0, y1, y2;
	float nextfreq, nextrq;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	LOOP(unit->mRate->mFilterLoops,
		nextfreq = ZXP(freq);
		nextrq = ZXP(rq);
		if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)) {
			w0 = twopi * (double)nextfreq * SAMPLEDUR;
			alpha = sin(w0) * (double)nextrq * 0.5;
			b0rz = 1. / (1. + alpha);
			b1 = 2. * b0rz * cos(w0);
			a0 = (1. - alpha) * b0rz;
			a1 = -b1;
			a2 = 1.;
			b2 = -a0;
			unit->m_freq = nextfreq;
			unit->m_rq = nextrq;
		}
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = ZXP(in) + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = ZXP(in) + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	);
	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
		unit->m_freq = ZXP(freq);
		unit->m_rq = ZXP(rq);
	);

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


void BAllPass_next_kk(BAllPass *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrq = ZIN0(2);

	double a0, a1, a2, b1, b2;
	double y1 = unit->m_y1;
	double y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_rq != nextrq)) {
		double a0slope, a1slope, a2slope, b1slope, b2slope;
		double next_a0, next_a1, next_a2, next_b1, next_b2, next_w0, next_alpha, next_b0rz;
		next_w0 = twopi * (double)nextfreq * SAMPLEDUR;
		next_alpha = sin(next_w0) * 0.5 * (double)nextrq;
		next_b0rz = 1. / (1. + next_alpha);
		next_b1 = cos(next_w0) * 2. * next_b0rz;
		next_a0 = (1. - next_alpha) * next_b0rz;
		next_a1 = -next_b1;
		next_a2 = 1.;
		next_b2 = -next_a0;
		a0slope = (next_a0 - a0) * unit->mRate->mFilterSlope;
		a1slope = (next_a1 - a1) * unit->mRate->mFilterSlope;
		a2slope = (next_a2 - a2) * unit->mRate->mFilterSlope;
		b1slope = (next_b1 - b1) * unit->mRate->mFilterSlope;
		b2slope = (next_b2 - b2) * unit->mRate->mFilterSlope;
		unit->m_freq = nextfreq;
		unit->m_rq = nextrq;

		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2, a0slope, a1slope, a2slope, b1slope, b2slope);

		unit->m_a0 = next_a0;
		unit->m_a1 = next_a1;
		unit->m_a2 = next_a2;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;

	} else
		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}


/* BLowShelf */
struct BLowShelf : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rs, m_db;
};

extern "C"
{
	void BLowShelf_next_kkk(BLowShelf *unit, int inNumSamples);
	void BLowShelf_next_aaa(BLowShelf *unit, int inNumSamples);
	void BLowShelf_Ctor(BLowShelf* unit);
}

void BLowShelf_Ctor(BLowShelf* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate) && (INRATE(3) == calc_FullRate))
		SETCALC(BLowShelf_next_aaa);
	else
		SETCALC(BLowShelf_next_kkk);

	float freq = unit->m_freq = ZIN0(1);
	float rs = unit->m_rs = ZIN0(2);
	float db = unit->m_db = ZIN0(3);
	double a = pow(10., (double)db * 0.025);
	double w0 = twopi * (double)freq * SAMPLEDUR;
	double cosw0 = cos(w0);
	double sinw0 = sin(w0);
	double alpha = sinw0 * 0.5 * sqrt((a + (1./a)) * ((double)rs - 1.) + 2.);
	double i = (a + 1.) * cosw0;
	double j = (a - 1.) * cosw0;
	double k = 2. * sqrt(a) * alpha;
	double b0rz = 1. / ((a + 1.) + j + k);
	unit->m_a0 = a * (( a + 1.) - j + k) * b0rz;
	unit->m_a1 = 2. * a * ((a - 1.) - i) * b0rz;
	unit->m_a2 = a * ((a + 1.) - j - k) * b0rz;
	unit->m_b1 = 2. * ((a - 1.) + i) * b0rz;
	unit->m_b2 = ((a + 1.) + j - k) * -b0rz;

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	PUSH_LOOPVALS
	BLowShelf_next_kkk(unit, 1);
	POP_LOOPVALS
}

void BLowShelf_next_aaa(BLowShelf *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rs = ZIN(2);
	float *db = ZIN(3);

	double a0, a1, a2, b1, b2, a, w0, cosw0, sinw0, alpha, i, j, k, b0rz;
	double y0, y1, y2;
	float nextfreq, nextrs, nextdb;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	LOOP(unit->mRate->mFilterLoops,
		nextfreq = ZXP(freq);
		nextrs = ZXP(rs);
		nextdb = ZXP(db);
		if ((unit->m_freq != nextfreq) || (unit->m_rs != nextrs) || (unit->m_db != nextdb)) {
			a = pow(10., (double)nextdb * 0.025);
			w0 = twopi * (double)nextfreq * SAMPLEDUR;
			sinw0 = sin(w0);
			cosw0 = cos(w0);
			alpha = sinw0 * 0.5 * sqrt((a + (1./a)) * ((double)nextrs - 1.) + 2.);
			i = (a + 1.) * cosw0;
			j = (a - 1.) * cosw0;
			k = 2. * sqrt(a) * alpha;
			b0rz = 1. / ((a + 1.) + j + k);
			a0 = a * (( a + 1.) - j + k) * b0rz;
			a1 = 2. * a * ((a - 1.) - i) * b0rz;
			a2 = a * ((a + 1.) - j - k) * b0rz;
			b1 = 2. * ((a - 1.) + i) * b0rz;
			b2 = ((a + 1.) + j - k) * -b0rz;
			unit->m_freq = nextfreq;
			unit->m_rs = nextrs;
			unit->m_db = nextdb;
		}
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = ZXP(in) + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = ZXP(in) + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	);
	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
		unit->m_freq = ZXP(freq);
		unit->m_rs = ZXP(rs);
		unit->m_db = ZXP(db);
	);

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void BLowShelf_next_kkk(BLowShelf *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrs = ZIN0(2);
	float nextdb = ZIN0(3);

	double a0, a1, a2, b1, b2, a, w0, cosw0, sinw0, alpha, i, j, k, b0rz;
	double y1, y2;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_rs != nextrs) || (unit->m_db != nextdb)) {
		double a0slope, a1slope, a2slope, b1slope, b2slope, nexta0, nexta1, nexta2, nextb1, nextb2;
		a = pow(10., (double)nextdb * 0.025);
		w0 = twopi * (double)nextfreq * SAMPLEDUR;
		sinw0 = sin(w0);
		cosw0 = cos(w0);
		alpha = sinw0 * 0.5 * sqrt((a + (1./a)) * ((double)nextrs - 1.) + 2.);
		i = (a + 1.) * cosw0;
		j = (a - 1.) * cosw0;
		k = 2. * sqrt(a) * alpha;
		b0rz = 1. / ((a + 1.) + j + k);
		nexta0 = a * (( a + 1.) - j + k) * b0rz;
		nexta1 = 2. * a * ((a - 1.) - i) * b0rz;
		nexta2 = a * ((a + 1.) - j - k) * b0rz;
		nextb1 = 2. * ((a - 1.) + i) * b0rz;
		nextb2 = ((a + 1.) + j - k) * -b0rz;
		a0slope = (nexta0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0);
		a1slope = (nexta1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1);
		a2slope = (nexta2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2);
		b1slope = (nextb1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1);
		b2slope = (nextb2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2);
		unit->m_freq = nextfreq;
		unit->m_db = nextdb;
		unit->m_rs = nextrs;

		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2, a0slope, a1slope, a2slope, b1slope, b2slope);

		unit->m_a0 = nexta0;
		unit->m_a1 = nexta1;
		unit->m_a2 = nexta2;
		unit->m_b1 = nextb1;
		unit->m_b2 = nextb2;
	} else
		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

/* BHiShelf */
struct BHiShelf : public Unit
{
	double m_y1, m_y2, m_a0, m_a1, m_a2, m_b1, m_b2;
	float m_freq, m_rs, m_db;
};

extern "C"
{
	void BHiShelf_next_kkk(BHiShelf *unit, int inNumSamples);
	void BHiShelf_next_aaa(BHiShelf *unit, int inNumSamples);
	void BHiShelf_Ctor(BHiShelf* unit);
}

void BHiShelf_Ctor(BHiShelf* unit)
{
	if ((INRATE(1) == calc_FullRate) && (INRATE(2) == calc_FullRate) && (INRATE(3) == calc_FullRate))
		SETCALC(BHiShelf_next_aaa);
	else
		SETCALC(BHiShelf_next_kkk);

	float freq = unit->m_freq = ZIN0(1);
	float rs = unit->m_rs = ZIN0(2);
	float db = unit->m_db = ZIN0(3);
	double a = pow(10., (double)db * 0.025);
	double w0 = twopi * (double)freq * SAMPLEDUR;
	double cosw0 = cos(w0);
	double sinw0 = sin(w0);
	double alpha = sinw0 * 0.5 * sqrt((a + (1./a)) * ((double)rs - 1.) + 2.);
	double i = (a + 1.) * cosw0;
	double j = (a - 1.) * cosw0;
	double k = 2. * sqrt(a) * alpha;

	double b0rz = 1. / ((a + 1.) - j + k);
	unit->m_a0 = a * (( a + 1.) + j + k) * b0rz;
	unit->m_a1 = -2. * a * ((a - 1.) + i) * b0rz;
	unit->m_a2 = a * ((a + 1.) + j - k) * b0rz;
	unit->m_b1 = -2. * ((a - 1.) - i) * b0rz;
	unit->m_b2 = ((a + 1.) - j - k) * -b0rz;

	unit->m_y1 = 0.;

	unit->m_y2 = 0.;
	PUSH_LOOPVALS
	BHiShelf_next_kkk(unit, 1);
	POP_LOOPVALS
}

void BHiShelf_next_aaa(BHiShelf *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float *freq = ZIN(1);
	float *rs = ZIN(2);
	float *db = ZIN(3);

	double a0, a1, a2, b1, b2, a, w0, cosw0, sinw0, alpha, i, j, k, b0rz;
	double y0, y1, y2;
	float nextfreq, nextrs, nextdb;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	LOOP(unit->mRate->mFilterLoops,
		nextfreq = ZXP(freq);
		nextrs = ZXP(rs);
		nextdb = ZXP(db);
		if ((unit->m_freq != nextfreq) || (unit->m_rs != nextrs) || (unit->m_db != nextdb)) {
			a = pow(10., (double)nextdb * 0.025);
			w0 = twopi * (double)nextfreq * SAMPLEDUR;
			sinw0 = sin(w0);
			cosw0 = cos(w0);
			alpha = sinw0 * 0.5 * sqrt((a + (1./a)) * ((double)nextrs - 1.) + 2.);
			i = (a + 1.) * cosw0;
			j = (a - 1.) * cosw0;
			k = 2. * sqrt(a) * alpha;
			b0rz = 1. / ((a + 1.) - j + k);
			a0 = a * (( a + 1.) + j + k) * b0rz;
			a1 = -2. * a * ((a - 1.) + i) * b0rz;
			a2 = a * ((a + 1.) + j - k) * b0rz;
			b1 = -2. * ((a - 1.) - i) * b0rz;
			b2 = ((a + 1.) - j - k) * -b0rz;
			unit->m_freq = ZXP(freq);
			unit->m_rs = ZXP(rs);
			unit->m_db = ZXP(db);
		}
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;

		y2 = ZXP(in) + b1 * y0 + b2 * y1;
		ZXP(out) = a0 * y2 + a1 * y0 + a2 * y1;

		y1 = ZXP(in) + b1 * y2 + b2 * y0;
		ZXP(out) = a0 * y1 + a1 * y2 + a2 * y0;
	);
	LOOP(unit->mRate->mFilterRemain,
		y0 = ZXP(in) + b1 * y1 + b2 * y2;
		ZXP(out) = a0 * y0 + a1 * y1 + a2 * y2;
		y2 = y1;
		y1 = y0;
		unit->m_freq = ZXP(freq);
		unit->m_rs = ZXP(rs);
		unit->m_db = ZXP(db);
	);

	unit->m_a0 = a0;
	unit->m_a1 = a1;
	unit->m_a2 = a2;
	unit->m_b1 = b1;
	unit->m_b2 = b2;
	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

void BHiShelf_next_kkk(BHiShelf *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *in = ZIN(0);
	float nextfreq = ZIN0(1);
	float nextrs = ZIN0(2);
	float nextdb = ZIN0(3);

	double a0, a1, a2, b1, b2, a, w0, cosw0, sinw0, alpha, i, j, k, b0rz;
	double y1, y2;

	y1 = unit->m_y1;
	y2 = unit->m_y2;

	a0 = unit->m_a0;
	a1 = unit->m_a1;
	a2 = unit->m_a2;
	b1 = unit->m_b1;
	b2 = unit->m_b2;

	if ((unit->m_freq != nextfreq) || (unit->m_rs != nextrs) || (unit->m_db != nextdb)) {
		double a0slope, a1slope, a2slope, b1slope, b2slope;
		double next_a0, next_a1, next_a2, next_b1, next_b2;
		a = pow(10., (double)nextdb * 0.025);
		w0 = twopi * (double)nextfreq * SAMPLEDUR;
		sinw0 = sin(w0);
		cosw0 = cos(w0);
		alpha = sinw0 * 0.5 * sqrt((a + (1./a)) * ((double)nextrs - 1.) + 2.);
		i = (a + 1.) * cosw0;
		j = (a - 1.) * cosw0;
		k = 2. * sqrt(a) * alpha;
		b0rz = 1. / ((a + 1.) - j + k);
		next_a0 = a * (( a + 1.) + j + k) * b0rz;
		next_a1 = -2. * a * ((a - 1.) + i) * b0rz;
		next_a2 = a * ((a + 1.) + j - k) * b0rz;
		next_b1 = -2. * ((a - 1.) - i) * b0rz;
		next_b2 = ((a + 1.) - j - k) * -b0rz;
		a0slope = (next_a0 - a0) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta0, a0);
		a1slope = (next_a1 - a1) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta1, a1);
		a2slope = (next_a2 - a2) * unit->mRate->mFilterSlope; //CALCSLOPE(nexta2, a2);
		b1slope = (next_b1 - b1) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb1, b1);
		b2slope = (next_b2 - b2) * unit->mRate->mFilterSlope; //CALCSLOPE(nextb2, b2);
		unit->m_freq = nextfreq;
		unit->m_db = nextdb;
		unit->m_rs = nextrs;

		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2, a0slope, a1slope, a2slope, b1slope, b2slope);

		unit->m_a0 = next_a0;
		unit->m_a1 = next_a1;
		unit->m_a2 = next_a2;
		unit->m_b1 = next_b1;
		unit->m_b2 = next_b2;

	} else
		BPerformFilterLoop(unit, out, in, y1, y2, a0, a1, a2, b1, b2);

	unit->m_y1 = zapgremlins(y1);
	unit->m_y2 = zapgremlins(y2);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
PluginLoad(Filter)
{
	ft = inTable;

	DefineSimpleUnit(Ramp);
	DefineSimpleUnit(Lag);
	DefineSimpleUnit(Lag2);
	DefineSimpleUnit(Lag3);
	DefineSimpleUnit(LagUD);
	DefineSimpleUnit(Lag2UD);
	DefineSimpleUnit(Lag3UD);
	DefineSimpleUnit(VarLag);
	DefineSimpleUnit(OnePole);
	DefineSimpleUnit(OneZero);
	DefineSimpleUnit(TwoPole);
	DefineSimpleUnit(TwoZero);
	DefineSimpleUnit(Decay);
	DefineSimpleUnit(Decay2);
	DefineSimpleUnit(Flip);
	DefineSimpleUnit(Delay1);
	DefineSimpleUnit(Delay2);
	DefineSimpleUnit(Integrator);
	DefineSimpleUnit(LeakDC);
	DefineSimpleUnit(LPZ1);
	DefineSimpleUnit(HPZ1);
	DefineSimpleUnit(LPZ2);
	DefineSimpleUnit(HPZ2);
	DefineSimpleUnit(BPZ2);
	DefineSimpleUnit(BRZ2);
	DefineSimpleUnit(APF);
	DefineSimpleUnit(LPF);
	DefineSimpleUnit(HPF);
	DefineSimpleUnit(BPF);
	DefineSimpleUnit(BRF);
	DefineSimpleUnit(RLPF);
	DefineSimpleUnit(RHPF);

	DefineSimpleUnit(Slew);
	DefineSimpleUnit(Slope);

	DefineSimpleUnit(MidEQ);
	DefineSimpleUnit(Median);

	DefineSimpleUnit(Resonz);
	DefineSimpleUnit(Ringz);
	DefineSimpleUnit(Formlet);

	DefineSimpleUnit(FOS);
	DefineSimpleUnit(SOS);

	DefineSimpleUnit(Compander);
	DefineDtorUnit(Limiter);
	DefineDtorUnit(Normalizer);

	DefineSimpleUnit(Amplitude);
	DefineSimpleUnit(DetectSilence);

	DefineSimpleUnit(Hilbert);
	DefineSimpleUnit(FreqShift);
	DefineSimpleUnit(MoogFF);

	/* BEQSuite UGens */
	DefineSimpleUnit(BLowPass);
	DefineSimpleUnit(BHiPass);
	DefineSimpleUnit(BBandPass);
	DefineSimpleUnit(BBandStop);
	DefineSimpleUnit(BPeakEQ);
	DefineSimpleUnit(BAllPass);
	DefineSimpleUnit(BLowShelf);
	DefineSimpleUnit(BHiShelf);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
