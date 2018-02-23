/*

 Package: dyncall
 Library: test
 File: test/gen-masm/call_x64.S
 Description: 
 License:

   Copyright (c) 2011-2015 Daniel Adler <dadler@uni-goettingen.de>,
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



#if defined(GEN_MASM)
.CODE
#  define BEGIN_ASM
#  define END_ASM END
#  define GLOBAL(X) X PROC
#  define BEGIN_PROC(X) 	OPTION PROLOGUE:NONE, EPILOGUE:NONE
#  define END_PROC(X)   X ENDP
#else
	.intel_syntax
	.text
#  define BEGIN_ASM
#  define END_ASM 
#  define GLOBAL(X) .globl X
#  define BEGIN_PROC(X)  X:
#  define END_PROC(X)
#endif

BEGIN_ASM

// ---------------------------------------------------------------------------
// Call Kernel for x64 System V

GLOBAL(dcCall_x64_sysv)
BEGIN_PROC(dcCall_x64_sysv)
	// rdi : size of arguments to be passed via stack 
	// rsi : pointer to arguments to be passed via the stack 
	// rdx : pointer to arguments of integral/pointer type to be passed via registers 
	// rcx : pointer to arguments of floating point type to be passed via registers 
	// r8  : target function pointer 

	push	rbp			// Pseudo-prolog - preserve rbp. 
	push	rbx			// Preserve rbx and store pointer to function in it. 

	mov	rbp, rsp		// Store stack pointer in rbp. 

	mov	rbx, r8

	movsd	xmm0, qword ptr[rcx   ]	// Copy first 8 floats to xmm0-xmm7 (this makes rcx free to use).
	movsd	xmm1, qword ptr[rcx+ 8]
	movsd	xmm2, qword ptr[rcx+16]
	movsd	xmm3, qword ptr[rcx+24]
	movsd	xmm4, qword ptr[rcx+32]
	movsd	xmm5, qword ptr[rcx+40]
	movsd	xmm6, qword ptr[rcx+48]
	movsd	xmm7, qword ptr[rcx+56]

	sub	rsp, rdi		// Setup stack frame by subtracting the size of the arguments. 

	mov	rax, rdi		// Align stack.
	add	rax, 8
	and	rax, 15
	sub	rsp, rax

	mov	rcx, rdi		// Store number of bytes to copy to stack in rcx (for rep movsb).
	mov	rdi, rsp		// Store pointer to beginning of stack arguments in rdi (for rep movsb). 

	rep movsb			// @@@ should be optimized (e.g. movq)

	mov	rdi, qword ptr[rdx   ]	// Copy first six int/pointer arguments to rdi, rsi, rdx, rcx, r8, r9.
	mov	rsi, qword ptr[rdx+ 8]
	mov	rcx, qword ptr[rdx+24]
	mov	r8,  qword ptr[rdx+32]
	mov	r9,  qword ptr[rdx+40]
	mov	rdx, qword ptr[rdx+16]	/* Set rdx last to not overwrite it to soon. */

	mov	al, 8						/* Put upper bound of number of used xmm registers in al. */
	call	rbx						/* Invoke function. */

	mov	rsp, rbp					/* Restore stack pointer (such that we can pop the preserved values). */

	pop	rbx						/* Restore rbx. */
	pop	rbp						/* Pseudo-epilog. */

	ret
END_PROC(dcCall_x64_sysv)

// ---------------------------------------------------------------------------
// Call Kernel for x64 Win64

GLOBAL(dcCall_x64_win64)
BEGIN_PROC(dcCall_x64_win64)
	push	rbp			// Pseudo-prolog - preserve rbp.
	push	rsi			// Preserve rsi and rdi.
	push	rdi

	mov	rbp, rsp		// Store stack pointer in rbp.

	sub	rsp, rcx		// Setup stack frame by subtracting the size of the arguments.

	mov	rax, rcx		// Align stack.
	add	rax, 8
	and	rax, 15
	sub	rsp, rax

	mov	rsi, rdx		// Let rsi point to the arguments.
	mov	rdi, rsp		// Store pointer to beginning of stack arguments in rdi (for rep movsb).
	mov	rax, r9			// Put function address in rax.

	rep movsb			// @@@ should be optimized (e.g. movq)

	mov	rcx,  qword ptr[r8   ]	// Copy first four arguments to rcx, rdx, r8, r9 and xmm0-xmm3.
	mov	rdx,  qword ptr[r8+ 8]
	mov	r9,   qword ptr[r8+24]	// Set r9 first to not overwrite r8 too soon.
	mov	r8,   qword ptr[r8+16]
	movd	xmm0, rcx
	movd	xmm1, rdx
	movd	xmm2, r8
	movd	xmm3, r9

	push	r9			// Push first four arguments onto the stack preserve area.
	push	r8
	push	rdx
	push	rcx

	call	rax			// Invoke function.

	mov	rsp, rbp		// Restore stack pointer (such that we can pop the preserved values).

	pop	rdi			// Restore rsi and rdi.
	pop	rsi
	pop	rbp			// Pseudo-epilog.

	ret
END_PROC(dcCall_x64_win64)

END_ASM

