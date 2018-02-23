/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_ppc64.c
 Description: Thunk - Implementation for ppc64
 License:

   Copyright (c) 2014-2015 Masanori Mitsugi <mitsugi@linux.vnet.ibm.com>

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

#define HIST16(x) (unsigned short)((((unsigned long)(x))>>48UL) & 0xffff)
#define HIER16(x) (unsigned short)((((unsigned long)(x))>>32UL) & 0xffff)
#define HI16(x)   (unsigned short)((((unsigned long)(x))>>16UL) & 0xffff)
#define LO16(x)   (unsigned short)( ((unsigned long)(x))        & 0xffff)

void dcbInitThunk(DCThunk* p, void (*entry)())
{
#if DC__ABI_PPC64_ELF_V != 2
  /*
    ppc64 thunk code:                (v1)
      oris   r11, r2, HI16(p)
      ori    r11,r11, LO16(p)
      ld     r12,48(r11)
      ld     r2,56(r11)
      mtctr r12
      bctr
  */

  p->thunk_entry  = (void *)&(p->code_load_hi);
  p->toc_thunk    = ((long)(p->thunk_entry) & 0xffffffff00000000UL);

  p->code_load_hi = 0x644bU;     /* oris  r11, r2, HI16(p) */
  p->addr_self_hi = HI16(p);
  p->code_load_lo = 0x616bU;     /* ori   r11,r11, LO16(p) */
  p->addr_self_lo = LO16(p);
  p->code_jump[0] = 0xe98b0030U; /* ld    r12,48(r11) */
  p->code_jump[1] = 0xe84b0038U; /* ld    r2,56(r11) */
  p->code_jump[2] = 0x7d8903a6U; /* mtclr r12 */
  p->code_jump[3] = 0x4e800420U; /* bctr */
  p->addr_entry   = (void *)*((long *)entry);
  p->toc_entry    = *((long *)(entry + 8));

#else
  /*
    ppc64 thunk code:                (v2)
      lis    r11, HIST16(p)
      ori    r11,r11, HIER16(p)
      rldicr r11,r11,32,31
      oris   r11,r11, HI16(p)
      ori    r11,r11, LO16(p)
      ld     r12,40(r11)
      mtctr r12
      bctr
  */

  p->code_load_hist = 0x3d60U;     /* lis    r11,HIST16(p) */
  p->addr_self_hist = HIST16(p);
  p->code_load_hier = 0x616bU;     /* ori    r11,r11, HIER16(p) */
  p->addr_self_hier = HIER16(p);
  p->code_rot       = 0x796b07c6U; /* rldicr r11,r11,32,31 */
  p->code_load_hi   = 0x656bU;     /* oris   r11,r11, HI16(p) */
  p->addr_self_hi   = HI16(p);
  p->code_load_lo   = 0x616bU;     /* ori    r11,r11, LO16(p) */
  p->addr_self_lo   = LO16(p);
  p->code_jump[0]   = 0xe98b0028U; /* ld     r12,40(r11) */
  p->code_jump[1]   = 0x7d8903a6U; /* mtclr  r12 */
  p->code_jump[2]   = 0x4e800420U; /* bctr */
  p->addr_entry     = (void *)(entry);
#endif
}

