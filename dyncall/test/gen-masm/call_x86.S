/*

 Package: dyncall
 Library: test
 File: test/gen-masm/call_x86.S
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
.386
.MODEL FLAT
.CODE
#  define BEGIN_ASM
#  define END_ASM END
#  define GLOBAL(X) _##X PROC
#  define BEGIN_PROC(X) 	OPTION PROLOGUE:NONE, EPILOGUE:NONE
#  define END_PROC(X)   _##X ENDP
#  define HEX(X) X##h
#else
	.intel_syntax
	.text
#  define BEGIN_ASM
#  define END_ASM 
#  include "../../dyncall/dyncall_macros.h"
#  if defined(DC__OS_Win32) || defined(DC__OS_Cygwin) || defined(DC__OS_MinGW) || defined(DC__OS_Darwin)
#    define CSYM(X) _##X
#  else
#    define CSYM(X) X
#  endif
#  define GLOBAL(X) .globl CSYM(X)
#  define BEGIN_PROC(X)  CSYM(X):
#  define END_PROC(X)
#  define HEX(X) 0x##X
#endif

// Package: dyncall
// File: dyncall/dyncall_call_x86_gas.S
// Description: All x86 abi call kernel implementations in GNU Assembler using C Preprocessor
// License:
//
// Copyright (c) 2007-2010 Daniel Adler <dadler@uni-goettingen.de>,
//                         Tassilo Philipp <tphilipp@potion-studios.com>
//
// Permission to use, copy, modify, and distribute this software for any
// purpose with or without fee is hereby granted, provided that the above
// copyright notice and this permission notice appear in all copies.
//
// THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
// WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
// ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
// WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
// ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//

// === end of macros ==========================================================

// ============================================================================
// DynCall Call Kernels for X86 Architecture
// ----------------------------------------------------------------------------
// C Interface:
//    dcCall_x86_XXX(void* target, void* args, size_t size);
//
// Parameter Stack layout:
//    size    :=  ebp + 16
//    args    :=  ebp + 12
//    target  :=  ebp +  8
//
// NOTES:
//  - epilog restore esp serves callee cleanup 

// --- default / cdecl --------------------------------------------------------
//  - caller clean-up
BEGIN_ASM
GLOBAL(dcCall_x86_cdecl)
BEGIN_PROC(dcCall_x86_cdecl)
	push ebp			// prolog
 	mov  ebp, esp
	push esi			// save preserved
	push edi
	mov  esi, dword ptr[ebp+12] 	// esi = copy source args
	mov  ecx, dword ptr[ebp+16] 	// ecx = size
	sub  esp, ecx         	    	// alloc stack size
	mov  edi, esp		    	// edi = copy target stack
	shr  ecx, 2			// ecx = count DWORDs
	rep  movsd			// copy DWORDs
	call dword ptr[ebp+8] 		// call target
	add  esp, dword ptr[ebp+16] 	// cleanup stack
	pop  edi               	    	// restore preserved 
	pop  esi
	mov  esp, ebp			// epilog
	pop  ebp
	ret
END_PROC(dcCall_x86_cdecl)

// ---- C++ this calls (microsoft) ------------------------------------------ 
// Details:
//   - this pointer is in ECX

GLOBAL(dcCall_x86_win32_msthis)
BEGIN_PROC(dcCall_x86_win32_msthis)
	push ebp               	// prolog 
	mov  ebp, esp
	push esi               // save preserved
	push edi
	mov esi, dword ptr [ebp+12] // esi = pointer on args 
	mov ecx, dword ptr [ebp+16] // ecx = size 
	mov eax, dword ptr [esi+0]  // eax = this pointer 
	add esi, 4           // increment args pointer by thisptr 
	sub ecx, 4         // decrement size by sizeof(thisptr) 
	sub esp, ecx         // allocate argument-block on stack 
	mov edi, esp       // edi = stack args
	rep movsb               // copy arguments
	mov ecx, eax       // ecx = this pointer
	call dword ptr[ebp+8]	// call function
	pop  edi		// restore preserved
	pop  esi
	mov esp, ebp        // epilog
	pop  ebp
	ret
END_PROC(dcCall_x86_win32_msthis)

// ---- win32 stdcall ---------------------------------------------------------
// - callee cleans up stack

GLOBAL(dcCall_x86_win32_std)
BEGIN_PROC(dcCall_x86_win32_std)
	push ebp               // prolog
	mov ebp, esp
	push esi               // save esi, edi
	push edi
	mov esi, dword ptr[ebp+12] 	// esi = args
	mov ecx, dword ptr[ebp+16]	// ecx = size
	sub esp, ecx	// allocate size bytes on stack
	mov  edi, esp		// edi = copy destination stack
	rep movsb		// copy BYTEs
	call dword ptr[ebp+8]		// call target
	pop  edi		// restore edi, esi */
	pop  esi
	mov  esp, ebp		// epilog
	pop  ebp
	ret
END_PROC(dcCall_x86_win32_std)

// ---- win32 fastcall (GNU/Microsoft) ----------------------------------------
// - callee cleans up stack
// - first two integer (up to 32bits) are passed in ECX and EDX

GLOBAL(dcCall_x86_win32_fast)
BEGIN_PROC(dcCall_x86_win32_fast)
	push ebp               // prolog
	mov  ebp, esp
	push esi               // save preserved
	push edi
	mov  esi, dword ptr[ebp+12] 	// esi = copy source args
	mov ecx, dword ptr[ebp+16]	// ecx = size
	mov eax, dword ptr[esi+0]  	// eax = first argument
	mov edx, dword ptr[esi+4] // edx = second argument
	add esi, 8           // skip registers
	sub ecx, 8           // 
	mov dword ptr [ebp+16], ecx	// save stack alloc size
	sub esp, ecx        // allocate stack 
	mov edi, esp       // edi = stack args 
	rep movsb               // copy BYTEs
	mov  ecx, eax        // ecx = first argument 
	call dword ptr[ebp+8]          // call target
	pop  edi               // restore preserved
	pop  esi
	mov  esp, ebp        // epilog 
	pop  ebp
	ret
END_PROC(dcCall_x86_win32_fast)

// --- syscall int80 linux ---------------------------------------------------
//  - all arguments are passed via registers

GLOBAL(dcCall_x86_sys_int80h_linux)
BEGIN_PROC(dcCall_x86_sys_int80h_linux)
	push ebp        	// prolog
 	mov  ebp, esp
	push ebx		// save preserved
	push esi               
	push edi
	mov  eax, dword ptr[ebp+12]	// eax = argument buffer 
	mov  ebx, dword ptr[eax+0]	// move first five arguments
	mov  ecx, dword ptr[eax+4] 	
	mov  edx, dword ptr[eax+8] 
	mov  esi, dword ptr[eax+12]
	mov  edi, dword ptr[eax+16]
	mov  eax, dword ptr[ebp+8] 	// eax = syscall id
	int  HEX(80)
	pop  edi               // restore preserved
	pop  esi
	pop  ebx
	mov  esp, ebp        	// epilog
	pop  ebp
	ret
END_PROC(dcCall_x86_sys_int80h_linux)

// --- syscall int80 bsd -----------------------------------------------------
// - all arguments are passed via stack
	
GLOBAL(dcCall_x86_sys_int80h_bsd)
BEGIN_PROC(dcCall_x86_sys_int80h_bsd)
	push ebp               // prolog
 	mov  ebp, esp
	push esi               // save preserved
	push edi
	mov  esi, dword ptr[ebp+12]    	// esi = pointer on args
	mov  ecx, dword ptr[ebp+16]    	// ecx = size
	sub  esp, ecx       	// allocate stack space
	mov  edi, esp           // edi = stack args
	shr ecx, 2             // ecx = number of DWORDs
	rep movsd               // copy DWORDs
	mov  eax, dword ptr[ebp+8]	// load system call id
	call _do_int
	pop  edi               // restore preserved
	pop  esi
	mov  esp, ebp          // epilog
	pop  ebp
	ret
_do_int:
	int  HEX(80)
	ret
END_PROC(dcCall_x86_sys_int80h_bsd)
END_ASM
