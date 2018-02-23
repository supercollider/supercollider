/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_ppc32_apple.s
 Description: Callback Thunk - PowerPC 32-bit System V ABI
 License:

   Copyright (c) 2007-2015 Daniel Adler <dadler@uni-goettingen.de>,
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

.machine ppc
.text
.align 2

/* Callback Thunk Entry code for PowerPC 32-bit Darwin/Apple Mac OS X. */


/* Stack Frame Layout: 

	204 DCValue ()
	 56 DCArgs (32+104+4+8 = 148)
	 24 Parameter area (4*8 = 32)
	  0 Linkage area   (24)

*/

/* Constants. */
INT_REGS      = 8
FLOAT_REGS    = 13
SIZEOF_INT    = 4
SIZEOF_DOUBLE = 8


/* Linkage area. */
LINK_SP     = 0
LINK_CR     = 4
LINK_LR     = 8
LINK_OFFSET = 0
LINK_SIZE   = 24
/* Parameter area. */
PAR_OFFSET = LINK_SIZE
PAR_SIZE   = 32
/* local struct DCArgs */
ARGS_OFFSET = (PAR_OFFSET+PAR_SIZE)
ARGS_SIZE   = (SIZEOF_INT*INT_REGS)+(SIZEOF_DOUBLE*FLOAT_REGS) /* = 136 */
/* local struct DCValue */
RESULT_OFFSET = (ARGS_OFFSET+ARGS_SIZE)
RESULT_SIZE   = 16
/* additional locals (reg 30/31) */
LOCALS_OFFSET = (RESULT_OFFSET+RESULT_SIZE)
LOCALS_SIZE   = 2*SIZEOF_INT
/* total */
FRAME_SIZE = ((LOCALS_OFFSET+LOCALS_SIZE)+15 & (-16))

/* struct DCCallback */
DCB_THUNK      = 0
DCB_HANDLER    = 24
DCB_STACKCLEAN = 28
DCB_USERDATA   = 32

/* struct DCArgs */
DCA_IARRAY = 0
DCA_FARRAY = SIZEOF_INT*INT_REGS
DCA_SP     = DCA_FARRAY + SIZEOF_DOUBLE*FLOAT_REGS
DCA_ICOUNT = DCA_SP + 4
DCA_FCOUNT = DCA_ICOUNT + 4

iregfile = ARGS_OFFSET+DCA_IARRAY
fregfile = ARGS_OFFSET+DCA_FARRAY
save_sp  = ARGS_OFFSET+DCA_SP
icount   = ARGS_OFFSET+DCA_ICOUNT
fcount   = ARGS_OFFSET+DCA_FCOUNT

.globl _dcCallbackThunkEntry

/* 
  Thunk entry:
  r2 = DCCallback*
*/
_dcCallbackThunkEntry:

	mflr    r0
	stw     r0, 8(r1)             /* store return address */
	addi    r12,r1, PAR_OFFSET    /* temporary r12 = parameter area on callers stack frame */
	stwu    r1, -FRAME_SIZE(r1)   /* save callers stack pointer and make new stack frame. */
	stw     r3, iregfile+ 0*4(r1) /* spill 8 integer parameter registers */
	stw     r4, iregfile+ 1*4(r1)
	stw     r5, iregfile+ 2*4(r1)
	stw     r6, iregfile+ 3*4(r1)
	stw     r7, iregfile+ 4*4(r1)
	stw     r8, iregfile+ 5*4(r1)
	stw     r9, iregfile+ 6*4(r1)
	stw     r10,iregfile+ 7*4(r1)
	stfd    f1, fregfile+ 0*8(r1) /* spill 13 float parameter registers */
	stfd    f2, fregfile+ 1*8(r1)
	stfd    f3, fregfile+ 2*8(r1)
	stfd    f4, fregfile+ 3*8(r1)
	stfd    f5, fregfile+ 4*8(r1)
	stfd    f6, fregfile+ 5*8(r1)
	stfd    f7, fregfile+ 6*8(r1)
	stfd    f8, fregfile+ 7*8(r1)
	stfd    f9, fregfile+ 8*8(r1)
	stfd    f10,fregfile+ 9*8(r1)
	stfd    f11,fregfile+10*8(r1)
	stfd    f12,fregfile+11*8(r1)
	stfd    f13,fregfile+12*8(r1)
	/* initialize struct DCCallback */
	stw     r12,save_sp(r1) /* init stack pointer */
	xor     r0, r0, r0      /* init register counters */
	stw     r0, icount(r1)
	stw     r0, fcount(r1)
	/* invoke callback handler */
	mr      r3, r2                  /* arg 1: DCCallback* pcb */
	addi    r4, r1, ARGS_OFFSET     /* arg 2: DCArgs* args    */
	addi    r5, r1, RESULT_OFFSET   /* arg 3: DCValue* result */
	lwz     r6, DCB_USERDATA(r2)    /* arg 4: void* userdata  */

	/* branch-and-link to DCCallback.handler */
	lwz     r12,DCB_HANDLER(r2)
	mtctr   r12
	bctrl
	/* switch on base result type */
	cmpi    cr0, r3, 0x66 /* 'f */
	beq .f32
	cmpi    cr0, r3, 0x64 /* 'd */
	beq .f64
.i64:
	lwz     r3, RESULT_OFFSET     (r1)
	lwz     r4, RESULT_OFFSET + 4 (r1)
.end:
	lwz     r1,  0(r1) /* restore stack pointer */
	lwz     r0,  8(r1) /* load link register with return address */
	mtlr    r0
	blr                /* branch back to link register */
.f32:
	lfs     f1, RESULT_OFFSET(r1)
	b .end
.f64:
	lfd     f1, RESULT_OFFSET(r1)
	b .end

