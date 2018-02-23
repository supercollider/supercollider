/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_arm64_apple.c
 Description: ARM 64-bit Apple ABI implementation
 License:

   Copyright (c) 2015 Daniel Adler <dadler@uni-goettingen.de>, 
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


#include "dyncall_callvm_arm64.h"
#include "dyncall_alloc.h"


static void reset(DCCallVM* in_p)
{
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  p->i = 0;
  p->f = 0;
  dcVecReset(&p->mVecHead);
}


static void deinit(DCCallVM* in_p)
{
  dcFreeMem(in_p);
}



static void a_i64(DCCallVM* in_p, DClonglong x)
{
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  if (p->i < 8) {
    p->I[p->i] = x;
    p->i++;
  } else {
    dcVecAlign(&p->mVecHead, sizeof(DClonglong));
    dcVecAppend(&p->mVecHead, &x, sizeof(DClonglong));
  }
}

static void var_i64     (DCCallVM* in_p, DClonglong  x) 
{ 
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  dcVecAlign(&p->mVecHead, sizeof(DClonglong));
  dcVecAppend(&p->mVecHead, &x, sizeof(DClonglong));
}
static void var_bool    (DCCallVM* in_p, DCbool  x) { var_i64( in_p, ((DClonglong) x) ); }
static void var_char    (DCCallVM* in_p, DCchar  x) { var_i64( in_p, ((DClonglong) x) ); }
static void var_short   (DCCallVM* in_p, DCshort x) { var_i64( in_p, ((DClonglong) x) ); }
static void var_int     (DCCallVM* in_p, DCint   x) { var_i64( in_p, ((DClonglong) x) ); }
static void var_long    (DCCallVM* in_p, DClong  x) { var_i64( in_p, ((DClonglong) x) ); }
static void var_double  (DCCallVM* in_p, DCdouble x) {
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  dcVecAlign(&p->mVecHead, sizeof(DCdouble));
  dcVecAppend(&p->mVecHead, &x, sizeof(DCdouble)); 
}   
static void var_float   (DCCallVM* in_p, DCfloat x) {   
  var_double( in_p, (DCdouble) x );
}
static void var_pointer (DCCallVM* in_p, DCpointer x) {
  var_i64(in_p, (DClonglong) x );
}

static void a_bool    (DCCallVM* in_p, DCbool  x)   
{ 
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  if (p->i < 8) {
    p->I[p->i] = (DClonglong) x;
    p->i++;
  } else {
    dcVecAlign(&p->mVecHead, sizeof(DCbool));
    dcVecAppend(&p->mVecHead, &x, sizeof(DCbool));
  }
}
static void a_char    (DCCallVM* in_p, DCchar  x)
{ 
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  if (p->i < 8) {
    p->I[p->i] = (DClonglong) x;
    p->i++;
  } else {
    dcVecAppend(&p->mVecHead, &x, sizeof(DCchar));
  }
}
static void a_short   (DCCallVM* in_p, DCshort x)   
{ 
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  if (p->i < 8) {
    p->I[p->i] = (DClonglong) x;
    p->i++;
  } else {
    dcVecAlign(&p->mVecHead, sizeof(DCshort));
    dcVecAppend(&p->mVecHead, &x, sizeof(DCshort));
  }
}
static void a_int     (DCCallVM* in_p, DCint x)     
{ 
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  if (p->i < 8) {
    p->I[p->i] = (DClonglong) x;
    p->i++;
  } else {
    dcVecAlign(&p->mVecHead, sizeof(DCint));
    dcVecAppend(&p->mVecHead, &x, sizeof(DCint));
  }
}
static void a_long    (DCCallVM* in_p, DClong  x)   
{ 
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  if (p->i < 8) {
    p->I[p->i] = (DClonglong) x;
    p->i++;
  } else {
    dcVecAlign(&p->mVecHead, sizeof(DClong));
    dcVecAppend(&p->mVecHead, &x, sizeof(DClong));
  }
}
static void a_pointer (DCCallVM* in_p, DCpointer x) { a_i64(in_p, (DClonglong) x ); }

static void a_float(DCCallVM* in_p, DCfloat x)
{
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;

  if (p->f < 8) {
    p->u.S[ p->f << 1 ] = x;
    p->f++;
  } else {
    dcVecAlign(&p->mVecHead, sizeof(DCfloat));
    dcVecAppend(&p->mVecHead, &x, sizeof(DCfloat));
  }
}

static void a_double(DCCallVM* in_p, DCdouble x)
{
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  if (p->f < 8) {
    p->u.D[ p->f ] = x;
    p->f++; 
  } else {
    dcVecAlign(&p->mVecHead, sizeof(DCdouble));
    dcVecAppend(&p->mVecHead, &x, sizeof(DCdouble));
  }
}

void call(DCCallVM* in_p, DCpointer target)
{
  DCCallVM_arm64* p = (DCCallVM_arm64*)in_p;
  
  /*
  ** copy 'size' argument is given in number of 16-byte 'pair' blocks.
  */
  
  dcCall_arm64(target, dcVecData(&p->mVecHead), ( dcVecSize(&p->mVecHead) + 15 ) & -16, &p->u.S[0]);
}

static void mode(DCCallVM* in_p,DCint mode);

DCCallVM_vt vt_arm64 =
{
  &deinit
, &reset
, &mode
, &a_bool
, &a_char
, &a_short 
, &a_int
, &a_long
, &a_i64
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

DCCallVM_vt vt_arm64_variadic =
{
  &deinit
, &reset
, &mode
, &var_bool
, &var_char
, &var_short 
, &var_int
, &var_long
, &var_i64
, &var_float
, &var_double
, &var_pointer
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
  DCCallVM_arm64* self = (DCCallVM_arm64*)in_self;
  DCCallVM_vt* vt;

  switch(mode) {
    case DC_CALL_C_DEFAULT:        
    case DC_CALL_C_ARM64:
    case DC_CALL_C_ELLIPSIS:
      vt = &vt_arm64;
      break;
    case DC_CALL_C_ELLIPSIS_VARARGS:
      vt = &vt_arm64_variadic;
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
  DCCallVM_arm64* p = (DCCallVM_arm64*)dcAllocMem(sizeof(DCCallVM_arm64)+size);

  mode((DCCallVM*)p, DC_CALL_C_DEFAULT);

  dcVecInit(&p->mVecHead, size);
  reset((DCCallVM*)p);

  return (DCCallVM*)p;
}

