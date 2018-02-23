/*

 Package: dyncall
 Library: autovar
 File: autovar/autovar_ARCH.h
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

#ifndef AUTOVAR_ARCH_H
#define AUTOVAR_ARCH_H

/* Check architecture. */
#if defined(_M_X64_) || defined(_M_AMD64) || defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) 
# define ARCH_X64
#elif defined(_M_IX86) || defined(__i386__) || defined(__i486__) || defined(__i586__) || defined(__i686__) || defined(__386__) || defined(__i386)
# define ARCH_X86
#elif defined(_M_IA64) || defined(__ia64__)
# define ARCH_IA64
#elif defined(_M_PPC) || defined(__powerpc__) || defined(__powerpc) || defined(__POWERPC__) || defined(__ppc__) || defined(__power__)
# if defined(__ppc64__) || defined(_ARCH_PPC64) || defined(__power64__) || defined(__powerpc64__)
#   define ARCH_PPC64
# else
#   define ARCH_PPC
# endif
#elif defined(__mips64__) || defined(__mips64)
# define ARCH_MIPS64
#elif defined(_M_MRX000) || defined(__mips__) || defined(__mips) || defined(_mips)
# define ARCH_MIPS
#elif defined(__arm__)
# define ARCH_ARM
# if defined(__thumb__)
#   define ARCH_THUMB
# endif
#elif defined(__aarch64__)
# define ARCH_ARM64
#elif defined(__sh__)
# define ARCH_SH
#elif defined(__sparc) || defined(__sparc__)
# if defined(__sparcv9) || defined(__sparc_v9__) || defined(__sparc64__) || defined(__arch64__)
#  define ARCH_SPARC64
# else
#  define ARCH_SPARC
# endif
#endif

#endif /* AUTOVAR_ARCH_H */
