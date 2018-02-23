/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_mips_o32_gas.s
 Description: Callback Thunk - Implementation mips32 o32
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

	.section .mdebug.abi32
	.previous
	.abicalls
	.text
	.align  2
	.globl dcCallbackThunkEntry
	.ent   dcCallbackThunkEntry
	.type  dcCallbackThunkEntry, @function

/* Called by thunk - thunk stores pointer to DCCallback in $12 ($t4), and */
/* pointer to called function in $25 ($t9, required for PIC)              */
dcCallbackThunkEntry:
	.set    noreorder

	/* Prolog. Just store the minimum, return address, spill area.     */
	/* Frame size of 56b comes from following areas (each 8b aligned): */
	/*   local: fpregs:16 + retval:8 + DCArgs:8 */
	/*   save:  ra:4 (+ pad:4)                  */
	/*   param: spill:16                        */
	subu  $sp, 56       /* open frame */
	sw    $ra, 20($sp)  /* save link register */

	.frame $fp,56,$31   /* specify our frame: fp,size,lr; creates virt $fp */
	                    /* code below doesn't use $fp though, as n/a with -O1 */
	/* Init return value */
	sw $zero, 32($sp)
	sw $zero, 36($sp)

	/* Store the arguments passed via registers somewhere for dcArg* to access. */
	/* For $4-$7 ($a0-$a3), use dedicated spill area (caller doesn't spill, but */
	/* provides it at end of _caller's_ frame, so $fp points right to it).      */
	/* For $f12 and $f14 use our space (in local data), which is adjacent.      */
	s.d $f12, 40($sp) /* -16($fp) */
	s.d $f14, 48($sp) /*  -8($fp) */
	sw $4,    56($sp) /*   0($fp) */
	sw $5,    60($sp) /*   4($fp) */
	sw $6,    64($sp) /*   8($fp) */
	sw $7,    68($sp) /*  12($fp) */

	/* Init DCArg, which contains reg_count and stackptr* to the args. Point  */
	/* stackptr to the area where the non-float args start (which is at $fp). */
	addiu $4, $sp, 56 /* non-$fp replacement for: */
	sw    $4, 28($sp) /*   sw $fp, 28($sp) */
	sw $zero, 24($sp)

	/* Prepare callback handler call. */
	move  $4, $12       /* Param 0 = DCCallback*, $12 ($t4) holds pointer to thunk */
	addiu $5, $sp, 24   /* Param 1 = DCArgs*, pointer to where pointer to args is stored */
	addiu $6, $sp, 32   /* Param 2 = results pointer to 8b of local data on stack */
	lw    $7, 24($12)   /* Param 3 = userdata pointer */

	lw    $25, 20($12)  /* store handler entry in $25 ($t9), required for PIC */
	jalr  $25           /* jump */
	nop                 /* branch delay nop */

	/* Copy result in corresponding registers $2-$3 ($v0-$v1) and $f0 */
	lw     $2, 32($sp)
	lw     $3, 36($sp)
	l.d   $f0, 32($sp)

	/* Epilog. Tear down frame and return. */
	lw    $ra, 20($sp)  /* restore return address */
	addiu $sp, $sp, 56  /* close frame */
	j     $ra           /* return */
	nop                 /* branch delay nop */

	.set    reorder
	.end    dcCallbackThunkEntry
	.ident  "handwritten"

