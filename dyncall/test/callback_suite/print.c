/*

 Package: dyncall
 Library: test
 File: test/callback_suite/print.c
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

#include "../common/platformInit.h"

void PrintUsage(const char* appName)
{
  fprintf(stdout, "usage:\n\
%s [ -v ] [ from [to] ]\n\
where\n\
  from, to: test range\n\
options\n\
  -v        verbose reports\n\
  -h        help on usage\n\
\n\
", appName);
  fflush(stdout);
}

void PrintHeader()
{
  fprintf(stdout, "case\tsignat.\tresult\n");
}

void PrintCaseInfo(int caseId, const char* signatureString)
{
  fprintf(stdout, "f%d(\t%s", caseId, signatureString);
  fflush(stdout);
}

void PrintCaseResult(int resultId)
{
  fprintf(stdout, " :%d\n", resultId);
  fflush(stdout);
}

void PrintTotalResult(int resultId)
{
  printf("result: callback_suite: %d\n", resultId);
  fflush(stdout);
}

