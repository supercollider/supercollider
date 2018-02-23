/*

 Package: dyncall
 Library: test
 File: test/sign/sign.c
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


#include "dyncall.h"
#include "../common/platformInit.h"

// This program demonstrates the need for 'unsigned' integers and the
// ability to implement zero/sign extensions for small integers.
// This test program indicates failure on powerpc 32-bit with
// compiler optimizations enabled (e.g. '-O2' compilation flags).
//
// A fix to be incorporated in the API is also given.
//

int add1s(unsigned short x) { return x + 1; }

// API BUGFIX:
#define dcArgUShort(vm,x) dcArgInt(vm,(int)(unsigned int)(x))

// OLD TEST: int add1(unsigned char x) { return x + 1; }

int main(int argc, char* argv[])
{
  DCCallVM* vm;
  int result;
  int total;
  vm = dcNewCallVM(4096);
  dcReset(vm);

  dcArgShort(vm, 0xFFFF );
  result = dcCallInt( vm, &add1s );
  total = (result == 0x10000);
  printf("result: sign: %d\n", total); 
 
  if (!total) {
    //
    // TEST BUGFIX: use instead..
    //
    dcReset(vm);
    dcArgUShort( vm, 0xFFFF );
    result = dcCallInt( vm, &add1s );
    total = (result == 0x10000);
    printf("result: sign (bugfix): %d\n", total); 
  }
  
  // result = dcCallInt( vm, &add1s );
  // total = (result == 0x10000);
  
  // OLD TEST: updated to using 'short'
  // dcArgChar( vm, (char) 255 );
  // result = dcCallInt( vm, &add1 );
  // total = (result == 256);
  
  // printf("result: sign: %d\n", total); 
  return 0;
}
