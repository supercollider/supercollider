/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_ppc64.S
 Description: Call Kernel for PowerPC 64-bit Architecture
 License:

   Copyright (c) 2014-2015 Masanori Mitsugi <mitsugi@linux.vnet.ibm.com>

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


#include "../portasm/portasm-ppc64.S"

/*
  Call Kernel Implementations for PowerPC64.
*/

/* ============================================================================
   DynCall Call Kernels for PPC64 Architecture
   -------------------------------------------------------------------------
   C Interface:
     struct DCRegData { int i[8]; double d[13]; };
     dcCall_ppc64(DCpointer target, struct DCRegData* pRegData, DCsize stacksize, DCptr stackdata);
   
  ChangeLog:
  2015-07-08: Added support for system calls
  2014-08-07: Initial Support

*/

/* ----------------------------------------------------------------------------

  Call Kernel for ppc64

  Input:
    r3 : target address ptr 
    r4 : register data ptr (8 x GPR 64 bytes, 13 x FPR 64 bytes)
    r5 : stack data size  
    r6 : stack data ptr

  Details:
   - Stack frames are always aligned on 16 byte
   - Reserve GPR2 (System register)  
   - The GPR3 .. GPR10 are loaded
   - The FPR1 .. FPR8 are loaded
   - No support for Vector Parameters so far. 
   - Parameter Area (min. v1:64 Bytes v2:0 Byte)
   - Frame Header Area (v1:48 Bytes v2:32 Bytes)

   Frame structure:

     on entry, parent frame layout:

     offset
     16:     LR save word (Callee stores LR in parent frame)
     0:      parent stack frame (back-chain)

     after frame initialization:

    	v1: stack size = ( (48+64+8+15) + stacksize ) & -(16)
    	v2: stack size = ( (32+0+8+15) + stacksize ) & -(16)

     ...     locals and register spills
     48 or 32:      parameter list area
     16:      LR save word (Callee stores LR in parent frame)
     0:      parent stack frame (back-chain)
*/

/* Constants */
#if DC__ABI_PPC64_ELF_V != 2
STACK_MIN  = 120  /* v1 */
TOC_SAVE   = 40
PARAM_SAVE = 48
#else
STACK_MIN  = 40   /* v2 */
TOC_SAVE   = 24
PARAM_SAVE = 32
#endif

.text
.align 2
GLOBAL_C(dcCall_ppc64)
ENTRY_C(dcCall_ppc64)
	mflr r0                 /* r0 = return address */
	std  r0,16(r1)          /* store r0 to link-area */
	std  r31,-8(r1)

	/* compute aligned stack-size */
	
	/* add link area and align to 16 byte (+15) */
	
	/* r0 = stacksize + frame parameter(back-chain link, this callee's call return address) */
	addi r0,r5,STACK_MIN+15 /* r0 = r5 + STACK_MIN + 15 */
	rlwinm r0,r0,0,0,27     /* r0 = r0 and -15 */
	neg r0,r0               /* r0 = -r0 */
	stdux r1,r1,r0          /* store r1 and decrement */

	/* copy stack data */

	subi r6,r6,8            /* r6 = 8 bytes before source stack ptr */
	addi r7,r1,PARAM_SAVE-8 /* r7 = 8 bytes before target stack parameter-block */

	srwi r5,r5,3            /* r5 = size in double words */

	cmpi cr0,r5,0           /* if stacksize != 0 .. */
	beq  cr0,.copy_done

	mtctr r5                /* copy loop */

.copy_next:
	ldu  r0, 8(r6)
	stdu r0, 8(r7)
	bdnz .copy_next

.copy_done:

	/* this call support using ctr branch register */

	mr    r12, r3           /* r12 = target function */
	std   r2,TOC_SAVE(r1)
#if DC__ABI_PPC64_ELF_V != 2
	ld    r2,8(r12)
	ld    r0,0(r12)
	mtctr r0
#else
	mtctr r12
#endif
	mr    r11, r4           /* r11 = reg data */

	/* load 8 integer registers */
	
	ld  r3 , 0(r11)
	ld  r4 , 8(r11)
	ld  r5 ,16(r11)
	ld  r6 ,24(r11)
	ld  r7 ,32(r11)
	ld  r8 ,40(r11)
	ld  r9 ,48(r11)
	ld  r10,56(r11)

	/* load 13 float registers */
	
	lfd  f1 , 64(r11)
	lfd  f2 , 72(r11)
	lfd  f3 , 80(r11)
	lfd  f4 , 88(r11)
	lfd  f5 , 96(r11)
	lfd  f6 ,104(r11)
	lfd  f7 ,112(r11)
	lfd  f8 ,120(r11)
	lfd  f9 ,128(r11)
	lfd  f10,136(r11)
	lfd  f11,144(r11)
	lfd  f12,152(r11)
	lfd  f13,160(r11)

	bctrl                    /* branch with this call support */

	/* epilog */

	ld   r2,TOC_SAVE(r1)
	ld   r1, 0(r1)          /* restore stack */
	ld   r31,-8(r1)
	ld   r0,16(r1)          /* r0 = return address */
	mtlr r0                 /* setup link register */
	blr                     /* return */

.align 2
GLOBAL_C(dcCall_ppc64_syscall)
ENTRY_C(dcCall_ppc64_syscall)
	mflr r0                 /* r0 = return address */
	std  r0,16(r1)          /* store r0 to link-area */
	std  r31,-8(r1)
	li   r0, -STACK_MIN
	stdux r1,r1,r0          /* store r1 and decrement */

	mr   r0, r3             /* r0 = syscall number ( passed as 'target function' ) */
	mr   r11, r4            /* r11 = reg data */

	/* load 5 integer registers */
	ld  r3 , 0(r11)
	ld  r4 , 8(r11)
	ld  r5 ,16(r11)
	ld  r6 ,24(r11)
	ld  r7 ,32(r11)
	
	sc                    /* system call */

	/* epilog */

	ld   r2,TOC_SAVE(r1)
	ld   r1, 0(r1)          /* restore stack */
	ld   r31,-8(r1)
	ld   r0,16(r1)          /* r0 = return address */
	mtlr r0                 /* setup link register */
	blr                     /* return */
