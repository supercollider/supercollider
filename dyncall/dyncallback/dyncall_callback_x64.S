/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_x64.S
 Description: Callback Thunk entry for x64 (portasm version)
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



#include "../portasm/portasm-x64.S"

/* structure sizes */

SET(DCThunk_size,24)
SET(DCArgs_size_win64,80)
SET(DCArgs_size_sysv,128)
SET(DCValue_size,8)

/* frame local variable offsets relative to %rbp*/

SET(FRAME_arg0_win64,48)
SET(FRAME_arg0_sysv,16)
SET(FRAME_return,8)
SET(FRAME_parent,0)
SET(FRAME_DCArgs_sysv,-128)
SET(FRAME_DCValue_sysv,-136)
SET(FRAME_DCArgs_win64,-80)
SET(FRAME_DCValue_win64,-80)

/* struct DCCallback */

SET(CTX_thunk,0)
SET(CTX_handler,24)
SET(CTX_userdata,32)
SET(DCCallback_size,40)

/* character constants */

#define ASCII_f 102
#define ASCII_d 100

GLOBAL(dcCallback_x64_sysv)
BEGIN_PROC(dcCallback_x64_sysv)

	PUSH(RBP)
	MOV(RSP,RBP)

	/* initialize DCArgs */

	/* float parameters (8 registers spill to DCArgs) */

	SUB(LIT(8*8),RSP)

	MOVSD(XMM7, QWORD(RSP,8*7))		/* struct offset 120: float parameter 7 */
	MOVSD(XMM6, QWORD(RSP,8*6))		/* struct offset 112: float parameter 6 */
	MOVSD(XMM5, QWORD(RSP,8*5))		/* struct offset 104: float parameter 5 */
	MOVSD(XMM4, QWORD(RSP,8*4))		/* struct offset  96: float parameter 4 */
	MOVSD(XMM3, QWORD(RSP,8*3))		/* struct offset  88: float parameter 3 */
	MOVSD(XMM2, QWORD(RSP,8*2))		/* struct offset  80: float parameter 2 */
	MOVSD(XMM1, QWORD(RSP,8*1))		/* struct offset  72: float parameter 1 */
	MOVSD(XMM0, QWORD(RSP,8*0))		/* struct offset  64: float parameter 0 */

	/* integer parameters (6 registers spill to DCArgs) */

	PUSH(R9)				/* struct offset 56: parameter 5 */
	PUSH(R8)				/* struct offset 48: parameter 4 */
	PUSH(RCX)				/* struct offset 40: parameter 3 */
	PUSH(RDX)				/* struct offset 32: parameter 2 */
	PUSH(RSI)				/* struct offset 24: parameter 1 */
	PUSH(RDI)				/* struct offset 16: parameter 0 */
	
	/* register counts for integer/pointer and float regs */

	PUSH(LIT(0))				/* struct offset 12: fcount */
						/* struct offset  8: icount */

	LEA(QWORD(RBP,FRAME_arg0_sysv),RDX)		/* struct offset  0: stack pointer */
	PUSH(RDX)

	MOV(RSP,RSI)				/* arg 1 RSI : DCArgs* */

	/* initialize DCValue */

	PUSH(LIT(0))				/* struct offset 0: return value (max long long) */

	/* call handler( *ctx, *args, *value, *userdata) */

	MOV(RAX,RDI)				/* arg 0 RDI : DCCallback* (RAX) */
	MOV(QWORD(RDI,CTX_userdata),RCX)	/* arg 3 RCX : userdata* */
	MOV(RSP,RDX)				/* arg 2 RDX : DCValue*  */
	PUSH(LIT(0))				/* align to 16 bytes */
	CALL_REG(QWORD(RAX,CTX_handler))

	/* pass return type via registers, handle ints and floats */
	
	MOV(QWORD(RBP,FRAME_DCValue_sysv),RAX)
	MOVD(RAX,XMM0)

	MOV(RBP,RSP)
	POP(RBP)
	RET()

END_PROC(dcCallback_x64_sysv)

GLOBAL(dcCallback_x64_win64)
BEGIN_PROC(dcCallback_x64_win64)

	PUSH(RBP)
	MOV(RSP,RBP)

	/* initialize DCArgs */

	/* float parameters (4 registers spill to DCArgs) */

	SUB(LIT(4*8),RSP)

	MOVSD(XMM3, QWORD(RSP,8*3))		/* struct offset  72: float parameter 3 */
	MOVSD(XMM2, QWORD(RSP,8*2))		/* struct offset  64: float parameter 2 */
	MOVSD(XMM1, QWORD(RSP,8*1))		/* struct offset  56: float parameter 1 */
	MOVSD(XMM0, QWORD(RSP,8*0))		/* struct offset  48: float parameter 0 */

	/* integer parameters (4 registers spill to DCArgs) */

	PUSH(R9)				/* struct offset 40: parameter 3 */
	PUSH(R8)				/* struct offset 32: parameter 2 */
	PUSH(RDX)				/* struct offset 24: parameter 1 */
	PUSH(RCX)				/* struct offset 16: parameter 0 */
	
	/* register counts for integer/pointer and float regs */

	PUSH(LIT(0))				/* struct offset 12: fcount */
						/* struct offset  8: icount */

	LEA(QWORD(RBP,FRAME_arg0_win64),RDX)		/* struct offset  0: stack pointer */
	PUSH(RDX)

	MOV(RSP,RDX)				/* arg 1 RDX : DCArgs* */
	
	/* initialize DCValue */

	// PUSHQ(LIT(0))				/* struct offset 0: return value (max long long) */

	/* call handler( *ctx, *args, *value, *userdata) */

	MOV(RAX,RCX)				/* arg 0 RCX : DCCallback* (RAX) */
	MOV(QWORD(RAX,CTX_userdata),R9)	/* arg 3 R9  : userdata* */
	MOV(RSP,R8)				/* arg 2 R8  : DCValue*  */
	SUB(LIT(4*8),RSP)                      /* make room for spill area and call */
	CALL_REG(QWORD(RAX,CTX_handler))

	/* pass return type via registers, handle ints and floats */
	
	MOV(QWORD(RBP,FRAME_DCValue_win64),RAX)
	MOVD(RAX,XMM0)

	MOV(RBP,RSP)
	POP(RBP)
	RET()

END_PROC(dcCallback_x64_win64)

END_ASM

