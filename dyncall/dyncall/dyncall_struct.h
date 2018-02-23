/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_struct.h
 Description: C interface to compute struct size
 License:

   Copyright (c) 2010-2015 Olivier Chafik <olivier.chafik@centraliens.net>

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

  dyncall struct metadata structures

  REVISION
  2007/12/11 initial
  
*/

#ifndef DYNCALL_STRUCT_H
#define DYNCALL_STRUCT_H

#include "dyncall.h"

#ifdef __cplusplus
extern "C" {
#endif 

typedef struct DCfield_ {
	DCsize offset, size, alignment, arrayLength;
	DCint type;
	DCstruct* pSubStruct;
} DCfield;

struct DCstruct_ {
	DCfield *pFields;
	DCsize size, alignment, fieldCount;
	
	/* struct building uses a state machine. */
	DCint nextField;       /* == -1 if struct is closed */
	DCstruct *pCurrentStruct, *pLastStruct; /* == this, unless we're in a sub struct */
};


/* Helper. */
void dcArgStructUnroll(DCCallVM* vm, DCstruct* s, DCpointer  value);


#ifdef __cplusplus
}
#endif

#endif /* DYNCALL_H */

