/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_mips.c
 Description: Thunk - Implementation for MIPS
 License:

   Copyright (c) 2013-2015 Daniel Adler <dadler@uni-goettingen.de>,
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

Thunk Register: $t4 ($12)
According to o32abi: $t9 

'The Linux/MIPS convention is that all PIC calls use t9 to hold the address of
the called function.' [See MIPS Run, p.413]

    mips thunk code:
      lui $t4,      %hi(p)
      lui $t9,      %hi(entry)
      ori $t9, $t9, %lo(entry)
      jr  $t9
      ori $t4, $t4, %lo(p)        ; branch delay slot


00000000 <thunk>:
   0:  3c0c0000  lui  t4,0x0
   4:  3c190000  lui  t9,0x0
   8:  37390000  ori  t9,t9,0x0
   c:  03200008  jr   t9
  10:  00200825  move at,at
  14:  358c0000  ori  t4,t4,0x0
  18:  00200825  move at,at
  1c:  00200825  move at,at

*/

#if defined(DC__Endian_BIG)

  p->data[0] = 0x3c0c; p->data[1] = hi16(p);     /* lui $t4, hi(p) */
  p->data[2] = 0x3c19; p->data[3] = hi16(entry); /* lui $t9, hi(entry) */
  p->data[4] = 0x3739; p->data[5] = lo16(entry); /* ori $t9, $t9, lo(entry) */
  p->jump    = 0x03200008;                       /* jr $t9 */
  p->bddt[0] = 0x358c; p->bddt[1] = lo16(p);     /* ori $t4, $t4, lo(p) - branch delay slot */

#else /* defined(DC__Endian_LITTLE) */
  
  p->data[1] = 0x3c0c; p->data[0] = hi16(p);     /* lui $t4, hi(p) */
  p->data[3] = 0x3c19; p->data[2] = hi16(entry); /* lui $t9, hi(entry) */
  p->data[5] = 0x3739; p->data[4] = lo16(entry); /* ori $t9, $t9, lo(entry) */
  p->jump    = 0x03200008;                       /* jr $t9 */
  p->bddt[1] = 0x358c; p->bddt[0] = lo16(p);     /* ori $t4, $t4, lo(p) - branch delay slot */
  
#endif

}

