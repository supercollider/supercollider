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

static InterfaceTable *ft;

struct LinPan2 : public Unit
{
	float m_pos, m_level, m_leftamp, m_rightamp;
};

struct Balance2 : public Unit
{
	float m_pos, m_level, m_leftamp, m_rightamp;
};

struct XFade2 : public Unit
{
	float m_pos, m_level, m_leftamp, m_rightamp;
};

struct LinXFade2 : public Unit
{
	float m_pos, m_level, m_amp;
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

struct PanAz : public Unit
{
	float m_chanamp[16];
};


//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

	void LinPan2_next_ak(LinPan2 *unit, int inNumSamples);
	void LinPan2_next_aa(LinPan2 *unit, int inNumSamples);
	void LinPan2_Ctor(LinPan2* unit);

	void Balance2_next_ak(Balance2 *unit, int inNumSamples);
	void Balance2_next_aa(Balance2 *unit, int inNumSamples);
	void Balance2_Ctor(Balance2* unit);

	void XFade2_next_ak(XFade2 *unit, int inNumSamples);
	void XFade2_next_aa(XFade2 *unit, int inNumSamples);
	void XFade2_Ctor(XFade2* unit);

	void LinXFade2_next_k(LinXFade2 *unit, int inNumSamples);
	void LinXFade2_next_a(LinXFade2 *unit, int inNumSamples);
	void LinXFade2_Ctor(LinXFade2* unit);

	void Pan2_next_ak(Pan2 *unit, int inNumSamples);
	void Pan2_next_aa(Pan2 *unit, int inNumSamples);
	void Pan2_Ctor(Pan2* unit);

	void Pan4_next(Pan4 *unit, int inNumSamples);
	void Pan4_Ctor(Pan4* unit);

	void PanB_next(PanB *unit, int inNumSamples);
	void PanB_Ctor(PanB* unit);

	void PanB2_next(PanB2 *unit, int inNumSamples);
	void PanB2_Ctor(PanB2* unit);

	void PanAz_next(PanAz *unit, int inNumSamples);
	void PanAz_Ctor(PanAz* unit);

}

//////////////////////////////////////////////////////////////////////////////////////////////////

void LinPan2_Ctor(LinPan2 *unit)
{	
	if (INRATE(1) == calc_FullRate) {
		SETCALC(LinPan2_next_aa);
	} else {
		SETCALC(LinPan2_next_ak);
	}
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
		float pan = pos * 0.5 + 0.5;
		float nextrightamp = level * pan;
		float nextleftamp  = level - nextrightamp;

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		LOOP(inNumSamples, 
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;	
			leftamp += leftampslope;
			rightamp += rightampslope;
		);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = leftamp;
		unit->m_rightamp = rightamp;
	} else {
		LOOP(inNumSamples, 
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;	
		);
	}
}

void LinPan2_next_aa(LinPan2 *unit, int inNumSamples)
{
	float *leftout = ZOUT(0);
	float *rightout = ZOUT(1);
	float *in = ZIN(0);
	float *pos = ZIN(1);
	float nextlevel = ZIN0(2);
	float level = unit->m_level;
	float levelSlope = (nextlevel - level) * unit->mRate->mSlopeFactor;
	
	LOOP(inNumSamples, 
		float pan = ZXP(pos) * 0.5 + 0.5;
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
		SETCALC(Balance2_next_ak);
	}
	unit->m_pos = ZIN0(2);
	unit->m_level = ZIN0(3);
	int32 ipos = (int32)(1024.f * unit->m_pos + 1024.f);
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
		int32 ipos = (int32)(1024.f * pos + 1024.f);
		ipos = sc_clip(ipos, 0, 2048);
		
		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		LOOP(inNumSamples, 
			ZXP(leftout)  = ZXP(leftin) * leftamp;
			ZXP(rightout) = ZXP(rightin) * rightamp;	
			leftamp += leftampslope;
			rightamp += rightampslope;
		);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = leftamp;
		unit->m_rightamp = rightamp;
	} else {
		LOOP(inNumSamples, 
			ZXP(leftout)  = ZXP(leftin) * leftamp;
			ZXP(rightout) = ZXP(rightin) * rightamp;	
		);
	}
}

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
		LOOP(inNumSamples, 
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f);
			ipos = sc_clip(ipos, 0, 2048);
			
			float leftamp  = level * sineTable[2048 - ipos];
			float rightamp = level * sineTable[ipos];
			ZXP(leftout)  = ZXP(leftin) * leftamp;
			ZXP(rightout) = ZXP(rightin) * rightamp;	
			level += levelSlope;	
		);
		unit->m_level = level;
	} else {
		LOOP(inNumSamples, 
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f);
			ipos = sc_clip(ipos, 0, 2048);
			
			float leftamp  = level * sineTable[2048 - ipos];
			float rightamp = level * sineTable[ipos];
			ZXP(leftout)  = ZXP(leftin) * leftamp;
			ZXP(rightout) = ZXP(rightin) * rightamp;	
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void XFade2_Ctor(XFade2 *unit)
{	
	if (INRATE(1) == calc_FullRate) {
		SETCALC(XFade2_next_aa);
	} else {
		SETCALC(XFade2_next_ak);
	}
	unit->m_pos = ZIN0(1);
	unit->m_level = ZIN0(2);
	int32 ipos = (int32)(1024.f * unit->m_pos + 1024.f);
	ipos = sc_clip(ipos, 0, 2048);
	
	unit->m_leftamp  = unit->m_level * ft->mSine[2048 - ipos];
	unit->m_rightamp = unit->m_level * ft->mSine[ipos];
	XFade2_next_aa(unit, 1);
}

void XFade2_next_ak(XFade2 *unit, int inNumSamples)
{
	float *leftout = ZOUT(0);
	float *rightout = ZOUT(1);
	float *in = ZIN(0);
	float pos = ZIN0(1);
	float level = ZIN0(2);
	float leftamp =  unit->m_leftamp;
	float rightamp = unit->m_rightamp;	

	if (pos != unit->m_pos || unit->m_level != level) {
		int32 ipos = (int32)(1024.f * pos + 1024.f);
		ipos = sc_clip(ipos, 0, 2048);
		
		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		LOOP(inNumSamples, 
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;	
			leftamp += leftampslope;
			rightamp += rightampslope;
		);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = leftamp;
		unit->m_rightamp = rightamp;
	} else {
		LOOP(inNumSamples, 
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;	
		);
	}
}

void XFade2_next_aa(XFade2 *unit, int inNumSamples)
{
	float *leftout = ZOUT(0);
	float *rightout = ZOUT(1);
	float *in = ZIN(0);
	float *pos = ZIN(2);
	float nextlevel = ZIN0(2);
	float level = unit->m_level;
	
	float *sineTable = ft->mSine;
	if (level != nextlevel) {
		float levelSlope = (nextlevel - level) * unit->mRate->mSlopeFactor;
		LOOP(inNumSamples, 
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f);
			ipos = sc_clip(ipos, 0, 2048);
			
			float leftamp  = level * sineTable[2048 - ipos];
			float rightamp = level * sineTable[ipos];
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;	
			level += levelSlope;	
		);
		unit->m_level = level;
	} else {
		LOOP(inNumSamples, 
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f);
			ipos = sc_clip(ipos, 0, 2048);
			
			float leftamp  = level * sineTable[2048 - ipos];
			float rightamp = level * sineTable[ipos];
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;	
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////

void LinXFade2_Ctor(LinXFade2 *unit)
{	
	if (INRATE(2) == calc_FullRate) {
		SETCALC(LinXFade2_next_a);
	} else {
		SETCALC(LinXFade2_next_k);
	}
	unit->m_pos = ZIN0(2);
	unit->m_level = ZIN0(3);
	unit->m_pos = sc_clip(unit->m_pos, -1.f, 1.f);
	unit->m_amp = unit->m_level * (unit->m_pos * 0.5 + 0.5);
	
	LinXFade2_next_a(unit, 1);
}

void LinXFade2_next_k(LinXFade2 *unit, int inNumSamples)
{
	float *out = ZOUT(0);
	float *leftin = ZIN(0);
	float *rightin = ZIN(1);
	float pos = ZIN0(2);
	float level = ZIN0(3);
	float amp = unit->m_amp;

	if (pos != unit->m_pos || unit->m_level != level) {
		pos = sc_clip(pos, -1.f, 1.f);
		
		float nextamp  = level * (pos * 0.5 + 0.5);
		float amp_slope  = (nextamp - amp) * unit->mRate->mSlopeFactor;

		LOOP(inNumSamples, 
			float l = ZXP(leftin);
			float r = ZXP(rightin);
			ZXP(out) = l + amp * (r - l);
			amp += amp_slope;
		);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_amp = amp;
	} else {
		LOOP(inNumSamples, 
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
	float nextlevel = ZIN0(3);
	float level = unit->m_level;

	if (level != nextlevel) {
		
		float level_slope  = (nextlevel - level) * unit->mRate->mSlopeFactor;
		float pos, amp;
		LOOP(inNumSamples, 
			pos = ZXP(posp);
			pos = sc_clip(pos, -1.f, 1.f);
			amp = level * (pos * 0.5 + 0.5);
			float l = ZXP(leftin);
			float r = ZXP(rightin);
			ZXP(out) = l + amp * (r - l);
			level += level_slope;
		);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_amp = amp;
	} else {
		LOOP(inNumSamples, 
			float pos = ZXP(posp);
			pos = sc_clip(pos, -1.f, 1.f);
			float amp = level * (pos * 0.5 + 0.5);
			float l = ZXP(leftin);
			float r = ZXP(rightin);
			ZXP(out) = l + amp * (r - l);
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pan2_Ctor(Pan2 *unit)
{	
	if (INRATE(1) == calc_FullRate) {
		SETCALC(Pan2_next_aa);
	} else {
		SETCALC(Pan2_next_ak);
	}
	unit->m_pos = ZIN0(1);
	unit->m_level = ZIN0(2);
	int32 ipos = (int32)(1024.f * unit->m_pos + 1024.f);
	ipos = sc_clip(ipos, 0, 2048);
	
	unit->m_leftamp  = unit->m_level * ft->mSine[2048 - ipos];
	unit->m_rightamp = unit->m_level * ft->mSine[ipos];
	Pan2_next_aa(unit, 1);
}

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
		int32 ipos = (int32)(1024.f * pos + 1024.f);
		ipos = sc_clip(ipos, 0, 2048);
		
		float nextleftamp  = level * ft->mSine[2048 - ipos];
		float nextrightamp = level * ft->mSine[ipos];

		float slopeFactor = unit->mRate->mSlopeFactor;
		float leftampslope  = (nextleftamp  - leftamp)  * slopeFactor;
		float rightampslope = (nextrightamp - rightamp) * slopeFactor;

		LOOP(inNumSamples, 
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;	
			leftamp += leftampslope;
			rightamp += rightampslope;
		);
		unit->m_pos = pos;
		unit->m_level = level;
		unit->m_leftamp = leftamp;
		unit->m_rightamp = rightamp;
	} else {
		LOOP(inNumSamples, 
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;	
		);
	}
}

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
		
		LOOP(inNumSamples, 
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f);
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
		LOOP(inNumSamples, 
			int32 ipos = (int32)(1024.f * ZXP(pos) + 1024.f);
			ipos = sc_clip(ipos, 0, 2048);
			
			float leftamp  = level * ft->mSine[2048 - ipos];
			float rightamp = level * ft->mSine[ipos];
			float zin = ZXP(in);
			ZXP(leftout)  = zin * leftamp;
			ZXP(rightout) = zin * rightamp;	
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void Pan4_Ctor(Pan4 *unit)
{	
	SETCALC(Pan4_next);
	Pan4_next(unit, 1);
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

		int32 ixpos = (int32)(1024.f * xpos + 1024.f);
		ixpos = sc_clip(ixpos, 0, 2048);
		float leftamp  = ft->mSine[2048 - ixpos];
		float rightamp = ft->mSine[ixpos];
		
		int32 iypos = (int32)(1024.f * ypos + 1024.f);
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
		
		LOOP(inNumSamples, 
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
		LOOP(inNumSamples, 
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
		
		long iazimuth   = kSineMask & (long)(azimuth   * (kSineSize * 0.5));
		long ielevation = kSineMask & (long)(elevation * (kSineSize * 0.25));
		float sina = ft->mSine[iazimuth];
		float sinb = ft->mSine[ielevation];
		
		iazimuth   = kSineMask & (iazimuth   + (kSineSize>>2));
		ielevation = kSineMask & (ielevation + (kSineSize>>2));
		float cosa = ft->mSine[iazimuth];
		float cosb = ft->mSine[ielevation];
		
		float next_W_amp = rsqrt2 * level;
		float next_X_amp = cosa * cosb * level;
		float next_Y_amp = sina * cosb * level;
		float next_Z_amp = sinb * level;
		
		float W_slope = CALCSLOPE(next_W_amp, W_amp);
		float X_slope = CALCSLOPE(next_X_amp, X_amp);
		float Y_slope = CALCSLOPE(next_Y_amp, Y_amp);
		float Z_slope = CALCSLOPE(next_Z_amp, Z_amp);
		
		LOOP(inNumSamples, 
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
		LOOP(inNumSamples, 
			float z = ZXP(in);
			ZXP(Wout) = z * W_amp;
			ZXP(Xout) = z * X_amp;
			ZXP(Yout) = z * Y_amp;
			ZXP(Zout) = z * Z_amp;
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void PanB2_Ctor(PanB2 *unit)
{
	SETCALC(PanB2_next);
	PanB2_next(unit, 1);
}

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
		
		long iazimuth   = kSineMask & (long)(azimuth   * (kSineSize * 0.5));
		float sina = ft->mSine[iazimuth];
		
		iazimuth   = kSineMask & (iazimuth   + (kSineSize>>2));
		float cosa = ft->mSine[iazimuth];
		
		float next_W_amp = rsqrt2 * level;
		float next_X_amp = cosa * level;
		float next_Y_amp = sina * level;
		
		float W_slope = CALCSLOPE(next_W_amp, W_amp);
		float X_slope = CALCSLOPE(next_X_amp, X_amp);
		float Y_slope = CALCSLOPE(next_Y_amp, Y_amp);
		
		LOOP(inNumSamples, 
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
		LOOP(inNumSamples, 
			float z = ZXP(in);
			ZXP(Wout) = z * W_amp;
			ZXP(Xout) = z * X_amp;
			ZXP(Yout) = z * Y_amp;
		);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////

void PanAz_Ctor(PanAz *unit)
{	
	SETCALC(PanAz_next);
	ZOUT0(0) = 0.f;
}

void PanAz_next(PanAz *unit, int inNumSamples)
{
	float pos = ZIN0(1);
	float level = ZIN0(2);
	float width = ZIN0(3);
	
	int numOutputs = unit->mNumOutputs;
	float rwidth = 1.f / width;
	float range = numOutputs * rwidth;
	float rrange = 1.f / range;
	
	pos *= numOutputs;
	
	for (int i=0; i<numOutputs; ++i) {
		float *out = ZOUT(i);
		float nextchanamp;
		float chanpos = pos - i;
		chanpos *= rwidth;
		chanpos = chanpos - range * floor(rrange * chanpos); 
		if (chanpos > 1.f) {
			nextchanamp = 0.f;
		} else {
			nextchanamp  = level * ft->mSine[(long)(4096.f * chanpos)];
		}
		float chanamp = unit->m_chanamp[i];

		if (nextchanamp == chanamp) {
			if (nextchanamp == 0.f) {
				Clear(inNumSamples, out);
			} else {
				float *in = ZIN(0);
				LOOP(inNumSamples, 
					ZXP(out) = ZXP(in) * chanamp;
				)
			}
		} else {
			float chanampslope  = CALCSLOPE(nextchanamp, chanamp);
			float *in = ZIN(0);
			LOOP(inNumSamples, 
				ZXP(out) = ZXP(in) * chanamp;
				chanamp += chanampslope;
			)
			unit->m_chanamp[i] = nextchanamp;
		}
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////////////

void load(InterfaceTable *inTable)
{
	ft = inTable;

	DefineSimpleUnit(Pan2);
	DefineSimpleUnit(Pan4);
	DefineSimpleUnit(LinPan2);
	DefineSimpleUnit(Balance2);
	DefineSimpleUnit(XFade2);
	DefineSimpleUnit(LinXFade2);
	DefineSimpleUnit(PanB);
	DefineSimpleUnit(PanB2);
	DefineSimpleUnit(PanAz);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
