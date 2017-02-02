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

//Convolution by nick collins for sc3 (later input from Marije as below, Dan Stowell and Nick worked on sc_fft compatibility late 2008)
//see ch18 http://www.dspguide.com/ch18.htm Steven W Smith

//Convolution2 adapted by marije baalman for triggered kernel swap, with help from alberto de campo
//Convolution2L (with linear crossfade) by marije baalman
//Convolution3 (time-based) by marije baalman

#include "FFT_UGens.h"

//#include "SC_fftlib.h"

#include <stdio.h>


struct Convolution : Unit
{
	int m_pos, m_framesize, m_fftsize;
	float *m_inbuf1,*m_inbuf2, *m_fftbuf1, *m_fftbuf2, *m_outbuf,*m_overlapbuf;
	scfft *m_scfft1, *m_scfft2, *m_scfftR;
};


struct Convolution2 : Unit
{
	int m_pos, m_framesize, m_fftsize;
	float m_prevtrig;
	float *m_inbuf1, *m_fftbuf1, *m_fftbuf2, *m_outbuf,*m_overlapbuf;
	scfft *m_scfft1, *m_scfft2, *m_scfftR;
};

struct Convolution2L : Unit
{
	int m_pos, m_framesize, m_fftsize;
	int m_cfpos, m_cflength, m_curbuf;	// for crossfading
	int m_log2n;
	float m_prevtrig;
	float *m_inbuf1, *m_fftbuf1, *m_fftbuf2, *m_outbuf,*m_overlapbuf;
	float *m_tempbuf, *m_fftbuf3;		// for crossfading

	scfft *m_scfft1, *m_scfft2, *m_scfft3, *m_scfftR, *m_scfftR2; //source plus two kernels forwards, and two inverse from outbuf and from tempbuf
};


//could be done also using complex signal fft and appropriate unwrapping, but sc_fft doesn't support that
struct StereoConvolution2L : Unit
{
	int m_pos, m_framesize, m_fftsize;
	int m_cfpos, m_cflength, m_curbuf;	// for crossfading
	int m_log2n;
	float m_prevtrig;
	float *m_inbuf1, *m_fftbuf1; // input
	float *m_fftbuf2[2], *m_outbuf[2],*m_overlapbuf[2]; // output
	float *m_tempbuf[2], *m_fftbuf3[2];		// for crossfading

	scfft *m_scfft1, *m_scfft2[2], *m_scfft3[2], *m_scfftR[2], *m_scfftR2[2];
};

struct Convolution3 : Unit
{
	int m_pos, m_framesize;
	float m_prevtrig;
	float *m_inbuf1, *m_inbuf2, *m_outbuf;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void Convolution_next(Convolution *unit, int wrongNumSamples);
	void Convolution_Ctor(Convolution *unit);
	void Convolution_Dtor(Convolution *unit);

	void Convolution2_next(Convolution2 *unit, int wrongNumSamples);
	void Convolution2_Ctor(Convolution2 *unit);
	void Convolution2_Dtor(Convolution2 *unit);

	void Convolution2L_next(Convolution2L *unit, int wrongNumSamples);
	void Convolution2L_Ctor(Convolution2L *unit);
	void Convolution2L_Dtor(Convolution2L *unit);

	void StereoConvolution2L_next(StereoConvolution2L *unit, int wrongNumSamples);
	void StereoConvolution2L_Ctor(StereoConvolution2L *unit);
	void StereoConvolution2L_Dtor(StereoConvolution2L *unit);

	void Convolution3_next_a(Convolution3 *unit);
	void Convolution3_next_k(Convolution3 *unit);
	void Convolution3_Ctor(Convolution3 *unit);
	void Convolution3_Dtor(Convolution3 *unit);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

//PROPER CONVOLVER
//two possibilities- fixed kernel (in which case can derive the kernel spectrum in the constructor)
//and changing kernel (same size as target)

void Convolution_Ctor(Convolution *unit)
{
	//require size N+M-1 to be a power of two
	unit->m_framesize=(int)ZIN0(2);

	unit->m_fftsize=2*(unit->m_framesize);
	//just use memory for the input buffers and fft buffers
	int framesize_f = unit->m_framesize * sizeof(float);
	int fftsize_f = unit->m_fftsize * sizeof(float);

	unit->m_inbuf1 = (float*)RTAlloc(unit->mWorld, framesize_f);
	unit->m_inbuf2 = (float*)RTAlloc(unit->mWorld, framesize_f);
	unit->m_fftbuf1 = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_fftbuf2 = (float*)RTAlloc(unit->mWorld, fftsize_f);

	unit->m_outbuf = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_overlapbuf = (float*)RTAlloc(unit->mWorld, framesize_f);

	memset(unit->m_outbuf, 0, fftsize_f);
	memset(unit->m_overlapbuf, 0, framesize_f);

	unit->m_pos = 0;

	SCWorld_Allocator alloc(ft, unit->mWorld);
	unit->m_scfft1 = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf1, unit->m_fftbuf1, kForward, alloc);
	unit->m_scfft2 = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf2, unit->m_fftbuf2, kForward, alloc);
	unit->m_scfftR = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf1, unit->m_outbuf, kBackward, alloc);

	SETCALC(Convolution_next);

	// initialize output
	OUT0(0) = IN0(0);
}


void Convolution_Dtor(Convolution *unit)
{
	RTFree(unit->mWorld, unit->m_inbuf1);
	RTFree(unit->mWorld, unit->m_inbuf2);
	RTFree(unit->mWorld, unit->m_fftbuf1);
	RTFree(unit->mWorld, unit->m_fftbuf2);
	RTFree(unit->mWorld, unit->m_outbuf);
	RTFree(unit->mWorld, unit->m_overlapbuf);
	SCWorld_Allocator alloc(ft, unit->mWorld);
	scfft_destroy(unit->m_scfft1, alloc);
	scfft_destroy(unit->m_scfft2, alloc);
	scfft_destroy(unit->m_scfftR, alloc);
}


void Convolution_next(Convolution *unit, int numSamples)
{
	float *in1 = IN(0);
	float *in2 = IN(1);

	float *out1 = unit->m_inbuf1 + unit->m_pos;
	float *out2 = unit->m_inbuf2 + unit->m_pos;

	//int numSamples = unit->mWorld->mFullRate.mBufLength;

	// copy input
	Copy(numSamples, out1, in1);
	Copy(numSamples, out2, in2);

	unit->m_pos += numSamples;

	int framesize= unit->m_framesize;

	if (unit->m_pos & framesize) {
        //have collected enough samples to transform next frame
		unit->m_pos = 0; //reset collection counter

		int memsize= framesize*sizeof(float);

	        // copy to fftbuf
		memcpy(unit->m_fftbuf1, unit->m_inbuf1, memsize);
		memcpy(unit->m_fftbuf2, unit->m_inbuf2, memsize);

		//zero pad second part of buffer to allow for convolution
		memset(unit->m_fftbuf1+unit->m_framesize, 0, memsize);
		memset(unit->m_fftbuf2+unit->m_framesize, 0, memsize);

		// do fft
		//in place transform for now
		scfft_dofft(unit->m_scfft1);
		scfft_dofft(unit->m_scfft2);

		//complex multiply time
		float * p1= unit->m_fftbuf1;
		float * p2= unit->m_fftbuf2;

		p1[0] *= p2[0];
		p1[1] *= p2[1];

		//complex multiply
		for (int i=1; i<framesize; ++i) {
			float real,imag;
			int realind,imagind;
			realind= 2*i; imagind= realind+1;
			real= p1[realind]*p2[realind]- p1[imagind]*p2[imagind];
			imag= p1[realind]*p2[imagind]+ p1[imagind]*p2[realind];

			p1[realind] = real;
			p1[imagind]= imag;
		}

		//copy second part from before to overlap
		memcpy(unit->m_overlapbuf, unit->m_outbuf+unit->m_framesize, memsize);

		//inverse fft into outbuf
		memcpy(unit->m_outbuf, unit->m_fftbuf1, unit->m_fftsize * sizeof(float));

		//in place
		scfft_doifft(unit->m_scfftR);
	}

	//write out samples copied from outbuf, with overlap added in

	float *output = ZOUT(0);
	float *out= unit->m_outbuf+unit->m_pos;
	float *overlap= unit->m_overlapbuf+unit->m_pos;

	for (int i=0; i<numSamples; ++i)
		ZXP(output) = out[i] + overlap[i];
}



//include local buffer test in one place
static SndBuf * ConvGetBuffer(Unit * unit, uint32 bufnum, const char * ugenName, int inNumSamples)
{
	SndBuf *buf;
	World *world = unit->mWorld;

	if (bufnum >= world->mNumSndBufs) {
		int localBufNum = bufnum - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if (localBufNum <= parent->localMaxBufNum) {
			buf = parent->mLocalSndBufs + localBufNum;
		} else {
			if (unit->mWorld->mVerbosity > -1)
				Print("%s: invalid buffer number (%d).\n", ugenName, bufnum);
			goto handle_failure;
		}
	} else {
		buf = world->mSndBufs + bufnum;
	}

	if (buf->data == NULL) {
		if (unit->mWorld->mVerbosity > -1)
			Print("%s: uninitialized buffer (%i).\n", ugenName, bufnum);
		goto handle_failure;
	}

	return buf;

handle_failure:
	SETCALC(*ClearUnitOutputs);
	ClearUnitOutputs(unit, inNumSamples);
	unit->mDone = true;
	return NULL;
}

void Convolution2_Ctor(Convolution2 *unit)
{
	//require size N+M-1 to be a power of two
	unit->m_framesize=(int)ZIN0(3);
	uint32 kernelbufnum = (int)ZIN0(1);
	World *world = unit->mWorld;

	SndBuf *kernelbuf = ConvGetBuffer(unit, kernelbufnum, "Convolution2", 1);

	if(kernelbuf) {
		if ( unit->m_framesize <= 0 ){ // if smaller than zero, we use the size of the buffer
			unit->m_framesize=kernelbuf->frames;
		}

		unit->m_fftsize=2*(unit->m_framesize);

		if(unit->m_fftsize > SC_FFT_ABSOLUTE_MAXSIZE){
			printf( "Convolution2: FFT size is larger than SC_FFT_ABSOLUTE_MAXSIZE, cannot run. We suggest PartConv instead.\n" );
			SETCALC(*ClearUnitOutputs);
		}

		// allocate memory internally for the input buffers and fft buffers
		int framesize_f = unit->m_framesize * sizeof(float);
		int fftsize_f = unit->m_fftsize * sizeof(float);
		unit->m_inbuf1 = (float*)RTAlloc(world, framesize_f);
		unit->m_fftbuf1 = (float*)RTAlloc(world, fftsize_f);
		unit->m_fftbuf2 = (float*)RTAlloc(world, fftsize_f);

		unit->m_outbuf = (float*)RTAlloc(world, fftsize_f);
		memset(unit->m_outbuf, 0, fftsize_f);
		unit->m_overlapbuf = (float*)RTAlloc(world, framesize_f);
		memset(unit->m_overlapbuf, 0, framesize_f);

		unit->m_pos = 0;

		SCWorld_Allocator alloc(ft, unit->mWorld);
		unit->m_scfft1 = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf1, unit->m_fftbuf1, kForward, alloc);
		unit->m_scfft2 = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf2, unit->m_fftbuf2, kForward, alloc);
		unit->m_scfftR = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf1, unit->m_outbuf, kBackward, alloc);
		if(!unit->m_scfft1 || !unit->m_scfft2 || !unit->m_scfftR){
			printf( "Could not create scfft.\n" );
			SETCALC(*ClearUnitOutputs);
			unit->mDone = true;
			return;
		}
		//calculate fft for kernel straight away
		// we cannot use a kernel larger than the fft size, so truncate if needed. the kernel may be smaller though.
		uint32 framesize = unit->m_framesize;
		size_t kernelcopysize = sc_min(kernelbuf->frames, framesize);
		memcpy(unit->m_fftbuf2, kernelbuf->data, kernelcopysize * sizeof(float));
		//zero pad second part of buffer to allow for convolution
		memset(unit->m_fftbuf2 + kernelcopysize, 0, (2 * framesize - kernelcopysize) * sizeof(float));

		scfft_dofft(unit->m_scfft2);

		unit->m_pos = 0;

		unit->m_prevtrig = 0.f;
		unit->m_prevtrig = ZIN0(2);

		if ( unit->m_framesize >= world->mFullRate.mBufLength ) {
			SETCALC(Convolution2_next);

			// initialize output
			OUT0(0) = IN0(0);
		} else {
			printf( "Convolution2 framesize smaller than blocksize \n" );
			SETCALC(*ClearUnitOutputs);
			unit->mDone = true;
		}
	} else {
		unit->m_scfft2 = unit->m_scfft1 = unit->m_scfftR = NULL;
		printf("Convolution2_Ctor: can't get kernel buffer, giving up.\n");
		SETCALC(*ClearUnitOutputs);
	}

}

void Convolution2_Dtor(Convolution2 *unit)
{
	RTFree(unit->mWorld, unit->m_inbuf1);
	RTFree(unit->mWorld, unit->m_fftbuf1);
	RTFree(unit->mWorld, unit->m_fftbuf2);
	RTFree(unit->mWorld, unit->m_outbuf);
	RTFree(unit->mWorld, unit->m_overlapbuf);

	SCWorld_Allocator alloc(ft, unit->mWorld);
	scfft_destroy(unit->m_scfft1, alloc);
	scfft_destroy(unit->m_scfft2, alloc);
	scfft_destroy(unit->m_scfftR, alloc);
}


void Convolution2_next(Convolution2 *unit, int wrongNumSamples)
{
	float *in1 = IN(0);
	float curtrig = ZIN0(2);

	float *inbuf1writepos = unit->m_inbuf1 + unit->m_pos;

	int numSamples = unit->mWorld->mFullRate.mBufLength;
	uint32 framesize = unit->m_framesize;
	uint32 framesize_f = framesize * sizeof(float);

	// copy input
	Copy(numSamples, inbuf1writepos, in1);

	unit->m_pos += numSamples;

	if (unit->m_prevtrig <= 0.f && curtrig > 0.f){
		SndBuf *kernelbuf = ConvGetBuffer(unit,(uint32)ZIN0(1), "Convolution2", numSamples);
		if (!kernelbuf)
			return;
		LOCK_SNDBUF_SHARED(kernelbuf);

		// we cannot use a kernel larger than the fft size, so truncate if needed. the kernel may be smaller though.
		size_t kernelcopysize = sc_min(kernelbuf->frames, framesize);
		memcpy(unit->m_fftbuf2, kernelbuf->data, kernelcopysize * sizeof(float));
		memset(unit->m_fftbuf2 + kernelcopysize, 0, (2 * framesize - kernelcopysize) * sizeof(float));

		scfft_dofft(unit->m_scfft2);
	}

	if (unit->m_pos >= framesize) {
		//have collected enough samples to transform next frame
		unit->m_pos = 0; //reset collection counter

		// copy to fftbuf
		memcpy(unit->m_fftbuf1, unit->m_inbuf1, framesize_f);

		//zero pad second part of buffer to allow for convolution
		memset(unit->m_fftbuf1+unit->m_framesize, 0, framesize_f);

		scfft_dofft(unit->m_scfft1);

		//complex multiply time
		int numbins = unit->m_fftsize >> 1;

		float * p1= unit->m_fftbuf1;
		float * p2= unit->m_fftbuf2;

		p1[0] *= p2[0];
		p1[1] *= p2[1];

		//complex multiply
		for (int i=1; i<numbins; ++i) {
			float real,imag;
			int realind,imagind;
			realind= 2*i;
			imagind= realind+1;
			real= p1[realind]*p2[realind]- p1[imagind]*p2[imagind];
			imag= p1[realind]*p2[imagind]+ p1[imagind]*p2[realind];
			p1[realind] = real;
			p1[imagind]= imag;
		}

		//copy second part from before to overlap
		memcpy(unit->m_overlapbuf, unit->m_outbuf+unit->m_framesize, framesize_f);
		//inverse fft into outbuf
		scfft_doifft(unit->m_scfftR);
	}

	//write out samples copied from outbuf, with overlap added in

	float *output = ZOUT(0);
	float *out= unit->m_outbuf+unit->m_pos;
	float *overlap= unit->m_overlapbuf+unit->m_pos;
	unit->m_prevtrig = curtrig;

	for (int i=0; i<numSamples; ++i)
		ZXP(output) = out[i] + overlap[i];
}

void Convolution2L_Ctor(Convolution2L *unit)
{
	//require size N+M-1 to be a power of two
	unit->m_framesize=(int)ZIN0(3);
	unit->m_cflength = (int)ZIN0(4);
	unit->m_curbuf = 0;
	unit->m_cfpos = unit->m_cflength;


	unit->m_fftsize=2*(unit->m_framesize);
	//just use memory for the input buffers and fft buffers
	int framesize_f = unit->m_framesize * sizeof(float);
	int fftsize_f = unit->m_fftsize * sizeof(float);

	unit->m_inbuf1 = (float*)RTAlloc(unit->mWorld, framesize_f);

	unit->m_fftbuf1 = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_fftbuf2 = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_fftbuf3 = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_tempbuf = (float*)RTAlloc(unit->mWorld, fftsize_f);

	uint32 bufnum = (int)ZIN0(1); //fbufnum;

	SndBuf *buf = ConvGetBuffer(unit, bufnum, "Convolution2L", 1);

	if(buf) {
		unit->m_outbuf = (float*)RTAlloc(unit->mWorld, fftsize_f);
		unit->m_overlapbuf = (float*)RTAlloc(unit->mWorld, framesize_f);

		memset(unit->m_outbuf, 0, fftsize_f);
		memset(unit->m_overlapbuf, 0, framesize_f);

		//calculate fft for kernel straight away
		memcpy(unit->m_fftbuf2, buf->data, framesize_f);
		//zero pad second part of buffer to allow for convolution
		memset(unit->m_fftbuf2+unit->m_framesize, 0, framesize_f);

		SCWorld_Allocator alloc(ft, unit->mWorld);
		unit->m_scfft1 = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf1, unit->m_fftbuf1, kForward, alloc);
		unit->m_scfft2 = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf2, unit->m_fftbuf2, kForward, alloc);
		unit->m_scfft3 = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf3, unit->m_fftbuf3, kForward, alloc);
		unit->m_scfftR = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_outbuf, unit->m_outbuf, kBackward, alloc);
		unit->m_scfftR2 = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_tempbuf, unit->m_tempbuf, kBackward, alloc);

		scfft_dofft(unit->m_scfft2);

		unit->m_pos = 0;
		unit->m_prevtrig = 0.f;

		SETCALC(Convolution2L_next);

		// initialize output
		OUT0(0) = IN0(0);
	} else {
		unit->m_scfft1 = unit->m_scfft2 = unit->m_scfft3 = unit->m_scfftR = unit->m_scfftR2 = NULL;
	}
}

void Convolution2L_Dtor(Convolution2L *unit)
{
	SCWorld_Allocator alloc(ft, unit->mWorld);
	scfft_destroy(unit->m_scfft1, alloc);
	scfft_destroy(unit->m_scfft2, alloc);
	scfft_destroy(unit->m_scfft3, alloc);
	scfft_destroy(unit->m_scfftR, alloc);
	scfft_destroy(unit->m_scfftR2, alloc);


	RTFree(unit->mWorld, unit->m_inbuf1);
	RTFree(unit->mWorld, unit->m_fftbuf1);
	RTFree(unit->mWorld, unit->m_fftbuf2);
	RTFree(unit->mWorld, unit->m_fftbuf3);
	RTFree(unit->mWorld, unit->m_tempbuf);
	RTFree(unit->mWorld, unit->m_outbuf);
	RTFree(unit->mWorld, unit->m_overlapbuf);
}


void Convolution2L_next(Convolution2L *unit, int numSamples)
{
	float *in1 = IN(0);
	float curtrig = ZIN0(2);
	float *out1 = unit->m_inbuf1 + unit->m_pos;
	int framesize_f = unit->m_framesize * sizeof(float);

	// copy input
	memcpy(out1,in1,numSamples*sizeof(float));

	unit->m_pos += numSamples;

	if (unit->m_prevtrig <= 0.f && curtrig > 0.f){

		uint32 bufnum = (int)ZIN0(1);
		SndBuf *buf = ConvGetBuffer(unit, bufnum, "Convolution2L", numSamples);
		if (!buf)
			return;

		unit->m_cflength = (int)ZIN0(4);

		unit->m_cfpos = 0;
		if ( unit->m_curbuf == 1 )
		{
			LOCK_SNDBUF_SHARED(buf);
			memcpy(unit->m_fftbuf2, buf->data, framesize_f);
			memset(unit->m_fftbuf2+unit->m_framesize, 0, framesize_f);

			scfft_dofft(unit->m_scfft2);
		}
		else if ( unit->m_curbuf == 0 )
		{
			LOCK_SNDBUF_SHARED(buf);
			memcpy(unit->m_fftbuf3, buf->data, framesize_f);
			memset(unit->m_fftbuf3+unit->m_framesize, 0, framesize_f);

			scfft_dofft(unit->m_scfft3);
		}
	}

	if (unit->m_pos & unit->m_framesize) {
	        //have collected enough samples to transform next frame
		unit->m_pos = 0; //reset collection counter

	        // copy to fftbuf
		memcpy(unit->m_fftbuf1, unit->m_inbuf1, framesize_f);

		//zero pad second part of buffer to allow for convolution
		memset(unit->m_fftbuf1+unit->m_framesize, 0, framesize_f);
		//in place transform for now
		scfft_dofft(unit->m_scfft1);

		//rffts(unit->m_fftbuf1, log2n, 1, cosTable[log2n]);
		//complex multiply time
		int numbins = unit->m_fftsize >> 1; //unit->m_fftsize - 2 >> 1;

		float * p1= unit->m_fftbuf1;
		float * p2;
		if ( unit->m_curbuf == 0 )
			p2 = unit->m_fftbuf2;
		else
			p2= unit->m_fftbuf3;
		float * p3= unit->m_tempbuf;

		// multiply the dc and nyquist components
		p1[0] *= p2[0];
		p1[1] *= p2[1];
		//complex multiply
		for (int i=1; i<numbins; ++i)
		{
			float real,imag;
			int realind,imagind;
			realind= 2*i;
			imagind= realind+1;
			real= p1[realind]*p2[realind]- p1[imagind]*p2[imagind];
			imag= p1[realind]*p2[imagind]+ p1[imagind]*p2[realind];
			p3[realind] = real;
			p3[imagind]= imag;
		}

		//copy second part from before to overlap
		memcpy(unit->m_overlapbuf, unit->m_outbuf+unit->m_framesize, unit->m_framesize * sizeof(float));
		//inverse fft into outbuf
		memcpy(unit->m_outbuf, unit->m_tempbuf, unit->m_fftsize * sizeof(float));
		//in place
		scfft_doifft(unit->m_scfftR);

		if ( unit->m_cfpos < unit->m_cflength )	// do crossfade
		{
			if ( unit->m_curbuf == 0 )
				p2= unit->m_fftbuf3;
			else
				p2= unit->m_fftbuf2;

			// multiply the dc and nyquist components
			p1[0] *= p2[0];
			p1[1] *= p2[1];

			//complex multiply
			for (int i=1; i<numbins; ++i)
			{
				float real,imag;
				int realind,imagind;
				realind= 2*i; imagind= realind+1;
				real= p1[realind]*p2[realind]- p1[imagind]*p2[imagind];
				imag= p1[realind]*p2[imagind]+ p1[imagind]*p2[realind];
				p1[realind] = real; //p2->bin[i];
				p1[imagind]= imag;
			}

			//copy second part from before to overlap
			//inverse fft into tempbuf
			memcpy(unit->m_tempbuf, unit->m_fftbuf1, unit->m_fftsize * sizeof(float));
			scfft_doifft(unit->m_scfftR2);

			// now crossfade between outbuf and tempbuf
			float fact1 = (float) unit->m_cfpos/unit->m_cflength; // crossfade amount startpoint
			float rc = 1.f/(unit->m_cflength*unit->m_framesize);     //crossfade amount increase per sample
			float * p4 = unit->m_outbuf;
			float * p5 = unit->m_tempbuf;
			for ( int i=0; i < unit->m_framesize; i++ )
			{
				float res;
				res = (1-fact1)*p4[i] + fact1*p5[i];
				fact1 += rc;
				p4[i] = res;
			}
			if ( unit->m_cflength == 1 )
				memcpy(unit->m_outbuf+unit->m_framesize, unit->m_tempbuf+unit->m_framesize, framesize_f);
			else
			{
				for ( int i=unit->m_framesize+1; i < unit->m_fftsize; i++ )
				{
					float res = (1-fact1)*p4[i] + fact1*p5[i];
					fact1 += rc;
					p4[i] = res;
				}
			}
			unit->m_cfpos++;
			if ( unit->m_cfpos == unit->m_cflength ) // at end of crossfade, update the current buffer index
			{
				if ( unit->m_curbuf == 0 )
					unit->m_curbuf = 1;
				else
					unit->m_curbuf = 0;
			}
		}
	}

	//write out samples copied from outbuf, with overlap added in
	float *output = ZOUT(0);
	float *out= unit->m_outbuf+unit->m_pos;
	float *overlap= unit->m_overlapbuf+unit->m_pos;
	unit->m_prevtrig = curtrig;

	for (int i=0; i<numSamples; ++i)
		ZXP(output) = out[i] + overlap[i];
}

/** basically the same as Convolution2L, but takes a stereo buffer to convolve with and outputs a stereo signal */
void StereoConvolution2L_Ctor(StereoConvolution2L *unit)
{
	//require size N+M-1 to be a power of two
	unit->m_framesize=(int)ZIN0(4);
	unit->m_cflength = (int)ZIN0(5);
	unit->m_curbuf = 0;
	unit->m_cfpos = unit->m_cflength;

	unit->m_fftsize=2*(unit->m_framesize);
	//just use memory for the input buffers and fft buffers
	int framesize_f = unit->m_framesize * sizeof(float);
	int fftsize_f = unit->m_fftsize * sizeof(float);

	unit->m_inbuf1 = (float*)RTAlloc(unit->mWorld, framesize_f);

	// source:
	unit->m_fftbuf1 = (float*)RTAlloc(unit->mWorld, fftsize_f);

	// 2 channel buffer:
	unit->m_fftbuf2[0] = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_fftbuf2[1] = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_fftbuf3[0] = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_fftbuf3[1] = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_tempbuf[0] = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_tempbuf[1] = (float*)RTAlloc(unit->mWorld, fftsize_f);
	// 		unit->m_tempfftbuf[0] = (float*)RTAlloc(unit->mWorld, fftsize_f);
	// 		unit->m_tempfftbuf[1] = (float*)RTAlloc(unit->mWorld, fftsize_f);

	unit->m_outbuf[0] = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_overlapbuf[0] = (float*)RTAlloc(unit->mWorld, framesize_f);
	unit->m_outbuf[1] = (float*)RTAlloc(unit->mWorld, fftsize_f);
	unit->m_overlapbuf[1] = (float*)RTAlloc(unit->mWorld, framesize_f);

	memset(unit->m_outbuf[0], 0, fftsize_f);
	memset(unit->m_overlapbuf[0], 0, framesize_f);
	memset(unit->m_outbuf[1], 0, fftsize_f);
	memset(unit->m_overlapbuf[1], 0, framesize_f);

	SCWorld_Allocator alloc(ft, unit->mWorld);
	unit->m_scfft1 = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf1, unit->m_fftbuf1, kForward, alloc);
	unit->m_scfft2[0] = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf2[0], unit->m_fftbuf2[0], kForward, alloc);
	unit->m_scfft3[0] = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf2[1], unit->m_fftbuf2[1], kForward, alloc);
	unit->m_scfftR[0] = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf3[0], unit->m_fftbuf3[0], kForward, alloc);
	unit->m_scfftR2[0] = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_fftbuf3[1], unit->m_fftbuf3[1], kForward, alloc);
	unit->m_scfft2[1] = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_outbuf[0], unit->m_outbuf[0], kBackward, alloc);
	unit->m_scfft3[1] = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_tempbuf[0], unit->m_tempbuf[0], kBackward, alloc);
	unit->m_scfftR[1] = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_outbuf[1], unit->m_outbuf[1], kBackward, alloc);
	unit->m_scfftR2[1] = scfft_create(unit->m_fftsize, unit->m_fftsize, kRectWindow, unit->m_tempbuf[1], unit->m_tempbuf[1], kBackward, alloc);

	float fbufnum  = ZIN0(1);
	uint32 bufnumL = (int)fbufnum;
	fbufnum  = ZIN0(2);
	uint32 bufnumR = (int)fbufnum;
	//printf("bufnum %i \n", bufnum);

	//unit->m_log2n = LOG2CEIL(unit->m_fftsize);
    //int log2n = unit->m_log2n;

	SndBuf *buf = ConvGetBuffer(unit, bufnumL, "StereoConvolution2L", 1);

	if (buf) {
		LOCK_SNDBUF_SHARED(buf);

		//calculate fft for kernel straight away
		memcpy(unit->m_fftbuf2[0], buf->data, framesize_f);
		//zero pad second part of buffer to allow for convolution
		memset(unit->m_fftbuf2[0]+unit->m_framesize, 0, framesize_f);
		//in place transform for now
		scfft_dofft(unit->m_scfft2[0]);
	} else
		return;

	buf = ConvGetBuffer(unit, bufnumR, "StereoConvolution2L", 1);
	if (buf) {
		LOCK_SNDBUF(buf);

		//calculate fft for kernel straight away
		memcpy(unit->m_fftbuf2[1], buf->data, framesize_f);
		//zero pad second part of buffer to allow for convolution
		memset(unit->m_fftbuf2[1]+unit->m_framesize, 0, framesize_f);
		//in place transform for now
		scfft_dofft(unit->m_scfft2[1]);
		unit->m_pos = 0;
		unit->m_prevtrig = 0.f;

		SETCALC(StereoConvolution2L_next);

		// initialize outputs
		OUT0(0) = IN0(0);
		OUT0(1) = IN0(0);
	}
}

void StereoConvolution2L_Dtor(StereoConvolution2L *unit)
{
	SCWorld_Allocator alloc(ft, unit->mWorld);
	scfft_destroy(unit->m_scfft1, alloc);
	scfft_destroy(unit->m_scfft2[0], alloc);
	scfft_destroy(unit->m_scfft3[0], alloc);
	scfft_destroy(unit->m_scfftR[0], alloc);
	scfft_destroy(unit->m_scfftR2[0], alloc);
	scfft_destroy(unit->m_scfft2[1], alloc);
	scfft_destroy(unit->m_scfft3[1], alloc);
	scfft_destroy(unit->m_scfftR[1], alloc);
	scfft_destroy(unit->m_scfftR2[1], alloc);

	RTFree(unit->mWorld, unit->m_inbuf1);
	RTFree(unit->mWorld, unit->m_fftbuf1);
	RTFree(unit->mWorld, unit->m_fftbuf2[0]);
	RTFree(unit->mWorld, unit->m_fftbuf3[0]);
	RTFree(unit->mWorld, unit->m_tempbuf[0]);
	RTFree(unit->mWorld, unit->m_outbuf[0]);
	RTFree(unit->mWorld, unit->m_overlapbuf[0]);
	RTFree(unit->mWorld, unit->m_fftbuf2[1]);
	RTFree(unit->mWorld, unit->m_fftbuf3[1]);
	RTFree(unit->mWorld, unit->m_tempbuf[1]);
	RTFree(unit->mWorld, unit->m_outbuf[1]);
	RTFree(unit->mWorld, unit->m_overlapbuf[1]);
}


void StereoConvolution2L_next(StereoConvolution2L *unit, int wrongNumSamples)
{
	float *in1 = IN(0);
	float curtrig = ZIN0(3);

	float *out1 = unit->m_inbuf1 + unit->m_pos;

	int numSamples = unit->mWorld->mFullRate.mBufLength;
	uint32 framesize_f=unit->m_framesize * sizeof(float);

	// copy input
	Copy(numSamples, out1, in1);

	unit->m_pos += numSamples;

	if (unit->m_prevtrig <= 0.f && curtrig > 0.f){
		float fbufnum  = ZIN0(1);
		uint32 bufnumL = (int)fbufnum;
		fbufnum  = ZIN0(2);
		uint32 bufnumR = (int)fbufnum;
		unit->m_cflength = (int)ZIN0(5);

		SndBuf *bufL = ConvGetBuffer(unit, bufnumL, "StereoConvolution2L", numSamples);
		SndBuf *bufR = ConvGetBuffer(unit, bufnumR, "StereoConvolution2L", numSamples);

		if (!bufL || !bufL)
			return;

		unit->m_cfpos = 0;
		if ( unit->m_curbuf == 1 )
		{
			LOCK_SNDBUF2_SHARED(bufL, bufR);
			memcpy(unit->m_fftbuf2[0], bufL->data, framesize_f);
			memset(unit->m_fftbuf2[0]+unit->m_framesize, 0, framesize_f);
			scfft_dofft(unit->m_scfft2[0]);
			memcpy(unit->m_fftbuf2[1], bufR->data, framesize_f);
			memset(unit->m_fftbuf2[1]+unit->m_framesize, 0, framesize_f);
			scfft_dofft(unit->m_scfft2[1]);
		}
		else if ( unit->m_curbuf == 0 )
		{
			LOCK_SNDBUF2_SHARED(bufL, bufR);
			memcpy(unit->m_fftbuf3[0], bufL->data, framesize_f);
			memset(unit->m_fftbuf3[0]+unit->m_framesize, 0, framesize_f);
			scfft_dofft(unit->m_scfft3[0]);
			memcpy(unit->m_fftbuf3[1], bufR->data, framesize_f);
			memset(unit->m_fftbuf3[1]+unit->m_framesize, 0, framesize_f);
			scfft_dofft(unit->m_scfft3[1]);
		}
	}

	if (unit->m_pos & unit->m_framesize) {
		//have collected enough samples to transform next frame
		unit->m_pos = 0; //reset collection counter
		// copy to fftbuf
		memcpy(unit->m_fftbuf1, unit->m_inbuf1, framesize_f);

		//zero pad second part of buffer to allow for convolution
		memset(unit->m_fftbuf1+unit->m_framesize, 0, framesize_f);
		//in place transform for now
		scfft_dofft(unit->m_scfft1);

		//complex multiply time
		int numbins = unit->m_fftsize >> 1; //unit->m_fftsize - 2 >> 1;

		float * p1= unit->m_fftbuf1;
		float * p2L, * p2R;
		if ( unit->m_curbuf == 0 ){
			p2L = unit->m_fftbuf2[0];
			p2R = unit->m_fftbuf2[1];
		} else {
			p2L = unit->m_fftbuf3[0];
			p2R = unit->m_fftbuf3[1];
		}
		float * p3L = unit->m_tempbuf[0];
		float * p3R = unit->m_tempbuf[1];

		//complex multiply
		for (int i=1; i<numbins; ++i)
		{
			float realL,imagL;
			float realR,imagR;
			int realind,imagind;
			realind= 2*i; imagind= realind+1;
			realL= p1[realind]*p2L[realind]- p1[imagind]*p2L[imagind];
			imagL= p1[realind]*p2L[imagind]+ p1[imagind]*p2L[realind];
			realR= p1[realind]*p2R[realind]- p1[imagind]*p2R[imagind];
			imagR= p1[realind]*p2R[imagind]+ p1[imagind]*p2R[realind];
			p3L[realind] = realL; //p2->bin[i];
			p3L[imagind] = imagL;
			p3R[realind] = realR; //p2->bin[i];
			p3R[imagind] = imagR;
		}

		for ( int i=0; i < 2; i++ ){
			//copy second part from before to overlap
			memcpy(unit->m_overlapbuf[i], unit->m_outbuf[i]+unit->m_framesize, unit->m_framesize * sizeof(float));
			//inverse fft into outbuf
			memcpy(unit->m_outbuf[i], unit->m_tempbuf[i], unit->m_fftsize * sizeof(float));
			//in place
			scfft_doifft(unit->m_scfftR[i]);

		}


		if ( unit->m_cfpos < unit->m_cflength )	// do crossfade
		{
			if ( unit->m_curbuf == 0 ){
				p2L= unit->m_fftbuf3[0];
				p2R= unit->m_fftbuf3[1];
			} else {
				p2L= unit->m_fftbuf2[0];
				p2R= unit->m_fftbuf2[1];
			}

			//complex multiply
			for (int i=1; i<numbins; ++i)
			{
				float realL,imagL;
				float realR,imagR;
				int realind,imagind;
				realind= 2*i; imagind= realind+1;
				realL= p1[realind]*p2L[realind]- p1[imagind]*p2L[imagind];
				imagL= p1[realind]*p2L[imagind]+ p1[imagind]*p2L[realind];
				realR= p1[realind]*p2R[realind]- p1[imagind]*p2R[imagind];
				imagR= p1[realind]*p2R[imagind]+ p1[imagind]*p2R[realind];
				p3L[realind] = realL; //p2->bin[i];
				p3L[imagind] = imagL;
				p3R[realind] = realR; //p2->bin[i];
				p3R[imagind] = imagR;
			}

			scfft_doifft(unit->m_scfftR2[0]);
			scfft_doifft(unit->m_scfftR2[1]);

			// now crossfade between outbuf and tempbuf
			float fact1 = (float) unit->m_cfpos/unit->m_cflength;  // crossfade amount startpoint
			float rc = 1.f/(unit->m_cflength*unit->m_framesize);      //crossfade amount increase per sample
			float * p4L = unit->m_outbuf[0];
			float * p5L = unit->m_tempbuf[0];
			float * p4R = unit->m_outbuf[1];
			float * p5R = unit->m_tempbuf[1];
			for ( int i=0; i < unit->m_framesize; i++ )
			{
				float res;
				res = (1-fact1)*p4L[i] + fact1*p5L[i];
				p4L[i] = res;
				res = (1-fact1)*p4R[i] + fact1*p5R[i];
				p4R[i] = res;
				fact1 += rc;
			}
			if ( unit->m_cflength == 1 ) // in the overlap the convolution is already at the new buffer
			{
				memcpy(unit->m_outbuf[0]+unit->m_framesize, unit->m_tempbuf[0]+unit->m_framesize,unit->m_framesize * sizeof(float));
				memcpy(unit->m_outbuf[1]+unit->m_framesize, unit->m_tempbuf[1]+unit->m_framesize,unit->m_framesize * sizeof(float));
			}
			else
			{
				for ( int i=unit->m_framesize+1; i < unit->m_fftsize; i++ )
				{
					float res;
					res = (1-fact1)*p4L[i] + fact1*p5L[i];
					p4L[i] = res;
					res = (1-fact1)*p4R[i] + fact1*p5R[i];
					p4R[i] = res;
					fact1 += rc;
				}
			}
			unit->m_cfpos++;
			if ( unit->m_cfpos == unit->m_cflength ) // at end of crossfade, update the current buffer index
			{
				if ( unit->m_curbuf == 0 )
					unit->m_curbuf = 1;
				else
					unit->m_curbuf = 0;
			}
		}
	}

	//write out samples copied from outbuf, with overlap added in
	float *outputL = ZOUT(0);
	float *outputR = ZOUT(1);
	float *outL= unit->m_outbuf[0]+unit->m_pos;
	float *overlapL= unit->m_overlapbuf[0]+unit->m_pos;
	float *outR= unit->m_outbuf[1]+unit->m_pos;
	float *overlapR= unit->m_overlapbuf[1]+unit->m_pos;
	unit->m_prevtrig = curtrig;

	for (int i=0; i<numSamples; ++i) {
		ZXP(outputL) = outL[i] + overlapL[i];
		ZXP(outputR) = outR[i] + overlapR[i];
	}
}

void Convolution3_Ctor(Convolution3 *unit)
{
	unit->m_framesize=(int)ZIN0(3);

	float fbufnum  = ZIN0(1);
	uint32 bufnum = (int)fbufnum;

	SndBuf *buf = ConvGetBuffer(unit, bufnum, "Convolution3", 1);

	if (buf) {
		if ( unit->m_framesize <= 0 ) // if smaller than zero, equal to size of buffer
		{
			unit->m_framesize=buf->frames;
		}

		//just use memory for the input buffers and out buffer
		int framesize_f = unit->m_framesize * sizeof(float);

		unit->m_inbuf1 = (float*)RTAlloc(unit->mWorld, framesize_f);
		unit->m_inbuf2 = (float*)RTAlloc(unit->mWorld, framesize_f);

		LOCK_SNDBUF_SHARED(buf);
		//calculate fft for kernel straight away
		memcpy(unit->m_inbuf2, buf->data, framesize_f);
		unit->m_pos = 0;

		unit->m_outbuf = (float*)RTAlloc(unit->mWorld, framesize_f);
		memset(unit->m_outbuf, 0, framesize_f);
		unit->m_prevtrig = 0.f;
		if ( INRATE(0) == calc_FullRate )
			SETCALC(Convolution3_next_a);
		else
			SETCALC(Convolution3_next_k);

		// initialize output
		OUT0(0) = IN0(0);
	}
}

void Convolution3_Dtor(Convolution3 *unit)
{
	RTFree(unit->mWorld, unit->m_inbuf1);
	RTFree(unit->mWorld, unit->m_inbuf2);
	RTFree(unit->mWorld, unit->m_outbuf);
}

void Convolution3_next_a(Convolution3 *unit)
{
	float *in = IN(0);
	float curtrig = ZIN0(2);

	float *pin1 = unit->m_inbuf1;

	int numSamples = unit->mWorld->mFullRate.mBufLength;

	// copy input
	Copy(numSamples, pin1, in);

	if (unit->m_prevtrig <= 0.f && curtrig > 0.f)
	{
		uint32 framesize_f=unit->m_framesize * sizeof(float);
		float fbufnum  = ZIN0(1);
		uint32 bufnum = (int)fbufnum;
		SndBuf *buf = ConvGetBuffer(unit, bufnum, "Convolution3", numSamples);
		LOCK_SNDBUF_SHARED(buf);
		memcpy(unit->m_inbuf2, buf->data, framesize_f);
	}

	float * pin2= unit->m_inbuf2;
	float * pout=unit->m_outbuf;
	int pos = unit->m_pos;
	int size = unit->m_framesize;

	for ( int j=0; j < numSamples; ++j)
	{
		float input = pin1[j];
		for ( int i=0; i < size; ++i )
		{
			int ind = (pos+i+j)%(size);
			pout[ind] = pout[ind] + pin2[i]*input;
		}
	}

	float *output = ZOUT(0);

	for (int i=0; i<numSamples; ++i) {
		int ind = (pos+i)%(size);
		*++output = pout[ind];
	}

	pos = pos + numSamples;
	if ( pos > size )
	{
		unit->m_pos = pos - size; //reset collection counter
	}
	else
	{
		unit->m_pos += numSamples;
	}
	unit->m_prevtrig = curtrig;
}


void Convolution3_next_k(Convolution3 *unit)
{
	float input = ZIN0(0);
	float curtrig = ZIN0(2);

	uint32 framesize_f=unit->m_framesize * sizeof(float);


	if (unit->m_prevtrig <= 0.f && curtrig > 0.f)
	{
		float fbufnum  = ZIN0(1);
		uint32 bufnum = (int)fbufnum;
		SndBuf *buf= ConvGetBuffer(unit, bufnum, "Convolution3", 1);
		if (!buf)
			return;
		LOCK_SNDBUF_SHARED(buf);

		memcpy(unit->m_inbuf2, buf->data, framesize_f);
	}

	float * pin= unit->m_inbuf2;
	float * pout=unit->m_outbuf;
	int pos = unit->m_pos;
	int size = unit->m_framesize;

	for ( int i=0; i < size; ++i )
	{
		int ind = (pos+i)%(size);
		pout[ind] = pout[ind] + pin[i]*input;
	}

	float *output = OUT(0);
	*output = pout[pos];

	if ( ++pos > size )
		unit->m_pos = 0; //reset collection counter
	else
		unit->m_pos++;
	unit->m_prevtrig = curtrig;
}

void initConvolution(InterfaceTable *it)
{
	DefineDtorUnit(Convolution);
	DefineDtorUnit(Convolution2);
	DefineDtorUnit(Convolution2L);
	DefineDtorUnit(StereoConvolution2L);
	DefineDtorUnit(Convolution3);
}

