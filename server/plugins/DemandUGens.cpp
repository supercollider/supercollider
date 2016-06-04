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
#include <cstdio>
#include <cmath>
#include <limits>

using std::floor;
using std::numeric_limits;

static InterfaceTable *ft;

struct Demand : public Unit
{
	float m_prevtrig;
	float m_prevreset;
	float *m_prevout;
	float **m_out;
};

struct Duty : public Unit
{
	float m_count;
	float m_prevreset;
	float m_prevout;
};

struct DemandEnvGen : public Unit
{
	float m_phase;
	float m_prevreset;

	double m_a1, m_a2, m_b1, m_y1, m_y2, m_grow, m_level, m_endLevel, m_curve;
	int m_shape;
	bool m_release, m_running;
};

struct TDuty : public Unit
{
	float m_count;
	float m_prevreset;
};


struct Dseries : public Unit
{
	double m_repeats;
	int32 m_repeatCount;
	double m_value;
	double m_step;
};

struct Dgeom : public Unit
{
	double m_repeats;
	int32 m_repeatCount;
	double m_value;
	double m_grow;
};

struct Dwhite : public Unit
{
	double m_repeats;
	int32 m_repeatCount;
	float m_lo;
	float m_range;
};

struct Dbrown : public Unit
{
	double m_repeats;
	int32 m_repeatCount;
	float m_lo;
	float m_hi;
	float m_step;
	float m_val;
};

struct Diwhite : public Unit
{
	double m_repeats;
	int32 m_repeatCount;
	int32 m_lo;
	int32 m_range;
};

struct Dibrown : public Unit
{
	double m_repeats;
	int32 m_repeatCount;
	int32 m_lo;
	int32 m_hi;
	int32 m_step;
	int32 m_val;
};

struct Dseq : public Unit
{
	double m_repeats;
	int32 m_repeatCount;
	int32 m_index;
	bool m_needToResetChild;
};

struct Dshuf : public Unit
{
	double m_repeats;
	int32 m_repeatCount;
	int32 m_index;
	bool m_needToResetChild;
	int32 *m_indices;
};

struct Dbufrd : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
};

struct Dbufwr : public Unit
{
	float m_fbufnum;
	SndBuf *m_buf;
};

struct Dser : public Dseq
{
};

struct Drand : public Dseq
{
};

struct Dxrand : public Dseq
{
};

struct Dwrand : public Dseq
{
	int32 m_weights_size;
};

struct Dswitch1 : public Unit
{
};

struct Dswitch : public Unit
{
	int m_index;
};

struct Dstutter : public Unit
{
	double m_repeats;
	double m_repeatCount;
	float m_value;
};

struct Donce : public Unit
{
	int m_bufcounter;
	float m_prev;
};

struct Dpoll : public Unit
{
	char *m_id_string;
	bool m_mayprint;
	float m_id;
};

struct Dreset : public Unit
{
	float prev_reset;
};

struct Dconst : public Unit
{
  float m_total;
  float m_runningsum;
  float m_tolerance;
};

extern "C"
{


void Demand_Ctor(Demand *unit);
void Demand_Dtor(Demand *unit);
void Demand_next_aa(Demand *unit, int inNumSamples);
void Demand_next_ak(Demand *unit, int inNumSamples);
void Demand_next_ka(Demand *unit, int inNumSamples);

void Duty_Ctor(Duty *unit);
void Duty_next_da(Duty *unit, int inNumSamples);
void Duty_next_dk(Duty *unit, int inNumSamples);
void Duty_next_dd(Duty *unit, int inNumSamples);

void TDuty_Ctor(TDuty *unit);
void TDuty_next_da(TDuty *unit, int inNumSamples);
void TDuty_next_dk(TDuty *unit, int inNumSamples);
void TDuty_next_dd(TDuty *unit, int inNumSamples);

void DemandEnvGen_Ctor(DemandEnvGen *unit);
void DemandEnvGen_next_k(DemandEnvGen *unit, int inNumSamples);
void DemandEnvGen_next_a(DemandEnvGen *unit, int inNumSamples);

void Dseries_Ctor(Dseries *unit);
void Dseries_next(Dseries *unit, int inNumSamples);

void Dgeom_Ctor(Dgeom *unit);
void Dgeom_next(Dgeom *unit, int inNumSamples);

void Dwhite_Ctor(Dwhite *unit);
void Dwhite_next(Dwhite *unit, int inNumSamples);

void Dbrown_Ctor(Dbrown *unit);
void Dbrown_next(Dbrown *unit, int inNumSamples);

void Diwhite_Ctor(Diwhite *unit);
void Diwhite_next(Diwhite *unit, int inNumSamples);

void Dibrown_Ctor(Dibrown *unit);
void Dibrown_next(Dibrown *unit, int inNumSamples);

void Dseq_Ctor(Dseq *unit);
void Dseq_next(Dseq *unit, int inNumSamples);

void Dshuf_Ctor(Dshuf *unit);
void Dshuf_Dtor(Dshuf *unit);
void Dshuf_next(Dshuf *unit, int inNumSamples);


void Dbufrd_Ctor(Dbufrd *unit);
void Dbufrd_next(Dbufrd *unit, int inNumSamples);

void Dbufwr_Ctor(Dbufwr *unit);
void Dbufwr_next(Dbufwr *unit, int inNumSamples);

void Dser_Ctor(Dser *unit);
void Dser_next(Dser *unit, int inNumSamples);

void Drand_Ctor(Drand *unit);
void Drand_next(Drand *unit, int inNumSamples);

void Dxrand_Ctor(Dxrand *unit);
void Dxrand_next(Dxrand *unit, int inNumSamples);

void Dwrand_Ctor(Dwrand *unit);
void Dwrand_next(Dwrand *unit, int inNumSamples);

void Dswitch1_Ctor(Dswitch1 *unit);
void Dswitch1_next(Dswitch1 *unit, int inNumSamples);

void Dswitch_Ctor(Dswitch *unit);
void Dswitch_next(Dswitch *unit, int inNumSamples);

void Dstutter_Ctor(Dstutter *unit);
void Dstutter_next(Dstutter *unit, int inNumSamples);

void Dpoll_Ctor(Dpoll *unit);
void Dpoll_Ctor(Dpoll *unit);
void Dpoll_next(Dpoll *unit, int inNumSamples);

void Dreset_Ctor(Dreset *unit);
void Dreset_next(Dreset *unit, int inNumSamples);

void Donce_Ctor(Donce *unit);
void Donce_next(Donce *unit, int inNumSamples);

void Dconst_Ctor(Dconst *unit);
void Dconst_next(Dconst *unit, int inNumSamples);

};



void Demand_next_aa(Demand *unit, int inNumSamples)
{
	float *trig = ZIN(0);
	float *reset = ZIN(1);

	float** out = unit->m_out;
	float* prevout = unit->m_prevout;

	for (int i=0; i<unit->mNumOutputs; ++i) {
		out[i] = OUT(i);
	}

	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;

	//Print("Demand_next_aa %d  %g\n", inNumSamples, prevtrig);
	for (int i=0; i<inNumSamples; ++i) {
		float ztrig = ZXP(trig);
		float zreset = ZXP(reset);
		if (zreset > 0.f && prevreset <= 0.f) {
			for (int j=2; j<unit->mNumInputs; ++j) {
				RESETINPUT(j);
			}
		}
		if (ztrig > 0.f && prevtrig <= 0.f) {
			//Print("triggered\n");
			for (int j=2, k=0; j<unit->mNumInputs; ++j, ++k) {
				float x = DEMANDINPUT_A(j, i + 1);
				//printf("in  %d %g\n", k, x);
				if (sc_isnan(x)) { x = prevout[k]; unit->mDone = true; }
				else prevout[k] = x;
				out[k][i] = x;
			}
		} else {
			for (int j=2, k=0; j<unit->mNumInputs; ++j, ++k) {
				out[k][i] = prevout[k];
			}
		}
		prevtrig = ztrig;
		prevreset = zreset;
	}

	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}


void Demand_next_ak(Demand *unit, int inNumSamples)
{
	float *trig = ZIN(0);
	float zreset = IN0(1);

	float** out = unit->m_out;
	float *prevout = unit->m_prevout;

	for (int i=0; i<unit->mNumOutputs; ++i) {
		out[i] = OUT(i);
	}

	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;

	for (int i=0; i<inNumSamples; ++i) {
		float ztrig = ZXP(trig);
		if (zreset > 0.f && prevreset <= 0.f) {
			for (int j=2; j<unit->mNumInputs; ++j) {
				RESETINPUT(j);
			}
		}

		if (ztrig > 0.f && prevtrig <= 0.f) {
			for (int j=2, k=0; j<unit->mNumInputs; ++j, ++k) {
				float x = DEMANDINPUT_A(j, i + 1);
				if (sc_isnan(x)) { x = prevout[k]; unit->mDone = true; }
				else prevout[k] = x;
				out[k][i] = x;
			}

		} else {
			for (int j=2, k=0; j<unit->mNumInputs; ++j, ++k) {
				out[k][i] = prevout[k];
			}

		}
		prevtrig = ztrig;
		prevreset = zreset;
	}

	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}


void Demand_next_ka(Demand *unit, int inNumSamples)
{
	float ztrig = IN0(0);
	float *reset = ZIN(1);

	float** out = unit->m_out;
	float *prevout = unit->m_prevout;

	for (int i=0; i<unit->mNumOutputs; ++i) {
		out[i] = OUT(i);
	}

	float prevtrig = unit->m_prevtrig;
	float prevreset = unit->m_prevreset;

	for (int i=0; i<inNumSamples; ++i) {
		float zreset = ZXP(reset);
		if (zreset > 0.f && prevreset <= 0.f) {
			for (int j=2; j<unit->mNumInputs; ++j) {
				RESETINPUT(j);
			}
		}
		if (ztrig > 0.f && prevtrig <= 0.f) {
			for (int j=2, k=0; j<unit->mNumInputs; ++j, ++k) {
				float x = DEMANDINPUT_A(j, i + 1);
				if (sc_isnan(x)) { x = prevout[k]; unit->mDone = true; }
				else prevout[k] = x;
				out[k][i] = x;
			}
		}
		prevtrig = ztrig;
		prevreset = zreset;
	}

	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
}



void Demand_Ctor(Demand *unit)
{
	//Print("Demand_Ctor\n");
	if (INRATE(0) == calc_FullRate) {
		if (INRATE(1) == calc_FullRate) {
			SETCALC(Demand_next_aa);
		} else {
			SETCALC(Demand_next_ak);
		}
	} else {
		if (INRATE(1) == calc_FullRate) {
			SETCALC(Demand_next_ka);
		} else {
			SETCALC(Demand_next_aa);
		}
	}

	for (int i=0; i<unit->mNumOutputs; ++i)
		OUT0(i) = 0.f;

	char * memoryChunk = (char*)RTAlloc(unit->mWorld, unit->mNumOutputs * (sizeof(float) + sizeof(float*)));

	if (!memoryChunk) {
		Print("Demand: RT memory allocation failed\n");
		SETCALC(ClearUnitOutputs);
		return;
	}

	unit->m_prevout = (float*)memoryChunk;
	unit->m_out = (float**)(memoryChunk + unit->mNumOutputs * sizeof(float));

	unit->m_prevtrig = 0.f;
	unit->m_prevreset = 0.f;
	std::fill_n(unit->m_prevout, unit->mNumOutputs, 0.f);
}

void Demand_Dtor(Demand* unit)
{
	if(unit->m_prevout) RTFree(unit->mWorld, unit->m_prevout);
}


/////////////////////////////////////////////////////////////////////////////

enum {
	duty_dur,
	duty_reset,
	duty_doneAction,
	duty_level
};

void Duty_next_da(Duty *unit, int inNumSamples)
{

	float *reset = ZIN(duty_reset);

	float *out = OUT(0);
	float prevout = unit->m_prevout;
	float count = unit->m_count;
	float prevreset = unit->m_prevreset;
	float sr = (float) SAMPLERATE;

	for (int i=0; i<inNumSamples; ++i) {

		float zreset = ZXP(reset);
		if (zreset > 0.f && prevreset <= 0.f) {

			RESETINPUT(duty_level);
			RESETINPUT(duty_dur);
			count = 0.f;
		}
		if (count <= 0.f) {
			count = DEMANDINPUT_A(duty_dur, i + 1) * sr + count;
			if(sc_isnan(count)) {
				int doneAction = (int)ZIN0(duty_doneAction);
				DoneAction(doneAction, unit);
			}
			float x = DEMANDINPUT_A(duty_level, i + 1);
			//printf("in  %d\n", count);
			if(sc_isnan(x)) {
				x = prevout;
				int doneAction = (int)ZIN0(duty_doneAction);
				DoneAction(doneAction, unit);
			} else {
				prevout = x;
			}
			out[i] = x;

		} else {
			out[i] = prevout;
		}
		count--;
		prevreset = zreset;
	}

	unit->m_count = count;
	unit->m_prevreset = prevreset;
	unit->m_prevout = prevout;
}

void Duty_next_dk(Duty *unit, int inNumSamples)
{
	float zreset = ZIN0(duty_reset);

	float *out = OUT(0);
	float prevout = unit->m_prevout;
	float count = unit->m_count;
	float prevreset = unit->m_prevreset;
	float sr = (float) SAMPLERATE;

	for (int i=0; i<inNumSamples; ++i) {

		if (zreset > 0.f && prevreset <= 0.f) {

			RESETINPUT(duty_level);
			RESETINPUT(duty_dur);
			count = 0.f;
		}
		if (count <= 0.f) {
			count = DEMANDINPUT_A(duty_dur, i + 1) * sr + count;
			if(sc_isnan(count)) {
				int doneAction = (int)ZIN0(duty_doneAction);
				DoneAction(doneAction, unit);
			}

			float x = DEMANDINPUT_A(duty_level, i + 1);
			if(sc_isnan(x)) {
				x = prevout;
				int doneAction = (int)ZIN0(duty_doneAction);
				DoneAction(doneAction, unit);
			} else {
				prevout = x;
			}

			out[i] = x;

		} else {

			out[i] = prevout;
		}
		count--;
		prevreset = zreset;
	}

	unit->m_count = count;
	unit->m_prevreset = prevreset;
	unit->m_prevout = prevout;

}


void Duty_next_dd(Duty *unit, int inNumSamples)
{
	float *out = OUT(0);
	float prevout = unit->m_prevout;
	float count = unit->m_count;
	float reset = unit->m_prevreset;
	float sr = (float) SAMPLERATE;

	for (int i=0; i<inNumSamples; ++i) {

		if (reset <= 0.f) {
			RESETINPUT(duty_level);
			RESETINPUT(duty_dur);
			count = 0.f;
			reset = DEMANDINPUT_A(duty_reset, i + 1) * sr + reset;
		} else {
			reset--;
		}
		if (count <= 0.f) {
			count = DEMANDINPUT_A(duty_dur, i + 1) * sr + count;
			if(sc_isnan(count)) {
				int doneAction = (int)ZIN0(duty_doneAction);
				DoneAction(doneAction, unit);
			}
			float x = DEMANDINPUT_A(duty_level, i + 1);
			//printf("in  %d %g\n", k, x);
			if(sc_isnan(x)) {
				x = prevout;
				int doneAction = (int)ZIN0(duty_doneAction);
				DoneAction(doneAction, unit);
			} else {
				prevout = x;
			}
		}

		out[i] = prevout;
		count--;
	}

	unit->m_count = count;
	unit->m_prevreset = reset;
	unit->m_prevout = prevout;
}


void Duty_Ctor(Duty *unit)
{
	if (INRATE(duty_reset) == calc_FullRate) {

			SETCALC(Duty_next_da);
			unit->m_prevreset = 0.f;

	} else {
		if(INRATE(duty_reset) == calc_DemandRate) {
			SETCALC(Duty_next_dd);
			unit->m_prevreset = DEMANDINPUT(duty_reset) * SAMPLERATE;
		} else {
			SETCALC(Duty_next_dk);
			unit->m_prevreset = 0.f;
		}
	}

	unit->m_count = DEMANDINPUT(duty_dur) * SAMPLERATE - 1;
	unit->m_prevout = DEMANDINPUT(duty_level);
	OUT0(0) = unit->m_prevout;

}


/////////////////////////////////////////////////////////////////////////////

enum {
	d_env_level,
	d_env_dur,
	d_env_shape,
	d_env_curve,
	d_env_gate,
	d_env_reset,
	d_env_levelScale,
	d_env_levelBias,
	d_env_timeScale,
	d_env_doneAction

};

enum {
	shape_Step,
	shape_Linear,
	shape_Exponential,
	shape_Sine,
	shape_Welch,
	shape_Curve,
	shape_Squared,
	shape_Cubed,
	shape_Sustain = 9999
};



void DemandEnvGen_next_k(DemandEnvGen *unit, int inNumSamples)
{
	float zreset = ZIN0(d_env_reset);

	float *out = ZOUT(0);
	double level = unit->m_level;
	float phase = unit->m_phase;
	double curve = unit->m_curve;
	bool release = unit->m_release;
	bool running = unit->m_running;
	int shape = unit->m_shape;

	// printf("phase %f level %f \n", phase, level);

	for (int i=0; i<inNumSamples; ++i) {

		if (zreset > 0.f && unit->m_prevreset <= 0.f) {
			//printf("reset: %f %f \n", zreset, unit->m_prevreset);
			RESETINPUT(d_env_level);
			RESETINPUT(d_env_dur);
			RESETINPUT(d_env_shape);
			RESETINPUT(d_env_curve);

			if(zreset <= 1.f) {
				DEMANDINPUT(d_env_level); // remove first level
			} else {
				level = DEMANDINPUT(d_env_level); // jump to first level
			}

			release = false;
			running = true;
			phase = 0.f;

		}


		if (phase <= 0.f && running) {

			// was a release during last segment?
			if(release) {

				running = false;
				release = false;
				// printf("release: %f %f \n", phase, level);
				int doneAction = (int)ZIN0(d_env_doneAction);
				DoneAction(doneAction, unit);

			} else {
				// new time

				float dur = DEMANDINPUT(d_env_dur);
				// printf("dur: %f \n", dur);
				if(sc_isnan(dur)) {
					release = true;
					running = false;
					phase = numeric_limits<float>::max();
				} else {
					phase = dur * ZIN0(d_env_timeScale) * SAMPLERATE + phase;
				}


				// new shape
				float fshape = DEMANDINPUT(d_env_shape);
				if (sc_isnan(fshape))
					shape = unit->m_shape;
				else
					shape = (int)fshape;

				curve = DEMANDINPUT(d_env_curve);
				if (sc_isnan(curve)) curve = unit->m_curve;

				float count;
				if (phase <= 1.f) {
					shape = 1; // shape_Linear
					count = 1.f;
				} else {
					count = phase;
				}
				if(dur * 0.5f < SAMPLEDUR) shape = 1;

				//printf("shape: %i, curve: %f, dur: %f \n", shape, curve, dur);


				// new end level

				double endLevel = DEMANDINPUT(d_env_level);
				// printf("levels: %f %f\n", level, endLevel);
				if (sc_isnan(endLevel)) {
					endLevel = unit->m_endLevel;
					release = true;
					phase = 0.f;
					shape = 0;
				} else  {
					endLevel = endLevel * ZIN0(d_env_levelScale) + ZIN0(d_env_levelBias);
					unit->m_endLevel = endLevel;
				}


				// calculate shape parameters

				switch (shape) {
					case shape_Step : {
						level = endLevel;
					} break;
					case shape_Linear : {
						unit->m_grow = (endLevel - level) / count;
					} break;
					case shape_Exponential : {
						unit->m_grow = pow(endLevel / level, 1.0 / count);
					} break;
					case shape_Sine : {
						double w = pi / count;

						unit->m_a2 = (endLevel + level) * 0.5;
						unit->m_b1 = 2. * cos(w);
						unit->m_y1 = (endLevel - level) * 0.5;
						unit->m_y2 = unit->m_y1 * sin(pi * 0.5 - w);
						level = unit->m_a2 - unit->m_y1;
					} break;
					case shape_Welch : {
						double w = (pi * 0.5) / count;

						unit->m_b1 = 2. * cos(w);

						if (endLevel >= level) {
							unit->m_a2 = level;
							unit->m_y1 = 0.;
							unit->m_y2 = -sin(w) * (endLevel - level);
						} else {
							unit->m_a2 = endLevel;
							unit->m_y1 = level - endLevel;
							unit->m_y2 = cos(w) * (level - endLevel);
						}
						level = unit->m_a2 + unit->m_y1;
					} break;
					case shape_Curve : {
						if (fabs(curve) < 0.001) {
							unit->m_shape = shape = 1; // shape_Linear
							unit->m_grow = (endLevel - level) / count;
						} else {
							double a1 = (endLevel - level) / (1.0 - exp(curve));
							unit->m_a2 = level + a1;
							unit->m_b1 = a1;
							unit->m_grow = exp(curve / ceil( count) );
						}
					} break;
					case shape_Squared : {
						unit->m_y1 = sqrt(level);
						unit->m_y2 = sqrt(endLevel);
						unit->m_grow = (unit->m_y2 - unit->m_y1) / count;
					} break;
					case shape_Cubed : {
						unit->m_y1 = pow(level, 0.33333333);
						unit->m_y2 = pow(endLevel, 0.33333333);
						unit->m_grow = (unit->m_y2 - unit->m_y1) / count;
					} break;
				}

			}
			}


			if(running) {

			switch (shape) {
				case shape_Step : {
				} break;
				case shape_Linear : {
					double grow = unit->m_grow;
							//Print("level %g\n", level);
						level += grow;
				} break;
				case shape_Exponential : {
					double grow = unit->m_grow;
						level *= grow;
				} break;
				case shape_Sine : {
					double a2 = unit->m_a2;
					double b1 = unit->m_b1;
					double y2 = unit->m_y2;
					double y1 = unit->m_y1;
						double y0 = b1 * y1 - y2;
						level = a2 - y0;
						y2 = y1;
						y1 = y0;
					unit->m_y1 = y1;
					unit->m_y2 = y2;
				} break;
				case shape_Welch : {
					double a2 = unit->m_a2;
					double b1 = unit->m_b1;
					double y2 = unit->m_y2;
					double y1 = unit->m_y1;
						double y0 = b1 * y1 - y2;
						level = a2 + y0;
						y2 = y1;
						y1 = y0;
					unit->m_y1 = y1;
					unit->m_y2 = y2;
				} break;
				case shape_Curve : {
					double a2 = unit->m_a2;
					double b1 = unit->m_b1;
					double grow = unit->m_grow;
						b1 *= grow;
						level = a2 - b1;
					unit->m_b1 = b1;
				} break;
				case shape_Squared : {
					double grow = unit->m_grow;
					double y1 = unit->m_y1;
						y1 += grow;
						level = y1*y1;
					unit->m_y1 = y1;
				} break;
				case shape_Cubed : {
					double grow = unit->m_grow;
					double y1 = unit->m_y1;
						y1 += grow;
						level = y1*y1*y1;
					unit->m_y1 = y1;
				} break;
				case shape_Sustain : {
				} break;
			}

			phase --;

			}


			ZXP(out) = level;

	}
	float zgate = ZIN0(d_env_gate);
	if(zgate >= 1.f) {
		unit->m_running = true;
	} else if (zgate > 0.f) {
		unit->m_running = true;
		release = true;  // release next time.
	} else {
		unit->m_running = false; // sample and hold
	}

	unit->m_level = level;
	unit->m_curve = curve;
	unit->m_shape = shape;
	unit->m_prevreset = zreset;
	unit->m_release = release;

	unit->m_phase = phase;
}



void DemandEnvGen_next_a(DemandEnvGen *unit, int inNumSamples)
{

	float *reset = ZIN(d_env_reset);
	float *gate = ZIN(d_env_gate);

	float *out = ZOUT(0);

	float prevreset = unit->m_prevreset;
	double level = unit->m_level;
	float phase = unit->m_phase;
	double curve = unit->m_curve;
	bool release = unit->m_release;
	bool running = unit->m_running;


	int shape = unit->m_shape;
	// printf("phase %f \n", phase);

	for (int i=0; i<inNumSamples; ++i) {

		float zreset = ZXP(reset);
		if (zreset > 0.f && prevreset <= 0.f) {
			// printf("reset: %f %f \n", zreset, unit->m_prevreset);
			RESETINPUT(d_env_level);
			if(zreset <= 1.f) {
				DEMANDINPUT_A(d_env_level, i + 1); // remove first level
			} else {
				level = DEMANDINPUT_A(d_env_level, i + 1); // jump to first level
			}

			RESETINPUT(d_env_dur);
			RESETINPUT(d_env_shape);
			release = false;
			running = true;

			phase = 0.f;
		}

		prevreset = zreset;


		if (phase <= 0.f && running) {

			// was a release?
			if(release) {

				running = false;
				release = false;
				// printf("release: %f %f \n", phase, level);
				int doneAction = (int)ZIN0(d_env_doneAction);
				DoneAction(doneAction, unit);

			} else {
				// new time

				float dur = DEMANDINPUT_A(d_env_dur, i + 1);
				// printf("dur: %f \n", dur);
				if(sc_isnan(dur)) {
					release = true;
					running = false;
					phase = numeric_limits<float>::max();
				} else {
					phase = dur * ZIN0(d_env_timeScale) * SAMPLERATE + phase;
				}

				// new shape
				float count;
				curve = DEMANDINPUT_A(d_env_curve, i + 1);

				// printf("shapes: %i \n", shape);
				if (sc_isnan(curve)) curve = unit->m_curve;

				float fshape = DEMANDINPUT_A(d_env_shape, i + 1);
				if (sc_isnan(fshape))
					shape = unit->m_shape;
				else
					shape = (int)fshape;

				if (phase <= 1.f) {
					shape = 1; // shape_Linear
					count = 1.f;
				} else {
					count = phase;
				}
				if(dur * 0.5f < SAMPLEDUR) shape = 1;


				// new end level

				double endLevel = DEMANDINPUT_A(d_env_level, i + 1);
				// printf("levels: %f %f\n", level, endLevel);
				if (sc_isnan(endLevel)) {
					endLevel = unit->m_endLevel;
					release = true;
					phase = 0.f;
					shape = 0;
				} else  {
					endLevel = endLevel * ZIN0(d_env_levelScale) + ZIN0(d_env_levelBias);
					unit->m_endLevel = endLevel;
				}


				// calculate shape parameters

				switch (shape) {
					case shape_Step : {
						level = endLevel;
					} break;
					case shape_Linear : {
						unit->m_grow = (endLevel - level) / count;
					} break;
					case shape_Exponential : {
						unit->m_grow = pow(endLevel / level, 1.0 / count);
					} break;
					case shape_Sine : {
						double w = pi / count;

						unit->m_a2 = (endLevel + level) * 0.5;
						unit->m_b1 = 2. * cos(w);
						unit->m_y1 = (endLevel - level) * 0.5;
						unit->m_y2 = unit->m_y1 * sin(pi * 0.5 - w);
						level = unit->m_a2 - unit->m_y1;
					} break;
					case shape_Welch : {
						double w = (pi * 0.5) / count;

						unit->m_b1 = 2. * cos(w);

						if (endLevel >= level) {
							unit->m_a2 = level;
							unit->m_y1 = 0.;
							unit->m_y2 = -sin(w) * (endLevel - level);
						} else {
							unit->m_a2 = endLevel;
							unit->m_y1 = level - endLevel;
							unit->m_y2 = cos(w) * (level - endLevel);
						}
						level = unit->m_a2 + unit->m_y1;
					} break;
					case shape_Curve : {
						if (fabs(curve) < 0.001) {
							unit->m_shape = shape = 1; // shape_Linear
							unit->m_grow = (endLevel - level) / count;
						} else {
							double a1 = (endLevel - level) / (1.0 - exp(curve));
							unit->m_a2 = level + a1;
							unit->m_b1 = a1;
							unit->m_grow = exp(curve / ceil( count ) );
						}
					} break;
					case shape_Squared : {
						unit->m_y1 = sqrt(level);
						unit->m_y2 = sqrt(endLevel);
						unit->m_grow = (unit->m_y2 - unit->m_y1) / count;
					} break;
					case shape_Cubed : {
						unit->m_y1 = pow(level, 0.33333333);
						unit->m_y2 = pow(endLevel, 0.33333333);
						unit->m_grow = (unit->m_y2 - unit->m_y1) / count;
					} break;
				}

			}
		}


		if(running) {
			switch (shape) {
				case shape_Step : {
				} break;
				case shape_Linear : {
					double grow = unit->m_grow;
							//Print("level %g\n", level);
						level += grow;
				} break;
				case shape_Exponential : {
					double grow = unit->m_grow;
						level *= grow;
				} break;
				case shape_Sine : {
					double a2 = unit->m_a2;
					double b1 = unit->m_b1;
					double y2 = unit->m_y2;
					double y1 = unit->m_y1;
						double y0 = b1 * y1 - y2;
						level = a2 - y0;
						y2 = y1;
						y1 = y0;
					unit->m_y1 = y1;
					unit->m_y2 = y2;
				} break;
				case shape_Welch : {
					double a2 = unit->m_a2;
					double b1 = unit->m_b1;
					double y2 = unit->m_y2;
					double y1 = unit->m_y1;
						double y0 = b1 * y1 - y2;
						level = a2 + y0;
						y2 = y1;
						y1 = y0;
					unit->m_y1 = y1;
					unit->m_y2 = y2;
				} break;
				case shape_Curve : {
					double a2 = unit->m_a2;
					double b1 = unit->m_b1;
					double grow = unit->m_grow;
						b1 *= grow;
						level = a2 - b1;
					unit->m_b1 = b1;
				} break;
				case shape_Squared : {
					double grow = unit->m_grow;
					double y1 = unit->m_y1;
						y1 += grow;
						level = y1*y1;
					unit->m_y1 = y1;
				} break;
				case shape_Cubed : {
					double grow = unit->m_grow;
					double y1 = unit->m_y1;
						y1 += grow;
						level = y1*y1*y1;
					unit->m_y1 = y1;
				} break;
				case shape_Sustain : {
				} break;
			}

			phase--;

		}

		ZXP(out) = level;
		float zgate = ZXP(gate);

		if(zgate >= 1.f) {
			unit->m_running = true;
		} else if (zgate > 0.f) {
			unit->m_running = true;
			release = true;  // release next time.
		} else {
			unit->m_running = false; // sample and hold
		}
	}

	unit->m_level = level;
	unit->m_curve = curve;
	unit->m_shape = shape;
	unit->m_prevreset = prevreset;
	unit->m_release = release;
	unit->m_phase = phase;
}


void DemandEnvGen_Ctor(DemandEnvGen *unit)
{
	// derive the first level.

	unit->m_level = DEMANDINPUT(d_env_level);
	if(sc_isnan(unit->m_level)) { unit->m_level = 0.f; }
	unit->m_endLevel = unit->m_level;
	unit->m_release = false;
	unit->m_prevreset = 0.f;
	unit->m_phase = 0.f;
	unit->m_running = ZIN0(d_env_gate) > 0.f;

	if(INRATE(d_env_gate) == calc_FullRate) {
			SETCALC(DemandEnvGen_next_a);
	} else {
			SETCALC(DemandEnvGen_next_k);
	}

	DemandEnvGen_next_k(unit, 1);

}


/////////////////////////////////////////////////////////////////////////////

void TDuty_next_da(TDuty *unit, int inNumSamples)
{

	float *reset = ZIN(duty_reset);
	float *out = OUT(0);

	float count = unit->m_count;
	float prevreset = unit->m_prevreset;
	float sr = (float) SAMPLERATE;

	for (int i=0; i<inNumSamples; ++i) {

		float zreset = ZXP(reset);
		if (zreset > 0.f && prevreset <= 0.f) {

			RESETINPUT(duty_level);
			RESETINPUT(duty_dur);
			count = 0.f;
		}
		if (count <= 0.f) {
			count = DEMANDINPUT_A(duty_dur, i + 1) * sr + count;
			if(sc_isnan(count)) {
				int doneAction = (int)ZIN0(2);
				DoneAction(doneAction, unit);
			}
			float x = DEMANDINPUT_A(duty_level, i + 1);
			//printf("in  %d %g\n", k, x);
			if (sc_isnan(x)) x = 0.f;
			out[i] = x;
		} else {
			out[i] = 0.f;
		}
		count--;
		prevreset = zreset;
	}

	unit->m_count = count;
	unit->m_prevreset = prevreset;

}

void TDuty_next_dk(TDuty *unit, int inNumSamples)
{

	float zreset = ZIN0(duty_reset);

	float *out = OUT(0);
	float count = unit->m_count;
	float prevreset = unit->m_prevreset;
	float sr = (float) SAMPLERATE;

	for (int i=0; i<inNumSamples; ++i) {

		if (zreset > 0.f && prevreset <= 0.f) {

			RESETINPUT(duty_level);
			RESETINPUT(duty_dur);
			count = 0.f;
		}
		if (count <= 0.f) {
			count = DEMANDINPUT_A(duty_dur, i + 1) * sr + count;
			if(sc_isnan(count)) {
				int doneAction = (int)ZIN0(2);
				DoneAction(doneAction, unit);
			}
			float x = DEMANDINPUT_A(duty_level, i + 1);
			//printf("in  %d %g\n", k, x);
			if (sc_isnan(x)) x = 0.f;
			out[i] = x;
		} else {
			out[i] = 0.f;
		}
		count--;
		prevreset = zreset;
	}

	unit->m_count = count;
	unit->m_prevreset = prevreset;
}


void TDuty_next_dd(TDuty *unit, int inNumSamples)
{

	float *out = OUT(0);
	float count = unit->m_count;
	float reset = unit->m_prevreset;
	float sr = (float) SAMPLERATE;

	for (int i=0; i<inNumSamples; ++i) {

		if (reset <= 0.f) {
			RESETINPUT(duty_level);
			RESETINPUT(duty_dur);
			count = 0.f;
			reset = DEMANDINPUT_A(duty_reset, i + 1) * sr + reset;
		} else {
			reset--;
		}
		if (count <= 0.f) {
			count = DEMANDINPUT_A(duty_dur, i + 1) * sr + count;
			if(sc_isnan(count)) {
				int doneAction = (int)ZIN0(2);
				DoneAction(doneAction, unit);
			}
			float x = DEMANDINPUT_A(duty_level, i + 1);
			//printf("in  %d %g\n", k, x);
			if (sc_isnan(x)) x = 0.f;
			out[i] = x;
		} else {

			out[i] = 0.f;
		}
		count--;
	}

	unit->m_count = count;
	unit->m_prevreset = reset;

}


void TDuty_Ctor(TDuty *unit)
{
	if (INRATE(1) == calc_FullRate) {

			SETCALC(TDuty_next_da);
			unit->m_prevreset = 0.f;

	} else {
		if(INRATE(1) == calc_DemandRate) {
			SETCALC(TDuty_next_dd);
			unit->m_prevreset = DEMANDINPUT(1) * SAMPLERATE;
		} else {
			SETCALC(TDuty_next_dk);
			unit->m_prevreset = 0.f;
		}
	}
	// support for gap-first.
	if(IN0(4)) {
		unit->m_count = DEMANDINPUT(duty_dur) * SAMPLERATE;
	} else {
		unit->m_count = 0.f;
	}
	OUT0(0) = 0.f;
}


/////////////////////////////////////////////////////////////////////////////



void Dseries_next(Dseries *unit, int inNumSamples)
{
	if (inNumSamples) {
		float step = DEMANDINPUT_A(2, inNumSamples);
		if(!sc_isnan(step)) { unit->m_step = step; }
		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
			unit->m_value = DEMANDINPUT_A(1, inNumSamples);
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		OUT0(0) = unit->m_value;
		unit->m_value += unit->m_step;
		unit->m_repeatCount++;
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
	}
}

void Dseries_Ctor(Dseries *unit)
{
	SETCALC(Dseries_next);
	Dseries_next(unit, 0);
	OUT0(0) = 0.f;
}


void Dgeom_next(Dgeom *unit, int inNumSamples)
{
	if (inNumSamples) {
		float grow = DEMANDINPUT_A(2, inNumSamples);
		if(!sc_isnan(grow)) { unit->m_grow = grow; }
		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
			unit->m_value = DEMANDINPUT_A(1, inNumSamples);
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		OUT0(0) = unit->m_value;
		unit->m_value *= unit->m_grow;
		unit->m_repeatCount++;
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
	}
}

void Dgeom_Ctor(Dgeom *unit)
{
	SETCALC(Dgeom_next);
	Dgeom_next(unit, 0);
	OUT0(0) = 0.f;
}


void Dwhite_next(Dwhite *unit, int inNumSamples)
{
	if (inNumSamples) {
		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		unit->m_repeatCount++;
		float lo = DEMANDINPUT_A(1, inNumSamples);
		float hi = DEMANDINPUT_A(2, inNumSamples);

		if(!sc_isnan(lo)) { unit->m_lo = lo;}
		if(!sc_isnan(hi)) { unit->m_range = hi - lo; }
		float x = unit->mParent->mRGen->frand() * unit->m_range + unit->m_lo;
		OUT0(0) = x;
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
	}
}

void Dwhite_Ctor(Dwhite *unit)
{
	SETCALC(Dwhite_next);
	Dwhite_next(unit, 0);
	OUT0(0) = 0.f;
}


void Diwhite_next(Diwhite *unit, int inNumSamples)
{
	if (inNumSamples) {
		float lo = DEMANDINPUT_A(1, inNumSamples);
		float hi = DEMANDINPUT_A(2, inNumSamples);

		if(!sc_isnan(lo)) { unit->m_lo = (int32)floor(DEMANDINPUT_A(1, inNumSamples) + 0.5f); }
		if(!sc_isnan(hi)) {
			int32 hi = (int32)floor(DEMANDINPUT_A(2, inNumSamples) + 0.5f);
			unit->m_range = hi - unit->m_lo + 1;
		}

		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		unit->m_repeatCount++;
		float x = unit->mParent->mRGen->irand(unit->m_range) + unit->m_lo;
		OUT0(0) = x;
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
	}
}

void Diwhite_Ctor(Diwhite *unit)
{
	SETCALC(Diwhite_next);
	Diwhite_next(unit, 0);
	OUT0(0) = 0.f;
}


void Dbrown_next(Dbrown *unit, int inNumSamples)
{
	if (inNumSamples) {
			float lo = DEMANDINPUT_A(1, inNumSamples); if(!sc_isnan(lo)) { unit->m_lo = lo; }
			float hi = DEMANDINPUT_A(2, inNumSamples); if(!sc_isnan(hi)) { unit->m_hi = hi; }
			float step = DEMANDINPUT_A(3, inNumSamples); if(!sc_isnan(step)) { unit->m_step = step; }

		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
			unit->m_val = unit->mParent->mRGen->frand() * (unit->m_hi - unit->m_lo) + unit->m_lo;
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		unit->m_repeatCount++;
		OUT0(0) = unit->m_val;
		float x = unit->m_val + unit->mParent->mRGen->frand2() * unit->m_step;
		unit->m_val = sc_fold(x, unit->m_lo, unit->m_hi);
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
	}
}

void Dbrown_Ctor(Dbrown *unit)
{
	SETCALC(Dbrown_next);
	Dbrown_next(unit, 0);
	OUT0(0) = 0.f;
}


void Dibrown_next(Dibrown *unit, int inNumSamples)
{
	if (inNumSamples) {

		float lo   = DEMANDINPUT_A(1, inNumSamples);    if(!sc_isnan(lo))   { unit->m_lo   = (int32)lo; }
		float hi   = DEMANDINPUT_A(2, inNumSamples);    if(!sc_isnan(hi))   { unit->m_hi   = (int32)hi; }
		float step = DEMANDINPUT_A(3, inNumSamples);    if(!sc_isnan(step)) { unit->m_step = (int32)step; }

		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
			unit->m_val = unit->mParent->mRGen->irand(unit->m_hi - unit->m_lo + 1) + unit->m_lo;
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		OUT0(0) = unit->m_val;
		int32 z = unit->m_val + unit->mParent->mRGen->irand2(unit->m_step);
		unit->m_val = sc_fold(z, unit->m_lo, unit->m_hi);
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
	}
}

void Dibrown_Ctor(Dibrown *unit)
{
	SETCALC(Dibrown_next);
	Dibrown_next(unit, 0);
	OUT0(0) = 0.f;
}



void Dseq_next(Dseq *unit, int inNumSamples)
{
	//Print("->Dseq_next %d\n", inNumSamples);
	if (inNumSamples) {
		//Print("   unit->m_repeats %d\n", unit->m_repeats);
		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
		}
		int attempts = 0;
		while (true) {
			//Print("   unit->m_index %d   unit->m_repeatCount %d\n", unit->m_index, unit->m_repeatCount);
			if (unit->m_index >= unit->mNumInputs) {
				unit->m_index = 1;
				unit->m_repeatCount++;
			}
			if (unit->m_repeatCount >= unit->m_repeats) {
				//Print("done\n");
				OUT0(0) = NAN;
				unit->m_index = 1;
				return;
			}
			if (ISDEMANDINPUT(unit->m_index)) {
				if (unit->m_needToResetChild) {
					unit->m_needToResetChild = false;
					RESETINPUT(unit->m_index);
				}
				float x = DEMANDINPUT_A(unit->m_index, inNumSamples);
				if (sc_isnan(x)) {
					unit->m_index++;
					unit->m_needToResetChild = true;
				} else {
					OUT0(0) = x;
					return;
				}
			} else {
				OUT0(0) = DEMANDINPUT_A(unit->m_index, inNumSamples);
				//Print("   unit->m_index %d   OUT0(0) %g\n", unit->m_index, OUT0(0));
				unit->m_index++;
				unit->m_needToResetChild = true;
				return;
			}

			if (attempts++ > unit->mNumInputs) {
				Print("Warning: empty sequence in Dseq\n");
				return;
			}
		}
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
		unit->m_needToResetChild = true;
		unit->m_index = 1;
	}
}


void Dseq_Ctor(Dseq *unit)
{
	SETCALC(Dseq_next);
	Dseq_next(unit, 0);
	OUT0(0) = 0.f;
}


void Dser_next(Dser *unit, int inNumSamples)
{
	if (inNumSamples) {
		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
		}
		while (true) {
			if (unit->m_index >= unit->mNumInputs) {
				unit->m_index = 1;
			}
			if (unit->m_repeatCount >= unit->m_repeats) {
				OUT0(0) = NAN;
				return;
			}
			if (ISDEMANDINPUT(unit->m_index)) {
				if (unit->m_needToResetChild) {
					unit->m_needToResetChild = false;
					RESETINPUT(unit->m_index);
				}
				float x = DEMANDINPUT_A(unit->m_index, inNumSamples);
				if (sc_isnan(x)) {
					unit->m_index++;
					unit->m_repeatCount++;
					unit->m_needToResetChild = true;
				} else {
					OUT0(0) = x;
					return;
				}
			} else {
				OUT0(0) = IN0(unit->m_index);
				unit->m_index++;
				unit->m_repeatCount++;
				unit->m_needToResetChild = true;
				return;
			}
		}
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
		unit->m_needToResetChild = true;
		unit->m_index = 1;
	}
}

void Dser_Ctor(Dser *unit)
{
	SETCALC(Dser_next);
	Dser_next(unit, 0);
	OUT0(0) = 0.f;
}



void Drand_next(Drand *unit, int inNumSamples)
{
	if (inNumSamples) {

		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
		}
		while (true) {
			if (unit->m_repeatCount >= unit->m_repeats) {
				OUT0(0) = NAN;
				return;
			}
			if (ISDEMANDINPUT(unit->m_index)) {
				if (unit->m_needToResetChild) {
					unit->m_needToResetChild = false;
					RESETINPUT(unit->m_index);
				}
				float x = DEMANDINPUT_A(unit->m_index, inNumSamples);
				if (sc_isnan(x)) {
					unit->m_index = unit->mParent->mRGen->irand(unit->mNumInputs - 1) + 1;
					unit->m_repeatCount++;
					unit->m_needToResetChild = true;
				} else {
					OUT0(0) = x;
					return;
				}
			} else {
				OUT0(0) = DEMANDINPUT_A(unit->m_index, inNumSamples);
				unit->m_index = unit->mParent->mRGen->irand(unit->mNumInputs - 1) + 1;
				unit->m_repeatCount++;
				unit->m_needToResetChild = true;
				return;
			}
		}
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
		unit->m_needToResetChild = true;
		unit->m_index = unit->mParent->mRGen->irand(unit->mNumInputs - 1) + 1;
	}
}

void Drand_Ctor(Drand *unit)
{
	SETCALC(Drand_next);
	Drand_next(unit, 0);
	OUT0(0) = 0.f;
}



void Dxrand_next(Dxrand *unit, int inNumSamples)
{
	if (inNumSamples) {
		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
		}
		while (true) {
			if (unit->m_index >= unit->mNumInputs) {
				unit->m_index = 1;
			}
			if (unit->m_repeatCount >= unit->m_repeats) {
				OUT0(0) = NAN;
				return;
			}
			if (ISDEMANDINPUT(unit->m_index)) {
				if (unit->m_needToResetChild) {
					unit->m_needToResetChild = false;
					RESETINPUT(unit->m_index);
				}
				float x = DEMANDINPUT_A(unit->m_index, inNumSamples);
				if (sc_isnan(x)) {
					int newindex = unit->mParent->mRGen->irand(unit->mNumInputs - 2) + 1;
					unit->m_index = newindex < unit->m_index ? newindex : newindex + 1;
					unit->m_repeatCount++;
					unit->m_needToResetChild = true;
				} else {
					OUT0(0) = x;
					return;
				}
			} else {
				OUT0(0) = DEMANDINPUT_A(unit->m_index, inNumSamples);
				int newindex = unit->mParent->mRGen->irand(unit->mNumInputs - 2) + 1;
				unit->m_index = newindex < unit->m_index ? newindex : newindex + 1;
				unit->m_repeatCount++;
				unit->m_needToResetChild = true;
				return;
			}
		}
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
		unit->m_needToResetChild = true;
		int newindex = unit->mParent->mRGen->irand(unit->mNumInputs - 2) + 1;
		unit->m_index = newindex < unit->m_index ? newindex : newindex + 1;
	}
}

void Dxrand_Ctor(Dxrand *unit)
{
	SETCALC(Dxrand_next);
	Dxrand_next(unit, 0);
	OUT0(0) = 0.f;
}

#define WINDEX \
float w, sum = 0.0; \
float r = unit->mParent->mRGen->frand(); \
for (int i=0; i<weights_size; ++i) { \
	w = IN0(2 + i); \
	sum += w; \
	if (sum >= r) { \
		unit->m_index = i + offset; \
		break; \
	} \
} \


void Dwrand_next(Dwrand *unit, int inNumSamples)
{
	int offset = unit->m_weights_size + 2;
	int weights_size = unit->mNumInputs - offset;
	if (inNumSamples) {

		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
		}
		while (true) {

			if (unit->m_repeatCount >= unit->m_repeats) {
				OUT0(0) = NAN;
				return;
			}

			if (ISDEMANDINPUT(unit->m_index)) {
				if (unit->m_needToResetChild) {
					unit->m_needToResetChild = false;
					RESETINPUT(unit->m_index);
				}
				float x = DEMANDINPUT_A(unit->m_index, inNumSamples);
				if (sc_isnan(x)) {

					WINDEX;
					unit->m_repeatCount++;
					unit->m_needToResetChild = true;
				} else {
					OUT0(0) = x;
					return;
				}
			} else {
				OUT0(0) = DEMANDINPUT_A(unit->m_index, inNumSamples);
				WINDEX;
				unit->m_repeatCount++;
				unit->m_needToResetChild = true;
				return;
			}
		}
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
		unit->m_needToResetChild = true;
		WINDEX;
	}
}

void Dwrand_Ctor(Dwrand *unit)
{
	SETCALC(Dwrand_next);
	unit->m_weights_size = IN0(1);
	Dwrand_next(unit, 0);
	OUT0(0) = 0.f;
}



static void Dshuf_scramble(Dshuf *unit);

void Dshuf_next(Dshuf *unit, int inNumSamples)
{
	// Print("->Dshuf_next %d\n", inNumSamples);
	if (inNumSamples) {
		//Print("   unit->m_repeats %d\n", unit->m_repeats);
		if (unit->m_repeats < 0.) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_repeats = sc_isnan(x) ? 0.f : floor(x + 0.5f);
		}
		while (true) {
			//Print("   unit->m_index %d   unit->m_repeatCount %d\n", unit->m_index, unit->m_repeatCount);
			if (unit->m_index >= (unit->mNumInputs - 1)) {
				unit->m_index = 0;
				unit->m_repeatCount++;
			}
			if (unit->m_repeatCount >= unit->m_repeats) {
				//Print("done\n");
				OUT0(0) = NAN;
				unit->m_index = 0;
				return;
			}
			if (ISDEMANDINPUT(unit->m_indices[unit->m_index])) {
				if (unit->m_needToResetChild) {
					unit->m_needToResetChild = false;
					RESETINPUT(unit->m_indices[unit->m_index]);
				}
				float x = DEMANDINPUT_A(unit->m_indices[unit->m_index], inNumSamples);

				if (sc_isnan(x)) {
					unit->m_index++;
					unit->m_needToResetChild = true;
				} else {
					OUT0(0) = x;
					return;
				}
			} else {
				OUT0(0) = DEMANDINPUT_A(unit->m_indices[unit->m_index], inNumSamples);
				//Print("   unit->m_index %d   OUT0(0) %g\n", unit->m_index, OUT0(0));
				unit->m_index++;
				unit->m_needToResetChild = true;
				return;
			}
		}
	} else {
		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0;
		unit->m_needToResetChild = true;
		unit->m_index = 0;
		Dshuf_scramble(unit);
	}
}

void Dshuf_scramble(Dshuf *unit) {
	int32 i, j, m, k, size;
	int32 temp;

	size = (int32)(unit->mNumInputs) - 1;

	if (size > 1) {
		k = size;
		for (i=0, m=k; i<k-1; ++i, --m) {
			j = i + unit->mParent->mRGen->irand(m);
			temp = unit->m_indices[i];
			unit->m_indices[i] = unit->m_indices[j];
			unit->m_indices[j] = temp;
		}
	}

}

void Dshuf_Ctor(Dshuf *unit)
{
	OUT0(0) = 0.f;

	uint32 size = (unit->mNumInputs) - 1;
	unit->m_indices = (int32*)RTAlloc(unit->mWorld, size * sizeof(int32));

	if (!unit->m_indices) {
		Print("Dshuf: RT memory allocation failed\n");
		SETCALC(ClearUnitOutputs);
		return;
	}

	for(uint32 i=0; i < size; ++i)
		unit->m_indices[i] = i + 1;

	SETCALC(Dshuf_next);
	Dshuf_next(unit, 0);
}

void Dshuf_Dtor(Dshuf *unit)
{
	RTFree(unit->mWorld, unit->m_indices);
}


void Dswitch1_next(Dswitch1 *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		if (sc_isnan(x)) {
			OUT0(0) = x;
			return;
		}
		int index = (int32)floor(x + 0.5f);
		index = sc_wrap(index, 0, unit->mNumInputs - 2) + 1;
		OUT0(0) = DEMANDINPUT_A(index, inNumSamples);
	} else {
		for (int i=0; i<unit->mNumInputs; ++i) {
			RESETINPUT(i);
		}
	}
}

void Dswitch1_Ctor(Dswitch1 *unit)
{
	SETCALC(Dswitch1_next);
	OUT0(0) = 0.f;
}

/////////////////////////////

void Dswitch_next(Dswitch *unit, int inNumSamples)
{
	if (inNumSamples) {
		float val = DEMANDINPUT_A(unit->m_index, inNumSamples);
		//printf("index: %i\n", (int) val);
		if(sc_isnan(val)) {
			float ival = DEMANDINPUT_A(0, inNumSamples);

			if(sc_isnan(ival))
				val = ival;
			else {
				int index = (int32)floor(ival + 0.5f);
				index = sc_wrap(index, 0, unit->mNumInputs - 2) + 1;
				val = DEMANDINPUT_A(index, inNumSamples);

				RESETINPUT(unit->m_index);
				// printf("resetting index: %i\n", unit->m_index);
				unit->m_index = index;
			}
		}
		OUT0(0) = val;

	} else {
		printf("...\n");
		for (int i=0; i<unit->mNumInputs; ++i) {
			RESETINPUT(i);
		}
		int index = (int32)floor(DEMANDINPUT(0) + 0.5f);
		index = sc_wrap(index, 0, unit->mNumInputs - 1) + 1;
		unit->m_index = index;
	}
}

void Dswitch_Ctor(Dswitch *unit)
{
	SETCALC(Dswitch_next);
	int index = (int32)floor(DEMANDINPUT(0) + 0.5f);
	index = sc_wrap(index, 0, unit->mNumInputs - 1) + 1;
	unit->m_index = index;
	OUT0(0) = 0.f;
}

//////////////////////////////


void Dstutter_next(Dstutter *unit, int inNumSamples)
{
	if (inNumSamples) {

		if (unit->m_repeatCount >= unit->m_repeats) {

			float val = DEMANDINPUT_A(1, inNumSamples);
			float repeats = DEMANDINPUT_A(0, inNumSamples);

			if(sc_isnan(repeats) || sc_isnan(val)) {
				OUT0(0) = NAN;
				return;
			} else {
				unit->m_value = val;
				unit->m_repeats = floor(repeats + 0.5f);
				unit->m_repeatCount = 0.f;
			}
		}

		OUT0(0) = unit->m_value;
		unit->m_repeatCount++;

	} else {

		unit->m_repeats = -1.f;
		unit->m_repeatCount = 0.f;
		RESETINPUT(0);
		RESETINPUT(1);

	}
}

void Dstutter_Ctor(Dstutter *unit)
{
	SETCALC(Dstutter_next);
	Dstutter_next(unit, 0);
	OUT0(0) = 0.f;
}

//////////////////////////////

void Dconst_next(Dconst *unit, int inNumSamples)
{
	float total, tolerance;

	if (inNumSamples) {
		if(sc_isnan(unit->m_runningsum)) {
			OUT0(0) = NAN;
			return;
		}

		if (unit->m_total < 0.f) {

			total = DEMANDINPUT_A(0, inNumSamples);
			tolerance = DEMANDINPUT_A(2, inNumSamples);
			if(sc_isnan(total) || sc_isnan(tolerance)) {
				OUT0(0) = NAN;
				return;
			}
			unit->m_total = total;
			unit->m_tolerance = tolerance;

		} {
			total = unit->m_total;
			tolerance = unit->m_tolerance;
		}

		float val = DEMANDINPUT_A(1, inNumSamples);

		if(sc_isnan(val)) {
			OUT0(0) = NAN;
			return;
		} else {
			float runningsum = unit->m_runningsum + val;
			if((runningsum > total) || (fabs(total - runningsum) <= tolerance)) {
				OUT0(0) = total - unit->m_runningsum;
				unit->m_runningsum = NAN;	// stop next time
			} else {
				unit->m_runningsum = runningsum;
				OUT0(0) = val;
			}
		}

	} else {

		unit->m_total = -1.f;
		unit->m_runningsum = 0.f;
		RESETINPUT(0);
		RESETINPUT(1);
		RESETINPUT(2);

	}
}

void Dconst_Ctor(Dconst *unit)
{
	SETCALC(Dconst_next);
	Dconst_next(unit, 0);
	OUT0(0) = 0.f;
}

//////////////////////////////



void Dpoll_next(Dpoll *unit, int inNumSamples)
{
	if (inNumSamples) {
			float x = DEMANDINPUT_A(0, inNumSamples);
			float run = DEMANDINPUT_A(2, inNumSamples) > 0.f;
			if(unit->m_mayprint && run) {
				Print("%s: %g block offset: %d\n", unit->m_id_string, x, inNumSamples - 1);
			}
			if(IN0(1) >= 0.0) SendTrigger(&unit->mParent->mNode, (int)IN0(1), x);
			OUT0(0) = x;
	} else {
		RESETINPUT(0);
	}
}

void Dpoll_Ctor(Dpoll *unit)
{
	OUT0(0) = 0.f;

	const int idStringSize = (int)IN0(3);

	unit->m_id_string = (char*)RTAlloc(unit->mWorld, (idStringSize + 1) * sizeof(char));

	if (!unit->m_id_string) {
		Print("Dpoll: RT memory allocation failed\n");
		SETCALC(ClearUnitOutputs);
		return;
	}

	for(int i = 0; i < idStringSize; i++)
		unit->m_id_string[i] = (char)IN0(4+i);

	SETCALC(Dpoll_next);
	unit->m_id_string[idStringSize] = '\0';
	unit->m_mayprint = unit->mWorld->mVerbosity >= -1;
	OUT0(0) = 0.f;
}

void Dpoll_Dtor(Dpoll* unit)
{
	RTFree(unit->mWorld, unit->m_id_string);
}

//////////////////////////////



void Dreset_next(Dreset *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT_A(0, inNumSamples);
		float reset = DEMANDINPUT_A(1, inNumSamples);
		if(sc_isnan(x)) {
			OUT0(0) = NAN;
			return;
		}
		if(reset > 0.0 && (unit->prev_reset <= 0.0)) { RESETINPUT(0); }
		unit->prev_reset = reset;
		OUT0(0) = x;
	} else {
		RESETINPUT(0);
	}
}

void Dreset_Ctor(Dreset *unit)
{
	SETCALC(Dreset_next);
	unit->prev_reset = 0.0;
	Dreset_next(unit, 0);
}



//////////////////////////////



void Donce_next(Donce *unit, int inNumSamples)
{
	if (inNumSamples) {
		if (unit->m_bufcounter == unit->mWorld->mBufCounter) {
			OUT0(0) = unit->m_prev;
		} else {
			float x = DEMANDINPUT_A(0, inNumSamples);
			unit->m_prev = x;
			OUT0(0) = x;
		}
	} else {
		RESETINPUT(0);
	}
}

void Donce_Ctor(Donce *unit)
{
	SETCALC(Donce_next);
	OUT0(0) = 0.f;
}


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

#define D_CHECK_BUF \
	if (!bufData) { \
                unit->mDone = true; \
		ClearUnitOutputs(unit, 1); \
		return; \
	}

#define D_GET_BUF \
	float fbufnum  = DEMANDINPUT_A(0, inNumSamples);; \
	if (sc_isnan(fbufnum)) { \
		OUT0(0) = NAN; \
		return; \
	} \
	fbufnum = sc_max(0.f, fbufnum); \
	if (fbufnum != unit->m_fbufnum) { \
		int bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum < 0) { bufnum = 0; } \
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
		unit->m_fbufnum = fbufnum; \
	} \
	SndBuf *buf = unit->m_buf; \
	LOCK_SNDBUF(buf); \
	float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
	uint32 bufFrames = buf->frames; \
	int mask __attribute__((__unused__)) = buf->mask; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2;

#define D_GET_BUF_SHARED \
	float fbufnum  = DEMANDINPUT_A(0, inNumSamples);; \
	if (sc_isnan(fbufnum)) { \
		OUT0(0) = NAN; \
		return; \
	} \
	fbufnum = sc_max(0.f, fbufnum); \
	if (fbufnum != unit->m_fbufnum) { \
		int bufnum = (int)fbufnum; \
		World *world = unit->mWorld; \
		if (bufnum < 0) { bufnum = 0; } \
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
		unit->m_fbufnum = fbufnum; \
	} \
	const SndBuf *buf = unit->m_buf; \
	LOCK_SNDBUF_SHARED(buf); \
	const float *bufData __attribute__((__unused__)) = buf->data; \
	uint32 bufChannels __attribute__((__unused__)) = buf->channels; \
	uint32 bufSamples __attribute__((__unused__)) = buf->samples; \
	uint32 bufFrames = buf->frames; \
	int mask __attribute__((__unused__)) = buf->mask; \
	int guardFrame __attribute__((__unused__)) = bufFrames - 2;


void Dbufrd_next(Dbufrd *unit, int inNumSamples)
{
	int32 loop     = (int32)DEMANDINPUT_A(2, inNumSamples);

	D_GET_BUF_SHARED
	D_CHECK_BUF

	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);

	double phase;
	if (inNumSamples)
		{
			float x = DEMANDINPUT_A(1, inNumSamples);
			if (sc_isnan(x)) {
					OUT0(0) = NAN;
					return;
			}
			phase = x;

			phase = sc_loop((Unit*)unit, phase, loopMax, loop);
			int32 iphase = (int32)phase;
			const float* table1 = bufData + iphase * bufChannels;
			OUT0(0) = table1[0];
		}
		else
		{
			RESETINPUT(1);
		}
}

void Dbufrd_Ctor(Dbufrd *unit)
{
	SETCALC(Dbufrd_next);

	unit->m_fbufnum = -1e9f;

	Dbufrd_next(unit, 0);
	OUT0(0) = 0.f;
}

////////////////////////////////////

void Dbufwr_next(Dbufwr *unit, int inNumSamples)
{
	int32 loop     = (int32)DEMANDINPUT_A(3, inNumSamples);

	D_GET_BUF
	D_CHECK_BUF

	double loopMax = (double)(loop ? bufFrames : bufFrames - 1);

	double phase;
	float val;
	if (inNumSamples)
	{
		float x = DEMANDINPUT_A(1, inNumSamples);
		if (sc_isnan(x)) {
			OUT0(0) = NAN;
			return;
		}
		phase = x;
		val = DEMANDINPUT_A(2, inNumSamples);
		if (sc_isnan(val)) {
			OUT0(0) = NAN;
			return;
		}

		phase = sc_loop((Unit*)unit, phase, loopMax, loop);
		int32 iphase = (int32)phase;
		float* table0 = bufData + iphase * bufChannels;
		table0[0] = val;
		OUT0(0) = val;
	}
	else
	{
		RESETINPUT(1);
		RESETINPUT(2);
	}
}

void Dbufwr_Ctor(Dbufwr *unit)
{
	SETCALC(Dbufwr_next);

	unit->m_fbufnum = -1e9f;

	Dbufwr_next(unit, 0);
	OUT0(0) = 0.f;
}


//////////////////////////////////////////////////////



PluginLoad(Demand)
{
	ft = inTable;

	DefineDtorCantAliasUnit(Demand);
	DefineSimpleCantAliasUnit(Duty);
	DefineSimpleCantAliasUnit(DemandEnvGen);
	DefineSimpleCantAliasUnit(TDuty);
	DefineSimpleUnit(Dseries);
	DefineSimpleUnit(Dgeom);
	DefineSimpleUnit(Dwhite);
	DefineSimpleUnit(Dbrown);
	DefineSimpleUnit(Diwhite);
	DefineSimpleUnit(Dibrown);
	DefineSimpleUnit(Dseq);
	DefineSimpleUnit(Dser);
	DefineSimpleUnit(Dbufrd);
	DefineSimpleUnit(Dbufwr);
	DefineSimpleUnit(Drand);
	DefineSimpleUnit(Dwrand);
	DefineSimpleUnit(Dxrand);
	DefineDtorUnit(Dshuf);
	DefineSimpleUnit(Dswitch1);
	DefineSimpleUnit(Dswitch);
	DefineSimpleUnit(Dstutter);
	DefineSimpleUnit(Dconst);
	DefineSimpleUnit(Donce);
	DefineSimpleUnit(Dreset);
	DefineDtorUnit(Dpoll);
}
