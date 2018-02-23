/*

 Package: dyncall
 Library: test
 File: test/plain/test_main.c
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




#include "../common/test_framework.h"
#include "../../dyncall/dyncall.h"
#include "../common/platformInit.h"
#include "../common/platformInit.c" /* Impl. for functions only used in this translation unit */


/* ------------------------------------------------------------------------- 
 * test: identity function calls 
 * ------------------------------------------------------------------------- */

#define DEF_FUNCS(API,NAME) \
void       API fun_##NAME##_v()             {           } \
DCbool     API fun_##NAME##_b(DCbool x)     { return x; } \
DCint      API fun_##NAME##_i(DCint x)      { return x; } \
DClong     API fun_##NAME##_j(DClong x)     { return x; } \
DClonglong API fun_##NAME##_l(DClonglong x) { return x; } \
DCfloat    API fun_##NAME##_f(DCfloat x)    { return x; } \
DCdouble   API fun_##NAME##_d(DCdouble x)   { return x; } \
DCpointer  API fun_##NAME##_p(DCpointer  x) { return x; }

/* __cdecl */

#if !defined(DC__OS_Win32)
#  define __declspec(X)
#  define __cdecl
#endif

DEF_FUNCS(__cdecl,c)

DC_DEFINE_TEST_FUNC_BEGIN(testCallC)

  DCCallVM* pc = dcNewCallVM(4096);
  dcMode(pc,DC_CALL_C_DEFAULT);
  /* void */
  dcReset(pc);
  dcCallVoid(pc, (DCpointer) &fun_c_v);
  /* bool */
  {
    DCbool r, val=DC_TRUE; 
    dcReset(pc);
    dcArgBool(pc, val);
    r = dcCallBool(pc, (DCpointer) &fun_c_b);
    DC_TEST(r == val);

    val=DC_FALSE;
    dcReset(pc);
    dcArgBool(pc, val);
    r = dcCallBool(pc, (DCpointer) &fun_c_b);
    DC_TEST(r == val);
  }
  /* int */
  {
    DCint r, val=1234;
    dcReset(pc);
    dcArgInt(pc, val);
    r = dcCallInt(pc, (DCpointer) &fun_c_i);
    DC_TEST(r == val);
  }
  /* long */
  {
    DClong r, val=(DClong) 0xCAFEBABEL;
    dcReset(pc);
    dcArgLong(pc, val);
    r = dcCallLong(pc, (DCpointer) &fun_c_j);
    DC_TEST(r == val);
  }
  /* long long */
  {
    DClonglong r, val=(DClonglong) 0xCAFEBABEDEADC0DELL;
    dcReset(pc);
    dcArgLongLong(pc, val);
    r = dcCallLongLong(pc, (DCpointer) &fun_c_l);
    DC_TEST(r == (DClonglong)val);
  }
  /* float */
  {
    DCfloat r, val=1.234567f;
    dcReset(pc);
    dcArgFloat(pc, val);
    r = dcCallFloat(pc, (DCpointer) &fun_c_f);
    DC_TEST(r == val);
  }
  /* double */
  {
    DCdouble r, val=1.23456789;
    dcReset(pc);
    dcArgDouble(pc, val);
    r = dcCallDouble(pc, (DCpointer) &fun_c_d);
    DC_TEST(r == val);
  }
  /* ptr */
  {
    DCpointer r;
    dcReset(pc);
    dcArgPointer(pc, (DCpointer) &fun_c_b);
    r = dcCallPointer(pc, (DCpointer) &fun_c_p);
    DC_TEST(r == (DCpointer) &fun_c_b);
  }
  dcFree(pc);

DC_DEFINE_TEST_FUNC_END


#ifdef DC__OS_Win32
/* win32 __stdcall */

DEF_FUNCS(__stdcall,std)

DC_DEFINE_TEST_FUNC_BEGIN(testCallStd)

  DCCallVM* pc = dcNewCallVM(4096);
  dcMode(pc,DC_CALL_C_X86_WIN32_STD);
  /* void */
  dcReset(pc);
  dcCallVoid(pc, (DCpointer) &fun_std_v);
  /* bool */
  {
    DCbool r, val=DC_TRUE;
    dcReset(pc);
    dcArgBool(pc, val);
    r = dcCallBool(pc, (DCpointer) &fun_std_b);
    DC_TEST(r == val);

    val=DC_FALSE;
    dcReset(pc);
    dcArgBool(pc, val);
    r = dcCallBool(pc, (DCpointer) &fun_std_b);
    DC_TEST(r == val);
  }
  /* int */
  {
    DCint r, val=1234;
    dcReset(pc);
    dcArgInt(pc, val);
    r = dcCallInt(pc, (DCpointer) &fun_std_i);
    DC_TEST(r == val);
  }
  /* long */
  {
    DClong r, val=0xCAFEBABEUL;
    dcReset(pc);
    dcArgLong(pc, val);
    r = dcCallLong(pc, (DCpointer) &fun_std_j);
    DC_TEST(r == val);
  }
  /* long long */
  {
    DClonglong r, val=0xCAFEBABEDEADC0DEULL;
    dcReset(pc);
    dcArgLongLong(pc, val);
    r = dcCallLongLong(pc, (DCpointer) &fun_std_l);
    DC_TEST(r == val);
  }
  /* float */
  {
    DCfloat r, val=1.234567f;
    dcReset(pc);
    dcArgFloat(pc, val);
    r = dcCallFloat(pc, (DCpointer) &fun_std_f);
    DC_TEST(r == val);
  }
  /* double */
  {
    DCdouble r, val=1.23456789;
    dcReset(pc);
    dcArgDouble(pc, val);
    r = dcCallDouble(pc, (DCpointer) &fun_std_d);
    DC_TEST(r == val);
  }
  /* ptr */
  {
    DCpointer r;
    dcReset(pc);
    dcArgPointer(pc, (DCpointer) &fun_c_b);
    r = dcCallPointer(pc, (DCpointer) &fun_std_p);
    DC_TEST(r == &fun_c_b);
  }
  dcFree(pc);

DC_DEFINE_TEST_FUNC_END

#endif


#ifdef DC__OS_Win32
/* win32 __fastcall */

DEF_FUNCS(__fastcall,fast)

DC_DEFINE_TEST_FUNC_BEGIN(testCallFast)

  DCCallVM* pc = dcNewCallVM(4096);
#ifdef DC__C_GNU
  dcMode(pc,DC_CALL_C_X86_WIN32_FAST_GNU);
#else
  dcMode(pc,DC_CALL_C_X86_WIN32_FAST_MS);
#endif
  /* void */
  dcReset(pc);
  dcCallVoid(pc, (DCpointer) &fun_fast_v);
  /* bool */
  {
    DCbool r, val=DC_TRUE;
    dcReset(pc);
    dcArgBool(pc, val);
    r = dcCallBool(pc, (DCpointer) &fun_fast_b);
    DC_TEST(r == val);

    val=DC_FALSE;
    dcReset(pc);
    dcArgBool(pc, val);
    r = dcCallBool(pc, (DCpointer) &fun_fast_b);
    DC_TEST(r == val);
  }
  /* int */
  {
    DCint r, val=1234;
    dcReset(pc);
    dcArgInt(pc, val);
    r = dcCallInt(pc, (DCpointer) &fun_fast_i);
    DC_TEST(r == val);
  }
  /* long */
  {
    DClong r, val=0xCAFEBABEUL;
    dcReset(pc);
    dcArgLong(pc, val);
    r = dcCallLong(pc, (DCpointer) &fun_fast_j);
    DC_TEST(r == val);
  }
  /* long long */
  {
    DClonglong r, val=0xCAFEBABEDEADC0DEULL;
    dcReset(pc);
    dcArgLongLong(pc, val);
    r = dcCallLongLong(pc, (DCpointer) &fun_fast_l);
    DC_TEST(r == val);
  }
  /* float */
  {
    DCfloat r, val=1.234567f;
    dcReset(pc);
    dcArgFloat(pc, val);
    r = dcCallFloat(pc, (DCpointer) &fun_fast_f);
    DC_TEST(r == val);
  }
  /* double */
  {
    DCdouble r, val=1.23456789;
    dcReset(pc);
    dcArgDouble(pc, val);
    r = dcCallDouble(pc, (DCpointer) &fun_fast_d);
    DC_TEST(r == val);
  }
  /* ptr */
  {
    DCpointer r;
    dcReset(pc);
    dcArgPointer(pc, (DCpointer) &fun_c_b);
    r = dcCallPointer(pc, (DCpointer) &fun_fast_p);
    DC_TEST(r == &fun_c_b);
  }
  dcFree(pc);

DC_DEFINE_TEST_FUNC_END
#endif

int testCallStructs();
int testStructSizes();
int main(int argc, char* argv[])
{
  int b = TRUE;
  dcTest_initPlatform();
  
  b = b && testCallC();
  printf("C:%d\n",b);

  b = b && testStructSizes();
  printf("Struct Sizes:%d\n",b);

  /*b = b && testCallStructs();
  printf("Call Structs:%d\n",b);*/

#if defined(DC__OS_Win32)
  
  b = b && testCallStd();
  printf("Std:%d\n",b);

  b = b && testCallFast();
#ifdef DC__C_GNU
  printf("FastGNU:%d\n",b);
#else
  printf("FastMS:%d\n",b);
#endif

#endif

  printf("result: plain: %d\n", b);

  dcTest_deInitPlatform();

  return !b;
}

