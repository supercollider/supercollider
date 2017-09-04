#!/bin/bash

# Runs the test suite in testsuite/classlibrary
# Tests in testsuite/classlibrary/server are ignored,
# as are tests for the UnitTesting Quark itself.

echo "Attempting to configure test environment."
if "$SCLANG" -i none "$TRAVIS_BUILD_DIR/.travis/configureTestEnvironment.scd"
then
	echo "Configuration complete. Attempting to run unit tests now."
	SCLANG_CONF="$TRAVIS_BUILD_DIR/.travis/sclang_conf.yaml"
	echo "sclang configuration: "
	cat "$SCLANG_CONF"
	"$SCLANG" -i none -l "$SCLANG_CONF" "$TRAVIS_BUILD_DIR/.travis/runAllUnitTests.scd"
	exit $?
else
	echo "Error while configuring test environment."
	exit 1
fi

