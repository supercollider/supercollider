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

// ********** this version for mac. for windows and linux see UIUGens.cpp

#include <SC_Lock.h>

#import <AppKit/AppKit.h>
#include <unistd.h>

#include "SC_PlugIn.h"

static InterfaceTable* ft;

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

void gstate_update_func() {
    CGDirectDisplayID display = kCGDirectMainDisplay; // to grab the main display ID
    CGRect bounds = CGDisplayBounds(display);
    float rscreenWidth = 1. / bounds.size.width;
    float rscreenHeight = 1. / bounds.size.height;

    while (inputThreadRunning.load(std::memory_order_relaxed)) {
        NSPoint p;
        p = [NSEvent mouseLocation];
        gMouseUGenGlobals.mouseX = (float)p.x * rscreenWidth;
        gMouseUGenGlobals.mouseY = (float)p.y * rscreenHeight;
        gMouseUGenGlobals.mouseButton = (bool)[NSEvent pressedMouseButtons];

        std::this_thread::sleep_for(std::chrono::milliseconds(17));
    }

    return;
}

//////////////////////////////////////////////////////////////////////////////////////////////////

void KeyState_next(KeyState* unit, int inNumSamples) {
    CGKeyCode keynum = (CGKeyCode)ZIN0(0);
    int val = CGEventSourceKeyState(kCGEventSourceStateCombinedSessionState, keynum);

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
}
