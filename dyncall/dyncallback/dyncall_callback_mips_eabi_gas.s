/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_mips_eabi_gas.s
 Description: Callback Thunk - Implementation for mips32 eabi
 License:

   Copyright (c) 2016 Tassilo Philipp <tphilipp@potion-studios.com>

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
	$t4    -> thunk
	$t4+20 -> cb handler
	$t4+24 -> userdata
*/

.text
.globl dcCallbackThunkEntry
.ent   dcCallbackThunkEntry

/* Called by thunk - thunk stores pointer to DCCallback in $12 ($t4), and */
/* pointer to called function in $25 ($t9, required for PIC)              */
dcCallbackThunkEntry:
	.set    noreorder

	/* Prolog. */
	/* Frame size of 88b comes from following: */
	/*   DCargs(fregs:32 + iregs:32 + regcounts:4 + stackptr:4) + retval:8 + ra:4 (+ pad:4) */
	subu  $sp, 88       /* open frame */
	sw    $ra, 84($sp)  /* save link register */

	.frame $fp,88,$31   /* specify our frame: fp,size,lr; creates virt $fp */
	                    /* code below doesn't use $fp though, as n/a with -O1 */
	/* Init return value */
	sw $zero, 72($sp)
	sw $zero, 76($sp)

	/* Store float and int args where our DCargs member arrays are, in local area. */
	sw  $4,    0($sp)
	sw  $5,    4($sp)
	sw  $6,    8($sp)
	sw  $7,   12($sp)
	sw  $8,   16($sp)
	sw  $9,   20($sp)
	sw $10,   24($sp)
	sw $11,   28($sp)
	s.s $f12, 32($sp)
	s.s $f13, 36($sp)
	s.s $f14, 40($sp)
	s.s $f15, 44($sp)
	s.s $f16, 48($sp)
	s.s $f17, 52($sp)
	s.s $f18, 56($sp)
	s.s $f19, 60($sp)

	/* Init DCarg's reg_counts and stackptr. */
	sw $zero, 64($sp) /* reg_count */
	addiu $4, $sp, 88
	sw    $4, 68($sp) /* stackptr */

	/* Prepare callback handler call. */
	move  $4, $12       /* Param 0 = DCCallback*, $12 ($t4) holds pointer to thunk */
	move  $5, $sp       /* Param 1 = DCArgs*, pointer to where pointer to args is stored */
	addiu $6, $sp, 72   /* Param 2 = results pointer to 8b of local data on stack */
	lw    $7, 24($12)   /* Param 3 = userdata pointer */

	lw    $25, 20($12)  /* store handler entry in $25 ($t9), required for PIC */
	jalr  $25           /* jump */
	nop                 /* branch delay nop */

	/* Copy result in corresponding registers $2-$3 ($v0-$v1) and $f0 */
	lw     $2, 72($sp)
	lw     $3, 76($sp)
	l.d   $f0, 72($sp)

	/* Epilog. Tear down frame and return. */
	lw    $ra, 84($sp)  /* restore return address */
	addiu $sp, $sp, 88  /* close frame */
	j     $ra           /* return */
	nop                 /* branch delay nop */

	.set    reorder
	.end    dcCallbackThunkEntry
	.ident  "handwritten"

