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

void initMathPrimitives();

int prAddNum(VMGlobals* g, int numArgsPushed);
int prSubNum(VMGlobals* g, int numArgsPushed);
int prMulNum(VMGlobals* g, int numArgsPushed);

int prAddInt(VMGlobals* g, int numArgsPushed);
int prSubInt(VMGlobals* g, int numArgsPushed);
int prMulInt(VMGlobals* g, int numArgsPushed);

int prAddFloat(VMGlobals* g, int numArgsPushed);
int prSubFloat(VMGlobals* g, int numArgsPushed);
int prMulFloat(VMGlobals* g, int numArgsPushed);

int mathClip(VMGlobals* g, int numArgsPushed);
int mathWrap(VMGlobals* g, int numArgsPushed);
int mathFold(VMGlobals* g, int numArgsPushed);
int mathClipInt(VMGlobals* g, int numArgsPushed);
int mathWrapInt(VMGlobals* g, int numArgsPushed);
int mathFoldInt(VMGlobals* g, int numArgsPushed);
int mathClipFloat(VMGlobals* g, int numArgsPushed);
int mathWrapFloat(VMGlobals* g, int numArgsPushed);
int mathFoldFloat(VMGlobals* g, int numArgsPushed);
int mathClipSignal(VMGlobals* g, int numArgsPushed);
int mathWrapSignal(VMGlobals* g, int numArgsPushed);
int mathFoldSignal(VMGlobals* g, int numArgsPushed);
