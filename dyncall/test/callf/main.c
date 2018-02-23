/*

 Package: dyncall
 Library: test
 File: test/callf/main.c
 Description: 
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



/* test dcCallF API */

#include "../../dyncall/dyncall_callf.h"
#include "../common/platformInit.h"
#include "../common/platformInit.c" /* Impl. for functions only used in this translation unit */


/* sample void function */

int vf_iii(int x,int y,int z)
{
  int r = (x == 1 && y == 2 && z == 3);
  printf("%d %d %d: %d", x, y, z, r);
  return r;
}

int vf_ffiffiffi(float a, float b, int c, float d, float e, int f, float g, float h, int i)
{
  int r = (a == 1.f && b == 2.f && c == 3 && d == 4.f && e == 5.f && f == 6 && g == 7.f && h == 8.f && i == 9);
  printf("%f %f %d %f %f %d %f %f %d: %d", a, b, c, d, e, f, g, h, i, r);
  return r;
}

/* main */

int main(int argc, char* argv[])
{
  DCCallVM* vm;
  DCValue ret;
  int r = 1;

  dcTest_initPlatform();

  /* allocate call vm */
  vm = dcNewCallVM(4096);


  /* calls using 'formatted' API */
  dcReset(vm);
  printf("callf iii)i:       ");
  dcCallF(vm, &ret, (void*)&vf_iii, "iii)i", 1, 2, 3);
  r = ret.i && r;

  dcReset(vm);
  printf("\ncallf ffiffiffi)i: ");
  dcCallF(vm, &ret, (void*)&vf_ffiffiffi, "ffiffiffi)i", 1.f, 2.f, 3, 4.f, 5.f, 6, 7.f, 8.f, 9);
  r = ret.i && r;


  /* arg binding then call using 'formatted' API */
  dcReset(vm);
  printf("\nargf iii)i       then call: ");
  dcArgF(vm, "iii)i", 1, 2, 3);
  r = r && dcCallInt(vm, (void*)&vf_iii);

  dcReset(vm);
  printf("\nargf iii         then call: ");
  dcArgF(vm, "iii", 1, 2, 3);
  r = r && dcCallInt(vm, (void*)&vf_iii);

  dcReset(vm);
  printf("\nargf ffiffiffi)i then call: ");
  dcArgF(vm, "ffiffiffi)i", 1.f, 2.f, 3, 4.f, 5.f, 6, 7.f, 8.f, 9);
  r = r && dcCallInt(vm, (void*)&vf_ffiffiffi);

  dcReset(vm);
  printf("\nargf ffiffiffi   then call: ");
  dcArgF(vm, "ffiffiffi", 1.f, 2.f, 3, 4.f, 5.f, 6, 7.f, 8.f, 9);
  r = r && dcCallInt(vm, (void*)&vf_ffiffiffi);


  /* free vm */
  dcFree(vm);

  printf("\nresult: callf: %d\n", r);

  dcTest_deInitPlatform();
  
  return 0;
}

