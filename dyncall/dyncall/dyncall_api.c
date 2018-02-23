/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_api.c
 Description: C interface to call vm
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



#include "dyncall.h"
#include "dyncall_callvm.h"
#include "dyncall_alloc.h"

void dcReset(DCCallVM* vm)
{ 
  vm->mVTpointer->reset(vm); 
}

void dcFree(DCCallVM* vm) 
{ 
  vm->mVTpointer->free(vm); 
}

void dcMode(DCCallVM* vm,DCint mode) 
{ 
  vm->mVTpointer->mode(vm,mode);
  /* dcReset(vm); -- in order to support ellipsis calls, we need to allow
   * a dcMode(callvm, DC_CALL_C_ELLIPSIS_VARARGS) */
}

void dcArgBool(DCCallVM* vm,DCbool x) 
{ 
  vm->mVTpointer->argBool(vm, x); 
}

void dcArgChar(DCCallVM* vm,DCchar x)
{ 
  vm->mVTpointer->argChar(vm, x); 
}

void dcArgShort(DCCallVM* vm,DCshort x) 
{ 
  vm->mVTpointer->argShort(vm, x); 
}

void dcArgInt(DCCallVM* vm,DCint x) 
{ 
  vm->mVTpointer->argInt(vm, x); 
}

void dcArgLong(DCCallVM* vm,DClong x) 
{ 
  vm->mVTpointer->argLong(vm, x); 
}

void dcArgLongLong(DCCallVM* vm, DClonglong x) 
{ 
  vm->mVTpointer->argLongLong(vm, x); 
}

void dcArgFloat(DCCallVM* vm, DCfloat x) 
{ 
  vm->mVTpointer->argFloat(vm, x); 
}

void dcArgDouble(DCCallVM* vm, DCdouble x) 
{ 
  vm->mVTpointer->argDouble(vm, x); 
}

void dcArgPointer(DCCallVM* vm, DCpointer x) 
{ 
  vm->mVTpointer->argPointer(vm, x); 
}

void dcArgStruct(DCCallVM* vm, DCstruct* s, DCpointer x) 
{ 
  vm->mVTpointer->argStruct(vm, s, x); 
}


void dcCallVoid(DCCallVM* vm, DCpointer funcptr) 
{        
  vm->mVTpointer->callVoid(vm, funcptr); 
}

DCchar dcCallChar(DCCallVM* vm, DCpointer funcptr) 
{ 
  return vm->mVTpointer->callChar(vm, funcptr); 
}

DCbool dcCallBool(DCCallVM* vm, DCpointer funcptr) 
{ 
  return vm->mVTpointer->callBool(vm, funcptr); 
}

DCshort dcCallShort(DCCallVM* vm, DCpointer funcptr) 
{ 
  return vm->mVTpointer->callShort(vm, funcptr); 
}

DCint dcCallInt(DCCallVM* vm, DCpointer funcptr) 
{ 
  return vm->mVTpointer->callInt(vm, funcptr); 
}

DClong dcCallLong(DCCallVM* vm, DCpointer funcptr) 
{ 
  return vm->mVTpointer->callLong(vm, funcptr); 
}

DClonglong dcCallLongLong(DCCallVM* vm, DCpointer funcptr) 
{ 
  return vm->mVTpointer->callLongLong(vm, funcptr); 
}

DCfloat dcCallFloat(DCCallVM* vm, DCpointer funcptr) 
{ 
  return vm->mVTpointer->callFloat(vm, funcptr); 
}

DCdouble dcCallDouble(DCCallVM* vm, DCpointer funcptr) 
{ 
  return vm->mVTpointer->callDouble(vm, funcptr); 
}

DCpointer dcCallPointer(DCCallVM* vm, DCpointer funcptr) 
{ 
  return vm->mVTpointer->callPointer(vm, funcptr); 
}

void dcCallStruct(DCCallVM* vm, DCpointer funcptr, DCstruct* s, DCpointer x) 
{ 
  vm->mVTpointer->callStruct(vm, funcptr, s, x); 
}

DCint dcGetError(DCCallVM *vm)
{
  return vm->mError;
}

/*@@@ not used, (re)introduce or cleanup
const char* dcGetErrorString(int mode)
{
  switch(mode) {
    case DC_ERROR_NONE: return "none";
    case DC_ERROR_UNSUPPORTED_MODE: return "unsupported mode";
    default: return "(unknown mode id)";
  }
}
*/

