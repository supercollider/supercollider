/*
 *  SC_DemandUnit.h
 *  xSC3plugins
 *
 *  Created by James McCartney on Thu Apr 08 2004.
 *  Copyright (c) 2004 __MyCompanyName__. All rights reserved.
 *
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
