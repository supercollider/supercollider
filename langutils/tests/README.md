# Tests

## Error Regression

Check two things: where anything has changed after altering the compiler, and does a few basic asserts.
Every new feature must have a regression test.

Because of the vast number of these needs, AI has written most of these... that being said, always check to see if there are any edges cases it has missed.


### To run

```
ctest -R -j $N langutils.regression_test
```
Where `$N` is the number of threads you want to run on.


These first build sc_inspect, which can take a moment, after this the tests can be run in parallel.

If the output of a test has changed and an error is produced, you need to regenerate the expected output.

Run the `test.py` 
```
test.py --override
```
