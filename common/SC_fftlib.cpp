/*
SC_fftlib.cpp
An interface to abstract over different FFT libraries, for SuperCollider 3.
(c) 2007-2008 Dan Stowell, incorporating code from
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
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA

*/

#include "SC_fftlib.h"
#include "clz.h"
#include <stdlib.h>
#include <cmath>
#include <stdio.h>
#include <cstring>

// We include vDSP even if not using for FFT, since we want to use some vectorised add/mul tricks
#ifdef __APPLE__
	#include "vecLib/vDSP.h"
#endif

static float *fftWindow[2][SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1];

#if SC_FFT_VDSP
	static FFTSetup fftSetup[SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1]; // vDSP setups, one per FFT size
	static COMPLEX_SPLIT splitBuf; // Temp buf for holding rearranged data
#endif

#if SC_FFT_GREEN
	extern "C" {
		#include "fftlib.h"
		static float *cosTable[SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1];
	}
#endif

#define pi 3.1415926535898f
#define twopi 6.28318530717952646f

////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions

#if SC_FFT_GREEN
static float* create_cosTable(int log2n);
static float* create_cosTable(int log2n)
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

float* scfft_create_fftwindow(int wintype, int log2n);
float* scfft_create_fftwindow(int wintype, int log2n)
{
	int size = 1 << log2n;
	unsigned short i;
	float *win = (float*)malloc(size * sizeof(float));

	double winc;
	switch(wintype){
		case WINDOW_SINE:
			winc = pi / size;
			for (i=0; i<size; ++i) {
				double w = i * winc;
				win[i] = sin(w);
			}
			break;
		case WINDOW_HANN:
			winc = twopi / size;
			for (i=0; i<size; ++i) {
				double w = i * winc;
				win[i] = 0.5 - 0.5 * cos(w);
			}
			break;
	}

	return win;

}

void scfft_global_init(){
	unsigned short wintype, i;
	for (wintype=0; wintype<2; ++wintype) {
		for (i=0; i< SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1; ++i) {
			fftWindow[wintype][i] = 0;
		}
		for (i= SC_FFT_LOG2_MINSIZE; i < SC_FFT_LOG2_MAXSIZE+1; ++i) {
			fftWindow[wintype][i] = scfft_create_fftwindow(wintype, i);
		}
	}
	#if SC_FFT_GREEN
		for (i=0; i< SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1; ++i) {
			cosTable[i] = 0;
		}
		for (i= SC_FFT_LOG2_MINSIZE; i < SC_FFT_LOG2_MAXSIZE+1; ++i) {
			cosTable[i] = create_cosTable(i);
		}
		printf("SC FFT global init: cosTable initialised.\n");
	#elif SC_FFT_VDSP
		// vDSP inits its twiddle factors
		for (i= SC_FFT_LOG2_MINSIZE; i < SC_FFT_LOG2_MAXSIZE+1; ++i) {
			fftSetup[i] = vDSP_create_fftsetup (i, FFT_RADIX2);
			if(fftSetup[i] == NULL){
				printf("FFT ERROR: Mac vDSP library could not allocate FFT setup for size %i\n", 1<<i);
			}
		}
		// vDSP prepares its memory-aligned buffer for rearranging input data.
		// Note max size here - meaning max input buffer size is these two sizes added together.
		// vec_malloc used in API docs, but apparently that's deprecated and malloc is sufficient for aligned memory on OSX.
		splitBuf.realp = (float*) malloc ( SC_FFT_MAXSIZE * sizeof(float) / 2);
		splitBuf.imagp = (float*) malloc ( SC_FFT_MAXSIZE * sizeof(float) / 2);
		//printf("SC FFT global init: vDSP initialised.\n");
	#elif SC_FFT_FFTW
		//printf("SC FFT global init: FFTW, no init needed.\n");
	#endif
}

size_t scfft_trbufsize(unsigned int fullsize){
	#if SC_FFT_FFTW
		// Transform buf is two floats "too big" because of FFTWF's output ordering
		return (fullsize+2) * sizeof(float);
	#else
		// vDSP packs the nyquist in with the DC, so size is same as input buffer (plus zeropadding)
		// Green does this too
		return (fullsize  ) * sizeof(float);
	#endif
}

int scfft_create(scfft *f, unsigned int fullsize, unsigned int winsize, short wintype, float *indata, float *outdata, float *trbuf, bool forward){
	f->nfull = fullsize;
	f->nwin  =  winsize;
	f->log2nfull = LOG2CEIL(fullsize);
	f->log2nwin  = LOG2CEIL( winsize);
	f->wintype = wintype;
	f->indata  = indata;
	f->outdata = outdata;
	f->trbuf   = trbuf;

	// Buffer is larger than the range of sizes we provide for at startup; we can get ready just-in-time though
	if (fullsize > SC_FFT_MAXSIZE){
		scfft_ensurewindow(f->log2nfull, f->log2nwin, wintype);
	}

	#if SC_FFT_FFTW
		if(forward)
			f->plan = fftwf_plan_dft_r2c_1d(fullsize, trbuf, (fftwf_complex*) trbuf, FFTW_ESTIMATE);
		else
			f->plan = fftwf_plan_dft_c2r_1d(fullsize, (fftwf_complex*) trbuf, outdata, FFTW_ESTIMATE);
	#endif

	// The scale factors rescale the data to unity gain. The old Green lib did this itself, meaning scalefacs would here be 1...
	if(forward){
		#if SC_FFT_VDSP
			f->scalefac = 0.5f;
		#else // forward FFTW and Green factor
			f->scalefac = 1.f;
		#endif
	}else{ // backward FFTW and VDSP factor
		#if SC_FFT_GREEN
			f->scalefac = 1.f;
		#else  // fftw, vdsp
			f->scalefac = 1.f / fullsize;
		#endif
	}

	memset(trbuf, 0, scfft_trbufsize(fullsize));

	return 0;
}

static int largest_log2n = SC_FFT_LOG2_MAXSIZE;
void scfft_ensurewindow(unsigned short log2_fullsize, unsigned short log2_winsize, short wintype){

	// Ensure we have enough space to do our calcs
	if(log2_fullsize > largest_log2n){
		largest_log2n = log2_fullsize;
 		#if SC_FFT_VDSP
			size_t newsize = (1 << largest_log2n) * sizeof(float) / 2;
			splitBuf.realp = (float*) realloc (splitBuf.realp, newsize);
			splitBuf.imagp = (float*) realloc (splitBuf.imagp, newsize);
   		#endif
	}

	// Ensure our window has been created
	if((wintype != -1) && (fftWindow[wintype][log2_winsize] == 0)){
		fftWindow[wintype][log2_winsize] = scfft_create_fftwindow(wintype, log2_winsize);
	}

	// Ensure our FFT twiddle factors (or whatever) have been created
 	#if SC_FFT_VDSP
		if(fftSetup[log2_fullsize] == 0)
			fftSetup[log2_fullsize] = vDSP_create_fftsetup (log2_fullsize, FFT_RADIX2);
	#elif SC_FFT_GREEN
		if(cosTable[log2_fullsize] == 0)
			cosTable[log2_fullsize] = create_cosTable(log2_fullsize);
   	#endif
}

void scfft_dowindowing(float *data, unsigned int winsize, unsigned int fullsize, unsigned short log2_winsize, short wintype, float scalefac){
	int i;
	if(wintype != WINDOW_RECT){
		float *win = fftWindow[wintype][log2_winsize];
		if (!win) return;
		#ifdef __APPLE__
			vDSP_vmul(data, 1, win, 1, data, 1, winsize);
		#else
			--win;
			float *in = data - 1;
			for (i=0; i< winsize ; ++i) {
				*++in *= *++win;
			}
		#endif

	}

		// scale factor is different for different libs. But the compiler switch here is about using vDSP's fast multiplication method.
	#ifdef __APPLE__
		vDSP_vsmul(data, 1, &scalefac, data, 1, winsize);
	#else
		for(int i=0; i<winsize; ++i){
			data[i] *= scalefac;
		}
	#endif

	// Zero-padding:
	memset(data + winsize, 0, (fullsize - winsize) * sizeof(float));
}

void scfft_dofft(scfft *f){
	// Data goes to transform buf
	memcpy(f->trbuf, f->indata, f->nwin * sizeof(float));
	scfft_dowindowing(f->trbuf, f->nwin, f->nfull, f->log2nfull, f->wintype, f->scalefac);
	#if SC_FFT_FFTW
		fftwf_execute(f->plan);
		// Rearrange output data onto public buffer
		memcpy(f->outdata, f->trbuf, f->nfull * sizeof(float));
		f->outdata[1] = f->trbuf[f->nfull]; // Pack nyquist val in
	#elif SC_FFT_VDSP
		// Perform even-odd split
		vDSP_ctoz((COMPLEX*) f->trbuf, 2, &splitBuf, 1, f->nfull >> 1);
		// Now the actual FFT
		vDSP_fft_zrip(fftSetup[f->log2nfull], &splitBuf, 1, f->log2nfull, FFT_FORWARD);
		// Copy the data to the public output buf, transforming it back out of "split" representation
		vDSP_ztoc(&splitBuf, 1, (DSPComplex*) f->outdata, 2, f->nfull >> 1);
	#elif SC_FFT_GREEN
		// Green FFT is in-place
		rffts(f->trbuf, f->log2nfull, 1, cosTable[f->log2nfull]);
		// Copy to public buffer
		memcpy(f->outdata, f->trbuf, f->nfull * sizeof(float));
	#endif
}

void scfft_doifft(scfft *f){
	#if SC_FFT_FFTW
		float *trbuf = f->trbuf;
		size_t bytesize = f->nfull * sizeof(float);
		memcpy(trbuf, f->indata, bytesize);
		trbuf[1] = 0.f;
		trbuf[f->nfull] = f->indata[1];  // Nyquist goes all the way down to the end of the line...
		trbuf[f->nfull+1] = 0.f;

		fftwf_execute(f->plan);
		// NB the plan already includes copying data to f->outbuf

	#elif SC_FFT_VDSP
		vDSP_ctoz((COMPLEX*) f->indata, 2, &splitBuf, 1, f->nfull >> 1);
		vDSP_fft_zrip(fftSetup[f->log2nfull], &splitBuf, 1, f->log2nfull, FFT_INVERSE);
		vDSP_ztoc(&splitBuf, 1, (DSPComplex*) f->outdata, 2, f->nfull >> 1);
	#elif SC_FFT_GREEN
		float *trbuf = f->trbuf;
		size_t bytesize = f->nfull * sizeof(float);
		memcpy(trbuf, f->indata, bytesize);
		// Green FFT is in-place
		riffts(trbuf, f->log2nfull, 1, cosTable[f->log2nfull]);
		// Copy to public buffer
		memcpy(f->outdata, trbuf, f->nwin * sizeof(float));
	#endif
	scfft_dowindowing(f->outdata, f->nwin, f->nfull, f->log2nfull, f->wintype, f->scalefac);
}

void scfft_destroy(scfft *f){
	#if SC_FFT_FFTW
		fftwf_destroy_plan(f->plan);
	#endif
}
