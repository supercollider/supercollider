/*

 Package: dyncall
 Library: test
 File: test/nm/nm.c
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



#include "../../dynload/dynload.h"
#include "../common/platformInit.h"

void list_syms(const char* filePath)
{
  DLSyms* pSyms;
  int i,n;
  
  pSyms = dlSymsInit(filePath);
  if (!pSyms) {
    fprintf(stderr, "dlSymsInit fail.");
    return; 
  }
  i = 0, n = dlSymsCount(pSyms);

  for (; i < n; ++i) {
    const char* name = dlSymsName(pSyms,i);
    printf("%s\n", name);
  }
  dlSymsCleanup(pSyms);
}


int main(int argc, char* argv[])
{
  int i, n;
  DLLib* pLib;
  const char* libPath;

  if (argc == 1) {
    fprintf(stderr, "usage : %s <dllpath>\n", argv[0]);
    return 1;
  }

  libPath = argv[1];

  /* load lib */

  pLib = dlLoadLibrary(libPath);

  if (!pLib) {
    fprintf(stderr, "unable to open library %s\n", libPath);
    return 2;
  }

  list_syms(libPath);

  dlFreeLibrary(pLib);
  return 0;
}

