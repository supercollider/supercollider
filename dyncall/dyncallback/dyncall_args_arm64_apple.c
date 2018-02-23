/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_arm64_apple.c
 Description: Callback's Arguments VM - Implementation for Apple's ARM64 / ARMv8 / AAPCS64
 License:

   Copyright (c) 2015 Daniel Adler <dadler@uni-goettingen.de>,
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

#include "dyncall_args.h"

#include <stdint.h>

typedef union {
  struct { double value; } d;
  struct { float  value; } f;
} DCFPU_t;

struct DCArgs
{
  /* buffers and stack-pointer: */

  uint64_t  I[8];
  DCFPU_t   F[8];
  uint8_t*  sp;
  
  /* counters: */
  int i;
  int f;
  int s;
  int reserved;
};

static inline uint8_t* align(uint8_t* p, size_t v)
{
  return (uint8_t*) ( ( ( (ptrdiff_t) p ) + v - 1) & (ptrdiff_t) -v );
}


DClonglong dcbArgLongLong (DCArgs* p) 
{
  if (p->i < 8) {
    return p->I[p->i++];
  } else {
    DClonglong value;
    p->sp = align(p->sp,sizeof(DClonglong));
    value =  * ( (DClonglong*) p->sp );
    p->sp += sizeof(DClonglong);
    return value;
  }
}
DCdouble  dcbArgDouble (DCArgs* p) {
  if (p->f < 8) { 
    return p->F[p->f++].d.value;
  } else {
    DCdouble value;
    p->sp = align(p->sp,sizeof(DCdouble));
    value =  * ( (DCdouble*) p->sp );
    p->sp += sizeof(DCdouble);
    return value;
  }
}
DCfloat   dcbArgFloat  (DCArgs* p) {
  if (p->f < 8) {
    return p->F[p->f++].f.value;
  } else {
    DCfloat value;
    p->sp = align(p->sp,sizeof(DCfloat));
    value =  * ( (DCfloat*) p->sp );
    p->sp += sizeof(DCfloat);
    return value;
  }
}

DClong    dcbArgLong   (DCArgs* p) { 
  if (p->i < 8) {
    return (DClong) p->I[p->i++];
  } else {
    DClong value;
    p->sp = align(p->sp,sizeof(DClong));
    value =  * ( (DClong*) p->sp );
    p->sp += sizeof(DClong);
    return value;
  }
}

DCint     dcbArgInt    (DCArgs* p) { 
  if (p->i < 8) {
    return (DCint) p->I[p->i++];
  } else {
    DCint value;
    p->sp = align(p->sp,sizeof(DCint));
    value =  * ( (DCint*) p->sp );
    p->sp += sizeof(DCint);
    return value;
  }
}

DCshort   dcbArgShort  (DCArgs* p) { 
  if (p->i < 8) {
    return (DCshort) p->I[p->i++];
  } else {
    DCshort value;
    p->sp = align(p->sp,sizeof(DCshort));
    value =  * ( (DCshort*) p->sp );
    p->sp += sizeof(DCshort);
    return value;
  }
}

DCchar    dcbArgChar   (DCArgs* p) { 
  if (p->i < 8) {
    return (DCchar) p->I[p->i++];
  } else {
    DCchar value;
    p->sp = align(p->sp,sizeof(DCchar));
    value =  * ( (DCchar*) p->sp );
    p->sp += sizeof(DCchar);
    return value;
  }
}

DCbool    dcbArgBool   (DCArgs* p) { 
  if (p->i < 8) {
    return (DCbool) p->I[p->i++];
  } else {
    DCbool value;
    p->sp = align(p->sp,sizeof(DCbool));
    value =  * ( (DCbool*) p->sp );
    p->sp += sizeof(DCbool);
    return value;
  }
}

DCpointer dcbArgPointer(DCArgs* p) { 
  return (DCpointer) dcbArgLongLong(p); 
}

DCuint      dcbArgUInt     (DCArgs* p) { return (DCuint)      dcbArgInt(p);      }
DCuchar     dcbArgUChar    (DCArgs* p) { return (DCuchar)     dcbArgChar(p);     }
DCushort    dcbArgUShort   (DCArgs* p) { return (DCushort)    dcbArgShort(p);    }
DCulong     dcbArgULong    (DCArgs* p) { return (DCulong)     dcbArgLong(p);     }
DCulonglong dcbArgULongLong(DCArgs* p) { return (DCulonglong) dcbArgLongLong(p); }

