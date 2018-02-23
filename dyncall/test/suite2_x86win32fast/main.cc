/*

 Package: dyncall
 Library: test
 File: test/suite2_x86win32fast/main.cc
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
#include "../../dyncall/dyncall_macros.h"
#include "funcinfo.h"
#include <stdlib.h>
#include "../common/platformInit.h"
#include "../common/platformInit.c" /* Impl. for functions only used in this translation unit */


int   getId();
DCpointer getFunc(int x);
DCValue* getArg(int pos);



DCbool     valueBool[MAXARGS];
DCchar     valueChar[MAXARGS];
DCshort    valueShort[MAXARGS];
DCint      valueInt[MAXARGS];
DClonglong valueLongLong[MAXARGS];
DCdouble   valueDouble[MAXARGS];
DCpointer  valuePointer[MAXARGS];
DCfloat    valueFloat[MAXARGS];


void clearValues();

void init()
{
  for (int i = 0 ; i < MAXARGS ; ++i ) {

    valueBool[i] = DCbool( (i % 1) ? DC_TRUE : DC_FALSE );
    valueChar[i] = DCchar(i);
    valueShort[i] = DCshort(i);
    valueInt[i] = DCint(i);
    valueLongLong[i] = DClonglong(i);
    valueDouble[i] = DCdouble(i);
    valuePointer[i] = DCpointer(i);
    valueFloat[i] = DCfloat(i);
  }
}

void error(const char* message)
{
  fprintf(stderr, "error: %s\n", message);
  exit(-1);
}


bool test(int x)
{
  bool r = true;
  clearValues();

  DCCallVM* pCall;

  funcinfo* fi = &gFuncInfos[x];

  const DCsigchar* sig = &fi->sig[2];
  const DCsigchar* ptr = sig;
  DCsigchar ch;
  int pos;
 
  pos = 0;

  pCall = dcNewCallVM(4096);
#ifdef DC__C_GNU
  dcMode(pCall, DC_CALL_C_X86_WIN32_FAST_GNU);
#else
  dcMode(pCall, DC_CALL_C_X86_WIN32_FAST_MS);
#endif
  dcReset(pCall);
 
  while ( (ch=*ptr++) != '\0' ) {
    switch(ch) {
      case DC_SIGCHAR_BOOL: dcArgBool( pCall, valueBool[pos] ); break;
      case DC_SIGCHAR_CHAR: dcArgChar( pCall, valueChar[pos] ); break;
      case DC_SIGCHAR_SHORT: dcArgShort( pCall, valueShort[pos] ); break;
      case DC_SIGCHAR_INT: dcArgInt( pCall, valueInt[pos] ); break;
      case DC_SIGCHAR_LONGLONG: dcArgLongLong( pCall, valueLongLong[pos] ); break;
      case DC_SIGCHAR_FLOAT: dcArgFloat( pCall, valueFloat[pos] ); break;
      case DC_SIGCHAR_DOUBLE: dcArgDouble( pCall, valueDouble[pos] ); break;
      case DC_SIGCHAR_POINTER: dcArgPointer( pCall, valuePointer[pos] ); break;
      default: error("unsupported signature character"); break;
    }
    ++pos;
  }

  dcCallVoid( pCall, fi->funcptr );

 
  if ( getId() == x ) { 

    ptr = sig;
    pos = 0;
    while( ( ch=*ptr++ ) != '\0' ) {

      switch(ch) {
        case DC_SIGCHAR_BOOL: if ( getArg(pos)->B != valueBool  [pos] ) r = false; break;
        case DC_SIGCHAR_CHAR: if ( getArg(pos)->c != valueChar  [pos] ) r = false; break;
        case DC_SIGCHAR_SHORT: if ( getArg(pos)->s != valueShort  [pos] ) r = false; break;
        case DC_SIGCHAR_INT: if ( getArg(pos)->i != valueInt   [pos] ) r = false; break;
        case DC_SIGCHAR_LONGLONG: if ( getArg(pos)->L != valueLongLong  [pos] ) r = false; break;
        case DC_SIGCHAR_FLOAT: if ( getArg(pos)->f != valueFloat [pos] ) r = false; break;
        case DC_SIGCHAR_DOUBLE: if ( getArg(pos)->d != valueDouble[pos] ) r = false; break;
        case DC_SIGCHAR_POINTER: if ( getArg(pos)->p != valuePointer   [pos] ) r = false; break;
        default: error("unsupported signature character"); break;
      }
      ++pos;
    }

  } else {
    r = false;
  }

  printf("%d-%s:%d\n", x, sig, r);
   
  dcFree(pCall);

  return r;
}


bool run_range(int from, int to)
{
  bool r = true;
  for (int i = from ; i < to ; ++i )
    r &= test(i);
  return r;
}

extern "C" {

int main(int argc, char* argv[])
{
  dcTest_initPlatform();

  bool success = false;
  init();
  if (argc == 2) {
    int index = atoi(argv[1]);
    success = run_range( index, index+1 ); 
  } else if (argc == 3) {
    int from = atoi(argv[1]);
    int to   = atoi(argv[2])+1;
    success = run_range(from,to);
  } else {
    success = run_range(0,NCASES);
  }

  printf("result: suite2_x86win32fast: %s\n", success ? "1" : "0");
  
  dcTest_deInitPlatform();

  return (success) ? 0 : -1;
}

}  // extern "C"

