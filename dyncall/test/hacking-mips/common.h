/*

 Package: dyncall
 Library: test
 File: test/hacking-mips/common.h
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

typedef int (*fp)(int,int,int,int,int,int);

extern int f(int,int,int,int,int,int);
extern int dispatch(fp,int,int,int,int,int,int);
extern void ext();
extern void n0();
extern void n1(int a);
extern void n2(int a, int b);
extern void n4(int a, int b, int c, int d);
extern void n8(int a, int b, int c, int d, int e, int f, int g, int h);
extern void call_f0();
extern void call_f4();
extern void call_f8();

