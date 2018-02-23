/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_x64.c
 Description: Thunk - Implementation for x64
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

void dcbInitThunk(DCThunk* p, void (*entry)())
{
  /*
    # x64 thunk code:
    .intel_syntax

    thunk:
        lea   rax, (rip)  # copy RIP (=p?) to RAX and use address in
        jmp   [rax+16]    # 'entry' (stored at RIP+16) for jump
        nop
        nop
        nop
    entry:
        .resq 1
   */

  p->code[0] = 0xfffffffff9058d48ULL;
  p->code[1] = 0x9090900000000325ULL;
  p->entry   = entry;
}

