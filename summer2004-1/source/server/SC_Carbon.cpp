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
# include <Carbon/Carbon.h>
#endif

#ifdef SC_LINUX
# include "SC_World.h"
# ifdef __ALTIVEC__
#  include <stdio.h>
#  include <stdlib.h>
#  include <string.h>
#  include <unistd.h>
#  include <signal.h>
#  include <setjmp.h>

// niklas werner: kernel independent altivec detection
// borrowed from MPlayer, who borrowed from libmpeg2

static sigjmp_buf sigIllJmpBuf;
static volatile sig_atomic_t sigIllCanJump = 0;
static sighandler_t sigIllOldHandler = SIG_DFL;

static void sigIllHandler(int sig)
{
	if (!sigIllCanJump) {
		signal(sig, sigIllOldHandler);
		raise(sig);
	}
	sigIllCanJump = 0;
	siglongjmp(sigIllJmpBuf, 1);
}
#endif // __ALTIVEC__
#endif // SC_LINUX

bool HasAltivec()
{
	bool hasAltivec = false;

#if defined(SC_DARWIN) && __VEC__
	long response;
	Gestalt(gestaltPowerPCProcessorFeatures, &response);
	//printf("HasAltivec %08X %d\n", response, response & (1<<gestaltPowerPCHasVectorInstructions));
	hasAltivec = response & (1<<gestaltPowerPCHasVectorInstructions);
#endif // SC_DARWIN

#ifdef SC_LINUX
# ifdef __ALTIVEC__
	if (getenv("SC_NOVEC") == 0) {
		sigIllOldHandler = signal(SIGILL, sigIllHandler);

		if (sigsetjmp(sigIllJmpBuf, 1)) {
			signal(SIGILL, sigIllOldHandler);
		} else {
			sigIllCanJump = 1;
			asm volatile ("mtspr 256, %0\n\t"
						  "vand %%v0, %%v0, %%v0"
						  :
						  : "r" (-1));
			signal(SIGILL, sigIllOldHandler);
			hasAltivec = true;
		}
	} else {
		hasAltivec = strcmp(getenv("SC_NOVEC"), "0") == 0;
	}
# endif // __ALTIVEC__
	scprintf("Using Altivec: %s\n", hasAltivec ? "yes" : "no");
#endif // SC_LINUX

	return hasAltivec;
}
