/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_mips_n64_gas.s
 Description: mips "n64" abi call kernel implementation in GNU Assembler
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

	.section .mdebug.abi64
	.previous
	.abicalls
	.text
	.align	2
	.globl	dcCall_mips_n64
	.ent	dcCall_mips_n64
dcCall_mips_n64:

	/* Stack-frame prolog */

	# .frame	$fp,64,$31		/* vars=16, regs=3/0, args=0, extra=16 */
	# .mask	0xd0000000,-8
	# .fmask	0x00000000,0
	dsubu	$sp,$sp,64
	sd	$31,48($sp)	/* save return address register (ra) */
	sd	$30,40($sp)	/* save frame pointer register (fp) */
	sd	$28,32($sp)	/* save global pointer register (gp) */
	move	$fp,$sp


	/* arguments: */
		
	/* $4 target function */
 	/* $5 register data */
	/* $6 stack size */
	/* $7 stack data */
	

	/* allocate argument stack space */

	dsubu	$sp, $sp, $6
	
	/* copy stack data */

	/* n64 abi call assumptions:
           - stack data is 16-byte aligned.
           - no extra-storage for arguments passed via registers.
        */

	/* $12  source pointer (parameter stack data) */
	/* $14  destination (stack pointer) */
	/* $6   byte count */

	move	$12, $7
	move	$14, $sp

.next:
	beq	$6, $0, .skip
	# nop
	daddiu	$6, $6, -8
	ld	$2, 0($12)
	sd	$2, 0($14)
	daddiu	$12,$12, 8
	daddiu	$14,$14, 8
	b	.next
.skip:
	move	$25, $4

	/* load registers */

	/* locals: */
	/* $13 = register data */
	/* $14 = useDouble flags */
	move	$13, $5
	ld	$14, 128($13)
	
	/* load integer parameter registers */

	ld	$4 , 0($13)
	ld	$5 , 8($13)
	ld	$6 ,16($13)
	ld	$7 ,24($13)
	ld	$8 ,32($13)
	ld	$9 ,40($13)
	ld	$10,48($13)
	ld	$11,56($13)

	/* load float-or-double floating pointer parameter registers 
           a 64-bit bitmask given at byte offset 128 of regdata indicates
           if loading a float (bit cleared) or double (bit set), starting
           at bit position 0 in bitmask.
        */
.t0:
	and     $15, $14, 1
	bgtz	$15, .d0
.f0:
	l.s	$f12, 64($13)
	j	.t1
.d0:
	l.d	$f12, 64($13)

.t1:
	and	$15, $14, 2
	bgtz	$15, .d1
.f1:
	l.s	$f13, 72($13)
	j	.t2
.d1:
	l.d	$f13, 72($13)
.t2:	
	and	$15, $14, 4
	bgtz	$15, .d2
.f2:
	l.s	$f14, 80($13)
	j	.t3
.d2:	
	l.d	$f14, 80($13)
.t3:
	and	$15, $14, 8
	bgtz	$15, .d3
.f3:
	l.s	$f15, 88($13)
	j	.t4
.d3:
	l.d	$f15, 88($13)
.t4:
	and	$15, $14, 16
	bgtz	$15, .d4
.f4:
	l.s	$f16, 96($13)
	j	.t5
.d4:
	l.d	$f16, 96($13)
.t5:
	and	$15, $14, 32
	bgtz	$15, .d5
.f5:
	l.s	$f17,104($13)
	j	.t6
.d5:
	l.d	$f17,104($13)
.t6:
	and	$15, $14, 64
	bgtz	$15, .d6
.f6:
	l.s	$f18,112($13)
	j	.t7
.d6:
	l.d	$f18,112($13)
.t7:
	and	$15, $14, 128
	bgtz	$15, .d7
.f7:
	l.s	$f19,120($13)
	j	.fregend
.d7:
	l.d	$f19,120($13)

.fregend:

	/* jump-and-link to register $25 */

	jal	$31, $25
	nop /* branch-delay slot - no nop according to gcc assembly */
	    /* output, but let's play safe */	

	/* Stack-frame epilog */	
	move	$sp,$fp 
	ld	$ra,48($sp)	/* restore ra register */
	ld	$fp,40($sp)	/* restore fp register */
	ld	$gp,32($sp)	/* restore gp register */
	daddu	$sp,$sp,64
	j	$ra
	.end	dcCall_mips_n64
	.size	dcCall_mips_n64, .-dcCall_mips_n64

