#!/bin/bash

# Runs the test suite in testsuite/classlibrary
# Tests in testsuite/classlibrary/server are ignored,
# as are tests for the UnitTesting Quark itself.

if $SCLANG "$TRAVIS_BUILD_DIR/.travis/configureTestEnvironment.scd"
then
	exit `$SCLANG $TRAVIS_BUILD_DIR/.travis/runAllUnitTests.scd`
else
	echo "Error while configuring test environment."
	exit 1
fi

