/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm.c
 Description: auto-select default callvm (includes other C sources).
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

#if defined(DC__Arch_Intel_x86)
#  include "dyncall_callvm_x86.c"
#elif defined(DC__Arch_AMD64)
#  include "dyncall_callvm_x64.c"
#elif defined(DC__Arch_PPC32)
#  include "dyncall_callvm_ppc32.c"
#elif defined(DC__Arch_PPC64)
#  include "dyncall_callvm_ppc64.c"
#elif defined(DC__Arch_MIPS) || defined(DC__Arch_MIPS64)
#  if defined(DC__ABI_MIPS_EABI)
#    include "dyncall_callvm_mips_eabi.c"
#  elif defined(DC__ABI_MIPS_O32)
#    include "dyncall_callvm_mips_o32.c"
#  elif defined(DC__ABI_MIPS_N64)
#    include "dyncall_callvm_mips_n64.c"
#  elif defined(DC__ABI_MIPS_N32)
#    include "dyncall_callvm_mips_n32.c"
#  else
#    error Unknown MIPS ABI.
#  endif /* DC__Arch_MIPS || DC__Arch_MIPS64 */
#elif defined(DC__Arch_ARM_ARM)
#  if defined(DC__ABI_ARM_HF)
#    include "dyncall_callvm_arm32_arm_armhf.c"
#  else
#    include "dyncall_callvm_arm32_arm.c"
#  endif
#elif defined(DC__Arch_ARM_THUMB)
#  if defined(DC__ABI_ARM_HF)
#    include "dyncall_callvm_arm32_arm_armhf.c"
#  else
#    include "dyncall_callvm_arm32_thumb.c"
#  endif
#elif defined(DC__Arch_ARM64)
#  if defined(DC__OS_Darwin)
#    include "dyncall_callvm_arm64_apple.c"
#  else
#    include "dyncall_callvm_arm64.c"
#  endif
#elif defined(DC__Arch_Sparc)
#  include "dyncall_callvm_sparc.c"
#elif defined(DC__Arch_Sparc64)
#  include "dyncall_callvm_sparc64.c"
#elif defined(DC__Arch_RiscV)
#  include "dyncall_callvm_riscv.c"
#else
#  error unsupported platform
#endif

