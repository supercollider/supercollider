/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_callback_arm32_thumb_gas.S
 Description: Callback Thunk - Implementation for ARM32 (THUMB mode)
 License:

   Copyright (c) 2007-2013 Daniel Adler <dadler@uni-goettingen.de>,
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


/* We can use the ARM mode callback code here, because the thunk switches  */
/* into ARM mode, the parameters passed use the same registers/stack space */
/* as the ARM mode, and the bx instruction switches back to THUMB mode, if */
/* the function to be called has a "THUMB function address" (=address+1).  */
#include "dyncall_callback_arm32_arm_gas.S"

