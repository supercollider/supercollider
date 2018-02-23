/*

 Package: dyncall
 Library: test
 File: test/suite_x86win32std/main.cc
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
#include <math.h>
#include "../common/platformInit.h"
#include "../common/platformInit.c" /* Impl. for functions only used in this translation unit */


int   getId();
DCpointer getFunc(int x);
DCValue* getArg(int pos);


typedef double precise;


DCbool     valueBool[NARGS];
DCshort    valueShort[NARGS];
DCchar     valueChar[NARGS];
DCint      valueInt[NARGS];
DClonglong valueLongLong[NARGS];
DCdouble   valueDouble[NARGS];
DCpointer  valuePointer[NARGS];
DCfloat    valueFloat[NARGS];


bool equals(int select, int pos, void* data)
{
  switch(select)
  {
    case 0: return ( getArg(pos)->B == valueBool[pos] ); break;
    case 1: return ( getArg(pos)->i == valueInt[pos] ); break;
    case 2: return ( getArg(pos)->L == valueLongLong[pos] ); break;
    case 3: return ( getArg(pos)->d == valueDouble[pos] ); break;
    case 4: return ( getArg(pos)->p == valuePointer[pos] ); break;
    case 5: return ( getArg(pos)->f == valueFloat[pos] ); break;
  }
  return false;
}


void clearValues();


void init()
{
  for (int i = 0 ; i < NARGS ; ++i ) {

    valueBool[i] = DCbool( (i % 1) ? DC_TRUE : DC_FALSE );
    valueInt[i] = DCint(i);
    valueLongLong[i] = DClonglong(i);
    valueDouble[i] = DCdouble(i);
    valuePointer[i] = DCpointer(i);
    valueFloat[i] = DCfloat(i);
  } 
}


void push(DCCallVM* pCall, int select, int pos)
{
  switch(select) 
  {
    case 0: dcArgBool( pCall, valueBool[pos] ); break;
    case 1: dcArgInt( pCall, valueInt[pos] ); break;
    case 2: dcArgLongLong( pCall, valueLongLong[pos] ); break;
    case 3: dcArgDouble( pCall, valueDouble[pos] ); break;
    case 4: dcArgPointer( pCall, valuePointer[pos] ); break;
    case 5: dcArgFloat( pCall, valueFloat[pos] ); break;
  }
}


#define assert(x) if (!(x)) return false


bool test(int x)
{
  clearValues();

  DCCallVM* pCall = dcNewCallVM(4096);
  dcMode(pCall, DC_CALL_C_X86_WIN32_STD);
  dcReset(pCall);
  int y = x;
  int selects[NARGS] = { 0, };
  int pos = 0;
  for(pos = 0;y>0;++pos) 
  {
    int select = (y-1) % NTYPES; 
    selects[pos] = select;
    push(pCall,select,pos);
    y = (y-1) / NTYPES;
  }
  dcCallVoid(pCall,getFunc(x));
  
  assert( getId() == x );
  
  for(int i = 0;i<pos;++i) {
    assert( equals( selects[i], i, getArg(i) ) );      
  }
  
  dcFree(pCall);
  return true;
}


int powerfact(int x, int n)
{
  if (n==0) return 0;
  return static_cast<int>( pow((double)x,n)+powerfact(x,n-1) );
}

#include <stdio.h>
#include <stdlib.h>

bool run_range(int from, int to)
{
  bool tr = true;
  for (int i = from ; i < to ; ++i ) {
    printf("%d:",i);
    bool r = test(i);
    printf("%d\n", r);
    tr &= r;
  }
  return tr;
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
    int ncalls = powerfact(NTYPES,NARGS)+1;
    success = run_range(0,ncalls);
  }

  printf("result: suite_x86win32std: %s\n", success ? "1" : "0");
  
  dcTest_deInitPlatform();

  return (success) ? 0 : -1;
}

}  // extern "C"

