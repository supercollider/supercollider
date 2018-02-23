/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_mips_gas.S
 Description: auto-select (via C preprocessor) mips abi call kernel
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



#if defined(DC__ABI_MIPS_O32)
#include "dyncall_call_mips_o32_gas.s"
#elif defined(DC__ABI_MIPS_N64)
#include "dyncall_call_mips_n64_gas.s"
#elif defined(DC__ABI_MIPS_N32)
#include "dyncall_call_mips_n32_gas.s"
#else
#include "dyncall_call_mips_eabi_gas.s"
#endif

