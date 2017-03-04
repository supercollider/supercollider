# How to use these tests

1. Unzip test_output.zip, and place its four folders in this directory.
2. Add all the files in the /lpc directory to your SuperCollider class path
(make sure you also have the UnitTest classes).
3. Run the code in test_script.scd
4. If there are any differences, they will be written to files in the
corresponding directories, with the name matching the test specification
plus a `_diff` suffix.

## Known issues

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
