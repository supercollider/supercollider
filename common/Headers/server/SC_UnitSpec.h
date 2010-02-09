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

#ifndef _SC_UnitSpec_
#define _SC_UnitSpec_

#include <ctype.h> // for size_t

#include "SC_Unit.h"

struct UnitSpec
{
	struct UnitDef* mUnitDef;
	int16 mCalcRate;
	uint16 mNumInputs, mNumOutputs;
	int16 mSpecialIndex;
	struct InputSpec* mInputSpec;
	struct OutputSpec* mOutputSpec;
	struct Rate* mRateInfo;
	size_t mAllocSize;
};
typedef struct UnitSpec UnitSpec;

#endif
