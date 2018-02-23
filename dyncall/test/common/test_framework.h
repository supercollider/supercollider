/*

 Package: dyncall
 Library: test
 File: test/plain_c++/test_framework.h
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



// 2007-10-11

#ifndef TRUE
#define TRUE (1)
#endif

#ifndef FALSE
#define FALSE (0)
#endif

// Statement breaking into debugger (for various platforms). @@@add if defined, etc...
#ifdef _WIN32
# ifdef _MSC_VER
#  define DC_TEST_BREAK __debugbreak
# else
#  include <windows.h>
#  define DC_TEST_BREAK DebugBreak
# endif
#else
# define DC_TEST_BREAK
#endif

// Test state - evaluates to false inside a test function if a test fails.
#define DC_TEST_STATE				testFramework_state

// Test functions defined by DC_DEFINE_TEST_FUNC_BEGIN and DC_DEFINE_TEST_FUNC_END pairs do come with
// a default parameter used to pass a path to them specifying where to find external data needed to
// run the tests (if desired). NULL by default. It can be queried by using DC_TEST_PATH.
#define DC_TEST_PATH				testFramework_suitePath

// Test a single expression - the expression must evaluate to true in order to succeed.
// #define DC_TEST(exp)				{ TEST_STATE = TEST_STATE && (exp); if(!TEST_STATE) DC_TEST_BREAK(); }

#include <assert.h>

#define DC_TEST(exp)        assert(exp)
// { if (! (exp) ) DC_TEST_BREAK(); }

// Macros used to define a test functions.
#define DC_DEFINE_TEST_FUNC_BEGIN(name)	/*static */int name(/*const char* DC_TEST_PATH=NULL*/) { /*@@@ add logging*/ int TEST_STATE = TRUE;
#define DC_DEFINE_TEST_FUNC_END			/*@@@ add logging*/ return TEST_STATE; }
