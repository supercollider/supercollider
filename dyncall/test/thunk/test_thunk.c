/*

 Package: dyncall
 Library: test
 File: test/thunk/test_thunk.c
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

#include "../../dyncall/dyncall.h"
#include "../../dyncallback/dyncall_thunk.h"
#include "../../dyncallback/dyncall_alloc_wx.h"
#include "../common/platformInit.h"
#include "../common/platformInit.c" /* Impl. for functions only used in this translation unit */


/**
 ** test: thunks with several allocation modes
 **       such as on stack, on heap and explicit wx
 **
 **/

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <setjmp.h>
#include <string.h>

jmp_buf jbuf;


void segv_handler(int sig)
{
  longjmp(jbuf, 1);
}

void my_entry(const char* text)
{
  printf("%s: %d\n", text, strcmp(text, "wx") == 0 || strcmp(text, "stack") == 0 || strcmp(text, "heap") == 0); /* @@@ */
}

typedef void (printfun)(const char*);

void test_stack()
{
  DCThunk t;
  printfun* fp;
  dcbInitThunk(&t, &my_entry);
  fp = (printfun*)&t;
  if(setjmp(jbuf) != 0)
    printf("sigsegv\n");
  else
    fp("stack");
}

#include <stdlib.h>

void test_heap()
{
  printfun* fp;
  DCThunk* p = (DCThunk*)malloc(sizeof(DCThunk));
  if(!p) {
    printf("0\n");
    return;
  }
  dcbInitThunk(p, &my_entry);
  fp = (printfun*)p;
  if(setjmp(jbuf) != 0)
    printf("sigsegv\n");
  else
    fp("heap");
  free(p);
}

void test_wx()
{
  DCThunk* p;
  printfun* fp;
  int err = dcAllocWX(sizeof(DCThunk), (void**)&p);
  if(err || !p) {
    printf("0\n");
    return;
  }
  dcbInitThunk(p, &my_entry);
  err = dcInitExecWX((void*)p, sizeof(DCThunk));
  if(err) {
    dcFreeWX((void*)p, sizeof(DCThunk));
    printf("0\n");
    return;
  }
  fp = (printfun*)p;
  if(setjmp(jbuf) != 0)
    printf("sigsegv\n");
  else
    fp("wx");
  dcFreeWX((void*)p, sizeof(DCThunk));
}

int main()
{
  dcTest_initPlatform();

  signal(SIGSEGV, segv_handler);

  printf("Allocating ...\n");
  printf("... W^X memory: ");
  test_wx();
  printf("... heap memory: ");
  test_heap();
  printf("... stack memory: ");
  test_stack();

  dcTest_deInitPlatform();

  return 0;
}

