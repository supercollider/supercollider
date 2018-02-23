/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_thunk_mips64.h
 Description: Thunk - Header for MIPS64
 License:

   Copyright (c) 2016 Tassilo Philipp <tphilipp@potion-studios.com>

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


#ifndef DYNCALL_THUNK_MIPS64_H
#define DYNCALL_THUNK_MIPS64_H

struct DCThunk_
{
  union {
    unsigned short s[26];
    unsigned int   i[13]; /* don't change from 'int', must be 32bit wide */
  } text;
  unsigned int padding; /* 4 bytes, to get struct size to multiple of 8 */
};

#define DCTHUNK_MIPS64_SIZE 56

#endif /* DYNCALL_THUNK_MIPS64_H */

