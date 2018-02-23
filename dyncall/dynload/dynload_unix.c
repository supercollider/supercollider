/*

 Package: dyncall
 Library: dynload
 File: dynload/dynload_unix.c
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

  dynload_unix.c

  dynload module for .so (unix) and .dylib (mach-o darwin/OS X) files

*/


#include "dynload.h"
#include "../autovar/autovar_OS.h"

#include <string.h>

#if defined(__GLIBC__) /* to access dlinfo */
#  define _GNU_SOURCE
#  define __USE_GNU
#endif
#include <dlfcn.h>


DLLib* dlLoadLibrary(const char* libPath)
{
  return (DLLib*)dlopen(libPath, RTLD_NOW|RTLD_GLOBAL); //@@@ should use RTLD_LAZY, maybe?
}


void* dlFindSymbol(DLLib* pLib, const char* pSymbolName)
{
  return dlsym((void*)pLib, pSymbolName);
}


void dlFreeLibrary(DLLib* pLib)
{
  /* Check for NULL for cross-platform consistency. *BSD seems to do that in
  dlclose, Linux does not. POSIX states "if handle does not refer to an open
  object, dlclose() returns a non-zero value", which unfortunately sounds
  like it's not explicitly specified. */
  if(pLib)
    dlclose((void*)pLib);
}



/* for dlopen-based dlGetLibraryPath impls below, prefer RTLD_NOLOAD */
/* that merely checks lib names */
#if defined(RTLD_NOLOAD)
#  define RTLD_LIGHTEST RTLD_NOLOAD
#else
#  define RTLD_LIGHTEST RTLD_LAZY
#endif


/* code for dlGetLibraryPath is platform specific - if dlinfo() exists use */
/* that: check for RTLD_DI_LINKMAP (#define for dlinfo()), or if GNU C Lib */
/* is used (where RTLD_DI_LINKMAP is an enum), or by OS (dlinfo comes from */
/* Solaris), etc. */
#if defined(RTLD_DI_LINKMAP) || defined(OS_SunOS) || defined(__GLIBC__) /* @@@ dlinfo() was introduced in glibc 2.3.3 (in 2003), somehow check for that, also */

#include <link.h>

int dlGetLibraryPath(DLLib* pLib, char* sOut, int bufSize)
{
  struct link_map* p;
  int l = -1;
  if(dlinfo(pLib, RTLD_DI_LINKMAP, &p) == 0) {
    l = strlen(p->l_name);
    if(l < bufSize) /* l+'\0' <= bufSize */
      strcpy(sOut, p->l_name);
  }
  return l+1; /* strlen + '\0' */
}


/* specific implementation needed on Darwin -----> */
#elif defined(OS_Darwin)

#include <stdint.h>
#include <mach-o/dyld.h>

int dlGetLibraryPath(DLLib* pLib, char* sOut, int bufSize)
{
  uint32_t i;
  int l = -1;

  /*if(pLib == RTLD_DEFAULT)
    return NULL; @@@ return exec's path */

  /* Darwin's code doesn't come with (non-standard) dlinfo(), so use dyld(1) */
  /* code. There doesn't seem to be a direct way to query the library path,  */
  /* so "double-load" temporarily all already loaded images (just increases  */
  /* ref count) and compare handles until we found ours. Return the name.    */
  for(i=_dyld_image_count(); i>0;) /* iterate libs from end, more likely ours */
  {
    const char* libPath = _dyld_get_image_name(--i);
    void* lib = dlopen(libPath, RTLD_LIGHTEST);
    if(lib) {
      dlclose(lib);
      /* compare handle pointers' high bits (in low 2 bits some flags might */
      /* be stored - should be safe b/c address needs alignment, anywas) */
      if(((intptr_t)pLib ^ (intptr_t)lib) < 4) {
        l = strlen(libPath);
        if(l < bufSize) /* l+'\0' <= bufSize */
          strcpy(sOut, libPath);
        break;
      }
    }
  }

  return l+1; /* strlen + '\0' */
}


/* OpenBSD >= 3.7 has dl_iterate_phdr(), use it if not explicitly requesting */
/* to use dladdr()-based guessing (set by configure) -----> */
#elif defined(OS_OpenBSD) && !defined(DL_DLADDR_TO_LIBPATH)

#include <sys/types.h>
#include <link.h>

typedef struct {
  DLLib* pLib;
  char*  sOut;
  int    bufSize;
} iter_phdr_data;

static int iter_phdr_cb(struct dl_phdr_info* info, size_t size, void* data)
{
  int l = -1;
  iter_phdr_data* d = (iter_phdr_data*)data;
  /* unable to relate info->dlpi_addr directly to our dlopen handle, let's */
  /* do what we do on macOS above, re-dlopen the already loaded lib (just  */
  /* increases ref count) and compare handles. */
  void* lib = dlopen(info->dlpi_name, RTLD_LIGHTEST);
  if(lib) {
    dlclose(lib);
    if(lib == (void*)d->pLib) {
      l = strlen(info->dlpi_name);
      if(l < d->bufSize) /* l+'\0' <= bufSize */
        strcpy(d->sOut, info->dlpi_name);
    }
  }
  return l+1; /* strlen + '\0'; is 0 if lib not found, which continues iter */
}

int dlGetLibraryPath(DLLib* pLib, char* sOut, int bufSize)
{
  iter_phdr_data d = { pLib, sOut, bufSize };
  return dl_iterate_phdr(iter_phdr_cb, &d);
}


/* fallback to dladdr() hack */
#else

/* if nothing else is available, fall back to guessing using dladdr() - this */
/* might not always work, as it's trying to getit via the _fini() symbol,    */
/* which is usually defined in ELF files, but not guaranteed                 */

/* @@@Note: On some platforms this might be improved, e.g. on BeOS we have */
/* lt_dlgetinfo, which requires iterating over ltdl stuff, but was unable  */
/* to get that to work (would also introduce a link dependency on libltdl) */

int dlGetLibraryPath(DLLib* pLib, char* sOut, int bufSize)
{
  /* cross fingers that shared object is standard ELF and look for _fini */
  int l = -1;
  void* s = dlsym((void*)pLib, "_fini");
  if(s) {
    Dl_info i;
    if(dladdr(s, &i) != 0) {
      l = strlen(i.dli_fname);
      if(l < bufSize) /* l+'\0' <= bufSize */
        strcpy(sOut, i.dli_fname);
    }
  }
  return l+1; /* strlen + '\0' */
}

#endif

