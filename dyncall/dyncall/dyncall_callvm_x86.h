/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_x86.h
 Description: Call virtual machine for x86 architecture header
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



#ifndef DYNCALL_CALLVM_X86_H
#define DYNCALL_CALLVM_X86_H

/*

  dyncall callvm for 32bit X86 architectures
  Copyright 2007 Daniel Adler.

  SUPPORTED CALLING CONVENTIONS
  cdecl,stdcall,fastcall,thiscall,msthiscall

  REVISION
  2007/12/10 initial

*/


#include "dyncall_call_x86.h"
#include "dyncall_callvm.h"
#include "dyncall_vector.h"

typedef struct DCCallVM_x86_ DCCallVM_x86;

struct DCCallVM_x86_
{
  DCCallVM  mInterface;
  DCpointer mpCallFunc;
  int       mIntRegs;     /* used by fastcall implementation */
  DCVecHead mVecHead;
};

#endif /* DYNCALL_CALLVM_X86_H */

