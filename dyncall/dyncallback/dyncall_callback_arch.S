/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_arch.S
 Description: Callback Thunk - Implementation Back-end selector
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

#if defined(DC__C_MSVC)
#  if defined(DC__OS_Win32)
#    include "dyncall_callback_x86_masm.asm"
#  elif defined(DC__OS_Win64)
#    include "dyncall_callback_x64_masm.asm"
#  endif
#elif defined(DC__OS_Plan9)
#  if defined(DC__Arch_Intel_x86)
#    include "dyncall_callback_x86_8a.s"
#  else
#    error Unsupported Architecture on Plan9.
#  endif
#elif defined(DC__OS_Darwin)
#  if defined(DC__Arch_PPC32)
#    include "dyncall_callback_ppc32_apple.s"
#  elif defined(DC__Arch_ARM_ARM)
#    include "dyncall_callback_arm32_arm_apple.s"
#  elif defined(DC__Arch_ARM_THUMB)
#    include "dyncall_callback_arm32_thumb_apple.S"
#  elif defined(DC__Arch_AMD64)
#    include "dyncall_callback_x64.S"
#  elif defined(DC__Arch_Intel_x86)
#    include "dyncall_callback_x86.S"
#  elif defined(DC__Arch_ARM64)
#    include "dyncall_callback_arm64.S"
#  else // e.g. PowerPC G5, which is PPC64, etc.
#    error Unsupported Architecture on Darwin/OS X.
#  endif
#else 
#  if defined(DC__Arch_Intel_x86)
#    include "dyncall_callback_x86.S"
#  elif defined(DC__Arch_AMD64)
#    include "dyncall_callback_x64.S"
#  elif defined(DC__Arch_PPC32)
#    include "dyncall_callback_ppc32.S"
#  elif defined(DC__Arch_PPC64)
#    include "dyncall_callback_ppc64.S"
#  elif defined(DC__Arch_MIPS) || defined(DC__Arch_MIPS64)
#    include "dyncall_callback_mips_gas.S"
#  elif defined(DC__Arch_ARM_ARM)
#    include "dyncall_callback_arm32_arm_gas.S"
#  elif defined(DC__Arch_ARM_THUMB)
#    include "dyncall_callback_arm32_thumb_gas.S"
#  elif defined(DC__Arch_Sparc)
#    include "dyncall_callback_sparc32.s"
#  elif defined(DC__Arch_Sparc64)
#    include "dyncall_callback_sparc64.s"
#  elif defined(DC__Arch_ARM64)
#    include "dyncall_callback_arm64.S"
#  endif
#endif


/* Use .note.GNU-stack to explicitly indicate a non-exec stack, b/c of bad   */
/* default behaviour when translating handwritten assembly files (needed on  */
/* GNU platforms, Android and FreeBSD, as tests have shown).                 */
#if (defined(DC__C_CLANG) || defined(DC__C_GNU)) && defined(__ELF__) && (defined(DC__OS_Linux) || defined(DC__OS_FreeBSD))
.section .note.GNU-stack,"",%progbits
#endif

