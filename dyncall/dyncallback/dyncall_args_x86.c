/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_x86.c
 Description: Callback's Arguments VM - Implementation for x86
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


#include "dyncall_args_x86.h"

/* ---------------------------------------------------------------------------- */
/* C API implementation: */

/* base operations */

DCint      dcbArgInt     (DCArgs* p) { return p->vt->i32(p); }
DClonglong dcbArgLongLong(DCArgs* p) { return p->vt->i64(p); }
DCfloat    dcbArgFloat   (DCArgs* p) { return p->vt->f32(p); }
DCdouble   dcbArgDouble  (DCArgs* p) { return p->vt->f64(p); }

/* promote to integer: bool, char, short, long and pointer */

DCbool     dcbArgBool    (DCArgs* p) { return ( dcbArgInt(p) == 0 ) ? 0 : 1; }
DCchar     dcbArgChar    (DCArgs* p) { return (char)      dcbArgInt(p); }
DCshort    dcbArgShort   (DCArgs* p) { return (short)     dcbArgInt(p); }
DClong     dcbArgLong    (DCArgs* p) { return (long)      dcbArgInt(p); }
DCpointer  dcbArgPointer (DCArgs* p) { return (DCpointer) dcbArgInt(p); }

/* unsigned types */

DCuint      dcbArgUInt     (DCArgs* p) { return (DCuint)      dcbArgInt(p); }
DCuchar     dcbArgUChar    (DCArgs* p) { return (DCuchar)     dcbArgChar(p); }
DCushort    dcbArgUShort   (DCArgs* p) { return (DCushort)    dcbArgShort(p); }
DCulong     dcbArgULong    (DCArgs* p) { return (DCulong)     dcbArgLong(p); }
DCulonglong dcbArgULongLong(DCArgs* p) { return (DCulonglong) dcbArgLongLong(p); }

/* ---------------------------------------------------------------------------- */
/* virtual tables: */

/* cdecl calling convention */

static int default_i32(DCArgs* args)
{
	return *args->stack_ptr++;
}

static long long default_i64(DCArgs* args)
{
	long long result = * (long long*) args->stack_ptr;
	args->stack_ptr += 2;
	return result;
}

static float default_f32(DCArgs* args)
{
	float result = * ( (float*) args->stack_ptr );
	args->stack_ptr++;
	return result;
}

static double default_f64(DCArgs* args)
{
	double result = * ( (double*) args->stack_ptr );
	args->stack_ptr += 2;
	return  result;
}

DCArgsVT dcArgsVT_default   = { default_i32, default_i64, default_f32, default_f64 };

/* thiscall (microsoft) calling convention */

static int this_i32(DCArgs* args)
{
	if(args->fast_data[0]) { /* ecx register = this pointer */
		int thisArg = args->fast_data[0];
		args->fast_data[0] = 0;
		return thisArg;
	}
	return *args->stack_ptr++;
}

DCArgsVT dcArgsVT_this_ms = { this_i32, default_i64, default_f32, default_f64 };

/* fastcall (microsoft) calling convention */

static int fast_i32(DCArgs* args)
{
	if(args->fast_count < 2)
		return args->fast_data[args->fast_count++];
	else
		return default_i32(args);
}

DCArgsVT dcArgsVT_fast_ms = { fast_i32, default_i64, default_f32, default_f64 };

/* fastcall (gnu) calling convention */

static long long fast_gnu_i64(DCArgs* args)
{
	args->fast_count += 2;
	return default_i64(args);
}

DCArgsVT dcArgsVT_fast_gnu = { fast_i32, fast_gnu_i64, default_f32, default_f64 };
