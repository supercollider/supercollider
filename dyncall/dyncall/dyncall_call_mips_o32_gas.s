/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_mips_o32_gas.s
 Description: mips "o32" abi call kernel implementation in GNU Assembler
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
	/* $4   target function */
 	/* $5   register data */
	/* $6   stack size (min 16-byte aligned to 8-bytes already) */
	/* $7   stack data */



	.section .mdebug.abi32
	.previous
	.abicalls
	.text
	.align	2
	.globl	dcCall_mips_o32
	.ent	dcCall_mips_o32
	.type	dcCall_mips_o32, @function
dcCall_mips_o32:
	.frame	$fp,40,$31		/* vars=8, regs=2/0, args=16, gp=8 */
	.mask	0xc0000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	
	addiu	$sp,$sp,-8
	sw	$31,4($sp)	/* save link register */
	sw	$fp,0($sp)	/* save frame pointer */
	nop
	move	$fp,$sp		/* frame pointer = sp */
	sub	$sp, $sp, $6	/* increment stack */

	/* copy stack data */

	/* $12  source pointer (parameter stack data) */
	/* $14  destination (stack pointer) */
	/* $6   byte count */

	move	$12, $7
	move	$14, $sp

.next:
	beq	$6, $0, .skip
	nop
	lw	$2, 0($12)
	nop
	sw	$2, 0($14)
	addiu	$12,$12, 4
	addiu	$14,$14, 4
	addiu	$6, $6, -4
	j	.next
	nop
.skip:

	/* load two double-precision floating-point argument registers ($f12, $f14) */

	l.d     $f12, 0($5)
	l.d     $f14, 8($5)

	/* prepare call */

	move	$12, $7		/* $12  stack data */
	move	$25, $4		/* $25  target function */

	/* load first four integer arguments ($4-$7) */

	lw	$4, 0($12)
	lw	$5, 4($12)
	lw	$6, 8($12)
	lw	$7,12($12)

	/* call target function */

	jalr	$25
	nop
	move	$sp,$fp 	/* restore stack pointer */
	nop
	lw	$31,4($sp)	/* restore return address */
	lw	$fp,0($sp)	/* restore frame pointer */
	addiu	$sp,$sp,8	/* end stack frame */
	j	$31		/* return */
	nop

	.set	reorder
	.end	dcCall_mips_o32 
	.ident 	"handwritten"

