;//////////////////////////////////////////////////////////////////////////////
;
; Copyright (c) 2007,2009 Daniel Adler <dadler@uni-goettingen.de>, 
;                         Tassilo Philipp <tphilipp@potion-studios.com>
;
; Permission to use, copy, modify, and distribute this software for any
; purpose with or without fee is hereby granted, provided that the above
; copyright notice and this permission notice appear in all copies.
;
; THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
; WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
; MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
; ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
; WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
; ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
; OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
;
;//////////////////////////////////////////////////////////////////////////////

;///////////////////////////////////////////////////////////////////////
;
;	dyncall_call_x86_nasm.nasm
;
;	X86 Calls for nasm assembler
;
;///////////////////////////////////////////////////////////////////////


    BITS 32
    section .text

;///////////////////////////////////////////////////////////////////////
; CSYM macro
;///////////////////////////////////////////////////////////////////////

%ifdef BUILD_OS_windows

%macro EXPORT_C 1
  global _%1
_%1:
%endmacro

%else

%macro EXPORT_C 1
  global %1
%1:
%endmacro

%endif

; -----------------------------------------------------------------------------
; Calling Convention x86 standard C
; - all arguments are on the stack
; - caller cleans up stack
;
; C proto 
;   dcCallC(DCptr funptr, DCptr args, DCsize size)
; -----------------------------------------------------------------------------

EXPORT_C dcCall_x86_cdecl

	push ebp              ; prolog 
	mov  ebp, esp         

	; arguments:
	;
	; funptr  ebp+8
	; args    ebp+12
	; size    ebp+16
	; result  ebp+20

	push esi              ; save esi, edi
	push edi

	mov  esi, [ebp+12]    ; esi = pointer on args
	mov  ecx, [ebp+16]    ; ecx = size

	sub  esp, ecx         ; cdecl call: allocate 'size' bytes on stack
	mov  edi, esp         ; edi = stack args

	rep movsb             ; copy arguments

	call [ebp+8]          ; call function

	add  esp, [ebp+16]    ; cdecl call: cleanup stack

	pop  edi              ; restore edi, esi
	pop  esi

	mov  esp, ebp         ; epilog
	pop  ebp

	ret

; -----------------------------------------------------------------------------
; Calling Convention x86 microsoft thiscall
; - thispointer is in ECX, rest is on the stack
; - callee cleans up stack
;  
; C proto
;   dcCallThisMS(DCptr funptr, DCptr args, DCsize size)
; -----------------------------------------------------------------------------
EXPORT_C dcCall_x86_win32_msthis
    
	push ebp              ; prolog 
	mov  ebp, esp         

	; arguments:
	;
	; funptr  ebp+8
	; args    ebp+12
	; size    ebp+16

	push esi              ; save esi, edi
	push edi

	mov  esi, [ebp+12]    ; esi = pointer on args
	mov  ecx, [ebp+16]    ; ecx = size

	mov  eax, [esi]       ; eax = this pointer
	add  esi, 4           ; increment args pointer by thisptr
	sub  ecx, 4           ; decrement size by sizeof(thisptr)

	sub  esp, ecx         ; allocate argument-block on stack
	mov  edi, esp         ; edi = stack args

	rep movsb             ; copy arguments

	mov  ecx, eax         ; ecx = this pointer

	call [ebp+8]          ; call function (thiscall: cleanup by callee)

	pop  edi              ; restore edi, esi
	pop  esi

	mov  esp, ebp         ; epilog
	pop  ebp

	ret    

; -----------------------------------------------------------------------------
; Calling Convention x86 win32 stdcall
; - all arguments are passed by stack
; - callee cleans up stack
; 
; C proto
;   dcCallStd(DCptr funptr, DCptr args, DCsize size)
; -----------------------------------------------------------------------------
EXPORT_C dcCall_x86_win32_std

	push ebp              ; prolog 
	mov  ebp, esp         

	; arguments:
	;
	; funptr  ebp+8
	; args    ebp+12
	; size    ebp+16

	push esi              ; save esi, edi
	push edi

	mov  esi, [ebp+12]    ; esi = pointer on args
	mov  ecx, [ebp+16]    ; ecx = size

	sub  esp, ecx         ; stdcall: allocate 'size'-8 bytes on stack
	mov  edi, esp         ; edi = stack args

	rep movsb             ; copy arguments

	call [ebp+8]          ; call function (stdcall: cleanup by callee)

	pop  edi              ; restore edi, esi
	pop  esi

	mov  esp, ebp         ; epilog
	pop  ebp

	ret

; -----------------------------------------------------------------------------
; Calling Convention x86 win32 fastcall
; - first two integer (up to 32bits) are passed in ECX and EDX
; - others are passed on the stack
; - callee cleans up stack
; 
; C proto
;   dcCallFast(DCptr funptr, DCptr args, DCsize size)
; -----------------------------------------------------------------------------
EXPORT_C dcCall_x86_win32_fast

	push ebp              ; prolog 
	mov  ebp, esp

	; arguments:
	;
	; funptr  ebp+8
	; args    ebp+12
	; size    ebp+16

	push esi              ; save esi, edi
	push edi

	mov  esi, [ebp+12]    ; esi = pointer on args
	mov  ecx, [ebp+16]    ; ecx = size
	mov  eax, [esi]       ; eax = first argument
	mov  edx, [esi+4]     ; edx = second argument
	add  esi, 8           ; increment source pointer
	sub  ecx, 8           ; decrement size by 8

	sub  esp, ecx         ; fastcall: allocate 'size'-8 bytes on stack
	mov  edi, esp         ; edi = stack args

	rep movsb             ; copy arguments

	mov  ecx, eax         ; ecx = first argument

	call [ebp+8]          ; call function (fastcall: cleanup by callee)

	pop  edi              ; restore edi, esi
	pop  esi

	mov  esp, ebp         ; epilog
	pop  ebp

	ret    

;; Stack markings for ELF/GNU to specify no executable stack */
;
;%ifidn __OUTPUT_FORMAT__,elf
;section .note.GNU-stack noalloc noexec nowrite progbits
;%endif

