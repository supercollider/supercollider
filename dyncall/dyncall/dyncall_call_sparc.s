/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_sparc.S
 Description: Call kernel for sparc processor architecture.
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




/* --------------------------------------------------------------------------- 

@@@ this should all go in manual

call kernel for sparc 32-bit
----------------------------
tested on linux/debian [gcc54.fsffrance.org - thanx to the farm!] 

new C Interface:
  void dcCall_sparc (DCCallVM* callvm, DCpointer target);
                     %i0               %1 

we need to do that, due to the special property of sparc, its 'register windows'
that propagate input registers..
otherwise, we would have a 'void' return-value layer which results in failure
to propagate back return values.
instead of implementing 'dummy'-C return-values, we call directly.

in sparc, this is simply a leaf-function layer using %o3.

old C Interface:
  void dcCall_sparc (DCpointer target, DCsize size, DCpointer data);
                     %i0             , %i1        , %i2


Input:
  i0   callvm
  i1   target

old Input:
  i0   target
  i1   size
  i2   data

Description:
We need to raise a dynamic stack frame.
Therefore we need to compute the stack size in the context of the caller as a leaf note (using o3 in addition).
Then we raise the frame.

sparc:
- big endian

sparc V8:
- integer/pointer: 32 32-bit integers.
- float: 8 quad precision, 16 double precision, 32 single precision.

sparc V9:
- integer/pointer: 32 64-bit integers.

plan9:
- completely different scheme - similar to mips/plan9.
- registers are named r0 .. r31
  r1 stack pointer
  r2 static base register
  .. to be continued..

Stack Layout 32-Bit Model:
- sp+92 seventh argument
- sp+68 first argument
- sp+64 
- 16 registers save area (in/local).

	XX: should be 8 byte aligned (min stack frame size is 96).
	            ...
	92: on stack argument 6
	88: input argument 5 spill
	            ...
	68: input argument 0 spill
	64: struct/union pointer return value
	 0: 16 registers save area

Stack Layout 64-Bit Model:
	 XX: should be 16 byte aligned (min stack frame size is 172).
	168: on stack argument 6
	136: input argument 0 spill
	128: struct/union poiner return value
	  0: 16 registers save area



Register Usage:
%sp or  %o6: stack pointer, always 8 (or 16?)-byte aligned.
%fp or  %i6: frame pointer.
%i0 and %o0: integer and pointer return values.
%i7 and %o7: return address. (caller puts return address to %o7, callee uses %i7)
%f0 and %f1: return value (float).
%i0..%i5:    input argument registers 
%o0..%o5:    output argument registers
%g0:         always zero, writes to it have no effect.

Register Mappings:
r0-7    -> globals
r8-15   -> outs
r16-r23 -> locals
r24-r31 -> ins

*/

REGSIZE        =  4
ALIGN          = 16
CALLVM_size    = 12
CALLVM_dataoff = 16

.text
.global dcCall_sparc
dcCall_sparc:

/* Basic Prolog: supports up to 6 arguments. */

	/* o0-1: callvm,target */
	or   %o0, %g0, %o3             /* %o3: callvm */
	or   %o1, %g0, %o0             /* %o0: target */
	ld  [%o3+CALLVM_size], %o1     /* %o1: size */
	add  %o3, CALLVM_dataoff, %o2  /* %o2: data */
	/*o0-2:target,size,data*/

	/*leaf functions: may use the first six output registers.*/
	/*o3-5:free to use */

	/* Compute a matching stack size (approximate): o3 = align(92+o1,16) */
	add     %o1, (16+1+6)*REGSIZE+ALIGN-1, %o3
	and     %o3,   -ALIGN, %o3
	neg     %o3

	/* Prolog. */
	save    %sp, %o3, %sp    /* min stack size (16+1+6)*sizeof(ptr)=92 paddded to 8-byte alignment => min frame size of 96 bytes. */

	/* Load output registers. */
	ld      [%i2           ],%o0
	ld      [%i2+REGSIZE*1 ],%o1
	ld      [%i2+REGSIZE*2 ],%o2
	ld      [%i2+REGSIZE*3 ],%o3
	ld      [%i2+REGSIZE*4 ],%o4
	ld      [%i2+REGSIZE*5 ],%o5

	/* Copy on stack? */
	sub     %i1, REGSIZE*6, %i1   /* i1 = decrement copy size by 6 regs (=6 regs x 4 bytes = 24 bytes total). */
	cmp     %i1, 0
	ble     .do_call
	nop

	/* Copy loop: */
	add     %i2, REGSIZE*6, %i2         /* i2 = address of 7th word of args buffer. */
	or      %g0, %g0, %l0               /* l0 = offset initialized to 0. */
	add     %sp, (16+1+6)*REGSIZE, %l2  /* l2 = argument area on stack space (7th word). (64+4+6*4 = byte offset 92). */
.next:
	ld      [%i2+%l0],%l1      /* Read from arg buffer(%i2) to %l1. */
	st      %l1, [%l2+%l0]     /* Write %l1 to stack space(%l2). */
	add     %l0, REGSIZE, %l0  /* Increment offset. */
	sub     %i1, REGSIZE, %i1  /* Decrement copy size. */
	cmp     %i1, 0
	bgt     .next
	nop
.do_call:
	call    %i0   /* Call target. */
	nop
	or      %o0, %g0, %i0
	or      %o1, %g0, %i1
	jmpl    %i7 + 8, %g0       /* optimized restore;retl;nop */
	restore

