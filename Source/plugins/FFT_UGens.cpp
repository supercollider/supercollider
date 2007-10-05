/*
	Improved FFT and IFFT UGens for SuperCollider 3
	Copyright (c) 2007 Dan Stowell, incorporating much code from
	SuperCollider 3 Copyright (c) 2002 James McCartney.
	All rights reserved.

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

// We include vDSP even if not using for FFT, since we want to use some vectorised add/mul tricks
#if SC_DARWIN
#include "vecLib/vDSP.h"
#endif

// Decisions here about which FFT library to use - vDSP only exists on Mac BTW.
// We include the relevant libs but also ensure that one, and only one, of them is active...
#if !(SC_FFT_FFTW || SC_FFT_GREEN) && SC_DARWIN

#define SC_FFT_FFTW 0
#define SC_FFT_VDSP 1
#define SC_FFT_GREEN 0

#elif SC_FFT_GREEN

#define SC_FFT_FFTW 0
#define SC_FFT_VDSP 0
#define SC_FFT_GREEN 1
extern "C" {
	#include "fftlib.h"
	static float *cosTable[32];
}

#else
//#elif SC_FFT_FFTW

#define SC_FFT_FFTW 1
#define SC_FFT_VDSP 0
#define SC_FFT_GREEN 0
#include <fftw3.h>

#endif



extern InterfaceTable *ft;

const int WINDOW_RECT = -1;
const int WINDOW_WELCH = 0;
const int WINDOW_HANN = 1;

static float *fftWindow[2][32];

#if SC_FFT_VDSP
static FFTSetup fftSetup[32]; // vDSP setups, one per FFT size
static COMPLEX_SPLIT splitBuf; // Temp buf for holding rearranged data
#endif

struct FFTBase : public Unit
{
	SndBuf *m_fftsndbuf;
	float *m_fftbuf;
	
	int m_pos, m_bufsize;
	int m_log2n;
	
	uint32 m_fftbufnum;
	
	//Added (since FFTBase from original FFT UGens):
#if SC_FFT_FFTW
	fftwf_plan m_plan;
#endif
	float *m_transformbuf;
	int m_hopsize, m_shuntsize; // These add up to m_bufsize
	int m_wintype;
};

struct FFT : public FFTBase
{
	float *m_inbuf; 
};

struct IFFT : public FFTBase
{
	float *m_olabuf;
	float m_scalefactor; // Different FFT algorithms introduce different scaling through ->FFT->IFFT->
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void FFT_Ctor(FFT* unit);
	void FFT_next(FFT *unit, int inNumSamples);
	void FFT_Dtor(FFT* unit);

	void IFFT_Ctor(IFFT* unit);
	void IFFT_next(IFFT *unit, int inNumSamples);
	void IFFT_Dtor(IFFT* unit);

}

//////////////////////////////////////////////////////////////////////////////////////////////////


void DoWindowing(FFTBase *unit, float* data);
void DoWindowing(FFTBase *unit, float* data)
{
	if(unit->m_wintype == WINDOW_RECT) return;
	
	float *win = fftWindow[unit->m_wintype][unit->m_log2n];
	if (!win) return;
	win--;
	int size = unit->m_bufsize;
#if SC_DARWIN
	vDSP_vmul(data, 1, win+1, 1, data, 1, size);
#else
	float *in = data - 1;
	for (int i=0; i< size ; ++i) {
		*++in *= *++win;
	}
#endif

#if SC_FFT_VDSP
	// vDSP scales the data differently during the forward FFT transform, compared against Green or FFTW, so we need to rescale
	float scalefac = 0.5f;
	vDSP_vsmul(data, 1, &scalefac, data, 1, size); // i.e. multiply all in-place by scalefac
#endif
}

void FFTBase_Ctor(FFTBase *unit);
void FFTBase_Ctor(FFTBase *unit)
{
	World *world = unit->mWorld;

	uint32 bufnum = (uint32)ZIN0(0);
	//Print("FFTBase_Ctor: bufnum is %i\n", bufnum);
	if (bufnum >= world->mNumSndBufs) bufnum = 0;
	SndBuf *buf = world->mSndBufs + bufnum; 
	
	unit->m_fftsndbuf = buf;
	unit->m_fftbufnum = bufnum;
	unit->m_bufsize = buf->samples;


	// Although FFTW allows non-power-of-two buffers (vDSP doesn't), this would complicate the windowing, so we don't allow it.
	if (!ISPOWEROFTWO(unit->m_bufsize)) {
		Print("FFTBase_Ctor error: buffer size (%i) not a power of two.\n", unit->m_bufsize);
		SETCALC(*ClearUnitOutputs);
		return;
	}
	else if (unit->m_bufsize < 8 || 
			(((int)(unit->m_bufsize / unit->mWorld->mFullRate.mBufLength)) 
					* unit->mWorld->mFullRate.mBufLength != unit->m_bufsize)) {
		Print("FFTBase_Ctor error: buffer size (%i) not a multiple of the block size (%i).\n", unit->m_bufsize, unit->mWorld->mFullRate.mBufLength);
		SETCALC(*ClearUnitOutputs);
		return;
	}
	
	unit->m_log2n = LOG2CEIL(unit->m_bufsize);
	unit->m_pos = 0;
	
	ZOUT0(0) = ZIN0(0);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void FFT_Ctor(FFT *unit)
{
	FFTBase_Ctor(unit);
	int size = unit->m_bufsize * sizeof(float);
	
	int hopsize = (int)(sc_max(sc_min(ZIN0(2), 1.f), 0.f) * unit->m_bufsize);
	if (((int)(hopsize / unit->mWorld->mFullRate.mBufLength)) * unit->mWorld->mFullRate.mBufLength 
				!= hopsize) {
		Print("FFT_Ctor: hopsize (%i) not an exact multiple of SC's block size (%i) - automatically corrected.\n", hopsize, unit->mWorld->mFullRate.mBufLength);
		hopsize = ((int)(hopsize / unit->mWorld->mFullRate.mBufLength)) * unit->mWorld->mFullRate.mBufLength;
	}
	unit->m_hopsize = hopsize;
	unit->m_shuntsize = unit->m_bufsize - hopsize;
	
	unit->m_wintype = (int)ZIN0(3);
	
	unit->m_inbuf = (float*)RTAlloc(unit->mWorld, size);
	
#if SC_FFT_FFTW
	if(unit->mWorld->mVerbosity > 1)
		Print("FFT using FFTW\n");
	// Transform buf is two floats "too big" because of FFTWF's output ordering
	unit->m_transformbuf = (float*)RTAlloc(unit->mWorld, size + sizeof(float) + sizeof(float));
	unit->m_plan = fftwf_plan_dft_r2c_1d(unit->m_bufsize, unit->m_transformbuf, 
			(fftwf_complex*) unit->m_transformbuf, FFTW_ESTIMATE);
	memset(unit->m_transformbuf, 0, size + sizeof(float) + sizeof(float));
#elif SC_FFT_GREEN
	if(unit->mWorld->mVerbosity > 1)
		Print("FFT using Green\n");
	// Green packs the nyquist in with the DC, so size is same as input buffer
	unit->m_transformbuf = (float*)RTAlloc(unit->mWorld, size);
#else
	if(unit->mWorld->mVerbosity > 1)
		Print("FFT using vDSP\n");
	// vDSP packs the nyquist in with the DC, so size is same as input buffer
	unit->m_transformbuf = (float*)RTAlloc(unit->mWorld, size);
	memset(unit->m_transformbuf, 0, size);
#endif
	
	memset(unit->m_inbuf, 0, size);
	
	//Print("FFT_Ctor: hopsize %i, shuntsize %i, bufsize %i, wintype %i, \n",
	//	unit->m_hopsize, unit->m_shuntsize, unit->m_bufsize, unit->m_wintype);
	
	SETCALC(FFT_next);	
}

void FFT_Dtor(FFT *unit)
{
#if SC_FFT_FFTW
	fftwf_destroy_plan(unit->m_plan);
#endif
	
	RTFree(unit->mWorld, unit->m_inbuf);
	RTFree(unit->mWorld, unit->m_transformbuf);
}

void FFT_next(FFT *unit, int wrongNumSamples)
{
	float *in = IN(1);
	float *out = unit->m_inbuf + unit->m_pos + unit->m_shuntsize;
	
	int numSamples = unit->mWorld->mFullRate.mBufLength;
	
	// copy input
	//Copy(numSamples, out, in);
	memcpy(out, in, numSamples * sizeof(float));
	
	unit->m_pos += numSamples;
	
	bool gate = ZIN0(4) > 0.f; // Buffer shunting continues, but no FFTing
	
	if (unit->m_pos != unit->m_hopsize || unit->m_fftsndbuf->samples != unit->m_bufsize) {
		ZOUT0(0) = -1.f;
	} else {
		
		unit->m_fftbuf = unit->m_fftsndbuf->data;

		unit->m_pos = 0;		
		
		if(gate){
			int size = unit->m_bufsize * sizeof(float);
			
			// Data goes to transform buf
			memcpy(unit->m_transformbuf, unit->m_inbuf, size);
			
			DoWindowing(unit, unit->m_transformbuf);

#if SC_FFT_FFTW
			fftwf_execute(unit->m_plan);
			// Rearrange output data onto public buffer
			memcpy(unit->m_fftbuf, unit->m_transformbuf, size);
			unit->m_fftbuf[1] = unit->m_transformbuf[unit->m_bufsize]; // Pack nyquist val in
#elif SC_FFT_GREEN
			// Green FFT is in-place
			rffts(unit->m_transformbuf, unit->m_log2n, 1, cosTable[unit->m_log2n]);
			// Copy to public buffer
			memcpy(unit->m_fftbuf, unit->m_transformbuf, size);
#else
			// Perform even-odd split
			vDSP_ctoz((COMPLEX*) unit->m_transformbuf, 2, &splitBuf, 1, unit->m_bufsize >> 1);
			// Now the actual FFT
			vDSP_fft_zrip(fftSetup[unit->m_log2n], &splitBuf, 1, unit->m_log2n, FFT_FORWARD);
			// Copy the data to the public output buf, transforming it back out of "split" representation
			vDSP_ztoc(&splitBuf, 1, (DSPComplex*) unit->m_fftbuf, 2, unit->m_bufsize >> 1);
#endif
			
			unit->m_fftsndbuf->coord = coord_Complex;
			ZOUT0(0) = unit->m_fftbufnum;
		} else {
			ZOUT0(0) = -1;
		}
		// Shunt input buf down
		memcpy(unit->m_inbuf, unit->m_inbuf + unit->m_hopsize, unit->m_shuntsize * sizeof(float));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

void IFFT_Ctor(IFFT* unit){
	FFTBase_Ctor(unit);
	
	// This will hold the transformed and progressively overlap-added data ready for outputting.
	unit->m_olabuf = (float*)RTAlloc(unit->mWorld, unit->m_bufsize * sizeof(float));
	memset(unit->m_olabuf, 0, unit->m_bufsize * sizeof(float));
	
	unit->m_wintype = (int)ZIN0(1);
	
#if SC_FFT_FFTW
	// Transform buf is two floats "too big" because of FFTWF's output ordering
	unit->m_transformbuf = (float*)RTAlloc(unit->mWorld, (unit->m_bufsize+2) * sizeof(float));
	// We'll be going from m_transformbuf back to fftbuf
	unit->m_plan = fftwf_plan_dft_c2r_1d(unit->m_bufsize, (fftwf_complex*) unit->m_transformbuf, 
			unit->m_fftsndbuf->data, FFTW_ESTIMATE);

	memset(unit->m_transformbuf, 0, (unit->m_bufsize+2) * sizeof(float));
	unit->m_scalefactor = 1.f / unit->m_bufsize;
#elif SC_FFT_GREEN
	unit->m_scalefactor = 1.f;
#else
	unit->m_scalefactor = 2.f / unit->m_bufsize;
#endif
	
	// "pos" will be reset to zero when each frame comes in. Until then, the following ensures silent output at first:
	unit->m_pos = unit->m_bufsize;
	
	SETCALC(IFFT_next);	
}

void IFFT_Dtor(IFFT* unit){
	RTFree(unit->mWorld, unit->m_olabuf);
#if SC_FFT_FFTW
	fftwf_destroy_plan(unit->m_plan);
	RTFree(unit->mWorld, unit->m_transformbuf);
#endif
}

void IFFT_next(IFFT *unit, int wrongNumSamples){
	
#if SC_DARWIN
	float *out = OUT(0);
#else
	float *out = ZOUT(0);
#endif

	// Load state from struct into local scope
	int pos     = unit->m_pos;
	int bufsize = unit->m_bufsize;
	int numSamples = unit->mWorld->mFullRate.mBufLength;
	float *olabuf = unit->m_olabuf;
	float fbufnum = ZIN0(0);
	float scalefactor = unit->m_scalefactor;
	
	// Only run the IFFT if we're receiving a new block of input data - otherwise just output data already received
	if (fbufnum >= 0.f){
		
		// Ensure it's in cartesian format, not polar
		ToComplexApx(unit->m_fftsndbuf);
		
		float* fftbuf = unit->m_fftsndbuf->data;
		
		// Do IFFT "in-place"... or however the different FFT libraries would like us to...
#if SC_FFT_FFTW
		// For FFTW we can't do it in-place because it expects a slightly different data format
		float *transformbuf = unit->m_transformbuf;
		memcpy(transformbuf, fftbuf, unit->m_bufsize * sizeof(float));
		transformbuf[1] = 0.f;
		transformbuf[unit->m_bufsize] = fftbuf[1];  // Nyquist goes all the way down to the end of the line...
		transformbuf[unit->m_bufsize+1] = 0.f;
		fftwf_execute(unit->m_plan);
#elif SC_FFT_GREEN
		riffts(fftbuf, unit->m_log2n, 1, cosTable[unit->m_log2n]);
#else
		// For vDSP we can't really do it in place either,
		//  because the data needs to be converted into then out of "split" (odd/even) representation
		vDSP_ctoz((COMPLEX*) fftbuf, 2, &splitBuf, 1, unit->m_bufsize >> 1);
		vDSP_fft_zrip(fftSetup[unit->m_log2n], &splitBuf, 1, unit->m_log2n, FFT_INVERSE);
		vDSP_ztoc(&splitBuf, 1, (DSPComplex*) fftbuf, 2, unit->m_bufsize >> 1);
#endif		
		
		// Then apply window in-place
		DoWindowing(unit, fftbuf);
		
		// Then shunt the "old" time-domain output down by one hop
		int hopsamps = pos;
		int shuntsamps = bufsize - hopsamps;
		if(hopsamps != bufsize){  // There's only copying to be done if the position isn't all the way to the end of the buffer
			memcpy(olabuf, olabuf+hopsamps, shuntsamps * sizeof(float));
		}
		
		// Then mix the "new" time-domain data in - adding at first, then just setting (copying) where the "old" is supposed to be zero.
		// NB we re-use the "pos" variable temporarily here for write rather than read
		for(pos = 0; pos < shuntsamps; pos++){
			olabuf[pos] += fftbuf[pos];
		}
		memcpy(olabuf + pos, fftbuf + pos, (bufsize-pos) * sizeof(float));
		
		// Move the pointer back to zero, which is where playback will next begin
		pos = 0;
		
	} // End of has-the-chain-fired
	
	// Now we can output some stuff, as long as there is still data waiting to be output.
	// If there is NOT data waiting to be output, we output zero. (Either irregular/negative-overlap 
	//     FFT firing, or FFT has given up, or at very start of execution.)
	if(pos >= bufsize){
		ClearUnitOutputs(unit, numSamples);
	}else{
#if SC_DARWIN
	vDSP_vsmul(olabuf + pos, 1, &scalefactor, out, 1, numSamples);
	pos += numSamples;
#else
		for(int i=0; i<numSamples; i++){
			ZXP(out) = (*(olabuf + (pos++))) * scalefactor;
		}
#endif
	}
	unit->m_pos = pos;
	
}



/////////////////////////////////////////////////////////////////////////////////////////////

#if SC_FFT_GREEN
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
#endif

float* create_fftwindow(int wintype, int log2n);
float* create_fftwindow(int wintype, int log2n)
{
	int size = 1 << log2n;
	float *win = (float*)malloc(size * sizeof(float));
	
	double winc;
	switch(wintype){
		case WINDOW_WELCH:
			winc = pi / size;
			for (int i=0; i<size; ++i) {
				double w = i * winc;
				win[i] = sin(w);
			}
			break;
		case WINDOW_HANN:
			winc = twopi / size;
			for (int i=0; i<size; ++i) {
				double w = i * winc;
				win[i] = 0.5 - 0.5 * cos(w);
			}
			break;
	}
	
	return win;
	
}

void init_ffts();
void init_ffts()
{
	for (int wintype=0; wintype<2; ++wintype) {
		for (int i=0; i<32; ++i) {
			fftWindow[wintype][i] = 0;
		}
		for (int i=3; i<13; ++i) {
			fftWindow[wintype][i] = create_fftwindow(wintype, i);
		}
	}
#if SC_FFT_GREEN
	for (int i=0; i<32; ++i) {
		cosTable[i] = 0;
	}
	for (int i=3; i<13; ++i) {
		cosTable[i] = create_cosTable(i);
	}
	//Print("FFT init: cosTable initialised.\n");
#elif SC_FFT_VDSP
	// vDSP inits its twiddle factors
	for (int i=3; i<13; ++i) {
		fftSetup[i] = vDSP_create_fftsetup (i, FFT_RADIX2);
		if(fftSetup[i] == NULL){
			Print("FFT ERROR: Mac vDSP library could not allocate FFT setup for size %i\n", 1<<i);
		}
	}
	// vDSP prepares its memory-aligned buffer for rearranging input data.
	// Note max size here - meaning max input buffer size is these two sizes added together.
	// vec_malloc used in API docs, but apparently that's deprecated and malloc is sufficient for aligned memory on OSX.
	splitBuf.realp = (float*) malloc ( 4096 * sizeof(float) );
    splitBuf.imagp = (float*) malloc ( 4096 * sizeof(float) );
	//Print("FFT init: vDSP initialised.\n");
#else
	//Print("FFT init: FFTW, no init needed.\n");
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

