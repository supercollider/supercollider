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

#ifndef _SC_Graph_
#define _SC_Graph_

#include "SC_Node.h"
#include "SC_Rate.h"
#include "SC_Dimension.h"

struct Graph 
{
	Node mNode;
	
	char *mMemory;
	
	int mNumWires;
	struct Wire *mWire;
	
	int mNumControls;
	float *mControls;
	float **mMapControls;
	int mControlTouched;
		
	int mNumUnits;
	struct Unit **mUnits;
	
	int mNumCalcUnits;
	struct Unit **mCalcUnits; // excludes i-rate units.

	int mSampleOffset;
};
typedef struct Graph Graph;

#endif
