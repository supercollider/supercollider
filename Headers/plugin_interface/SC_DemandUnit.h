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

#ifndef _SC_DemandUnit_
#define _SC_DemandUnit_

#include "SC_Unit.h"
#include "SC_Wire.h"

// demand rate unit support.

inline bool IsDemandInput(Unit* unit, int index)
{
	Unit* fromUnit = unit->mInput[index]->mFromUnit;
	return fromUnit && fromUnit->mCalcRate == calc_DemandRate;
}

inline float DemandInput(Unit* unit, int index)
{
	Unit* fromUnit = unit->mInput[index]->mFromUnit;
	if (fromUnit && fromUnit->mCalcRate == calc_DemandRate)
		(fromUnit->mCalcFunc)(fromUnit, 1);
	return IN0(index);
}

inline void ResetInput(Unit* unit, int index)
{
	Unit* fromUnit = unit->mInput[index]->mFromUnit;
	if (fromUnit && fromUnit->mCalcRate == calc_DemandRate)
		(fromUnit->mCalcFunc)(fromUnit, 0);
}

#define ISDEMANDINPUT(index) IsDemandInput(unit, (index))
#define DEMANDINPUT(index) DemandInput(unit, (index))
#define RESETINPUT(index) ResetInput(unit, (index))

#endif
