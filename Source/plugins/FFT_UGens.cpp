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

#include "FFT_UGens.h"

#if __VEC__
	#include <vecLib/vecLib.h>
	FFTSetup fftsetup[32];
#else
extern "C" {
	#include "fftlib.h"
	float *cosTable[32];
}
#endif

extern InterfaceTable *ft;

float *fftWindow[32];

struct FFTBase : public Unit
{
	SndBuf *m_fftsndbuf;
	float *m_fftbuf;
	
	int m_pos, m_bufsize, m_mask;
	int m_log2n, m_stage, m_whichOverlap;
	int m_stride;
	
	float m_fftbufnum;
};

struct FFT : public FFTBase
{
	float *m_inbuf; 
};

struct IFFT : public FFTBase
{
	float *m_outbuf[kNUMOVERLAPS]; 
};
//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{

	void FFT_next(FFT *unit, int inNumSamples);
	void FFT_Ctor(FFT* unit);
	void FFT_Dtor(FFT* unit);

	void IFFT_next(IFFT *unit, int inNumSamples);
	void IFFT_Ctor(IFFT* unit);
	void IFFT_Dtor(IFFT* unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void CopyInput(FFT *unit);
void CopyInput(FFT *unit)
{
	if (unit->m_whichOverlap == 0) {
		memcpy(unit->m_fftbuf, unit->m_inbuf, unit->m_bufsize * sizeof(float));
	} else {
		int stride = unit->m_stride;
		int32 size1 = unit->m_whichOverlap * stride;
		int32 size2 = (kNUMOVERLAPS - unit->m_whichOverlap) * stride;
		memcpy(unit->m_fftbuf, unit->m_inbuf + size1, size2 * sizeof(float));
		memcpy(unit->m_fftbuf + size2, unit->m_inbuf, size1 * sizeof(float));
	}
}

void CopyOutput(IFFT *unit);
void CopyOutput(IFFT *unit)
{
	float *out = unit->m_outbuf[unit->m_whichOverlap];	
	memcpy(out, unit->m_fftbuf, unit->m_bufsize * sizeof(float));	
}

void DoWindowing(FFTBase *unit);
void DoWindowing(FFTBase *unit)
{
	float *win = fftWindow[unit->m_log2n];
	if (!win) return;
	float *in = unit->m_fftbuf - 1;
	win--;
	int size = unit->m_bufsize;
	for (int i=0; i<size; ++i) {
		*++in *= *++win;
	}
}

void FFTBase_Ctor(FFTBase *unit);
void FFTBase_Ctor(FFTBase *unit)
{
	World *world = unit->mWorld;

	uint32 bufnum = (uint32)ZIN0(0);
	if (bufnum >= world->mNumSndBufs) bufnum = 0;
	SndBuf *buf = world->mSndBufs + bufnum; 
	
	unit->m_fftsndbuf = buf;
	unit->m_fftbufnum = bufnum;
	unit->m_bufsize = buf->samples;
	if (unit->m_bufsize < 8 || !ISPOWEROFTWO(unit->m_bufsize)) {
		//Print("FFTBase_Ctor: buffer size not a power of two\n");
		// buffer not a power of two
		SETCALC(*ClearUnitOutputs);
		return;
	}
	
	unit->m_log2n = LOG2CEIL(unit->m_bufsize);
	unit->m_mask = buf->mask / kNUMOVERLAPS;
	unit->m_stride  = unit->m_bufsize / kNUMOVERLAPS;
	unit->m_whichOverlap = 0;
	unit->m_stage = 0;
	unit->m_pos = 0;
	
	ZOUT0(0) = ZIN0(0);
}

void FFT_Ctor(FFT *unit)
{
	FFTBase_Ctor(unit);
	int size = unit->m_bufsize * sizeof(float);
	unit->m_inbuf = (float*)RTAlloc(unit->mWorld, size);
	memset(unit->m_inbuf, 0, size);
	
	SETCALC(FFT_next);	
}

void FFT_Dtor(FFT *unit)
{
	RTFree(unit->mWorld, unit->m_inbuf);
}

void FFT_next(FFT *unit, int wrongNumSamples)
{
	float *in = IN(1);
	float *out = unit->m_inbuf + unit->m_pos;
	
	int numSamples = unit->mWorld->mFullRate.mBufLength;
	
	// copy input
	Copy(numSamples, out, in);
	
	unit->m_pos += numSamples;
	
	if (unit->m_pos & unit->m_mask || unit->m_fftsndbuf->samples != unit->m_bufsize) {
		ZOUT0(0) = -1.f;
	} else {
		ZOUT0(0) = unit->m_fftbufnum;
		unit->m_fftbuf = unit->m_fftsndbuf->data;

		unit->m_whichOverlap = (unit->m_whichOverlap+1) & (kNUMOVERLAPS-1);
		if (unit->m_pos == unit->m_bufsize) unit->m_pos = 0;
				
		// copy to fftbuf
		CopyInput(unit);
		
		// do windowing
		DoWindowing(unit);
		
		// do fft
		int log2n = unit->m_log2n;
#if __VEC__
		ctoz(unit->m_fftbuf, 2, outbuf, 1, 1L<<log2n);
		
#else
		rffts(unit->m_fftbuf, log2n, 1, cosTable[log2n]);
#endif
		
		unit->m_fftsndbuf->coord = coord_Complex;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

void OverlapAddOutput0(IFFT *unit, int inNumSamples, float *out);
void OverlapAddOutput0(IFFT *unit, int inNumSamples, float *out)
{
	for (int i=0; i<inNumSamples; ++i) {
		*++out = 0.;
	}
}

void OverlapAddOutput1(IFFT *unit, int inNumSamples, float *out);
void OverlapAddOutput1(IFFT *unit, int inNumSamples, float *out)
{
	int mask = unit->m_bufsize - 1;
	int pos = unit->m_pos;
	int stride = unit->m_stride;
	int pos1 = (pos + stride) & mask;
	float *outbuf1 = (float*)unit->m_outbuf[1] + pos1 - 1;
	
	for (int i=0; i<inNumSamples; ++i) {
		*++out = *++outbuf1;
	}
}

void OverlapAddOutput2(IFFT *unit, int inNumSamples, float *out);
void OverlapAddOutput2(IFFT *unit, int inNumSamples, float *out)
{
	int mask = unit->m_bufsize - 1;
	int pos = unit->m_pos;
	int stride = unit->m_stride;
	int pos0 = pos & mask;
	int pos1 = (pos + stride) & mask;
	float *outbuf0 = (float*)unit->m_outbuf[0] + pos0 - 1;
	float *outbuf1 = (float*)unit->m_outbuf[1] + pos1 - 1;

	for (int i=0; i<inNumSamples; ++i) {
		*++out = *++outbuf0 + *++outbuf1;
	}
}

/*
void OverlapAddOutput3(IFFT *unit, int inNumSamples, float *out);
void OverlapAddOutput3(IFFT *unit, int inNumSamples, float *out)
{
	int mask = unit->m_mask;
	int pos = unit->m_pos;
	int stride = unit->m_stride;
	float *fftbuf0 = (float*)unit->m_outbuf[0] + (pos & mask) - 1;
	float *fftbuf1 = (float*)unit->m_outbuf[1] + ((pos + stride) & mask) - 1;
	float *fftbuf2 = (float*)unit->m_outbuf[2] + ((pos + 2*stride) & mask) - 1;
	
	for (int i=0; i<inNumSamples; ++i) {
		*++out = *++fftbuf0 + *++fftbuf1 + *++fftbuf2;
	}
}

void OverlapAddOutput4(IFFT *unit, int inNumSamples, float *out);
void OverlapAddOutput4(IFFT *unit, int inNumSamples, float *out)
{
	int mask = unit->m_mask;
	int pos = unit->m_pos;
	int stride = unit->m_stride;
	float *fftbuf0 = (float*)unit->m_outbuf[0] + (pos & mask) - 1;
	float *fftbuf1 = (float*)unit->m_outbuf[1] + ((pos +   stride) & mask) - 1;
	float *fftbuf2 = (float*)unit->m_outbuf[2] + ((pos + 2*stride) & mask) - 1;
	float *fftbuf3 = (float*)unit->m_outbuf[3] + ((pos + 3*stride) & mask) - 1;
	
	for (int i=0; i<inNumSamples; ++i) {
		*++out = *++fftbuf0 + *++fftbuf1 + *++fftbuf2 + *++fftbuf3;
	}
}
*/

/////////////////////////////////////////////////////////////////////////////////////////////

void IFFT_Ctor(IFFT *unit)
{
	FFTBase_Ctor(unit);
	unit->m_outbuf[0] = (float*)RTAlloc(unit->mWorld, kNUMOVERLAPS * unit->m_bufsize * sizeof(float));
	unit->m_outbuf[1] = unit->m_outbuf[0] + unit->m_bufsize;	
	
	SETCALC(IFFT_next);		
}

void IFFT_Dtor(IFFT *unit)
{
	RTFree(unit->mWorld, unit->m_outbuf[0]);
}

void IFFT_next(IFFT *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	
	switch (unit->m_stage) {
		case 0 : OverlapAddOutput0(unit, inNumSamples, out); break;
		case 1 : OverlapAddOutput1(unit, inNumSamples, out); break;
		case 2 : OverlapAddOutput2(unit, inNumSamples, out); break;
//		case 3 : OverlapAddOutput3(unit, inNumSamples, out); break;
//		case 4 : OverlapAddOutput4(unit, inNumSamples, out); break;
	}
	
	unit->m_pos += inNumSamples;
	
	if (unit->m_pos & unit->m_mask || unit->m_fftsndbuf->samples != unit->m_bufsize) {
		unit->m_fftsndbuf->coord = coord_None;
	} else {		
		unit->m_fftbuf = unit->m_fftsndbuf->data;
		unit->m_whichOverlap = (unit->m_whichOverlap+1) & (kNUMOVERLAPS-1);
		if (unit->m_pos == unit->m_bufsize) unit->m_pos = 0;
		if (unit->m_stage < kNUMOVERLAPS) unit->m_stage++;

		ToComplexApx(unit->m_fftsndbuf);
		
		// do ifft
		int log2n = unit->m_log2n;
		riffts(unit->m_fftbuf, log2n, 1, cosTable[log2n]);	

		// do windowing
		DoWindowing(unit);
		
		CopyOutput(unit);

	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

float* create_cosTable(int log2n);
float* create_cosTable(int log2n)
{
	int size = 1 << log2n;
	int size2 = size / 4 + 1;
	float *win = (float*)malloc(size2 * sizeof(float));
	double winc = twopi / size;
	for (int i=0; i<size2; ++i) {
		double w = i * winc;
		win[i] = cos(w);
	}
	return win;
}

float* create_fftwindow(int log2n);
float* create_fftwindow(int log2n)
{
	int size = 1 << log2n;
	float *win = (float*)malloc(size * sizeof(float));
	//double winc = twopi / size;
	double winc = pi / size;
	for (int i=0; i<size; ++i) {
		double w = i * winc;
		//win[i] = 0.5 - 0.5 * cos(w);
		win[i] = sin(w);
	}
	return win;
}

void init_ffts();
void init_ffts()
{
#if __VEC__
	
	for (int i=0; i<32; ++i) {
		fftsetup[i] = 0;
	}
	for (int i=0; i<13; ++i) {
		fftsetup[i] = create_fftsetup(i, kFFTRadix2);
	}
#else
	for (int i=0; i<32; ++i) {
		cosTable[i] = 0;
		fftWindow[i] = 0;
	}
	for (int i=3; i<13; ++i) {
		cosTable[i] = create_cosTable(i);
		fftWindow[i] = create_fftwindow(i);
	}
#endif
}

void init_SCComplex(InterfaceTable *inTable);

void initFFT(InterfaceTable *inTable)
{
	ft = inTable;

	init_ffts();

	DefineDtorUnit(FFT);
	DefineDtorUnit(IFFT);
	
}

