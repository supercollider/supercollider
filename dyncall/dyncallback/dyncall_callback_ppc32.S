/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_ppc32.S
 Description: Callback Thunk Entry for PowerPC 32-bit System V Big-Endian ABI
 License:

   Copyright (c) 2015 Daniel Adler <dadler@uni-goettingen.de>

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


#include "../portasm/portasm-ppc.S"

.machine ppc
.text
.align 2

/* Struct DCCallback */

DCB_THUNK    = 0
DCB_HANDLER  = 24
DCB_CLEANUP  = 28
DCB_USERDATA = 32

/* Struct DCArgs */

ARGS_IREGS = 0
ARGS_FREGS = ARGS_IREGS + 4*8
ARGS_SP    = ARGS_FREGS + 8*13
ARGS_ICNT  = ARGS_SP    + 4
ARGS_FCNT  = ARGS_ICNT  + 4
ARGS_SIZE  = ARGS_FCNT  + 4

/* Struct DCValue  */

RESULT_SIZE = 8

/* Stack Offsets: */

SP_PREV   = 0
SP_LR     = (SP_PREV  + 4)
SP_PAR    = SP_LR + 4
PAR_SZ    = 0
SP_ARGS   = SP_PAR    + PAR_SZ
SP_IREGS  = SP_ARGS   + ARGS_IREGS
SP_FREGS  = SP_ARGS   + ARGS_FREGS
SP_SP     = SP_ARGS   + ARGS_SP
SP_ICNT   = SP_ARGS   + ARGS_ICNT
SP_FCNT   = SP_ARGS   + ARGS_FCNT
SP_RESULT = SP_ARGS   + ARGS_SIZE
SP_SIZE   = SP_RESULT + RESULT_SIZE

#define ALIGN(M,X) ( M+(X-1) & (-X) )

FRAMESIZE = ALIGN(SP_SIZE,16)

GLOBAL_C(dcCallbackThunkEntry)
ENTRY_C(dcCallbackThunkEntry)

/* --------------------------------------------------------------------------

Input:
	r1      Stack Pointer            
	r3-r10  Integer Arguments        
	f1-f8   Floating-point Arguments 
	r11     Thunk Pointer            

*/

	/* prolog */

	mflr    r0
	stw     r0,     SP_LR(r1)  /* store return address */
	addi    r12,r1, SP_PAR     /* temporary r12: parameter area on callers stack frame */
	stwu    r1, -FRAMESIZE(r1)

	stw     r3, SP_IREGS + 0*4(r1)  /* spill 8 integer parameter registers */
	stw     r4, SP_IREGS + 1*4(r1)
	stw     r5, SP_IREGS + 2*4(r1)
	stw     r6, SP_IREGS + 3*4(r1)
	stw     r7, SP_IREGS + 4*4(r1)
	stw     r8, SP_IREGS + 5*4(r1)
	stw     r9, SP_IREGS + 6*4(r1)
	stw     r10,SP_IREGS + 7*4(r1)

	stfd    f1, SP_FREGS + 0*8(r1)  /* spill 8 (of 13) float parameter registers */
	stfd    f2, SP_FREGS + 1*8(r1)
	stfd    f3, SP_FREGS + 2*8(r1)
	stfd    f4, SP_FREGS + 3*8(r1)
	stfd    f5, SP_FREGS + 4*8(r1)
	stfd    f6, SP_FREGS + 5*8(r1)
	stfd    f7, SP_FREGS + 6*8(r1)
	stfd    f8, SP_FREGS + 7*8(r1)

	stw     r12,SP_SP(r1)         /* init stack pointer */
	xor     r0, r0, r0            /* init register counters */
	stw     r0, SP_ICNT(r1)
	stw     r0, SP_FCNT(r1)
	stw     r0, SP_RESULT(r1)     /* init result object */
	stw     r0, SP_RESULT + 4(r1)
	/* invoke callback handler */
	mr      r3, r11               /* arg 1: DCCallback* pcb (r11 = thunk ptr) */
	addi    r4, r1, SP_ARGS       /* arg 2: DCArgs* args    */
	addi    r5, r1, SP_RESULT     /* arg 3: DCValue* result */
	lwz     r6, DCB_USERDATA(r11) /* arg 4: void* userdata  */

	/* branch-and-link to DCCallback.handler */
	lwz     r12,  DCB_HANDLER(r11)
	mtctr   r12
	bctrl
	/* check result type */
	cmpi    cr0, r3, 0x66 /* 'f */
	beq .f32
	cmpi    cr0, r3, 0x64 /* 'd */
	beq .f64
.i64:
	lwz     r3, SP_RESULT     (r1)
	lwz     r4, SP_RESULT + 4 (r1) 
.end:
	lwz     r1, SP_PREV(r1)  /* restore stack pointer */
	lwz     r0, SP_LR(r1)    /* load link register with return address */
	mtlr    r0
	blr                      /* branch back to link register */
.f32:
	lfs     f1, SP_RESULT(r1)
	b .end
.f64:
	lfd     f1, SP_RESULT(r1)
	b .end

