/*

 Package: dyncall
 Library: dynload
 File: dynload/dynload_syms_elf.c
 Description: 
 License:

   Copyright (c) 2007-2017 Daniel Adler <dadler@uni-goettingen.de>, 
                           Tassilo Philipp <tphilipp@potion-studios.com>,
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



#include "../autovar/autovar_OS.h"

/*
 
 dynamic symbol resolver for elf

*/

#include "dynload.h"
#if defined(OS_OpenBSD)
#  include <stdint.h>
#  include <elf_abi.h>
#elif defined(OS_NetBSD)
#  include <stddef.h>
#  include <elf.h>
#elif defined(OS_SunOS)
#  include <libelf.h>
#elif defined(OS_BeOS)
#  include <elf32.h>
#else
#  include <elf.h>
#endif

#if defined(__GLIBC__)
#  define _GNU_SOURCE
#  define __USE_GNU
#endif

#include "dynload_alloc.h"

#include <assert.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h> 

/* run-time configuration 64/32 */
#if defined(OS_OpenBSD)
#else 
#  include "../autovar/autovar_ABI.h"
#  ifdef ABI_ELF64

typedef Elf64_Ehdr   Elf_Ehdr;
typedef Elf64_Phdr   Elf_Phdr;
typedef Elf64_Shdr   Elf_Shdr;
typedef Elf64_Sym    Elf_Sym;
#    ifndef OS_SunOS
typedef Elf64_Dyn    Elf_Dyn;
#    endif
typedef Elf64_Sxword Elf_tag;
typedef Elf64_Addr   Elf_Addr;

#  else
#    if defined(OS_BeOS)
typedef struct Elf32_Ehdr   Elf_Ehdr;
typedef struct Elf32_Phdr   Elf_Phdr;
typedef struct Elf32_Shdr   Elf_Shdr;
typedef struct Elf32_Sym    Elf_Sym;
typedef struct Elf32_Dyn    Elf_Dyn;
typedef        Elf32_Sword  Elf_tag;
typedef        Elf32_Addr   Elf_Addr;

#    else

typedef Elf32_Ehdr   Elf_Ehdr;
typedef Elf32_Phdr   Elf_Phdr;
typedef Elf32_Shdr   Elf_Shdr;
typedef Elf32_Sym    Elf_Sym;
#      ifndef OS_SunOS
typedef Elf32_Dyn    Elf_Dyn;
#      endif
typedef Elf32_Sword  Elf_tag;
typedef Elf32_Addr   Elf_Addr;

#    endif
#  endif
#endif


struct DLSyms_
{
  const char* pStrTab;
  Elf_Sym*    pSymTab;
  size_t      strTabSize;
  size_t      nSymbols;
  Elf_Ehdr*   pElf_Ehdr;	/* pointer to elf header */
  int         file;			/* fd of lib */
  size_t      fileSize;	    /* filesize of open lib */
};


DLSyms* dlSymsInit(const char* libPath)
{
  unsigned char* pMem;
  int i;
  struct stat st;
  Elf_Shdr* pS;
  DLSyms* pSyms;

  if(stat(libPath, &st) == -1)
    return NULL;

  i = open(libPath, O_RDONLY);
  if(i == -1)
    return NULL;

  pSyms = (DLSyms*)dlAllocMem(sizeof(DLSyms));
  memset(pSyms, 0, sizeof(DLSyms));
  pSyms->file = i;
  pSyms->fileSize = st.st_size;
  pSyms->pElf_Ehdr = (Elf_Ehdr*) mmap((void*) NULL, pSyms->fileSize, PROT_READ, MAP_SHARED, pSyms->file, 0);

#ifdef ABI_ELF32
  assert(pSyms->pElf_Ehdr->e_ident[EI_CLASS] == ELFCLASS32);
#else
  assert(pSyms->pElf_Ehdr->e_ident[EI_CLASS] == ELFCLASS64);
#endif

  assert(pSyms->pElf_Ehdr->e_phoff > 0);
  assert(pSyms->pElf_Ehdr->e_shoff > 0);


  pMem = (unsigned char*)pSyms->pElf_Ehdr;
  
  /* traverse section headers */
  pS = (Elf_Shdr*) ( pMem + pSyms->pElf_Ehdr->e_shoff );
  /* skip section 0 which is always zero due to the Elf standard. */
  for (i = 1; i < pSyms->pElf_Ehdr->e_shnum; i++) 
  {
    Elf_Shdr* pSection = &pS[i];
    void* pSectionContent = ((char*)pMem) + pSection->sh_offset;
    switch (pSection->sh_type)
    {
      case SHT_DYNSYM:
        if (!pSyms->pSymTab) {
          pSyms->pSymTab  = (Elf_Sym*)pSectionContent;
          pSyms->nSymbols = pSection->sh_size / pSection->sh_entsize;
        }
        break;
      case SHT_STRTAB:
        /* Do not trust pSyms->pElf_Ehdr->e_shstrndx! */
        if (!pSyms->pStrTab) {
          pSyms->pStrTab  = (const char*)pSectionContent;
          pSyms->strTabSize = pSection->sh_size;
        }
        break;
    }
    if (pSyms->pSymTab && pSyms->pStrTab)
      break;
  }
  return pSyms;
}


void dlSymsCleanup(DLSyms* pSyms)
{
  if(pSyms) {
    munmap((void*) pSyms->pElf_Ehdr, pSyms->fileSize);
    close(pSyms->file);
    dlFreeMem(pSyms);
  }
}


int dlSymsCount(DLSyms* pSyms)
{
  return pSyms ? pSyms->nSymbols : 0;
}


const char* dlSymsName(DLSyms* pSyms, int index)
{
  int str_index;
  if(!pSyms || !pSyms->pSymTab || index < 0 || index >= pSyms->nSymbols)
    return NULL;

  str_index = pSyms->pSymTab[index].st_name;
  if (str_index < 0 || str_index >= pSyms->strTabSize)
    return NULL;
  return &pSyms->pStrTab[str_index];
}


const char* dlSymsNameFromValue(DLSyms* pSyms, void* value)
{
  Dl_info info;
  return (dladdr(value, &info) && (value == info.dli_saddr))
    ? info.dli_sname
    : NULL;
}

