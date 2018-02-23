/*

 Package: dyncall
 Library: autovar
 File: autovar/autovar_ABI.h
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

#ifndef AUTOVAR_ABI_H
#define AUTOVAR_ABI_H

#include "autovar_OS.h"

#if defined(OS_Win32) || defined(OS_Win64) || defined(OS_Cygwin) || defined(OS_MinGW)
#define ABI_PE
#elif defined(OS_Darwin)
#define ABI_Mach
#elif !defined(OS_Minix) || defined(__ELF__) /* Minix >= 3.2 (2012) uses ELF */
#define ABI_ELF
# if defined(__LP64__) || defined(_LP64)
#   define ABI_ELF64
# else
#   define ABI_ELF32
# endif
#else
#define ABI_Unknown
#endif

#endif /* AUTOVAR_ABI_H */

