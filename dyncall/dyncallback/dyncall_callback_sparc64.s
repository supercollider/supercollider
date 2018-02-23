/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_sparc64.s
 Description: Callback - Implementation for Sparc 64-bit
 License:

   Copyright (c) 2017 Tassilo Philipp <tphilipp@potion-studios.com>

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
	$i0+56 -> cb handler
	$i0+64 -> userdata
*/

/* NOTE: %sp/%fp for v9 are offset, using them needs a "BIAS" of 2047 */
BIAS = 2047

.text
.globl dcCallbackThunkEntry

/* Called by thunk - thunk stores pointer to DCCallback */
/* in %g1, and pointer to called function in %g2        */
dcCallbackThunkEntry:

	/* Prolog. */
	/* Frame size of 336b comes from needing storage space for the following: */
	/*   req_reg_save_area:128 + spill:48 + dcargs:144 + retval:8 + pad:8 */
	/* Spill area could theoretically be only 32b, b/c cbHandler function has */
 	/* 4 arguments, but let's be conservative. */
	save %sp, -336, %sp

	/* Spill register args as dcargs is based on that (in prev frame, after */
	/* req_reg_save_area). */
	add  %fp, BIAS + 128, %l0
	stx  %i0,  [ %l0 +  0 ]          /* reg arg 0 */
	stx  %i1,  [ %l0 +  8 ]          /* reg arg 1 */
	stx  %i2,  [ %l0 + 16 ]          /* reg arg 2 */
	stx  %i3,  [ %l0 + 24 ]          /* reg arg 3 */
	stx  %i4,  [ %l0 + 32 ]          /* reg arg 4 */
	stx  %i5,  [ %l0 + 40 ]          /* reg arg 5 */
	stx  %l0,  [ %sp + BIAS + 176 ]  /* set DCArg's arg_ptr */
	st   %f0,  [ %sp + BIAS + 184 ]  /* store fp args in DCArgs's dreg_data */
	st   %f1,  [ %sp + BIAS + 188 ]  /* @@@ I think stx should work to store */
	st   %f2,  [ %sp + BIAS + 192 ]  /*     all 64bits, but I get "Illegal   */
	st   %f3,  [ %sp + BIAS + 196 ]  /*     Operands", so using single prec. */
	st   %f4,  [ %sp + BIAS + 200 ]  /*     store (st) */
	st   %f5,  [ %sp + BIAS + 204 ]
	st   %f6,  [ %sp + BIAS + 208 ]
	st   %f7,  [ %sp + BIAS + 212 ]
	st   %f8,  [ %sp + BIAS + 216 ]
	st   %f9,  [ %sp + BIAS + 220 ]
	st   %f10, [ %sp + BIAS + 224 ]
	st   %f11, [ %sp + BIAS + 228 ]
	st   %f12, [ %sp + BIAS + 232 ]
	st   %f13, [ %sp + BIAS + 236 ]
	st   %f14, [ %sp + BIAS + 240 ]
	st   %f15, [ %sp + BIAS + 244 ]
	st   %f16, [ %sp + BIAS + 248 ]
	st   %f17, [ %sp + BIAS + 252 ]
	st   %f18, [ %sp + BIAS + 256 ]
	st   %f19, [ %sp + BIAS + 260 ]
	st   %f20, [ %sp + BIAS + 264 ]
	st   %f21, [ %sp + BIAS + 268 ]
	st   %f22, [ %sp + BIAS + 272 ]
	st   %f23, [ %sp + BIAS + 276 ]
	st   %f24, [ %sp + BIAS + 280 ]
	st   %f25, [ %sp + BIAS + 284 ]
	st   %f26, [ %sp + BIAS + 288 ]
	st   %f27, [ %sp + BIAS + 292 ]
	st   %f28, [ %sp + BIAS + 296 ]
	st   %f29, [ %sp + BIAS + 300 ]
	st   %f30, [ %sp + BIAS + 304 ]
	st   %f31, [ %sp + BIAS + 308 ]
	stx  %g0,  [ %sp + BIAS + 312 ] /* init DCArg's i */

	/* Zero retval store. */
	stx  %g0, [ %sp + BIAS + 320 ]

	/* Prepare callback handler call. */
	mov  %g1, %o0                   /* Param 0 = DCCallback*, %g1 holds ptr to thunk */
	add  %sp, BIAS + 176, %o1       /* Param 1 = DCArgs* (ptr to struct with args ptr) */
	add  %sp, BIAS + 320, %o2       /* Param 2 = results ptr to 8b of local stack data */
	ldx  [ %g1 + 64 ], %o3          /* Param 3 = userdata ptr */

	/* Fetch callback handler address (after thunk blob) and call. */
	ldx  [ %g1 + 56 ], %l0
	call %l0
	nop

	/* Put retval in %i0 (to be in caller's %o0), and %f0. */
	ldx  [ %sp + BIAS + 320 ], %i0
	ldd  [ %sp + BIAS + 320 ], %f0

	/* Epilog. */
	restore                 /* unshift reg window */
	retl                    /* Return from proc. -- jmpl %i7 + 8, %g0 */
	nop

