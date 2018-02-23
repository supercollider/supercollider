/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_sparc64.c
 Description: Thunk - Implementation for sparc64
 License:

   Copyright (c) 2007-2017 Daniel Adler <dadler@uni-goettingen.de>,
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
   8:   83 28 70 20   sllx   %g1, 0x20, %g1
  12:   21 00 00 00   sethi  %hi(0), %l0
  16:   a0 14 20 00   or     %l0, 0, %l0
  20:   82 04 00 01   add    %l0, %g1, %g1
                      ; use g2 for entry pointer and "call" it, b/c branch instructions can't handle 64bit addresses
  24:   05 00 00 00   sethi  %hi(0), %g2
  28:   84 10 a0 00   or     %g2, 0, %g2
  32:   85 28 b0 20   sllx   %g2, 0x20, %g2
  36:   21 00 00 00   sethi  %hi(0), %l0
  40:   a0 14 20 00   or     %l0, 0, %l0
  44:   84 04 00 02   add    %l0, %g2, %g2
                      ; jump - write link to %g0, effectively discarding it; also nop for delay slot
  48:   81 c0 80 00   jmpl   %g2, %g0
  52:   01 00 00 00   nop
  */

  union {
    unsigned int x[2];
    void* p;
  } t, e;
  t.p = p;
  e.p = entry;

  p->code[ 0] = 0x03000000 | (t.x[0] >> 10);      /* sethi %hi(p), %g1      -- hi 22 bits */
  p->code[ 1] = 0x82106000 | (t.x[0] & 0x3ff);    /* or %g1, <p&0x3ff>, %g1 -- lo 10 bits */
  p->code[ 2] = 0x83287020;                       /* sllx %g1, 0x20, %g1                  */
  p->code[ 3] = 0x21000000 | (t.x[1] >> 10);      /* sethi %hi(p), %l0                    */
  p->code[ 4] = 0xa0142000 | (t.x[1] & 0x3ff);    /* or %l0, <p&0x3ff>, %l0               */
  p->code[ 5] = 0x82040001;                       /* add %l0, %g1, %g1                    */
  p->code[ 6] = 0x05000000 | (e.x[0] >> 10);      /* sethi %hi(entry), %g2                */
  p->code[ 7] = 0x8410a000 | (e.x[0] & 0x3ff);    /* or %g2, <entry&0x3ff>, %g2           */
  p->code[ 8] = 0x8528b020;                       /* sllx %g2, 0x20, %g2                  */
  p->code[ 9] = 0x21000000 | (e.x[1] >> 10);      /* sethi %hi(entry), %l0                */
  p->code[10] = 0xa0142000 | (e.x[1] & 0x3ff);    /* or %l0, <entry&0x3ff>, %l0           */
  p->code[11] = 0x84040002;                       /* add %l0, %g2, %g2                    */
  p->code[12] = 0x81c08000;                       /* jmpl %g2, %g0  -- discards link addr */
  p->code[13] = 0x01000000;                       /* nop                                  */
}

