/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm.h
 Description: Common call vm binary interface.
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



#ifndef DYNCALL_CALLVM_H
#define DYNCALL_CALLVM_H

#include "dyncall.h"

/* --- callvm virtual function table --------------------------------------- */

/* NOTE: if something changes in DCCallVM_, update offset marks in dyncall_call_sparc.S */

typedef struct DCCallVM_vt_ DCCallVM_vt;

struct DCCallVM_
{
  DCCallVM_vt* mVTpointer;
  DCint        mError;
};

struct DCCallVM_vt_
{
  void         (*free)           (DCCallVM* vm);
  void         (*reset)          (DCCallVM* vm);
  void         (*mode)           (DCCallVM* vm,DCint        mode);
  void         (*argBool)        (DCCallVM* vm,DCbool          B);
  void         (*argChar)        (DCCallVM* vm,DCchar          c);
  void         (*argShort)       (DCCallVM* vm,DCshort         s);
  void         (*argInt)         (DCCallVM* vm,DCint           i);
  void         (*argLong)        (DCCallVM* vm,DClong          l);
  void         (*argLongLong)    (DCCallVM* vm,DClonglong      L);
  void         (*argFloat)       (DCCallVM* vm,DCfloat         f);
  void         (*argDouble)      (DCCallVM* vm,DCdouble        d);
  void         (*argPointer)     (DCCallVM* vm,DCpointer       p);
  void         (*argStruct)      (DCCallVM* vm,DCstruct* s, DCpointer p);
  void         (*callVoid)       (DCCallVM* vm,DCpointer funcptr);
  DCbool       (*callBool)       (DCCallVM* vm,DCpointer funcptr);
  DCchar       (*callChar)       (DCCallVM* vm,DCpointer funcptr);
  DCshort      (*callShort)      (DCCallVM* vm,DCpointer funcptr);
  DCint        (*callInt)        (DCCallVM* vm,DCpointer funcptr);
  DClong       (*callLong)       (DCCallVM* vm,DCpointer funcptr);
  DClonglong   (*callLongLong)   (DCCallVM* vm,DCpointer funcptr);
  DCfloat      (*callFloat)      (DCCallVM* vm,DCpointer funcptr);
  DCdouble     (*callDouble)     (DCCallVM* vm,DCpointer funcptr);
  DCpointer    (*callPointer)    (DCCallVM* vm,DCpointer funcptr);
  void         (*callStruct)     (DCCallVM* vm,DCpointer funcptr,DCstruct* s, DCpointer returnValue);
};

typedef DCvoid       (DCvoidvmfunc)      (DCCallVM* vm,DCpointer funcptr); 
typedef DCbool       (DCboolvmfunc)      (DCCallVM* vn,DCpointer funcptr);
typedef DCchar       (DCcharvmfunc)      (DCCallVM* vm,DCpointer funcptr);
typedef DCshort      (DCshortvmfunc)     (DCCallVM* vm,DCpointer funcptr);
typedef DCint        (DCintvmfunc)       (DCCallVM* vm,DCpointer funcptr);
typedef DClong       (DClongvmfunc)      (DCCallVM* vm,DCpointer funcptr);
typedef DClonglong   (DClonglongvmfunc)  (DCCallVM* vm,DCpointer funcptr);
typedef DCfloat      (DCfloatvmfunc)     (DCCallVM* vm,DCpointer funcptr);
typedef DCdouble     (DCdoublevmfunc)    (DCCallVM* vm,DCpointer funcptr);
typedef DCpointer    (DCpointervmfunc)   (DCCallVM* vm,DCpointer funcptr);

/* Common base functions for CallVM implementations. */

void dc_callvm_base_init(DCCallVM *pInstance, DCCallVM_vt* pVTable);

#endif /* DYNCALL_CALLVM_H */

