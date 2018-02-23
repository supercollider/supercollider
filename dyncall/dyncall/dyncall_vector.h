/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_vector.h
 Description: Simple dynamic vector container type header
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



#ifndef DC_VECTOR_H
#define DC_VECTOR_H

#include "dyncall_types.h"

typedef struct
{
  DCsize mTotal;
  DCsize mSize;
} DCVecHead;

#define DC_SIZEOF_DCVector(size) (sizeof(DCVecHead)+size)

#define dcVecInit(p,size)   (p)->mTotal=size;(p)->mSize=0
#define dcVecReset(p)       (p)->mSize=0
#define dcVecResize(p,size) (p)->mSize=(size)
#define dcVecSkip(p,size)   (p)->mSize+=(size)
#define dcVecData(p)        ( (unsigned char*) (((DCVecHead*)(p))+1) )
#define dcVecAt(p,index)    ( dcVecData(p)+index )
#define dcVecSize(p)        ( (p)->mSize )
#define dcVecAlign(p,align) (p)->mSize=( (p)->mSize + align-1 ) & -align

/*
#include <string.h>
 #define dcVecAppend(p,s,n) memcpy( dcVecData(p)+p->mSize, s, n );p->mSize+=n
*/

void dcVecAppend(DCVecHead* pHead, const void* source, size_t length);

#endif /* DC_VECTOR_H */

