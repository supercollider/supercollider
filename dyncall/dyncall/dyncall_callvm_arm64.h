/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_arm64.h
 Description: 
 License:

   Copyright (c) 2015 Daniel Adler <dadler@uni-goettingen.de>, 
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

#ifndef DYNCALL_CALLVM_ARM64_H
#define DYNCALL_CALLVM_ARM64_H

#include "dyncall_call_arm64.h"
#include "dyncall_callvm.h"
#include "dyncall_vector.h"

typedef struct
{
  DCCallVM mInterface;
  unsigned int i;	/* int register counter */
  unsigned int f;	/* float register counter */
  union {		/* float register buffer */
    DCfloat  S[16];
    DCdouble D[8];
  } u;
  unsigned long long I[8]; /* int register buffer */
  DCVecHead mVecHead;	/* argument buffer head */
} DCCallVM_arm64;

#endif /* DYNCALL_CALLVM_ARM64_DEBIAN_H */

