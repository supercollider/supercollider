/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_x86.S
 Description: Callback Thunk entry for x86
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


#include "../portasm/portasm-x86.S"
#include "../dyncall/dyncall_macros.h"

BEGIN_ASM
DCThunk_size = 16
DCArgs_size  = 20
DCValue_size =  8

CTX_thunk         =  0
CTX_phandler      = 16
CTX_pargsvt       = 20
CTX_stack_cleanup = 24
CTX_userdata      = 28

frame_arg0    =  8
frame_ret     =  4
frame_parent  =  0
frame_CTX     = -4
frame_DCArgs  = -24
frame_DCValue = -32

#define ASCII_L 76
#define ASCII_l 108
#define ASCII_d 100
#define ASCII_f 102
#define ASCII_i 105
#define ASCII_v 118

GLOBAL(dcCallbackThunkEntry)
BEGIN_PROC(dcCallbackThunkEntry)
	PUSH(EBP)
	MOVL(ESP,EBP)
	/* local variable frame_CTX) */
	PUSH(EAX)				/* EAX = CTX* */
	/* initialize DCArgs */
	PUSH(LIT(0))				/* fast_count */
	PUSH(EDX)				/* fast_data[1] */
	PUSH(ECX)				/* fast_data[0] */
	LEA(DWORD(EBP,frame_arg0),ECX)		/* compute arg stack address */
	PUSH(ECX)				/* stack-ptr */
	PUSH(DWORD(EAX,CTX_pargsvt))		/* vtbl-ptr */
	MOVL(ESP,ECX)				/* ECX = DCArgs* */
	/* initialize DCvalue */
	PUSH(LIT(0))
	PUSH(LIT(0))
	
	MOVL(ESP,EDX)				/* EDX = DCValue* */
	ANDL(LIT(-16),ESP)			/* align stack to 16 bytes. */
	/* call handler(context) */
	PUSH(DWORD(EAX,CTX_userdata))		/* userdata */
	PUSH(EDX)				/* DCValue* */
	PUSH(ECX)				/* DCargs* */
	PUSH(EAX)				/* DCCallback* */
	CALL_DWORD(EAX,CTX_phandler)
	/* cleanup stack */
	MOVL(EBP,ESP)				/* reset esp to frame */
	POP(ECX)				/* skip parent frame */
	POP(ECX)				/* pop return address */
	MOVL(DWORD(EBP,frame_CTX),EDX)	
	ADD(DWORD(EDX,CTX_stack_cleanup),ESP)	/* cleanup stack */
	PUSH(ECX)				/* push back return address */
	LEA(DWORD(EBP,frame_DCValue), EDX)
	MOVL(DWORD(EBP,0), EBP)			/* EBP = parent frame */
	/* handle return value */

#if !defined(DC__OS_Minix)
	CMP(LIT(ASCII_f),AL)
	JE(LOCAL(return_f32))
	CMP(LIT(ASCII_d),AL)
	JE(LOCAL(return_f64))
#endif

	/* All int cases (+ pointer & string cases) fall in the return_i64 case, here */
LOCAL(return_i64):
	MOVL(DWORD(EDX,0),EAX)
	MOVL(DWORD(EDX,4),EDX)
	RET()
LOCAL(return_f32):
	FLDS(DWORD(EDX,0))
	RET()
LOCAL(return_f64):
	FLDL(QWORD(EDX,0))
LOCAL(return_void):
	RET()
END_PROC(dcCallbackThunkEntry)
END_ASM

