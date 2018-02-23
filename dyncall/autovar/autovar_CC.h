/*

 Package: dyncall
 Library: autovar
 File: autovar/autovar_CC.h
 Description: 
 License:

   Copyright (c) 2011-2015 Daniel Adler <dadler@uni-goettingen.de>,
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

#ifndef AUTOVAR_CC_H
#define AUTOVAR_CC_H

/* Compiler specific defines. Do not change the order, because  */
/* some of the compilers define flags for compatible ones, too. */

#if defined(__INTEL_COMPILER)
#define CC_INTEL
#elif defined(_MSC_VER)
#define CC_MSVC
#elif defined(__clang__) || defined(__llvm__)
#define CC_CLANG
#elif defined(__GNUC__)
#define CC_GNU
#elif defined(__WATCOMC__)
#define CC_WATCOM
#elif defined(__PCC__)
#define CC_PCC
#elif defined(__SUNPRO_C)
#define CC_SUN
#endif

#endif /* AUTOVAR_CC_H */

