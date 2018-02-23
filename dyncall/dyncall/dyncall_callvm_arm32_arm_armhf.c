/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_arm32_arm_armhf.c
 Description: ARM 'armhf' ABI implementation
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


#include "dyncall_callvm_arm32_arm_armhf.h"
#include "dyncall_alloc.h"


static void deinit(DCCallVM* in_self)
{
  dcFreeMem(in_self);
}


static void reset(DCCallVM* in_p)
{
  DCCallVM_arm32_armhf* p = (DCCallVM_arm32_armhf*)in_p;
  p->i = 0;
  p->s = 0;
  p->d = 0;
  dcVecResize(&p->mVecHead, 16);
}

static void a_int(DCCallVM* in_self, DCint x)
{
  DCCallVM_arm32_armhf* p = (DCCallVM_arm32_armhf*)in_self;
  if (p->i < 16) {
    * (DCint*) dcVecAt(&p->mVecHead, p->i) = x;
    p->i += 4;
  } else {
    dcVecAppend(&p->mVecHead, &x, sizeof(DCint));
  }
}

static void a_bool    (DCCallVM* in_self, DCbool  x) { a_int(in_self, (DCint)x); }
static void a_char    (DCCallVM* in_self, DCchar  x) { a_int(in_self, x); }
static void a_short   (DCCallVM* in_self, DCshort x) { a_int(in_self, x); }
static void a_long    (DCCallVM* in_self, DClong  x) { a_int(in_self, x); }

static void a_longlong(DCCallVM* in_self, DClonglong x)
{
  DCCallVM_arm32_armhf* p = (DCCallVM_arm32_armhf*)in_self;

  p->i = (p->i+4) & -8;
  if (p->i < 16) {
    * (DClonglong*) dcVecAt(&p->mVecHead, p->i) = x;
    p->i += 8;
  } else {
    /* 64 bit values need to be aligned on 8 byte boundaries */
    dcVecSkip(&p->mVecHead, dcVecSize(&p->mVecHead) & 4);
    dcVecAppend(&p->mVecHead, &x, sizeof(DClonglong));
  }
}

static void a_pointer(DCCallVM* in_p, DCpointer x) { a_int(in_p, (DCint) x ); }

static void a_float(DCCallVM* in_p, DCfloat x)
{
  DCCallVM_arm32_armhf* p = (DCCallVM_arm32_armhf*)in_p;
  if (p->s < 16) {
    p->S[p->s++] = x;
    if (p->d < p->s) {
      p->d = (p->s+1) & ~(1U);
    } else {
      p->s = p->d;
    }
  } else {
    dcVecAppend(&p->mVecHead, &x, sizeof(DCfloat));
  }
}

static void a_double(DCCallVM* in_p, DCdouble x)
{
  union {
    DCdouble d;
    DCchar   b[8];
  } v;

  DCCallVM_arm32_armhf* p = (DCCallVM_arm32_armhf*)in_p;
  if (p->d < 16) {
    * (DCdouble*) &p->S[p->d] = x;
    p->d += 2;
    if (!(p->s & 1)) {
      /* if s is even it always equals d. otherwise, s points to an odd float register. */
      p->s = p->d;
    }
  } else {
    p->s = 16; /* fp registers all full - need to use stack now: stop filling gaps for single precision, also */
    v.d = x;
    /* 64 bit values need to be aligned on 8 byte boundaries */
    dcVecSkip(&p->mVecHead, dcVecSize(&p->mVecHead) & 4);
    dcVecAppend(&p->mVecHead, &v.b[0], sizeof(DCdouble));
  }
}

static void a_float_ellipsis(DCCallVM* in_p, DCfloat x)
{
  a_int(in_p, *(DCint*)&x);
}

static void a_double_ellipsis(DCCallVM* in_p, DCdouble x)
{
  a_longlong(in_p, *(DClonglong*)&x);
}

void call(DCCallVM* in_p, DCpointer target)
{
  DCCallVM_arm32_armhf* p = (DCCallVM_arm32_armhf*)in_p;
  dcCall_arm32_armhf(target, dcVecData(&p->mVecHead), dcVecSize(&p->mVecHead), &p->S[0]);
}

static void mode(DCCallVM* in_self,DCint mode);

DCCallVM_vt vt_armhf =
{
  &deinit
, &reset
, &mode
, &a_bool
, &a_char
, &a_short
, &a_int
, &a_long
, &a_longlong
, &a_float
, &a_double
, &a_pointer
, NULL /* argStruct */
, (DCvoidvmfunc*)       &call
, (DCboolvmfunc*)       &call
, (DCcharvmfunc*)       &call
, (DCshortvmfunc*)      &call
, (DCintvmfunc*)        &call
, (DClongvmfunc*)       &call
, (DClonglongvmfunc*)   &call
, (DCfloatvmfunc*)      &call
, (DCdoublevmfunc*)     &call
, (DCpointervmfunc*)    &call
, NULL /* callStruct */
};

DCCallVM_vt vt_armhf_ellipsis =
{
  &deinit
, &reset
, &mode
, &a_bool
, &a_char
, &a_short
, &a_int
, &a_long
, &a_longlong
, &a_float_ellipsis
, &a_double_ellipsis
, &a_pointer
, NULL /* argStruct */
, (DCvoidvmfunc*)       &call
, (DCboolvmfunc*)       &call
, (DCcharvmfunc*)       &call
, (DCshortvmfunc*)      &call
, (DCintvmfunc*)        &call
, (DClongvmfunc*)       &call
, (DClonglongvmfunc*)   &call
, (DCfloatvmfunc*)      &call
, (DCdoublevmfunc*)     &call
, (DCpointervmfunc*)    &call
, NULL /* callStruct */
};

static void mode(DCCallVM* in_self, DCint mode)
{
  DCCallVM_arm32_armhf* self = (DCCallVM_arm32_armhf*)in_self;
  DCCallVM_vt* vt;

  switch(mode) {
    case DC_CALL_C_DEFAULT:
    case DC_CALL_C_ARM_ARMHF:
      vt = &vt_armhf;
      break;
    case DC_CALL_C_ELLIPSIS:
    case DC_CALL_C_ELLIPSIS_VARARGS:
      vt = &vt_armhf_ellipsis;
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
  /* Store at least 16 bytes (4 words for first 4 int args) for internal spill area. Assembly code depends on it. */
  DCCallVM_arm32_armhf* p = (DCCallVM_arm32_armhf*)dcAllocMem(sizeof(DCCallVM_arm32_armhf)+size+16);

  mode((DCCallVM*)p, DC_CALL_C_DEFAULT);

  dcVecInit(&p->mVecHead, size);
  reset((DCCallVM*)p);

  return (DCCallVM*)p;
}

