/*

 Package: dyncall
 Library: test
 File: test/hacking-mips/main.c
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

#include "common.h"
#include <assert.h>

#define v0 0
#define v1 1
#define v2 2
#define v3 3
#define v4 4
#define v5 5

void ext()
{
}

int main(int argc, char* argv[])
{
  int result;
  call_f0();
  call_f4();
  call_f8();
  n0();
  n8(1,2,3,4,5,6,7,8);
  result = dispatch(&f,v0,v1,v2,v3,v4,v5);
  assert(result == v0+v1+v2+v3+v4+v5+v0+v1+v2 );
  return 0;
}

