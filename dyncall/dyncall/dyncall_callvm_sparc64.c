/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_sparc64.c
 Description: Call VM for sparc64 (v9) ABI.
 License:

   Copyright (c) 2011-2015 Daniel Adler <dadler@uni-goettingen.de>

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



#include "dyncall_callvm_sparc64.h"
#include "dyncall_call_sparc64.h"
#include "dyncall_alloc.h"

/* Reset argument buffer. */
static void dc_callvm_reset_v9(DCCallVM* in_self)
{
  DCCallVM_v9* self = (DCCallVM_v9*)in_self;
  dcVecResize(&self->mVecHead, 0);
}


/* Destructor. */
static void dc_callvm_free_v9(DCCallVM* in_self)
{
  dcFreeMem(in_self);
}

static void dc_callvm_argLongLong_v9(DCCallVM* in_self, DClonglong x)
{
  DCCallVM_v9* self = (DCCallVM_v9*)in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DClonglong));
}

/* all integers are promoted to 64-bit. */
static void dc_callvm_argLong_v9   (DCCallVM* in_self, DClong    x) { dc_callvm_argLongLong_v9(in_self, (DClonglong) x ); }
static void dc_callvm_argInt_v9    (DCCallVM* in_self, DCint     x) { dc_callvm_argLongLong_v9(in_self, (DClonglong) x ); }
static void dc_callvm_argBool_v9   (DCCallVM* in_self, DCbool    x) { dc_callvm_argLongLong_v9(in_self, (DClonglong) x ); }
static void dc_callvm_argChar_v9   (DCCallVM* in_self, DCchar    x) { dc_callvm_argLongLong_v9(in_self, (DClonglong) x ); }
static void dc_callvm_argShort_v9  (DCCallVM* in_self, DCshort   x) { dc_callvm_argLongLong_v9(in_self, (DClonglong) x ); }
static void dc_callvm_argPointer_v9(DCCallVM* in_self, DCpointer x) { dc_callvm_argLongLong_v9(in_self, (DClonglong) x ); }

static void dc_callvm_argDouble_v9(DCCallVM* in_self, DCdouble x)
{
  DCCallVM_v9* self = (DCCallVM_v9*)in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DCdouble));
}

static void dc_callvm_argDouble_v9_ellipsis(DCCallVM* in_self, DCdouble x)
{
  union {
    long long l;
    double d;
  } u;
  u.d = x;
  dc_callvm_argLongLong_v9(in_self, u.l);
}

static void dc_callvm_argFloat_v9_ellipsis(DCCallVM* in_self, DCfloat x)
{
  dc_callvm_argDouble_v9_ellipsis(in_self, (DCdouble) x);
}

static void dc_callvm_argFloat_v9(DCCallVM* in_self, DCfloat x)
{
  union {
    double d;
    float  f[2];
  } u;
  u.f[1] = x;
  dc_callvm_argDouble_v9(in_self, u.d);
}

static void dc_callvm_mode_v9(DCCallVM* in_self, DCint mode);

DCCallVM_vt gVT_v9_ellipsis = 
{
  &dc_callvm_free_v9, 
  &dc_callvm_reset_v9, 
  &dc_callvm_mode_v9, 
  &dc_callvm_argBool_v9, 
  &dc_callvm_argChar_v9, 
  &dc_callvm_argShort_v9, 
  &dc_callvm_argInt_v9, 
  &dc_callvm_argLong_v9, 
  &dc_callvm_argLongLong_v9, 
  &dc_callvm_argFloat_v9_ellipsis, 
  &dc_callvm_argDouble_v9_ellipsis, 
  &dc_callvm_argPointer_v9, 
  NULL /* argStruct */, 
  (DCvoidvmfunc*)       &dcCall_v9, 
  (DCboolvmfunc*)       &dcCall_v9, 
  (DCcharvmfunc*)       &dcCall_v9, 
  (DCshortvmfunc*)      &dcCall_v9, 
  (DCintvmfunc*)        &dcCall_v9, 
  (DClongvmfunc*)       &dcCall_v9, 
  (DClonglongvmfunc*)   &dcCall_v9, 
  (DCfloatvmfunc*)      &dcCall_v9, 
  (DCdoublevmfunc*)     &dcCall_v9, 
  (DCpointervmfunc*)    &dcCall_v9, 
  NULL /* callStruct */
};

/* CallVM virtual table. */
DCCallVM_vt gVT_v9 =
{
  &dc_callvm_free_v9, 
  &dc_callvm_reset_v9, 
  &dc_callvm_mode_v9, 
  &dc_callvm_argBool_v9, 
  &dc_callvm_argChar_v9, 
  &dc_callvm_argShort_v9, 
  &dc_callvm_argInt_v9, 
  &dc_callvm_argLong_v9, 
  &dc_callvm_argLongLong_v9, 
  &dc_callvm_argFloat_v9, 
  &dc_callvm_argDouble_v9, 
  &dc_callvm_argPointer_v9, 
  NULL /* argStruct */, 
  (DCvoidvmfunc*)       &dcCall_v9, 
  (DCboolvmfunc*)       &dcCall_v9, 
  (DCcharvmfunc*)       &dcCall_v9, 
  (DCshortvmfunc*)      &dcCall_v9, 
  (DCintvmfunc*)        &dcCall_v9, 
  (DClongvmfunc*)       &dcCall_v9, 
  (DClonglongvmfunc*)   &dcCall_v9, 
  (DCfloatvmfunc*)      &dcCall_v9, 
  (DCdoublevmfunc*)     &dcCall_v9, 
  (DCpointervmfunc*)    &dcCall_v9, 
  NULL /* callStruct */
};

/* mode: only a single mode available currently. */
static void dc_callvm_mode_v9(DCCallVM* in_self, DCint mode)
{
  DCCallVM_v9* self = (DCCallVM_v9*)in_self;
  DCCallVM_vt* vt;

  switch(mode) {
    case DC_CALL_C_DEFAULT:
    case DC_CALL_C_SPARC64:
    case DC_CALL_C_ELLIPSIS:
      vt = &gVT_v9;
      break;
    case DC_CALL_C_ELLIPSIS_VARARGS:
      vt = &gVT_v9_ellipsis;
      break;
    default:
      self->mInterface.mError = DC_ERROR_UNSUPPORTED_MODE; 
      return; 
  }
  dc_callvm_base_init(&self->mInterface, vt);
}

/* Public API. */
DCCallVM* dcNewCallVM(DCsize size)
{
  DCCallVM_v9* p = (DCCallVM_v9*)dcAllocMem(sizeof(DCCallVM_v9)+size);

  dc_callvm_mode_v9((DCCallVM*)p, DC_CALL_C_DEFAULT);

  dcVecInit(&p->mVecHead,size);
  dc_callvm_reset_v9(&p->mInterface);

  return (DCCallVM*)p;
}

