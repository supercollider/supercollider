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
#include <sndfile.h>
#include "SCComplex.h"

// not ready for altivec yet..
#undef __VEC__
#define __VEC__ 0


#if __VEC__
	#include <vecLib/vecLib.h>
	extern FFTSetup fftsetup[32];
#else
extern "C" {
	#include "fftlib.h"
	extern float *cosTable[32];
}
#endif

#include <string.h>

extern float *fftWindow[32];

const int kNUMOVERLAPS = 2;

struct SCComplexBuf 
{
	float dc, nyq;
	SCComplex bin[1];
};

struct SCPolarBuf 
{
	float dc, nyq;
	SCPolar bin[1];
};

SCPolarBuf* ToPolarApx(SndBuf *buf);
SCComplexBuf* ToComplexApx(SndBuf *buf);



struct PV_Unit : Unit
{
};


#define sc_clipbuf(x, hi) ((x) >= (hi) ? 0 : ((x) < 0 ? 0 : (x)))

// for operation on one buffer
#define PV_GET_BUF \
	float fbufnum = ZIN0(0); \
	if (fbufnum < 0.f) { ZOUT0(0) = -1.f; return; } \
	ZOUT0(0) = fbufnum; \
	uint32 ibufnum = (uint32)fbufnum; \
	World *world = unit->mWorld; \
	if (ibufnum >= world->mNumSndBufs) ibufnum = 0; \
	SndBuf *buf = world->mSndBufs + ibufnum; \
	int numbins = buf->samples - 2 >> 1;

// for operation on two input buffers, result goes in first one.
#define PV_GET_BUF2 \
	float fbufnum1 = ZIN0(0); \
	float fbufnum2 = ZIN0(1); \
	if (fbufnum1 < 0.f || fbufnum2 < 0.f) { ZOUT0(0) = -1.f; return; } \
	ZOUT0(0) = fbufnum1; \
	uint32 ibufnum1 = (int)fbufnum1; \
	uint32 ibufnum2 = (int)fbufnum2; \
	World *world = unit->mWorld; \
	if (ibufnum1 >= world->mNumSndBufs) ibufnum1 = 0; \
	if (ibufnum2 >= world->mNumSndBufs) ibufnum2 = 0; \
	SndBuf *buf1 = world->mSndBufs + ibufnum1; \
	SndBuf *buf2 = world->mSndBufs + ibufnum2; \
	if (buf1->samples != buf2->samples) return; \
	int numbins = buf1->samples - 2 >> 1;

#define MAKE_TEMP_BUF \
	if (!unit->m_tempbuf) { \
		unit->m_tempbuf = (float*)RTAlloc(unit->mWorld, buf->samples * sizeof(float)); \
		unit->m_numbins = numbins; \
	} else if (numbins != unit->m_numbins) return; 



//declared here so accessible to all target .cpp files
extern InterfaceTable *ft;

