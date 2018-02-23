/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_sparc64.h
 Description: Call VM for sparc64 (v9) ABI.
 License:

   Copyright (c) 2011-2015 Daniel Adler <dadler@uni-goettingen.de>

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



#ifndef DYNCALL_CALLVM_SPARC_V9_H
#define DYNCALL_CALLVM_SPARC_V9_H

#include "dyncall_callvm.h"
#include "dyncall_vector.h"

typedef struct 
{
  DCCallVM     mInterface;      /*   0: +12:8 = 16*/
  DCVecHead    mVecHead;        /* 16   152: mTotalSize +8 */
                                /* 24   160: mSize      +8 */
                                /* 32  168: mData  */ 
} DCCallVM_v9;

#endif /* DYNCALL_CALLVM_SPARC_V9_H */

