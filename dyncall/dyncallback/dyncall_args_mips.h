/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_mips.h
 Description: Callback's Arguments VM - Header for MIPS
 License:

   Copyright (c) 2013-2015 Daniel Adler <dadler@uni-goettingen.de>,
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


#ifndef DYNCALLBACK_ARGS_MIPS_H
#define DYNCALLBACK_ARGS_MIPS_H

#include "dyncall_args.h"

struct DCArgs
{
	/* Don't change order or types, laid out for asm code to fill in! */
#if defined(DC__Arch_MIPS) && defined(DC__ABI_MIPS_O32)
	DCint freg_count;
#else
#  define DCARGS_MIPS_NUM_IREGS 8
#  define DCARGS_MIPS_NUM_FREGS 8
#  if defined(DC__Arch_MIPS)
	DCint   ireg_data[DCARGS_MIPS_NUM_IREGS];
	DCfloat freg_data[DCARGS_MIPS_NUM_FREGS];
	struct { DCshort i; DCshort f; } reg_count;
#  elif defined(DC__Arch_MIPS64)
	DClonglong ireg_data[DCARGS_MIPS_NUM_IREGS];
	DCdouble   freg_data[DCARGS_MIPS_NUM_FREGS];
	DClonglong reg_count;
#  endif
#endif
	DCuchar* stackptr;
};

#endif /* DYNCALLBACK_ARGS_MIPS_H */

