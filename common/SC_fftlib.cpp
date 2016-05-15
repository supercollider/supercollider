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

NOTE:
vDSP uses a "SplitBuf" as an intermediate representation of the data.
For speed we keep this global, although this makes the code non-thread-safe.
(This is not new to this refactoring. Just worth noting.)
*/

#include "clz.h"
#include <stdlib.h>
#include <cmath>
#include <stdio.h>
#include <cstring>
#include <cassert>

#include "SC_fftlib.h"
#include "../server/supernova/utilities/malloc_aligned.hpp"

#ifdef NOVA_SIMD
#include "simd_binary_arithmetic.hpp"
#endif


// We include vDSP even if not using for FFT, since we want to use some vectorised add/mul tricks
#if defined(__APPLE__)
	#include <Accelerate/Accelerate.h>
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants and structs

// Decisions here about which FFT library to use.
// We include the relevant libs but also ensure that one, and only one, of them is active...
#ifdef SC_FFT_FFTW

#define SC_FFT_FFTW 1
#define SC_FFT_VDSP 0
#define SC_FFT_GREEN 0

#include <fftw3.h>

#elif defined(SC_FFT_GREEN)

#define SC_FFT_FFTW 0
#define SC_FFT_VDSP 0
#define SC_FFT_GREEN 1

extern "C" {
#include "fftlib.h"
}

#elif defined(__APPLE__)

#define SC_FFT_FFTW 0
#define SC_FFT_VDSP 1
#define SC_FFT_GREEN 0

#else

#define SC_FFT_FFTW 0
#define SC_FFT_VDSP 0
#define SC_FFT_GREEN 0

#endif


// This struct is a bit like FFTW's idea of a "plan": it represents an FFT operation that may be applied once or repeatedly.
// It should be possible for indata and outdata to be the same, for quasi-in-place operation.
typedef struct scfft {
	unsigned int nfull, nwin, log2nfull, log2nwin; // Lengths of full FFT frame, and the (possibly shorter) windowed data frame
	short wintype;
	float *indata, *outdata, *trbuf;
	float scalefac; // Used to rescale the data to unity gain
} scfft;


static float *fftWindow[2][SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1];

#if SC_FFT_VDSP
	static FFTSetup fftSetup[SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1]; // vDSP setups, one per FFT size
	static COMPLEX_SPLIT splitBuf; // Temp buf for holding rearranged data
#endif

#if SC_FFT_GREEN
	static float *cosTable[SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1];
#endif

#if SC_FFT_FFTW
static fftwf_plan precompiledForwardPlans[SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1];
static fftwf_plan precompiledBackwardPlans[SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1];
static fftwf_plan precompiledForwardPlansInPlace[SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1];
static fftwf_plan precompiledBackwardPlansInPlace[SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1];
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
	float *win = (float*)nova::malloc_aligned(size2 * sizeof(float));
	if (win == NULL)
		return NULL;

	double winc = twopi / size;
	for (int i=0; i<size2; ++i) {
		double w = i * winc;
		win[i] = cos(w);
	}
	return win;
}
#endif

static inline float* scfft_create_fftwindow(int wintype, int log2n)
{
	int size = 1 << log2n;

	float * win = (float*)nova::malloc_aligned(size * sizeof(float));

	if (!win) return NULL;

	double winc;
	switch (wintype) {
	case kSineWindow:
		winc = pi / size;
		for (int i=0; i<size; ++i) {
			double w = i * winc;
			win[i] = sin(w);
		}
		break;
	case kHannWindow:
		winc = twopi / size;
		for (int i=0; i<size; ++i) {
			double w = i * winc;
			win[i] = 0.5 - 0.5 * cos(w);
		}
		break;
	}

	return win;
}

static void scfft_ensurewindow(unsigned short log2_fullsize, unsigned short log2_winsize, short wintype);

static bool scfft_global_initialization (void)
{
	for (int wintype=0; wintype<2; ++wintype) {
		for (int i=0; i< SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1; ++i) {
			fftWindow[wintype][i] = 0;
		}
		for (int i= SC_FFT_LOG2_MINSIZE; i < SC_FFT_LOG2_MAXSIZE+1; ++i) {
			fftWindow[wintype][i] = scfft_create_fftwindow(wintype, i);
		}
	}
#if SC_FFT_GREEN
	for (int i=0; i< SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1; ++i) {
		cosTable[i] = 0;
	}
	for (int i= SC_FFT_LOG2_MINSIZE; i < SC_FFT_LOG2_MAXSIZE+1; ++i) {
		cosTable[i] = create_cosTable(i);
	}
	printf("SC FFT global init: cosTable initialised.\n");
#elif SC_FFT_VDSP
	// vDSP inits its twiddle factors
	for (int i= SC_FFT_LOG2_MINSIZE; i < SC_FFT_LOG2_MAXSIZE+1; ++i) {
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
	size_t maxSize = 1<<SC_FFT_LOG2_MAXSIZE;
	float * buffer1 = (float*)fftwf_malloc((maxSize + 1) * sizeof(float));
	float * buffer2 = (float*)fftwf_malloc((maxSize + 1) * sizeof(float));
	for (int i= SC_FFT_LOG2_MINSIZE; i < SC_FFT_LOG2_MAXSIZE+1; ++i) {
		size_t currentSize = 1<<i;

		precompiledForwardPlans[i]  = fftwf_plan_dft_r2c_1d(currentSize, buffer1, (fftwf_complex*) buffer2, FFTW_ESTIMATE);
		precompiledBackwardPlans[i] = fftwf_plan_dft_c2r_1d(currentSize, (fftwf_complex*) buffer2, buffer1, FFTW_ESTIMATE);

		precompiledForwardPlansInPlace[i]  = fftwf_plan_dft_r2c_1d(currentSize, buffer1, (fftwf_complex*) buffer1, FFTW_ESTIMATE);
		precompiledBackwardPlansInPlace[i] = fftwf_plan_dft_c2r_1d(currentSize, (fftwf_complex*) buffer1, buffer1, FFTW_ESTIMATE);
	}
	fftwf_free(buffer1);
	fftwf_free(buffer2);
#endif
	return false;
}

static bool dummy = scfft_global_initialization();

// You need to provide an intermediate "transform buffer". Size will depend on which underlying lib is being used.
// "fullsize" is the number of samples in the input buffer (inc any padding), aka the number of "points" in the FFT.
//   Often in an SC plugin you can get this number from buf->samples if you're grabbing an external buffer.
// The input value is given in samples.
// The return value is given in bytes.
static size_t scfft_trbufsize(unsigned int fullsize)
{
#if SC_FFT_FFTW
	// Transform buf is two floats "too big" because of FFTWF's output ordering
	return (fullsize+2) * sizeof(float);
#else
	// vDSP packs the nyquist in with the DC, so size is same as input buffer (plus zeropadding)
	// Green does this too
	return (fullsize  ) * sizeof(float);
#endif
}

static int largest_log2n = SC_FFT_LOG2_MAXSIZE;
static int largest_fftsize = 1 << largest_log2n;

scfft * scfft_create(size_t fullsize, size_t winsize, SCFFT_WindowFunction wintype,
					 float *indata, float *outdata, SCFFT_Direction forward, SCFFT_Allocator & alloc)
{
	if ( (fullsize > SC_FFT_ABSOLUTE_MAXSIZE) || (fullsize < SC_FFT_MINSIZE) )
		return NULL;

	const int alignment = 128; // in bytes
	char * chunk = (char*) alloc.alloc(sizeof(scfft) + scfft_trbufsize(fullsize) + alignment);
	if (!chunk)
		return NULL;

	scfft * f = (scfft*)chunk;
	float *trbuf = (float*)(chunk + sizeof(scfft));
	trbuf = (float*) ((size_t)((char*)trbuf + (alignment - 1)) & -alignment); // FIXME: should be intptr_t instead of size_t once we use c++11

#ifdef NOVA_SIMD
	assert(nova::vec<float>::is_aligned(trbuf));
#endif

	f->nfull = fullsize;
	f->nwin  =  winsize;
	f->log2nfull = LOG2CEIL(fullsize);
	f->log2nwin  = LOG2CEIL( winsize);
	f->wintype = wintype;
	f->indata  = indata;
	f->outdata = outdata;
	f->trbuf   = trbuf;

	// Buffer is larger than the range of sizes we provide for at startup; we can get ready just-in-time though
	if (fullsize > largest_fftsize){
		scfft_ensurewindow(f->log2nfull, f->log2nwin, wintype);
	}

	// The scale factors rescale the data to unity gain. The old Green lib did this itself, meaning scalefacs would here be 1...
	if (forward) {
#if SC_FFT_VDSP
		f->scalefac = 0.5f;
#else // forward FFTW and Green factor
		f->scalefac = 1.f;
#endif
	} else { // backward FFTW and VDSP factor
#if SC_FFT_GREEN
		f->scalefac = 1.f;
#else  // fftw, vdsp
		f->scalefac = 1.f / fullsize;
#endif
	}

	memset(trbuf, 0, scfft_trbufsize(fullsize));

	return f;
}



// check the global list of windows incs ours; create if not.
// Note that expanding the table, if triggered, will cause a CPU hit as things are malloc'ed, realloc'ed, etc.
void scfft_ensurewindow(unsigned short log2_fullsize, unsigned short log2_winsize, short wintype)
{
	// Ensure we have enough space to do our calcs
	int old_log2n = largest_log2n;
	if(log2_fullsize > largest_log2n){
		largest_log2n = log2_fullsize;
		largest_fftsize = 1 << largest_log2n;
#if SC_FFT_VDSP
		size_t newsize = (1 << largest_log2n) * sizeof(float) / 2;
		splitBuf.realp = (float*) realloc (splitBuf.realp, newsize);
		splitBuf.imagp = (float*) realloc (splitBuf.imagp, newsize);
#endif
	}
#if SC_FFT_FFTW
	size_t maxSize = 1<<largest_log2n;
	float * buffer1 = (float*)fftwf_malloc((maxSize + 1) * sizeof(float));
	float * buffer2 = (float*)fftwf_malloc((maxSize + 1) * sizeof(float));
	for (int i= old_log2n; i < largest_log2n+1; ++i) {
		size_t currentSize = 1<<i;

		precompiledForwardPlans[i]  = fftwf_plan_dft_r2c_1d(currentSize, buffer1, (fftwf_complex*) buffer2, FFTW_ESTIMATE);
		precompiledBackwardPlans[i] = fftwf_plan_dft_c2r_1d(currentSize, (fftwf_complex*) buffer2, buffer1, FFTW_ESTIMATE);

		precompiledForwardPlansInPlace[i]  = fftwf_plan_dft_r2c_1d(currentSize, buffer1, (fftwf_complex*) buffer1, FFTW_ESTIMATE);
		precompiledBackwardPlansInPlace[i] = fftwf_plan_dft_c2r_1d(currentSize, (fftwf_complex*) buffer1, buffer1, FFTW_ESTIMATE);
	}
	fftwf_free(buffer1);
	fftwf_free(buffer2);
#endif
	// Ensure our window has been created
	if ( (wintype != kRectWindow) && (fftWindow[wintype][log2_winsize] == 0)){
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

// these do the main jobs.
static void scfft_dowindowing(float *data, unsigned int winsize, unsigned int fullsize, unsigned short log2_winsize,
							  short wintype, float scalefac)
{
	if (wintype != kRectWindow) {
		float *win = fftWindow[wintype][log2_winsize];
		if (!win){
			//return;
			win = fftWindow[wintype][log2_winsize] = scfft_create_fftwindow(wintype, log2_winsize);
		}
#if SC_FFT_VDSP
		vDSP_vmul(data, 1, win, 1, data, 1, winsize);
#elif defined (NOVA_SIMD)
		using namespace nova;
		if (((vec<float>::objects_per_cacheline & winsize) == 0) && vec<float>::is_aligned(data))
			times_vec_simd(data, data, win, winsize);
		else
			times_vec(data, data, win, winsize);
#else
		--win;
		float *in = data - 1;
		for (int i=0; i< winsize ; ++i) {
			*++in *= *++win;
		}
#endif
	}

	// scale factor is different for different libs. But the compiler switch here is about using vDSP's fast multiplication method.
	if (scalefac != 1.f) {
#if SC_FFT_VDSP
		vDSP_vsmul(data, 1, &scalefac, data, 1, winsize);
#else
		for(unsigned int i=0; i<winsize; ++i)
			data[i] *= scalefac;
#endif
	}

	// Zero-padding:
	memset(data + winsize, 0, (fullsize - winsize) * sizeof(float));
}

void scfft_dofft(scfft * f)
{
	// Data goes to transform buf
	memcpy(f->trbuf, f->indata, f->nwin * sizeof(float));
	scfft_dowindowing(f->trbuf, f->nwin, f->nfull, f->log2nwin, f->wintype, f->scalefac);
#if SC_FFT_FFTW
	// forward transformation is in-place
	fftwf_execute_dft_r2c(precompiledForwardPlansInPlace[f->log2nfull], f->trbuf, (fftwf_complex*)f->trbuf);

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

void scfft_doifft(scfft * f)
{
#if SC_FFT_FFTW
	float *trbuf = f->trbuf;
	size_t bytesize = f->nfull * sizeof(float);
	memcpy(trbuf, f->indata, bytesize);
	trbuf[1] = 0.f;
	trbuf[f->nfull] = f->indata[1];  // Nyquist goes all the way down to the end of the line...
	trbuf[f->nfull+1] = 0.f;

	fftwf_execute_dft_c2r(precompiledBackwardPlans[f->log2nfull], (fftwf_complex*)trbuf, f->outdata);

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
	scfft_dowindowing(f->outdata, f->nwin, f->nfull, f->log2nwin, f->wintype, f->scalefac);
}

void scfft_destroy(scfft *f, SCFFT_Allocator & alloc)
{
	alloc.free(f);
}
