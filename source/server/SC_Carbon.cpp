/*
 *  SC_Carbon.cpp
 *  SC3synth
 *
 *  Created by James McCartney on Sat Mar 15 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include <Carbon/Carbon.h>

bool HasAltivec()
{
#if __VEC__
	long response;
	Gestalt(gestaltPowerPCProcessorFeatures, &response);
	printf("HasAltivec %08X %d\n", response, response & (1<<gestaltPowerPCHasVectorInstructions));
	return response & (1<<gestaltPowerPCHasVectorInstructions);
#else
	printf("HasAltivec false\n");
	return false;
#endif
}

