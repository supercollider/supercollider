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

#define MAXCHANNELS 32

struct Demand : public Unit
{
	float m_prevtrig;
	float m_prevreset;
	float m_prevout[MAXCHANNELS];
};

struct Dseries : public Unit
{
	int32 m_repeats;
	int32 m_repeatCount;
	double m_value;
	double m_step;
};

struct Dgeom : public Unit
{
	int32 m_repeats;
	int32 m_repeatCount;
	double m_value;
	double m_grow;
};

struct Dwhite : public Unit
{
	int32 m_repeats;
	int32 m_repeatCount;
	float m_lo;
	float m_range;
};

struct Dbrown : public Unit
{
	int32 m_repeats;
	int32 m_repeatCount;
	float m_lo;
	float m_hi;
	float m_step;
	float m_val;
};

struct Diwhite : public Unit
{
	int32 m_repeats;
	int32 m_repeatCount;
	int32 m_lo;
	int32 m_range;
};

struct Dibrown : public Unit
{
	int32 m_repeats;
	int32 m_repeatCount;
	int32 m_lo;
	int32 m_hi;
	int32 m_step;
	int32 m_val;
};

struct Dseq : public Unit
{
	int32 m_repeats;
	int32 m_repeatCount;
	int32 m_index;
	bool m_needToResetChild;
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

struct Dswitch1 : public Unit
{
};

struct Donce : public Unit
{
	int m_bufcounter; 
	float m_prev;
};

extern "C"
{
void load(InterfaceTable *inTable);



void Demand_Ctor(Demand *unit);
void Demand_next(Demand *unit, int inNumSamples);

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

void Dser_Ctor(Dser *unit);
void Dser_next(Dser *unit, int inNumSamples);

void Drand_Ctor(Drand *unit);
void Drand_next(Drand *unit, int inNumSamples);

void Dxrand_Ctor(Dxrand *unit);
void Dxrand_next(Dxrand *unit, int inNumSamples);

void Dswitch1_Ctor(Dswitch1 *unit);
void Dswitch1_next(Dswitch1 *unit, int inNumSamples);

void Donce_Ctor(Donce *unit);
void Donce_next(Donce *unit, int inNumSamples);

};



void Demand_next_aa(Demand *unit, int inNumSamples)
{
	float *trig = ZIN(0);
	float *reset = ZIN(1);

	float *out[MAXCHANNELS];
	float prevout[MAXCHANNELS];
	for (int i=0; i<unit->mNumOutputs; ++i) {
		out[i] = OUT(i); 
		prevout[i] = unit->m_prevout[i];
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
				float x = DEMANDINPUT(j);
				//printf("in  %d %g\n", k, x);
				if (sc_isnan(x)) x = prevout[k];
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
	for (int i=0; i<unit->mNumOutputs; ++i) {
		unit->m_prevout[i] = prevout[i];
	}
}


void Demand_next_ak(Demand *unit, int inNumSamples)
{
	float *trig = ZIN(0);
	float zreset = IN0(1);

	float *out[MAXCHANNELS];
	float prevout[MAXCHANNELS];
	for (int i=0; i<unit->mNumOutputs; ++i) {
		out[i] = OUT(i); 
		prevout[i] = unit->m_prevout[i];
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
				float x = DEMANDINPUT(j);
				if (sc_isnan(x)) x = prevout[k];
				else prevout[k] = x;
				out[k][i] = x;
			}
		}
		prevtrig = ztrig;
		prevreset = zreset;
	}
	
	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
	for (int i=0; i<unit->mNumOutputs; ++i) {
		unit->m_prevout[i] = prevout[i];
	}
}


void Demand_next_ka(Demand *unit, int inNumSamples)
{
	float ztrig = IN0(0);
	float *reset = ZIN(1);

	float *out[MAXCHANNELS];
	float prevout[MAXCHANNELS];
	for (int i=0; i<unit->mNumOutputs; ++i) {
		out[i] = OUT(i); 
		prevout[i] = unit->m_prevout[i];
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
				float x = DEMANDINPUT(j);
				if (sc_isnan(x)) x = prevout[k];
				else prevout[k] = x;
				out[k][i] = x;
			}
		}
		prevtrig = ztrig;
		prevreset = zreset;
	}
	
	unit->m_prevtrig = prevtrig;
	unit->m_prevreset = prevreset;
	for (int i=0; i<unit->mNumOutputs; ++i) {
		unit->m_prevout[i] = prevout[i];
	}
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
	//Print("Demand_Ctor calc %08X\n", unit->mCalcFunc);
	unit->m_prevtrig = 0.f;
	unit->m_prevreset = 0.f;
	for (int i=0; i<unit->mNumOutputs; ++i) {
		unit->m_prevout[i] = 0.f;
		OUT0(i) = 0.f;
	}
}



void Dseries_next(Dseries *unit, int inNumSamples)
{
	if (inNumSamples) {
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
			unit->m_value = DEMANDINPUT(1);
			unit->m_step = DEMANDINPUT(2);
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		OUT0(0) = unit->m_value;
		unit->m_value += unit->m_step;
		unit->m_repeatCount++;
	} else {
		unit->m_repeats = -1;
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
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
			unit->m_value = DEMANDINPUT(1);
			unit->m_grow = DEMANDINPUT(2);
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		OUT0(0) = unit->m_value;
		unit->m_value *= unit->m_grow;
		unit->m_repeatCount++;
	} else {
		unit->m_repeats = -1;
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
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
			unit->m_lo = DEMANDINPUT(1);
			float hi = DEMANDINPUT(2);
			unit->m_range = hi - unit->m_lo;
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		float x = unit->mParent->mRGen->frand() * unit->m_range + unit->m_lo;
		OUT0(0) = x;
	} else {
		unit->m_repeats = -1;
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
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
			unit->m_lo = (int32)floor(DEMANDINPUT(1) + 0.5f);
			int32 hi = (int32)floor(DEMANDINPUT(2) + 0.5f);
			unit->m_range = hi - unit->m_lo + 1;
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		float x = unit->mParent->mRGen->irand(unit->m_range) + unit->m_lo;
		OUT0(0) = x;
	} else {
		unit->m_repeats = -1;
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
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
			unit->m_lo = DEMANDINPUT(1);
			unit->m_hi = DEMANDINPUT(2);
			unit->m_step = DEMANDINPUT(3);
			unit->m_val = unit->mParent->mRGen->frand() * (unit->m_hi - unit->m_lo) + unit->m_lo;
		}
		if (unit->m_repeatCount >= unit->m_repeats) {
			OUT0(0) = NAN;
			return;
		}
		OUT0(0) = unit->m_val;
		float x = unit->m_val + unit->mParent->mRGen->frand2() * unit->m_step;
		unit->m_val = sc_fold(x, unit->m_lo, unit->m_hi);
	} else {
		unit->m_repeats = -1;
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
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
			unit->m_lo = (int32)floor(DEMANDINPUT(1) + 0.5f);
			unit->m_hi = (int32)floor(DEMANDINPUT(2) + 0.5f);
			unit->m_step = (int32)floor(DEMANDINPUT(3) + 0.5f);
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
		unit->m_repeats = -1;
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
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
		}
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
				float x = DEMANDINPUT(unit->m_index);
				if (sc_isnan(x)) {
					unit->m_index++;
					unit->m_needToResetChild = true;
				} else {
					OUT0(0) = x;
					return;
				}
			} else {
				OUT0(0) = IN0(unit->m_index);
				//Print("   unit->m_index %d   OUT0(0) %g\n", unit->m_index, OUT0(0));
				unit->m_index++;
				unit->m_needToResetChild = true;
				return;
			}
		}
	} else {
		unit->m_repeats = -1;
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
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
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
				float x = DEMANDINPUT(unit->m_index);
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
		unit->m_repeats = -1;
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
		
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
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
				float x = DEMANDINPUT(unit->m_index);
				if (sc_isnan(x)) {
					unit->m_index = unit->mParent->mRGen->irand(unit->mNumInputs - 1) + 1;
					unit->m_repeatCount++;
					unit->m_needToResetChild = true;
				} else {
					OUT0(0) = x;
					return;
				}
			} else {
				OUT0(0) = IN0(unit->m_index);
				unit->m_index = unit->mParent->mRGen->irand(unit->mNumInputs - 1) + 1;
				unit->m_repeatCount++;
				unit->m_needToResetChild = true;
				return;
			}
		}
	} else {
		unit->m_repeats = -1;
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
		if (unit->m_repeats < 0) {
			float x = DEMANDINPUT(0);
			unit->m_repeats = sc_isnan(x) ? 0 : (int32)floor(x + 0.5f);
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
				float x = DEMANDINPUT(unit->m_index);
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
				OUT0(0) = IN0(unit->m_index);
				int newindex = unit->mParent->mRGen->irand(unit->mNumInputs - 2) + 1;
				unit->m_index = newindex < unit->m_index ? newindex : newindex + 1;
				unit->m_repeatCount++;
				unit->m_needToResetChild = true;
				return;
			}
		}
	} else {
		unit->m_repeats = -1;
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



void Dswitch1_next(Dswitch1 *unit, int inNumSamples)
{
	if (inNumSamples) {
		float x = DEMANDINPUT(0);
		if (sc_isnan(x)) {
			OUT0(0) = x;
			return;
		}
		int index = (int32)floor(x + 0.5f);
		index = sc_wrap(index, 0, unit->mNumInputs - 1) + 1;
		OUT0(0) = DEMANDINPUT(index);
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



void Donce_next(Donce *unit, int inNumSamples)
{
	if (inNumSamples) {
		if (unit->m_bufcounter == unit->mWorld->mBufCounter) {
			OUT0(0) = unit->m_prev;
		} else {
			float x = DEMANDINPUT(0);
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


void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineSimpleCantAliasUnit(Demand);
	DefineSimpleUnit(Dseries);
	DefineSimpleUnit(Dgeom);
	DefineSimpleUnit(Dwhite);
	DefineSimpleUnit(Dbrown);
	DefineSimpleUnit(Diwhite);
	DefineSimpleUnit(Dibrown);
	DefineSimpleUnit(Dseq);
	DefineSimpleUnit(Dser);
	DefineSimpleUnit(Drand);
	DefineSimpleUnit(Dxrand);
	DefineSimpleUnit(Dswitch1);
	DefineSimpleUnit(Donce);
}
