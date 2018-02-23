/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_arm32.c
 Description: Thunk - Implementation for ARM32 (ARM and THUMB mode)
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


/* Since we can mix ARM and THUMB assembly, this works for both modes */

#include "dyncall_thunk.h"

void dcbInitThunk(DCThunk* p, void (*entry)())
{
  /*
    # ARM32 (ARM mode) thunk code:
    .code 32
      sub %r12, %r15, #8
      ldr %r15, [%r15, #-4]
  */

  /* This code stores a ptr to DCCallback in r12 (equals ptr to thunk,     */
  /* which is PC (r15) minus 8, as PC points to current instruction+8.     */
  /* Then it loads the callback 'entry' into PC. The -4 is needed, also bc */
  /* of the PC pointing ahead.                                             */
  p->code[0]  = 0xe24fc008UL;  /* sub %r12, %r15, #8 */
  p->code[1]  = 0xe51ff004UL;  /* ldr %r15, [%r15, #-4] */
  p->entry = entry;
}
