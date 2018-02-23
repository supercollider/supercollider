/*

 Package: dyncall
 Library: test
 File: test/hacking-mips/calls.c
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

typedef int i;
typedef float f;
typedef double d;
void _();
void _iiiiiiiiii(i,i,i,i,i,i,i,i,i,i);
void _ffffffffff(f,f,f,f,f,f,f,f,f,f);
void _dddddddddd(d,d,d,d,d,d,d,d,d,d);
void _ifffffffff(i,f,f,f,f,f,f,f,f,f);
void _iddddddddd(i,d,d,d,d,d,d,d,d,d);
void _fdidfd(f,d,i,d,f,d);
void _ddidfd(d,d,i,d,f,d);
void _ididfd(i,d,i,d,f,d);
int    ai[] = { 0   , 1   , 2   , 3   , 4   , 5   , 6   , 7   , 8   , 9    };
float  af[] = { 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f }; 
double ad[] = { 0.0 , 1.0 , 2.0 , 3.0 , 4.0 , 5.0 , 6.0 , 7.0 , 8.0 , 9.0  };

void call_()       { _(); }
void call_iiiiiiiiii() { _iiiiiiiiii(ai[0],ai[1],ai[2],ai[3],ai[4],ai[5],ai[6],ai[7],ai[8],ai[9]); }
void call_ffffffffff() { _ffffffffff(af[0],af[1],af[2],af[3],af[4],af[5],af[6],af[7],af[8],af[9]); }
void call_dddddddddd() { _dddddddddd(ad[0],ad[1],ad[2],ad[3],ad[4],ad[5],ad[6],ad[7],ad[8],ad[9]); }
void call_ifffffffff() { _ifffffffff(ai[0],af[1],af[2],af[3],af[4],af[5],af[6],af[7],af[8],af[9]); } 
void call_iddddddddd() { _iddddddddd(ai[0],ad[1],ad[2],ad[3],ad[4],ad[5],ad[6],ad[7],ad[8],ad[9]); }

void call_fififififi() { _fififififi(af[0],ai[1],af[2],ai[3],af[4],ai[5],af[6],ai[7],af[8],ai[9]); }

void call_fdidfd() { _fdidfd(1.0f,2.0, 3   ,4.0, 5.0f,6.0 ); }
void call_ddidfd() { _ddidfd(1.0, 2.0, 3   ,4.0, 5.0f,6.0 ); }
void call_ididfd() { _ididfd(1  , 2.0, 3   ,4.0, 5.0f,6.0 ); }

