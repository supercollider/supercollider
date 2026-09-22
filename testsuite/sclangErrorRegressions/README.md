To regenerate the regression.txt you need to navigate to the build directory and run
```
sclang -a --include-path SCClassLibrary --include-path testsuite/sclangErrorRegressions testsuite/sclangErrorRegressions/generateError.scd OUTPUT_FILE.txt
```

This is required or else the paths will not match.
