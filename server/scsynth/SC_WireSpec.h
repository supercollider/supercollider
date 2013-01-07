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

#ifndef _SC_WireSpec_
#define _SC_WireSpec_

#include "SC_Types.h"

struct InputSpec
{
	// read from file:
	int32 mFromUnitIndex; // changed from int16, ver 2
	int32 mFromOutputIndex; // changed from int16, ver 2
	// computed:
	int32 mWireIndex; // changed from int16, ver 2
};
typedef struct InputSpec InputSpec;

struct OutputSpec
{
	// read from file:
	int16 mCalcRate;
	// computed:
	int32 mWireIndex; // changed from uint16, ver 2
	int64 mBufferIndex;
	uint32 mNumConsumers;
};
typedef struct OutputSpec OutputSpec;

#endif
