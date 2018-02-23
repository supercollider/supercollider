/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_ppc32.S
 Description: Call Kernel for PowerPC 32-bit Architecture
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



#include "../portasm/portasm-ppc.S"

/*
  Call Kernel Implementations for PowerPC.
  Supported Calling Conventions: sysv, darwin, syscall

*/

	.machine ppc
	.text

/* ============================================================================
   DynCall Call Kernels for PPC32 Architecture
   -------------------------------------------------------------------------
   C Interface:
     struct DCRegData { int i[8]; double d[13]; };
     dcCall_ppc32_XXX(DCpointer target, struct DCRegData* pRegData, DCsize stacksize, DCptr stackdata);
   
   Where XXX is one of the following Calling Conventions:
	darwin, sysv
  
  ChangeLog:
  2015-01-15: Added support for system calls.
  2011-04-03: Using portasm.
  2009-01-09: Added Support for System V ABI.
  2007-11-28: Initial Support for Darwin.

*/

/*---------------------------------------------------------------------------
  
  Call Kernel for ppc32 Darwin

  Input:	
    r3 : target address ptr 
    r4 : register data ptr (8 x GPR 32 bytes, 13 x FPR 64 bytes)
    r5 : stack data size  
    r6 : stack data ptr

  Details:
  - Stack frames are always aligned on 16 byte
  - The GPR3 .. GPR10 are loaded
  - The FPR1 .. FPR13 are loaded
  - No support for Vector Parameters so far. 
  - Parameter Area (min. 32 Bytes)
  - Linkage Area (24 Bytes)
*/

	.align 2
GLOBAL_C(dcCall_ppc32_darwin)
ENTRY_C(dcCall_ppc32_darwin)


	mflr r0			/* r0 = return address */
	stw  r0,8(r1)		/* store return address in caller link-area */

	/* compute aligned stack-size */

	/* add link area and align to 16 byte border */

	addi r0,r5,24+15	/* r0 = stacksize + link area */
                                
	rlwinm r0,r0,0,0,27	/* r0 = r0 and -15 */
				/* r0 = r0 and -15 */
	neg r2,r0		/* r2 = -stacksize */

	stwux r1,r1,r2		/* r1 = r1 - stacksize */

	/* copy stack data */

	subi r6,r6,4		/* r6 = 4 bytes before source stack ptr */
	addi r7,r1,20		/* r7 = 4 bytes before target stack parameter-block */

	srwi r5,r5,2		/* r5 = size in words */

	cmpi cr0,r5,0		/* if stacksize != 0 .. */
	beq  cr0,.osx_done

	mtctr r5		/* copy loop */

.osx_next:	
	lwzu r0, 4(r6)		
	stwu r0, 4(r7)
	bdnz .osx_next

.osx_done:

	mr    r12, r3		/* r12 = target function */
	mtctr r12		/* control register = target function */
	mr     r2, r4		/* r2 = reg data */

        /* load 8 integer registers */

	lwz  r3 , 0(r2)
	lwz  r4 , 4(r2)
	lwz  r5 , 8(r2)
	lwz  r6 ,12(r2)
	lwz  r7 ,16(r2)
	lwz  r8 ,20(r2)
	lwz  r9 ,24(r2)
	lwz  r10,28(r2)

	/* load 13 float registers */

	lfd  f1 ,32(r2)
	lfd  f2 ,40(r2)
	lfd  f3 ,48(r2)
	lfd  f4 ,56(r2)
	lfd  f5 ,64(r2)
	lfd  f6 ,72(r2)
	lfd  f7 ,80(r2)
	lfd  f8 ,88(r2)
	lfd  f9 ,96(r2)
	lfd  f10,104(r2)
	lfd  f11,112(r2)
	lfd  f12,120(r2)
	lfd  f13,128(r2)

	/* branch */

	bctrl

	/* epilog */

	lwz  r1, 0(r1)		/* restore stack */
	lwz  r0, 8(r1)		/* r0 = return address */
	mtlr r0			/* setup link register */
	blr			/* return */

/* ----------------------------------------------------------------------------

  Call Kernel for ppc32 System 

  Input:	
    r3 : target address ptr 
    r4 : register data ptr (8 x GPR 32 bytes, 8 x FPR 64 bytes)
    r5 : stack data size  
    r6 : stack data ptr

  Details:
   - Stack frames are always aligned on 16 byte
   - Reserve GPR2 (System register)  
   - The GPR3 .. GPR10 are loaded
   - The FPR1 .. FPR8 are loaded
   - No support for Vector Parameters so far. 

   Frame structure:

     on entry, parent frame layout:

     offset
     4:      LR save word (Callee stores LR in parent frame)
     0:      parent stack frame (back-chain)

     after frame initialization:

    	stack size = ( (8+15) + stacksize ) & -(16)

     ...     locals and register spills
     8:      parameter list area
     4:      LR save word (Callee stores LR in parent frame)
     0:      parent stack frame (back-chain)
*/

	.align 2
GLOBAL_C(dcCall_ppc32_sysv)
ENTRY_C(dcCall_ppc32_sysv)
	mflr r0			/* r0 = return address */
	stw  r0,4(r1)		/* store r0 to link-area */

	/* compute aligned stack-size */
	
	/* add link area (+8) and align to 16 byte (+15) */
	
	/* r0 = stacksize + frame parameter(back-chain link, this callee's call return address) */
	addi r0,r5,8+15		/* r0 = r5 + 8 + 15 */
	rlwinm r0,r0,0,0,27  	/* r0 = r0 and -15 */
	neg r0,r0 		/* r0 = -r0 */
	stwux r1,r1,r0 		/* store r1 and decrement */

	/* copy stack data */

	subi r6,r6,4		/* r6 = 4 bytes before source stack ptr */
	                      
			        /* 4 bytes before target stack parameter-block */
	addi r7,r1,4		/* r7 = r1 + 8 offset - 4 displacement */

	srwi r5,r5,2		/* r5 = size in words */

	cmpi cr0,r5,0		/* if stacksize != 0 .. */
	beq  cr0,.sysv_done

	mtctr r5		/* copy loop */

.sysv_next:	
	lwzu r0, 4(r6)		
	stwu r0, 4(r7)
	bdnz .sysv_next

.sysv_done:

	/* this call support using ctr branch register */

	mr    r12, r3		/* r12 = target function */
	mtctr r12		/* control register = r12 */
	mr    r11, r4		/* r11 = reg data */

	/* load 8 integer registers */
	
	lwz  r3 , 0(r11)
	lwz  r4 , 4(r11)
	lwz  r5 , 8(r11)
	lwz  r6 ,12(r11)
	lwz  r7 ,16(r11)
	lwz  r8 ,20(r11)
	lwz  r9 ,24(r11)
	lwz  r10,28(r11)

	/* load 8 float registers */
	
	lfd  f1 ,32(r11)
	lfd  f2 ,40(r11)
	lfd  f3 ,48(r11)
	lfd  f4 ,56(r11)
	lfd  f5 ,64(r11)
	lfd  f6 ,72(r11)
	lfd  f7 ,80(r11)
	lfd  f8 ,88(r11)
	
	creqv 6,6,6		/* used for ellipsis calls */
	
	bctrl 			/* branch with this call support */

	/* epilog */

	lwz  r1, 0(r1)		/* restore stack */
	lwz  r0, 4(r1)		/* r0 = return address */
	mtlr r0			/* setup link register */
	blr			/* return */


	.align 2

GLOBAL_C(dcCall_ppc32_syscall)
ENTRY_C(dcCall_ppc32_syscall)
	mflr r0			/* r0 = return address */
	stw  r0,4(r1)		/* store r0 to link-area */
	li   r0, -8
	stwux r1,r1,r0 		/* store r1 and decrement */
	
	mr   r0, r3		/* r0 = syscall number ( passed as 'target function' ) */
	mr   r11, r4		/* r11 = reg data */
	lwz  r3 , 0(r11)
	lwz  r4 , 4(r11)
	lwz  r5 , 8(r11)
	lwz  r6 ,12(r11)
	lwz  r7 ,16(r11)
	sc

	lwz  r1, 0(r1)		/* restore stack */
	lwz  r0, 4(r1)		/* r0 = return address */
	mtlr r0			/* setup link register */
	blr
	
