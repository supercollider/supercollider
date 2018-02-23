/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_arm32_arm_armhf.S
 Description: Call Kernel for ARM 32-bit ARM Architecture - Hard Float
 License:

   Copyright (c) 2007-2016 Daniel Adler <dadler@uni-goettingen.de>,
                           Tassilo Philipp <tphilipp@potion-studios.com>

   Permission to use, copy, modify, and distribute this software for any
   purpose with or without fee is hereby granted, provided that the above
   copyright notice and this permission notice appear in all copies.

   THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
   WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
   ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
   WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
   ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
   OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/



#include "../portasm/portasm-arm.S"

/* ============================================================================
   DynCall Call Kernel for ARM 32-bit ARM Architecture Hard-Float
   ----------------------------------------------------------------------------
   C Interface:
     dcCall_arm32_armhf (DCpointer target, DCpointer argv, DCsize size, DCfloat* regdata);

   This Call Kernel was tested on Raspberry Pi/Raspbian (Debian)
*/

.text
.code 32	/* ARM mode */
.arch armv6
.fpu  vfp

/*
  1st arg / r0 = funptr
  2st arg / r1 = ptr to int args
  3st arg / r2 = size
  4st arg / r3 = ptr to float args
*/
GLOBAL_C(dcCall_arm32_armhf)
ENTRY_C(dcCall_arm32_armhf)

	/* Prolog. This function never needs to spill inside its prolog, so just store the permanent registers. */
	mov r12 , r13  /* Stack ptr (r13) -> temporary (r12). */
	stmdb r13!, {r4-r5, r11, r12, r14} /* Permanent registers and stack pointer (now in r12), etc... -> save area on stack (except counter). */

	mov r11, r12     /* Set frame ptr. */
	mov r4,  r0      /* r4 = fptr */
	add r5,  r1, #16 /* r5 = stack args (after intreg ones) */

	/* Load 16 single-precision registers (= 8 double-precision registers). */
	fldmiad r3, {d0-d7}

	/* prep stack parameter area (includes room for spill area, callee spills if needed) */
	sub r13, r13, r2
	and r13, r13, #-8  /* align 8-byte. */

	sub r2, r2, #16    /* loop counters for stack params to copy */
	mov r3, #0
armhf_pushArgs:
	cmp r2, r3
	ble armhf_call
	ldr r0, [r5,  +r3] /* load word into r0 ... */
	str r0, [r13, +r3] /* ... then push onto stack */
	add r3, r3, #4
	b   armhf_pushArgs

armhf_call:
	ldmia r1, {r0-r3}  /* Load first 4 arguments for new call into r0-r3. */
	                   /* 'blx %r4' workaround for ARMv4t: */
	mov r14, r15       /* Branch return address(r15) -> link register (r14) -- r15 always points to address of current + 2 instructions (= Epilog code). */ 
	bx  r4             /* Call (ARM/THUMB), available for ARMv4t. */

	/* Epilog. */
	ldmdb r11, {r4-r5, r11, r13, r15} /* Restore permanent registers (ignore temporary (r12), restore stack ptr and program counter).@@@db not needed since we rewrite r13? */

