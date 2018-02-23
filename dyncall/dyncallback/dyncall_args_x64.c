/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_x64.c
 Description: Callback's Arguments VM - Implementation for x64
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



#include "dyncall_args_x64.h"


static int64* arg_i64(DCArgs* args)
{
  if (args->reg_count.i < numIntRegs)
    return &args->reg_data.i[args->reg_count.i++];
  else
    return args->stack_ptr++;
}


static double* arg_f64(DCArgs* args)
{
  if (args->reg_count.f < numFloatRegs)
    return &args->reg_data.f[args->reg_count.f++];
  else
  {
    return (double*)args->stack_ptr++;
  }
}


// ----------------------------------------------------------------------------
// C API implementation:


// base operations:

DClonglong  dcbArgLongLong (DCArgs* p) { return *arg_i64(p); }
DCint       dcbArgInt      (DCArgs* p) { return (int)   dcbArgLongLong(p); }
DClong      dcbArgLong     (DCArgs* p) { return (long)  dcbArgLongLong(p); }
DCchar      dcbArgChar     (DCArgs* p) { return (char)  dcbArgLongLong(p); }
DCshort     dcbArgShort    (DCArgs* p) { return (short) dcbArgLongLong(p); }
DCbool      dcbArgBool     (DCArgs* p) { return (dcbArgInt(p) == 0) ? 0 : 1; }

DCuint      dcbArgUInt     (DCArgs* p) { return (DCuint)      dcbArgInt(p);      }
DCuchar     dcbArgUChar    (DCArgs* p) { return (DCuchar)     dcbArgChar(p);     }
DCushort    dcbArgUShort   (DCArgs* p) { return (DCushort)    dcbArgShort(p);    }
DCulong     dcbArgULong    (DCArgs* p) { return (DCulong)     dcbArgLong(p);     }
DCulonglong dcbArgULongLong(DCArgs* p) { return (DCulonglong) dcbArgLongLong(p); }


DCpointer   dcbArgPointer  (DCArgs* p) { return (DCpointer)   dcbArgLongLong(p); }

DCdouble    dcbArgDouble   (DCArgs* p) { return *arg_f64(p); }
DCfloat     dcbArgFloat    (DCArgs* p) { return *(float*)arg_f64(p); }
