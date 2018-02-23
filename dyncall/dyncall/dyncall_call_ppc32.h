/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_ppc32.h
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


#ifndef DYNCALL_PPC32_H
#define DYNCALL_PPC32_H

#include "dyncall_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct DCRegData_ppc32_
{
  DCint     mIntData[8];
  DCdouble  mFloatData[13];	/* 13 for darwin, 8 for sysv */
};

/* 
** PowerPC 32-bit calling convention call
**
** - hybrid return-type call (bool ... pointer)
**
*/

/* Darwin ABI */

void     dcCall_ppc32_darwin (DCpointer target, struct DCRegData_ppc32_* ppc32data, DCsize stksize, DCpointer stkdata);

/* System V (Linux) ABI */

void     dcCall_ppc32_sysv (DCpointer target, struct DCRegData_ppc32_* ppc32data, DCsize stksize, DCpointer stkdata);

#ifdef __cplusplus
}
#endif

#endif /* DYNCALL_PPC32_H */

