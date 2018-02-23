/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_arm32_thumb_apple.s
 Description: ARM Thumb call kernel implementation for apple assembler.
 License:

   Copyright (c) 2007-2011 Daniel Adler <dadler@uni-goettingen.de>, 
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


.text
.thumb
.code 16 
.globl _dcCall_arm32_thumb

/* Main dyncall call. */
.thumb_func
_dcCall_arm32_thumb:

	/* Prolog. This function never needs to spill inside its prolog, so just store the permanent registers. */
	push	{r4-r7, r14}	   	/* Frame ptr, permanent registers, link register -> save area on stack. */
	mov		r7, r13	/* Set frame ptr. */

	/* Call. */
	mov		r4, r0	/* Move 'fptr' to r4 (1st argument is passed in r0). */

	/* Disable 'thumb' address forcing... */

	/* mov		r0, #1	*/	/* Assure that LSB is set to 1 (THUMB call). - Not Required and not useful for interworking calls */
	/* orr		r4, r0 */
	
	mov		r5, r1	/* Move 'args' to r5 (2nd argument is passed in r1). */
	mov		r6, r2	/* Move 'size' to r6 (3rd argument is passed in r2). */

	cmp		r6, #16	/* Jump to call if no more than 4 arguments. */
	ble		call

	sub		r6, #16	/* Size of remaining arguments. */
	mov		r0, r13	/* Set stack pointer to top of stack. */
	sub		r0, r0, r6
	lsr		r0, #3		/* Align stack on 8 byte boundaries. */
	lsl		r0, #3
	mov		r13, r0

	add		r1, #16	/* Let r1 point to remaining arguments. */
	mov		r2, #0		/* Init byte counter to 0. */
.thumb_func
pushArgs:
	ldrb		r3, [r1, r2]		/* Load a byte into r3. */
	strb		r3, [r0, r2]		/* Push byte onto stack. */
	add		r2, r2, #1	/* Increment byte counter. */
	cmp		r2, r6
	bne		pushArgs
.thumb_func
call:
	ldmia	r5!, {r0-r3}		/* Load first 4 arguments for new call into r0-r3. */
					
					/* 'blx %r4' workaround for ARMv4t in THUMB: */
	blx		r4		/* Branch and force THUMB-mode return (LR bit 0 set). */

	/* Epilog. */
	mov		r13, r7		/* Reset stack ptr. */
	pop		{r4-r7, r15}	/* Restore permanent registers and program counter. (Force a stay in THUMB in ARMv4, whether ARMv5 can return in ARM or THUMB depending on the bit 0. */

