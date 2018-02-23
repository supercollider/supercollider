/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_x64.S
 Description: All x64 abi call kernel implementation
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



#include "../portasm/portasm-x64.S"

BEGIN_ASM

/*---------------------------------------------------------------------------

  Call Kernel for x64 System V

  Input:
    RDI : size of arguments to be passed via stack
    RSI : pointer to arguments to be passed via the stack
    RDX : pointer to arguments of integral/pointer type to be passed via registers
    RCX : pointer to arguments of floating point type to be passed via registers
    R8  : target function pointer
  Notes:
    RSP+8: is always 16-byte aligned (32-byte align if __m256 is used)
*/

GLOBAL(dcCall_x64_sysv)
BEGIN_PROC(dcCall_x64_sysv)
	PUSH(RBP)			/* Pseudo-prolog - preserve RBP. */
	PUSH(RBX)			/* Preserve RBX and store pointer to function in it. */
	MOV(RSP,RBP)			/* Store stack pointer in RBP. */
	MOV(R8 ,RBX)
	MOVSD(QWORD(RCX,0) ,XMM0)	/* Copy first 8 floats to XMM0-XMM7. */
	MOVSD(QWORD(RCX,8) ,XMM1)
	MOVSD(QWORD(RCX,16),XMM2)
	MOVSD(QWORD(RCX,24),XMM3)
	MOVSD(QWORD(RCX,32),XMM4)
	MOVSD(QWORD(RCX,40),XMM5)
	MOVSD(QWORD(RCX,48),XMM6)
	MOVSD(QWORD(RCX,56),XMM7)

	ADD(LIT(31),RDI)		/* Align stack to 32-byte. */
	AND(LIT(-32),RDI)
	ADD(LIT(8),RDI)		/* Adjust by 8-byte for the return-address. */
	SUB(RDI,RSP)			/* Setup stack frame by subtracting the size of arguments. */

	MOV(RDI,RCX)			/* Store number of bytes to copy to stack in RCX (for rep movsb). */
	MOV(RSP,RDI)			/* Store pointer to beginning of stack arguments in RDI (for rep movsb). */

	REP(MOVSB)			/* copy bytes (@@@ should be optimized, movq?). */

	MOV(QWORD(RDX,0),RDI)		/* copy first six int/pointer arguments to RDI, RSI, RDX, RCX, R8, R9. */
	MOV(QWORD(RDX,8),RSI)
	MOV(QWORD(RDX,24),RCX)
	MOV(QWORD(RDX,32),R8)
	MOV(QWORD(RDX,40),R9)
	MOV(QWORD(RDX,16),RDX)		/* Set RDX last to not overwrite it to soon. */

	MOVB(LIT(8),AL)			/* Put upper bound of number of used xmm registers in AL. */
	CALL_REG(RBX)			/* Call function. */

	MOV(RBP,RSP)			/* Restore stack pointer. */
	POP(RBX)			/* Restore RBX. */
	POP(RBP)			/* Pseudo-epilog. */
	RET()
END_PROC(dcCall_x64_sysv)

/*---------------------------------------------------------------------------

  Call Kernel for x64 Win64

  Input:
    RCX : size of arguments to be passed via stack
    RDX : pointer to arguments to be passed via the stack
    R8  : pointer to arguments of integral/pointer type to be passed via registers
    R9  : target function pointer

*/

GLOBAL(dcCall_x64_win64)
BEGIN_PROC(dcCall_x64_win64)

	PUSH(RBP)			/* Pseudo-prolog - preserve RBP. */
	PUSH(RSI)			/* Preserve RSI and RDI. */
	PUSH(RDI)

	MOV(RSP,RBP)			/* Store stack pointer in RBP. */

	ADD(LIT(15),RCX)		/* Align stack size to 16 bytes. */
	AND(LIT(-16),RCX)
	SUB(RCX,RSP)			/* Setup stack frame by subtracting the size of the arguments. */


	MOV(RDX, RSI)			/* Let RSI point to the arguments. */
	MOV(RSP, RDI)			/* Store pointer to beginning of stack arguments in RDI (for rep movsb). */
	MOV(R9,  RAX)			/* Put function address in RAX. */

	REP(MOVSB) 			/* @@@ should be optimized (e.g. movq) */

	MOV(QWORD(R8,0),RCX)		/* Copy first four arguments to RCX, RDX, R9, R8 ( and XMM0-XMM3. ) */
	MOV(QWORD(R8,8),RDX)
	MOV(QWORD(R8,24),R9)
	MOV(QWORD(R8,16),R8)

	MOVD(RCX, XMM0)
	MOVD(RDX, XMM1)
	MOVD(R8,  XMM2)
	MOVD(R9,  XMM3)

	PUSH(R9)			/* Push first four arguments onto the stack preserve area. */
	PUSH(R8)
	PUSH(RDX)
	PUSH(RCX)

	CALL_REG(RAX)			/* Invoke function. */

	MOV(RBP, RSP)			/* Restore stack pointer (such that we can pop the preserved vALues). */

	POP(RDI)			/* Restore RSI and RDI. */
	POP(RSI)
	POP(RBP)			/* Pseudo-epilog. */

	RET()

END_PROC(dcCall_x64_win64)

END_ASM

