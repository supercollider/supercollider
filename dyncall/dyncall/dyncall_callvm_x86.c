/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_x86.c
 Description: Call VM for x86 architecture implementation
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



#include "dyncall_callvm_x86.h"
#include "dyncall_alloc.h"


void dc_callvm_mode_x86(DCCallVM* in_self, DCint mode);

/* call vm destructor */

static void dc_callvm_free_x86(DCCallVM* in_self)
{
  dcFreeMem(in_self);
}

/* reset */

static void dc_callvm_reset_x86(DCCallVM* in_self)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcVecReset(&self->mVecHead);
  self->mIntRegs = 0;
}

/* arg (bool,char,short,long auto-promoted) to int */

static void dc_callvm_argInt_x86(DCCallVM* in_self, DCint x)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DCint) );
}

/* arg bool - promoted to int */

static void dc_callvm_argBool_x86(DCCallVM* in_self, DCbool x)
{

  DCint v = (DCint) x;
  dc_callvm_argInt_x86(in_self, v);
}

/* arg char - promoted to int */

static void dc_callvm_argChar_x86(DCCallVM* in_self, DCchar x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86(in_self, v);
}

/* arg short - promoted to int */

static void dc_callvm_argShort_x86(DCCallVM* in_self, DCshort x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86(in_self, v);
}

/* arg long - promoted to int */

static void dc_callvm_argLong_x86(DCCallVM* in_self, DClong x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86(in_self, v);
}

/* arg long long */

static void dc_callvm_argLongLong_x86(DCCallVM* in_self, DClonglong x)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DClonglong) );
}

/* arg float */

static void dc_callvm_argFloat_x86(DCCallVM* in_self, DCfloat x)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DCfloat) );
}

/* arg double */

static void dc_callvm_argDouble_x86(DCCallVM* in_self, DCdouble x)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DCdouble) );
}

/* arg pointer */

static void dc_callvm_argPointer_x86(DCCallVM* in_self, DCpointer x)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcVecAppend(&self->mVecHead, &x, sizeof(DCpointer) );
}


/* Plan9 specific calling convention. */
#if defined(DC__OS_Plan9)

/* call 'plan9' */

void dc_callvm_call_x86_plan9(DCCallVM* in_self, DCpointer target)
{
  /* Calls with 32-bit return values have it returned via EAX, so we don't */
  /* need to do anything special here.                                     */
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcCall_x86_plan9(target, dcVecData(&self->mVecHead), dcVecSize(&self->mVecHead));
}

DClonglong dc_callvm_call_x86_plan9_ll(DCCallVM* in_self, DCpointer target)
{
  /* Call for 64 bit integer return values is a bit different, call a    */
  /* different assembler stub that stores the return value in a variable */
  /* for us, and return the latter.                                      */
  DClonglong ret;
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcCall_x86_plan9_ll(target, dcVecData(&self->mVecHead), dcVecSize(&self->mVecHead), &ret );
  return ret;
}

DCCallVM_vt gVT_x86_plan9 =
{
  &dc_callvm_free_x86
, &dc_callvm_reset_x86
, &dc_callvm_mode_x86
, &dc_callvm_argBool_x86
, &dc_callvm_argChar_x86
, &dc_callvm_argShort_x86 
, &dc_callvm_argInt_x86
, &dc_callvm_argLong_x86
, &dc_callvm_argLongLong_x86
, &dc_callvm_argFloat_x86
, &dc_callvm_argDouble_x86
, &dc_callvm_argPointer_x86
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_x86_plan9
, (DCboolvmfunc*)       &dc_callvm_call_x86_plan9
, (DCcharvmfunc*)       &dc_callvm_call_x86_plan9
, (DCshortvmfunc*)      &dc_callvm_call_x86_plan9
, (DCintvmfunc*)        &dc_callvm_call_x86_plan9
, (DClongvmfunc*)       &dc_callvm_call_x86_plan9
, (DClonglongvmfunc*)   &dc_callvm_call_x86_plan9_ll
, (DCfloatvmfunc*)      &dc_callvm_call_x86_plan9
, (DCdoublevmfunc*)     &dc_callvm_call_x86_plan9
, (DCpointervmfunc*)    &dc_callvm_call_x86_plan9
, NULL /* callStruct */
};


#else


/* call 'cdecl' */

void dc_callvm_call_x86_cdecl(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcCall_x86_cdecl( target, dcVecData(&self->mVecHead), dcVecSize(&self->mVecHead) );
}

DCCallVM_vt gVT_x86_cdecl =
{
  &dc_callvm_free_x86
, &dc_callvm_reset_x86
, &dc_callvm_mode_x86
, &dc_callvm_argBool_x86
, &dc_callvm_argChar_x86
, &dc_callvm_argShort_x86 
, &dc_callvm_argInt_x86
, &dc_callvm_argLong_x86
, &dc_callvm_argLongLong_x86
, &dc_callvm_argFloat_x86
, &dc_callvm_argDouble_x86
, &dc_callvm_argPointer_x86
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_x86_cdecl
, (DCboolvmfunc*)       &dc_callvm_call_x86_cdecl
, (DCcharvmfunc*)       &dc_callvm_call_x86_cdecl
, (DCshortvmfunc*)      &dc_callvm_call_x86_cdecl
, (DCintvmfunc*)        &dc_callvm_call_x86_cdecl
, (DClongvmfunc*)       &dc_callvm_call_x86_cdecl
, (DClonglongvmfunc*)   &dc_callvm_call_x86_cdecl
, (DCfloatvmfunc*)      &dc_callvm_call_x86_cdecl
, (DCdoublevmfunc*)     &dc_callvm_call_x86_cdecl
, (DCpointervmfunc*)    &dc_callvm_call_x86_cdecl
, NULL /* callStruct */
};



/* --- stdcall -------------------------------------------------------------- */

/* call win32/std */

void dc_callvm_call_x86_win32_std(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcCall_x86_win32_std( target, dcVecData(&self->mVecHead), dcVecSize(&self->mVecHead) );
}

/* win32/std vtable */

DCCallVM_vt gVT_x86_win32_std =
{
  &dc_callvm_free_x86
, &dc_callvm_reset_x86
, &dc_callvm_mode_x86
, &dc_callvm_argBool_x86
, &dc_callvm_argChar_x86
, &dc_callvm_argShort_x86
, &dc_callvm_argInt_x86
, &dc_callvm_argLong_x86
, &dc_callvm_argLongLong_x86
, &dc_callvm_argFloat_x86
, &dc_callvm_argDouble_x86
, &dc_callvm_argPointer_x86
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_x86_win32_std
, (DCboolvmfunc*)       &dc_callvm_call_x86_win32_std
, (DCcharvmfunc*)       &dc_callvm_call_x86_win32_std
, (DCshortvmfunc*)      &dc_callvm_call_x86_win32_std
, (DCintvmfunc*)        &dc_callvm_call_x86_win32_std
, (DClongvmfunc*)       &dc_callvm_call_x86_win32_std
, (DClonglongvmfunc*)   &dc_callvm_call_x86_win32_std
, (DCfloatvmfunc*)      &dc_callvm_call_x86_win32_std
, (DCdoublevmfunc*)     &dc_callvm_call_x86_win32_std
, (DCpointervmfunc*)    &dc_callvm_call_x86_win32_std
, NULL /* callStruct */
};


/* --- fastcall common (ms/gnu) -------------------------------------------- */

/* call win32 ms fast */

static void dc_callvm_call_x86_win32_fast(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcCall_x86_win32_fast( target, dcVecData(&self->mVecHead), dcVecSize(&self->mVecHead) );
}

/* reset - always resize to 8 bytes (stores ECX and EDX) */

static void dc_callvm_reset_x86_win32_fast(DCCallVM* in_self)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcVecResize(&self->mVecHead, sizeof(DCint) * 2 );
  self->mIntRegs = 0;
}


/* --- fastcall ms --------------------------------------------------------- */

/* arg int - probably hold in ECX and EDX */

static void dc_callvm_argInt_x86_win32_fast_ms(DCCallVM* in_self, DCint x)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  if (self->mIntRegs < 2) {
    *( (int*) dcVecAt(&self->mVecHead, sizeof(DCint) * self->mIntRegs ) ) = x;
    ++( self->mIntRegs );
  } else
    dcVecAppend(&self->mVecHead, &x, sizeof(DCint) );
}

/* arg bool - promote to int */

static void dc_callvm_argBool_x86_win32_fast_ms(DCCallVM* in_self, DCbool x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86_win32_fast_ms(in_self,v);
}

/* arg char - promote to int */

static void dc_callvm_argChar_x86_win32_fast_ms(DCCallVM* in_self, DCchar x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86_win32_fast_ms(in_self,v);
}

/* arg short - promote to int */

static void dc_callvm_argShort_x86_win32_fast_ms(DCCallVM* in_self, DCshort x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86_win32_fast_ms(in_self,v);
}

/* arg long - promote to int */

static void dc_callvm_argLong_x86_win32_fast_ms(DCCallVM* in_self, DClong x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86_win32_fast_ms(in_self,v);
}

/* arg pointer - promote to int */

static void dc_callvm_argPointer_x86_win32_fast_ms(DCCallVM* in_self, DCpointer x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86_win32_fast_ms(in_self,v);
}

/* win32/fast vt */

DCCallVM_vt gVT_x86_win32_fast_ms =
{
  &dc_callvm_free_x86
, &dc_callvm_reset_x86_win32_fast
, &dc_callvm_mode_x86
, &dc_callvm_argBool_x86_win32_fast_ms
, &dc_callvm_argChar_x86_win32_fast_ms
, &dc_callvm_argShort_x86_win32_fast_ms
, &dc_callvm_argInt_x86_win32_fast_ms
, &dc_callvm_argLong_x86_win32_fast_ms
, &dc_callvm_argLongLong_x86
, &dc_callvm_argFloat_x86
, &dc_callvm_argDouble_x86
, &dc_callvm_argPointer_x86_win32_fast_ms
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_x86_win32_fast
, (DCboolvmfunc*)       &dc_callvm_call_x86_win32_fast
, (DCcharvmfunc*)       &dc_callvm_call_x86_win32_fast
, (DCshortvmfunc*)      &dc_callvm_call_x86_win32_fast
, (DCintvmfunc*)        &dc_callvm_call_x86_win32_fast
, (DClongvmfunc*)       &dc_callvm_call_x86_win32_fast
, (DClonglongvmfunc*)   &dc_callvm_call_x86_win32_fast
, (DCfloatvmfunc*)      &dc_callvm_call_x86_win32_fast
, (DCdoublevmfunc*)     &dc_callvm_call_x86_win32_fast
, (DCpointervmfunc*)    &dc_callvm_call_x86_win32_fast
, NULL /* callStruct */
};


/* --- gnu fastcall -------------------------------------------------------- */

/* arg int - probably hold in ECX and EDX */

static void dc_callvm_argInt_x86_win32_fast_gnu(DCCallVM* in_self, DCint x)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  if (self->mIntRegs < 2) {
    *( (int*) dcVecAt(&self->mVecHead, sizeof(DCint) * self->mIntRegs ) ) = x;
    ++( self->mIntRegs );
  } else
    dcVecAppend(&self->mVecHead, &x, sizeof(DCint) );
}

/* arg bool - promote to int */

static void dc_callvm_argBool_x86_win32_fast_gnu(DCCallVM* in_self, DCbool x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86_win32_fast_gnu(in_self,v);
}

/* arg char - promote to int */

static void dc_callvm_argChar_x86_win32_fast_gnu(DCCallVM* in_self, DCchar x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86_win32_fast_gnu(in_self,v);
}

/* arg short - promote to int */

static void dc_callvm_argShort_x86_win32_fast_gnu(DCCallVM* in_self, DCshort x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86_win32_fast_gnu(in_self,v);
}

/* arg long - promote to int */

static void dc_callvm_argLong_x86_win32_fast_gnu(DCCallVM* in_self, DClong x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86_win32_fast_gnu(in_self,v);
}

/* arg pointer - promote to int */

static void dc_callvm_argPointer_x86_win32_fast_gnu(DCCallVM* in_self, DCpointer x)
{
  DCint v = (DCint) x;
  dc_callvm_argInt_x86_win32_fast_gnu(in_self,v);
}

/* arg long long - skip registers and push on stack */

static void dc_callvm_argLongLong_x86_win32_fast_gnu(DCCallVM* in_self, DClonglong x)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  self->mIntRegs = 2;
  dc_callvm_argLongLong_x86(in_self,x);
}

/* win32/fast/gnu vt */

DCCallVM_vt gVT_x86_win32_fast_gnu =
{
  &dc_callvm_free_x86
, &dc_callvm_reset_x86_win32_fast
, &dc_callvm_mode_x86
, &dc_callvm_argBool_x86_win32_fast_gnu
, &dc_callvm_argChar_x86_win32_fast_gnu
, &dc_callvm_argShort_x86_win32_fast_gnu
, &dc_callvm_argInt_x86_win32_fast_gnu
, &dc_callvm_argLong_x86_win32_fast_gnu
, &dc_callvm_argLongLong_x86_win32_fast_gnu
, &dc_callvm_argFloat_x86
, &dc_callvm_argDouble_x86
, &dc_callvm_argPointer_x86_win32_fast_gnu
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_x86_win32_fast
, (DCboolvmfunc*)       &dc_callvm_call_x86_win32_fast
, (DCcharvmfunc*)       &dc_callvm_call_x86_win32_fast
, (DCshortvmfunc*)      &dc_callvm_call_x86_win32_fast
, (DCintvmfunc*)        &dc_callvm_call_x86_win32_fast
, (DClongvmfunc*)       &dc_callvm_call_x86_win32_fast
, (DClonglongvmfunc*)   &dc_callvm_call_x86_win32_fast
, (DCfloatvmfunc*)      &dc_callvm_call_x86_win32_fast
, (DCdoublevmfunc*)     &dc_callvm_call_x86_win32_fast
, (DCpointervmfunc*)    &dc_callvm_call_x86_win32_fast
, NULL /* callStruct */
};


/* --- this ms ------------------------------------------------------------- */

/* call win32/this/ms */

void dc_callvm_call_x86_win32_this_ms(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcCall_x86_win32_msthis( target, dcVecData(&self->mVecHead), dcVecSize(&self->mVecHead) );
}

/* win32/this/ms vt */

DCCallVM_vt gVT_x86_win32_this_ms =
{
  &dc_callvm_free_x86
, &dc_callvm_reset_x86
, &dc_callvm_mode_x86
, &dc_callvm_argBool_x86
, &dc_callvm_argChar_x86
, &dc_callvm_argShort_x86
, &dc_callvm_argInt_x86
, &dc_callvm_argLong_x86
, &dc_callvm_argLongLong_x86
, &dc_callvm_argFloat_x86
, &dc_callvm_argDouble_x86
, &dc_callvm_argPointer_x86
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_x86_win32_this_ms
, (DCboolvmfunc*)       &dc_callvm_call_x86_win32_this_ms
, (DCcharvmfunc*)       &dc_callvm_call_x86_win32_this_ms
, (DCshortvmfunc*)      &dc_callvm_call_x86_win32_this_ms
, (DCintvmfunc*)        &dc_callvm_call_x86_win32_this_ms
, (DClongvmfunc*)       &dc_callvm_call_x86_win32_this_ms
, (DClonglongvmfunc*)   &dc_callvm_call_x86_win32_this_ms
, (DCfloatvmfunc*)      &dc_callvm_call_x86_win32_this_ms
, (DCdoublevmfunc*)     &dc_callvm_call_x86_win32_this_ms
, (DCpointervmfunc*)    &dc_callvm_call_x86_win32_this_ms
, NULL /* callStruct */
};

/* --- syscall ------------------------------------------------------------- */

/* call syscall */

void dc_callvm_call_x86_sys_int80h_linux(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcCall_x86_sys_int80h_linux( target, dcVecData(&self->mVecHead), dcVecSize(&self->mVecHead) );
}

void dc_callvm_call_x86_sys_int80h_bsd(DCCallVM* in_self, DCpointer target)
{
  DCCallVM_x86* self = (DCCallVM_x86*) in_self;
  dcCall_x86_sys_int80h_bsd( target, dcVecData(&self->mVecHead), dcVecSize(&self->mVecHead) );
}

DCCallVM_vt gVT_x86_sys_int80h_linux =
{
  &dc_callvm_free_x86
, &dc_callvm_reset_x86
, &dc_callvm_mode_x86
, &dc_callvm_argBool_x86
, &dc_callvm_argChar_x86
, &dc_callvm_argShort_x86
, &dc_callvm_argInt_x86
, &dc_callvm_argLong_x86
, &dc_callvm_argLongLong_x86
, &dc_callvm_argFloat_x86
, &dc_callvm_argDouble_x86
, &dc_callvm_argPointer_x86
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_x86_sys_int80h_linux
, (DCboolvmfunc*)       &dc_callvm_call_x86_sys_int80h_linux
, (DCcharvmfunc*)       &dc_callvm_call_x86_sys_int80h_linux
, (DCshortvmfunc*)      &dc_callvm_call_x86_sys_int80h_linux
, (DCintvmfunc*)        &dc_callvm_call_x86_sys_int80h_linux
, (DClongvmfunc*)       &dc_callvm_call_x86_sys_int80h_linux
, (DClonglongvmfunc*)   &dc_callvm_call_x86_sys_int80h_linux
, (DCfloatvmfunc*)      &dc_callvm_call_x86_sys_int80h_linux
, (DCdoublevmfunc*)     &dc_callvm_call_x86_sys_int80h_linux
, (DCpointervmfunc*)    &dc_callvm_call_x86_sys_int80h_linux
, NULL /* callStruct */
};

DCCallVM_vt gVT_x86_sys_int80h_bsd =
{
  &dc_callvm_free_x86
, &dc_callvm_reset_x86
, &dc_callvm_mode_x86
, &dc_callvm_argBool_x86
, &dc_callvm_argChar_x86
, &dc_callvm_argShort_x86
, &dc_callvm_argInt_x86
, &dc_callvm_argLong_x86
, &dc_callvm_argLongLong_x86
, &dc_callvm_argFloat_x86
, &dc_callvm_argDouble_x86
, &dc_callvm_argPointer_x86
, NULL /* argStruct */
, (DCvoidvmfunc*)       &dc_callvm_call_x86_sys_int80h_bsd
, (DCboolvmfunc*)       &dc_callvm_call_x86_sys_int80h_bsd
, (DCcharvmfunc*)       &dc_callvm_call_x86_sys_int80h_bsd
, (DCshortvmfunc*)      &dc_callvm_call_x86_sys_int80h_bsd
, (DCintvmfunc*)        &dc_callvm_call_x86_sys_int80h_bsd
, (DClongvmfunc*)       &dc_callvm_call_x86_sys_int80h_bsd
, (DClonglongvmfunc*)   &dc_callvm_call_x86_sys_int80h_bsd
, (DCfloatvmfunc*)      &dc_callvm_call_x86_sys_int80h_bsd
, (DCdoublevmfunc*)     &dc_callvm_call_x86_sys_int80h_bsd
, (DCpointervmfunc*)    &dc_callvm_call_x86_sys_int80h_bsd
, NULL /* callStruct */
};


#endif


/* mode */

void dc_callvm_mode_x86(DCCallVM* in_self, DCint mode)
{
  DCCallVM_x86* self = (DCCallVM_x86*)in_self;
  DCCallVM_vt* vt;

  switch(mode) {
    case DC_CALL_C_DEFAULT:
    case DC_CALL_C_ELLIPSIS:
    case DC_CALL_C_ELLIPSIS_VARARGS:
/* Plan9 (and forks) have their own calling convention (and no support for foreign ones). */
#if defined(DC_PLAN9)
    case DC_CALL_C_X86_PLAN9:          vt = &gVT_x86_plan9;          break;
#else
    case DC_CALL_C_X86_CDECL:          vt = &gVT_x86_cdecl;          break;
    case DC_CALL_C_X86_WIN32_STD:      vt = &gVT_x86_win32_std;      break;
    case DC_CALL_C_X86_WIN32_FAST_MS:  vt = &gVT_x86_win32_fast_ms;  break;
    case DC_CALL_C_X86_WIN32_THIS_MS:  vt = &gVT_x86_win32_this_ms;  break;
    case DC_CALL_C_X86_WIN32_FAST_GNU: vt = &gVT_x86_win32_fast_gnu; break;
    case DC_CALL_C_X86_WIN32_THIS_GNU: vt = &gVT_x86_cdecl;          break;
    case DC_CALL_SYS_DEFAULT:         
# if defined DC_UNIX
#   if defined DC__OS_Linux
      vt = &gVT_x86_sys_int80h_linux; break;
#   else
      vt = &gVT_x86_sys_int80h_bsd;   break;
#   endif
# else
      self->mInterface.mError = DC_ERROR_UNSUPPORTED_MODE; return;
# endif
    case DC_CALL_SYS_X86_INT80H_LINUX: vt = &gVT_x86_sys_int80h_linux; break;
    case DC_CALL_SYS_X86_INT80H_BSD:   vt = &gVT_x86_sys_int80h_bsd;   break;
#endif
    default: 
      self->mInterface.mError = DC_ERROR_UNSUPPORTED_MODE;
      return;
  }
  dc_callvm_base_init(&self->mInterface, vt);
}

/* Public API. */
DCCallVM* dcNewCallVM(DCsize size)
{
  DCCallVM_x86* p = (DCCallVM_x86*)dcAllocMem(sizeof(DCCallVM_x86)+size);

  dc_callvm_mode_x86((DCCallVM*)p, DC_CALL_C_DEFAULT);

  dcVecInit(&p->mVecHead, size);
  dc_callvm_reset_x86((DCCallVM*)p);

  return (DCCallVM*)p;
}

