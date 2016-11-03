/*

"Unpack FFT" UGens for SuperCollider 3.
Copyright (c) 2007 Dan Stowell. All rights reserved.

(Written during the SC Symposium 2007! Thanks to all whose conversation fed into this.)


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
#include "SCComplex.h"
#include "FFT_UGens.h"

struct Unpack1FFT : Unit {
	int bufsize;
	int latestMomentProcessed; // To avoid processing a given FFT frame more than once

	int binindex;

	bool wantmag; // yes for mag, no for phase
	float outval;

	//int numOldSkipped; // for debug
};

struct PackFFT : Unit {
	int bufsize, numinvals, frombin, tobin;
	bool zeroothers;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void Unpack1FFT_Ctor(Unpack1FFT *unit);
	void Unpack1FFT_next_dc(Unpack1FFT *unit, int inNumSamples);
	void Unpack1FFT_next_nyq(Unpack1FFT *unit, int inNumSamples);
	void Unpack1FFT_next_mag(Unpack1FFT *unit, int inNumSamples);
	void Unpack1FFT_next_phase(Unpack1FFT *unit, int inNumSamples);

	void PackFFT_Ctor(PackFFT *unit);
	void PackFFT_Dtor(PackFFT *unit);
	void PackFFT_next(PackFFT *unit, int inNumSamples);
}

InterfaceTable *ft;

////////////////////////////////////////////////////////////////////////////////////////////////////////


void Unpack1FFT_Ctor(Unpack1FFT* unit)
{
	unit->bufsize = (int)ZIN0(1);
	unit->latestMomentProcessed = -1;
	//unit->numOldSkipped = 0;

	unit->outval = 0.f;

	unit->binindex = (int)ZIN0(2);

	if(ZIN0(3) == 0.f){
		// Mags
		if(unit->binindex == 0){
			SETCALC(Unpack1FFT_next_dc);
		}else if(unit->binindex == unit->bufsize >> 1){
			SETCALC(Unpack1FFT_next_nyq);
		} else {
			SETCALC(Unpack1FFT_next_mag);
		}
	}else{
		// Phases
		if(unit->binindex == 0){
			SETCALC(*ClearUnitOutputs);
		}else if(unit->binindex == unit->bufsize >> 1){
			SETCALC(*ClearUnitOutputs);
		} else {
			SETCALC(Unpack1FFT_next_phase);
		}
	}

	ZOUT0(0) = unit->outval;
}


#define UNPACK1FFT_NEXT_COMMON 		float fbufnum = ZIN0(0); \
	if (fbufnum < 0.f) { \
		if(unit->mWorld->mVerbosity > -1){ \
			Print("Unpack1FFT_next: warning, fbufnum < 0\n"); \
		} \
		ZOUT0(0) = unit->outval; \
		return; \
	} \
	uint32 ibufnum = (uint32)fbufnum; \
	World *world = unit->mWorld; \
	SndBuf *buf; \
	if (ibufnum >= world->mNumSndBufs) { \
		int localBufNum = ibufnum - world->mNumSndBufs; \
		Graph *parent = unit->mParent; \
		if(localBufNum <= parent->localBufNum) { \
			buf = parent->mLocalSndBufs + localBufNum; \
		} else { \
			buf = world->mSndBufs; \
			if(unit->mWorld->mVerbosity > -1){ \
				Print("Unpack1FFT_next: warning, bufnum too large: i%\n", ibufnum); \
			} \
		} \
	} else { \
		buf = world->mSndBufs + ibufnum; \
	} \
	int binindex __attribute__((__unused__)) = unit->binindex; \
	LOCK_SNDBUF(buf); \
	SCComplexBuf *p = ToComplexApx(buf); \



void Unpack1FFT_next_mag(Unpack1FFT *unit, int inNumSamples)
{
	if(unit->latestMomentProcessed != unit->mWorld->mBufCounter){
		UNPACK1FFT_NEXT_COMMON

		unit->outval = hypotf(p->bin[binindex-1].real, p->bin[binindex-1].imag);

		unit->latestMomentProcessed = unit->mWorld->mBufCounter; // So we won't copy it again, not this frame anyway
		//unit->numOldSkipped = 0;
	//}else{
		//Print("skipold{%i,%i}", unit->mWorld->mBufCounter, 	++unit->numOldSkipped);
		//Print("Calculation previously done - skipping. unit->mWorld->mBufCounter = %i\n", unit->mWorld->mBufCounter);
	}

	ZOUT0(0) = unit->outval;
}

void Unpack1FFT_next_phase(Unpack1FFT *unit, int inNumSamples)
{
	if(unit->latestMomentProcessed != unit->mWorld->mBufCounter){
		UNPACK1FFT_NEXT_COMMON

		unit->outval = atan2(p->bin[binindex-1].imag, p->bin[binindex-1].real);

		unit->latestMomentProcessed = unit->mWorld->mBufCounter; // So we won't copy it again, not this frame anyway
		//unit->numOldSkipped = 0;
	//}else{
		//Print("skipold{%i,%i}", unit->mWorld->mBufCounter, 	++unit->numOldSkipped);
		//Print("Calculation previously done - skipping. unit->mWorld->mBufCounter = %i\n", unit->mWorld->mBufCounter);
	}

	ZOUT0(0) = unit->outval;
}

void Unpack1FFT_next_dc(Unpack1FFT *unit, int inNumSamples)
{
	if(unit->latestMomentProcessed != unit->mWorld->mBufCounter){
		UNPACK1FFT_NEXT_COMMON

		unit->outval = p->dc;

		unit->latestMomentProcessed = unit->mWorld->mBufCounter; // So we won't copy it again, not this frame anyway
		//unit->numOldSkipped = 0;
	//}else{
		//Print("skipold{%i,%i}", unit->mWorld->mBufCounter, 	++unit->numOldSkipped);
		//Print("Calculation previously done - skipping. unit->mWorld->mBufCounter = %i\n", unit->mWorld->mBufCounter);
	}

	ZOUT0(0) = unit->outval;
}

void Unpack1FFT_next_nyq(Unpack1FFT *unit, int inNumSamples)
{
	if(unit->latestMomentProcessed != unit->mWorld->mBufCounter){
		UNPACK1FFT_NEXT_COMMON

		unit->outval = p->nyq;

		unit->latestMomentProcessed = unit->mWorld->mBufCounter; // So we won't copy it again, not this frame anyway
		//unit->numOldSkipped = 0;
	//}else{
		//Print("skipold{%i,%i}", unit->mWorld->mBufCounter, 	++unit->numOldSkipped);
		//Print("Calculation previously done - skipping. unit->mWorld->mBufCounter = %i\n", unit->mWorld->mBufCounter);
	}

	ZOUT0(0) = unit->outval;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////


void PackFFT_Ctor(PackFFT* unit)
{
	SETCALC(PackFFT_next);

	unit->bufsize = (int)ZIN0(1);
	unit->frombin = (int)ZIN0(2);
	unit->tobin = (int)ZIN0(3);
	unit->zeroothers = ZIN0(4) > 0;
	unit->numinvals = (int)ZIN0(5);

//	Print("PackFFT_Ctor: Passing chain through, val %g\n", ZIN0(0));
	ZOUT0(0) = ZIN0(0); // Required: allows the buffer index to fall through nicely to the IFFT
}

#define PACKFFT_INPUTSOFFSET 6

void PackFFT_next(PackFFT *unit, int inNumSamples)
{
	/////////////////// cf PV_GET_BUF
	float fbufnum = ZIN0(0);
	if (fbufnum < 0.f) {
		ZOUT0(0) = -1.f;
		return;
	}
	uint32 ibufnum = (uint32)fbufnum;
	World *world = unit->mWorld;
	SndBuf *buf;
	if (ibufnum >= world->mNumSndBufs) {
		int localBufNum = ibufnum - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localBufNum) {
			buf = parent->mLocalSndBufs + localBufNum;
		} else {
			buf = world->mSndBufs;
		}
	} else {
		buf = world->mSndBufs + ibufnum;
	}
	LOCK_SNDBUF(buf);

	int numbins = (buf->samples - 2) >> 1;
	/////////////////// cf PV_GET_BUF

//RM	Print("PackFFT_next: fbufnum = %g\n", fbufnum);

	int numinvals = unit->numinvals;

	SCComplexBuf *p = ToComplexApx(buf);

	int frombin    = unit->frombin;
	int tobin      = unit->tobin;
	int zeroothers = unit->zeroothers;


	// Load data from inputs into "p"

	if(frombin==0){
		p->dc = DEMANDINPUT(PACKFFT_INPUTSOFFSET);
	}else if(zeroothers){
		p->dc = 0.f;
	}
	//Print("New DC is %g\n", p->dc);

	if(tobin == numbins + 1){
		//Print("PackFFT: Fetching nyquist from input #%i\n", (PACKFFT_INPUTSOFFSET + numinvals - 2 - frombin - frombin));
		p->nyq = DEMANDINPUT(PACKFFT_INPUTSOFFSET + numinvals - 2 - frombin - frombin);
	}else if(zeroothers){
		p->nyq = 0.f;
	}
	//Print("New nyq (input #%i) is %g\n", numinvals, p->nyq);

	// real, imag = (mag * cos(phase), mag * sin(phase))
	float mag, phase;
	int startat = frombin==0 ? 0 : frombin-1;
	int endbefore = sc_min(numbins, tobin);
	for(int i = startat; i < endbefore; i++){
		//Print("PackFFT: Fetching mag from input #%i\n", (i + i + PACKFFT_INPUTSOFFSET + 2 - frombin - frombin));
		mag   = DEMANDINPUT(i + i + PACKFFT_INPUTSOFFSET + 2 - frombin - frombin);
		phase = DEMANDINPUT(i + i + PACKFFT_INPUTSOFFSET + 3 - frombin - frombin);
		p->bin[i].real = mag * cos(phase);
		p->bin[i].imag = mag * sin(phase);
	}
	//Print("New bin 7 is %g,%g\n", p->bin[7].real, p->bin[7].imag);

	if(zeroothers){
		// Iterate through the ones we didn't fill in, wiping the magnitude
		for(int i=0; i<startat; i++)
			p->bin[i].real = p->bin[i].imag = 0.f;
		for(int i=endbefore; i<numbins; i++)
			p->bin[i].real = p->bin[i].imag = 0.f;
	}

	ZOUT0(0) = fbufnum;
	//Print("PackFFT: fbufnum=%g, ibufnum=%i, numinvals=%i, frombin=%i, tobin=%i, zeroothers=%i\n",
	//				fbufnum, ibufnum, numinvals, frombin, tobin, zeroothers);
	//Print("PackFFT: p->bin[4].real=%g, p->bin[4].imag=%g, p->bin[5].real=%g, p->bin[5].imag=%g\n",
	//				p->bin[4].real, p->bin[4].imag, p->bin[5].real, p->bin[5].imag);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////


PluginLoad(UnpackFFTUGens)
{
	ft= inTable;

	DefineSimpleUnit(Unpack1FFT);
	DefineSimpleUnit(PackFFT);
}
