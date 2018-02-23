/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callf.c
 Description: formatted call C interface (extension module)
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



#include "dyncall_callf.h"


/* Shareable implementation for argument binding used in ArgF and CallF below. */
static void dcArgF_impl(DCCallVM* vm, const DCsigchar** sigptr, va_list args)
{
  DCsigchar ch;
  dcReset(vm);
  while((ch=*(*sigptr)++) != '\0' && ch != DC_SIGCHAR_ENDARG) {
    switch(ch) {
      case DC_SIGCHAR_BOOL:      dcArgBool    (vm, (DCbool)           va_arg(args, DCint     )); break;
      case DC_SIGCHAR_CHAR:      dcArgChar    (vm, (DCchar)           va_arg(args, DCint     )); break;
      case DC_SIGCHAR_UCHAR:     dcArgChar    (vm, (DCchar)(DCuchar)  va_arg(args, DCint     )); break;
      case DC_SIGCHAR_SHORT:     dcArgShort   (vm, (DCshort)          va_arg(args, DCint     )); break;
      case DC_SIGCHAR_USHORT:    dcArgShort   (vm, (DCshort)(DCushort)va_arg(args, DCint     )); break;
      case DC_SIGCHAR_INT:       dcArgInt     (vm, (DCint)            va_arg(args, DCint     )); break;
      case DC_SIGCHAR_UINT:      dcArgInt     (vm, (DCint)(DCuint)    va_arg(args, DCint     )); break;
      case DC_SIGCHAR_LONG:      dcArgLong    (vm, (DClong)           va_arg(args, DClong    )); break;
      case DC_SIGCHAR_ULONG:     dcArgLong    (vm, (DCulong)          va_arg(args, DClong    )); break;
      case DC_SIGCHAR_LONGLONG:  dcArgLongLong(vm, (DClonglong)       va_arg(args, DClonglong)); break;
      case DC_SIGCHAR_ULONGLONG: dcArgLongLong(vm, (DCulonglong)      va_arg(args, DClonglong)); break;
      case DC_SIGCHAR_FLOAT:     dcArgFloat   (vm, (DCfloat)          va_arg(args, DCdouble  )); break;
      case DC_SIGCHAR_DOUBLE:    dcArgDouble  (vm, (DCdouble)         va_arg(args, DCdouble  )); break;
      case DC_SIGCHAR_POINTER:   dcArgPointer (vm, (DCpointer)        va_arg(args, DCpointer )); break;
      case DC_SIGCHAR_STRING:    dcArgPointer (vm, (DCpointer)        va_arg(args, DCpointer )); break;
    }
  }
}

void dcVArgF(DCCallVM* vm, const DCsigchar* signature, va_list args)
{
  dcArgF_impl(vm, &signature, args);
}

void dcArgF(DCCallVM* vm, const DCsigchar* signature, ...)
{
  va_list va;
  va_start(va, signature);
  dcVArgF(vm,signature,va);
  va_end(va);
}

void dcVCallF(DCCallVM* vm, DCValue* result, DCpointer funcptr, const DCsigchar* signature, va_list args)
{
  const DCsigchar* ptr = signature;
  dcArgF_impl(vm, &ptr, args);

  switch(*ptr) {
    case DC_SIGCHAR_VOID:                   dcCallVoid             (vm,funcptr); break;
    case DC_SIGCHAR_BOOL:       result->B = dcCallBool             (vm,funcptr); break;
    case DC_SIGCHAR_CHAR:       result->c = dcCallChar             (vm,funcptr); break;
    case DC_SIGCHAR_UCHAR:      result->C = (DCuchar)dcCallChar    (vm,funcptr); break;
    case DC_SIGCHAR_SHORT:      result->s = dcCallShort            (vm,funcptr); break;
    case DC_SIGCHAR_USHORT:     result->S = dcCallShort            (vm,funcptr); break;
    case DC_SIGCHAR_INT:        result->i = dcCallInt              (vm,funcptr); break;
    case DC_SIGCHAR_UINT:       result->I = dcCallInt              (vm,funcptr); break;
    case DC_SIGCHAR_LONG:       result->j = dcCallLong             (vm,funcptr); break;
    case DC_SIGCHAR_ULONG:      result->J = dcCallLong             (vm,funcptr); break;
    case DC_SIGCHAR_LONGLONG:   result->l = dcCallLongLong         (vm,funcptr); break;
    case DC_SIGCHAR_ULONGLONG:  result->L = dcCallLongLong         (vm,funcptr); break;
    case DC_SIGCHAR_FLOAT:      result->f = dcCallFloat            (vm,funcptr); break;
    case DC_SIGCHAR_DOUBLE:     result->d = dcCallDouble           (vm,funcptr); break;
    case DC_SIGCHAR_POINTER:    result->p = dcCallPointer          (vm,funcptr); break;
    case DC_SIGCHAR_STRING:     result->Z = (DCstring)dcCallPointer(vm,funcptr); break;
  }
}

void dcCallF(DCCallVM* vm, DCValue* result, DCpointer funcptr, const DCsigchar* signature, ...)
{
  va_list va;
  va_start(va, signature);
  dcVCallF(vm,result,funcptr,signature,va);
  va_end(va);
}

