/*

 Package: dyncall
 Library: test
 File: test/suite/case.c
 Description: 
 License:

   Copyright (c) 2007-2015 Daniel Adler <dadler@uni-goettingen.de>, 
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
#include "config.h"
#include "../../dyncall/dyncall_value.h"

DCValue mValue[NARGS];

void clearValues() { int i; for(i=0; i<NARGS; ++i) mValue[i].L = 0xCAFEBABEDEADC0DEULL; }

void g_DCbool    (DCbool     value, int pos) { mValue[pos].B = value; }
void g_DCchar    (DCchar     value, int pos) { mValue[pos].c = value; }
void g_DCshort   (DCshort    value, int pos) { mValue[pos].s = value; }
void g_DCint     (DCint      value, int pos) { mValue[pos].i = value; }
void g_DClonglong(DClonglong value, int pos) { mValue[pos].l = value; }
void g_DCfloat   (DCfloat    value, int pos) { mValue[pos].f = value; }
void g_DCdouble  (DCdouble   value, int pos) { mValue[pos].d = value; }
void g_DCpointer (DCpointer  value, int pos) { mValue[pos].p = value; }

DCValue* getArg(int pos) { return &mValue[pos]; }

int gID;
int getId() { return gID; }

// Generate function definitions.
#define VF0( id,                               S)  void S(                                                              ) {gID=id;                                                                                                                                    }
#define VF1( id,A1,                            S)  void S(A1 a1                                                         ) {gID=id;g_##A1(a1,0);                                                                                                                       }
#define VF2( id,A1,A2,                         S)  void S(A1 a1, A2 a2                                                  ) {gID=id;g_##A1(a1,0);g_##A2(a2,1);                                                                                                          }
#define VF3( id,A1,A2,A3,                      S)  void S(A1 a1, A2 a2,A3 a3                                            ) {gID=id;g_##A1(a1,0);g_##A2(a2,1);g_##A3(a3,2);                                                                                             }
#define VF4( id,A1,A2,A3,A4,                   S)  void S(A1 a1, A2 a2,A3 a3,A4 a4                                      ) {gID=id;g_##A1(a1,0);g_##A2(a2,1);g_##A3(a3,2);g_##A4(a4,3);                                                                                }
#define VF5( id,A1,A2,A3,A4,A5,                S)  void S(A1 a1, A2 a2,A3 a3,A4 a4,A5 a5                                ) {gID=id;g_##A1(a1,0);g_##A2(a2,1);g_##A3(a3,2);g_##A4(a4,3);g_##A5(a5,4);                                                                   }
#define VF6( id,A1,A2,A3,A4,A5,A6,             S)  void S(A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6                          ) {gID=id;g_##A1(a1,0);g_##A2(a2,1);g_##A3(a3,2);g_##A4(a4,3);g_##A5(a5,4);g_##A6(a6,5);                                                      }
#define VF7( id,A1,A2,A3,A4,A5,A6,A7,          S)  void S(A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7                    ) {gID=id;g_##A1(a1,0);g_##A2(a2,1);g_##A3(a3,2);g_##A4(a4,3);g_##A5(a5,4);g_##A6(a6,5);g_##A7(a7,6);                                         }
#define VF8( id,A1,A2,A3,A4,A5,A6,A7,A8,       S)  void S(A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8              ) {gID=id;g_##A1(a1,0);g_##A2(a2,1);g_##A3(a3,2);g_##A4(a4,3);g_##A5(a5,4);g_##A6(a6,5);g_##A7(a7,6);g_##A8(a8,7);                            }
#define VF9( id,A1,A2,A3,A4,A5,A6,A7,A8,A9,    S)  void S(A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9        ) {gID=id;g_##A1(a1,0);g_##A2(a2,1);g_##A3(a3,2);g_##A4(a4,3);g_##A5(a5,4);g_##A6(a6,5);g_##A7(a7,6);g_##A8(a8,7);g_##A9(a9,8);               }
#define VF10(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,S)  void S(A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10) {gID=id;g_##A1(a1,0);g_##A2(a2,1);g_##A3(a3,2);g_##A4(a4,3);g_##A5(a5,4);g_##A6(a6,5);g_##A7(a7,6);g_##A8(a8,7);g_##A9(a9,8);g_##A10(a10,9);}
#include "case.h"


// Generate function pointer table used for loop.
#undef VF0
#undef VF1
#undef VF2
#undef VF3
#undef VF4
#undef VF5
#undef VF6
#undef VF7
#undef VF8
#undef VF9
#undef VF10

#define VF0( id,                               S) (void(*)())(S),
#define VF1( id,A1,                            S) (void(*)())(S),
#define VF2( id,A1,A2,                         S) (void(*)())(S),
#define VF3( id,A1,A2,A3,                      S) (void(*)())(S),
#define VF4( id,A1,A2,A3,A4,                   S) (void(*)())(S),
#define VF5( id,A1,A2,A3,A4,A5,                S) (void(*)())(S),
#define VF6( id,A1,A2,A3,A4,A5,A6,             S) (void(*)())(S),
#define VF7( id,A1,A2,A3,A4,A5,A6,A7,          S) (void(*)())(S),
#define VF8( id,A1,A2,A3,A4,A5,A6,A7,A8,       S) (void(*)())(S),
#define VF9( id,A1,A2,A3,A4,A5,A6,A7,A8,A9,    S) (void(*)())(S),
#define VF10(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,S) (void(*)())(S),

typedef void (*fp)();
fp gFuncTable[] = {
#include "case.h"
};

DCpointer getFunc(int x) {
  return (DCpointer)gFuncTable[x];
}
