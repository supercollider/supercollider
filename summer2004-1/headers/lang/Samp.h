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

/*

Samp is a typedef for the output sample type. 
Should be defined to be either float or double.

*/

#ifndef _Samp_
#define _Samp_

#include "SC_Types.h"

const long kSineSize = 8192;
const long kSineMask = kSineSize - 1;
const double kSinePhaseScale = kSineSize / (2.0 * 3.1415926535897932384626433832795);

extern float32 gSine[kSineSize+1];
extern float32 gPMSine[kSineSize+1];
extern float32 gInvSine[kSineSize+1];
extern float32 gSineWavetable[2*kSineSize];

void SignalAsWavetable(float32* signal, float32* wavetable, long inSize);
void WavetableAsSignal(float32* wavetable, float32* signal, long inSize);

#endif

