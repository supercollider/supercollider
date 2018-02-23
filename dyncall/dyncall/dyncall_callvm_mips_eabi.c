/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_mips_eabi.c
 Description: Implementation of Call VM for mips "eabi" abi.
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



#include "dyncall_callvm_mips_eabi.h"
#include "dyncall_alloc.h"

static void dc_callvm_reset_mips_eabi(DCCallVM* in_self)
{
  DCCallVM_mips_eabi* self = (DCCallVM_mips_eabi*)in_self;
  dcVecReset(&self->mVecHead);
  self->mIntRegs = 0;
  self->mSingleRegs = 0;
}

static void dc_callvm_free_mips_eabi(DCCallVM* in_self)
{
  dcFreeMem(in_self);
}

/* arg int -- fillup integer register file OR push on stack */

static void dc_callvm_argInt_mips_eabi(DCCallVM* in_self, DCint i)
{
  DCCallVM_mips_eabi* self = (DCCallVM_mips_eabi*)in_self;
  /* fillup integer register file */
  if (self->mIntRegs < 8)
    self->mRegData.mIntData[self->mIntRegs++] = i;
  else
    dcVecAppend(&self->mVecHead, &i, sizeof(DCint));
}

static void dc_callvm_argPointer_mips_eabi(DCCallVM* in_self, DCpointer x)
{
  dc_callvm_argInt_mips_eabi(in_self, * (DCint*) &x );
}

static void dc_callvm_argBool_mips_eabi(DCCallVM* in_self, DCbool x)
{
  dc_callvm_argInt_mips_eabi(in_self, (DCint)x);
}

static void dc_callvm_argChar_mips_eabi(DCCallVM* in_self, DCchar x)
{
  dc_callvm_argInt_mips_eabi(in_self, (DCint)x);
}

static void dc_callvm_argShort_mips_eabi(DCCallVM* in_self, DCshort x)
{
  dc_callvm_argInt_mips_eabi(in_self, (DCint)x);
}

static void dc_callvm_argLong_mips_eabi(DCCallVM* in_self, DClong x)
{
  dc_callvm_argInt_mips_eabi(in_self, (DCint)x);
}

static void dc_callvm_argLongLong_mips_eabi(DCCallVM* in_self, DClonglong Lv)
{
  DCCallVM_mips_eabi* self = (DCCallVM_mips_eabi*)in_self;

  if (self->mIntRegs < 7) {
    DCint* p = (DCint*) &Lv;
    /* skip odd register (align 64 bit) */
    self->mIntRegs += self->mIntRegs & 1;
    self->mRegData.mIntData[self->mIntRegs++] = p[0];
    self->mRegData.mIntData[self->mIntRegs++] = p[1];
  } else {
    self->mIntRegs = 8;
    /* 64 bit values need to be aligned on 8 byte boundaries */
    dcVecSkip(&self->mVecHead, dcVecSize(&self->mVecHead) & 4);
    dcVecAppend(&self->mVecHead, &Lv, sizeof(DClonglong));
  }
}

static void dc_callvm_argFloat_mips_eabi(DCCallVM* in_self, DCfloat x)
{
  DCCallVM_mips_eabi* self = (DCCallVM_mips_eabi*)in_self;
  if (self->mSingleRegs < 8) {
    self->mRegData.mSingleData[self->mSingleRegs++] = x;
  } else {
    dcVecAppend(&self->mVecHead, &x, sizeof(DCfloat) );
  }
}

static void dc_callvm_argDouble_mips_eabi(DCCallVM* in_self, DCdouble x)
{
  DClonglong* p = (DClonglong*) &x;
  dc_callvm_argLongLong_mips_eabi(in_self, *p);
}

/* Call. */
void dc_callvm_call_mips_eabi(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_mips_eabi* self = (DCCallVM_mips_eabi*)in_self;
  dcCall_mips_eabi(target, &self->mRegData, dcVecSize(&self->mVecHead), dcVecData(&self->mVecHead));
}

static void dc_callvm_mode_mips_eabi(DCCallVM* in_self, DCint mode);

DCCallVM_vt gVT_mips_eabi =
{
  &dc_callvm_free_mips_eabi
, &dc_callvm_reset_mips_eabi
, &dc_callvm_mode_mips_eabi
, &dc_callvm_argBool_mips_eabi
, &dc_callvm_argChar_mips_eabi
, &dc_callvm_argShort_mips_eabi 
, &dc_callvm_argInt_mips_eabi
, &dc_callvm_argLong_mips_eabi
, &dc_callvm_argLongLong_mips_eabi
, &dc_callvm_argFloat_mips_eabi
, &dc_callvm_argDouble_mips_eabi
, &dc_callvm_argPointer_mips_eabi
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_mips_eabi
, (DCboolvmfunc*)       &dc_callvm_call_mips_eabi
, (DCcharvmfunc*)       &dc_callvm_call_mips_eabi
, (DCshortvmfunc*)      &dc_callvm_call_mips_eabi
, (DCintvmfunc*)        &dc_callvm_call_mips_eabi
, (DClongvmfunc*)       &dc_callvm_call_mips_eabi
, (DClonglongvmfunc*)   &dc_callvm_call_mips_eabi
, (DCfloatvmfunc*)      &dc_callvm_call_mips_eabi
, (DCdoublevmfunc*)     &dc_callvm_call_mips_eabi
, (DCpointervmfunc*)    &dc_callvm_call_mips_eabi
, NULL /* callStruct */
};

/* mode: only a single mode available currently. */
static void dc_callvm_mode_mips_eabi(DCCallVM* in_self, DCint mode)
{
  DCCallVM_mips_eabi* self = (DCCallVM_mips_eabi*)in_self;
  DCCallVM_vt* vt;

  switch(mode) {
    case DC_CALL_C_DEFAULT:
    case DC_CALL_C_MIPS32_EABI:
    case DC_CALL_C_ELLIPSIS:
    case DC_CALL_C_ELLIPSIS_VARARGS:
      vt = &gVT_mips_eabi;
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
  /* Store at least 16 bytes (4 words) for internal spill area. Assembly code depends on it. */
  DCCallVM_mips_eabi* p = (DCCallVM_mips_eabi*)dcAllocMem(sizeof(DCCallVM_mips_eabi)+size+16);

  dc_callvm_mode_mips_eabi((DCCallVM*)p, DC_CALL_C_DEFAULT);

  dcVecInit(&p->mVecHead, size);
  dc_callvm_reset_mips_eabi((DCCallVM*)p);

  return (DCCallVM*)p;
}

