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


#ifndef _SC_Samp_
#define _SC_Samp_

#include "SC_Types.h"

const int kSineSize = 8192;
const int kSineMask = kSineSize - 1;

extern float32 gSine[kSineSize+1];
extern float32 gInvSine[kSineSize+1];
extern float32 gSineWavetable[2*kSineSize];

void SignalAsWavetable(float32* signal, float32* wavetable, long inSize);

#endif
