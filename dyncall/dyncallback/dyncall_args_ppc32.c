/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_ppc32.c
 Description: Callback's Arguments VM - Implementation for ppc32
 License:

   Copyright (c) 2007-2017 Daniel Adler <dadler@uni-goettingen.de>,
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


#include "dyncall_args_ppc32.h"

DCint       dcbArgInt      (DCArgs* p) 
{
  DCint value;
  if (p->ireg_count < 8)
    value = p->ireg_data[p->ireg_count++];
  else
    value = *( (int*) p->stackptr );
  p->stackptr += sizeof(int);
  return value;
}
DCuint      dcbArgUInt     (DCArgs* p) { return (DCuint)  dcbArgInt(p);  }

DCulonglong  dcbArgULongLong (DCArgs* p) 
{
  DCulonglong value;
  value  = ( (DCulonglong) dcbArgUInt(p) ) << 32UL;
  value |= dcbArgUInt(p);
  return value;
}
DClonglong  dcbArgLongLong(DCArgs* p) { return (DClonglong)dcbArgULongLong(p); }

DClong      dcbArgLong     (DCArgs* p) { return (DClong)  dcbArgUInt(p); }
DCulong     dcbArgULong    (DCArgs* p) { return (DCulong) dcbArgUInt(p); }
DCchar      dcbArgChar     (DCArgs* p) { return (DCchar)  dcbArgUInt(p); }
DCuchar     dcbArgUChar    (DCArgs* p) { return (DCuchar) dcbArgUInt(p); }
DCshort     dcbArgShort    (DCArgs* p) { return (DCshort) dcbArgUInt(p); }
DCushort    dcbArgUShort   (DCArgs* p) { return (DCushort)dcbArgUInt(p); }
DCbool      dcbArgBool     (DCArgs* p) { return (DCbool)  dcbArgUInt(p); }

DCpointer   dcbArgPointer  (DCArgs* p) { return (DCpointer)dcbArgUInt(p); }

DCdouble    dcbArgDouble   (DCArgs* p) 
{ 
  DCdouble result;
  if (p->ireg_count < 7) { 
    p->ireg_count+=2;
  } else if (p->ireg_count == 7) {
    p->ireg_count = 8;
  }
  if (p->freg_count < 13) {
    result = p->freg_data[p->freg_count++];
  } else {
    result = * ( (double*) p->stackptr );
  }
  p->stackptr += sizeof(double);
  return result;
}
DCfloat     dcbArgFloat    (DCArgs* p)
{ 
  DCfloat result;
  if (p->ireg_count < 8)
    p->ireg_count++;
  if (p->freg_count < 13) {
    result = (DCfloat) p->freg_data[p->freg_count++];
  } else {
    result = * ( (float*) p->stackptr );
  }
  p->stackptr += sizeof(float);
  return result; 
}

