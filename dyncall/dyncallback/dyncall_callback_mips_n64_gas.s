/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_mips_n64_gas.s
 Description: Callback Thunk - Implementation for mips64 n64
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
		$t8    -> thunk
		$t8+56 -> cb handler
		$t8+64 -> userdata
	*/

	.section .mdebug.abi64
	.previous
	.abicalls
	.text
	.align  2
	.globl dcCallbackThunkEntry
	.ent   dcCallbackThunkEntry
dcCallbackThunkEntry:
	.set    noreorder

	/* Prolog. */
	/* Frame size of 160b comes from following: */
	/*   DCargs(fregs:64 + iregs:64 + regcounts:8 + stackptr:8) + retval:8 + ra:8 */
	daddiu $sp, $sp, -160 /* open frame */
	sd     $ra, 152($sp)  /* save link register */

	.frame $fp,160,$31    /* specify our frame: fp,size,lr; creates virt $fp */
	                      /* code below doesn't use $fp though, as n/a with -O1 */
	/* Init return value */
	sd $zero, 144($sp)

	/* Store float and int args where our DCargs member arrays are, in local area. */
	sd  $4,     0($sp)
	sd  $5,     8($sp)
	sd  $6,    16($sp)
	sd  $7,    24($sp)
	sd  $8,    32($sp)
	sd  $9,    40($sp)
	sd $10,    48($sp)
	sd $11,    56($sp)
	s.d $f12,  64($sp)
	s.d $f13,  72($sp)
	s.d $f14,  80($sp)
	s.d $f15,  88($sp)
	s.d $f16,  96($sp)
	s.d $f17, 104($sp)
	s.d $f18, 112($sp)
	s.d $f19, 120($sp)

	/* Init DCarg's reg_counts and stackptr. */
	sd $zero, 128($sp)    /* reg_count */
	daddiu $4, $sp, 160
	sd     $4, 136($sp)   /* stackptr */

	/* Prepare callback handler call. */
	move   $4, $24        /* Param 0 = DCCallback*, $24 ($t8) holds pointer to thunk */
	move   $5, $sp        /* Param 1 = DCArgs*, pointer to where pointer to args is stored */
	daddiu $6, $sp, 144   /* Param 2 = results pointer to 8b of local data on stack */
	ld     $7, 64($24)    /* Param 3 = userdata pointer */

	ld     $25, 56($24)   /* store handler entry in $25 ($t9), required for PIC */
	jalr   $25            /* jump */
	nop                   /* branch delay nop */

	/* Copy result in corresponding registers $2-$3 ($v0-$v1) and $f0 */
	ld     $2, 144($sp)   /* note: ignoring second possible retval in $3, here */
	l.d   $f0, 144($sp)

	/* Epilog. Tear down frame and return. */
	ld    $ra, 152($sp)   /* restore return address */
	daddiu $sp, $sp, 160  /* close frame */
	j     $ra             /* return */
	nop                   /* branch delay nop */

	.set    reorder
	.end    dcCallbackThunkEntry
	.ident  "handwritten"

