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
 *  SC_Carbon.cpp
 *  SC3synth
 *
 *  Created by James McCartney on Sat Mar 15 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#ifdef SC_DARWIN

#include <Carbon/Carbon.h>

bool HasAltivec()
{
#if __VEC__
	long response;
	Gestalt(gestaltPowerPCProcessorFeatures, &response);
	//printf("HasAltivec %08X %d\n", response, response & (1<<gestaltPowerPCHasVectorInstructions));
	return response & (1<<gestaltPowerPCHasVectorInstructions);
#else
	//printf("HasAltivec false\n");
	return false;
#endif
}

#else // !SC_DARWIN

bool HasAltivec()
{
	// sk: how to do this on ppc linux?
	return false;
}

#endif // SC_DARWIN