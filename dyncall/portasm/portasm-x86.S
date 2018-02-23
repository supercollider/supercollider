/*

 Package: dyncall
 Library: portasm
 File: portasm/portasm-x86.S
 Description: Portable Assembler Macros for x86
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



/* Common macros. */
#define XCONCAT(A,B) A##B
/* MASM syntax. */
#if defined(GEN_MASM)
.386
.MODEL FLAT
.CODE
#  define BEGIN_ASM
#  define END_ASM END
#  define GLOBAL(X) _##X PROC
#  define BEGIN_PROC(X) 	OPTION PROLOGUE:NONE, EPILOGUE:NONE
#  define END_PROC(X)   _##X ENDP
#  define PUSH(R) push R
#  define POP(R) pop R
#  define MOVL(S,D) mov D,S
#  define ADDL(S,D) add D,S
#  define ANDL(S,D) and D,S
#  define SUBL(S,D) sub D,S
#  define SHRL(S,D) shr D,S
#  define RET() ret
#  define CALL_DWORD(R,OFF) call DWORD(R,OFF)
#  define REP(X) rep X
#  define MOVSB movsb
#  define MOVSW movsw
#  define MOVSD movsd
#  define DWORD(R,OFF) dword ptr [R+OFF]
#  define QWORD(R,OFF) qword ptr [R+OFF]
#  define LIT(X) X
#  define INT(X) int X
#  define HEX(X) XCONCAT(X,h)
#  define CALL(X) call X
#  define LEA(S,D) lea D,S
#  define ADD(S,D) add D,S
#  define CMP(S,D) cmp D,S
#  define JE(T) je T
#  define FLDS(OP) fld OP
#  define FLDL(OP) fld OP
#  define LOCAL(NAME) NAME
/* @@@ check if masm support wanted/needed */
#  define SECTION_NOTE_NXSTACK
#else
/* GNU/SunPro Assembler AT&T Syntax */
	.text
#  define BEGIN_ASM
#  define END_ASM 
#  include "../autovar/autovar_ABI.h"
#  if defined(OS_Win32) || defined(OS_Cygwin) || defined(OS_MinGW) || defined(OS_Darwin) || defined(OS_Minix)
#    define CSYM(X) _##X
#  else
#    define CSYM(X) X
#  endif
/* Systems that work without '%' prefix: MinGW,Apple */
#  define EAX %eax
#  define EBX %ebx
#  define ECX %ecx
#  define EDX %edx
#  define ESI %esi
#  define EDI %edi
#  define EBP %ebp
#  define ESP %esp
#  define AL  %al
#  define AH  %ah
#  define BL  %bl
#  define BH  %bh
#  define CL  %cl
#  define CH  %ch
#  define DL  %dl
#  define DH  %dh
#  define GLOBAL(X) .globl CSYM(X)
#  define BEGIN_PROC(X)  CSYM(X):
#  define END_PROC(X)
#  define PUSH(R) pushl R
#  define POP(R) popl R
#  define MOVL(S,D) movl S,D
#  define ADDL(S,D) addl S,D
#  define ANDL(S,D) andl S,D
#  define SUBL(S,D) subl S,D
#  define SHRL(S,D) shrl S,D
#  define RET() ret
#  define CALL_DWORD(R,OFF) call *DWORD(R,OFF)
#  define REP(X) rep; X
#  define MOVSB movsb
#  define MOVSW movsw
#  define MOVSD movsd
#  define DWORD(R,OFF) OFF(R)
#  define QWORD(R,OFF) OFF(R)
#  include "../autovar/autovar_CC.h"
#  if defined CC_SUN
#    define LIT(X) $X
#  else
#    define LIT(X) XCONCAT($,X)
#  endif
#  define INT(X) int X
#  define HEX(X) XCONCAT(0x,X)
#  define CALL(X) call X
#  define LEA(A,B) lea A,B
#  define CMP(A,B) cmp A,B
#  define JE(X)    je X
#  define FLDS(X)  flds X
#  define FLDL(X)  fldl X
#  define ADD(A,B) add A,B
#  define LOCAL(X) .X
#endif

