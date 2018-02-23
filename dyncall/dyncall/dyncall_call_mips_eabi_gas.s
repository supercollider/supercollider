/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_mips_eabi_gas.s
 Description: mips "eabi" abi call kernel implementation in GNU Assembler
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


/*//////////////////////////////////////////////////////////////////////

	dyncall_call_mips_eabi_gas.s

	MIPS 32bit family of processors.
	2008-01-03

//////////////////////////////////////////////////////////////////////*/
.text
.globl dcCall_mips_eabi

dcCall_mips_eabi:
	/* $4 target function */
	/* $5 register data */
	/* $6 stack size */
	/* $7 stack data */
	addiu	$sp,$sp,-16
	sw	$16,8($sp)
	sw	$31,4($sp)
	sw	$fp,0($sp)

	move	$fp,$sp

	move	$2, $0
	add	$2, 8
	neg	$2
	and	$sp, $2
	add	$6, 7
	and	$6, $2

	move	$12,$4		/* target function */
	move	$13,$5		/* register data   */
	move    $16,$6		/* stack size      */

	sub	$sp,$sp,$16	/* allocate stack frame */

	/* copy stack data */

.next:
	beq	$6,$0, .skip
	nop
	addiu	$6,$6, -4

	lw	$2, 0($7)
	sw	$2, 0($sp)
	addiu	$7,$7, 4
	addiu	$sp,$sp, 4
	j	.next
	nop

.skip:

	sub	$sp,$sp,$16

	/* load integer parameter registers */

	lw	$4 , 0($13)
	lw	$5 , 4($13)
	lw	$6 , 8($13)
	lw	$7 ,12($13)
	lw	$8 ,16($13)
	lw	$9 ,20($13)
	lw	$10,24($13)
	lw	$11,28($13)

	/* load single-precision floating pointer parameter registers */

	l.s	$f12, 32($13)
	l.s	$f13, 36($13)
	l.s	$f14, 40($13)
	l.s	$f15, 44($13)
	l.s	$f16, 48($13)
	l.s	$f17, 52($13)
	l.s	$f18, 56($13)
	l.s	$f19, 60($13)

	jal	$12
	nop

	/* add	$sp,$sp,$16  */
	move	$sp,$fp 

	lw	$16,8($sp)
	lw	$31,4($sp)
	lw	$fp,0($sp)
	addiu	$sp,$sp,16
	j	$31
	nop

