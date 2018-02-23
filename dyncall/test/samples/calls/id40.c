/*

 Package: dyncall
 Library: test
 File: test/samples/calls/id40.c
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

#define d double
#define i int
void id40(
  i,d,i,d,
  i,d,i,d,
  i,d,i,d,
  i,d,i,d,
  i,d,i,d,
  i,d,i,d,
  i,d,i,d,
  i,d,i,d,
  i,d,i,d,
  i,d,i,d
);
void t(int*x,double*y)
{
  id40(
    x[ 0],y[ 1],x[ 2],y[ 3],x[ 4],y[ 5],x[ 6],y[ 7],x[ 8],y[ 9],x[10],y[11],x[12],y[13],x[14],y[15],x[16],y[17],x[18],y[19],
    x[20],y[21],x[22],y[23],x[24],y[25],x[26],y[27],x[28],y[29],x[30],y[31],x[32],y[33],x[34],y[35],x[36],y[37],x[38],y[39]
  );	
}
