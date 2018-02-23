/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_x86.S
 Description: All - except Plan9 - x86 abi call kernel implementation
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



#include "../portasm/portasm-x86.S"
BEGIN_ASM
/* ============================================================================
   DynCall Call Kernels for X86 Architecture
   ----------------------------------------------------------------------------
   C Interface:
      dcCall_x86_XXX(void* target, void* args, size_t size);
      ddCall_x86_sys_XXX(int_ptr target, void* args, size_t size);
 
   Where XXX is one of calling-conventions,
      cdecl, win32_msthis, win32_std, win32_fastcall.

   Parameter Stack layout:
      size    :=  EBP + 16
      args    :=  EBP + 12
      target  :=  EBP +  8
  
   NOTES:
    - epilog restore ESP serves callee cleanup 
    - 16 byte alignment (to be compatible with darwin).
*/


/*--- default / cdecl --------------------------------------------------------

  Details:
  - caller clean-up

*/

GLOBAL(dcCall_x86_cdecl)
BEGIN_PROC(dcCall_x86_cdecl)
	PUSH(EBP)			/* prolog. */
	MOVL(ESP,EBP)
	PUSH(ESI)			/* save preserved registers. */
	PUSH(EDI)
	MOVL(DWORD(EBP,12),ESI)		/* ESI = arg buffer ptr */
	MOVL(DWORD(EBP,16),ECX)		/* ECX = arg buffer size */
	ADDL(LIT(15),ECX)		/* ECX = align(ECX,16) */
	ANDL(LIT(-16),ECX)
	MOVL(ECX,DWORD(EBP,16))		/* save ECX. */
	SUBL(ECX,ESP)			/* allocate stack size */
	MOVL(ESP,EDI)			/* EDI = stack ptr */

	/* 
		work around for rep movsd (not supported by SunPro) 

	SHRL(LIT(2),ECX)
	REP(MOVSD)

	*/

	REP(MOVSB)
	CALL_DWORD(EBP,8)
	ADDL(DWORD(EBP,16),ESP)
	POP(EDI)
	POP(ESI)
	MOVL(EBP,ESP)
	POP(EBP)
	RET()
END_PROC(dcCall_x86_cdecl)
/* ---- C++ this calls (microsoft) ------------------------------------------ 

  Details:
  - this pointer is in ECX

*/

GLOBAL(dcCall_x86_win32_msthis)
BEGIN_PROC(dcCall_x86_win32_msthis)
	PUSH(EBP)               	/* prolog. */
	MOVL(ESP,EBP)
	PUSH(ESI)               	/* save preserved. */
	PUSH(EDI)
	MOVL(DWORD(EBP,12),ESI)		/* ESI = pointer on args. */
	MOVL(DWORD(EBP,16),ECX)		/* ECX = size. */
	MOVL(DWORD(ESI,0),EAX) 		/* EAX = this pointer. */ 
	ADDL(LIT(4),ESI)           		/* increment args pointer by thisptr. */
	SUBL(LIT(4),ECX)        		/* decrement size by sizeof(thisptr). */
	SUBL(ECX,ESP)         		/* allocate argument-block on stack. */
	MOVL(ESP,EDI)       		/* EDI = stack args. */
	REP(MOVSB)              	/* copy arguments. */
	MOVL(EAX,ECX)        		/* ECX = this pointer. */
	CALL_DWORD(EBP,8)		/* call function. */
	POP(EDI)			/* restore preserved. */
	POP(ESI)
	MOVL(EBP,ESP)       		/* epilog. */
	POP(EBP)
	RET()
END_PROC(dcCall_x86_win32_msthis)

/*---- win32 stdcall ---------------------------------------------------------
  
  Details:
  - callee cleans up stack

*/

GLOBAL(dcCall_x86_win32_std)
BEGIN_PROC(dcCall_x86_win32_std)
	PUSH(EBP)			/* prolog. */
	MOVL(ESP,EBP)
	PUSH(ESI)			/* save ESI, EDI. */
	PUSH(EDI)
	MOVL(DWORD(EBP,12),ESI)		/* ESI = args. */
	MOVL(DWORD(EBP,16),ECX)		/* ECX = size. */
	SUBL(ECX,ESP)			/* allocate size bytes on stack. */
	MOVL(ESP,EDI)			/* EDI = copy destination stack. */
	REP(MOVSB)			/* copy BYTEs. */
	CALL_DWORD(EBP,8)		/* call target. */
	POP(EDI)			/* restore EDI, ESI. */
	POP(ESI)
	MOVL(EBP,ESP)			/* epilog. */
	POP(EBP)
	RET()
END_PROC(dcCall_x86_win32_std)

/*---- win32 fastcall (GNU/Microsoft) ----------------------------------------

  Details:
  - callee cleans up stack
  - first two integer (up to 32bits) are passed in ECX and EDX

*/

GLOBAL(dcCall_x86_win32_fast)
BEGIN_PROC(dcCall_x86_win32_fast)
	PUSH(EBP)			/* prolog. */
	MOVL(ESP,EBP)
	PUSH(ESI)			/* save preserved. */
	PUSH(EDI)
	MOVL(DWORD(EBP,12),ESI)		/* ESI = copy source args. */
	MOVL(DWORD(EBP,16),ECX)		/* ECX = size. */
	MOVL(DWORD(ESI,0),EAX)		/* EAX = first argument. */
	MOVL(DWORD(ESI,4),EDX)		/* EDX = second argument. */
	ADDL(LIT(8),ESI)		/* skip registers. */
	SUBL(LIT(8),ECX)
	MOVL(ECX,DWORD(EBP,16))		/* save stack alloc size. */
	SUBL(ECX,ESP)  			/* allocate stack. */
	MOVL(ESP,EDI)  			/* EDI = stack args. */ 
	REP(MOVSB) 			/* copy BYTEs. */
	MOVL(EAX,ECX)			/* ECX = first argument. */ 
	CALL_DWORD(EBP,8)		/* call target. */
	POP(EDI)			/* restore preserved. */
	POP(ESI)
	MOVL(EBP,ESP)			/* epilog. */
	POP(EBP)
	RET()
END_PROC(dcCall_x86_win32_fast)

/*--- syscall int80 linux ---------------------------------------------------
  
  Details:
  - all arguments are passed via registers

*/

GLOBAL(dcCall_x86_sys_int80h_linux)
BEGIN_PROC(dcCall_x86_sys_int80h_linux)
	PUSH(EBP)        		/* prolog. */
	MOVL(ESP,EBP)
	PUSH(EBX)			/* save preserved. */
	PUSH(ESI)               
	PUSH(EDI)
	MOVL(DWORD(EBP,12),EAX)		/* EAX = argument buffer. */
	MOVL(DWORD(EAX,0),EBX)		/* move first five arguments. */
	MOVL(DWORD(EAX,4),ECX)	
	MOVL(DWORD(EAX,8),EDX)
	MOVL(DWORD(EAX,12),ESI)
	MOVL(DWORD(EAX,16),EDI)
	MOVL(DWORD(EBP,8),EAX)		/* EAX = syscall id. */
	INT(LIT(HEX(80)))
	POP(EDI)			/* restore preserved. */
	POP(ESI)
	POP(EBX)
	MOVL(EBP,ESP)			/* epilog. */
	POP(EBP)
	RET()
END_PROC(dcCall_x86_sys_int80h_linux)

/*--- syscall int80 bsd -----------------------------------------------------

  Details:
  - all arguments are passed via stack

*/
	
GLOBAL(dcCall_x86_sys_int80h_bsd)
BEGIN_PROC(dcCall_x86_sys_int80h_bsd)
	PUSH(EBP)			/* prolog. */
	MOVL(ESP,EBP)
	PUSH(ESI)			/* save preserved. */
	PUSH(EDI)
	MOVL(DWORD(EBP,12),ESI)    	/* ESI = pointer on args. */
	MOVL(DWORD(EBP,16),ECX)   	/* ECX = size. */
	SUBL(ECX,ESP)       		/* allocate stack space. */
	MOVL(ESP,EDI)           		/* EDI = stack args. */
	REP(MOVSB)
	MOVL(DWORD(EBP,8),EAX)		/* load system call id. */
	CALL(_do_int)
	POP(EDI)               		/* restore preserved. */
	POP(ESI)
	MOVL(EBP,ESP)          		/* epilog. */
	POP(EBP)
	RET()
_do_int:
	INT(LIT(HEX(80)))
	RET()
END_PROC(dcCall_x86_sys_int80h_bsd)

END_ASM

