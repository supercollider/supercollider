/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_mips_o32.c
 Description: mips "o32" ABI callvm implementation
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

  dyncall callvm for mips o32 abi

  REVISION
  2010/06/03 initial

  NOTES:
  we need an argument counter for supporting floating point arguments
  correctly.

  first two (if any) double/float arguments are mapped via a common structure --
  code must take care to write the right float argument indices which
  differs on C and Assembly-side depending on endianness. (therefore
  both sources have two variants 'mipseb' and 'mipsel'.)
  (only for the first two float/double arguments) see float/double handling

  although, the abi does not expect usage of floats if first argument is
  not floating point, the call kernel can be used universal for all cases.

*/


#include "dyncall_callvm_mips_o32.h"
#include "dyncall_alloc.h"
#include "dyncall_utils.h"


static void dc_callvm_reset_mips_o32(DCCallVM* in_self)
{
  DCCallVM_mips_o32* self = (DCCallVM_mips_o32*)in_self;
  dcVecReset(&self->mVecHead);
  self->mArgCount = 0;
}


static void dc_callvm_free_mips_o32(DCCallVM* in_self)
{
  dcFreeMem(in_self);
}

/* arg int -- fillup integer register file OR push on stack */

static void dc_callvm_argInt_mips_o32(DCCallVM* in_self, DCint i)
{
  DCCallVM_mips_o32* self = (DCCallVM_mips_o32*)in_self;
  dcVecAppend(&self->mVecHead, &i, sizeof(DCint));
  self->mArgCount++;
}

static void dc_callvm_argPointer_mips_o32(DCCallVM* in_self, DCpointer x)
{
  dc_callvm_argInt_mips_o32(in_self, * (DCint*) &x );
}

static void dc_callvm_argBool_mips_o32(DCCallVM* in_self, DCbool x)
{
  dc_callvm_argInt_mips_o32(in_self, (DCint)x);
}

static void dc_callvm_argChar_mips_o32(DCCallVM* in_self, DCchar x)
{
  dc_callvm_argInt_mips_o32(in_self, (DCint)x);
}

static void dc_callvm_argShort_mips_o32(DCCallVM* in_self, DCshort x)
{
  dc_callvm_argInt_mips_o32(in_self, (DCint)x);
}

static void dc_callvm_argLong_mips_o32(DCCallVM* in_self, DClong x)
{
  dc_callvm_argInt_mips_o32(in_self, (DCint)x);
}

static void dc_callvm_argLongLong_mips_o32(DCCallVM* in_self, DClonglong Lv)
{
  DCCallVM_mips_o32* self = (DCCallVM_mips_o32*)in_self;
    
  /* 64-bit values need to be aligned on 8 byte boundaries */
  dcVecSkip(&self->mVecHead, dcVecSize(&self->mVecHead) & 4);
  dcVecAppend(&self->mVecHead, &Lv, sizeof(DClonglong));
  self->mArgCount += 1;
}

static void dc_callvm_argFloat_mips_o32(DCCallVM* in_self, DCfloat x)
{
  DCCallVM_mips_o32* self = (DCCallVM_mips_o32*)in_self;

  dcVecAppend(&self->mVecHead, &x, sizeof(DCfloat) );
  if (self->mArgCount < 2) {
#if defined(DC__Endian_LITTLE)
    self->mRegData.u[self->mArgCount].f[0] = x;
#else
    self->mRegData.u[self->mArgCount].f[1] = x;
#endif
#if 0
    self->mRegData.u[self->mArgCount].f[1] = x;
   call kernel
        
        mips:
        lwc1	$f12, 4($5)       <--- byte offset 4
	lwc1	$f13, 0($5)
	lwc1	$f14, 12($5)      <--- byte offset 12 
	lwc1	$f15, 8($5)
        mipsel:
        lwc1	$f12, 0($5)       <--- byte offset 4
	lwc1	$f13, 4($5)
	lwc1	$f14, 8($5)      <--- byte offset 12 
	lwc1	$f15, 12($5)

#if defined(DC__Endian_LITTLE)
    /* index 0 and 2 */
    self->mRegData.floats[self->mArgCount*2] = x;
#else
    /* index 1 and 3 */
    self->mRegData.floats[self->mArgCount*2+1] = x;
#endif
#endif
  }
  self->mArgCount++;
}

static void dc_callvm_argDouble_mips_o32(DCCallVM* in_self, DCdouble x)
{
  DCCallVM_mips_o32* self = (DCCallVM_mips_o32*)in_self;
  /* 64-bit values need to be aligned on 8 byte boundaries */
  dcVecSkip(&self->mVecHead, dcVecSize(&self->mVecHead) & 4);
  dcVecAppend(&self->mVecHead, &x, sizeof(DCdouble) );
  if (self->mArgCount < 2)
    self->mRegData.u[self->mArgCount].d = x;
  self->mArgCount++;
}

/* Call. */
void dc_callvm_call_mips_o32(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_mips_o32* self = (DCCallVM_mips_o32*)in_self;
  /* at minimum provide 16-bytes
     which hold the first four integer register as spill area 
     and are automatically loaded to $4-$7
   */

  size_t size = DC_MAX(16, ( ( (unsigned) dcVecSize(&self->mVecHead) ) +7UL ) & (-8UL) );

  dcCall_mips_o32(target, &self->mRegData, size, dcVecData(&self->mVecHead));
}

static void dc_callvm_mode_mips_o32(DCCallVM* in_self, DCint mode);

DCCallVM_vt gVT_mips_o32 =
{
  &dc_callvm_free_mips_o32
, &dc_callvm_reset_mips_o32
, &dc_callvm_mode_mips_o32
, &dc_callvm_argBool_mips_o32
, &dc_callvm_argChar_mips_o32
, &dc_callvm_argShort_mips_o32 
, &dc_callvm_argInt_mips_o32
, &dc_callvm_argLong_mips_o32
, &dc_callvm_argLongLong_mips_o32
, &dc_callvm_argFloat_mips_o32
, &dc_callvm_argDouble_mips_o32
, &dc_callvm_argPointer_mips_o32
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_mips_o32
, (DCboolvmfunc*)       &dc_callvm_call_mips_o32
, (DCcharvmfunc*)       &dc_callvm_call_mips_o32
, (DCshortvmfunc*)      &dc_callvm_call_mips_o32
, (DCintvmfunc*)        &dc_callvm_call_mips_o32
, (DClongvmfunc*)       &dc_callvm_call_mips_o32
, (DClonglongvmfunc*)   &dc_callvm_call_mips_o32
, (DCfloatvmfunc*)      &dc_callvm_call_mips_o32
, (DCdoublevmfunc*)     &dc_callvm_call_mips_o32
, (DCpointervmfunc*)    &dc_callvm_call_mips_o32
, NULL /* callStruct */
};

/* mode: only a single mode available currently. */
static void dc_callvm_mode_mips_o32(DCCallVM* in_self, DCint mode)
{
  DCCallVM_mips_o32* self = (DCCallVM_mips_o32*)in_self;
  DCCallVM_vt* vt;

  switch(mode) {
    case DC_CALL_C_DEFAULT:
    case DC_CALL_C_MIPS32_O32:
    case DC_CALL_C_ELLIPSIS:
    case DC_CALL_C_ELLIPSIS_VARARGS:
      vt = &gVT_mips_o32;
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
  DCCallVM_mips_o32* p = (DCCallVM_mips_o32*)dcAllocMem(sizeof(DCCallVM_mips_o32)+size);

  dc_callvm_mode_mips_o32((DCCallVM*)p, DC_CALL_C_DEFAULT);

  dcVecInit(&p->mVecHead, size);
  dc_callvm_reset_mips_o32((DCCallVM*)p);

  return (DCCallVM*)p;
}

