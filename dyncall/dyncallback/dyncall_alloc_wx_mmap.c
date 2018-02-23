/*

 Package: dyncall
 Library: dyncallback
 File: dyncallback/dyncall_alloc_wx_mmap.c
 Description: Allocate write/executable memory - Implementation for posix
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


#include "dyncall_alloc_wx.h"

#include <sys/types.h>
#include <sys/mman.h>

/* MAP_ANON is not POSIX, if lacking, we'll use a portable fallback on *nix systems */
#if !defined(MAP_ANON)
#  if defined(MAP_ANONYMOUS)
#    define MAP_ANON MAP_ANONYMOUS
#  elif defined(DC_UNIX)
#    include <fcntl.h>
#  else
#    error "Platform implementation missing for anonymous rwx memory"
#  endif
#endif

/* platforms without mprotect */
#if defined(DC__OS_Minix)
#  define NO_MPROTECT
#endif

/* if no mprotect() available/wanted, make mmap alloc pages as rwx */
#if defined(NO_MPROTECT)
#  define MMAP_PROT_INIT (PROT_READ|PROT_WRITE|PROT_EXEC)
#else
#  define MMAP_PROT_INIT (PROT_READ|PROT_WRITE)
#endif

DCerror dcAllocWX(size_t size, void** pp)
{
  void* p;
#if !defined(MAP_ANON) && defined(DC_UNIX)
  /* Hack around not having POSIX' MAP_ANON by going through /dev/zero; store
     file descr to close on dcFreeWX at beginning of memory, as tiny hack */
  int fd = open("/dev/zero", O_RDWR);
  if(fd == -1)
    return -1;
  p = mmap(0, size+sizeof(int), MMAP_PROT_INIT, MAP_PRIVATE, fd, 0);
  if(p == MAP_FAILED) {
    close(fd);
    return -1;
  }
  *(int*)p = fd;
  p += sizeof(int);
#else
  p = mmap(0, size, MMAP_PROT_INIT, MAP_PRIVATE|MAP_ANON, -1, 0);
  if(p == MAP_FAILED)
    return -1;
#endif

  *pp = p;
  return 0;
}

DCerror dcInitExecWX(void* p, size_t size)
{
#if defined(NO_MPROTECT)
  return 0;
#else

#if !defined(MAP_ANON) && defined(DC_UNIX)
  /* Fixup pointer for no-MAP_ANON workaround (see above) */
  p -= sizeof(int);
  size += sizeof(int);
#endif
  return mprotect(p, size, PROT_READ|PROT_EXEC);
#endif
}

void dcFreeWX(void* p, size_t size)
{
#if !defined(MAP_ANON) && defined(DC_UNIX)
  /* Close file descriptor for no-MAP_ANON workaround (see above) */
  p -= sizeof(int);
  size += sizeof(int);
  close(*(int*)p);
#endif
  munmap(p, size);
}

