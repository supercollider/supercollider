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


#ifdef __APPLE__
#include <Carbon/Carbon.h>
#include <unistd.h>
#else
# ifndef _WIN32
#  include <time.h>
#  include <X11/Intrinsic.h>
# else
# endif
#endif

#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct MouseUGenGlobalState {
	float mouseX, mouseY;
	bool mouseButton;
} gMouseUGenGlobals;

struct MouseInputUGen : public Unit
{
	MouseUGenGlobalState* gstate;
	float m_y1, m_b1, m_lag;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void load(InterfaceTable *inTable);

	void MouseX_next(MouseInputUGen *unit, int inNumSamples);
	void MouseY_next(MouseInputUGen *unit, int inNumSamples);
	void MouseButton_next(MouseInputUGen *unit, int inNumSamples);

	void MouseX_Ctor(MouseInputUGen *unit);
	void MouseY_Ctor(MouseInputUGen *unit);
	void MouseButton_Ctor(MouseInputUGen *unit);
};

//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __APPLE__
# if (__x86_64__)

void* gstate_update_func(void* arg)
{
	MouseUGenGlobalState* gstate = &gMouseUGenGlobals;

	CGDirectDisplayID display = kCGDirectMainDisplay; // to grab the main display ID
	CGRect bounds = CGDisplayBounds(display);
	float rscreenWidth = 1. / bounds.size.width;
	float rscreenHeight = 1. / bounds.size.height;
	for (;;) {
		HIPoint point;
		HICoordinateSpace space = 2;
		HIGetMousePosition(space, NULL, &point);

		gstate->mouseX = point.x * rscreenWidth; //(float)p.h * rscreenWidth;
		gstate->mouseY = point.y * rscreenHeight; //(float)p.v * rscreenHeight;
		gstate->mouseButton = Button();
		usleep(17000);
	}

	return 0;
}

# else

void* gstate_update_func(void* arg)
{
	MouseUGenGlobalState* gstate = &gMouseUGenGlobals;
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
		usleep(17000);
	}

	return 0;
}

# endif

#elif defined (_WIN32)

void* gstate_update_func(void* arg)
{
	POINT p;
	int mButton;
	MouseUGenGlobalState* gstate;

	if(GetSystemMetrics(SM_SWAPBUTTON))
		mButton = VK_RBUTTON; // if  swapped
	else
		mButton = VK_LBUTTON; // not swapped (normal)

	int screenWidth  = GetSystemMetrics( SM_CXSCREEN );
	int screenHeight = GetSystemMetrics( SM_CYSCREEN );
	// default: SM_CX/CYSCREEN gets the size of a primary screen.
	// lines uncommented below are just for a specially need on multi-display.
	//int screenWidth  = GetSystemMetrics( SM_CXVIRTUALSCREEN );
	//int screenHeight = GetSystemMetrics( SM_CYVIRTUALSCREEN );
	float r_screenWidth  = 1.f / (float)(screenWidth  -1);
	float r_screenHeight = 1.f / (float)(screenHeight -1);

	gstate = &gMouseUGenGlobals;

	for(;;) {
		GetCursorPos(&p);
		gstate->mouseX = (float)p.x * r_screenWidth;
		gstate->mouseY = 1.f - (float)p.y * r_screenHeight;
		gstate->mouseButton = (GetKeyState(mButton) < 0);
		::Sleep(17); // 17msec.
	}
	return 0;
}

# else
static Display * d = 0;
void* gstate_update_func(void* arg)
{
	MouseUGenGlobalState* gstate;
	Window r;
	Window rep_root, rep_child;
	XWindowAttributes attributes;
	int rep_rootx, rep_rooty ;
	unsigned int rep_mask;
	int dx, dy;
	float r_width;
	float r_height;
	struct timespec requested_time, remaining_time;

	requested_time.tv_sec = 0;
	requested_time.tv_nsec = 17000 * 1000;

	d = XOpenDisplay ( NULL );
	if (!d) return 0;

	r = DefaultRootWindow ( d );
	XGetWindowAttributes ( d, r, &attributes );
	r_width = 1.0 / (float)attributes.width;
	r_height = 1.0 / (float)attributes.height;

	gstate = &gMouseUGenGlobals;

	for (;;) {
		XQueryPointer ( d, r,
				&rep_root, &rep_child,
				&rep_rootx, &rep_rooty,
				&dx, &dy,
				&rep_mask);

		gstate->mouseX = (float)dx * r_width;
		gstate->mouseY = 1.f - ( (float)dy * r_height );

		gstate->mouseButton = (bool) ( rep_mask & Button1Mask );

		nanosleep ( &requested_time , &remaining_time );
	}

	return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////

void MouseX_next(MouseInputUGen *unit, int inNumSamples)
{
	// minval, maxval, warp, lag

	float minval = ZIN0(0);
	float maxval = ZIN0(1);
	float warp = ZIN0(2);
	float lag = ZIN0(3);

	float y1 = unit->m_y1;
	float b1 = unit->m_b1;

	if (lag != unit->m_lag) {
		unit->m_b1 = lag == 0.f ? 0.f : (float)exp(log001 / (lag * unit->mRate->mSampleRate));
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

void MouseX_Ctor(MouseInputUGen *unit)
{
	SETCALC(MouseX_next);
	unit->gstate = &gMouseUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	MouseX_next(unit, 1);
}


void MouseY_next(MouseInputUGen *unit, int inNumSamples)
{
	// minval, maxval, warp, lag

	float minval = ZIN0(0);
	float maxval = ZIN0(1);
	float warp = ZIN0(2);
	float lag = ZIN0(3);

	float y1 = unit->m_y1;
	float b1 = unit->m_b1;

	if (lag != unit->m_lag) {
		unit->m_b1 = lag == 0.f ? 0.f : (float)exp(log001 / (lag * unit->mRate->mSampleRate));
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

void MouseY_Ctor(MouseInputUGen *unit)
{
	SETCALC(MouseY_next);
	unit->gstate = &gMouseUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	MouseY_next(unit, 1);
}


void MouseButton_next(MouseInputUGen *unit, int inNumSamples)
{
	// minval, maxval, warp, lag

	float minval = ZIN0(0);
	float maxval = ZIN0(1);
	float lag = ZIN0(2);

	float y1 = unit->m_y1;
	float b1 = unit->m_b1;

	if (lag != unit->m_lag) {
		unit->m_b1 = lag == 0.f ? 0.f : (float)exp(log001 / (lag * unit->mRate->mSampleRate));
		unit->m_lag = lag;
	}
	float y0 = unit->gstate->mouseButton ? maxval : minval;
	ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
	unit->m_y1 = zapgremlins(y1);
}

void MouseButton_Ctor(MouseInputUGen *unit)
{
	SETCALC(MouseButton_next);
	unit->gstate = &gMouseUGenGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	MouseButton_next(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////

// example of implementing a plug in command with async execution.

struct MyPluginData // data for the global instance of the plugin
{
	float a, b;
};

struct MyCmdData // data for each command
{
	MyPluginData* myPlugin;
	float x, y;
	char *name;
};

MyPluginData gMyPlugin; // global

bool cmdStage2(World* world, void* inUserData)
{
	// user data is the command.
	MyCmdData* myCmdData = (MyCmdData*)inUserData;

	// just print out the values
	Print("cmdStage2 a %g  b %g  x %g  y %g  name %s\n",
		myCmdData->myPlugin->a, myCmdData->myPlugin->b,
		myCmdData->x, myCmdData->y,
		myCmdData->name);

	return true;
}

bool cmdStage3(World* world, void* inUserData)
{
	// user data is the command.
	MyCmdData* myCmdData = (MyCmdData*)inUserData;

	// just print out the values
	Print("cmdStage3 a %g  b %g  x %g  y %g  name %s\n",
		myCmdData->myPlugin->a, myCmdData->myPlugin->b,
		myCmdData->x, myCmdData->y,
		myCmdData->name);

	// scsynth will perform completion message after this returns
	return true;
}

bool cmdStage4(World* world, void* inUserData)
{
	// user data is the command.
	MyCmdData* myCmdData = (MyCmdData*)inUserData;

	// just print out the values
	Print("cmdStage4 a %g  b %g  x %g  y %g  name %s\n",
		myCmdData->myPlugin->a, myCmdData->myPlugin->b,
		myCmdData->x, myCmdData->y,
		myCmdData->name);

	// scsynth will send /done after this returns
	return true;
}

void cmdCleanup(World* world, void* inUserData)
{
	// user data is the command.
	MyCmdData* myCmdData = (MyCmdData*)inUserData;

	Print("cmdCleanup a %g  b %g  x %g  y %g  name %s\n",
		myCmdData->myPlugin->a, myCmdData->myPlugin->b,
		myCmdData->x, myCmdData->y,
		myCmdData->name);

	RTFree(world, myCmdData->name); // free the string
	RTFree(world, myCmdData); // free command data
	// scsynth will delete the completion message for you.
}

void cmdDemoFunc(World *inWorld, void* inUserData, struct sc_msg_iter *args, void *replyAddr)
{
	Print("->cmdDemoFunc %08X\n", inUserData);

	// user data is the plug-in's user data.
	MyPluginData* thePlugInData = (MyPluginData*)inUserData;

	// allocate command data, free it in cmdCleanup.
	MyCmdData* myCmdData = (MyCmdData*)RTAlloc(inWorld, sizeof(MyCmdData));
	myCmdData->myPlugin = thePlugInData;

	// ..get data from args..
	myCmdData->x = 0.;
	myCmdData->y = 0.;
	myCmdData->name = 0;

	// float arguments
	myCmdData->x = args->getf();
	myCmdData->y = args->getf();

	// how to pass a string argument:
	const char *name = args->gets(); // get the string argument
	if (name) {
		myCmdData->name = (char*)RTAlloc(inWorld, strlen(name)+1); // allocate space, free it in cmdCleanup.
		strcpy(myCmdData->name, name); // copy the string
	}

	// how to pass a completion message
	int msgSize = args->getbsize();
	char* msgData = 0;
	if (msgSize) {
		// allocate space for completion message
		// scsynth will delete the completion message for you.
		msgData = (char*)RTAlloc(inWorld, msgSize);
		args->getb(msgData, msgSize); // copy completion message.
	}

	DoAsynchronousCommand(inWorld, replyAddr, "cmdDemoFunc", (void*)myCmdData,
						  (AsyncStageFn)cmdStage2,
						  (AsyncStageFn)cmdStage3,
						  (AsyncStageFn)cmdStage4,
						  cmdCleanup,
						  msgSize, msgData);

	Print("<-cmdDemoFunc\n");
}

/*
to test the above, send the server these commands:


SynthDef(\sine, { Out.ar(0, SinOsc.ar(800,0,0.2)) }).load(s);
s.sendMsg(\cmd, \pluginCmdDemo, 7, 9, \mno, [\s_new, \sine, 900, 0, 0]);
s.sendMsg(\n_free, 900);
s.sendMsg(\cmd, \pluginCmdDemo, 7, 9, \mno);
s.sendMsg(\cmd, \pluginCmdDemo, 7, 9);
s.sendMsg(\cmd, \pluginCmdDemo, 7);
s.sendMsg(\cmd, \pluginCmdDemo);

*/

void load(InterfaceTable *inTable)
{
	ft = inTable;

	pthread_t mouseListenThread;
	pthread_create (&mouseListenThread, NULL, gstate_update_func, (void*)0);

	DefineUnit("MouseX", sizeof(MouseInputUGen), (UnitCtorFunc)&MouseX_Ctor, 0, 0);
	DefineUnit("MouseY", sizeof(MouseInputUGen), (UnitCtorFunc)&MouseY_Ctor, 0, 0);
	DefineUnit("MouseButton", sizeof(MouseInputUGen), (UnitCtorFunc)&MouseButton_Ctor, 0, 0);

	// define a plugin command - example code
	gMyPlugin.a = 1.2f;
	gMyPlugin.b = 3.4f;
	DefinePlugInCmd("pluginCmdDemo", cmdDemoFunc, (void*)&gMyPlugin);
}

#ifdef __GNUC__
#if !defined(__APPLE__) && !defined(_WIN32)
static void __attribute__ ((destructor)) finalize(void)
{
	if (d)
		XCloseDisplay(d);
}
#endif
#endif
