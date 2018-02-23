/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_arm64.c
 Description: Thunk - Implementation for ARM64 / ARMv8 / AAPCS64
 License:

   Copyright (c) 2015 Daniel Adler <dadler@uni-goettingen.de>,
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



#include "dyncall_thunk.h"

/*

Thunk Register: x9

Thunk:
	adr x9,  Thunk
	ldr x10, .target
	br  x10
	nop
.target:
	.xword 0

-- Encoded in:

0000000000000000 <Thunk>:
   0:	10000009 	adr	x9, 0 <Thunk>
   4:	5800006a 	ldr	x10, 10 <.target>
   8:	d61f0140 	br	x10
   c:	d503201f 	nop

0000000000000010 <.target>:
  10:	76543210 	.word	0x76543210
  14:	fedcba98 	.word	0xfedcba98
*/

void dcbInitThunk(DCThunk* p, void (*entry)())
{
  p->code[0] = 0x10000009; //   adr x9, 0
  p->code[1] = 0x5800006a; //	ldr x9, entry
  p->code[2] = 0xd61f0140; //   br  x9
  p->code[3] = 0xd503201f; //   nop
  p->entry   = entry;      // entry: 
                           //   .xword 0
}

