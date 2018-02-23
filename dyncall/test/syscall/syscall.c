/*

 Package: dyncall
 Library: test
 File: test/syscall/syscall.c
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
#include <sys/syscall.h> 
#include <assert.h>
DCCallVM* callvm;

void syscallvm_init()
{
  callvm = dcNewCallVM(4096);
  dcMode(callvm, DC_CALL_SYS_DEFAULT);
  assert( dcGetError(callvm) == DC_ERROR_NONE );
}

int syscall_write(int fd, char* buf, size_t len)
{
  dcReset(callvm);
  dcArgInt(callvm, fd);
  dcArgPointer(callvm, buf);
  dcArgInt(callvm, len);
  return dcCallInt(callvm, (DCpointer)(ptrdiff_t)SYS_write);
}

int main(int argc, char* argv[])
{
  syscallvm_init();
  syscall_write(1/*stdout*/, "result: syscall: 1\n", 19);
  return 0;
}

