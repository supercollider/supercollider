/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_arm32_arm.c
 Description: ARM 32-bit "arm" ABI callvm implementation
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



/*

  dyncall callvm for 32bit ARM32 family of processors

  SUPPORTED CALLING CONVENTIONS
  armcall

  REVISION
  2007/12/11 initial

*/


#include "dyncall_callvm_arm32_arm.h"
#include "dyncall_alloc.h"

static void dc_callvm_free_arm32_arm(DCCallVM* in_self)
{
  dcFreeMem(in_self);
}


static void dc_callvm_reset_arm32_arm(DCCallVM* in_self)
{
  DCCallVM_arm32_arm* self = (DCCallVM_arm32_arm*)in_self;
  dcVecReset(&self->mVecHead);
}


static void dc_callvm_argInt_arm32_arm(DCCallVM* in_self, DCint x)
{
  DCCallVM_arm32_arm* self = (DCCallVM_arm32_arm*)in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DCint));
}


static void dc_callvm_argBool_arm32_arm(DCCallVM* in_self, DCbool x)
{
  dc_callvm_argInt_arm32_arm(in_self, (DCint)x);
}


static void dc_callvm_argChar_arm32_arm(DCCallVM* in_self, DCchar x)
{
  dc_callvm_argInt_arm32_arm(in_self, x);
}


static void dc_callvm_argShort_arm32_arm(DCCallVM* in_self, DCshort x)
{
  dc_callvm_argInt_arm32_arm(in_self, x);
}


static void dc_callvm_argLong_arm32_arm(DCCallVM* in_self, DClong x)
{
  dc_callvm_argInt_arm32_arm(in_self, x);
}


static void dc_callvm_argLongLong_arm32_arm(DCCallVM* in_self, DClonglong x)
{
  DCCallVM_arm32_arm* self = (DCCallVM_arm32_arm*)in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DClonglong));
}


static void dc_callvm_argLongLong_arm32_arm_eabi(DCCallVM* in_self, DClonglong x)
{
  DCCallVM_arm32_arm* self = (DCCallVM_arm32_arm*)in_self;

  /* 64 bit values need to be aligned on 8 byte boundaries */
  dcVecSkip(&self->mVecHead, dcVecSize(&self->mVecHead) & 4);
  dcVecAppend(&self->mVecHead, &x, sizeof(DClonglong));
}


static void dc_callvm_argFloat_arm32_arm(DCCallVM* in_self, DCfloat x)
{
  DCCallVM_arm32_arm* self = (DCCallVM_arm32_arm*)in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DCfloat));
}


static void dc_callvm_argDouble_arm32_arm(DCCallVM* in_self, DCdouble x)
{
  DCCallVM_arm32_arm* self = (DCCallVM_arm32_arm*)in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DCdouble));
}


static void dc_callvm_argDouble_arm32_arm_eabi(DCCallVM* in_self, DCdouble x)
{
  DCCallVM_arm32_arm* self = (DCCallVM_arm32_arm*)in_self;

  /* 64 bit values need to be aligned on 8 byte boundaries */
  dcVecSkip(&self->mVecHead, dcVecSize(&self->mVecHead) & 4);
  dcVecAppend(&self->mVecHead, &x, sizeof(DCdouble));
}


static void dc_callvm_argPointer_arm32_arm(DCCallVM* in_self, DCpointer x)
{
  DCCallVM_arm32_arm* self = (DCCallVM_arm32_arm*)in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DCpointer));
}


/* Call. */
void dc_callvm_call_arm32_arm(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_arm32_arm* self = (DCCallVM_arm32_arm*)in_self;
  dcCall_arm32_arm(target, dcVecData(&self->mVecHead), dcVecSize(&self->mVecHead));
}

static void dc_callvm_mode_arm32_arm(DCCallVM* in_self, DCint mode);

DCCallVM_vt gVT_arm32_arm =
{
  &dc_callvm_free_arm32_arm
, &dc_callvm_reset_arm32_arm
, &dc_callvm_mode_arm32_arm
, &dc_callvm_argBool_arm32_arm
, &dc_callvm_argChar_arm32_arm
, &dc_callvm_argShort_arm32_arm 
, &dc_callvm_argInt_arm32_arm
, &dc_callvm_argLong_arm32_arm
, &dc_callvm_argLongLong_arm32_arm
, &dc_callvm_argFloat_arm32_arm
, &dc_callvm_argDouble_arm32_arm
, &dc_callvm_argPointer_arm32_arm
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_arm32_arm
, (DCboolvmfunc*)       &dc_callvm_call_arm32_arm
, (DCcharvmfunc*)       &dc_callvm_call_arm32_arm
, (DCshortvmfunc*)      &dc_callvm_call_arm32_arm
, (DCintvmfunc*)        &dc_callvm_call_arm32_arm
, (DClongvmfunc*)       &dc_callvm_call_arm32_arm
, (DClonglongvmfunc*)   &dc_callvm_call_arm32_arm
, (DCfloatvmfunc*)      &dc_callvm_call_arm32_arm
, (DCdoublevmfunc*)     &dc_callvm_call_arm32_arm
, (DCpointervmfunc*)    &dc_callvm_call_arm32_arm
, NULL /* callStruct */
};

DCCallVM_vt gVT_arm32_arm_eabi =
{
  &dc_callvm_free_arm32_arm
, &dc_callvm_reset_arm32_arm
, &dc_callvm_mode_arm32_arm
, &dc_callvm_argBool_arm32_arm
, &dc_callvm_argChar_arm32_arm
, &dc_callvm_argShort_arm32_arm 
, &dc_callvm_argInt_arm32_arm
, &dc_callvm_argLong_arm32_arm
, &dc_callvm_argLongLong_arm32_arm_eabi
, &dc_callvm_argFloat_arm32_arm
, &dc_callvm_argDouble_arm32_arm_eabi
, &dc_callvm_argPointer_arm32_arm
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_arm32_arm
, (DCboolvmfunc*)       &dc_callvm_call_arm32_arm
, (DCcharvmfunc*)       &dc_callvm_call_arm32_arm
, (DCshortvmfunc*)      &dc_callvm_call_arm32_arm
, (DCintvmfunc*)        &dc_callvm_call_arm32_arm
, (DClongvmfunc*)       &dc_callvm_call_arm32_arm
, (DClonglongvmfunc*)   &dc_callvm_call_arm32_arm
, (DCfloatvmfunc*)      &dc_callvm_call_arm32_arm
, (DCdoublevmfunc*)     &dc_callvm_call_arm32_arm
, (DCpointervmfunc*)    &dc_callvm_call_arm32_arm
, NULL /* callStruct */
};

static void dc_callvm_mode_arm32_arm(DCCallVM* in_self, DCint mode)
{
  DCCallVM_arm32_arm* self = (DCCallVM_arm32_arm*)in_self;
  DCCallVM_vt* vt;

  switch(mode) {
    case DC_CALL_C_ELLIPSIS:
    case DC_CALL_C_ELLIPSIS_VARARGS:
/* Check OS if we need EABI as default. */
#if defined(DC__ABI_ARM_EABI)
    case DC_CALL_C_DEFAULT:       vt = &gVT_arm32_arm_eabi; break;
#else
    case DC_CALL_C_DEFAULT:       vt = &gVT_arm32_arm;      break;
#endif
    case DC_CALL_C_ARM_ARM:       vt = &gVT_arm32_arm;      break;
    case DC_CALL_C_ARM_ARM_EABI:  vt = &gVT_arm32_arm_eabi; break;
    default: 
      self->mInterface.mError = DC_ERROR_UNSUPPORTED_MODE; 
      return;
  }
  dc_callvm_base_init(&self->mInterface, vt);
}

/* Public API. */
DCCallVM* dcNewCallVM(DCsize size)
{
  /* Store at least 16 bytes (4 words) for internal spill area. Assembly code depends on it. */
  DCCallVM_arm32_arm* p = (DCCallVM_arm32_arm*)dcAllocMem(sizeof(DCCallVM_arm32_arm)+size+16);

  dc_callvm_mode_arm32_arm((DCCallVM*)p, DC_CALL_C_DEFAULT);

  dcVecInit(&p->mVecHead, size);

  return (DCCallVM*)p;
}

