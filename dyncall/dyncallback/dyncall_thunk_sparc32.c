/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_sparc32.c
 Description: Thunk - Implementation for sparc32
 License:

   Copyright (c) 2007-2016 Daniel Adler <dadler@uni-goettingen.de>,
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

void dcbInitThunk(DCThunk* p, void (*entry)())
{
  /* example
                      ; put thunk pointer in g1
   0:   03 00 00 00   sethi  %hi(0), %g1
   4:   82 10 60 00   or     %g1, 0, %g1
                      ; use g2 for entry pointer and "call" it, b/c branch instructions can't handle 32bit addresses
   8:   05 00 00 00   sethi  %hi(0), %g2
  12:   84 10 a0 00   or     %g2, 0, %g2
                      ; jump - write link to %g0, effectively discarding it; also nop for delay slot
  16:   81 c0 80 00   jmpl   %g2, %g0
  20:   01 00 00 00   nop
  */

  p->code[0] = 0x03000000 | ((unsigned int)p >> 10);       /* sethi %hi(p), %g1      -- hi 22 bits */
  p->code[1] = 0x82106000 | ((unsigned int)p & 0x3ff);     /* or %g1, <p&0x3ff>, %g1 -- lo 10 bits */
  p->code[2] = 0x05000000 | ((unsigned int)entry >> 10);   /* sethi %hi(entry), %g2                */
  p->code[3] = 0x8410a000 | ((unsigned int)entry & 0x3ff); /* or %g2, <entry&0x3ff>, %g2           */
  p->code[4] = 0x81c08000;                                 /* jmpl %g2, %g0  -- discards link addr */
  p->code[5] = 0x01000000;                                 /* nop                                  */
}

