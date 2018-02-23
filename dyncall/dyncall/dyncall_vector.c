/*

 Package: dyncall
 Library: dyncall
 File: dyncall/dyncall_vector.c
 Description: Simple dynamic vector container type implementation
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



#include "dyncall_vector.h"
#include <string.h>


void dcVecAppend(DCVecHead* pHead, const void* pData, size_t size)
{
  size_t newSize = pHead->mSize + size;
  if(newSize <= pHead->mTotal) 
  {
  	void* dst = (DCchar*)dcVecData(pHead) + pHead->mSize;
  	switch (size) {
  	  case 1: *(DCchar    *)dst = *(const DCchar    *)pData; break;
  	  case 2: *(DCshort   *)dst = *(const DCshort   *)pData; break;
  	  case 4: *(DCint     *)dst = *(const DCint     *)pData; break;
  	  case 8: *(DCint     *)( ( (char*)dst )+4) = *(const DCint     *)( ( (char*)pData )+4); 
  	          *(DCint     *)dst = *(const DCint     *)pData; break;
 	  /* On sparc 32-bit, this one crashes if ptrs are not aligned.
          case 8: *(DClonglong*)dst = *(const DClonglong*)pData; break;
  	  */
          default: memcpy(dst, pData, size); /* for all the rest. */
  	}
    pHead->mSize = newSize;
  }
  /*else @@@ warning? error?*/
}

