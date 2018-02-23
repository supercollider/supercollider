/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_sparc.h
 Description: Call VM for sparc processor architecture.
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



#ifndef DYNCALL_CALLVM_SPARC_H
#define DYNCALL_CALLVM_SPARC_H

#include "dyncall_callvm.h"
#include "dyncall_vector.h"

/* NOTE: if something changes here, update offset marks in dyncall_call_sparc.S */

typedef struct DCCallVM_sparc_ DCCallVM_sparc;
struct DCCallVM_sparc_
{
  DCCallVM  mInterface;	/* 8 bytes (vtable,errorid) */
  DCVecHead mVecHead;	/* 8 bytes (max,size)       */
};

#endif /* DYNCALL_CALLVM_SPARC_H */

