/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_mips64.c
 Description: Callback's Arguments VM - Implementation for MIPS64 n32&n64
 License:

   Copyright (c) 2016 Tassilo Philipp <tphilipp@potion-studios.com>

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


#include "dyncall_args_mips.h"

/* @@@ test and add n32 code here, also, if needed */

DClonglong dcbArgLongLong(DCArgs* p)
{
  DClonglong value;
  if(p->reg_count < DCARGS_MIPS_NUM_IREGS)
    value = p->ireg_data[p->reg_count++];
  else {
    value = *((DClonglong*)p->stackptr);
    p->stackptr += sizeof(DClonglong);
  }
  return value;
}
DCulonglong dcbArgULongLong(DCArgs* p) { return (DCulonglong)dcbArgLongLong(p); }

DCint       dcbArgInt    (DCArgs* p) { return (DCint)    dcbArgLongLong(p); }
DCuint      dcbArgUInt   (DCArgs* p) { return (DCuint)   dcbArgLongLong(p); }
DClong      dcbArgLong   (DCArgs* p) { return (DClong)   dcbArgLongLong(p); }
DCulong     dcbArgULong  (DCArgs* p) { return (DCulong)  dcbArgLongLong(p); }
DCchar      dcbArgChar   (DCArgs* p) { return (DCchar)   dcbArgLongLong(p); }
DCuchar     dcbArgUChar  (DCArgs* p) { return (DCuchar)  dcbArgLongLong(p); }
DCshort     dcbArgShort  (DCArgs* p) { return (DCshort)  dcbArgLongLong(p); }
DCushort    dcbArgUShort (DCArgs* p) { return (DCushort) dcbArgLongLong(p); }
DCbool      dcbArgBool   (DCArgs* p) { return (DCbool)   dcbArgLongLong(p); }
DCpointer   dcbArgPointer(DCArgs* p) { return (DCpointer)dcbArgLongLong(p); }

DCdouble dcbArgDouble(DCArgs* p)
{
  DCdouble result;
  if(p->reg_count < DCARGS_MIPS_NUM_FREGS)
    result = p->freg_data[p->reg_count++];
  else {
    result = *((DCdouble*)p->stackptr);
    p->stackptr += sizeof(DCdouble);
  }
  return result;
}
DCfloat dcbArgFloat(DCArgs* p)
{
  DCfloat result;
  if(p->reg_count < DCARGS_MIPS_NUM_FREGS) {
    result = ((DCfloat*)&p->freg_data[p->reg_count++])
#if defined(DC__Endian_LITTLE)
      [0];
#else
      [1]; /* single precision floats are right-justified in big-endian registers */
#endif
  } else {
    result = *((DCfloat*)p->stackptr); /* single precision floats are left-justified on stack in 64bit slots */
    p->stackptr += sizeof(DCdouble);
  }
  return result;
}

