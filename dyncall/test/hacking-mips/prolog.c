/*

 Package: dyncall
 Library: test
 File: test/hacking-mips/prolog.c
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

#define DEF_CALL1(RT) \
typedef RT (*f_##RT) (i,i,i,i,i,i,i,i); \
RT call_##RT(void* target,void* regdata, int* data, int size) \
{ \
  f_##RT f = (f_##RT) target; \
  return f(data[0],data[1],data[2],data[3],data[4],data[5],data[6],data[7]); \
}

#define DEF_CALL(RT) \
typedef RT (*f_##RT) (i,i,i,i,i,i,i,i); \
RT call_##RT(void* target,void* regdata, int* data, int size) \
{ \
  f_##RT f = (f_##RT) target; \
  return f(); \
}

typedef long long llong;
typedef long double ldouble;

DEF_CALL(int)
DEF_CALL(float)
DEF_CALL(double)
DEF_CALL(llong)
DEF_CALL(ldouble)
DEF_CALL(void)

