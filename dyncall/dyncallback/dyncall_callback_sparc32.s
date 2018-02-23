/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_sparc32.s
 Description: Callback - Implementation for Sparc 32-bit
 License:

   Copyright (c) 2007-2017 Daniel Adler <dadler@uni-goettingen.de>,
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

/* input:
	$i0    -> thunk
	$i0+24 -> cb handler
	$i0+28 -> userdata
*/

.text
.globl dcCallbackThunkEntry

/* Called by thunk - thunk stores pointer to DCCallback */
/* in %g1, and pointer to called function in %g2        */
dcCallbackThunkEntry:

	/* Prolog. */
	/* Frame size of 104b comes from needing storage space for the following: */
	/*   req_reg_save_area:64 + spill:24 + dcargs:4 + retval:8 + pad:4 */
	/* Spill area could theoretically be only 16b, b/c cbHandler function has */
 	/* 4 arguments, but let's be conservative. */ /* cbHandler function might decide to spill (6x4b) @@@ testcode, or rely on 8byte return space in parent stack */
	save %sp, -104, %sp 

	/* Spill register args as dcargs is based on that (in prev frame, after */
	/* req_reg_save_area and struct_ret_ptr). */
	add  %fp, 68, %l0
	st   %i0, [ %l0 +  0 ]  /* reg arg 0 */
	st   %i1, [ %l0 +  4 ]  /* reg arg 1 */
	st   %i2, [ %l0 +  8 ]  /* reg arg 2 */
	st   %i3, [ %l0 + 12 ]  /* reg arg 3 */
	st   %i4, [ %l0 + 16 ]  /* reg arg 4 */
	st   %i5, [ %l0 + 20 ]  /* reg arg 5 */
	st   %l0, [ %sp + 88 ]  /* init arg_ptr */

	/* Zero retval store. */
	st   %g0, [ %sp +  96 ]
	st   %g0, [ %sp + 100 ]

	/* Prepare callback handler call. */
	mov  %g1, %o0           /* Param 0 = DCCallback*, %g1 holds ptr to thunk */
	add  %sp, 88, %o1       /* Param 1 = DCArgs* (ptr to struct with args ptr) */
	add  %sp, 96, %o2       /* Param 2 = results ptr to 8b of local stack data */
	ld   [ %g1 + 28 ], %o3  /* Param 3 = userdata ptr */

	ld   [ %g1 + 24 ], %l0
	call %l0
	nop

	/* Put retval in %i0/%i1 (to be in caller's %o0/%o1), and %f0/%f1. */
	ld   [ %sp +  96 ], %i0
	ld   [ %sp + 100 ], %i1
	ld   [ %sp +  96 ], %f0
	ld   [ %sp + 100 ], %f1

	/* Epilog. */
	restore                 /* unshift reg window */
	retl                    /* Return from proc. -- jmpl %i7 + 8, %g0 */
	nop

