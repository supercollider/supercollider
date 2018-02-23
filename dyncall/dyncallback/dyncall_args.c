/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_args.c
 Description: Callback's Arguments VM - Implementation back-end selector
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

#include "../dyncall/dyncall_macros.h"

#if defined(DC__Arch_Intel_x86)
#  include "dyncall_args_x86.c"
#elif defined(DC__Arch_AMD64)
#  include "dyncall_args_x64.c"
#elif defined(DC__Arch_PPC32)
#  if defined(DC__OS_Darwin)
#    include "dyncall_args_ppc32.c"
#  else
#    include "dyncall_args_ppc32_sysv.c"
#  endif
#elif defined(DC__Arch_PPC64)
#  include "dyncall_args_ppc64.c"
#elif defined(DC__Arch_ARM)
#  include "dyncall_args_arm32.c"
#elif defined(DC__Arch_MIPS)
#  if defined(DC__ABI_MIPS_O32)
#    include "dyncall_args_mips_o32.c"
#  else
#    include "dyncall_args_mips.c"
#  endif
#elif defined(DC__Arch_MIPS64)
#    include "dyncall_args_mips64.c"
#elif defined(DC__Arch_Sparc)
#  include "dyncall_args_sparc32.c"
#elif defined(DC__Arch_Sparc64)
#  include "dyncall_args_sparc64.c"
#elif defined(DC__Arch_ARM64)
#  if defined(DC__OS_Darwin)
#    include "dyncall_args_arm64_apple.c"
#  else
#    include "dyncall_args_arm64.c"
#  endif
#endif

