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
#include <Carbon/Carbon.h>

static InterfaceTable *ft;


struct MacUGenGlobalState {
	float mouseX, mouseY;
	bool mouseButton;
	uint8 keys[16];
} gMacUGenGlobals;

struct MacUGen : public Unit
{
	MacUGenGlobalState* gstate;
	float m_y1, m_b1, m_lag;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

	void MouseX_next(MacUGen *unit, int inNumSamples);
	void MouseY_next(MacUGen *unit, int inNumSamples);
	void MouseButton_next(MacUGen *unit, int inNumSamples);

	void MouseX_Ctor(MacUGen *unit);
	void MouseY_Ctor(MacUGen *unit);
	void MouseButton_Ctor(MacUGen *unit);
};

//////////////////////////////////////////////////////////////////////////////////////////////////

void* gstate_update_func(void* arg)
{
	MacUGenGlobalState* gstate = &gMacUGenGlobals;
	RgnHandle rgn = GetGrayRgn();
	Rect screenBounds;
	GetRegionBounds(rgn, &screenBounds);
	float rscreenWidth = 1. / (screenBounds.right - screenBounds.left);
	float rscreenHeight = 1. / (screenBounds.bottom - screenBounds.top);
	for (;;) {
		Point p;
		GetGlobalMouse(&p);
		gstate->mouseX = (float)p.h * rscreenWidth;
		gstate->mouseY = (float)p.v * rscreenHeight;
		gstate->mouseButton = Button();
		GetKeys((long*)gstate->keys);
		usleep(17000);
	}
	
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MouseX_next(MacUGen *unit, int inNumSamples)
{	
	// minval, maxval, warp, lag

	float minval = ZIN0(0);
	float maxval = ZIN0(1);
	float warp = ZIN0(2);
	float lag = ZIN0(3);

	float y1 = unit->m_y1;
	float b1 = unit->m_b1;
	
	if (lag != unit->m_lag) {
		unit->m_b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
		unit->m_lag = lag;
	}
	float y0 = unit->gstate->mouseX; 
	if (warp == 0.0) {
		y0 = (maxval - minval) * y0 + minval;
	} else {
		y0 = pow(maxval/minval, y0) * minval;
	}
	ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
	unit->m_y1 = zapgremlins(y1);
}

void MouseX_Ctor(MacUGen *unit)
{	
	SETCALC(MouseX_next);
	unit->gstate = &gMacUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	MouseX_next(unit, 1);
}


void MouseY_next(MacUGen *unit, int inNumSamples)
{	
	// minval, maxval, warp, lag

	float minval = ZIN0(0);
	float maxval = ZIN0(1);
	float warp = ZIN0(2);
	float lag = ZIN0(3);

	float y1 = unit->m_y1;
	float b1 = unit->m_b1;
	
	if (lag != unit->m_lag) {
		unit->m_b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
		unit->m_lag = lag;
	}
	float y0 = unit->gstate->mouseY; 
	if (warp == 0.0) {
		y0 = (maxval - minval) * y0 + minval;
	} else {
		y0 = pow(maxval/minval, y0) * minval;
	}
	ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
	unit->m_y1 = zapgremlins(y1);
}

void MouseY_Ctor(MacUGen *unit)
{	
	SETCALC(MouseY_next);
	unit->gstate = &gMacUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	MouseY_next(unit, 1);
}



void MouseButton_next(MacUGen *unit, int inNumSamples)
{	
	// minval, maxval, warp, lag

	float minval = ZIN0(0);
	float maxval = ZIN0(1);
	float lag = ZIN0(2);

	float y1 = unit->m_y1;
	float b1 = unit->m_b1;
	
	if (lag != unit->m_lag) {
		unit->m_b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
		unit->m_lag = lag;
	}
	float y0 = unit->gstate->mouseButton ? maxval : minval; 
	ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
	unit->m_y1 = zapgremlins(y1);
}

void MouseButton_Ctor(MacUGen *unit)
{	
	SETCALC(MouseButton_next);
	unit->gstate = &gMacUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	MouseButton_next(unit, 1);
}


void KeyState_next(MacUGen *unit, int inNumSamples)
{	
	// minval, maxval, warp, lag
	uint8 *keys = unit->gstate->keys;
	int keynum = (int)ZIN0(0);
	int byte = (keynum >> 3) & 15;
	int bit = keynum & 7;
	int val = keys[byte] & (1 << bit);
		
	float minval = ZIN0(1);
	float maxval = ZIN0(2);
	float lag = ZIN0(3);

	float y1 = unit->m_y1;
	float b1 = unit->m_b1;
	
	if (lag != unit->m_lag) {
		unit->m_b1 = lag == 0.f ? 0.f : exp(log001 / (lag * unit->mRate->mSampleRate));
		unit->m_lag = lag;
	}
	float y0 = val ? maxval : minval; 
	ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
	unit->m_y1 = zapgremlins(y1);
}

void KeyState_Ctor(MacUGen *unit)
{	
	SETCALC(KeyState_next);
	unit->gstate = &gMacUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	KeyState_next(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////


void load(InterfaceTable *inTable)
{
	ft = inTable;
	
	pthread_t macThread;
	pthread_create (&macThread, NULL, gstate_update_func, (void*)0);

	DefineUnit("MouseX", sizeof(MacUGen), (UnitCtorFunc)&MouseX_Ctor, 0);
	DefineUnit("MouseY", sizeof(MacUGen), (UnitCtorFunc)&MouseY_Ctor, 0);
	DefineUnit("MouseButton", sizeof(MacUGen), (UnitCtorFunc)&MouseButton_Ctor, 0);
	DefineUnit("KeyState", sizeof(MacUGen), (UnitCtorFunc)&KeyState_Ctor, 0);
}
