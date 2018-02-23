/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_arm64.h
 Description: ARM 64-bit
 License:

   Copyright (c) 2015 Daniel Adler <dadler@uni-goettingen.de>, 
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



#ifndef DYNCALL_CALL_ARM64_DEBIAN_H
#define DYNCALL_CALL_ARM64_DEBIAN_H


#include "dyncall_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void dcCall_arm64(DCpointer target, DCpointer data, DCsize size, DCpointer regdata);

#ifdef __cplusplus
}
#endif


#endif /* DYNCALL_CALL_ARM64_DEBIAN_H */
