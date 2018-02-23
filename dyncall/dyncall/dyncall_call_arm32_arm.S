/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_arm32_arm.S
 Description: Call Kernel for ARM 32-bit ARM Architecture
 License:

   Copyright (c) 2007-2015 Daniel Adler <dadler@uni-goettingen.de>, 
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
   DynCall Call Kernel for ARM 32-bit ARM Architecture
   ----------------------------------------------------------------------------
   C Interface:
     dcCall_arm32_arm(DCpointer target, DCpointer argv, DCsize size);

   This Call Kernel works across multiple OS.

*/

.text
.code 32	/* ARM mode */
.align 4
GLOBAL_C(dcCall_arm32_arm)
ENTRY_C(dcCall_arm32_arm)
	/* Prolog. This function never needs to spill inside its prolog, so just store the permanent registers. */
	mov	r12, r13	/* Stack ptr (r13) -> temporary (r12). */
	stmdb	r13!, {r4-r12, r14}	/* Permanent registers and stack pointer (now in r12), etc... -> save area on stack (except counter). */
	mov	r11, r12	/* Set frame ptr. */

	/* Call. */
	mov	r4, r0		/* r4 = 'fptr' (1st argument is passed in r0). */
	mov	r5, r1		/* r5 = 'args' (2nd argument is passed in r1). */
	mov	r6, r2		/* r6 = 'size' (3rd argument is passed in r2). */
	ldmia	r5!, {r0-r3}	/* Load first 4 arguments for new call into r0-r3. */

	subs	r6, r6, #16	/* Size of remaining arguments. */
	ble	call		/* Jump to call if no more arguments. */

	sub	r13, r13, r6	/* Set stack pointer to top of stack. */
	and	r9, r6, #7	/* Align stack on 8 byte boundaries. */
	sub	r13, r13, r9

	mov	r8, r13		/* Temp. destination pointer. */
	mov	r9, #0		/* Init byte counter. */

pushArgs:
	ldrb	r7, [r5, r9]	/* Load a byte into r7. */
	strb	r7, [r8, r9]	/* Push byte onto stack. */
	add	r9, r9, #1	/* Increment byte counter. */
	cmp	r9, r6
	bne	pushArgs

call:
				/* 'blx %r4' workaround for ARMv4t: */
	mov	r14, r15	/*   Branch return address(r15) -> link register (r14) -- r15 always points to address of current + 2 instructions (= Epilog code). */ 
	bx	r4		/*   Call (ARM/THUMB), available for ARMv4t. */

	/* Epilog. */
	ldmdb	r11, {r4-r11, r13, r15}	/* Restore permanent registers (ignore temporary (r12), restore stack ptr and program counter).@@@db not needed since we rewrite r13? */

