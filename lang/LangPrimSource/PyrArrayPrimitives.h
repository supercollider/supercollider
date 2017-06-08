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

#ifndef _PYRARRAYPRIM_H_
#define _PYRARRAYPRIM_H_


int basicSize(VMGlobals *g, int numArgsPushed);
int basicMaxSize(VMGlobals *g, int numArgsPushed);

int basicSwap(struct VMGlobals *g, int numArgsPushed);
int basicAt(VMGlobals *g, int numArgsPushed);
int basicRemoveAt(VMGlobals *g, int numArgsPushed);
int basicClipAt(VMGlobals *g, int numArgsPushed);
int basicWrapAt(VMGlobals *g, int numArgsPushed);
int basicFoldAt(VMGlobals *g, int numArgsPushed);
int basicPut(VMGlobals *g, int numArgsPushed);
int basicClipPut(VMGlobals *g, int numArgsPushed);
int basicWrapPut(VMGlobals *g, int numArgsPushed);
int basicFoldPut(VMGlobals *g, int numArgsPushed);

int prArrayAdd(VMGlobals *g, int numArgsPushed);
int prArrayFill(VMGlobals *g, int numArgsPushed);
int prArrayPop(VMGlobals *g, int numArgsPushed);
int prArrayGrow(VMGlobals *g, int numArgsPushed);
int prArrayCat(VMGlobals *g, int numArgsPushed);

int prArrayReverse(VMGlobals *g, int numArgsPushed);
int prArrayScramble(VMGlobals *g, int numArgsPushed);
int prArrayRotate(VMGlobals *g, int numArgsPushed);
int prArrayStutter(VMGlobals *g, int numArgsPushed);
int prArrayMirror(VMGlobals *g, int numArgsPushed);
int prArrayMirror1(VMGlobals *g, int numArgsPushed);
int prArrayMirror2(VMGlobals *g, int numArgsPushed);
int prArrayExtendWrap(VMGlobals *g, int numArgsPushed);
int prArrayExtendFold(VMGlobals *g, int numArgsPushed);
int prArrayPermute(VMGlobals *g, int numArgsPushed);
int prArrayPyramid(VMGlobals *g, int numArgsPushed);
int prArraySlide(VMGlobals *g, int numArgsPushed);
int prArrayLace(VMGlobals *g, int numArgsPushed);
int prArrayContainsSeqColl(VMGlobals *g, int numArgsPushed);
int prArrayMaxDepth(VMGlobals *g, int numArgsPushed);
int prArrayMaxSizeAtDepth(VMGlobals *g, int numArgsPushed);
int prArrayWIndex(VMGlobals *g, int numArgsPushed);
int prArrayNormalizeSum(VMGlobals *g, int numArgsPushed);
int prArrayIndexOfGreaterThan(VMGlobals *g, int numArgsPushed);



#endif
