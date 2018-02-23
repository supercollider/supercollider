/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_mips64.c
 Description: Thunk - Implementation for MIPS64
 License:

   Copyright (c) 2016 Tassilo Philipp <tphilipp@potion-studios.com>

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

#define b_48_63(x) ((unsigned short)(((unsigned long long)x)>>48UL))
#define b_32_47(x) ((unsigned short)(((unsigned long long)x)>>32UL))
#define b_16_31(x) ((unsigned short)(((unsigned long long)x)>>16UL))
#define b_00_15(x) ((unsigned short) ((unsigned long long)x)       )

void dcbInitThunk(DCThunk* p, void (*entry)())
{
/*

Thunk Register: $t8 ($24)
Call Address (sticking to t9 as for PIC calls on mips32): $t9 ($25)

mips64 thunk code:
        lui  $t8, p[48:63]
        ori  $t8, $t8, p[32:47]
        dsll $t8, 16
        ori  $t8, $t8, p[16:31]
        dsll $t8, 16
        lui  $t9, entry[48:63]
        ori  $t9, $t9, entry[32:47]
        dsll $t9, 16
        ori  $t9, $t9, entry[16:31]
        dsll $t9, 16
        ori  $t9, $t9, entry[0:15]
        jr   $t9
        ori  $t8, $t8, p[0:15]  ; branch delay slot

Disassembly of section .text:

0000000000000000 <thunk>:
   0:   3c180000        lui     t8,0x0
   4:   37180000        ori     t8,t8,0x0
   8:   0018c438        dsll    t8,t8,0x10
   c:   37180000        ori     t8,t8,0x0
  10:   0018c438        dsll    t8,t8,0x10
  14:   3c190000        lui     t9,0x0
  18:   37390000        ori     t9,t9,0x0
  1c:   0019cc38        dsll    t9,t9,0x10
  20:   37390000        ori     t9,t9,0x0
  24:   0019cc38        dsll    t9,t9,0x10
  28:   37390000        ori     t9,t9,0x0
  2c:   03200008        jr      t9
  30:   37180000        ori     t8,t8,0x0

*/

#if defined(DC__Endian_BIG)

  p->text.s[ 0] = 0x3c18; p->text.s[ 1] = b_48_63(p);     /* lui $t8, p[48:63] */
  p->text.s[ 2] = 0x3718; p->text.s[ 3] = b_32_47(p);     /* ori $t8, $t8, p[32:47] */
  p->text.s[ 6] = 0x3718; p->text.s[ 7] = b_16_31(p);     /* ori $t8, $t8, p[16:31] */
  p->text.s[10] = 0x3c19; p->text.s[11] = b_48_63(entry); /* lui $t9, entry[48:63] */
  p->text.s[12] = 0x3739; p->text.s[13] = b_32_47(entry); /* ori $t9, $t9, entry[32:47] */
  p->text.s[16] = 0x3739; p->text.s[17] = b_16_31(entry); /* ori $t9, $t9, entry[16:31] */
  p->text.s[20] = 0x3739; p->text.s[21] = b_00_15(entry); /* ori $t9, $t9, entry[0:15] */
  p->text.s[24] = 0x3718; p->text.s[25] = b_00_15(p);     /* ori $t8, $t8, p[0:15] - branch delay slot */

#else /* defined(DC__Endian_LITTLE) */

  p->text.s[ 1] = 0x3c18; p->text.s[ 0] = b_48_63(p);     /* lui $t8, p[48:63] */
  p->text.s[ 3] = 0x3718; p->text.s[ 2] = b_32_47(p);     /* ori $t8, $t8, p[32:47] */
  p->text.s[ 7] = 0x3718; p->text.s[ 6] = b_16_31(p);     /* ori $t8, $t8, p[16:31] */
  p->text.s[11] = 0x3c19; p->text.s[10] = b_48_63(entry); /* lui $t9, entry[48:63] */
  p->text.s[13] = 0x3739; p->text.s[12] = b_32_47(entry); /* ori $t9, $t9, entry[32:47] */
  p->text.s[17] = 0x3739; p->text.s[16] = b_16_31(entry); /* ori $t9, $t9, entry[16:31] */
  p->text.s[21] = 0x3739; p->text.s[20] = b_00_15(entry); /* ori $t9, $t9, entry[0:15] */
  p->text.s[25] = 0x3718; p->text.s[24] = b_00_15(p);     /* ori $t8, $t8, p[0:15] - branch delay slot */

#endif

  p->text.i[ 2] = 0x0018c438; /* dsll t8,t8,0x10 */
  p->text.i[ 4] = 0x0018c438; /* dsll t8,t8,0x10 */
  p->text.i[ 7] = 0x0019cc38; /* dsll t9,t9,0x10 */
  p->text.i[ 9] = 0x0019cc38; /* dsll t9,t9,0x10 */
  p->text.i[11] = 0x03200008; /* jr $t9 */
}

