/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_ppc32.c
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



/*

  dyncall callvm for ppc32 architectures

  SUPPORTED CALLING CONVENTIONS
  ppc32/osx 
  ppc32/linux (sysv abi)
  ppc32/syscall 

  REVISION
  2015/01/15 added syscall (tested on Linux)
  2009/01/09 added System V ABI support
  2007/12/11 initial support for Darwin ABI

*/

#include "dyncall_callvm_ppc32.h"
#include "dyncall_call_ppc32.h"
#include "dyncall_alloc.h"
#include "dyncall_macros.h"
#include "dyncall_types.h"
#include "dyncall_utils.h"

/* Support for Mac OS X (Darwin) and Systen V ABI for Power PC 32-bit */

#if defined(DC_UNIX)
#  if defined(DC__OS_Darwin)
#    define DC__ABI_Darwin
#  else
#    define DC__ABI_SysV
#  endif
#else
#  error Unsupported OS for ppc32 architecture.
#endif

static void dc_callvm_free_ppc32(DCCallVM* in_self)
{
  dcFreeMem(in_self);
}

static void dc_callvm_reset_ppc32(DCCallVM* in_self)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;
  dcVecReset(&self->mVecHead);
  self->mIntRegs   = 0;
  self->mFloatRegs = 0;
}

/* OS X/Darwin: fillup integer register file AND push on stack (for ellipsis) */

static void dc_callvm_argInt_ppc32_darwin(DCCallVM* in_self, DCint i)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;
  /* fillup integer register file */
  if (self->mIntRegs < 8)
    self->mRegData.mIntData[self->mIntRegs++] = i;
  /* AND push onto stack */
  dcVecAppend(&self->mVecHead,&i,sizeof(DCint));
}

static void dc_callvm_argInt_ppc32_sysv(DCCallVM* in_self, DCint i)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;
  /* fillup integer register file */
  if (self->mIntRegs < 8)
    self->mRegData.mIntData[self->mIntRegs++] = i;
  /* OR push onto stack */
  else 
    dcVecAppend(&self->mVecHead,&i,sizeof(DCint));
}

/** floating-point **/

/* double*/

static void dc_callvm_argDouble_ppc32_darwin(DCCallVM* in_self, DCdouble d)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;
  if (self->mFloatRegs < 13) {
    self->mRegData.mFloatData[self->mFloatRegs++] = d;
    /* skip two integer register file entries */
    if (self->mIntRegs < 8) 
      self->mRegData.mIntData[self->mIntRegs++] = ( (DCint*) &d )[0];
    if (self->mIntRegs < 8) 
      self->mRegData.mIntData[self->mIntRegs++] = ( (DCint*) &d )[1];
  }
  /* push on stack */
  dcVecAppend(&self->mVecHead, &d, sizeof(DCdouble));
}

#if 0
static void dc_callvm_argDouble_ppc32_sysv(DCCallVM* in_self, DCdouble d)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;
  if (self->mFloatRegs < 8) {
    self->mRegData.mFloatData[self->mFloatRegs++] = d;
    /* skip two integer register file entries */
    if (self->mIntRegs < 8) 
      self->mRegData.mIntData[self->mIntRegs++] = ( (DCint*) &d )[0];
    if (self->mIntRegs < 8) 
      self->mRegData.mIntData[self->mIntRegs++] = ( (DCint*) &d )[1];
  }
  /* push on stack */
  dcVecAppend(&self->mVecHead, &d, sizeof(DCdouble));
}
#endif
static void dc_callvm_argDouble_ppc32_sysv(DCCallVM* in_self, DCdouble d)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;
  if (self->mFloatRegs < 8) 
    self->mRegData.mFloatData[self->mFloatRegs++] = d;
  else /* OR push data on stack */
  {
    /* align stack to 8 byte boundary */
  dcVecResize(&self->mVecHead , ( dcVecSize(&self->mVecHead) + 7UL ) & -8UL ); 
    /* AND push data */
    dcVecAppend(&self->mVecHead,(DCpointer) &d,sizeof(DCdouble));
  }
}

/* Floating-point */
  
  
/* darwin:
 * - skip one integer register file entry (write in - for ellipsis calls) 
 * sysv:
 * - 
 */

static void dc_callvm_argFloat_ppc32_darwin(DCCallVM* in_self, DCfloat f)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;
  if (self->mFloatRegs < 13) {
    self->mRegData.mFloatData[self->mFloatRegs++] = (DCdouble) (f);
  }
  
  /* AND skip one integer register file entry (write in - for ellipsis calls) */
  
  if (self->mIntRegs < 8) 
    self->mRegData.mIntData[self->mIntRegs++] = *( (DCint*) &f );
  
  /* AND push on stack */
  
  dcVecAppend(&self->mVecHead, &f, sizeof(DCfloat));
}

static void dc_callvm_argFloat_ppc32_sysv(DCCallVM* in_self, DCfloat f)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;

  /* Put as float register (casted to double) */

  if (self->mFloatRegs < 8)
    self->mRegData.mFloatData[self->mFloatRegs++] = (DCdouble) (f);
  
  else /* OR put float on stack */
    dcVecAppend(&self->mVecHead, &f, sizeof(DCfloat));
}

/* long long integer */

static void dc_callvm_argLongLong_ppc32_darwin(DCCallVM* in_self, DClonglong L)
{
  DCint* p = (DCint*) &L;
  dcArgInt(in_self, p[0]);
  dcArgInt(in_self, p[1]);
}

static void dc_callvm_argLongLong_ppc32_sysv(DCCallVM* in_self, DClonglong L)
{
  DCint* p = (DCint*) &L;
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;
  /* fillup integer register file */
  if (self->mIntRegs < 7)
  {
    /* next free integer register is even (r0, r2, r3) ? */
    /* if not, skip one integer */
    if (self->mIntRegs & 1) self->mIntRegs++;

    self->mRegData.mIntData[self->mIntRegs++] = p[0];
    self->mRegData.mIntData[self->mIntRegs++] = p[1];
  }
  /* OR push onto stack */
  else  
  {
    /* in case, mIntRegs == 7, set it to 8 */
    self->mIntRegs = 8;
    /* align stack to 8 byte boundary */
    dcVecResize(&self->mVecHead , ( dcVecSize(&self->mVecHead) + 7 ) & (-8UL) );
    /* push data */
    dcVecAppend(&self->mVecHead,&L,sizeof(DClonglong));
  }
}


static void dc_callvm_argBool_ppc32(DCCallVM* in_self, DCbool x)
{
  /* promote to integer */
  dcArgInt(in_self, (x == 0) ? DC_FALSE : DC_TRUE );
}


static void dc_callvm_argChar_ppc32(DCCallVM* in_self, DCchar ch)
{
  /* promote to integer */
  dcArgInt(in_self, (DCint) ch );
}


static void dc_callvm_argShort_ppc32(DCCallVM* in_self, DCshort s)
{
  /* promote to integer */
  dcArgInt(in_self, (DCint) s );
}


static void dc_callvm_argLong_ppc32(DCCallVM* in_self, DClong l)
{
  /* promote to integer */
  dcArgInt(in_self, (DCint) l );
}

static void dc_callvm_argPointer_ppc32(DCCallVM* in_self, DCpointer p)
{
  /* promote to integer */
  dcArgInt(in_self, *(DCint*) &p );
}


void dc_callvm_call_ppc32_darwin(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;
  dcCall_ppc32_darwin( 
    target, 
    &self->mRegData, 
    DC_MAX(dcVecSize(&self->mVecHead), 8*4),
    dcVecData(&self->mVecHead)
  );
}

void dc_callvm_call_ppc32_sysv(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*) in_self;
  dcCall_ppc32_sysv( target, &self->mRegData, dcVecSize(&self->mVecHead) , dcVecData(&self->mVecHead));
}

void dc_callvm_call_ppc32_syscall(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*) in_self;
  dcCall_ppc32_syscall( target, &self->mRegData, dcVecSize(&self->mVecHead) , dcVecData(&self->mVecHead));
}

void dc_callvm_mode_ppc32(DCCallVM* in_self, DCint mode);

DCCallVM_vt gVT_ppc32_darwin =
{
  &dc_callvm_free_ppc32
, &dc_callvm_reset_ppc32
, &dc_callvm_mode_ppc32
, &dc_callvm_argBool_ppc32
, &dc_callvm_argChar_ppc32
, &dc_callvm_argShort_ppc32 
, &dc_callvm_argInt_ppc32_darwin
, &dc_callvm_argLong_ppc32
, &dc_callvm_argLongLong_ppc32_darwin
, &dc_callvm_argFloat_ppc32_darwin
, &dc_callvm_argDouble_ppc32_darwin
, &dc_callvm_argPointer_ppc32
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_ppc32_darwin
, (DCboolvmfunc*)       &dc_callvm_call_ppc32_darwin
, (DCcharvmfunc*)       &dc_callvm_call_ppc32_darwin
, (DCshortvmfunc*)      &dc_callvm_call_ppc32_darwin
, (DCintvmfunc*)        &dc_callvm_call_ppc32_darwin
, (DClongvmfunc*)       &dc_callvm_call_ppc32_darwin
, (DClonglongvmfunc*)   &dc_callvm_call_ppc32_darwin
, (DCfloatvmfunc*)      &dc_callvm_call_ppc32_darwin
, (DCdoublevmfunc*)     &dc_callvm_call_ppc32_darwin
, (DCpointervmfunc*)    &dc_callvm_call_ppc32_darwin
, NULL /* callStruct */
};

DCCallVM_vt gVT_ppc32_sysv =
{
  &dc_callvm_free_ppc32
, &dc_callvm_reset_ppc32
, &dc_callvm_mode_ppc32
, &dc_callvm_argBool_ppc32
, &dc_callvm_argChar_ppc32
, &dc_callvm_argShort_ppc32 
, &dc_callvm_argInt_ppc32_sysv
, &dc_callvm_argLong_ppc32
, &dc_callvm_argLongLong_ppc32_sysv
, &dc_callvm_argFloat_ppc32_sysv
, &dc_callvm_argDouble_ppc32_sysv
, &dc_callvm_argPointer_ppc32
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_ppc32_sysv
, (DCboolvmfunc*)       &dc_callvm_call_ppc32_sysv
, (DCcharvmfunc*)       &dc_callvm_call_ppc32_sysv
, (DCshortvmfunc*)      &dc_callvm_call_ppc32_sysv
, (DCintvmfunc*)        &dc_callvm_call_ppc32_sysv
, (DClongvmfunc*)       &dc_callvm_call_ppc32_sysv
, (DClonglongvmfunc*)   &dc_callvm_call_ppc32_sysv
, (DCfloatvmfunc*)      &dc_callvm_call_ppc32_sysv
, (DCdoublevmfunc*)     &dc_callvm_call_ppc32_sysv
, (DCpointervmfunc*)    &dc_callvm_call_ppc32_sysv
, NULL /* callStruct */
};

DCCallVM_vt gVT_ppc32_syscall =
{
  &dc_callvm_free_ppc32
, &dc_callvm_reset_ppc32
, &dc_callvm_mode_ppc32
, &dc_callvm_argBool_ppc32
, &dc_callvm_argChar_ppc32
, &dc_callvm_argShort_ppc32 
, &dc_callvm_argInt_ppc32_sysv
, &dc_callvm_argLong_ppc32
, &dc_callvm_argLongLong_ppc32_sysv
, &dc_callvm_argFloat_ppc32_sysv
, &dc_callvm_argDouble_ppc32_sysv
, &dc_callvm_argPointer_ppc32
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_ppc32_syscall
, (DCboolvmfunc*)       &dc_callvm_call_ppc32_syscall
, (DCcharvmfunc*)       &dc_callvm_call_ppc32_syscall
, (DCshortvmfunc*)      &dc_callvm_call_ppc32_syscall
, (DCintvmfunc*)        &dc_callvm_call_ppc32_syscall
, (DClongvmfunc*)       &dc_callvm_call_ppc32_syscall
, (DClonglongvmfunc*)   &dc_callvm_call_ppc32_syscall
, (DCfloatvmfunc*)      &dc_callvm_call_ppc32_syscall
, (DCdoublevmfunc*)     &dc_callvm_call_ppc32_syscall
, (DCpointervmfunc*)    &dc_callvm_call_ppc32_syscall
, NULL /* callStruct */
};


void dc_callvm_mode_ppc32(DCCallVM* in_self, DCint mode)
{
  DCCallVM_ppc32* self = (DCCallVM_ppc32*)in_self;
  DCCallVM_vt* vt;

  switch(mode) {

#if defined(DC__ABI_Darwin)
    case DC_CALL_C_DEFAULT:
    case DC_CALL_C_ELLIPSIS:
    case DC_CALL_C_ELLIPSIS_VARARGS:
#endif
    case DC_CALL_C_PPC32_OSX:  
      vt = &gVT_ppc32_darwin; 
      break;

#if defined(DC__ABI_SysV)
    case DC_CALL_C_DEFAULT:
    case DC_CALL_C_ELLIPSIS:
    case DC_CALL_C_ELLIPSIS_VARARGS:
#endif
    case DC_CALL_C_PPC32_SYSV: 
      vt = &gVT_ppc32_sysv;
      break;

    case DC_CALL_SYS_DEFAULT:
    case DC_CALL_SYS_PPC32:
      vt = &gVT_ppc32_syscall;
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
  DCCallVM_ppc32* p = (DCCallVM_ppc32*)dcAllocMem(sizeof(DCCallVM_ppc32)+size);

  dc_callvm_mode_ppc32((DCCallVM*)p, DC_CALL_C_DEFAULT);

  dcVecInit(&p->mVecHead, size);
  dc_callvm_reset_ppc32((DCCallVM*)p);

  return (DCCallVM*)p;
}

