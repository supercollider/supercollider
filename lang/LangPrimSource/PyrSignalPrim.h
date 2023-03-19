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


#pragma once

extern PyrSymbol* s_wavetable;
extern struct PyrClass* class_wavetable;

void initSignalPrimitives();

int prSignalCat(VMGlobals* g, int numArgsPushed);
int prSignalFill(VMGlobals* g, int numArgsPushed);
int prSignalRamp(VMGlobals* g, int numArgsPushed);
int prSignalScale(VMGlobals* g, int numArgsPushed);
int prSignalOffset(VMGlobals* g, int numArgsPushed);
int prSignalString(VMGlobals* g, int numArgsPushed);

int prSignalPeak(VMGlobals* g, int numArgsPushed);
int prSignalNormalize(VMGlobals* g, int numArgsPushed);
int prSignalNormalizeTransferFn(VMGlobals* g, int numArgsPushed);
int prSignalIntegral(VMGlobals* g, int numArgsPushed);

int prSignalOverDub(VMGlobals* g, int numArgsPushed);
int prSignalOverWrite(VMGlobals* g, int numArgsPushed);
int prSignalFade(VMGlobals* g, int numArgsPushed);
int prSignalAddHarmonic(VMGlobals* g, int numArgsPushed);
int prSignalAsWavetable(VMGlobals* g, int numArgsPushed);
int prWavetableAsSignal(VMGlobals* g, int numArgsPushed);

int prSignalInvert(VMGlobals* g, int numArgsPushed);
int prSignalReverse(VMGlobals* g, int numArgsPushed);
int prSignalRotate(VMGlobals* g, int numArgsPushed);

void signalAsWavetable(float* signal, float* wavetable, int size);
void wavetableAsSignal(float* wavetable, float* signal, int size);
