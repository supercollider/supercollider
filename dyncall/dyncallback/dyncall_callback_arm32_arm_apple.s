/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_arm32_arm_apple.s
 Description: Callback Thunk - Implementation for ARM32 (ARM mode) for Apple's as
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
.code 32	/* ARM mode */

.globl _dcCallbackThunkEntry
.align 4
/* sizes */
DCThunk_size    =   8
DCArgs_size     =  24
DCCallback_size =  20
DCValue_size    =   8

/* struct DCCallback offsets and size */
CTX_thunk       =   0
CTX_handler     =  12
CTX_userdata    =  16


/* Called by thunk - thunk stores pointer to DCCallback in r12 */
_dcCallbackThunkEntry:

	/* Prolog. This function never needs to spill inside its prolog, so just store the permanent registers. */
	stmdb	r13, {r4-r11, r13, r14}	/* Permanent registers and stack pointer, etc... -> save area on stack (except counter). */
	mov		r11, r13						/* Set frame pointer. */
	sub		r13, r13, #40					/* Adjust stack pointer. */

	/* Grab arguments. */
	mov		r4, #0
	stmdb	r13!, {r0-r4, r11}			/* Spill first 4 args to DCArgs, along with reg_count and (stack) pointer to remaining args. */

	/* Prepare callback handler call. */
	mov		r0, r12						/* Parameter 0 (r0) = DCCallback pointer (r12, pointer to thunk). */
	mov		r1, r13						/* Parameter 1 (r1) = DCArgs pointer (r13, stack pointer). */
	sub		r13, r13, #DCValue_size		/* Make room for return value. */
	mov		r2, r13						/* Parameter 2 (r2) = results pointer. */
	ldr		r3, [r12, #CTX_userdata]		/* Parameter 3 (r3) = userdata pointer. */

	/* Call. */
	ldr		r4, [r12, #CTX_handler]		/* Load callback handler pointer into r4. */
	mov		r14, r15						/* Branch return address(r15) -> link register (r14) -- r15 always points to address of current + 2 instructions (= Epilog code). */ 
	bx		r4								/* Call. */

	/* Return value. */
	ldmia	r13, {r0, r1}				/* Load return value in r0 and r1. */

	/* Epilog. */
	ldmdb	r11, {r4-r11, r13, r15}	/* Restore permanent registers (restore stack ptr and program counter).@@@db not needed since we rewrite r13? */

