/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_ppc64.c
 Description: 
 License:

   Copyright (c) 2014-2015 Masanori Mitsugi <mitsugi@linux.vnet.ibm.com>

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

  dyncall callvm for ppc64 architectures

  SUPPORTED CALLING CONVENTIONS
  ppc64/linux
  ppc64/syscall

  REVISION
  2015/07/08 added syscall
  2014/08/07 initial support

*/

#include "dyncall_callvm_ppc64.h"
#include "dyncall_call_ppc64.h"
#include "dyncall_alloc.h"
#include "dyncall_macros.h"
#include "dyncall_types.h"

/* Support for Power PC 64-bit */

static void dc_callvm_free_ppc64(DCCallVM* in_self)
{
  dcFreeMem(in_self);
}

static void dc_callvm_reset_ppc64(DCCallVM* in_self)
{
  DCCallVM_ppc64* self = (DCCallVM_ppc64*)in_self;
  dcVecReset(&self->mVecHead);
  self->mIntRegs   = 0;
  self->mFloatRegs = 0;
}

/* fillup integer register file AND push on stack (for ellipsis) */

static void dc_callvm_argInt_ppc64(DCCallVM* in_self, DCint i)
{
  /* promote to longlong */
  dcArgLongLong(in_self, (DClonglong)i);
}

/** floating-point **/

/* double*/

static void dc_callvm_argDouble_ppc64(DCCallVM* in_self, DCdouble d)
{
  DCCallVM_ppc64* self = (DCCallVM_ppc64*)in_self;

  if (self->mFloatRegs < 13) {
    self->mRegData.mFloatData[self->mFloatRegs++] = d;
    if (self->mIntRegs < 8) {
      self->mRegData.mIntData[self->mIntRegs++] = *( (DClonglong*) &d );
#if DC__ABI_PPC64_ELF_V == 2
      return;
#endif
    }
  }

#if DC__ABI_PPC64_ELF_V == 2
  if (dcVecSize(&self->mVecHead) == 0) {
    dcVecSkip(&self->mVecHead,sizeof(DClonglong)*8);
  }
#endif

  /* push on stack */
  dcVecAppend(&self->mVecHead,(DCpointer) &d,sizeof(DCdouble));
}

#if DC__ABI_PPC64_ELF_V == 2
static void dc_callvm_argDouble_ppc64_ellipsis(DCCallVM* in_self, DCdouble d)
{
  DCCallVM_ppc64* self = (DCCallVM_ppc64*)in_self;

  if (dcVecSize(&self->mVecHead) == 0)
    dcVecSkip(&self->mVecHead,(sizeof(DClonglong))*(self->mIntRegs));

  if (self->mFloatRegs < 13) {
    self->mRegData.mFloatData[self->mFloatRegs++] = d;
    if (self->mIntRegs < 8) {
      self->mRegData.mIntData[self->mIntRegs++] = *( (DClonglong*) &d );
    }
  }

  /* push on stack */
  dcVecAppend(&self->mVecHead,(DCpointer) &d,sizeof(DCdouble));
}
#endif


/* Floating-point */

static void dc_callvm_argFloat_ppc64(DCCallVM* in_self, DCfloat f)
{
  DCCallVM_ppc64* self = (DCCallVM_ppc64*)in_self;

  DCdouble d;
#if defined(DC__Endian_BIG)
  struct { DCfloat f_pad; DCfloat f; } sf;
#else /* Endian_LITTLE */
  struct { DCfloat f; DCfloat f_pad; } sf;
#endif

  if (self->mFloatRegs < 13) {
    d = (DCdouble)f;
    self->mRegData.mFloatData[self->mFloatRegs++] = d;
    if (self->mIntRegs < 8) {
      self->mRegData.mIntData[self->mIntRegs++] = *( (DClonglong*) &d );
#if DC__ABI_PPC64_ELF_V == 2
      return;
#endif
    }
  }

#if DC__ABI_PPC64_ELF_V == 2
  if (dcVecSize(&self->mVecHead) == 0) {
    dcVecSkip(&self->mVecHead,sizeof(DClonglong)*8);
  }
#endif

  /* push on stack */
  sf.f = f;
  dcVecAppend(&self->mVecHead,(DCpointer) &sf,sizeof(DCdouble));
}

#if DC__ABI_PPC64_ELF_V == 2
static void dc_callvm_argFloat_ppc64_ellipsis(DCCallVM* in_self, DCfloat f)
{
  /* promote to double */
  dcArgDouble(in_self, (DCdouble) f );
}
#endif


/* long long integer */

static void dc_callvm_argLongLong_ppc64(DCCallVM* in_self, DClonglong L)
{
  DCCallVM_ppc64* self = (DCCallVM_ppc64*)in_self;

  /* fillup integer register file */
  if (self->mIntRegs < 8) {
    self->mRegData.mIntData[self->mIntRegs++] = L;
#if DC__ABI_PPC64_ELF_V == 2
    return;
#endif
  }

#if DC__ABI_PPC64_ELF_V == 2
  if (dcVecSize(&self->mVecHead) == 0) {
    dcVecSkip(&self->mVecHead,sizeof(DClonglong)*8);
  }
#endif

  /* push on stack */
  dcVecAppend(&self->mVecHead,&L,sizeof(DClonglong));
}

#if DC__ABI_PPC64_ELF_V == 2
static void dc_callvm_argLongLong_ppc64_ellipsis(DCCallVM* in_self, DClonglong L)
{
  DCCallVM_ppc64* self = (DCCallVM_ppc64*)in_self;

  if (dcVecSize(&self->mVecHead) == 0) 
    dcVecSkip(&self->mVecHead,(sizeof(DClonglong))*(self->mIntRegs));

  if (self->mIntRegs < 8) 
    self->mRegData.mIntData[self->mIntRegs++] = L;

  /* push on stack */
  dcVecAppend(&self->mVecHead,&L,sizeof(DClonglong));
}
#endif


static void dc_callvm_argBool_ppc64(DCCallVM* in_self, DCbool x)
{
  /* promote to longlong */
  dcArgLongLong(in_self, (DClonglong) x );
}


static void dc_callvm_argChar_ppc64(DCCallVM* in_self, DCchar ch)
{
  /* promote to longlong */
  dcArgLongLong(in_self, (DClonglong) ch );
}


static void dc_callvm_argShort_ppc64(DCCallVM* in_self, DCshort s)
{
  /* promote to longlong */
  dcArgLongLong(in_self, (DClonglong) s );
}


static void dc_callvm_argLong_ppc64(DCCallVM* in_self, DClong l)
{
  /* promote to longlong */
  dcArgLongLong(in_self, (DClonglong) l );
}


static void dc_callvm_argPointer_ppc64(DCCallVM* in_self, DCpointer p)
{
  /* promote to longlong */
  dcArgLongLong(in_self, *(DClonglong *) &p );
}


void dc_callvm_call_ppc64(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_ppc64* self = (DCCallVM_ppc64*) in_self;
  int size = dcVecSize(&self->mVecHead);

  if (size < 64) {
	dcVecSkip(&self->mVecHead, 64-size);
  }
  
  dcCall_ppc64( target, &self->mRegData, dcVecSize(&self->mVecHead) , dcVecData(&self->mVecHead));
}

void dc_callvm_call_ppc64_syscall(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_ppc64* self = (DCCallVM_ppc64*) in_self;
  dcCall_ppc64_syscall( target, &self->mRegData, dcVecSize(&self->mVecHead) , dcVecData(&self->mVecHead));
}

void dc_callvm_mode_ppc64(DCCallVM* in_self, DCint mode);

DCCallVM_vt gVT_ppc64 =
{
  &dc_callvm_free_ppc64
, &dc_callvm_reset_ppc64
, &dc_callvm_mode_ppc64
, &dc_callvm_argBool_ppc64
, &dc_callvm_argChar_ppc64
, &dc_callvm_argShort_ppc64
, &dc_callvm_argInt_ppc64
, &dc_callvm_argLong_ppc64
, &dc_callvm_argLongLong_ppc64
, &dc_callvm_argFloat_ppc64
, &dc_callvm_argDouble_ppc64
, &dc_callvm_argPointer_ppc64
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_ppc64
, (DCboolvmfunc*)       &dc_callvm_call_ppc64
, (DCcharvmfunc*)       &dc_callvm_call_ppc64
, (DCshortvmfunc*)      &dc_callvm_call_ppc64
, (DCintvmfunc*)        &dc_callvm_call_ppc64
, (DClongvmfunc*)       &dc_callvm_call_ppc64
, (DClonglongvmfunc*)   &dc_callvm_call_ppc64
, (DCfloatvmfunc*)      &dc_callvm_call_ppc64
, (DCdoublevmfunc*)     &dc_callvm_call_ppc64
, (DCpointervmfunc*)    &dc_callvm_call_ppc64
, NULL /* callStruct */
};

#if DC__ABI_PPC64_ELF_V == 2
DCCallVM_vt gVT_ppc64_ellipsis =
{
  &dc_callvm_free_ppc64
, &dc_callvm_reset_ppc64
, &dc_callvm_mode_ppc64
, &dc_callvm_argBool_ppc64
, &dc_callvm_argChar_ppc64
, &dc_callvm_argShort_ppc64
, &dc_callvm_argInt_ppc64
, &dc_callvm_argLong_ppc64
, &dc_callvm_argLongLong_ppc64_ellipsis
, &dc_callvm_argFloat_ppc64_ellipsis
, &dc_callvm_argDouble_ppc64_ellipsis
, &dc_callvm_argPointer_ppc64
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_ppc64
, (DCboolvmfunc*)       &dc_callvm_call_ppc64
, (DCcharvmfunc*)       &dc_callvm_call_ppc64
, (DCshortvmfunc*)      &dc_callvm_call_ppc64
, (DCintvmfunc*)        &dc_callvm_call_ppc64
, (DClongvmfunc*)       &dc_callvm_call_ppc64
, (DClonglongvmfunc*)   &dc_callvm_call_ppc64
, (DCfloatvmfunc*)      &dc_callvm_call_ppc64
, (DCdoublevmfunc*)     &dc_callvm_call_ppc64
, (DCpointervmfunc*)    &dc_callvm_call_ppc64
, NULL /* callStruct */
};
#endif

DCCallVM_vt gVT_ppc64_syscall =
{
  &dc_callvm_free_ppc64
, &dc_callvm_reset_ppc64
, &dc_callvm_mode_ppc64
, &dc_callvm_argBool_ppc64
, &dc_callvm_argChar_ppc64
, &dc_callvm_argShort_ppc64
, &dc_callvm_argInt_ppc64
, &dc_callvm_argLong_ppc64
, &dc_callvm_argLongLong_ppc64
, &dc_callvm_argFloat_ppc64
, &dc_callvm_argDouble_ppc64
, &dc_callvm_argPointer_ppc64
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_ppc64_syscall
, (DCboolvmfunc*)       &dc_callvm_call_ppc64_syscall
, (DCcharvmfunc*)       &dc_callvm_call_ppc64_syscall
, (DCshortvmfunc*)      &dc_callvm_call_ppc64_syscall
, (DCintvmfunc*)        &dc_callvm_call_ppc64_syscall
, (DClongvmfunc*)       &dc_callvm_call_ppc64_syscall
, (DClonglongvmfunc*)   &dc_callvm_call_ppc64_syscall
, (DCfloatvmfunc*)      &dc_callvm_call_ppc64_syscall
, (DCdoublevmfunc*)     &dc_callvm_call_ppc64_syscall
, (DCpointervmfunc*)    &dc_callvm_call_ppc64_syscall
, NULL /* callStruct */
};

void dc_callvm_mode_ppc64(DCCallVM* in_self, DCint mode)
{
  DCCallVM_ppc64* self = (DCCallVM_ppc64*) in_self;
  DCCallVM_vt* vt;

  switch(mode) {
    case DC_CALL_C_DEFAULT:
    case DC_CALL_C_PPC64: 
    case DC_CALL_C_ELLIPSIS:
#if DC__ABI_PPC64_ELF_V == 2
      vt = &gVT_ppc64;
      break;
    case DC_CALL_C_ELLIPSIS_VARARGS:
      vt = &gVT_ppc64_ellipsis;
      break;
#else
    case DC_CALL_C_ELLIPSIS_VARARGS:
      vt = &gVT_ppc64;
      break;
#endif

    case DC_CALL_SYS_DEFAULT:
    case DC_CALL_SYS_PPC64:
      vt = &gVT_ppc64_syscall;
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
  DCCallVM_ppc64* p = (DCCallVM_ppc64*)dcAllocMem(sizeof(DCCallVM_ppc64)+size);

  dc_callvm_mode_ppc64((DCCallVM*)p, DC_CALL_C_DEFAULT);

  dcVecInit(&p->mVecHead, size);
  dc_callvm_reset_ppc64((DCCallVM*)p);

  return (DCCallVM*)p;
}

