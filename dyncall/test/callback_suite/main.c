/*

 Package: dyncall
 Library: test
 File: test/callback_suite/main.c
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

#include <assert.h>
#include <stdlib.h>
#include "_auto_config.h"
#include "env.h"
#include "print.h"
#include "../common/platformInit.h"
#include "../common/platformInit.c" /* Impl. for functions only used in this translation unit */


const char* appname = "unknown";

/* test one case, returns error code */
int DoTest(int id);

/* capture total results for failure (0) and success (1) */
int totalErrorCodes[2] = { 0, 0 };

void TestRange(int from, int to)
{
  int i;
  for(i = from ; i <= to ; ++i )
  {
    int status = DoTest(i);
    totalErrorCodes[status]++;
  }
}

void InitEnv();

void ExitWithUsage()
{
  PrintUsage(appname);
  exit(0);
}
        
#define Error(X, Y) fprintf(stderr, X, Y); ExitWithUsage()

int main(int argc, char* argv[] )
{
  int from = 1;
  int to = CONFIG_NSIGS;
  int ncases;

  int i;
  int pos;
  int number;
  int totalResult;

  dcTest_initPlatform();

  InitEnv();
  appname = argv[0];

  pos = 0;
  for(i = 1 ; i < argc ; ++i ) {

    if ( argv[i][0] == '-' ) {
      switch(argv[i][1]) {
        case 'v': OptionVerbose = 1; continue;
        case 'h': PrintUsage(appname); return 0;
        default: Error("invalid option: %s", argv[i]);
      }      
    }

    number = atoi(argv[i]);
    switch(pos) {
      case 0: to   = from = number; ++pos; break;
      case 1: to   = number; break;
      default: Error("too many arguments%s", "");
    }
  }

  assert(from > 0);
  assert(to   <= CONFIG_NSIGS);
  assert(from <= to);

  ncases = (to - from) + 1;

  PrintHeader();
  TestRange(from, to);
  totalResult = (totalErrorCodes[1] == ncases) ? 1 : 0; 
  PrintTotalResult(totalResult);

  dcTest_deInitPlatform();

  return 0;
}

