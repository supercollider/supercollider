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

#ifndef _SndBuf_
#define _SndBuf_

#include <sys/types.h>
#include <sndfile.h>

struct SndBuf
{
	double samplerate;
	double sampledur; // = 1/ samplerate
	float *data;
	int channels;
	int samples;
	int frames;
	int mask;	// for delay lines
	int mask1;	// for interpolating oscillators.
	int coord;	// used by fft ugens
	SNDFILE *sndfile; // used by disk i/o
};
typedef struct SndBuf SndBuf;

struct SndBufUpdates
{
	int reads;
	int writes;
};
typedef struct SndBufUpdates SndBufUpdates;

enum { coord_None, coord_Complex, coord_Polar };


inline float PhaseFrac(unsigned long inPhase) 
		{
			union { unsigned long itemp; float ftemp; } u;
			u.itemp = 0x3F800000 | (0x007FFF80 & ((inPhase)<<7));
			return u.ftemp - 1.f;
		}
		
inline float PhaseFrac1(unsigned long inPhase) 
		{
			union { unsigned long itemp; float ftemp; } u;
			u.itemp = 0x3F800000 | (0x007FFF80 & ((inPhase)<<7));
			return u.ftemp;
		}

inline float lookup(float *table, int32 phase, int32 mask)
{
	return table[(phase >> 16) & mask];
}


#define xlobits 14
#define xlobits1 13

inline float lookupi(float *table, uint32 phase, uint32 mask)
{
	float frac = PhaseFrac(phase);
	float *tbl = table + ((phase >> 16) & mask);
	float a = tbl[0];
	float b = tbl[1];
	return a + frac * (b - a);
}

inline float lookupi2(float *table, uint32 phase, uint32 mask)
{
	float frac = PhaseFrac1(phase);
	float *tbl = table + ((phase >> 16) & mask);
	float a = tbl[0];
	float b = tbl[1];
	return a + frac * b;
}

inline float lookupi1(float* table0, float* table1, uint32 pphase, int32 lomask)
{
	float pfrac = PhaseFrac1(pphase);
	uint32 index = ((pphase >> xlobits1) & lomask);
	float val1 = *(float*)((char*)table0 + index);
	float val2 = *(float*)((char*)table1 + index);
	return val1 + val2 * pfrac;
}


inline float lininterp(float x, float a, float b)
{
	return a + x * (b - a);
}


#endif
