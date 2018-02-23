/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args_sparc64.h
 Description: Callback's Arguments VM - Header for sparc64 - not yet
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


#ifndef DYNCALLBACK_ARGS_SPARC64_H
#define DYNCALLBACK_ARGS_SPARC64_H

#include "dyncall_args.h"

#define DCARGS_SPARC64_NUM_DOUBLE_REGS 16
struct DCArgs
{
	/* Don't change order or types, laid out for asm code to fill in! */
	DClonglong *arg_ptr;
	DCdouble   dreg_data[DCARGS_SPARC64_NUM_DOUBLE_REGS];
	DClonglong i; /* args fetched */
};

#endif /* DYNCALLBACK_ARGS_SPARC64_H */

