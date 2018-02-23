/*

 Package: dyncall
 Library: test
 File: test/call_suite/invoke.c
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
#include "globals.h"
#include "../common/platformInit.h"

int invoke(char const* signature, void* t)
{
  DCCallVM   * p = (DCCallVM*) G_callvm;
  char const * sig = signature;
  char         rtype;
  char         atype;
  int          pos = 0;
  int          s = 0;

  clear_V();
  
  rtype = *sig++;
  dcReset(p);

  while ( (atype = *sig++) != '\0') {
    pos++;
    switch(atype) {
      case 'c': dcArgChar    (p,K_c[pos]); break;
      case 's': dcArgShort   (p,K_s[pos]); break;
      case 'i': dcArgInt     (p,K_i[pos]); break;
      case 'j': dcArgLong    (p,K_j[pos]); break;
      case 'l': dcArgLongLong(p,K_l[pos]); break;
      case 'p': dcArgPointer (p,K_p[pos]); break;
      case 'f': dcArgFloat   (p,K_f[pos]); break;
      case 'd': dcArgDouble  (p,K_d[pos]); break;
      default: printf("unknown atype '%c' (1) ;", atype); return 0;
    }
  }
  
  switch(rtype) 
  {
    case 'v': dcCallVoid(p,t); s=1; /*TODO:check that no return-arg was touched.*/ break;
    case 'c': s = (dcCallChar    (p,t) == K_c[pos]) ; break;
    case 's': s = (dcCallShort   (p,t) == K_s[pos]) ; break;
    case 'i': s = (dcCallInt     (p,t) == K_i[pos]) ; break;
    case 'j': s = (dcCallLong    (p,t) == K_j[pos]) ; break;
    case 'l': s = (dcCallLongLong(p,t) == K_l[pos]) ; break;
    case 'p': s = (dcCallPointer (p,t) == K_p[pos]) ; break;
    case 'f': s = (dcCallFloat   (p,t) == K_f[pos]) ; break;
    case 'd': s = (dcCallDouble  (p,t) == K_d[pos]) ; break;
    default: printf("unknown rtype '%c'", rtype); return 0;
  }

  if (!s) { printf("rval wrong;"); return 0; }
  /* test: */
  sig = signature+1;
  pos = 1;
  while ( (atype = *sig++) != '\0') {
    switch(atype) {
#if 0
#define X(CH,T,QCH) case QCH: s = (V_##CH[pos] == K_##CH[pos]); break;
DEF_TYPES
#undef X
#endif
      case 'c': s = ( V_c[pos] == K_c[pos] ); if (!s) printf("'c':%d: %d != %d ; ", pos, V_c[pos], K_c[pos]); break;
      case 's': s = ( V_s[pos] == K_s[pos] ); if (!s) printf("'s':%d: %d != %d ; ", pos, V_s[pos], K_s[pos]); break;
      case 'i': s = ( V_i[pos] == K_i[pos] ); if (!s) printf("'i':%d: %d != %d ; ", pos, V_i[pos], K_i[pos]); break;
      case 'j': s = ( V_j[pos] == K_j[pos] ); if (!s) printf("'j':%d: %ld != %ld ; ", pos, V_j[pos], K_j[pos]); break;
      case 'l': s = ( V_l[pos] == K_l[pos] ); if (!s) printf("'l':%d: %lld != %lld ; ", pos, V_l[pos], K_l[pos]); break;
      case 'p': s = ( V_p[pos] == K_p[pos] ); if (!s) printf("'p':%d: %lld != %lld ; ", pos, (long long) V_p[pos], (long long) K_p[pos]); break;
      case 'f': s = ( V_f[pos] == K_f[pos] ); if (!s) printf("'f':%d: %f != %f ; ", pos, V_f[pos], K_f[pos]); break;
      case 'd': s = ( V_d[pos] == K_d[pos] ); if (!s) printf("'d':%d: %f != %f ; ", pos, V_d[pos], K_d[pos]); break;
      default: printf("unknown atype '%c' ; ", atype); return 0;
    }
    if (!s) {
      printf("arg mismatch at %d ; ", pos);
      return 0;
    }
    pos++;
  }
  return 1;
}

