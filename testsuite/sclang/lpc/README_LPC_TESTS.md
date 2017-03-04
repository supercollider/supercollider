# How to run these tests

1. Unzip test_output.zip, and place its four folders in this directory.
2. Add all the files in the /lpc directory to your SuperCollider class path
(make sure you also have the UnitTest classes).
3. Run the code in test_script.scd
4. If there are any differences, they will be written to files in the
corresponding directories, with the name matching the test specification
plus a `_diff` suffix.

# How to make your own test files

1. Set the return value of `makingValidationFiles` to `true` in all the test
classes for which you want new validation files.
2. Delete all the old `*_correct` files. Generation will fail if the test
suite attempts to overwrite an existing file.
3. Run the test suite (or specific tests) for which you want new validation
files.
4. If you're developing, re-zip and commit!

# Known issues

* These tests take a very long time to run due to the large and currently
unavoidable volume of error messages printed by the parser. It will be
faster if you run via command-line or better (perhaps worse) yet by building 
a custom version that does not print error messages.

* For unknown reasons, the interpreter will crash during some tests. This
does not seem to affect the output in any way, but is annyoing and seems to
mainly be caused by the high number of instructions being executed. The problem
may be circumvented by running the crashing test method individually, or in
extreme situations by forcing the test to run on only one alphabet at time by
modifying the definition of that class's `alphabetStringLengths` field found
in `initAlphabets`.

* The caret character `^` cannot be tested with the lexer under the current
state of sclang. This is because early termination through all levels of the
process whenever non-class-library code containing this character is executed.
There is a boolean value, `TestLexerBrutal.ignoringCaret`, which draws attention
to this issue, and may be experimentally set to false if this situation ever
changes.
