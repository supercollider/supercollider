/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_mips_n32.h
 Description: mips64 "n32" ABI call-kernel C interface.
 License:

   Copyright (c) 2007-2016 Daniel Adler <dadler@uni-goettingen.de>, 
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



#ifndef DYNCALL_CALL_MIPS_N32_H
#define DYNCALL_CALL_MIPS_N32_H

#include "dyncall_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
  two register-files for integer (promoted to 64-bit) and float (not promoted!)
  are used.

  arguments are transfered in a free slot on the corresponding register file.
  the other register-file will be skipped by one.

  float arguments are either loaded from single or double -
  a auto-conversion into double and then loaded as double precision
  turned out to fail for several tests.

  therefore a union for storage of float or double is used instead.
  a bitmask (mUseDouble) records which type is used and will be
  interpreted in the call-kernel.
*/

struct DCRegData_mips_n32
{
  DClonglong                       mIntData[8];
  union { DCfloat f; DCdouble d; } mFloatData[8];
  DClonglong                       mUseDouble; /* bitmask: lower 8 bits specify to use float or double from union array. */
};

void dcCall_mips_n32(DCpointer target, struct DCRegData_mips_n32* regdata, DCsize stksize, DCpointer stkdata);
/* @@@ this is the same as n64, combine code */

#ifdef __cplusplus
}
#endif


#endif /* DYNCALL_CALL_MIPS_N64_H */

