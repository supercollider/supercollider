/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_ppc64.c
 Description: Callback's Arguments VM - Implementation for ppc64
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

#include "dyncall_args_ppc64.h"

DCint       dcbArgInt      (DCArgs* p) { return (DCint)       dcbArgLongLong(p); }
DCuint      dcbArgUInt     (DCArgs* p) { return (DCuint)      dcbArgLongLong(p); }
DCulonglong dcbArgULongLong(DCArgs* p) { return (DCulonglong) dcbArgLongLong(p); }

DClonglong  dcbArgLongLong(DCArgs* p)
{
  DClonglong value;
  if (p->ireg_count < 8) {
    value = p->ireg_data[p->ireg_count++];
  } else {
    value = *( (long long*) p->stackptr );
  }
  p->stackptr += sizeof(long long);
  return value;
}

DClong      dcbArgLong     (DCArgs* p) { return (DClong)  dcbArgLongLong(p); }
DCulong     dcbArgULong    (DCArgs* p) { return (DCulong) dcbArgLongLong(p); }
DCchar      dcbArgChar     (DCArgs* p) { return (DCchar)  dcbArgLongLong(p); }
DCuchar     dcbArgUChar    (DCArgs* p) { return (DCuchar) dcbArgLongLong(p); }
DCshort     dcbArgShort    (DCArgs* p) { return (DCshort) dcbArgLongLong(p); }
DCushort    dcbArgUShort   (DCArgs* p) { return (DCushort)dcbArgLongLong(p); }
DCbool      dcbArgBool     (DCArgs* p) { return (DCbool)  dcbArgLongLong(p); }

DCpointer   dcbArgPointer  (DCArgs* p) { return (DCpointer)dcbArgLongLong(p); }

DCdouble    dcbArgDouble   (DCArgs* p)
{
  DCdouble result;

  if (p->freg_count < 13) {
    result = p->freg_data[p->freg_count++];
    if (p->ireg_count < 8) {
      p->ireg_count++;
    }
  } else {
    result = * ( (double*) p->stackptr );
  }

  p->stackptr += sizeof(double);
  return result;
}

DCfloat    dcbArgFloat   (DCArgs* p)
{
  DCfloat result;

#if defined(DC__Endian_BIG)
  struct sf { DCfloat f_pad; DCfloat f; } sf;
#else /* Endian_LITTLE */
  struct sf { DCfloat f; DCfloat f_pad; } sf;
#endif

  if (p->freg_count < 13) {
    result = (float)p->freg_data[p->freg_count++];
    if (p->ireg_count < 8) {
      p->ireg_count++;
    }
  } else {
    sf = * ( (struct sf*) p->stackptr );
    result = sf.f;
  }

  p->stackptr += sizeof(double);
  return result;
}
