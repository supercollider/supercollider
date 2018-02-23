/*

 Package: dyncall
 Library: test
 File: test/samples/calls/many.c
 Description: 
 License:

   Copyright (c) 2011-2015 Daniel Adler <dadler@uni-goettingen.de>,
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

#define i int
#define f float
#define d double
#define l long long

void fi16(
  f,i,f,i,
  f,i,f,i,
  f,i,f,i,
  f,i,f,i
);


void i16(
  i,i,i,i,
  i,i,i,i,
  i,i,i,i,
  i,i,i,i
);

void f16(
  f,f,f,f,
  f,f,f,f,
  f,f,f,f,
  f,f,f,f
);
void d16(
  d,d,d,d,
  d,d,d,d,
  d,d,d,d,
  d,d,d,d
);
void l16(
  l,l,l,l,
  l,l,l,l,
  l,l,l,l,
  l,l,l,l
);
void t()
{
/*
	l16(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);	
	d16(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);	
	i16(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);	
	f16(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);	
*/
	di16(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);	
	fi16(0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);	
}
