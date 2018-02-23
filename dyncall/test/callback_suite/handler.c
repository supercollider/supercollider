/*

 Package: dyncall
 Library: test
 File: test/callback_suite/handler.c
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

#include "_auto_config.h"
#include <assert.h>
#include "dyncall_callback.h"
#include "env.h"
#include "signature_utils.h"

char handler(DCCallback* that, DCArgs* input, DCValue* output, void* userdata)
{
  const char* signature = (const char*) userdata;
  int pos = 0; 
  char ch;

  signature = SignatureSkipCallPrefix(signature);

  for(;;) {
    ch = *signature++;
    if (ch == DC_SIGCHAR_ENDARG) break;
    Args[pos].L = 0xDEADC0DECAFEBABELL;
    switch(ch) {
      case DC_SIGCHAR_BOOL:     Args[pos].B = dcbArgBool     (input); break;
      case DC_SIGCHAR_CHAR:     Args[pos].c = dcbArgChar     (input); break;
      case DC_SIGCHAR_UCHAR:    Args[pos].C = dcbArgUChar    (input); break;
      case DC_SIGCHAR_SHORT:    Args[pos].s = dcbArgShort    (input); break;
      case DC_SIGCHAR_USHORT:   Args[pos].S = dcbArgUShort   (input); break;
      case DC_SIGCHAR_INT:      Args[pos].i = dcbArgInt      (input); break;
      case DC_SIGCHAR_UINT:     Args[pos].I = dcbArgUInt     (input); break;
      case DC_SIGCHAR_LONG:     Args[pos].j = dcbArgLong     (input); break;
      case DC_SIGCHAR_ULONG:    Args[pos].J = dcbArgULong    (input); break;
      case DC_SIGCHAR_LONGLONG: Args[pos].l = dcbArgLongLong (input); break;
      case DC_SIGCHAR_ULONGLONG:Args[pos].L = dcbArgULongLong(input); break;
      case DC_SIGCHAR_FLOAT:    Args[pos].f = dcbArgFloat    (input); break; 
      case DC_SIGCHAR_DOUBLE:   Args[pos].d = dcbArgDouble   (input); break;
      case DC_SIGCHAR_POINTER:  Args[pos].p = dcbArgPointer  (input); break;
    }
    ++pos;
  }

  ch = *signature++;

  /* currently, no void result is supported by the suite */
  GetReferenceResult(output, ch);

  switch(ch) {
    case DC_SIGCHAR_BOOL:     return 'B';
    case DC_SIGCHAR_CHAR:     return 'c';
    case DC_SIGCHAR_UCHAR:    return 'C';
    case DC_SIGCHAR_SHORT:    return 's';
    case DC_SIGCHAR_USHORT:   return 'S';
    case DC_SIGCHAR_INT:      return 'i';
    case DC_SIGCHAR_UINT:     return 'I';
    case DC_SIGCHAR_LONG:     return 'j'; 
    case DC_SIGCHAR_ULONG:    return 'J'; 
    case DC_SIGCHAR_LONGLONG: return 'l';
    case DC_SIGCHAR_ULONGLONG:return 'l';
    case DC_SIGCHAR_FLOAT:    return 'f';
    case DC_SIGCHAR_DOUBLE:   return 'd';
    case DC_SIGCHAR_POINTER:  return 'p';
    default: assert(0); return 'v';
  }
}

