/*

 Package: dyncall
 Library: test
 File: test/suite2_x86win32fast/case.cc
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
#include "funcinfo.h"

#ifdef DC__C_GNU
#define API __attribute__((fastcall))
#else
#define API __fastcall
#endif

DCValue mValue[MAXARGS];

void clearValues() { for(int i = 0;i<MAXARGS;++i) mValue[i].L = 0xCAFEBABEDEADC0DEULL; }

template<typename T> void g(T value, int pos);

template<> void g(DCchar value, int pos) { mValue[pos].c = value; }
template<> void g(DCshort value, int pos) { mValue[pos].s = value; }
template<> void g(DCint value, int pos) { mValue[pos].i = value; }
template<> void g(DCpointer value, int pos) { mValue[pos].p = value; }
template<> void g(DClonglong value, int pos) { mValue[pos].l = value; }
template<> void g(DCfloat value, int pos) { mValue[pos].f = value; }
template<> void g(DCdouble value, int pos) { mValue[pos].d = value; }

DCValue* getArg(int pos) { return &mValue[pos]; }

int gID;
int getId() { return gID; }

extern "C" {

#define VF0(id,S) void API f##id () {gID=id;}
#define VF1(id,A1,S) void API f##id (A1 a1) {gID=id;g<A1>(a1,0);}
#define VF2(id,A1,A2,S) void API f##id (A1 a1, A2 a2) {gID=id;g<A1>(a1,0);g<A2>(a2,1);}
#define VF3(id,A1,A2,A3,S) void API f##id (A1 a1, A2 a2,A3 a3) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);}
#define VF4(id,A1,A2,A3,A4,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);}
#define VF5(id,A1,A2,A3,A4,A5,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);}
#define VF6(id,A1,A2,A3,A4,A5,A6,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);}
#define VF7(id,A1,A2,A3,A4,A5,A6,A7,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);}
#define VF8(id,A1,A2,A3,A4,A5,A6,A7,A8,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);}
#define VF9(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);}
#define VF10(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);}
#define VF11(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10,A11 a11) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);g<A11>(a11,10);}
#define VF12(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10,A11 a11,A12 a12) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);g<A11>(a11,10);g<A12>(a12,11);}
#define VF13(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10,A11 a11,A12 a12,A13 a13) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);g<A11>(a11,10);g<A12>(a12,11);g<A13>(a13,12);}
#define VF14(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10,A11 a11,A12 a12,A13 a13,A14 a14) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);g<A11>(a11,10);g<A12>(a12,11);g<A13>(a13,12);g<A14>(a14,13);}
#define VF15(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10,A11 a11,A12 a12,A13 a13,A14 a14,A15 a15) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);g<A11>(a11,10);g<A12>(a12,11);g<A13>(a13,12);g<A14>(a14,13);g<A15>(a15,14);}
#define VF16(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10,A11 a11,A12 a12,A13 a13,A14 a14,A15 a15,A16 a16) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);g<A11>(a11,10);g<A12>(a12,11);g<A13>(a13,12);g<A14>(a14,13);g<A15>(a15,14);g<A16>(a16,15);}
#define VF17(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10,A11 a11,A12 a12,A13 a13,A14 a14,A15 a15,A16 a16,A17 a17) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);g<A11>(a11,10);g<A12>(a12,11);g<A13>(a13,12);g<A14>(a14,13);g<A15>(a15,14);g<A16>(a16,15);g<A17>(a17,16);}
#define VF18(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10,A11 a11,A12 a12,A13 a13,A14 a14,A15 a15,A16 a16,A17 a17,A18 a18) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);g<A11>(a11,10);g<A12>(a12,11);g<A13>(a13,12);g<A14>(a14,13);g<A15>(a15,14);g<A16>(a16,15);g<A17>(a17,16);g<A18>(a18,17);}
#define VF19(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10,A11 a11,A12 a12,A13 a13,A14 a14,A15 a15,A16 a16,A17 a17,A18 a18,A19 a19) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);g<A11>(a11,10);g<A12>(a12,11);g<A13>(a13,12);g<A14>(a14,13);g<A15>(a15,14);g<A16>(a16,15);g<A17>(a17,16);g<A18>(a18,17);g<A19>(a19,18);}
#define VF20(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,S) void API f##id (A1 a1, A2 a2,A3 a3,A4 a4,A5 a5,A6 a6,A7 a7,A8 a8,A9 a9,A10 a10,A11 a11,A12 a12,A13 a13,A14 a14,A15 a15,A16 a16,A17 a17,A18 a18,A19 a19,A20 a20) {gID=id;g<A1>(a1,0);g<A2>(a2,1);g<A3>(a3,2);g<A4>(a4,3);g<A5>(a5,4);g<A6>(a6,5);g<A7>(a7,6);g<A8>(a8,7);g<A9>(a9,8);g<A10>(a10,9);g<A11>(a11,10);g<A12>(a12,11);g<A13>(a13,12);g<A14>(a14,13);g<A15>(a15,14);g<A16>(a16,15);g<A17>(a17,16);g<A18>(a18,17);g<A19>(a19,18);g<A20>(a20,19);}

#include "case.h"

}

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
#undef VF11
#undef VF12
#undef VF13
#undef VF14
#undef VF15
#undef VF16
#undef VF17
#undef VF18
#undef VF19
#undef VF20

funcinfo gFuncInfos[] = {

#define VF0(id,S) (void*)(f##id), #S,
#define VF1(id,A1,S) (void*)(f##id), #S,
#define VF2(id,A1,A2,S) (void*)(f##id), #S,
#define VF3(id,A1,A2,A3,S) (void*)(f##id), #S,
#define VF4(id,A1,A2,A3,A4,S) (void*)(f##id), #S,
#define VF5(id,A1,A2,A3,A4,A5,S) (void*)(f##id), #S,
#define VF6(id,A1,A2,A3,A4,A5,A6,S) (void*)(f##id), #S,
#define VF7(id,A1,A2,A3,A4,A5,A6,A7,S) (void*)(f##id), #S,
#define VF8(id,A1,A2,A3,A4,A5,A6,A7,A8,S) (void*)(f##id), #S,
#define VF9(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,S) (void*)(f##id), #S,
#define VF10(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,S) (void*)(f##id), #S,
#define VF11(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,S) (void*)(f##id), #S,
#define VF12(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,S) (void*)(f##id), #S,
#define VF13(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,S) (void*)(f##id), #S,
#define VF14(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,S) (void*)(f##id), #S,
#define VF15(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,S) (void*)(f##id), #S,
#define VF16(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,S) (void*)(f##id), #S,
#define VF17(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,S) (void*)(f##id), #S,
#define VF18(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,S) (void*)(f##id), #S,
#define VF19(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,S) (void*)(f##id), #S,
#define VF20(id,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,S) (void*)(f##id), #S,

#include "case.h"

};



