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
#include <windows.h>
# endif
#endif

#include "SC_PlugIn.h"

static InterfaceTable *ft;

struct KeyboardUGenGlobalState {
#ifdef __APPLE__
//	uint8 keys[16];
	KeyMap keys;
#else
	uint8 keys[32];
#endif
} gKeyStateGlobals;

struct KeyState : public Unit
{
	KeyboardUGenGlobalState* gstate;
	float m_y1, m_b1, m_lag;
};

//////////////////////////////////////////////////////////////////////////////////////////////////

extern "C"
{
	void KeyState_next(KeyState *unit, int inNumSamples);
	void KeyState_Ctor(KeyState *unit);
};

//////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __APPLE__

void* gstate_update_func(void* arg)
{
	KeyboardUGenGlobalState* gstate = &gKeyStateGlobals;
	for (;;) {
		Point p;
		GetKeys(gstate->keys);
		usleep(17000);
	}

	return 0;
}

#elif defined(_WIN32)

void* gstate_update_func(void* arg)
{
	POINT p;
	int mButton;
	KeyboardUGenGlobalState* gstate;

	gstate = &gKeyStateGlobals;

	/* // "KeyState" is disabled for now, on Windows...
	for(;;)	{
		//GetKey((long*)gstate->keys);
		::Sleep(17); // 17msec.
	}
	*/
	return 0;
}

# else
static Display * d = 0;
void* gstate_update_func(void* arg)
{
	KeyboardUGenGlobalState* gstate;
	Window r;
	struct timespec requested_time , remaining_time;

	requested_time.tv_sec = 0;
	requested_time.tv_nsec = 17000 * 1000;

	XInitThreads(); // x api is called by both mouse and keyboard ugens

	d = XOpenDisplay ( NULL );
	if (!d) return 0;

	gstate = &gKeyStateGlobals;

	for (;;) {
		XQueryKeymap ( d , (char *) (gstate->keys) );
		nanosleep ( &requested_time , &remaining_time );
	}

	return 0;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////

void KeyState_next(KeyState *unit, int inNumSamples)
{
	// minval, maxval, warp, lag
	uint8 *keys = (uint8*)unit->gstate->keys;
	int keynum = (int)ZIN0(0);
#ifdef __APPLE__
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

void KeyState_Ctor(KeyState *unit)
{
	SETCALC(KeyState_next);
	unit->gstate = &gKeyStateGlobals;
	unit->m_b1 = 0.f;
	unit->m_lag = 0.f;
	KeyState_next(unit, 1);
}

PluginLoad(KeyboardUGens)
{
	ft = inTable;

	pthread_t keybListenThread;
	pthread_create (&keybListenThread, NULL, gstate_update_func, (void*)0);

	DefineSimpleUnit(KeyState);
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
