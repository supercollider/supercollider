/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_arm32.c
 Description: Callback's Arguments VM - Implementation for ARM32 (ARM and THUMB mode)
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



#include "dyncall_args_arm32.h"


static void arm_align_64(DCArgs* args)
{
  /* Look at signature to see if current calling convention needs alignment */
  /* or not (e.g. EABI has different alignment). If nothing specified, fall */
  /* back to default behaviour for this platform.                           */
  /* @@@ check signature string */

  int sig =
#if defined(DC__ABI_ARM_EABI) || defined(DC__ABI_ARM_HF)
    0; /* EABI */
#else
    1; /* ATPCS */
#endif
  if(sig == 0) {
    if(args->reg_count < 4)
      args->reg_count = (args->reg_count+1)&~1;
    if(args->reg_count >= 4 && (int)args->stack_ptr & 4)
      ++args->stack_ptr;
  }
}


static void* arm_word(DCArgs* args)
{
  if(args->reg_count < 4)
    return &args->reg_data[args->reg_count++];
  else
    return (void*)args->stack_ptr++;
}

static DCfloat arm_float(DCArgs* args)
{
#if defined(DC__ABI_ARM_HF)
  DCfloat f;
  if(args->freg_count < 16) {
    f = args->f[args->freg_count++];

    /* if freg_count was odd, sync with dreg_count */
    if(!(args->freg_count & 1) && (args->freg_count < args->dreg_count))
      args->freg_count = args->dreg_count;

    return f;
  }
#endif
  return *(DCfloat*)arm_word(args);
}

static DCdouble arm_double(DCArgs* args)
{
  union {
    DCdouble d;
    DClong   l[2];
  } d;
#if defined(DC__ABI_ARM_HF)
  if(args->dreg_count < args->freg_count)
    args->dreg_count = (args->freg_count+1)&0x1e; /* clear last bit, counter won't be higher than 16, anyways */

  if(args->dreg_count < 16) {
    d.d = *(DCdouble*)&args->f[args->dreg_count];
    args->dreg_count += 2;

    /* freg_count is either odd (pointing to a gap), or always the same as dreg_count */
    if(!(args->freg_count & 1))
      args->freg_count = args->dreg_count;
    return d.d;
  }
  args->freg_count = 16; /* fp registers all full - need to use stack now: stop filling gaps for single precision, also */
#endif
  arm_align_64(args);
  d.l[0] = *(DClong*)arm_word(args);
  d.l[1] = *(DClong*)arm_word(args);
  return d.d;
}

static DClonglong arm_longlong(DCArgs* args)
{
  union {
    DClonglong ll;
    DClong     l[2];
  } ll;
  arm_align_64(args);
  ll.l[0] = *(DClong*)arm_word(args);
  ll.l[1] = *(DClong*)arm_word(args);
  return ll.ll;
}



// ----------------------------------------------------------------------------
// C API implementation:


// base operations:

DClonglong  dcbArgLongLong (DCArgs* p) { return arm_longlong(p); }
DClong      dcbArgLong     (DCArgs* p) { return *(DClong*)arm_word(p); }
DCint       dcbArgInt      (DCArgs* p) { return (DCint)   dcbArgLong(p); }
DCchar      dcbArgChar     (DCArgs* p) { return (DCchar)  dcbArgLong(p); }
DCshort     dcbArgShort    (DCArgs* p) { return (DCshort) dcbArgLong(p); }
DCbool      dcbArgBool     (DCArgs* p) { return (dcbArgLong(p) == 0) ? 0 : 1; }

DCuint      dcbArgUInt     (DCArgs* p) { return (DCuint)     dcbArgInt(p);      }
DCuchar     dcbArgUChar    (DCArgs* p) { return (DCuchar)    dcbArgChar(p);     }
DCushort    dcbArgUShort   (DCArgs* p) { return (DCushort)   dcbArgShort(p);    }
DCulong     dcbArgULong    (DCArgs* p) { return (DCulong)    dcbArgLong(p);     }
DCulonglong dcbArgULongLong(DCArgs* p) { return (DCulonglong)dcbArgLongLong(p); }


DCpointer   dcbArgPointer  (DCArgs* p) { return (DCpointer)  dcbArgLong(p); }

DCdouble    dcbArgDouble   (DCArgs* p) { return arm_double(p); }
DCfloat     dcbArgFloat    (DCArgs* p) { return arm_float(p); }

