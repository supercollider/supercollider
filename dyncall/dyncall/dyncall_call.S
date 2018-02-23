/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call.S
 Description: assembly call kernel, auto-selected by gcc / Plan9 build system
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


#include "dyncall_macros.h"

/* Plan9 */
#if defined(DC__OS_Plan9)
#  if defined(DC__Arch_Intel_x86)
#    include "dyncall_call_x86_8a.s"
#  else
#    error Unsupported Architecture on Plan9.
#  endif
#elif defined(DC__C_MSVC)
#  if defined(DC__OS_Win32)
#    include "dyncall_call_x86_generic_masm.asm"
#  elif defined(DC__OS_Win64)
#    include "dyncall_call_x64_generic_masm.asm"
#  endif
#else
#  if defined(DC__Arch_Intel_x86)
#    include "dyncall_call_x86.S"
#  elif defined(DC__Arch_AMD64)
#    include "dyncall_call_x64.S"
#  elif defined(DC__Arch_ARM_ARM)
#    if defined(DC__ABI_ARM_HF)
#      include "dyncall_call_arm32_arm_armhf.S"
#    else
#      include "dyncall_call_arm32_arm.S"
#    endif
#  elif defined(DC__Arch_ARM_THUMB)
#    if defined(DC__OS_Darwin)
#      include "dyncall_call_arm32_thumb_apple.s"
#    else
#      if defined(DC__ABI_ARM_HF)
#        include "dyncall_call_arm32_thumb_armhf.S"
#      else
#        include "dyncall_call_arm32_thumb_gas.s"
#      endif
#    endif
#  elif defined(DC__Arch_ARM64)
#    include "dyncall_call_arm64.S"
#  elif defined(DC__Arch_PPC32)
#    include "dyncall_call_ppc32.S"
#  elif defined(DC__Arch_PPC64)
#    include "dyncall_call_ppc64.S"
#  elif defined(DC__Arch_MIPS) || defined(DC__Arch_MIPS64)
#    include "dyncall_call_mips_gas.S"
#  elif defined(DC__Arch_Sparc64)
#    include "dyncall_call_sparc64.s"
#  elif defined(DC__Arch_Sparc)
#    include "dyncall_call_sparc.s"
#  else
#    error Unsupported Architecture.
#  endif
#endif


/* Use .note.GNU-stack to explicitly indicate a non-exec stack, b/c of bad   */
/* default behaviour when translating handwritten assembly files (needed on  */
/* GNU platforms, Android and FreeBSD, as tests have shown).                 */
#if (defined(DC__C_CLANG) || defined(DC__C_GNU)) && defined(__ELF__) && (defined(DC__OS_Linux) || defined(DC__OS_FreeBSD))
.section .note.GNU-stack,"",%progbits
#endif

