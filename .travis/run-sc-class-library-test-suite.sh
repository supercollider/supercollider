#!/bin/bash

# Runs the test suite in testsuite/classlibrary
# Tests in testsuite/classlibrary/server are ignored,
# as are tests for the UnitTesting Quark itself.

echo "Attempting to configure test environment."
export SCLANG_CONF="$TRAVIS_BUILD_DIR/.travis/sclang_conf.yaml"
"$SCLANG" -i none "$TRAVIS_BUILD_DIR/.travis/configureTestEnvironment.scd"

echo "Testing for existence of $SCLANG_CONF"
if [[ -e "$SCLANG_CONF" ]]
then
	echo "Configuration complete. Attempting to run unit tests now."
	echo "sclang configuration: "
	cat "$SCLANG_CONF"
	echo ""
	"$SCLANG" -i none -l "$SCLANG_CONF" "$TRAVIS_BUILD_DIR/.travis/runAllUnitTests.scd"
	exit $?
else
	echo "ERROR: $SCLANG_CONF not found. Test environment configuration failed."
	exit 1
fi

