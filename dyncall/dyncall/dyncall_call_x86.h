/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_call_x86.h
 Description: heap memory management interface (header only)
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



/*

  dyncall 32bit Intel x86 family interface

  REVISION
  2007/12/10 initial

*/


#ifndef DYNCALL_CALL_X86_H
#define DYNCALL_CALL_X86_H


#include "dyncall_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 
** x86 calling convention calls 
**
** - hybrid return-type call (bool ... pointer)
**
*/

#if defined(DC__OS_Plan9) /* No support for other cconvs on Plan9 and vice-versa. */
void dcCall_x86_plan9        (DCpointer target, DCpointer stackdata, DCsize size);
#else
void dcCall_x86_cdecl        (DCpointer target, DCpointer stackdata, DCsize size);
void dcCall_x86_win32_std    (DCpointer target, DCpointer stackdata, DCsize size);
void dcCall_x86_win32_fast   (DCpointer target, DCpointer stackdata, DCsize size);
void dcCall_x86_win32_msthis (DCpointer target, DCpointer stackdata, DCsize size);
void dcCall_x86_sys_int80h_linux (DCpointer target, DCpointer stackdata, DCsize size);
void dcCall_x86_sys_int80h_bsd   (DCpointer target, DCpointer stackdata, DCsize size);
#endif

#ifdef __cplusplus
}
#endif


#endif /* DYNCALL_CALL_X86_H */
