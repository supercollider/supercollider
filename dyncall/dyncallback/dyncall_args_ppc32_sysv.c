/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_ppc32_sysv.c
 Description: Callback's Args Implementation for PowerPC 32-bit System V ABI
 License:

   Copyright (c) 2015 Daniel Adler <dadler@uni-goettingen.de>

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
  else {
    value = *( (int*) p->stackptr );
    p->stackptr += sizeof(int);
  }
  return value;
}
DCuint      dcbArgUInt     (DCArgs* p) { return (DCuint)  dcbArgInt(p);  }

DCulonglong  dcbArgULongLong (DCArgs* p) 
{
  DCulonglong value;
  if (p->ireg_count < 7)
  {
    /* next free integer register is even (r0, r2, r3) ? */
    /* if not, skip one integer */
    p->ireg_count = (p->ireg_count + 1) & -2;
    value = * (unsigned long long*) ( & p->ireg_data[p->ireg_count] );
    p->ireg_count += 2;
  } else {
    p->ireg_count = 8;
    /* 64-bit values are naturally aligned on stack */
    p->stackptr = (unsigned char*) ( ( ( (ptrdiff_t) (p->stackptr) ) + 7) & ( (ptrdiff_t) -8 ) );
    value = * ( (unsigned long long*) p->stackptr );
    p->stackptr += sizeof(unsigned long long);
  }
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
  if (p->freg_count < 8) {
    result = p->freg_data[p->freg_count++];
  } else {
    p->stackptr = (unsigned char*) ( ( ( (ptrdiff_t) (p->stackptr) ) + 7) & ( (ptrdiff_t) -8 ) );
    result = * ( (double*) p->stackptr );
    p->stackptr += sizeof(double);
  }
  return result;
}
DCfloat     dcbArgFloat    (DCArgs* p)
{ 
  DCfloat result;
  if (p->freg_count < 8) {
    result = (DCfloat) p->freg_data[p->freg_count++];
  } else {
    result = * ( (float*) p->stackptr );
    p->stackptr += sizeof(float);
  }
  return result; 
}

