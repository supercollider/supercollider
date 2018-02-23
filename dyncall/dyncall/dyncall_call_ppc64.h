/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_ppc64.h
 Description: 
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


#ifndef DYNCALL_PPC64_H
#define DYNCALL_PPC64_H

#include "dyncall_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DCRegData_ppc64_
{
  DClonglong mIntData[8];
  DCdouble   mFloatData[13];
};

/* 
** PowerPC 64-bit calling convention call
**
** - hybrid return-type call (bool ... pointer)
**
*/

void     dcCall_ppc64(DCpointer target, struct DCRegData_ppc64_* ppc64data, DCsize stksize, DCpointer stkdata);

#ifdef __cplusplus
}
#endif

#endif /* DYNCALL_PPC64_H */

