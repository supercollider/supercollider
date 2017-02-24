/*
	Improved FFT and IFFT UGens for SuperCollider 3
	Copyright (c) 2007-2008 Dan Stowell, incorporating code from
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


#include "FFT_UGens.h"

// We include vDSP even if not using for FFT, since we want to use some vectorised add/mul tricks
#if defined(__APPLE__) && !defined(SC_IPHONE)
#include <Accelerate/Accelerate.h>
#endif

struct FFTBase : public Unit
{
	SndBuf *m_fftsndbuf;
	float *m_fftbuf;

	int m_pos, m_fullbufsize, m_audiosize; // "fullbufsize" includes any zero-padding, "audiosize" does not.
	int m_log2n_full, m_log2n_audio;

	uint32 m_fftbufnum;

	scfft* m_scfft;

	int m_hopsize, m_shuntsize; // These add up to m_audiosize
	int m_wintype;

	int m_numSamples;
};

struct FFT : public FFTBase
{
	float *m_inbuf;
};

struct IFFT : public FFTBase
{
	float *m_olabuf;
	int m_numSamples;
};

struct FFTTrigger : public FFTBase
{
	int m_numPeriods, m_periodsRemain, m_polar;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void FFT_Ctor(FFT* unit);
	void FFT_ClearUnitOutputs(FFT *unit, int wrongNumSamples);
	void FFT_next(FFT *unit, int inNumSamples);
	void FFT_Dtor(FFT* unit);

	void IFFT_Ctor(IFFT* unit);
	void IFFT_next(IFFT *unit, int inNumSamples);
	void IFFT_Dtor(IFFT* unit);

	void FFTTrigger_Ctor(FFTTrigger* unit);
	void FFTTrigger_next(FFTTrigger* unit, int inNumSamples);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

static int FFTBase_Ctor(FFTBase *unit, int frmsizinput)
{
	World *world = unit->mWorld;

	uint32 bufnum = (uint32)ZIN0(0);
	SndBuf *buf;
	if (bufnum >= world->mNumSndBufs) {
		int localBufNum = bufnum - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localMaxBufNum) {
			buf = parent->mLocalSndBufs + localBufNum;
		} else {
			if(unit->mWorld->mVerbosity > -1){ Print("FFTBase_Ctor error: invalid buffer number: %i.\n", bufnum); }
			return 0;
		}
	} else {
		buf = world->mSndBufs + bufnum;
	}

	if (!buf->data) {
		if(unit->mWorld->mVerbosity > -1){ Print("FFTBase_Ctor error: Buffer %i not initialised.\n", bufnum); }
		return 0;
	}

	unit->m_fftsndbuf = buf;
	unit->m_fftbufnum = bufnum;
	unit->m_fullbufsize = buf->samples;
	int framesize = (int)ZIN0(frmsizinput);
	if(framesize < 1)
		unit->m_audiosize = buf->samples;
	else
		unit->m_audiosize = sc_min(buf->samples, framesize);

	unit->m_log2n_full  = LOG2CEIL(unit->m_fullbufsize);
	unit->m_log2n_audio = LOG2CEIL(unit->m_audiosize);


	// Although FFTW allows non-power-of-two buffers (vDSP doesn't), this would complicate the windowing, so we don't allow it.
	if (!ISPOWEROFTWO(unit->m_fullbufsize)) {
		Print("FFTBase_Ctor error: buffer size (%i) not a power of two.\n", unit->m_fullbufsize);
		return 0;
	}
	else if (!ISPOWEROFTWO(unit->m_audiosize)) {
		Print("FFTBase_Ctor error: audio frame size (%i) not a power of two.\n", unit->m_audiosize);
		return 0;
	}
	else if (unit->m_audiosize < SC_FFT_MINSIZE ||
			(((int)(unit->m_audiosize / unit->mWorld->mFullRate.mBufLength))
					* unit->mWorld->mFullRate.mBufLength != unit->m_audiosize)) {
		Print("FFTBase_Ctor error: audio frame size (%i) not a multiple of the block size (%i).\n", unit->m_audiosize, unit->mWorld->mFullRate.mBufLength);
		return 0;
	}

	unit->m_pos = 0;

	ZOUT0(0) = ZIN0(0);

	return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void FFT_Ctor(FFT *unit)
{
	int winType = sc_clip((int)ZIN0(3), -1, 1); // wintype may be used by the base ctor
	unit->m_wintype = winType;
	if(!FFTBase_Ctor(unit, 5)){
		SETCALC(FFT_ClearUnitOutputs);
		// These zeroes are to prevent the dtor freeing things that don't exist:
		unit->m_inbuf = 0;
		unit->m_scfft = 0;
		return;
	}
	int audiosize = unit->m_audiosize * sizeof(float);

	int hopsize = (int)(sc_max(sc_min(ZIN0(2), 1.f), 0.f) * unit->m_audiosize);
	if (hopsize < unit->mWorld->mFullRate.mBufLength) {
		Print("FFT_Ctor: hopsize smaller than SC's block size (%i) - automatically corrected.\n", hopsize, unit->mWorld->mFullRate.mBufLength);
		hopsize = unit->mWorld->mFullRate.mBufLength;
	} else if (((int)(hopsize / unit->mWorld->mFullRate.mBufLength)) * unit->mWorld->mFullRate.mBufLength
				!= hopsize) {
		Print("FFT_Ctor: hopsize (%i) not an exact multiple of SC's block size (%i) - automatically corrected.\n", hopsize, unit->mWorld->mFullRate.mBufLength);
		hopsize = ((int)(hopsize / unit->mWorld->mFullRate.mBufLength)) * unit->mWorld->mFullRate.mBufLength;
	}
	unit->m_hopsize = hopsize;
	unit->m_shuntsize = unit->m_audiosize - hopsize;

	unit->m_inbuf = (float*)RTAlloc(unit->mWorld, audiosize);

	SCWorld_Allocator alloc(ft, unit->mWorld);
	unit->m_scfft = scfft_create(unit->m_fullbufsize, unit->m_audiosize, (SCFFT_WindowFunction)unit->m_wintype, unit->m_inbuf,
								 unit->m_fftsndbuf->data, kForward, alloc);

	if (!unit->m_scfft) {
		SETCALC(*ClearUnitOutputs);
		return;
	}

	memset(unit->m_inbuf, 0, audiosize);

	//Print("FFT_Ctor: hopsize %i, shuntsize %i, bufsize %i, wintype %i, \n",
	//	unit->m_hopsize, unit->m_shuntsize, unit->m_bufsize, unit->m_wintype);

	if (INRATE(1) == calc_FullRate) {
		unit->m_numSamples = unit->mWorld->mFullRate.mBufLength;
	} else {
		unit->m_numSamples = 1;
	}

	SETCALC(FFT_next);
}

void FFT_Dtor(FFT *unit)
{
	SCWorld_Allocator alloc(ft, unit->mWorld);
	if(unit->m_scfft)
		scfft_destroy(unit->m_scfft, alloc);

	if(unit->m_inbuf)
		RTFree(unit->mWorld, unit->m_inbuf);
}

// Ordinary ClearUnitOutputs outputs zero, potentially telling the IFFT (+ PV UGens) to act on buffer zero, so let's skip that:
void FFT_ClearUnitOutputs(FFT *unit, int wrongNumSamples)
{
	ZOUT0(0) = -1;
}

void FFT_next(FFT *unit, int wrongNumSamples)
{
	float *in = IN(1);
	float *out = unit->m_inbuf + unit->m_pos + unit->m_shuntsize;

	int numSamples = unit->m_numSamples;

	// copy input
	memcpy(out, in, numSamples * sizeof(float));

	unit->m_pos += numSamples;

	bool gate = ZIN0(4) > 0.f; // Buffer shunting continues, but no FFTing

	if (unit->m_pos != unit->m_hopsize || !unit->m_fftsndbuf->data || unit->m_fftsndbuf->samples != unit->m_fullbufsize) {
		if(unit->m_pos == unit->m_hopsize)
			unit->m_pos = 0;
		ZOUT0(0) = -1.f;
	} else {

		unit->m_pos = 0;
		if(gate){
			scfft_dofft(unit->m_scfft);
			unit->m_fftsndbuf->coord = coord_Complex;
			ZOUT0(0) = unit->m_fftbufnum;
		} else {
			ZOUT0(0) = -1;
		}
		// Shunt input buf down
		memmove(unit->m_inbuf, unit->m_inbuf + unit->m_hopsize, unit->m_shuntsize * sizeof(float));
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////

void IFFT_Ctor(IFFT* unit){
	int winType = sc_clip((int)ZIN0(1), -1, 1); // wintype may be used by the base ctor
	unit->m_wintype = winType;

	if(!FFTBase_Ctor(unit, 2)){
		SETCALC(*ClearUnitOutputs);
		// These zeroes are to prevent the dtor freeing things that don't exist:
		unit->m_olabuf = 0;
		return;
	}

	// This will hold the transformed and progressively overlap-added data ready for outputting.
	unit->m_olabuf = (float*)RTAlloc(unit->mWorld, unit->m_audiosize * sizeof(float));
	memset(unit->m_olabuf, 0, unit->m_audiosize * sizeof(float));

	SCWorld_Allocator alloc(ft, unit->mWorld);
	unit->m_scfft = scfft_create(unit->m_fullbufsize, unit->m_audiosize, (SCFFT_WindowFunction)unit->m_wintype, unit->m_fftsndbuf->data,
								 unit->m_fftsndbuf->data, kBackward, alloc);

	if (!unit->m_scfft) {
		SETCALC(*ClearUnitOutputs);
		unit->m_olabuf = 0;
		return;
	}

	// "pos" will be reset to zero when each frame comes in. Until then, the following ensures silent output at first:
	unit->m_pos = 0; //unit->m_audiosize;

	if (unit->mCalcRate == calc_FullRate) {
		unit->m_numSamples = unit->mWorld->mFullRate.mBufLength;
	} else {
		unit->m_numSamples = 1;
	}

	SETCALC(IFFT_next);
	ClearUnitOutputs(unit, 1);
}

void IFFT_Dtor(IFFT* unit)
{
	if(unit->m_olabuf)
		RTFree(unit->mWorld, unit->m_olabuf);

	SCWorld_Allocator alloc(ft, unit->mWorld);
	if(unit->m_scfft)
		scfft_destroy(unit->m_scfft, alloc);
}

void IFFT_next(IFFT *unit, int wrongNumSamples)
{
	float *out = OUT(0); // NB not ZOUT0

	// Load state from struct into local scope
	int pos     = unit->m_pos;
	int audiosize = unit->m_audiosize;

	int numSamples = unit->m_numSamples;
	float *olabuf = unit->m_olabuf;
	float fbufnum = ZIN0(0);

	// Only run the IFFT if we're receiving a new block of input data - otherwise just output data already received
	if (fbufnum >= 0.f){
		// Ensure it's in cartesian format, not polar
		ToComplexApx(unit->m_fftsndbuf);

		float* fftbuf = unit->m_fftsndbuf->data;

		scfft_doifft(unit->m_scfft);

		// Then shunt the "old" time-domain output down by one hop
		int hopsamps = pos;
		int shuntsamps = audiosize - hopsamps;
		if(hopsamps != audiosize)  // There's only copying to be done if the position isn't all the way to the end of the buffer
			memmove(olabuf, olabuf+hopsamps, shuntsamps * sizeof(float));

		// Then mix the "new" time-domain data in - adding at first, then just setting (copying) where the "old" is supposed to be zero.
		#if defined(__APPLE__) && !defined(SC_IPHONE)
			vDSP_vadd(olabuf, 1, fftbuf, 1, olabuf, 1, shuntsamps);
		#else
			// NB we re-use the "pos" variable temporarily here for write rather than read
			for(pos = 0; pos < shuntsamps; ++pos){
				olabuf[pos] += fftbuf[pos];
			}
		#endif
		memcpy(olabuf + shuntsamps, fftbuf + shuntsamps, (hopsamps) * sizeof(float));

		// Move the pointer back to zero, which is where playback will next begin
		pos = 0;

	} // End of has-the-chain-fired

	// Now we can output some stuff, as long as there is still data waiting to be output.
	// If there is NOT data waiting to be output, we output zero. (Either irregular/negative-overlap
	//     FFT firing, or FFT has given up, or at very start of execution.)
	if(pos >= audiosize)
		ClearUnitOutputs(unit, numSamples);
	else {
		memcpy(out, olabuf + pos, numSamples * sizeof(float));
		pos += numSamples;
	}
	unit->m_pos = pos;
}

/////////////////////////////////////////////////////////////////////////////////////////////

void FFTTrigger_Ctor(FFTTrigger *unit)
{
	World *world = unit->mWorld;

/*
	uint32 bufnum = (uint32)IN0(0);
	Print("FFTTrigger_Ctor: bufnum is %i\n", bufnum);
	if (bufnum >= world->mNumSndBufs) bufnum = 0;
	SndBuf *buf = world->mSndBufs + bufnum;
*/


	uint32 bufnum = (uint32)IN0(0);
	//Print("FFTTrigger_Ctor: bufnum is %i\n", bufnum);
	SndBuf *buf;
	if (bufnum >= world->mNumSndBufs) {
		int localBufNum = bufnum - world->mNumSndBufs;
		Graph *parent = unit->mParent;
		if(localBufNum <= parent->localMaxBufNum) {
			buf = parent->mLocalSndBufs + localBufNum;
		} else {
			bufnum = 0;
			buf = world->mSndBufs + bufnum;
		}
	} else {
		buf = world->mSndBufs + bufnum;
	}
	LOCK_SNDBUF(buf);


	unit->m_fftsndbuf = buf;
	unit->m_fftbufnum = bufnum;
	unit->m_fullbufsize = buf->samples;

	int numSamples = unit->mWorld->mFullRate.mBufLength;
	float dataHopSize = IN0(1);
	unit->m_numPeriods = unit->m_periodsRemain = (int)(((float)unit->m_fullbufsize * dataHopSize) / numSamples) - 1;

	buf->coord = (IN0(2) == 1.f) ? coord_Polar : coord_Complex;

	OUT0(0) = IN0(0);
	SETCALC(FFTTrigger_next);
}

void FFTTrigger_next(FFTTrigger *unit, int inNumSamples)
{
	if (unit->m_periodsRemain > 0) {
		ZOUT0(0) = -1.f;
		unit->m_periodsRemain--;
	} else {
		ZOUT0(0) = unit->m_fftbufnum;
		unit->m_pos = 0;
		unit->m_periodsRemain = unit->m_numPeriods;
	}

}

void initFFT(InterfaceTable *inTable)
{
	ft = inTable;

	DefineDtorUnit(FFT);
	DefineDtorUnit(IFFT);
	DefineSimpleUnit(FFTTrigger);
}

