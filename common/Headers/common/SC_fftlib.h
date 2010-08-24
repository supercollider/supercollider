/*
SC_fftlib.h
An interface to abstract over different FFT libraries, for SuperCollider 3.
Copyright (c) 2008 Dan Stowell. All rights reserved.

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

#ifndef _SC_fftlib_
#define _SC_fftlib_

#include <cstring>

////////////////////////////////////////////////////////////////////////////////////////////////////
// Constants and structs

// Decisions here about which FFT library to use - vDSP only exists on Mac BTW.
// We include the relevant libs but also ensure that one, and only one, of them is active...
#if SC_FFT_NONE
	// "SC_FFT_NONE" allows compilation without FFT support; only expected to be used on very limited platforms
	#define SC_FFT_FFTW 0
	#define SC_FFT_VDSP 0
	#define SC_FFT_GREEN 0
#elif SC_FFT_GREEN
	#define SC_FFT_FFTW 0
	#define SC_FFT_VDSP 0
	#define SC_FFT_GREEN 1
#elif !SC_FFT_FFTW && defined(__APPLE__)
	#define SC_FFT_FFTW 0
	#define SC_FFT_VDSP 1
	#define SC_FFT_GREEN 0
#else
//#elif SC_FFT_FFTW
	#define SC_FFT_FFTW 1
	#define SC_FFT_VDSP 0
	#define SC_FFT_GREEN 0
	#include <fftw3.h>
#endif

// These specify the min & max FFT sizes expected (used when creating windows, also allocating some other arrays).
#define SC_FFT_MINSIZE 8
#define SC_FFT_LOG2_MINSIZE 3
#define SC_FFT_MAXSIZE 8192
#define SC_FFT_LOG2_MAXSIZE 13
// Note that things like *fftWindow actually allow for other sizes, to be created on user request.
#define SC_FFT_ABSOLUTE_MAXSIZE 2147483648
#define SC_FFT_LOG2_ABSOLUTE_MAXSIZE 31
#define SC_FFT_LOG2_ABSOLUTE_MAXSIZE_PLUS1 32

// These values are referred to from SC lang as well as in the following code - do not rearrange!
#define WINDOW_RECT -1
#define WINDOW_SINE 0
#define WINDOW_HANN 1

// This struct is a bit like FFTW's idea of a "plan": it represents an FFT operation that may be applied once or repeatedly.
// It should be possible for indata and outdata to be the same, for quasi-in-place operation.
typedef struct scfft{
	unsigned int nfull, nwin, log2nfull, log2nwin; // Lengths of full FFT frame, and the (possibly shorter) windowed data frame
	short wintype;
	float *indata, *outdata, *trbuf;
	float scalefac; // Used to rescale the data to unity gain
	#if SC_FFT_FFTW
		fftwf_plan plan;
	#endif
} scfft;

////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions

// called once on process launch
void scfft_global_init();

// You need to provide an intermediate "transform buffer". Size will depend on which underlying lib is being used.
// "fullsize" is the number of samples in the input buffer (inc any padding), aka the number of "points" in the FFT.
//   Often in an SC plugin you can get this number from buf->samples if you're grabbing an external buffer.
// The input value is given in samples.
// The return value is given in bytes.
size_t scfft_trbufsize(unsigned int fullsize);

// To initialise a specific FFT, ensure your input and output buffers exist, then
// allocate memory for the transform buffer and the scfft struct, then call this function.
// Both "fullsize" and "winsize" should be powers of two (this is not checked internally).
int scfft_create(scfft *f, unsigned int fullsize, unsigned int winsize, short wintype, float *indata, float *outdata, float *trbuf, bool forward);

// check the global list of windows incs ours; create if not.
// Note that expanding the table, if triggered, will cause a CPU hit as things are malloc'ed, realloc'ed, etc.
void scfft_ensurewindow(unsigned short log2_fullsize, unsigned short log2_winsize, short wintype);

// these do the main jobs.
// Note: you DON"T need to call the windowing function yourself, it'll be applied by the _dofft and _doifft funcs.
void scfft_dowindowing(float *data, unsigned int winsize, unsigned int fullsize, unsigned short log2_winsize, short wintype, float scalefac);
// These two will take data from indata, use trbuf to process it, and put their results in outdata.
void scfft_dofft(scfft *f);
void scfft_doifft(scfft *f);

// destroy any resources held internally. Note that the memory associated with indata/outdata/trbuf are for you to destroy.
void scfft_destroy(scfft *f);

#endif
