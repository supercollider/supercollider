/*

 Package: dyncall
 Library: test
 File: test/call_suite/globals.c
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

#include <stdlib.h>
#include "globals.h"
#include <float.h>

#define X(CH,T,QCH) T *V_##CH; T *K_##CH; 
DEF_TYPES
#undef X

int       fid;

double rand_d() { return ( ( (double) rand() )  / ( (double) RAND_MAX ) ); }

void init_K()
{
  int i;
#define X(CH,T,QCH) V_##CH = (T*) malloc(sizeof(T)*(G_maxargs+1)); K_##CH = (T*) malloc(sizeof(T)*(G_maxargs+1));
DEF_TYPES
#undef X
  for(i=0;i<G_maxargs+1;++i) {
    K_c[i] = (char)      (((rand_d()-0.5)*2) * (1<<7));
    K_s[i] = (short)     (((rand_d()-0.5)*2) * (1<<(sizeof(short)*8-1)));
    K_i[i] = (int)       (((rand_d()-0.5)*2) * (1<<(sizeof(int)*8-2)));
    K_j[i] = (long)      (((rand_d()-0.5)*2) * (1L<<(sizeof(long)*8-2)));
    K_l[i] = (long long) (((rand_d()-0.5)*2) * (1LL<<(sizeof(long long)*8-2)));
    K_p[i] = (void*)     (long) (((rand_d()-0.5)*2) * (1LL<<(sizeof(void*)*8-1)));
    K_f[i] = (float)     (rand_d() * FLT_MAX);	/* Plan9 doesn't know the macro. */
    K_d[i] = (double)    (((rand_d()-0.5)*2) * 1.7976931348623157E+308/*__DBL_MAX__*/);	/* Plan9 doesn't know the macro. */
  }
}

void clear_V()
{
  int i;
  for(i=0;i<G_maxargs+1;++i) {
#define X(CH,T,QCH) V_##CH[i] = (T) 0;
DEF_TYPES
#undef X
  }
}

