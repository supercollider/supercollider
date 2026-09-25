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

// ********** this version for windows and linux. for mac see UIUGens.mm

#include <SC_Lock.h>

#include <atomic>

#ifndef _WIN32
#    include <X11/Intrinsic.h>
#else
#    include "SC_Win32Utils.h"
#    include <windows.h>
#endif


#include "SC_PlugIn.h"

static InterfaceTable* ft;

struct KeyboardUGenGlobalState {
    uint8 keys[32];
} gKeyStateGlobals;

struct KeyState : public Unit {
    float m_y1, m_b1, m_lag;
};


struct MouseUGenGlobalState {
    float mouseX, mouseY;
    bool mouseButton;
} gMouseUGenGlobals;

struct MouseInputUGen : public Unit {
    float m_y1, m_b1, m_lag;
};


//////////////////////////////////////////////////////////////////////////////////////////////////

std::atomic_bool inputThreadRunning = { false };

#ifdef _WIN32

void gstate_update_func() {
    POINT p;
    int mButton;

    if (GetSystemMetrics(SM_SWAPBUTTON))
        mButton = VK_RBUTTON; // if  swapped
    else
        mButton = VK_LBUTTON; // not swapped (normal)

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    // default: SM_CX/CYSCREEN gets the size of a primary screen.
    // lines uncommented below are just for a specially need on multi-display.
    // int screenWidth  = GetSystemMetrics( SM_CXVIRTUALSCREEN );
    // int screenHeight = GetSystemMetrics( SM_CYVIRTUALSCREEN );
    float r_screenWidth = 1.f / (float)(screenWidth - 1);
    float r_screenHeight = 1.f / (float)(screenHeight - 1);


    while (inputThreadRunning.load(std::memory_order_relaxed)) {
        // "KeyState" is disabled for now, on Windows...
        // GetKey((long*)gstate->keys);

        GetCursorPos(&p);
        gMouseUGenGlobals.mouseX = (float)p.x * r_screenWidth;
        gMouseUGenGlobals.mouseY = 1.f - (float)p.y * r_screenHeight;
        gMouseUGenGlobals.mouseButton = (GetKeyState(mButton) < 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }
}

#else
static Display* d = 0;
void gstate_update_func() {
    Window r;
    struct timespec requested_time, remaining_time;
    // NOTE: should not be required as this is the only thread accessing the x11 API
    //       but omitting seems to cause troubles.
    XInitThreads();

    d = XOpenDisplay(NULL);
    if (!d)
        return;

    Window rep_root, rep_child;
    XWindowAttributes attributes;
    int rep_rootx, rep_rooty;
    unsigned int rep_mask;
    int dx, dy;
    float r_width;
    float r_height;

    r = DefaultRootWindow(d);
    XGetWindowAttributes(d, r, &attributes);
    r_width = 1.0 / (float)attributes.width;
    r_height = 1.0 / (float)attributes.height;

    while (inputThreadRunning.load(std::memory_order_relaxed)) {
        XQueryKeymap(d, (char*)(gKeyStateGlobals.keys));
        XQueryPointer(d, r, &rep_root, &rep_child, &rep_rootx, &rep_rooty, &dx, &dy, &rep_mask);

        gMouseUGenGlobals.mouseX = (float)dx * r_width;
        gMouseUGenGlobals.mouseY = 1.f - ((float)dy * r_height);

        gMouseUGenGlobals.mouseButton = (bool)(rep_mask & Button1Mask);

        std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////

void KeyState_next(KeyState* unit, int inNumSamples) {
    // minval, maxval, warp, lag
    uint8* keys = (uint8*)gKeyStateGlobals.keys;
    int keynum = (int)ZIN0(0);
    int byte = (keynum >> 3) & 31;
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

void KeyState_Ctor(KeyState* unit) {
    SETCALC(KeyState_next);
    unit->m_b1 = 0.f;
    unit->m_lag = 0.f;
    KeyState_next(unit, 1);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void MouseX_next(MouseInputUGen* unit, int inNumSamples) {
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
    float y0 = gMouseUGenGlobals.mouseX;
    if (warp == 0.0) {
        y0 = (maxval - minval) * y0 + minval;
    } else {
        y0 = pow(maxval / minval, y0) * minval;
    }
    ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
    unit->m_y1 = zapgremlins(y1);
}

void MouseX_Ctor(MouseInputUGen* unit) {
    SETCALC(MouseX_next);
    unit->m_b1 = 0.f;
    unit->m_lag = 0.f;
    MouseX_next(unit, 1);
}


void MouseY_next(MouseInputUGen* unit, int inNumSamples) {
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
    float y0 = gMouseUGenGlobals.mouseY;
    if (warp == 0.0) {
        y0 = (maxval - minval) * y0 + minval;
    } else {
        y0 = pow(maxval / minval, y0) * minval;
    }
    ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
    unit->m_y1 = zapgremlins(y1);
}

void MouseY_Ctor(MouseInputUGen* unit) {
    SETCALC(MouseY_next);
    unit->m_b1 = 0.f;
    unit->m_lag = 0.f;
    MouseY_next(unit, 1);
}


void MouseButton_next(MouseInputUGen* unit, int inNumSamples) {
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
    float y0 = gMouseUGenGlobals.mouseButton ? maxval : minval;
    ZOUT0(0) = y1 = y0 + b1 * (y1 - y0);
    unit->m_y1 = zapgremlins(y1);
}

void MouseButton_Ctor(MouseInputUGen* unit) {
    SETCALC(MouseButton_next);
    unit->m_b1 = 0.f;
    unit->m_lag = 0.f;
    MouseButton_next(unit, 1);
}

SC_Thread uiListenThread;

PluginLoad(UIUGens) {
    ft = inTable;

    inputThreadRunning = true;
    uiListenThread = std::thread(gstate_update_func);

    DefineSimpleUnit(KeyState);

    DefineUnit("MouseX", sizeof(MouseInputUGen), (UnitCtorFunc)&MouseX_Ctor, 0, 0);
    DefineUnit("MouseY", sizeof(MouseInputUGen), (UnitCtorFunc)&MouseY_Ctor, 0, 0);
    DefineUnit("MouseButton", sizeof(MouseInputUGen), (UnitCtorFunc)&MouseButton_Ctor, 0, 0);
}


PluginUnload(UIUGens) {
    inputThreadRunning = false;
    uiListenThread.join();

#ifndef _WIN32
    if (d)
        XCloseDisplay(d);
#endif
}
