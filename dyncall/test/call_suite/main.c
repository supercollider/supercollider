/*

 Package: dyncall
 Library: test
 File: test/call_suite/main.c
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
#include "globals.h"
#include <string.h>
#include "../common/platformInit.h"
#include "../common/platformInit.c" /* Impl. for functions only used in this translation unit */


char  linebuf[1024];
void* G_callvm;
  
int run_test(int i) {  
  char const * sig;
  void * target;
  int success;
  sig = G_sigtab[i];
  target = (void*) G_funtab[i];
  printf("%d:%s:",i,sig);
  success = invoke(sig,target);
  printf("%d\n",success);
  return success;
}

int run_all() {
  int i;
  int failure = 0;
  for(i=0;i<G_ncases;++i) {
    failure |= !( run_test(i) );
  }
  return !failure;
}

int main(int argc, char* argv[])
{
  int total;

  dcTest_initPlatform();

  init_K(G_maxargs);
  G_callvm = (DCCallVM*) dcNewCallVM(4096);
  dcReset(G_callvm);
  total = run_all();
  printf("result: call_suite: %d\n", total);

  dcTest_deInitPlatform();

  return 0;
}

