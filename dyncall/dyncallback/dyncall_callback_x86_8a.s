/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callback_x86_8a.s
 Description: x86 abi callback kernel implementation for Plan9's 8a
 License:

   Copyright (c) 2013 Tassilo Philipp <tphilipp@potion-studios.com>

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


TEXT dcCallbackThunkEntry(SB), $0

	/* input:
		AX    -> thunk
		AX+16 -> cb handler
		AX+20 -> dcargsvt
		AX+24 -> stack cleanup <-- not used for stack cleaning as caller cleans up,
		AX+28 -> userdata          however reused as flag to indicate 64bit return value)
	*/

	/* prolog */
	MOVL SP, BP

	/* copy of DCargs passed to cb handler */
	PUSHL $0           /* fast_count (unused on plan9, but using shared x86 dcargs struct) */
	SUBL  $8, SP       /* skip fast_data[] ( " ) */
	LEAL  8(BP), CX    /* ptr to args on stack, depending if return val is 64bit ... */
	CMPL  24(AX), $1
	JEQ   is_ll_ret
	LEAL  4(BP), CX    /* ... or not */
is_ll_ret:
	PUSHL CX
	PUSHL 20(AX)       /* args vtable ptr */
	MOVL  SP, CX       /* DCArgs-ptr (data pushed above) */

	/* space for return value (long long) */
	PUSHL $0
	PUSHL $0
	MOVL  SP, DX       /* retval ptr */

	/* call the handler */
	PUSHL 28(AX)       /* userdata for handler */
	PUSHL DX           /* results */
	PUSHL CX           /* args */
	PUSHL AX           /* callback obj */
	MOVL  16(AX), AX
	CALL  AX

	/* copy retval from ptr on stack to AX or stack space if 64bit */
	MOVL 8(SP), BX     /* ptr to retval */
	CMPL  AX, $0x6c    /* 'l' @@@ compares return from handler, might be different from sig - design currently in discussion */
	JEQ   ll_ret;
	CMPL  AX, $0x4c    /* 'L' @@@ compares return from handler, might be different from sig - design currently in discussion */
	JEQ   ll_ret;
	CMPL  AX, $0x66    /* 'f' @@@ compares return from handler, might be different from sig - design currently in discussion */
	JEQ   f_ret;
	CMPL  AX, $0x64    /* 'd' @@@ compares return from handler, might be different from sig - design currently in discussion */
	JEQ   d_ret;
	JMP   other_ret

ll_ret:
	MOVL  48(SP), DX    /* ptr to ret address space; 48 = stack size + caller's ret address */
	MOVL  (BX), CX
	MOVL  CX, (DX)
	MOVL  4(BX), CX
	MOVL  CX, 4(DX)
	JMP   cont_ret

f_ret:
	FMOVF (BX), F0
	JMP   cont_ret

d_ret:
	FMOVD (BX), F0
	JMP   cont_ret

other_ret:
	MOVL (BX), AX      /* 32bit non-fp are returned in AX */

	/* epilog */
cont_ret:
	ADDL $44, SP       /* Cleanup stack */
	POPL CX            /* hack to emulate RET without getting overly strict */
	JMP CX             /* 'unbalanced PUSH/POP' warning/error from 8l */

