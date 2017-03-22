# How to run these tests

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

# How to make your own test files

1. Set the return value of `performingValidation` to `false` in all the test
classes for which you want new validation files.
2. Run the test suite (or specific tests) for which you want new validation
files.
3. Rename the `actual` directory as `expected`.
4. If you're developing, re-zip and commit!

# Known issues

* These tests take a very long time to run due to the large and currently
unavoidable volume of error messages printed by the parser. It will be
faster if you run via command-line or better (perhaps worse) yet by building 
a custom version that does not print error messages.

* The caret character `^` cannot be tested with the lexer under the current
state of sclang. This is because early termination through all levels of the
process whenever non-class-library code containing this character is executed.
There is a boolean value, `TestLexerBrutal.ignoringCaret`, which draws attention
to this issue, and may be experimentally set to false if this situation ever
changes.
