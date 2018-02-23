/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_sparc64.c
 Description: Callback's Arguments VM - Implementation for sparc64 - not yet
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


#include "dyncall_args_sparc64.h"

DCulonglong dcbArgULongLong(DCArgs* p) { return p->arg_ptr[p->i++]; }
DClonglong  dcbArgLongLong (DCArgs* p) { return (DClonglong)dcbArgULongLong(p); }
DCulong     dcbArgULong    (DCArgs* p) { return (DCulong)   dcbArgULongLong(p); }
DClong      dcbArgLong     (DCArgs* p) { return (DClong)    dcbArgULongLong(p); }
DCuint      dcbArgUInt     (DCArgs* p) { return (DCuint)    dcbArgULongLong(p); }
DCint       dcbArgInt      (DCArgs* p) { return (DCint)     dcbArgULongLong(p); }
DCuchar     dcbArgUChar    (DCArgs* p) { return (DCuchar)   dcbArgULongLong(p); }
DCchar      dcbArgChar     (DCArgs* p) { return (DCchar)    dcbArgULongLong(p); }
DCushort    dcbArgUShort   (DCArgs* p) { return (DCushort)  dcbArgULongLong(p); }
DCshort     dcbArgShort    (DCArgs* p) { return (DCshort)   dcbArgULongLong(p); }
DCbool      dcbArgBool     (DCArgs* p) { return (DCbool)    dcbArgULongLong(p); }
DCpointer   dcbArgPointer  (DCArgs* p) { return (DCpointer) dcbArgULongLong(p); }

DCdouble dcbArgDouble(DCArgs* p)
{
	return (p->i < DCARGS_SPARC64_NUM_DOUBLE_REGS)
		? p->dreg_data[p->i++]
		: *(DCdouble*)(p->arg_ptr + p->i++);
}

DCfloat dcbArgFloat(DCArgs* p)
{
	return (p->i < DCARGS_SPARC64_NUM_DOUBLE_REGS)
		? *((DCfloat*)(p->dreg_data + p->i++)+1)  /* +1 bc single-prec fp args are */
		: *((DCfloat*)(p->arg_ptr   + p->i++)+1); /* right aligned in 64bit slot   */
}

