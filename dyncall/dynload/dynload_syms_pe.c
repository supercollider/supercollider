/*

 Package: dyncall
 Library: dynload
 File: dynload/dynload_syms_pe.c
 Description:
 License:

   Copyright (c) 2007-2017 Daniel Adler <dadler@uni-goettingen.de>,
                           Tassilo Philipp <tphilipp@potion-studios.com>
                           Olivier Chafik <olivier.chafik@gmail.com>

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



#include "dynload.h"
#include "dynload_alloc.h"

#include <windows.h>

struct DLSyms_
{
  DLLib*                pLib;
  const char*           pBase;
  const DWORD*          pNames;
  const DWORD*          pFuncs;
  const unsigned short* pOrds;
  size_t                count;
};


DLSyms* dlSymsInit(const char* libPath)
{
  DLLib*                  pLib;
  DLSyms*                 pSyms;
  IMAGE_DOS_HEADER*       pDOSHeader;
  IMAGE_NT_HEADERS*       pNTHeader;
  IMAGE_DATA_DIRECTORY*   pExportsDataDir;
  IMAGE_EXPORT_DIRECTORY* pExports;
  const char*             base;

  pLib = dlLoadLibrary(libPath);
  if(!pLib)
    return NULL;

  base            = (const char*)pLib;
  pDOSHeader      = (IMAGE_DOS_HEADER*)base;
  pNTHeader       = (IMAGE_NT_HEADERS*)(base + pDOSHeader->e_lfanew);
  pExportsDataDir = &pNTHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
  pExports        = (IMAGE_EXPORT_DIRECTORY*)(base + pExportsDataDir->VirtualAddress);

  pSyms         = (DLSyms*)dlAllocMem(sizeof(DLSyms));
  pSyms->pBase  = base;
  pSyms->pNames = (DWORD*)(base + pExports->AddressOfNames);
  pSyms->pFuncs = (DWORD*)(base + pExports->AddressOfFunctions);
  pSyms->pOrds  = (unsigned short*)(base + pExports->AddressOfNameOrdinals);
  pSyms->count  = (size_t)pExports->NumberOfNames;
  pSyms->pLib   = pLib;

  return pSyms;
}


void dlSymsCleanup(DLSyms* pSyms)
{
  if(pSyms) {
    dlFreeLibrary(pSyms->pLib);
    dlFreeMem(pSyms);
  }
}


int dlSymsCount(DLSyms* pSyms)
{
  return (int)pSyms->count;
}


const char* dlSymsName(DLSyms* pSyms, int index)
{
  return pSyms->pBase + pSyms->pNames[index];
}


const char* dlSymsNameFromValue(DLSyms* pSyms, void* value)
{
  int i, c=dlSymsCount(pSyms);
  for(i=0; i<c; ++i)
  {
    if((void*)(pSyms->pBase + pSyms->pFuncs[pSyms->pOrds[i]]) == value)
      return dlSymsName(pSyms, i);
  }

  /* Not found. */
  return NULL;
}

