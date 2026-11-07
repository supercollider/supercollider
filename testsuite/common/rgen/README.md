RGen reference output data
==========================

This project lives outside the main source tree and must be built separately.

It contains a custom target `generate_test_data` that (re)generates the RGen reference
output for a particular SC version.

Generally, this should only be necessary in two cases:
1. you modify `../rgen_test.cpp` or `rgen_generate.cpp`.
2. you make a breaking change to one of the RGen methods and need to create new reference data.

In either case, you have to build the `generate_test_data` target with the respective
SC versions to (re(generate the `rgen_output_<version>.hpp` headers.

### Build instructions:

    cd <sc_source_dir>/testsuite/common/rgen
    cmake -S . -B build -DSC_PATH=<path>
    cmake --build build --target generate_test_data

`SC_PATH` must point to the SC source code that the test data should be generated from.
By default it is set to our parent repository (../../..).

If you need to generate test data for the current branch, just leave `SC_PATH` unset.

If you need to generate test data for *previous* SC versions, you can do the following:
    # create a new git worktree and checkout the given version tag
    git worktree add <dir> <version_tag>
    # point SC_PATH to the worktree directory
    cmake -S . -B build -DSC_PATH=<dir>
    # build
    cmake --build build --target generate_test_data

To build test data for another version, just checkout out the version tag in the worktree
and then build this project again.
