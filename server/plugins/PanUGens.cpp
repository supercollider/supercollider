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


#include "SC_PlugIn.h"
#include "function_attributes.h"

#ifdef NOVA_SIMD
#include "simd_memory.hpp"
#include "simd_binary_arithmetic.hpp"
#include "simd_pan.hpp"
#include "simd_mix.hpp"
using nova::slope_argument;

#endif

#include <boost/align/is_aligned.hpp>

using namespace std; // for math functions

static InterfaceTable *ft;

struct LinPan2 : public Unit
{
	float m_pos, m_level, m_leftamp, m_rightamp;
};

struct Balance2 : public Unit
{
	float m_pos, m_level, m_leftamp, m_rightamp;
};

struct Rotate2 : public Unit
{
	float m_pos, m_sint, m_cost;
};

struct XFade2 : public Unit
{
	float m_pos, m_level, m_leftamp, m_rightamp;
};

struct LinXFade2 : public Unit
{
	float m_pos, m_amp;
};

struct Pan2 : public Unit
{
	float m_pos, m_level, m_leftamp, m_rightamp;
};

struct Pan4 : public Unit
{
	float m_xpos, m_ypos, m_level, m_LF_amp, m_RF_amp, m_LB_amp, m_RB_amp;
};

struct PanB : public Unit
{
	float m_azimuth, m_elevation, m_level, m_W_amp, m_X_amp, m_Y_amp, m_Z_amp;
};

struct PanB2 : public Unit
{
	float m_azimuth, m_level, m_W_amp, m_X_amp, m_Y_amp;
};

struct BiPanB2 : public Unit
{
	float m_azimuth, m_level, m_W_amp, m_X_amp, m_Y_amp;
};

struct PanAz : public Unit
{
	float * m_chanamp;
};

struct DecodeB2 : public Unit
{
	float m_cosa, m_sina;
	float m_W_amp, m_X_amp, m_Y_amp;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void LinPan2_next_ak(LinPan2 *unit, int inNumSamples);
	void LinPan2_next_aa(LinPan2 *unit, int inNumSamples);
	void LinPan2_Ctor(LinPan2* unit);

	void Balance2_next_ak(Balance2 *unit, int inNumSamples);
#ifdef NOVA_SIMD
	FLATTEN void Balance2_next_ak_nova(Balance2 *unit, int inNumSamples);
	FLATTEN void Balance2_next_ak_nova_64(Balance2 *unit, int inNumSamples);
#endif
	void Balance2_next_aa(Balance2 *unit, int inNumSamples);
	void Balance2_Ctor(Balance2* unit);

	void XFade2_next_ak(XFade2 *unit, int inNumSamples);
	void XFade2_next_aa(XFade2 *unit, int inNumSamples);
	void XFade2_Ctor(XFade2* unit);

	void LinXFade2_next_k(LinXFade2 *unit, int inNumSamples);
	void LinXFade2_next_a(LinXFade2 *unit, int inNumSamples);
	void LinXFade2_Ctor(LinXFade2* unit);

	void Pan2_next_ak(Pan2 *unit, int inNumSamples);
	void vPan2_next_ak(Pan2 *unit, int inNumSamples);
	void Pan2_next_aa(Pan2 *unit, int inNumSamples);
	void Pan2_Ctor(Pan2* unit);

	void Pan4_next(Pan4 *unit, int inNumSamples);
	void Pan4_Ctor(Pan4* unit);

	void PanB_next(PanB *unit, int inNumSamples);
	void PanB_Ctor(PanB* unit);

	void PanB2_next(PanB2 *unit, int inNumSamples);
	void PanB2_Ctor(PanB2* unit);

	void BiPanB2_next(BiPanB2 *unit, int inNumSamples);
	void BiPanB2_Ctor(BiPanB2* unit);

	void DecodeB2_next(DecodeB2 *unit, int inNumSamples);
	void DecodeB2_next_nova(DecodeB2 *unit, int inNumSamples);
	void vDecodeB2_next(DecodeB2 *unit, int inNumSamples);
	void DecodeB2_Ctor(DecodeB2* unit);

	void PanAz_next_ak(PanAz *unit, int inNumSamples);
	void PanAz_next_aa(PanAz *unit, int inNumSamples);
	void PanAz_Ctor(PanAz* unit);

	void Rotate2_next_ak(Rotate2 *unit, int inNumSamples);
	void Rotate2_Ctor(Rotate2 *unit);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NOVA_SIMD
FLATTEN void LinPan2_next_ak_nova(LinPan2 *unit, int inNumSamples);
FLATTEN void LinPan2_next_ak_nova_64(LinPan2 *unit, int inNumSamples);
#endif

void LinPan2_Ctor(LinPan2 *unit)
{
	if (INRATE(1) == calc_FullRate) {
		SETCALC(LinPan2_next_aa);
	} else {
#ifdef NOVA_SIMD
	if (BUFLENGTH == 64)
		SETCALC(LinPan2_next_ak_nova_64);
	if (boost::alignment::is_aligned( BUFLENGTH, 16 ))
		SETCALC(LinPan2_next_ak_nova);
	else
#endif
		SETCALC(LinPan2_next_ak);
	}
	// Now we need to initialise some values, which on the first _next run will be "previous"
	float pan = ZIN0(1) * 0.5f + 0.5f;
	unit->m_level = ZIN0(2);
	unit->m_rightamp = unit->m_level * pan;
	unit->m_leftamp  = unit->m_level - unit->m_rightamp;

	LinPan2_next_aa(unit, 1);
}

void LinPan2_next_ak(LinPan2 *unit, int inNumSamples)
{
	float *leftout = ZOUT(0);
	float *rightout = ZOUT(1);
	float *in = ZIN(0);
	float pos = ZIN0(1);
	float level = ZIN0(2);
	float leftamp =  unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		float pan = pos * 0.5f + 0.5f;
		float nextrightamp = level * pan;
		float nextleftamp  = level - nextrightamp;

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		LOOP1(inNumSamples,
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;
			leftamp += leftampslope;
			rightamp += rightampslope;
		);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;
	} else {
		LOOP1(inNumSamples,
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;
		);
	}
}

#ifdef NOVA_SIMD
void LinPan2_next_ak_nova(LinPan2 *unit, int inNumSamples)
{
	float pos = ZIN0(1);
	float level = ZIN0(2);
	float leftamp =	 unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		float pan = pos * 0.5f + 0.5f;
		float nextrightamp = level * pan;
		float nextleftamp  = level - nextrightamp;

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope	= (nextleftamp	- leftamp)	* slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		nova::pan2_vec_simd(OUT(0), OUT(1), IN(0), leftamp, leftampslope,
							rightamp, rightampslope, inNumSamples);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;
	} else
		nova::pan2_vec_simd(OUT(0), OUT(1), IN(0), leftamp, rightamp, inNumSamples);
}

void LinPan2_next_ak_nova_64(LinPan2 *unit, int inNumSamples)
{
	float pos = ZIN0(1);
	float level = ZIN0(2);
	float leftamp =  unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		float pan = pos * 0.5f + 0.5f;
		float nextrightamp = level * pan;
		float nextleftamp  = level - nextrightamp;

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		nova::pan2_vec_simd<64>(OUT(0), OUT(1), IN(0), leftamp, leftampslope,
								rightamp, rightampslope);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;
	} else
		nova::pan2_vec_simd<64>(OUT(0), OUT(1), IN(0), leftamp, rightamp);
}
#endif

void LinPan2_next_aa(LinPan2 *unit, int inNumSamples)
{
	float *leftout = ZOUT(0);
	float *rightout = ZOUT(1);
	float *in = ZIN(0);
	float *pos = ZIN(1);
	float nextlevel = ZIN0(2);
	float level = unit->m_level;
	float levelSlope = (nextlevel - level) * unit->mRate->mSlopeFactor;

	LOOP1(inNumSamples,
		float pan = ZXP(pos) * 0.5f + 0.5f;
		float rightamp = level * pan;
		float leftamp  = level - rightamp;
		float zin = ZXP(in);
		ZXP(leftout)  = zin * leftamp;
		ZXP(rightout) = zin * rightamp;
		level += levelSlope;
	);
	unit->m_level = level;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void Balance2_Ctor(Balance2 *unit)
{
	if (INRATE(2) == calc_FullRate) {
		SETCALC(Balance2_next_aa);
	} else {
#ifdef NOVA_SIMD
		if (BUFLENGTH == 64)
			SETCALC(Balance2_next_ak_nova_64);
		else if (boost::alignment::is_aligned( BUFLENGTH, 16 ))
			SETCALC(Balance2_next_ak_nova);
		else
			SETCALC(Balance2_next_ak);
#else
		SETCALC(Balance2_next_ak);
#endif
	}
	unit->m_pos = ZIN0(2);
	unit->m_level = ZIN0(3);
	int32 ipos = (int32)(1024.f * unit->m_pos + 1024.f + 0.5f);
	ipos = sc_clip(ipos, 0, 2048);

	unit->m_leftamp  = unit->m_level * ft->mSine[2048 - ipos];
	unit->m_rightamp = unit->m_level * ft->mSine[ipos];
	Balance2_next_aa(unit, 1);
}

void Balance2_next_ak(Balance2 *unit, int inNumSamples)
{
	float *leftout = ZOUT(0);
	float *rightout = ZOUT(1);
	float *leftin = ZIN(0);
	float *rightin = ZIN(1);
	float pos = ZIN0(2);
	float level = ZIN0(3);
	float leftamp =  unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		int32 ipos = (int32)(1024.f * pos + 1024.f + 0.5f);
		ipos = sc_clip(ipos, 0, 2048);

		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		LOOP1(inNumSamples,
			ZXP(leftout)  = ZXP(leftin) * leftamp;
			ZXP(rightout) = ZXP(rightin) * rightamp;
			leftamp += leftampslope;
			rightamp += rightampslope;
		);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;
	} else {
		LOOP1(inNumSamples,
			ZXP(leftout)  = ZXP(leftin) * leftamp;
			ZXP(rightout) = ZXP(rightin) * rightamp;
		);
	}
}

#ifdef NOVA_SIMD
void Balance2_next_ak_nova(Balance2 *unit, int inNumSamples)
{
	float pos = ZIN0(2);
	float level = ZIN0(3);
	float leftamp =  unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		int32 ipos = (int32)(1024.f * pos + 1024.f + 0.5f);
		ipos = sc_clip(ipos, 0, 2048);

		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;

		//nova::times_vec2_ramp_simd(OUT(0), IN(0), leftamp, leftampslope, OUT(1), IN(1), rightamp, rightampslope, inNumSamples);
		nova::times_vec_simd(OUT(0), IN(0), slope_argument(leftamp, leftampslope), inNumSamples);
		nova::times_vec_simd(OUT(1), IN(1), slope_argument(rightamp, rightampslope), inNumSamples);
	} else {
		//nova::times_vec2_simd(OUT(0), IN(0), leftamp, OUT(1), IN(1), rightamp, inNumSamples);
		nova::times_vec_simd(OUT(0), IN(0), leftamp, inNumSamples);
		nova::times_vec_simd(OUT(1), IN(1), rightamp, inNumSamples);
	}
}

void Balance2_next_ak_nova_64(Balance2 *unit, int inNumSamples)
{
	float pos = ZIN0(2);
	float level = ZIN0(3);
	float leftamp =  unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		int32 ipos = (int32)(1024.f * pos + 1024.f + 0.5f);
		ipos = sc_clip(ipos, 0, 2048);

		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;

		//nova::times_vec2_ramp_simd(OUT(0), IN(0), leftamp, leftampslope, OUT(1), IN(1), rightamp, rightampslope, inNumSamples);
		nova::times_vec_simd(OUT(0), IN(0), slope_argument(leftamp, leftampslope), inNumSamples);
		nova::times_vec_simd(OUT(1), IN(1), slope_argument(rightamp, rightampslope), inNumSamples);
	} else {
		//nova::times_vec2_simd(OUT(0), IN(0), leftamp, OUT(1), IN(1), rightamp, inNumSamples);
		nova::times_vec_simd<64>(OUT(0), IN(0), leftamp);
		nova::times_vec_simd<64>(OUT(1), IN(1), rightamp);
	}
}
#endif

void Balance2_next_aa(Balance2 *unit, int inNumSamples)
{
	float *leftout = ZOUT(0);
	float *rightout = ZOUT(1);
	float *leftin = ZIN(0);
	float *rightin = ZIN(1);
	float *pos = ZIN(2);
	float nextlevel = ZIN0(3);
	float level = unit->m_level;

	float *sineTable = ft->mSine;
	if (level != nextlevel) {
		float levelSlope = (nextlevel - level) * unit->mRate->mSlopeFactor;
		LOOP1(inNumSamples,
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f + 0.5f);
			ipos = sc_clip(ipos, 0, 2048);

			float leftamp  = level * sineTable[2048 - ipos];
			float rightamp = level * sineTable[ipos];
			ZXP(leftout)  = ZXP(leftin) * leftamp;
			ZXP(rightout) = ZXP(rightin) * rightamp;
			level += levelSlope;
		);
		unit->m_level = level;
	} else {
		LOOP1(inNumSamples,
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f + 0.5f);
			ipos = sc_clip(ipos, 0, 2048);

			float leftamp  = level * sineTable[2048 - ipos];
			float rightamp = level * sineTable[ipos];
			ZXP(leftout)  = ZXP(leftin) * leftamp;
			ZXP(rightout) = ZXP(rightin) * rightamp;
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef NOVA_SIMD
FLATTEN void XFade2_next_ak_nova(XFade2 *unit, int inNumSamples);
FLATTEN void XFade2_next_ak_nova_64(XFade2 *unit, int inNumSamples);
#endif

void XFade2_Ctor(XFade2 *unit)
{
	if (INRATE(2) == calc_FullRate) {
		SETCALC(XFade2_next_aa);
	} else {
#ifdef NOVA_SIMD
		if (BUFLENGTH == 64)
			SETCALC(XFade2_next_ak_nova_64);
		if (boost::alignment::is_aligned( BUFLENGTH, 16 ))
			SETCALC(XFade2_next_ak_nova);
		else
#endif
		SETCALC(XFade2_next_ak);
	}
	unit->m_pos = ZIN0(2);
	unit->m_level = ZIN0(3);
	int32 ipos = (int32)(1024.f * unit->m_pos + 1024.f + 0.5f);
	ipos = sc_clip(ipos, 0, 2048);

	unit->m_leftamp  = unit->m_level * ft->mSine[2048 - ipos];
	unit->m_rightamp = unit->m_level * ft->mSine[ipos];
	XFade2_next_aa(unit, 1);
}

void XFade2_next_ak(XFade2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *leftin = ZIN(0);
	float *rightin = ZIN(1);
	float pos = ZIN0(2);
	float level = ZIN0(3);
	float leftamp =  unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		int32 ipos = (int32)(1024.f * pos + 1024.f + 0.5f);
		ipos = sc_clip(ipos, 0, 2048);

		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		LOOP1(inNumSamples,
			ZXP(out) = ZXP(leftin) * leftamp + ZXP(rightin) * rightamp;
			leftamp += leftampslope;
			rightamp += rightampslope;
		);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;
	} else {
		LOOP1(inNumSamples,
			ZXP(out) = ZXP(leftin) * leftamp + ZXP(rightin) * rightamp;
		);
	}
}

#ifdef NOVA_SIMD
void XFade2_next_ak_nova(XFade2 *unit, int inNumSamples)
{
	float pos = ZIN0(2);
	float level = ZIN0(3);
	float leftamp =  unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		int32 ipos = (int32)(1024.f * pos + 1024.f + 0.5f);
		ipos = sc_clip(ipos, 0, 2048);

		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		nova::mix_vec_simd(OUT(0), IN(0), slope_argument(leftamp, leftampslope),
						   IN(1), slope_argument(rightamp, rightampslope), inNumSamples);

		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;
	} else
		nova::mix_vec_simd(OUT(0), IN(0), leftamp, IN(1), rightamp, inNumSamples);
}

void XFade2_next_ak_nova_64(XFade2 *unit, int inNumSamples)
{
	float pos = ZIN0(2);
	float level = ZIN0(3);
	float leftamp =  unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		int32 ipos = (int32)(1024.f * pos + 1024.f + 0.5f);
		ipos = sc_clip(ipos, 0, 2048);

		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		nova::mix_vec_simd<64>(OUT(0), IN(0), slope_argument(leftamp, leftampslope),
								  IN(1), slope_argument(rightamp, rightampslope));

		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;
	} else
		nova::mix_vec_simd<64>(OUT(0), IN(0), leftamp, IN(1), rightamp);
}

#endif

void XFade2_next_aa(XFade2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *leftin = ZIN(0);
	float *rightin = ZIN(1);
	float *pos = ZIN(2);
	float nextlevel = ZIN0(3);
	float level = unit->m_level;

	float *sineTable = ft->mSine;
	if (level != nextlevel) {
		float levelSlope = (nextlevel - level) * unit->mRate->mSlopeFactor;
		LOOP1(inNumSamples,
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f + 0.5f);
			ipos = sc_clip(ipos, 0, 2048);

			float leftamp  = level * sineTable[2048 - ipos];
			float rightamp = level * sineTable[ipos];
			ZXP(out) = ZXP(leftin) * leftamp + ZXP(rightin) * rightamp;
			level += levelSlope;
		);
		unit->m_level = level;
	} else {
		LOOP1(inNumSamples,
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f + 0.5f);
			ipos = sc_clip(ipos, 0, 2048);

			float leftamp  = level * sineTable[2048 - ipos];
			float rightamp = level * sineTable[ipos];
			ZXP(out) = ZXP(leftin) * leftamp + ZXP(rightin) * rightamp;
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////

void LinXFade2_next_i(LinXFade2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *leftin = ZIN(0);
	float *rightin = ZIN(1);
	float amp = unit->m_amp;

	LOOP1(inNumSamples,
		float l = ZXP(leftin);
		float r = ZXP(rightin);
		ZXP(out) = l + amp * (r - l);
	);
}

void LinXFade2_next_k(LinXFade2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *leftin = ZIN(0);
	float *rightin = ZIN(1);
	float pos = ZIN0(2);
	float amp = unit->m_amp;

	if (pos != unit->m_pos) {
		pos = sc_clip(pos, -1.f, 1.f);

		float nextamp  = pos * 0.5f + 0.5f;
		float amp_slope  = (nextamp - amp) * unit->mRate->mSlopeFactor;

		LOOP1(inNumSamples,
			float l = ZXP(leftin);
			float r = ZXP(rightin);
			ZXP(out) = l + amp * (r - l);
			amp += amp_slope;
		);
		unit->m_pos = pos;
		unit->m_amp = amp;
	} else {
		LOOP1(inNumSamples,
			float l = ZXP(leftin);
			float r = ZXP(rightin);
			ZXP(out) = l + amp * (r - l);
		);
	}
}

void LinXFade2_next_a(LinXFade2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *leftin = ZIN(0);
	float *rightin = ZIN(1);
	float *posp = ZIN(2);

	LOOP1(inNumSamples,
		float pos = ZXP(posp);
		pos = sc_clip(pos, -1.f, 1.f);
		float amp = pos * 0.5f + 0.5f;
		float l = ZXP(leftin);
		float r = ZXP(rightin);
		ZXP(out) = l + amp * (r - l);
	);
}

#ifdef NOVA_SIMD

FLATTEN void LinXFade2_next_i_nova(LinXFade2 *unit, int inNumSamples)
{
	float amp = unit->m_amp;
	nova::mix_vec_simd(OUT(0), IN(0), 1.f - amp, IN(1), amp, inNumSamples);
}

FLATTEN void LinXFade2_next_k_nova(LinXFade2 *unit, int inNumSamples)
{
	float pos = ZIN0(2);
	float amp = unit->m_amp;

	if (pos != unit->m_pos) {
		float oldAmpRight = amp;
		float oldAmpLeft  = 1.f - amp;

		pos = sc_clip(pos, -1.f, 1.f);

		float nextAmpRight = pos * 0.5f + 0.5f;
		float nextAmpLeft = 1.f - nextAmpRight;

		float leftSlope =  CALCSLOPE(nextAmpLeft, oldAmpLeft);
		float rightSlope = CALCSLOPE(nextAmpRight, oldAmpRight);

		unit->m_amp = nextAmpRight;
		unit->m_pos = pos;

		nova::mix_vec_simd(OUT(0), IN(0), nova::slope_argument(oldAmpLeft, leftSlope),
						   IN(1), nova::slope_argument(oldAmpRight, rightSlope),
						   inNumSamples);
	} else
		nova::mix_vec_simd(OUT(0), IN(0), 1.f - amp, IN(1), amp, inNumSamples);
}

#endif

void LinXFade2_Ctor(LinXFade2 *unit)
{
	switch (INRATE(2)) {
	case calc_FullRate:
		SETCALC(LinXFade2_next_a);
		break;

	case calc_BufRate:
#ifdef NOVA_SIMD
		if (boost::alignment::is_aligned( BUFLENGTH, 16 ))
			SETCALC(LinXFade2_next_k_nova);
		else
			SETCALC(LinXFade2_next_k);
#else
		SETCALC(LinXFade2_next_k);
#endif
		break;
	case calc_ScalarRate:
#ifdef NOVA_SIMD
		if (boost::alignment::is_aligned( BUFLENGTH, 16 ))
			SETCALC(LinXFade2_next_i_nova);
		else
			SETCALC(LinXFade2_next_i);
#else
		SETCALC(LinXFade2_next_i);
#endif
		break;
	}

	unit->m_pos = ZIN0(2);
	unit->m_pos = sc_clip(unit->m_pos, -1.f, 1.f);
	unit->m_amp = unit->m_pos * 0.5f + 0.5f;

	LinXFade2_next_a(unit, 1);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pan2_next_ak(Pan2 *unit, int inNumSamples)
{
	float *leftout = ZOUT(0);
	float *rightout = ZOUT(1);
	float *in = ZIN(0);
	float pos = ZIN0(1);
	float level = ZIN0(2);
	float leftamp =  unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		int32 ipos = (int32)(1024.f * pos + 1024.f + 0.5f);
		ipos = sc_clip(ipos, 0, 2048);

		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		LOOP1(inNumSamples,
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;
			leftamp += leftampslope;
			rightamp += rightampslope;
		);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;
	} else {
		LOOP1(inNumSamples,
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;
		);
	}
}

#ifdef NOVA_SIMD
void Pan2_next_ak_nova(Pan2 *unit, int inNumSamples)
{
	float pos = ZIN0(1);
	float level = ZIN0(2);
	float leftamp =	 unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		int32 ipos = (int32)(1024.f * pos + 1024.f + 0.5f);
		ipos = sc_clip(ipos, 0, 2048);

		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope = (nextleftamp - leftamp) * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		nova::pan2_vec_simd(OUT(0), OUT(1), IN(0), leftamp, leftampslope,
							rightamp, rightampslope, inNumSamples);

		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;
	} else
		nova::pan2_vec_simd(OUT(0), OUT(1), IN(0), leftamp, rightamp, inNumSamples);
}

void Pan2_next_ak_nova_64(Pan2 *unit, int inNumSamples)
{
	float pos = ZIN0(1);
	float level = ZIN0(2);
	float leftamp =	 unit->m_leftamp;
	float rightamp = unit->m_rightamp;

	if (pos != unit->m_pos || unit->m_level != level) {
		int32 ipos = (int32)(1024.f * pos + 1024.f + 0.5f);
		ipos = sc_clip(ipos, 0, 2048);

		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope = (nextleftamp - leftamp) * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		nova::pan2_vec_simd<64>(OUT(0), OUT(1), IN(0), leftamp, leftampslope,
								   rightamp, rightampslope);

		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = nextleftamp;
		unit->m_rightamp = nextrightamp;
	} else
		nova::pan2_vec_simd<64>(OUT(0), OUT(1), IN(0), leftamp, rightamp);
}

#endif


void Pan2_next_aa(Pan2 *unit, int inNumSamples)
{
	float *leftout = ZOUT(0);
	float *rightout = ZOUT(1);
	float *in = ZIN(0);
	float *pos = ZIN(1);
	float nextlevel = ZIN0(2);
	float level = unit->m_level;

	if (level != nextlevel) {
		float levelSlope = (nextlevel - level) * unit->mRate->mSlopeFactor;

		LOOP1(inNumSamples,
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f + 0.5f);
			ipos = sc_clip(ipos, 0, 2048);

			float leftamp  = level * ft->mSine[2048 - ipos];
			float rightamp = level * ft->mSine[ipos];
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;
			level += levelSlope;
		);
		unit->m_level = level;
	} else {
		LOOP1(inNumSamples,
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f + 0.5f);
			ipos = sc_clip(ipos, 0, 2048);

			float leftamp  = level * ft->mSine[2048 - ipos];
			float rightamp = level * ft->mSine[ipos];
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;
		);
	}
}

void Pan2_Ctor(Pan2 *unit)
{
	if (INRATE(1) == calc_FullRate) {
		SETCALC(Pan2_next_aa);
	} else {
#if defined(NOVA_SIMD)
		if (BUFLENGTH == 64)
			SETCALC(Pan2_next_ak_nova_64);
		if (boost::alignment::is_aligned( BUFLENGTH, 16 ))
			SETCALC(Pan2_next_ak_nova);
		else
			SETCALC(Pan2_next_ak);
#else
		SETCALC(Pan2_next_ak);
#endif
	}

	unit->m_pos = ZIN0(1);
	unit->m_level = ZIN0(2);
	int32 ipos = (int32)(1024.f * unit->m_pos + 1024.f + 0.5f);
	ipos = sc_clip(ipos, 0, 2048);

	unit->m_leftamp  = unit->m_level * ft->mSine[2048 - ipos];
	unit->m_rightamp = unit->m_level * ft->mSine[ipos];
	Pan2_next_aa(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pan4_Ctor(Pan4 *unit)
{
	SETCALC(Pan4_next);
	//float *in = ZIN(0);
	float xpos = ZIN0(1);
	float ypos = ZIN0(2);
	float level = ZIN0(3);

	unit->m_xpos = xpos;
	unit->m_ypos = ypos;
	unit->m_level = level;
	if (xpos < -1.f || xpos > 1.f || ypos < -1.f || ypos > 1.f) {
		float xabs = fabs(xpos);

		if (ypos > xabs) {
			xpos = (xpos + ypos) / ypos - 1.f;
			ypos = 1.f;
		} else if (ypos < -xabs) {
			xpos = (xpos - ypos) / -ypos - 1.f;
			ypos = -1.f;
		} else {
			float yabs = fabs(ypos);
			if (yabs < xpos) {
				ypos = (ypos + xpos) / xpos - 1.f;
				xpos = 1.f;
			} else {
				ypos = (ypos - xpos) / -xpos - 1.f;
				xpos = -1.f;
			}
		}
	}

	int32 ixpos = (int32)(1024.f * xpos + 1024.f + 0.5f);
	ixpos = sc_clip(ixpos, 0, 2048);
	float leftamp  = ft->mSine[2048 - ixpos];
	float rightamp = ft->mSine[ixpos];

	int32 iypos = (int32)(1024.f * ypos + 1024.f + 0.5f);
	iypos = sc_clip(iypos, 0, 2048);
	float frontamp = ft->mSine[iypos];
	float backamp  = ft->mSine[2048 - iypos];

	frontamp *= level;
	backamp  *= level;

	unit->m_LF_amp = leftamp  * frontamp;
	unit->m_RF_amp = rightamp * frontamp;
	unit->m_LB_amp = leftamp  * backamp;
	unit->m_RB_amp = rightamp * backamp;

	float z = ZIN0(0);
	ZOUT0(0) = z * unit->m_LF_amp;
	ZOUT0(1) = z * unit->m_RF_amp;
	ZOUT0(2) = z * unit->m_LB_amp;
	ZOUT0(3) = z * unit->m_RB_amp;
}

void Pan4_next(Pan4 *unit, int inNumSamples)
{
	float *LFout = ZOUT(0);
	float *RFout = ZOUT(1);
	float *LBout = ZOUT(2);
	float *RBout = ZOUT(3);

	float *in = ZIN(0);
	float xpos = ZIN0(1);
	float ypos = ZIN0(2);
	float level = ZIN0(3);

	float LF_amp = unit->m_LF_amp;
	float RF_amp = unit->m_RF_amp;
	float LB_amp = unit->m_LB_amp;
	float RB_amp = unit->m_RB_amp;

	if (xpos != unit->m_xpos || ypos != unit->m_ypos || level != unit->m_level) {
		unit->m_xpos = xpos;
		unit->m_ypos = ypos;
		unit->m_level = level;
		if (xpos < -1.f || xpos > 1.f || ypos < -1.f || ypos > 1.f) {
			float xabs = fabs(xpos);

			if (ypos > xabs) {
				xpos = (xpos + ypos) / ypos - 1.f;
				ypos = 1.f;
			} else if (ypos < -xabs) {
				xpos = (xpos - ypos) / -ypos - 1.f;
				ypos = -1.f;
			} else {
				float yabs = fabs(ypos);
				if (yabs < xpos) {
					ypos = (ypos + xpos) / xpos - 1.f;
					xpos = 1.f;
				} else {
					ypos = (ypos - xpos) / -xpos - 1.f;
					xpos = -1.f;
				}
			}
		}

		int32 ixpos = (int32)(1024.f * xpos + 1024.f + 0.5f);
		ixpos = sc_clip(ixpos, 0, 2048);
		float leftamp  = ft->mSine[2048 - ixpos];
		float rightamp = ft->mSine[ixpos];

		int32 iypos = (int32)(1024.f * ypos + 1024.f + 0.5f);
		iypos = sc_clip(iypos, 0, 2048);
		float frontamp = ft->mSine[iypos];
		float backamp  = ft->mSine[2048 - iypos];

		frontamp *= level;
		backamp  *= level;

		float next_LF_amp = leftamp  * frontamp;
		float next_RF_amp = rightamp * frontamp;
		float next_LB_amp = leftamp  * backamp;
		float next_RB_amp = rightamp * backamp;

		float LF_slope = CALCSLOPE(next_LF_amp, LF_amp);
		float RF_slope = CALCSLOPE(next_RF_amp, RF_amp);
		float LB_slope = CALCSLOPE(next_LB_amp, LB_amp);
		float RB_slope = CALCSLOPE(next_RB_amp, RB_amp);

		LOOP1(inNumSamples,
			float z = ZXP(in);
			ZXP(LFout) = z * LF_amp;
			ZXP(RFout) = z * RF_amp;
			ZXP(LBout) = z * LB_amp;
			ZXP(RBout) = z * RB_amp;
			LF_amp += LF_slope;
			RF_amp += RF_slope;
			LB_amp += LB_slope;
			RB_amp += RB_slope;
		);
		unit->m_LF_amp = LF_amp;
		unit->m_RF_amp = RF_amp;
		unit->m_LB_amp = LB_amp;
		unit->m_RB_amp = RB_amp;
	} else {
		LOOP1(inNumSamples,
			float z = ZXP(in);
			ZXP(LFout) = z * LF_amp;
			ZXP(RFout) = z * RF_amp;
			ZXP(LBout) = z * LB_amp;
			ZXP(RBout) = z * RB_amp;
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void PanB_Ctor(PanB *unit)
{
	SETCALC(PanB_next);
	float azimuth = unit->m_azimuth = ZIN0(1);
	float elevation = unit->m_elevation = ZIN0(2);
	float level = unit->m_level = ZIN0(3);

	int kSineSize = ft->mSineSize;
	int kSineMask = kSineSize - 1;

	long iazimuth   = kSineMask & (long)(azimuth   * (float)(kSineSize >> 1));
	long ielevation = kSineMask & (long)(elevation * (float)(kSineSize >> 2));
	float sina = -ft->mSine[iazimuth];
	float sinb =  ft->mSine[ielevation];

	iazimuth   = kSineMask & (iazimuth   + (kSineSize>>2));
	ielevation = kSineMask & (ielevation + (kSineSize>>2));
	float cosa = ft->mSine[iazimuth];
	float cosb = ft->mSine[ielevation];

	unit->m_W_amp = rsqrt2_f * level;
	unit->m_X_amp = cosa * cosb * level;
	unit->m_Y_amp = sina * cosb * level;
	unit->m_Z_amp = sinb * level;

	PanB_next(unit, 1);
}

void PanB_next(PanB *unit, int inNumSamples)
{
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);
	float *Zout = ZOUT(3);

	float *in = ZIN(0);
	float azimuth = ZIN0(1);
	float elevation = ZIN0(2);
	float level = ZIN0(3);

	float W_amp = unit->m_W_amp;
	float X_amp = unit->m_X_amp;
	float Y_amp = unit->m_Y_amp;
	float Z_amp = unit->m_Z_amp;

	int kSineSize = ft->mSineSize;
	int kSineMask = kSineSize - 1;
	if (azimuth != unit->m_azimuth || elevation != unit->m_elevation || level != unit->m_level) {
		unit->m_azimuth = azimuth;
		unit->m_elevation = elevation;
		unit->m_level = level;

		long iazimuth   = kSineMask & (long)(azimuth   * (float)(kSineSize >> 1));
		long ielevation = kSineMask & (long)(elevation * (float)(kSineSize >> 2));
		float sina = -ft->mSine[iazimuth];
		float sinb =  ft->mSine[ielevation];

		iazimuth   = kSineMask & (iazimuth   + (kSineSize>>2));
		ielevation = kSineMask & (ielevation + (kSineSize>>2));
		float cosa = ft->mSine[iazimuth];
		float cosb = ft->mSine[ielevation];

		float next_W_amp = rsqrt2_f * level;
		float next_X_amp = cosa * cosb * level;
		float next_Y_amp = sina * cosb * level;
		float next_Z_amp = sinb * level;

		float W_slope = CALCSLOPE(next_W_amp, W_amp);
		float X_slope = CALCSLOPE(next_X_amp, X_amp);
		float Y_slope = CALCSLOPE(next_Y_amp, Y_amp);
		float Z_slope = CALCSLOPE(next_Z_amp, Z_amp);

		LOOP1(inNumSamples,
			float z = ZXP(in);
			ZXP(Wout) = z * W_amp;
			ZXP(Xout) = z * X_amp;
			ZXP(Yout) = z * Y_amp;
			ZXP(Zout) = z * Z_amp;
			W_amp += W_slope;
			X_amp += X_slope;
			Y_amp += Y_slope;
			Z_amp += Z_slope;
		);
		unit->m_W_amp = W_amp;
		unit->m_X_amp = X_amp;
		unit->m_Y_amp = Y_amp;
		unit->m_Z_amp = Z_amp;
	} else {
		LOOP1(inNumSamples,
			float z = ZXP(in);
			ZXP(Wout) = z * W_amp;
			ZXP(Xout) = z * X_amp;
			ZXP(Yout) = z * Y_amp;
			ZXP(Zout) = z * Z_amp;
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void PanB2_next(PanB2 *unit, int inNumSamples)
{
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);

	float *in = ZIN(0);
	float azimuth = ZIN0(1);
	float level = ZIN0(2);

	float W_amp = unit->m_W_amp;
	float X_amp = unit->m_X_amp;
	float Y_amp = unit->m_Y_amp;

	int kSineSize = ft->mSineSize;
	int kSineMask = kSineSize - 1;
	if (azimuth != unit->m_azimuth || level != unit->m_level) {
		unit->m_azimuth = azimuth;
		unit->m_level = level;

		long isinpos = kSineMask & (long)(azimuth * (float)(kSineSize >> 1));
		float sina = -ft->mSine[isinpos];

		long icospos = kSineMask & (isinpos + (kSineSize>>2));
		float cosa = ft->mSine[icospos];

		float next_W_amp = rsqrt2_f * level;
		float next_X_amp = cosa * level;
		float next_Y_amp = sina * level;

		float W_slope = CALCSLOPE(next_W_amp, W_amp);
		float X_slope = CALCSLOPE(next_X_amp, X_amp);
		float Y_slope = CALCSLOPE(next_Y_amp, Y_amp);

		LOOP1(inNumSamples,
			float z = ZXP(in);
			ZXP(Wout) = z * W_amp;
			ZXP(Xout) = z * X_amp;
			ZXP(Yout) = z * Y_amp;
			W_amp += W_slope;
			X_amp += X_slope;
			Y_amp += Y_slope;
		);
		unit->m_W_amp = W_amp;
		unit->m_X_amp = X_amp;
		unit->m_Y_amp = Y_amp;
	} else {
		LOOP1(inNumSamples,
			float z = ZXP(in);
			ZXP(Wout) = z * W_amp;
			ZXP(Xout) = z * X_amp;
			ZXP(Yout) = z * Y_amp;
		);
	}
}

#ifdef NOVA_SIMD

FLATTEN void PanB2_next_nova(PanB2 *unit, int inNumSamples)
{
	float *Wout = OUT(0);
	float *Xout = OUT(1);
	float *Yout = OUT(2);

	float *in = IN(0);
	float azimuth = ZIN0(1);
	float level = ZIN0(2);

	float W_amp = unit->m_W_amp;
	float X_amp = unit->m_X_amp;
	float Y_amp = unit->m_Y_amp;

	int kSineSize = ft->mSineSize;
	int kSineMask = kSineSize - 1;
	if (azimuth != unit->m_azimuth || level != unit->m_level) {
		unit->m_azimuth = azimuth;
		unit->m_level = level;

		long isinpos = kSineMask & (long)(azimuth * (float)(kSineSize >> 1));
		float sina = -ft->mSine[isinpos];

		long icospos = kSineMask & (isinpos + (kSineSize>>2));
		float cosa = ft->mSine[icospos];

		float next_W_amp = rsqrt2_f * level;
		float next_X_amp = cosa * level;
		float next_Y_amp = sina * level;

		float W_slope = CALCSLOPE(next_W_amp, W_amp);
		float X_slope = CALCSLOPE(next_X_amp, X_amp);
		float Y_slope = CALCSLOPE(next_Y_amp, Y_amp);

		nova::times_vec_simd(Wout, in, slope_argument(W_amp, W_slope), inNumSamples);
		nova::times_vec_simd(Xout, in, slope_argument(X_amp, X_slope), inNumSamples);
		nova::times_vec_simd(Yout, in, slope_argument(Y_amp, Y_slope), inNumSamples);

		unit->m_W_amp = next_W_amp;
		unit->m_X_amp = next_X_amp;
		unit->m_Y_amp = next_Y_amp;
	} else {
		// TODO: can be further optimized by joining the loops
		nova::times_vec_simd(Wout, in, W_amp, inNumSamples);
		nova::times_vec_simd(Xout, in, X_amp, inNumSamples);
		nova::times_vec_simd(Yout, in, Y_amp, inNumSamples);
	}
}
#endif

void PanB2_Ctor(PanB2 *unit)
{
#if defined(NOVA_SIMD)
	if (boost::alignment::is_aligned( BUFLENGTH, 16 ))
		SETCALC(PanB2_next_nova);
	else
#endif
		SETCALC(PanB2_next);

	float azimuth = unit->m_azimuth = ZIN0(1);
	float level = unit->m_level = ZIN0(2);

	int kSineSize = ft->mSineSize;
	int kSineMask = kSineSize - 1;

	long isinpos = kSineMask & (long)(azimuth * (float)(kSineSize >> 1));
	float sina = -ft->mSine[isinpos];

	long icospos = kSineMask & (isinpos + (kSineSize>>2));
	float cosa = ft->mSine[icospos];

	unit->m_W_amp = rsqrt2_f * level;
	unit->m_X_amp = cosa * level;
	unit->m_Y_amp = sina * level;

	PanB2_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void BiPanB2_next(BiPanB2 *unit, int inNumSamples)
{
	float *Wout = ZOUT(0);
	float *Xout = ZOUT(1);
	float *Yout = ZOUT(2);

	float *inA = ZIN(0);
	float *inB = ZIN(1);
	float azimuth = ZIN0(2);
	float level = ZIN0(3);

	float W_amp = unit->m_W_amp;
	float X_amp = unit->m_X_amp;
	float Y_amp = unit->m_Y_amp;

	int kSineSize = ft->mSineSize;
	int kSineMask = kSineSize - 1;
	if (azimuth != unit->m_azimuth || level != unit->m_level) {
		unit->m_azimuth = azimuth;
		unit->m_level = level;

		long isinpos = kSineMask & (long)(azimuth * (float)(kSineSize >> 1));
		float sina = -ft->mSine[isinpos];

		long icospos = kSineMask & (isinpos + (kSineSize>>2));
		float cosa = ft->mSine[icospos];

		float next_W_amp = rsqrt2_f * level;
		float next_X_amp = cosa * level;
		float next_Y_amp = sina * level;

		float W_slope = CALCSLOPE(next_W_amp, W_amp);
		float X_slope = CALCSLOPE(next_X_amp, X_amp);
		float Y_slope = CALCSLOPE(next_Y_amp, Y_amp);

		if (W_slope == 0.f) {
			LOOP1(inNumSamples,
				float a = ZXP(inA);
				float b = ZXP(inB);
				float abdiff = a - b;
				ZXP(Wout) = (a + b) * W_amp;
				ZXP(Xout) = abdiff * X_amp;
				ZXP(Yout) = abdiff * Y_amp;
				X_amp += X_slope;
				Y_amp += Y_slope;
			);
		} else {
			LOOP1(inNumSamples,
				float a = ZXP(inA);
				float b = ZXP(inB);
				float abdiff = a - b;
				ZXP(Wout) = (a + b) * W_amp;
				ZXP(Xout) = abdiff * X_amp;
				ZXP(Yout) = abdiff * Y_amp;
				W_amp += W_slope;
				X_amp += X_slope;
				Y_amp += Y_slope;
			);
			unit->m_W_amp = W_amp;
		}
		unit->m_X_amp = X_amp;
		unit->m_Y_amp = Y_amp;
	} else {
		LOOP1(inNumSamples,
			float a = ZXP(inA);
			float b = ZXP(inB);
			float abdiff = a - b;
			ZXP(Wout) = (a + b) * W_amp;
			ZXP(Xout) = abdiff * X_amp;
			ZXP(Yout) = abdiff * Y_amp;
		);
	}
}

void BiPanB2_Ctor(BiPanB2 *unit)
{
	SETCALC(BiPanB2_next);

	float azimuth = unit->m_azimuth = ZIN0(2);
	float level = unit->m_level = ZIN0(3);

	int kSineSize = ft->mSineSize;
	int kSineMask = kSineSize - 1;

	long iazimuth = kSineMask & (long)(azimuth * (float)(kSineSize >> 1));
	float sina = -ft->mSine[iazimuth];

	iazimuth = kSineMask & (iazimuth + (kSineSize>>2));
	float cosa = ft->mSine[iazimuth];

	unit->m_W_amp = rsqrt2_f * level;
	unit->m_X_amp = cosa * level;
	unit->m_Y_amp = sina * level;

	BiPanB2_next(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////


/*
void calcPos(float pos, int numOutputs, float width, float orientation)
{
	float rwidth = 1.f / width;
	float range = numOutputs * rwidth;
	float rrange = 1.f / range;

	float zpos = pos * 0.5 * numOutputs + width * 0.5 + orientation;
	Print("pos %6.2g    rwidth %6.3g  range %6.3g  rrange %6.3g  zpos %6.3g\n",
		pos, rwidth, range, rrange, zpos);
	for (int i=0; i<numOutputs; ++i) {
		float amp;
		float chanpos = zpos - i;
		chanpos *= rwidth;
		float zchanpos = chanpos - range * floor(rrange * chanpos);
		if (zchanpos > 1.f) {
			amp = 0.f;
		} else {
			amp  = ft->mSine[(long)(4096.f * zchanpos)];
		}
		Print("    %d   chanpos %6.3g   zchanpos %6.3g   amp %g\n",
			i, chanpos, zchanpos, amp);
	}
}
*/

#ifdef NOVA_SIMD
void PanAz_next_ak_nova(PanAz *unit, int inNumSamples);
#endif

void PanAz_Ctor(PanAz *unit)
{
	if (INRATE(1) == calc_FullRate) {
		unit->m_chanamp = NULL;
		SETCALC(PanAz_next_aa);
	} else {
		int numOutputs = unit->mNumOutputs;
		for (int i=0; i<numOutputs; ++i)
			ZOUT0(i) = 0.f;

		unit->m_chanamp = (float*)RTAlloc(unit->mWorld, numOutputs*sizeof(float));
		if (!unit->m_chanamp) {
			Print("PanAz: RT memory allocation failed\n");
			SETCALC(ClearUnitOutputs);
			return;
		}

		std::fill_n(unit->m_chanamp, numOutputs, 0);

#ifdef NOVA_SIMD
		if (boost::alignment::is_aligned( BUFLENGTH, 16 ))
			SETCALC(PanAz_next_ak_nova);
		else
			SETCALC(PanAz_next_ak);
#else
		SETCALC(PanAz_next_ak);
#endif
	}
}

void PanAz_Dtor(PanAz *unit)
{
	if (unit->m_chanamp)
		RTFree(unit->mWorld, unit->m_chanamp);
}

void PanAz_next_ak(PanAz *unit, int inNumSamples)
{
	float pos = ZIN0(1);
	float level = ZIN0(2);
	float width = ZIN0(3);
	float orientation = ZIN0(4);

	int numOutputs = unit->mNumOutputs;
	float rwidth = sc_reciprocal( width );
	float range = numOutputs * rwidth;
	float rrange = sc_reciprocal( range );

	pos = pos * 0.5f * numOutputs + width * 0.5f + orientation;

	float *zin0 = ZIN(0);

	for (int i=0; i<numOutputs; ++i) {
		float *out = ZOUT(i);
		float nextchanamp;
		float chanpos = pos - i;
		chanpos *= rwidth;
		chanpos = chanpos - range * sc_floor(rrange * chanpos);
		if (chanpos > 1.f) {
			nextchanamp = 0.f;
		} else {
			nextchanamp  = level * ft->mSine[(long)(4096.f * chanpos)];
		}
		float chanamp = unit->m_chanamp[i];

		if (nextchanamp == chanamp) {
			if (nextchanamp == 0.f) {
				ZClear(inNumSamples, out);
			} else {
				float *in = zin0;
				LOOP1(inNumSamples,
					ZXP(out) = ZXP(in) * chanamp;
				)
			}
		} else {
			float chanampslope = CALCSLOPE(nextchanamp, chanamp);
			float *in = zin0;
			LOOP1(inNumSamples,
				ZXP(out) = ZXP(in) * chanamp;
				chanamp += chanampslope;
			)
			unit->m_chanamp[i] = nextchanamp;
		}
	}
}

#ifdef NOVA_SIMD
FLATTEN void PanAz_next_ak_nova(PanAz *unit, int inNumSamples)
{
	float pos = ZIN0(1);
	float level = ZIN0(2);
	float width = ZIN0(3);
	float orientation = ZIN0(4);

	int numOutputs = unit->mNumOutputs;
	float rwidth = sc_reciprocal( width );
	float range = numOutputs * rwidth;
	float rrange = sc_reciprocal( range );

	pos = pos * 0.5f * numOutputs + width * 0.5f + orientation;

	float * __restrict__ in = IN(0);
	float * __restrict__ chanamps = unit->m_chanamp;

	for (int i=0; i<numOutputs; ++i) {
		float chanamp = chanamps[i];
		float nextchanamp;
		float chanpos = pos - i;
		chanpos *= rwidth;
		chanpos = chanpos - range * sc_floor(rrange * chanpos);
		if (chanpos > 1.f) {
			nextchanamp = 0.f;
		} else {
			nextchanamp = level * ft->mSine[(long)(4096.f * chanpos)];
		}

		float *out = OUT(i);
		if (nextchanamp == chanamp) {
			if (nextchanamp == 0.f)
				nova::zerovec_simd(out, inNumSamples);
			else
				nova::times_vec_simd(out, in, chanamp, inNumSamples);
		} else {
			float chanampslope  = CALCSLOPE(nextchanamp, chanamp);
			nova::times_vec_simd(out, in, slope_argument(chanamp, chanampslope), inNumSamples);
			chanamps[i] = nextchanamp;
		}
	}
}
#endif

void PanAz_next_aa(PanAz *unit, int inNumSamples)
{
	float level = ZIN0(2);
	float width = ZIN0(3);
	float orientation = ZIN0(4);

	int numOutputs = unit->mNumOutputs;
	float rwidth = sc_reciprocal( width );
	float range = numOutputs * rwidth;
	float rrange = sc_reciprocal( range );


	// compute constant parts with which the pos has to be multiplied/added to respect numOutputs, width and orientation
	// see PanAz_next_ak for details
	float alignedPosFac = 0.5f * numOutputs;
	float alignedPosConst = width * 0.5f + orientation;


	float *zin0 = ZIN(0);
	float *pos = ZIN(1);

	for (int i=0; i<numOutputs; ++i) {
		float *out = ZOUT(i);

		float *in = zin0;
		float *thePos = pos;

		LOOP1(inNumSamples,
			float chanpos  = ((ZXP(thePos) * alignedPosFac + alignedPosConst) - i) * rwidth;
			chanpos = chanpos - range * sc_floor(rrange * chanpos);

			float chanamp;
			if (chanpos > 1.f) {
				chanamp = 0.f;
			} else {
				chanamp = level * ft->mSine[(long)(4096.f * chanpos)];
			}

			ZXP(out) = ZXP(in) * chanamp;

		)
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

void Rotate2_next_ak(Rotate2 *unit, int inNumSamples)
{
	float *xout = ZOUT(0);
	float *yout = ZOUT(1);
	float *xin = ZIN(0);
	float *yin = ZIN(1);
	float pos = ZIN0(2);
	float sint = unit->m_sint;
	float cost = unit->m_cost;

	if (pos != unit->m_pos) {
		int kSineSize = ft->mSineSize;
		int kSineMask = kSineSize - 1;

		int32 isinpos = kSineMask & (int32)(pos * (float)(kSineSize >> 1));
		int32 icospos = kSineMask & ((kSineSize>>2) + isinpos);

		float nextsint = unit->m_sint = ft->mSine[isinpos];
		float nextcost = unit->m_cost = ft->mSine[icospos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float sinslope = (nextsint - sint) * slopeFactor;
		float cosslope = (nextcost - cost) * slopeFactor;

		LOOP1(inNumSamples,
			float x = ZXP(xin);
			float y = ZXP(yin);
			ZXP(xout) = cost * x + sint * y;
			ZXP(yout) = cost * y - sint * x;
			sint += sinslope;
			cost += cosslope;
		);
		unit->m_pos = pos;
	} else {
		LOOP1(inNumSamples,
			float x = ZXP(xin);
			float y = ZXP(yin);
			ZXP(xout) = cost * x + sint * y;
			ZXP(yout) = cost * y - sint * x;
		);
	}
}

void Rotate2_Ctor(Rotate2 *unit)
{
	SETCALC(Rotate2_next_ak);

	unit->m_pos = ZIN0(2);
	int32 isinpos = 8191 & (int32)(4096.f * unit->m_pos);
	int32 icospos = 8191 & (2048 + isinpos);

	unit->m_sint = ft->mSine[isinpos];
	unit->m_cost = ft->mSine[icospos];

	Rotate2_next_ak(unit, 1);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void DecodeB2_Ctor(DecodeB2 *unit)
{
#if defined(NOVA_SIMD)
	if (boost::alignment::is_aligned( BUFLENGTH, 16 ))
		SETCALC(DecodeB2_next_nova);
	else
#endif
		SETCALC(DecodeB2_next);

	DecodeB2_next(unit, 1);

	float orientation = ZIN0(3);

	int numOutputs = unit->mNumOutputs;
	float angle = twopi_f / numOutputs;
	unit->m_cosa = cos(angle);
	unit->m_sina = sin(angle);
	unit->m_W_amp = 0.7071067811865476f;
	unit->m_X_amp = 0.5f * (float)cos(orientation * angle);
	unit->m_Y_amp = 0.5f * (float)sin(orientation * angle);
}

void DecodeB2_next(DecodeB2 *unit, int inNumSamples)
{
	float *Win0 = ZIN(0);
	float *Xin0 = ZIN(1);
	float *Yin0 = ZIN(2);

	float W_amp = unit->m_W_amp;
	float X_amp = unit->m_X_amp;
	float Y_amp = unit->m_Y_amp;
	float X_tmp;
	float cosa = unit->m_cosa;
	float sina = unit->m_sina;

	int numOutputs = unit->mNumOutputs;
	for (int i=0; i<numOutputs; ++i) {
		float *out = ZOUT(i);
		float *Win = Win0;
		float *Xin = Xin0;
		float *Yin = Yin0;
		LOOP1(inNumSamples,
			ZXP(out) = ZXP(Win) * W_amp + ZXP(Xin) * X_amp + ZXP(Yin) * Y_amp;
		);
		X_tmp = X_amp * cosa + Y_amp * sina;
		Y_amp = Y_amp * cosa - X_amp * sina;
		X_amp = X_tmp;
	}
}

#ifdef NOVA_SIMD
void DecodeB2_next_nova(DecodeB2 *unit, int inNumSamples)
{
	float *Win0 = IN(0);
	float *Xin0 = IN(1);
	float *Yin0 = IN(2);

	using namespace nova;
	vec<float> W_amp = unit->m_W_amp;
	vec<float> X_amp = unit->m_X_amp;
	vec<float> Y_amp = unit->m_Y_amp;
	vec<float> X_tmp;
	vec<float> cosa = unit->m_cosa;
	vec<float> sina = unit->m_sina;

	int numOutputs = unit->mNumOutputs;
	int vs = vec<float>::size;
	int loops = inNumSamples / vs;
	for (int i=0; i<numOutputs; ++i) {
		float *out = OUT(i);
		float *Win = Win0;
		float *Xin = Xin0;
		float *Yin = Yin0;

		for (int j = 0; j != loops; ++j) {
			vec<float> result, w, x, y;
			w.load_aligned(Win); x.load_aligned(Xin); y.load_aligned(Yin);
			result = w * W_amp + x * X_amp + y * Y_amp;
			result.store_aligned(out);
			out += vs; Win += vs; Xin += vs; Yin += vs;
		};

		X_tmp = X_amp * cosa + Y_amp * sina;
		Y_amp = Y_amp * cosa - X_amp * sina;
		X_amp = X_tmp;
	}
}
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////

PluginLoad(Pan)
{
	ft = inTable;

	DefineSimpleUnit(Pan2);
	DefineSimpleUnit(Pan4);
	DefineSimpleUnit(LinPan2);
	DefineSimpleCantAliasUnit(Balance2);
	DefineSimpleUnit(Rotate2);
	DefineSimpleUnit(XFade2);
	DefineSimpleUnit(LinXFade2);
	DefineSimpleUnit(PanB);
	DefineSimpleCantAliasUnit(PanB2);
	DefineSimpleUnit(BiPanB2);
	DefineDtorCantAliasUnit(PanAz);
	DefineSimpleCantAliasUnit(DecodeB2);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
