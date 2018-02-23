/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_sparc32.c
 Description: Callback's Arguments VM - Implementation for sparc32 - not yet
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


#include "dyncall_args_sparc32.h"

/* Compiler aligns this to 8-byte boundaries, b/c of dword members, a fact needed below */
typedef union {
  DCdouble    d;
  DCulonglong L;
  DCulong     l[2];
} DCAligned64BitVal_t;

static void* sparc_word(DCArgs* args)
{
  return args->arg_ptr++;
}

static void* sparc_dword(DCArgs* args)
{
  void *p = args->arg_ptr;
  args->arg_ptr += 2;
  return p;
}

/* copy words so that dwords are 8-byte aligned, for 64bit value indirection; unaligned
   access results in SIGBUS; not quite sure why compiler doesn't abstract this (maybe b/c
   of not having any idea about our assembler code?) */
DCulonglong dcbArgULongLong(DCArgs* p) { DCAligned64BitVal_t v; DCulong *l = (DCulong*)sparc_dword(p); v.l[0]=l[0]; v.l[1]=l[1]; return v.L; }
DCdouble    dcbArgDouble   (DCArgs* p) { DCAligned64BitVal_t v; DCulong *l = (DCulong*)sparc_dword(p); v.l[0]=l[0]; v.l[1]=l[1]; return v.d; }

/* rest of getters based on above functions */
DCuint      dcbArgUInt     (DCArgs* p) { return *(DCuint*)sparc_word(p); }
DClonglong  dcbArgLongLong (DCArgs* p) { return (DClonglong)dcbArgULongLong(p); }
DCint       dcbArgInt      (DCArgs* p) { return (DCint)     dcbArgUInt(p); }
DClong      dcbArgLong     (DCArgs* p) { return (DClong)    dcbArgUInt(p); }
DCulong     dcbArgULong    (DCArgs* p) { return (DCulong)   dcbArgUInt(p); }
DCchar      dcbArgChar     (DCArgs* p) { return (DCchar)    dcbArgUInt(p); }
DCuchar     dcbArgUChar    (DCArgs* p) { return (DCuchar)   dcbArgUInt(p); }
DCshort     dcbArgShort    (DCArgs* p) { return (DCshort)   dcbArgUInt(p); }
DCushort    dcbArgUShort   (DCArgs* p) { return (DCushort)  dcbArgUInt(p); }
DCbool      dcbArgBool     (DCArgs* p) { return (DCbool)    dcbArgUInt(p); }
DCpointer   dcbArgPointer  (DCArgs* p) { return (DCpointer) dcbArgUInt(p); }

DCfloat     dcbArgFloat    (DCArgs* p) { return *(DCfloat*) sparc_word(p); }

