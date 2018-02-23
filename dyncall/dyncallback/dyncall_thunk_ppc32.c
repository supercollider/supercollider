/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_ppc32.c
 Description: Thunk - Implementation for ppc32 darwin/apple
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


#include "dyncall_thunk.h"

static unsigned short hi16(x) { return ( (unsigned short) (((unsigned int)x)>>16UL) ); }
static unsigned short lo16(x) { return ( (unsigned short)  ((unsigned int)x)        ); }   

void dcbInitThunk(DCThunk* p, void (*entry)())
{
  /*
    ppc32 thunk code:
      lis   r2 , HI16(p)
      ori   r2 , r2, LO16(p)
      lwz   r12, 20(r2)
      mtctr r12
      bctr
  */

  p->code_load_hi = 0x3c40U;     /* lis  r2, HI16(p) */
  p->addr_self_hi = hi16(p);
  p->code_load_lo = 0x6042U;     /* ori  r2, r2, LO16(p) */
  p->addr_self_lo = lo16(p);
  p->code_jump[0] = 0x81820014U; /* lwz   r12, 20(r2) */
  p->code_jump[1] = 0x7d8903a6U; /* mtclr r12 */
  p->code_jump[2] = 0x4e800420U; /* bctr */
  p->addr_entry   = entry;
}

