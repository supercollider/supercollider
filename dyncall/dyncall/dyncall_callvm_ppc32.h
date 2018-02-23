/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_ppc32.h
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



#ifndef DYNCALL_CALLVM_PPC32_H
#define DYNCALL_CALLVM_PPC32_H

/*

  dyncall callvm for 32bit ppc architectures

  SUPPORTED CALLING CONVENTIONS
  standard and ... (ellipsis) calls

  REVISION
  2007/12/11 initial

*/


#include "dyncall_call_ppc32.h"
#include "dyncall_callvm.h"
#include "dyncall_vector.h"

typedef struct DCCallVM_ppc32_ DCCallVM_ppc32;

struct DCCallVM_ppc32_
{
  DCCallVM  mInterface;
  int       mIntRegs;
  int       mFloatRegs;
  struct DCRegData_ppc32_ mRegData;
  DCVecHead mVecHead;
};

#endif /* DYNCALL_CALLVM_PPC32_H */

