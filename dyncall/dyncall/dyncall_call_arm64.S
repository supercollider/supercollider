/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_arm64.S
 Description: Call Kernel for ARM 64-bit Architecture (aka ARM64, AArch64)
 License:

   Copyright (c) 2015 Daniel Adler <dadler@uni-goettingen.de>, 
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



#include "../portasm/portasm-arm.S"

/* ============================================================================
   DynCall Call Kernel for ARM 64-bit ARM Architecture 
   ----------------------------------------------------------------------------
   C Interface:
     dcCall_arm64 (DCpointer target, DCpointer data, DCsize size, DCfloat* regdata);

   This Call Kernel was tested on Debian/qemu-debootstrap arm64 jessie.
*/

.text
//
// DynCall Back-End arm64 
// 
// Supported ABIs:
// - 'ARM 64-bit AArch64 PCS' (@dadler: work in progress)
// 
// Useful Links:
// - http://lxr.free-electrons.com/source/arch/arm64/kernel/stacktrace.c

GLOBAL_C(dcCall_arm64)
ENTRY_C(dcCall_arm64)
.align 2

// input:
//   x0: target   (address of target)
//   x1: data     (address of stack copy data)
//   x2: size     (number of 'pair' 16-byte units)
//   x3: regdata  (address of register data)

// prolog:
	
	stp  x29, x30, [sp, #-16]!	// allocate frame
	mov  x29,  sp

// load 64-bit floating-point registers
        
	ldr  d0,  [x3,#0 ]
	ldr  d1,  [x3,#8 ]
	ldr  d2,  [x3,#16]
	ldr  d3,  [x3,#24]
	ldr  d4,  [x3,#32]
	ldr  d5,  [x3,#40]
	ldr  d6,  [x3,#48]
	ldr  d7,  [x3,#56]

// copy to stack
	
	sub  sp, sp, x2		// create call-frame

	eor  x4, x4, x4		// x4: cnt = 0

	mov  x5, x1		// x5: read pointer = data
	mov  x6, sp		// x6: write pointer = sp

.next:
	cmp  x4, x2
	b.ge  .done
	
	ldp  x7, x9, [x5], #16	// get pair from data
	stp  x7, x9, [x6], #16	// put to stack
	add  x4, x4, 16	        // advance 16 bytes

	b   .next

.done:
	
// rescue temp int registers

	mov  x9 , x0			// x9: target
	add  x10, x3, 64                // x3: integer reg buffer

// load 64-bit integer registers ( 8 x 64-bit )
	
	// load register set

	ldr  x0, [x10, #0]
	ldr  x1, [x10, #8]
	ldr  x2, [x10, #16]
	ldr  x3, [x10, #24]
	ldr  x4, [x10, #32]
	ldr  x5, [x10, #40]
	ldr  x6, [x10, #48]
	ldr  x7, [x10, #56]
	
// call target:
	
	blr  x9

// epilog:

	mov  sp,  x29
	ldp  x29, x30, [sp], 16

	ret

#if 0

	

// epilog:

	add  sp, x28, 0		// remove call record
	
	ret

	// -- OLD:

	str  x27,      [sp, 16]			// use 1 local var (size)
	

        ldr  q0,  [x3,#0 ]
	ldr  q1,  [x3,#8 ]
	ldr  q2,  [x3,#16]
	ldr  q3,  [x3,#24]
	ldr  q4,  [x3,#32]
	ldr  q5,  [x3,#40]
	ldr  q6,  [x3,#48]
	ldr  q7,  [x3,#56]

	ldr  d8,  [x3,#32]
	ldr  d9,  [x3,#36]
	ldr  d10, [x3,#40]
	ldr  d11, [x3,#44]
	ldr  d12, [x3,#48]
	ldr  d13, [x3,#52]
	ldr  d14, [x3,#56]
	ldr  d15, [x3,#60]


	// load float ( 16 x 32-bit ) 

        ldr  s0,  [x3,#0 ]
	ldr  s1,  [x3,#4 ]
	ldr  s2,  [x3,#8 ]
	ldr  s3,  [x3,#12]
	ldr  s4,  [x3,#16]
	ldr  s5,  [x3,#20]
	ldr  s6,  [x3,#24]
	ldr  s7,  [x3,#28]
	ldr  s8,  [x3,#32]
	ldr  s9,  [x3,#36]
	ldr  s10, [x3,#40]
	ldr  s11, [x3,#44]
	ldr  s12, [x3,#48]
	ldr  s13, [x3,#52]
	ldr  s14, [x3,#56]
	ldr  s15, [x3,#60]
 
	// call
	
	blr  x0

	// epilog

	ldp  x29, x30, [sp], 32
	ret

	// stack copy 

	sub  sp, sp, x2	// decrement stack by size (x2) 
	eor  x3, x3, x3	// x3 = counter, set to zero

// .next:
	ldr  x4, [x1, x3]  // x4 = 64-bit stack data
	str  x4, [sp, x3]  // store to stack
	add  x3, x3, #8
	cmp  x3, x2
	blt .next


	// rescue int registers

	mov  x9 , x0			// x9 = code ptr
	mov  x10, x2

	// load int ( 8 x 64-bit )

	ldr  x0, [sp, #0]
	ldr  x1, [sp, #8]
	ldr  x2, [sp, #16]
	ldr  x3, [sp, #24]
	ldr  x4, [sp, #32]
	ldr  x5, [sp, #40]
	ldr  x6, [sp, #48]
	ldr  x7, [sp, #56]

	// call
	
	blr  x9

	// epilog

	ldp  x29, x30, [sp], 32
	ret
#endif
 

