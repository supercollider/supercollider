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

#if SC_DARWIN
#include <Carbon/Carbon.h>
#else
#include <time.h>
#include <X11/Intrinsic.h>
#endif

static InterfaceTable *ft;

struct UserInputUGenGlobalState {
	float mouseX, mouseY;
	bool mouseButton;
#if SC_DARWIN
	uint8 keys[16];
#else
	uint8 keys[32];
#endif
} gUserInputUGenGlobals;

struct UserInputUGen : public Unit
{
	UserInputUGenGlobalState* gstate;
	float m_y1, m_b1, m_lag;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

	void MouseX_next(UserInputUGen *unit, int inNumSamples);
	void MouseY_next(UserInputUGen *unit, int inNumSamples);
	void MouseButton_next(UserInputUGen *unit, int inNumSamples);

	void MouseX_Ctor(UserInputUGen *unit);
	void MouseY_Ctor(UserInputUGen *unit);
	void MouseButton_Ctor(UserInputUGen *unit);
};

//////////////////////////////////////////////////////////////////////////////////////////////////

#if SC_DARWIN

void* gstate_update_func(void* arg)
{
	UserInputUGenGlobalState* gstate = &gUserInputUGenGlobals;
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

#else

void* gstate_update_func(void* arg)
{
  UserInputUGenGlobalState* gstate ;
  Display *d ;		
  Window r ;			
  Window rep_root , rep_child ;
  XWindowAttributes attributes ;
  int rep_rootx , rep_rooty ;
  unsigned int rep_mask ;
  int dx , dy ;
  float r_width ;
  float r_height ;
  struct timespec requested_time , remaining_time ;

  requested_time.tv_sec = 0 ;
  requested_time.tv_nsec = 17000 * 1000 ;

  d = XOpenDisplay ( NULL ) ;
  if (!d) return 0;
  
  r = DefaultRootWindow ( d ) ;
  XGetWindowAttributes ( d , r , &attributes ) ;
  r_width = 1.0 / (float)attributes.width ;
  r_height = 1.0 / (float)attributes.height ;
  
  gstate = &gUserInputUGenGlobals ;
	
  for (;;) {
    
    XQueryPointer ( d , r , 
		    &rep_root , &rep_child ,
		    &rep_rootx , &rep_rooty , 
		    &dx , &dy ,
		    &rep_mask ) ;
    
    gstate->mouseX = (float)dx * r_width ;
    gstate->mouseY = 1.0 - ( (float)dy * r_height ) ;
    
    gstate->mouseButton = (bool) ( rep_mask & Button1Mask ) ;

    XQueryKeymap ( d , (char *) (gstate->keys) ) ;

    nanosleep ( &requested_time , &remaining_time ) ;
  }
  
  return 0;
}

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////

void MouseX_next(UserInputUGen *unit, int inNumSamples)
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

void MouseX_Ctor(UserInputUGen *unit)
{	
	SETCALC(MouseX_next);
	unit->gstate = &gUserInputUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	MouseX_next(unit, 1);
}


void MouseY_next(UserInputUGen *unit, int inNumSamples)
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

void MouseY_Ctor(UserInputUGen *unit)
{	
	SETCALC(MouseY_next);
	unit->gstate = &gUserInputUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	MouseY_next(unit, 1);
}



void MouseButton_next(UserInputUGen *unit, int inNumSamples)
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

void MouseButton_Ctor(UserInputUGen *unit)
{	
	SETCALC(MouseButton_next);
	unit->gstate = &gUserInputUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	MouseButton_next(unit, 1);
}


void KeyState_next(UserInputUGen *unit, int inNumSamples)
{	
	// minval, maxval, warp, lag
	uint8 *keys = unit->gstate->keys;
	int keynum = (int)ZIN0(0);
#if SC_DARWIN
	int byte = (keynum >> 3) & 15;
#else
	int byte = (keynum >> 3) & 31;
#endif
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

void KeyState_Ctor(UserInputUGen *unit)
{	
	SETCALC(KeyState_next);
	unit->gstate = &gUserInputUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	KeyState_next(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

// example of implementing a plug in command with async execution.

bool cmdStage2(World* world, void* inUserData)
{
	Print("cmdStage2 %08X\n", inUserData);
	return true;
}

bool cmdStage3(World* world, void* inUserData)
{
	Print("cmdStage3 %08X\n", inUserData);
	return true;
}

bool cmdStage4(World* world, void* inUserData)
{
	Print("cmdStage4 %08X\n", inUserData);
	return true;
}

void cmdCleanup(World* world, void* inUserData)
{
	Print("cmdCleanup %08X\n", inUserData);
}

void cmdDemoFunc(World *inWorld, void* inUserData, struct sc_msg_iter *args, void *replyAddr)
{
	Print("->cmdDemoFunc %08X\n", inUserData);
	
	// ..get data from args..
	
	DoAsynchronousCommand(inWorld, replyAddr, "cmdDemoFunc", inUserData,
					cmdStage2, cmdStage3, cmdStage4, cmdCleanup,
					0, 0);
	
	Print("<-cmdDemoFunc\n");
}

void load(InterfaceTable *inTable)
{
	ft = inTable;
	
	pthread_t macThread;
	pthread_create (&macThread, NULL, gstate_update_func, (void*)0);

	DefineUnit("MouseX", sizeof(UserInputUGen), (UnitCtorFunc)&MouseX_Ctor, 0, 0);
	DefineUnit("MouseY", sizeof(UserInputUGen), (UnitCtorFunc)&MouseY_Ctor, 0, 0);
	DefineUnit("MouseButton", sizeof(UserInputUGen), (UnitCtorFunc)&MouseButton_Ctor, 0, 0);
	DefineUnit("KeyState", sizeof(UserInputUGen), (UnitCtorFunc)&KeyState_Ctor, 0, 0);
	
	DefinePlugInCmd("pluginCmdDemo", cmdDemoFunc, (void*)0x00012345);
}
