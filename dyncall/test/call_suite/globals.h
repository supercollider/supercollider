/*

 Package: dyncall
 Library: test
 File: test/call_suite/globals.h
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


#define DEF_TYPES X(c,char,'c') X(s,short,'s') X(i,int,'i') X(j,long,'j') X(l,long long,'l') X(p,void*,'p') X(f,float,'f') X(d,double,'d')

#define X(CH,T,QCH) extern T *K_##CH; extern T *V_##CH;
DEF_TYPES
#undef X

typedef void (*funptr)();

extern void       * G_callvm;
extern funptr       G_funtab[];
extern char const * G_sigtab[];
extern int          G_ncases;
extern int          G_maxargs;

void init_K();
void init_T();
void clear_V();
int  invoke(char const* signature, void* target);

#if 0
extern const char      *K_c;
extern const short     *K_s;
extern const int       *K_i;
extern const long      *K_j;
extern const long long *K_l;
extern void* const     *K_p;
extern const float     *K_f;
extern const double    *K_d;

extern char      *V_c;
extern short     *V_s;
extern int       *V_i;
extern long      *V_j;
extern long long *V_l;
extern void*     *V_p;
extern float     *V_f;
extern double    *V_d;
#endif 

