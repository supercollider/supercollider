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

/*
 *  SC_Carbon.cpp
 *  SC3synth
 *
 *  Created by James McCartney on Sat Mar 15 2003.
 *  Copyright (c) 2003 __MyCompanyName__. All rights reserved.
 *
 */

#include <stdlib.h>
#include <string.h>
#include "SC_World.h"

// Return true if CPU has vector unit.
bool sc_HasVectorUnit();

// Return true if vector unit is present and should be used.
// Vectorisation can be turned off by exporting SC_NOVEC in the
// process environment.
bool sc_UseVectorUnit();

// Set denormal FTZ mode on CPUs that need/support it.
void sc_SetDenormalFlags();

bool sc_UseVectorUnit()
{
	if (sc_HasVectorUnit()) {
		const char* novec = getenv("SC_NOVEC");
		return !novec || (strcmp(novec, "0") == 0);
	}
	return false;
}

#if defined(__APPLE__) && !defined(SC_IPHONE)
# include <Carbon/Carbon.h>
#include <TargetConditionals.h>

bool sc_HasVectorUnit()
{
	SInt32 response;
	Gestalt(gestaltPowerPCProcessorFeatures, &response);
	//printf("HasAltivec %p %d\n", response, response & (1<<gestaltPowerPCHasVectorInstructions));
	return response & (1<<gestaltPowerPCHasVectorInstructions);
}

#if TARGET_CPU_X86

void sc_SetDenormalFlags()
{
	// all Macs have SSE
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_mm_setcsr(_mm_getcsr() | 0x40);
}

#else

void sc_SetDenormalFlags()
{
}

#endif

#elif defined(__linux__) && defined(__ALTIVEC__)
# include <stdio.h>
# include <string.h>
# include <unistd.h>
# include <signal.h>
# include <setjmp.h>

// kernel independent altivec detection
// contributed by niklas werner
// borrowed from mplayer, who borrowed from libmpeg2
// <sk>

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

bool sc_HasVectorUnit()
{
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
		return true;
	}
	return false;
}

void sc_SetDenormalFlags()
{
}
#elif (defined(__linux__) || defined(__FreeBSD__)) && defined(__SSE__)
# include <xmmintrin.h>

// cpuid function that works with -fPIC from `minor' at http://red-bean.com
// http://svn.red-bean.com/repos/minor/trunk/gc/barriers-ia-32.c
// http://svn.red-bean.com/repos/minor/trunk/gc/barriers-amd64.c
// <sk>

# if defined(__i386__)
/* If the current processor supports the CPUID instruction, execute
   one, with REQUEST in %eax, and set *EAX, *EBX, *ECX, and *EDX to
   the values the 'cpuid' stored in those registers.  Return true if
   the current processor supports CPUID, false otherwise.  */
static bool cpuid(
	uint32_t request,
	uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
	uint32_t pre_change, post_change;
	const uint32_t id_flag = 0x200000;

	/* This is pretty much the standard way to detect whether the CPUID
	   instruction is supported, as recommended in the Intel
	   documentation: try to change the ID bit in the EFLAGS register.
	   If we can change it, then the CPUID instruction is
	   implemented.  */
	asm ("pushfl\n\t"             /* Save %eflags to restore later.  */
		 "pushfl\n\t"             /* Push second copy, for manipulation.  */
		 "popl %1\n\t"            /* Pop it into post_change.  */
		 "movl %1,%0\n\t"         /* Save present value in pre_change.   */
		 "xorl %2,%1\n\t"         /* Tweak bit in post_change.  */
		 "pushl %1\n\t"           /* Push tweaked copy... */
		 "popfl\n\t"              /* ... and pop it into %eflags.  */
		 "pushfl\n\t"             /* Did it change?  Push new %eflags... */
		 "popl %1\n\t"            /* ... and pop it into post_change.  */
		 "popfl"                  /* Restore original value of %eflags.  */
		 : "=&r" (pre_change), "=&r" (post_change)
		 : "ir" (id_flag));

	/* If the bit changed, then we support the CPUID instruction.  */
	if ((pre_change ^ post_change) & id_flag)
    {
		/* The IA-32 ABI specifies that, in position-independent code,
		   %ebx holds the address of the global offset table.  GCC seems
		   to be unable to handle asms in PIC with result operands that
		   live in %ebx.  For example:

            $ cat pic-ebx-asm.c
            int
            foo (void)
            {
              int i;

              asm ("" : "=b" (i));

              return i;
            }
            $ gcc -c -fPIC pic-ebx-asm.c
            pic-ebx-asm.c: In function `foo':
            pic-ebx-asm.c:6: error: can't find a register in class `BREG'
            while reloading `asm'
            $

         But the CPUID instruction leaves results in %ebx, so we have
         to use it.

         To work around this, we save %ebx in %esi, restore %ebx after
         we've done the 'cpuid', and return %ebx's value in %esi.
         Thus the interesting constraint / operand pair '"=S" (*ebx)'.

         We include "memory" in the clobber list, because this is a
         synchronizing instruction; the caller may be expecting other
         processors' writes to become visible here.  */
		asm volatile ("mov %%ebx, %%esi\n\t" /* Save %ebx.  */
					  "cpuid\n\t"
					  "xchgl %%ebx, %%esi" /* Restore %ebx.  */
					  : "=a" (*eax), "=S" (*ebx), "=c" (*ecx), "=d" (*edx)
					  : "0" (request)
					  : "memory");

		return true;
    }
	else
		return false;
}
# elif defined(__x86_64__)
/* Execute a CPUID instruction with REQUEST in %eax, and set *EAX,
   *EBX, *ECX, and *EDX to the values the 'cpuid' stored in those
   registers.  */
static bool cpuid(
	uint32_t request,
	uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
	/* We include "memory" in the clobber list, because this is a
	   synchronizing instruction; the caller may be expecting other
	   processors' writes to become visible here.  */
	asm volatile ("cpuid\n\t"
				  : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
				  : "0" (request)
				  : "memory");
	return true;
}
# else
# error Unknown SSE CPU
# endif

bool sc_HasVectorUnit()
{
	// SSE detection from steve harris
	// http://www.ecs.soton.ac.uk/~swh/denormal-finder/
	// <sk>
	uint32_t a, b, c, d;
	if (cpuid(1, &a, &b, &c, &d)) {
		// SSE support
		return d & 1<<25;
	}
	return false;
}

void sc_SetDenormalFlags()
{
	// SSE denormal setup from steve harris
	// http://www.ecs.soton.ac.uk/~swh/denormal-finder/
	// <sk>
	uint32_t a, b, c, d;

	if (sc_HasVectorUnit()) {
		// FTZ flag (flush to zero)
		_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);

		cpuid(0, &a, &b, &c, &d);
		if (b == 0x756e6547) { /* It's an Intel */
			int stepping, model, family, extfamily;

			family = (a >> 8) & 0xf;
			extfamily = (a >> 20) & 0xff;
			model = (a >> 4) & 0xf;
			stepping = a & 0xf;
			if (family == 15 && extfamily == 0 && model == 0 && stepping < 7) {
				return;
			}
		}

		cpuid(1, &a, &b, &c, &d);
		if (d & 1<<26) { /* bit 26, SSE2 support */
			// DAZ flag (disable denormals)
			_mm_setcsr(_mm_getcsr() | 0x40);
		}
    }
}
#else
bool sc_HasVectorUnit()
{
	return false;
}
void sc_SetDenormalFlags()
{
}
#endif
