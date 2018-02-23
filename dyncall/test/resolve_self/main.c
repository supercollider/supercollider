/*

 Package: dyncall
 Library: test
 File: test/resolve_self/main.c
 Description: 
 License:

   Copyright (c) 2011-2017 Daniel Adler <dadler@uni-goettingen.de>,
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

#include "../../dynload/dynload.h"
#include "../../dyncall/dyncall_macros.h"
#include <assert.h>
#include "../common/platformInit.h"

#ifdef DC_WINDOWS
#define DLL_EXPORT __declspec( dllexport )
#else
#define DLL_EXPORT
#endif

DLL_EXPORT double add_dd_d(double x, double y);

double add_dd_d(double x, double y) 
{
  return x+y;
}

int main(int argc, char* argv[])
{
  void* address;
  double result;
  int status;
  DLSyms* pSyms;
  DLLib* pLib = dlLoadLibrary(NULL);
  assert(pLib);
  printf("self loaded at %p\n", pLib);

  address = dlFindSymbol(pLib, "add_dd_d");
  if(address) {
    printf("address of function add_dd_d at %p\n", address);
    result = ( (double (*) (double,double) ) address ) (20.0, 3.0);
    status = (result == 23);
  } else {
    printf("can't resolve address of add_dd_d, it doesn't seem to be a *dynamic* symbol\n");
    status = 0;
  }

  dlFreeLibrary(pLib);

  /*pSyms = dlSymsInit(NULL);
  printf("syms handle: %p\n", pSyms);
  dlSymsCleanup(pSyms);*/

  printf("result: resolve_self: %d\n", status);
  return 0;
}

