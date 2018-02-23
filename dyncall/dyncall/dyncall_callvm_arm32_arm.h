/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_arm32_arm.h
 Description: 
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



/*

  dyncall callvm for 32bit ARM32 family of processors

  SUPPORTED CALLING CONVENTIONS
  armcall

  REVISION
  2007/12/11 initial

*/


#ifndef DYNCALL_CALLVM_ARM32_ARM_H
#define DYNCALL_CALLVM_ARM32_ARM_H

#include "dyncall_call_arm32_arm.h"
#include "dyncall_callvm.h"
#include "dyncall_vector.h"

typedef struct
{
  DCCallVM  mInterface;
  DCpointer mpCallFunc;
  DCVecHead mVecHead;
} DCCallVM_arm32_arm;

#endif /* DYNCALL_CALLVM_ARM32_ARM_H */

