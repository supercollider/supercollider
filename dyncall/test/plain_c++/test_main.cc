/*

 Package: dyncall
 Library: test
 File: test/plain_c++/test_main.cc
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


/* ------------------------------------------------------------------------- 
 * test: identity this calls 
 * ------------------------------------------------------------------------- */

union ValueUnion
{
  DCbool     B;
  DCint      i;
  DClong     j;
  DClonglong l;
  DCfloat    f;
  DCdouble   d;
  DCpointer  p;
};

/* C++ class using __cdecl this call */

// #define VTBI_DESTRUCTOR 0

/*
 * the layout of the VTable is non-standard and it is not clear what is the initial real first method index.
 * so for it turns out that: 
 * on vc/x86  : 1
 * on GCC/x86 : 2
 */

#if defined DC__C_MSVC
#define VTBI_BASE 1
#else
#define VTBI_BASE 2
#endif

#define VTBI_SET_BOOL VTBI_BASE+0
#define VTBI_GET_BOOL VTBI_BASE+1
#define VTBI_SET_INT  VTBI_BASE+2
#define VTBI_GET_INT  VTBI_BASE+3
#define VTBI_SET_LONG VTBI_BASE+4
#define VTBI_GET_LONG VTBI_BASE+5
#define VTBI_SET_LONG_LONG VTBI_BASE+6
#define VTBI_GET_LONG_LONG VTBI_BASE+7
#define VTBI_SET_FLOAT VTBI_BASE+8
#define VTBI_GET_FLOAT VTBI_BASE+9
#define VTBI_SET_DOUBLE VTBI_BASE+10
#define VTBI_GET_DOUBLE VTBI_BASE+11
#define VTBI_SET_POINTER VTBI_BASE+12
#define VTBI_GET_POINTER VTBI_BASE+13

class Value
{
public:
  virtual ~Value()   {}

  virtual void       __cdecl setBool(DCbool x)         { mValue.B = x; }
  virtual DCbool     __cdecl getBool()                 { return mValue.B; }
  virtual void       __cdecl setInt(DCint x)           { mValue.i = x; }
  virtual DCint      __cdecl getInt()                  { return mValue.i; }
  virtual void       __cdecl setLong(DClong x)         { mValue.j = x; }
  virtual DClong     __cdecl getLong()                 { return mValue.j; }
  virtual void       __cdecl setLongLong(DClonglong x) { mValue.l = x; }
  virtual DClonglong __cdecl getLongLong()             { return mValue.l; }
  virtual void       __cdecl setFloat(DCfloat x)       { mValue.f = x; }
  virtual DCfloat    __cdecl getFloat()                { return mValue.f; }
  virtual void       __cdecl setDouble(DCdouble x)     { mValue.d = x; }
  virtual DCdouble   __cdecl getDouble()               { return mValue.d; }
  virtual void       __cdecl setPtr(DCpointer x)       { mValue.p = x; }
  virtual DCpointer  __cdecl getPtr()                  { return mValue.p; }
private:
  ValueUnion mValue;
};

/* C++ class using (on win32: microsoft) this call */

class ValueMS
{
public:
  virtual ~ValueMS()    {}

  virtual void       setBool(DCbool x)         { mValue.B = x; }
  virtual DCbool     getBool()                 { return mValue.B; }
  virtual void       setInt(DCint x)           { mValue.i = x; }
  virtual DCint      getInt()                  { return mValue.i; }
  virtual void       setLong(DClong x)         { mValue.j = x; }
  virtual DClong     getLong()                 { return mValue.j; }
  virtual void       setLongLong(DClonglong x) { mValue.l = x; }
  virtual DClonglong getLongLong()             { return mValue.l; }
  virtual void       setFloat(DCfloat x)       { mValue.f = x; }
  virtual DCfloat    getFloat()                { return mValue.f; }
  virtual void       setDouble(DCdouble x)     { mValue.d = x; }
  virtual DCdouble   getDouble()               { return mValue.d; }
  virtual void       setPtr(DCpointer x)       { mValue.p = x; }
  virtual DCpointer  getPtr()                  { return mValue.p; }
private:
  ValueUnion mValue;
};

template<typename T>
void testCallValue(DCCallVM* pc)
{
  T o;
  T* pThis = &o;
  DCpointer* vtbl =  *( (DCpointer**) pThis ); /* vtbl is located at beginning of class */

  /* set/get bool (TRUE) */

  dcReset(pc);
  dcArgPointer(pc, pThis);
  dcArgBool(pc,DC_TRUE);
  dcCallVoid(pc, vtbl[VTBI_SET_BOOL] );
  dcReset(pc);
  dcArgPointer(pc, pThis);
  DC_TEST( dcCallBool(pc, vtbl[VTBI_GET_BOOL] ) == DC_TRUE );
  /* set/get bool (FALSE) */

  dcReset(pc);
  dcArgPointer(pc, pThis);
  dcArgBool(pc,DC_FALSE);
  dcCallVoid(pc, vtbl[VTBI_SET_BOOL] );
  dcReset(pc);
  dcArgPointer(pc, pThis);
  DC_TEST( dcCallBool(pc, vtbl[VTBI_GET_BOOL] ) == DC_FALSE );

  /* set/get int */

  dcReset(pc);
  dcArgPointer(pc, pThis);
  dcArgInt(pc,1234);
  dcCallVoid(pc, vtbl[VTBI_SET_INT] );
  dcReset(pc);
  dcArgPointer(pc, pThis);
  DC_TEST( dcCallInt(pc, vtbl[VTBI_GET_INT] ) == 1234 );

  /* set/get long */

  dcReset(pc);
  dcArgPointer(pc, pThis);
  dcArgLong(pc,0xCAFEBABEUL);
  dcCallVoid(pc, vtbl[VTBI_SET_LONG] );
  dcReset(pc);
  dcArgPointer(pc, pThis);
  DC_TEST( dcCallLong(pc, vtbl[VTBI_GET_LONG] ) == (DClong)0xCAFEBABEUL );

  /* set/get long long */

  dcReset(pc);
  dcArgPointer(pc, pThis);
  dcArgLongLong(pc,0xCAFEBABEDEADC0DELL);
  dcCallVoid(pc, vtbl[VTBI_SET_LONG_LONG] );
  dcReset(pc);
  dcArgPointer(pc, pThis);
  DC_TEST( dcCallLongLong(pc, vtbl[VTBI_GET_LONG_LONG] ) == (DClonglong)0xCAFEBABEDEADC0DELL );

  /* set/get float */

  dcReset(pc);
  dcArgPointer(pc, pThis);
  dcArgFloat(pc,1.2345f);
  dcCallVoid(pc, vtbl[VTBI_SET_FLOAT] );
  dcReset(pc);
  dcArgPointer(pc, pThis);
  DC_TEST( dcCallFloat(pc, vtbl[VTBI_GET_FLOAT] ) == 1.2345f );

  /* set/get double */

  dcReset(pc);
  dcArgPointer(pc, pThis);
  dcArgDouble(pc,1.23456789);
  dcCallVoid(pc, vtbl[VTBI_SET_DOUBLE] );
  dcReset(pc);
  dcArgPointer(pc, pThis);
  DC_TEST( dcCallDouble(pc, vtbl[VTBI_GET_DOUBLE] ) == 1.23456789 );

  /* set/get pointer */

  dcReset(pc);
  dcArgPointer(pc, pThis);
  dcArgPointer(pc, (DCpointer) 0xCAFEBABE );
  dcCallVoid(pc, vtbl[VTBI_SET_POINTER] );
  dcReset(pc);
  dcArgPointer(pc, pThis);
  DC_TEST( dcCallPointer(pc, vtbl[VTBI_GET_POINTER] ) == ( (DCpointer) 0xCAFEBABE ) );
}


#ifdef DC__OS_Win32

DC_DEFINE_TEST_FUNC_BEGIN(testCallThisMS)

  DCCallVM* pc = dcNewCallVM(4096);
  dcMode(pc,DC_CALL_C_X86_WIN32_THIS_MS);
  dcReset(pc);
  testCallValue<ValueMS>(pc); 
  dcFree(pc);

DC_DEFINE_TEST_FUNC_END

#endif


DC_DEFINE_TEST_FUNC_BEGIN(testCallThisC)

  DCCallVM* pc = dcNewCallVM(4096);
  dcReset(pc);
  testCallValue<Value>(pc); 
  dcFree(pc);

DC_DEFINE_TEST_FUNC_END


extern "C" {

int main(int argc, char* argv[])
{
  dcTest_initPlatform();

  int b = TRUE;
  
#if defined(DC__OS_Win32)	// ThisCall temporarily only for win 32 @@@
  
  b = b && testCallThisC();
  printf("ThisC:%d\n",b);

#if defined(DC__C_MSVC)
  b = b && testCallThisMS();
  printf("ThisMS:%d\n",b);
#endif

#endif

  printf("result: plain_cpp: %d\n", b);

  dcTest_deInitPlatform();

  return !b;
}

}  // extern "C"

