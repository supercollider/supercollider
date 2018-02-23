/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_mips_n32.c
 Description: mips64 "n32" ABI callvm implementation
 License:

   Copyright (c) 2007-2016 Daniel Adler <dadler@uni-goettingen.de>,
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

  dyncall callvm for 64bit MIPS family of processors

  SUPPORTED CALLING CONVENTIONS
  n32

  REVISION
  2010/05/30 initial

  in contrast to o32, there is no space reserved for parameters
  passed over registers.
  more registers are used and registers are always used.

  stack is always 16-byte aligned at entry (call to call-kernel automatically
  aligns argument stack.

  integer and float register-file is interleaved either taking one slot up
  skipping the other.


*/


#include "dyncall_callvm_mips_n32.h"
#include "dyncall_alloc.h"
#include "dyncall_utils.h"

static void dc_callvm_reset_mips_n32(DCCallVM* in_self)
{
  DCCallVM_mips_n32* self = (DCCallVM_mips_n32*)in_self;
  dcVecReset(&self->mVecHead);
  self->mRegCount = 0;
  self->mRegData.mUseDouble = 0LL;
}


static void dc_callvm_free_mips_n32(DCCallVM* in_self)
{
  dcFreeMem(in_self);
}

/* pass arguments :

   - promote to 64-bit integer.
   - fill up integers and float - left-to-right otherwise go over stack.
 */

/* arg int -- fillup 64-bit integer register file OR push on stack */

static void dc_callvm_argLongLong_mips_n32(DCCallVM* in_self, DClonglong Lv)
{
  DCCallVM_mips_n32* self = (DCCallVM_mips_n32*)in_self;
  /* fillup integer register file */
  if (self->mRegCount < 8)
    self->mRegData.mIntData[self->mRegCount++] = Lv;
  else
    dcVecAppend(&self->mVecHead, &Lv, sizeof(DClonglong));
}

static void dc_callvm_argInt_mips_n32(DCCallVM* in_self, DCint i)
{
  dc_callvm_argLongLong_mips_n32(in_self, (DClonglong) i );
}

static void dc_callvm_argPointer_mips_n32(DCCallVM* in_self, DCpointer x)
{
  dc_callvm_argLongLong_mips_n32(in_self, * (DClonglong*) &x );
}

static void dc_callvm_argBool_mips_n32(DCCallVM* in_self, DCbool x)
{
  dc_callvm_argLongLong_mips_n32(in_self, (DClonglong)x);
}

static void dc_callvm_argChar_mips_n32(DCCallVM* in_self, DCchar x)
{
  dc_callvm_argLongLong_mips_n32(in_self, (DClonglong)x);
}

static void dc_callvm_argShort_mips_n32(DCCallVM* in_self, DCshort x)
{
  dc_callvm_argLongLong_mips_n32(in_self, (DClonglong)x);
}

static void dc_callvm_argLong_mips_n32(DCCallVM* in_self, DClong x)
{
  dc_callvm_argLongLong_mips_n32(in_self, (DClonglong)x);
}

static void dc_callvm_argDouble_mips_n32(DCCallVM* in_self, DCdouble x)
{
  DCCallVM_mips_n32* self = (DCCallVM_mips_n32*)in_self;
  if (self->mRegCount < 8) {
    self->mRegData.mUseDouble |= 1<<( self->mRegCount );
    self->mRegData.mFloatData[self->mRegCount++].d = x;
  } else {
    dcVecAppend(&self->mVecHead, &x, sizeof(DCdouble) );
  }
}

static void dc_callvm_argFloat_mips_n32(DCCallVM* in_self, DCfloat x)
{
  DCCallVM_mips_n32* self = (DCCallVM_mips_n32*)in_self;
  if (self->mRegCount < 8) {
    /*self->mRegData.mFloatData[self->mRegCount++].d = (DCdouble) x;*/
    self->mRegData.mFloatData[self->mRegCount++].f = x;
  } else {
    dcVecAppend(&self->mVecHead, &x, sizeof(DCfloat) );
    dcVecSkip(&self->mVecHead, sizeof(DCfloat) );
  }
}


/* Ellipsis calls:
   - float is promoted to double (due to ANSI C).
   - double is passed via integer register-file (due to MIPS ABI).
*/

static void dc_callvm_argDouble_mips_n32_ellipsis(DCCallVM* in_self, DCdouble x)
{
  dc_callvm_argLongLong_mips_n32(in_self, * ( (DClonglong*) &x ) );
}

static void dc_callvm_argFloat_mips_n32_ellipsis(DCCallVM* in_self, DCfloat x)
{
  dc_callvm_argDouble_mips_n32_ellipsis(in_self, (DCdouble) x );
}


/* Call. */

void dc_callvm_call_mips_n32(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_mips_n32* self = (DCCallVM_mips_n32*)in_self;
  /* at minimum provide 16-bytes
     which hold the first four integer register as spill area
     and are automatically loaded to $4-$7
   */
  size_t size = DC_MAX(16, ( ( (unsigned) dcVecSize(&self->mVecHead) ) +7UL ) & (-8UL) );
  dcCall_mips_n32(target, &self->mRegData, size, dcVecData(&self->mVecHead));
}

/* Forward Declaration. */
static void dc_callvm_mode_mips_n32(DCCallVM* in_self,DCint mode);

DCCallVM_vt gVT_mips_n32 =
{
  &dc_callvm_free_mips_n32
, &dc_callvm_reset_mips_n32
, &dc_callvm_mode_mips_n32
, &dc_callvm_argBool_mips_n32
, &dc_callvm_argChar_mips_n32
, &dc_callvm_argShort_mips_n32
, &dc_callvm_argInt_mips_n32
, &dc_callvm_argLong_mips_n32
, &dc_callvm_argLongLong_mips_n32
, &dc_callvm_argFloat_mips_n32
, &dc_callvm_argDouble_mips_n32
, &dc_callvm_argPointer_mips_n32
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_mips_n32
, (DCboolvmfunc*)       &dc_callvm_call_mips_n32
, (DCcharvmfunc*)       &dc_callvm_call_mips_n32
, (DCshortvmfunc*)      &dc_callvm_call_mips_n32
, (DCintvmfunc*)        &dc_callvm_call_mips_n32
, (DClongvmfunc*)       &dc_callvm_call_mips_n32
, (DClonglongvmfunc*)   &dc_callvm_call_mips_n32
, (DCfloatvmfunc*)      &dc_callvm_call_mips_n32
, (DCdoublevmfunc*)     &dc_callvm_call_mips_n32
, (DCpointervmfunc*)    &dc_callvm_call_mips_n32
, NULL /* callStruct */
};

DCCallVM_vt gVT_mips_n32_ellipsis =
{
  &dc_callvm_free_mips_n32
, &dc_callvm_reset_mips_n32
, &dc_callvm_mode_mips_n32
, &dc_callvm_argBool_mips_n32
, &dc_callvm_argChar_mips_n32
, &dc_callvm_argShort_mips_n32
, &dc_callvm_argInt_mips_n32
, &dc_callvm_argLong_mips_n32
, &dc_callvm_argLongLong_mips_n32
, &dc_callvm_argFloat_mips_n32_ellipsis
, &dc_callvm_argDouble_mips_n32_ellipsis
, &dc_callvm_argPointer_mips_n32
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_mips_n32
, (DCboolvmfunc*)       &dc_callvm_call_mips_n32
, (DCcharvmfunc*)       &dc_callvm_call_mips_n32
, (DCshortvmfunc*)      &dc_callvm_call_mips_n32
, (DCintvmfunc*)        &dc_callvm_call_mips_n32
, (DClongvmfunc*)       &dc_callvm_call_mips_n32
, (DClonglongvmfunc*)   &dc_callvm_call_mips_n32
, (DCfloatvmfunc*)      &dc_callvm_call_mips_n32
, (DCdoublevmfunc*)     &dc_callvm_call_mips_n32
, (DCpointervmfunc*)    &dc_callvm_call_mips_n32
, NULL /* callStruct */
};

static void dc_callvm_mode_mips_n32(DCCallVM* in_self, DCint mode)
{
  DCCallVM_mips_n32* self = (DCCallVM_mips_n32*)in_self;
  DCCallVM_vt* vt;

  switch(mode) {
    case DC_CALL_C_DEFAULT:
    case DC_CALL_C_MIPS64_N32:
    case DC_CALL_C_ELLIPSIS:
      vt = &gVT_mips_n32;
      break;
    case DC_CALL_C_ELLIPSIS_VARARGS:
      vt = &gVT_mips_n32_ellipsis;
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
  DCCallVM_mips_n32* p = (DCCallVM_mips_n32*)dcAllocMem(sizeof(DCCallVM_mips_n32)+size);

  dc_callvm_mode_mips_n32((DCCallVM*)p, DC_CALL_C_DEFAULT);

  dcVecInit(&p->mVecHead, size);
  dc_callvm_reset_mips_n32((DCCallVM*)p);

  return (DCCallVM*)p;
}

