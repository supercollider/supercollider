/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_callvm_x64.h
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

  dyncall callvm for x64 architecture

  SUPPORTED CALLING CONVENTIONS
  MS Windows x64 calling convention, AMD64 SystemV ABI 

  REVISION
  2007/12/11 initial

*/


#ifndef DYNCALL_CALLVM_X64_H
#define DYNCALL_CALLVM_X64_H

#include "dyncall_macros.h"
#include "dyncall_call_x64.h"
#include "dyncall_callvm.h"
#include "dyncall_vector.h"


#if defined(DC_WINDOWS)

typedef long long int64;	/* llp64 */

#define numIntRegs   4
#define numFloatRegs 4
#define DCRegCount_x64 DCRegCount_x64_u
#define DCRegData_x64  DCRegData_x64_u

#elif defined(DC_UNIX)

typedef long int64;		/* lp64 */

#define numIntRegs   6
#define numFloatRegs 8
#define DCRegCount_x64 DCRegCount_x64_s
#define DCRegData_x64  DCRegData_x64_s

#else

#error Unsupported OS.

#endif

typedef union
{
  int i;
  int f;
} DCRegCount_x64_u;

typedef struct
{
  int i;
  int f;
} DCRegCount_x64_s;

typedef union
{
  int64  i[numIntRegs  ];
  double f[numFloatRegs];
} DCRegData_x64_u;

typedef struct
{
  int64  i[numIntRegs  ];
  double f[numFloatRegs];
} DCRegData_x64_s;


typedef struct
{
  DCCallVM       mInterface;  /* This CallVM interface.                                  */
  DCpointer      mpCallFunc;  /* Function to call.                                       */
  DCRegCount_x64 mRegCount;   /* Number of int/sse registers used for parameter passing. */
  DCRegData_x64  mRegData;    /* Parameters to be passed via registers.                  */
  DCVecHead      mVecHead;    /* Parameters to be pushed onto stack.                     */
} DCCallVM_x64;

#endif /* DYNCALL_CALLVM_X64_H */

