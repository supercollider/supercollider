/*

 Package: dyncall
 Library: test
 File: test/callback_suite/do_test.c
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

#include <assert.h>
#include "_auto_config.h"
#include "invokers.h"
#include "dyncall_callback.h"
#include "sigstrings.h"
#include "signature_utils.h"
#include "env.h"
#include "print.h"
#include "../common/platformInit.h"

int CompareValues(char type, DCValue* a, DCValue* b)
{
  int isequal = 0;
  switch(type) 
  {
    case DC_SIGCHAR_BOOL:      isequal = (a->B == b->B) ? 1 : 0 ; break;
    case DC_SIGCHAR_CHAR:      isequal = (a->c == b->c) ? 1 : 0 ; break;
    case DC_SIGCHAR_UCHAR:     isequal = (a->C == b->C) ? 1 : 0 ; break;
    case DC_SIGCHAR_SHORT:     isequal = (a->s == b->s) ? 1 : 0 ; break;
    case DC_SIGCHAR_USHORT:    isequal = (a->S == b->S) ? 1 : 0 ; break;
    case DC_SIGCHAR_INT:       isequal = (a->i == b->i) ? 1 : 0 ; break;
    case DC_SIGCHAR_UINT:      isequal = (a->I == b->I) ? 1 : 0 ; break;
    case DC_SIGCHAR_LONG:      isequal = (a->j == b->j) ? 1 : 0 ; break;
    case DC_SIGCHAR_ULONG:     isequal = (a->J == b->J) ? 1 : 0 ; break;
    case DC_SIGCHAR_LONGLONG:  isequal = (a->l == b->l) ? 1 : 0 ; break;
    case DC_SIGCHAR_ULONGLONG: isequal = (a->L == b->L) ? 1 : 0 ; break;
    case DC_SIGCHAR_FLOAT:     isequal = (a->f == b->f) ? 1 : 0 ; break;
    case DC_SIGCHAR_DOUBLE:    isequal = (a->d == b->d) ? 1 : 0 ; break;
    case DC_SIGCHAR_POINTER:   isequal = (a->p == b->p) ? 1 : 0 ; break;
    default: assert(0);
  }
  return isequal;
}

int Compare(const char* signature)
{
  DCValue ref;
  int total = 1;
  int pos;
  int isequal;
  char ch;

  /* skip prefix */

  signature = SignatureSkipCallPrefix(signature);

  /* check arguments */

  pos = 0;

  for(;;) {

    ch  = *signature++;
    
    if (ch == DC_SIGCHAR_ENDARG) break;
    GetReferenceArg(&ref, ch, pos);
    isequal = CompareValues( ch, &ref, &Args[pos] );
    if ( !isequal ) {
      if (OptionVerbose) { total = 0; fprintf(stdout, " @%d[%c] ", pos, ch); }
      else return 0;
    }
    ++ pos;
  }
    
  ch  = *signature++;

  /* check result */

  GetReferenceResult(&ref, ch);

  isequal = CompareValues(ch, &ref, &Result);
  if (!isequal) {
    if (OptionVerbose) { total = 0; fprintf(stdout, " @-1 "); }
    else return 0;
  }

  return total;
}


extern DCCallbackHandler handler; /* see handler.c for implementation */

int DoTest(int id)
{
  int index, result;
  const char* signature;
  DCCallback* pcb;

  assert( id > 0 && id <= CONFIG_NSIGS );
  index = id - 1;
  
  signature = GetSignature(index);
  PrintCaseInfo(id,signature);

  pcb = dcbNewCallback( signature, handler, (void*) signature );
  assert(pcb != NULL);
  DoInvoke(index, (void*) pcb);
  result = Compare(signature); 
  PrintCaseResult(result);
  dcbFreeCallback(pcb);
  return result;
}

