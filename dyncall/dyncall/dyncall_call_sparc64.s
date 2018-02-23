/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_sparc64.S
 Description: Call kernel for sparc64 (v9) ABI.
 License:

   Copyright (c) 2011-2015 Daniel Adler <dadler@uni-goettingen.de>

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


/* NOTE: %sp/%fp for v9 are offset, using them needs a "BIAS" of 2047 */
BIAS = 2047

.text
.global dcCall_v9
	/* dcCall_sparc64( DCCallVM* , void * target ) */ 
        /*                 o0          o1              */
dcCall_v9:
	or      %o0, %g0, %o3  /* o3: callvm   */
	or      %o1, %g0, %o0  /* o0: target   */
	ldx     [%o3+24], %o1  /* o1: mVecSize */
	add     %o3, 32,  %o2  /* o2: stack    */
	/* Compute a matching stack size (approximate): o3 = align(o1+136,16) */

	add     %o1, (16+1+6)*8+15, %o3
	and     %o3, -16, %o3
	neg     %o3            /* o3: -stacksize */
	save    %sp, %o3, %sp

	ldd     [%i2+8*0 ], %f0  /* Load double-precision float registers. */
	ldd     [%i2+8*1 ], %f2
	ldd     [%i2+8*2 ], %f4
	ldd     [%i2+8*3 ], %f6
	ldd     [%i2+8*4 ], %f8
	ldd     [%i2+8*5 ], %f10
	ldd     [%i2+8*6 ], %f12
	ldd     [%i2+8*7 ], %f14
	ldd     [%i2+8*8 ], %f16
	ldd     [%i2+8*9 ], %f18
	ldd     [%i2+8*10], %f20
	ldd     [%i2+8*11], %f22
	ldd     [%i2+8*12], %f24
	ldd     [%i2+8*13], %f26
	ldd     [%i2+8*14], %f28
	ldd     [%i2+8*15], %f30
	ldx     [%i2+8*0 ], %o0  /* Load output registers. */
	ldx     [%i2+8*1 ], %o1
	ldx     [%i2+8*2 ], %o2
	ldx     [%i2+8*3 ], %o3
	ldx     [%i2+8*4 ], %o4
	ldx     [%i2+8*5 ], %o5
	sub     %i1, 48, %i1
	cmp     %i1, 0
	ble     .do_call
	nop
	/* Copy loop: */
	add     %i2, 48, %i2   /* skip homing area */
	or      %g0, %g0, %l0  /* l0 = offset initialized to 0. */
	add     %sp, BIAS+((16+6)*8), %l2  /* l2 = argument area on stack space (7th word). (64+4+6*4 = byte offset 92). */
.next:
	ldx     [%i2+%l0],%l1   /* Read from arg buffer(%i2) to %l1. */
	stx     %l1, [%l2+%l0]  /* Write %l1 to stack space(%l2). */
	add     %l0, 8, %l0     /* Increment offset. */
	sub     %i1, 8, %i1     /* Decrement copy size. */
	cmp     %i1, 0
	bgt     .next
	nop
.do_call:
	call    %i0  /* Call target. */
	nop
	or      %o0, %g0, %i0   /* pass out retval (mov o0 -> i0) */
	jmpl    %i7 + 8, %g0    /* optimized restore;retl;nop */
	restore

/* 
@@@ complete manual with this, and remove from here once done

Changes from v8:
- fundamental data types
	- (un)signed int: 8,16,32,64
	- float: 32,64,128
- float: IEEE 754 compilant
	32 32-bit  float registers f0,f1,..,f31
	32 64-bit  float registers f0,f2,..,f62
	16 128-bit float registers f0,f4,..,f60

Description:
We need to raise up a dynamic stack frame.
Therefore we need to compute the stack size. We do this first, 
in the context of the caller as a leaf function (using o3 as scratch for addition).
Then we raise the frame, ending up in o0-o3 is then i0-i3.


Stack Layout:
   BIAS = 2047

   BIAS+XX: should be 16 byte aligned.
                 ...
       136: argument overflow area
       128:  1 extended word  for struct/union poiner return value
   BIAS+ 0: 16 extended words for registers (in/local) save area [register window]


Function Argument Passing:
- integer %o0..%o5 (caller view).
- floating-point %f0 .. %f31
- continuous memory starting at %sp+BIAS+136 (caller view).

Register Usage:
%fp0..%fp31  : floating-point arguments.
%sp  or  %o6 : stack pointer, always 8 (or 16?)-byte aligned.
%fp  or  %i6 : frame pointer.
%i0  and %o0 : integer and pointer return values.
%i7  and %o7 : return address. (caller puts return address to %o7, callee uses %i7)
%fp0 and %fp1: return value (float).
%i0..%i5     : input argument registers 
%o0..%o5     : output argument registers
%g0          : always zero, writes to it have no effect.

Register Mappings:
r0-7    -> globals
r8-15   -> outs
r16-r23 -> locals
r24-r31 -> ins

Integer Register Overview Table:
ID   Class   Name   Description
------------------------------------------------------------------------------
0   globals   g0    always zero, writes to it have no effect
1             g1
2             g2
3             g3
4             g4
5             g5
6             g6
7             g7
8   out       o0    [int/ptr] arg 0 and return
9             o1              arg 1
10            o2              arg 2
11            o3              arg 3
12            o4              arg 4
13            o5              arg 5
14            o6    stack pointer
15            o7
16  local     l0    scratch
17            l1
18            l2
19            l3
20            l4
21            l5
22            l6
23            l7
24  in        i0    [int/pt] arg 0 and return
25            i1
26            i2
27            i3
28            i4
29            i5
30            i6    frame pointer
31            i7
*/

