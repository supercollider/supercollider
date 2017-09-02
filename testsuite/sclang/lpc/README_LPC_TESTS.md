# sclang lexer, parser, and compiler (LPC) tests

Author: Brian Heim, 2017

## Purpose

This test suite provides regression tests and related utilities that will
allow developers to examine the behavior of the lexer/parser/compiler under
a wide and thorough set of conditions. They should be used anytime changes
to the language specification or compiler are being considered.

## Usage

### How to run these tests

1. Unzip test\_data.zip.
2. Add all the files in the /lpc directory to your SuperCollider class path
(make sure you also have the UnitTest classes).
3. Configure your class library so that only the core library, the UnitTest 
quark, and these test classes are compiled. This is very important: some 
non-core components such as AY.sc and the MathLib quark will affect parsing.
4. Run the code in test\_script.scd
5. If there are any differences, they will be written to files in the
corresponding directories, with the name matching the test specification
plus a `_diff` suffix.

### How to make your own test files

1. Set the return value of `performingValidation` to `false` in all the test
classes for which you want new validation files.
2. Run the test suite (or specific tests) for which you want new validation
files.
3. Rename the `actual` directory as `expected`.
4. If you're developing, re-zip and commit.

## Test format

Generally, testing proceeds by recording the results of lexing or compiling a
large number of test strings. Each string comprises a sequence of elements from
an alphabet.

Each individual test takes an alphabet `A` and string length `l`, along with an
optional prefix and suffix, and tests every possible string of length `l` that
can be made with `A`.

For instance, if `A` is all lowercase letters ([a-z]), `l` = 3, the prefix is
"hello_", and the suffix is "_world", then the tests would cover:

hello_aaa_world
hello_aab_world
hello_aac_world
...
hello_mqx_world
...
hello_zzy_world
hello_zzz_world

Generally, two methods are used:

1. Compile the string. If that succeeds, interpret it. Record the resulting
object using .asString, and if applicable, the resulting object's class.
2. Compile the string. If that succeeds, record its FunctionDef's bytecodes.

If any step fails, a unique ID indicating that either a compile-time or run-time
error occurred is recorded. In this way, we can compare the output of the
parser/lexer/compiler across versions of SC to see what input is legal, what is
illegal, and how certain expressions compile (or don't compile).

After running each test, sclang's output is checked against output from previous
output from the same test suite. Any differences between the expected and actual
results are noted in a diff file.

## Known issues

* These tests take a very long time to run due to the large and currently
unavoidable volume of error messages printed by the parser. It will be faster
if you run via command-line or better yet by building a custom version that does
not print error messages.

* The caret character `^` cannot be tested with the lexer under the current
state of sclang. This is because early termination through all levels of the
process whenever non-class-library code containing this character is executed.
There is a boolean value, `TestLexerBrutal.ignoringCaret`, which draws attention
to this issue, and may be experimentally set to false if this situation ever
changes.

* The interpreter has two modes: "command-line" and "class compilation". These
tests only cover the first of those. In other words, behavior for compiling
classes is not tested. This could be solved by giving direct access to the
interpreter in its class-compiling state, but much can be done without that.

## Developer notes

The files in this directory have the following purposes:

- `test_data.zip`: results of the test suite as run with SC 3.9-dev on macOS.
- `test_script.scd`: contains all the code needed to run the test suite.
- `LPCTestUtils.sc`: utilities for the test suite, including I/O. See this file
    first to get an idea of how the code works.
- `AbstractLPCBrutalTest.sc`: abstract superclass for test classes encapsulating
    notions of alphabets, prefixes, and suffixes
- `TestLexerBrutal.sc`: general tests for the lexer, focusing on large/full
    alphabets.
- `TestLexerTargetedBrutal.sc`: targeted tests for specific elements of the lexer:
    comments, floats, radices, flat/sharp notation, etc.
- `TestParserBrutal.sc`: general tests for the parser, focusing on sequences of
    keywords and functional elements.
- `TestCompilerBrutal.sc`: general tests for the compiler, focusing on bytecode
    generation.
- `TestLPCTestUtils.sc`: test suite for `LPCTestUtils.sc`; only for dev use.
