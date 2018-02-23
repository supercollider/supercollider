/*

 Package: dyncall
 Library: dynload
 File: dynload/dynload_windows.c
 Description: 
 License:

   Copyright (c) 2007-2017 Daniel Adler <dadler@uni-goettingen.de>, 
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

  dynload_windows.c

  dynload module for .dll files

*/


#include "dynload.h"

#include <windows.h>


DLLib* dlLoadLibrary(const char* libPath)
{
  return (DLLib*)(libPath != NULL ? LoadLibraryA(libPath) : GetModuleHandle(NULL));
}


void* dlFindSymbol(DLLib* pLib, const char* pSymbolName)
{
  return (void*)GetProcAddress((HINSTANCE)pLib, pSymbolName);
}


void dlFreeLibrary(DLLib* pLib)
{
  FreeLibrary((HINSTANCE)pLib);
}


int dlGetLibraryPath(DLLib* pLib, char* sOut, int bufSize)
{
  return GetModuleFileNameA((HMODULE)pLib, sOut, bufSize)+1; /* strlen + '\0' */
}

